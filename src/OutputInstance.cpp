#include "OutputInstance.h"
#include "model/SlideGroup.h"
#include "MainWindow.h"
#include "AppSettings.h"
#include "MyGraphicsScene.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QVBoxLayout>

#include <QImageWriter>

#include "SlideGroupViewer.h"
#include "MyGraphicsScene.h"
#include "model/Slide.h"
#include "model/Output.h"
#include "model/AbstractItemFilter.h"
#include "JpegServer.h"
#include "OutputServer.h"

#include "itemlistfilters/SlideTextOnlyFilter.h"

// Size of the jpeg server image buffer before it starts
// deleting frames. 10 is just an arbitrary magic nbr.
#define MAX_IMGBUFFER_SIZE 10

bool OuputInstance_slide_num_compare(Slide *a, Slide *b)
{
	return (a && b) ? a->slideNumber() < b->slideNumber() : true;
}



OutputInstance::OutputInstance(Output *out, QWidget *parent)
	: QWidget(parent)
	, m_output(out)
	, m_viewer(0)
	, m_slideGroup(0)
	, m_slideNum(-1)
	, m_isFoldback(false)
	, m_jpegServer(0)
	, m_outputServer(0)
{
	out->setInstance(this);
	
	if(out->tags().toLower().indexOf("foldback") >= 0 ||
	   out->name().toLower().indexOf("foldback") >= 0)
		m_isFoldback = true;
	
	// MUST be created after main window, so allow segfault if no main window
	MainWindow *mw = MainWindow::mw();
		
	connect(mw, SIGNAL(appSettingsChanged()), this, SLOT(applyOutputSettings()));
	
	// even though we dont worry about AR ourself, we need to tell the networked client about AR changes
	//connect(mw, SIGNAL(aspectRatioChanged(double)), this, SLOT(aspectRatioChanged(double)));

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0,0,0,0);
	
	m_viewer = new SlideGroupViewer(this);
	m_viewer->setBackground(Qt::black);
	m_viewer->setCursor(Qt::BlankCursor);
	connect(m_viewer, SIGNAL(nextGroup()), this, SLOT(slotNextGroup()));
	
	layout->addWidget(m_viewer);
	
	m_grabTimer = new QTimer();
	connect(m_grabTimer, SIGNAL(timeout()), this, SLOT(slotGrabPixmap()));
			
	applyOutputSettings();
	
}

OutputInstance::~OutputInstance() 
{
	if(!m_imgBuffer.isEmpty())
	{
		qDeleteAll(m_imgBuffer);
		m_imgBuffer.clear();
	}
}

void OutputInstance::slotGrabPixmap()
{
	if(!m_output->mjpegServerEnabled())
		return;
		
	if(!m_jpegServer ||
	    m_jpegServer->serverPort() != m_output->mjpegServerPort())
	{
		if(m_jpegServer)
			delete m_jpegServer;
		
		m_jpegServer = new JpegServer();
		m_jpegServer->setProvider(this, SIGNAL(imageReady(QImage*)), false); // false = dont delete the image
		
		if (!m_jpegServer->listen(QHostAddress::Any,m_output->mjpegServerPort())) 
		{
			qDebug() << "JpegServer could not start: "<<m_jpegServer->errorString();
		}
		else
		{
			qDebug() << "OutputInstance"<<output()->name()<<": JpegServer listening on "<<m_jpegServer->myAddress();
		}
	}
	
// 	static int frameCounter = 0;
// 	static double frameTimeSum = 0;
// 	
// 	frameCounter++;
// 	
// 	QTime t;
// 	t.start();
	
	QPixmap frame = QPixmap::grabWidget(m_viewer);
	
	QImage * frameImg = new QImage(frame.toImage());
	
	emit imageReady(frameImg);
	
// 	double elapsed = ((double)t.elapsed()); // / 1000.0;
// 	
// 	frameTimeSum += elapsed;
	
	m_imgBuffer.append(frameImg);
	
	if(m_imgBuffer.size() >= MAX_IMGBUFFER_SIZE)
		delete m_imgBuffer.takeFirst();
	
	//qDebug("Frame: %d, Elapsed: %.02f, Avg: %.02f, FPmS: %.02f, Port: %d", frameCounter, elapsed, frameTimeSum / frameCounter, frameCounter / frameTimeSum, m_jpegServer->serverPort());
}

void OutputInstance::applyOutputSettings()
{
	Output::OutputType x = m_output->outputType();
	if(x == Output::Screen || x == Output::Custom)
	{
		QRect geom;
		if(x == Output::Screen)
		{
			QDesktopWidget *d = QApplication::desktop();
			
			int screenNum = m_output->screenNum();
			geom = d->screenGeometry(screenNum);
			
			setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
		}
		else
		{
			geom = m_output->customRect();
			if(m_output->stayOnTop())
				setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint); // | Qt::ToolTip);
			else
				setWindowFlags(Qt::FramelessWindowHint);
		}

		resize(geom.width(),geom.height());
		move(geom.left(),geom.top());
		
		setVisible(m_output->isEnabled());
		
		setWindowTitle(QString("%1 Output - DViz").arg(m_output->name()));
		setWindowIcon(QIcon(":/data/icon-d.png"));
		
		
		if(m_grabTimer->isActive())
			m_grabTimer->stop();
			
		if(m_output->mjpegServerEnabled())
			m_grabTimer->start(1000 / m_output->mjpegServerFPS());

	}
	else
	if(x == Output::Preview)
	{
		m_viewer->setCursor(Qt::ArrowCursor);
	}
	else
	if(x == Output::Network)
	{
		//qDebug("Warning: Output to network not supported yet. Still to be written.");
		setVisible(false);
			
		if(!m_outputServer ||
		    m_outputServer->serverPort() != m_output->port())
		{
			if(m_outputServer)
				delete m_outputServer;
			
			m_outputServer = new OutputServer();
			m_outputServer->setInstance(this);
			
			if (!m_outputServer->listen(QHostAddress::Any,m_output->port())) 
			{
				qDebug() << "OutputSErver could not start: "<<m_outputServer->errorString();
			}
			else
			{
				qDebug() << "OutputInstance"<<output()->name()<<": OutputServer listening on "<<m_outputServer->myAddress();
			}
		}
	}
}

// proxy methods
void OutputInstance::slotNextGroup()
{
	emit nextGroup();
}


void OutputInstance::slideChanged(Slide *slide, QString slideOperation, AbstractItem */*item*/, QString /*operation*/, QString /*fieldName*/, QVariant /*value*/)
{
	if(!m_slideGroup)
		return;
		
	if(slideOperation == "add" || slideOperation == "remove")
	{
		QList<Slide*> slist = m_slideGroup->slideList();
		qSort(slist.begin(), slist.end(), OuputInstance_slide_num_compare);
		m_sortedSlides = slist;
		
		if(m_slideNum >= m_sortedSlides.size())
			m_slideNum = 0;
	}
	
}

void OutputInstance::setSlideGroup(SlideGroup *group, int startSlide)
{
	if(!group)
		return;
	if(startSlide < 0)
		startSlide = 0;
	if(startSlide > group->numSlides()-1)
		startSlide = group->numSlides()-1;
	setSlideGroup(group, group->at(startSlide));
}

void OutputInstance::setSlideGroup(SlideGroup *group, Slide * startSlide)
{
	Output::OutputType x = m_output->outputType();
	if(x == Output::Screen || x == Output::Custom || x == Output::Preview)
	{
		//qDebug() << "OutputInstance::setSlideGroup: ["<<m_output->name()<<"] Calling m_viewer->setSlideGroup()";
		m_viewer->setSlideGroup(group,startSlide);
	}
	else
	{
		if(startSlide)
			m_slideNum = m_sortedSlides.indexOf(startSlide);
		else
			m_slideNum = 0;
		
		if(m_outputServer)
		{
			qDebug() << "OutputInstance::m_outputServer: Sending group to server, num:"<<m_slideNum;
			m_outputServer->cmdSetSlideGroup(group,m_slideNum);
		}
		else
		{
			qDebug() << "OutputInstance::m_outputSErver: No server created.";
		}
	}
	
	// Replicate the OutputInstance logic internally.
	// Right now, not needed. But for a network viewer, we'll do this to reduce network logic & trafic needed
	
	
	if(m_slideGroup == group)
		return;
		
	m_slideNum = 0;

	if(m_slideGroup)
		disconnect(m_slideGroup,0,this,0);
	
	connect(group,SIGNAL(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)),this,SLOT(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)));

	m_slideGroup  = group;
	
	QList<Slide*> slist = group->slideList();
	qSort(slist.begin(), slist.end(), OuputInstance_slide_num_compare);
	m_sortedSlides = slist;
	
	if(startSlide)
		m_slideNum = m_sortedSlides.indexOf(startSlide);
	else
	{
		QList<Slide*> slist = group->slideList();
		if(slist.size() > 0)
			m_slideNum = 0;
		else
			qDebug("OutputInstance::setSlideGroup: Group[0] has 0 slides");
	}
}

SlideGroup * OutputInstance::slideGroup()
{
	return m_slideGroup;
}

int OutputInstance::numSlides()
{
	return m_slideGroup ? m_slideGroup->numSlides() : -1;
}

void OutputInstance::clear()
{
	Output::OutputType outType = m_output->outputType();
	if(outType == Output::Screen || outType == Output::Custom || outType == Output::Preview)
	{
		m_viewer->clear();
		
	}
	else
	{
		//if(m_outputServer)
		//	cmdClear();
		// TODO
	}
}

void OutputInstance::setBackground(QColor color)
{
	Output::OutputType outType = m_output->outputType();
	if(outType == Output::Screen || outType == Output::Custom || outType == Output::Preview)
	{
		m_viewer->setBackground(color);
	}
	else
	{
		if(m_outputServer)
			m_outputServer->cmdSetBackroundColor(color.name());
	}
}

void OutputInstance::setCanZoom(bool flag)
{
	Output::OutputType outType = m_output->outputType();
	if(outType == Output::Screen || outType == Output::Custom || outType == Output::Preview)
	{
		m_viewer->setCanZoom(flag);
	}
	else
	{
		//if(m_outputServer)
		//	setCanZoom(flag);
		// TODO
	}
}

void OutputInstance::setSceneContextHint(MyGraphicsScene::ContextHint hint)
{
	Output::OutputType outType = m_output->outputType();
	if(outType == Output::Screen || outType == Output::Custom || outType == Output::Preview)
	{
		m_viewer->setSceneContextHint(hint);
	}
	else
	{
		//if(m_outputServer)
		//	cmdSetSceneContextHint(hint);
		// TODO
	}
}

void OutputInstance::setOverlaySlide(Slide * newSlide)
{
	m_overlaySlide = newSlide;
	if(!m_output->isEnabled())
		return;
	Output::OutputType x = m_output->outputType();
	if(x == Output::Screen || x == Output::Custom || x == Output::Preview)
	{
		m_viewer->setOverlaySlide(newSlide);
	}
	else
	{
		if(m_outputServer)
			m_outputServer->cmdSetOverlaySlide(newSlide);
	}
}

void OutputInstance::setOverlayEnabled(bool enable)
{
	m_overlayEnabled = enable;
	if(!m_output->isEnabled())
		return;
	Output::OutputType x = m_output->outputType();
	if(x == Output::Screen || x == Output::Custom || x == Output::Preview)
	{
		m_viewer->setOverlayEnabled(enable);
	}
	else
	{
		//if(m_outputServer)
		//	m_outputServer->setOverlayEnabled(enable);
		// TODO
	}
}

void OutputInstance::setTextOnlyFilterEnabled(bool enable)
{
	m_textOnlyFilter = enable;
	if(!m_output->isEnabled())
		return;
	Output::OutputType x = m_output->outputType();
	if(x == Output::Screen || x == Output::Custom || x == Output::Preview)
	{
		m_viewer->setTextOnlyFilterEnabled(enable);
	}
	else
	{
		// int filterId =  ...
		//if(m_outputServer)
		//	m_outputServer->addFilter(filterId);
	}
	
	m_textOnlyFilter = enable;
	if(enable)
		addFilter(SlideTextOnlyFilter::instance());
	else
		removeFilter(SlideTextOnlyFilter::instance());
}

void OutputInstance::addFilter(AbstractItemFilter * filter)
{
	if(!m_slideFilters.contains(filter))
		m_slideFilters.append(filter);
	//applySlideFilters();
	
	if(!m_output->isEnabled())
		return;
	Output::OutputType x = m_output->outputType();
	if(x == Output::Screen || x == Output::Custom || x == Output::Preview)
	{
		m_viewer->addFilter(filter);
	}
	else
	{
		// int filterId = ...
		// if(m_outputServer)
		//	m_outputServer->addFilter(filterId);
	}
}

void OutputInstance::removeFilter(AbstractItemFilter *filter)
{
	m_slideFilters.removeAll(filter);
	//applySlideFilters();
	
	if(!m_output->isEnabled())
		return;
	Output::OutputType x = m_output->outputType();
	if(x == Output::Screen || x == Output::Custom || x == Output::Preview)
	{
		m_viewer->removeFilter(filter);
	}
	else
	{
		// int filterId = ...
		// if(m_outputServer)
		//	m_outputServer->removeFilter(filterId);
	}
}

void OutputInstance::removeAllFilters()
{
	
	
	if(!m_output->isEnabled())
		return;
	Output::OutputType x = m_output->outputType();
	if(x == Output::Screen || x == Output::Custom || x == Output::Preview)
	{
		m_viewer->removeAllFilters();
	}
	else
	{
// 		foreach(AbstractFilter *filter, m_slideFilters)
// 			// if(m_outputServer)
//			m_outputServer->removeFilter(filterId);
	}
	
	m_slideFilters.clear();
}

void OutputInstance::setAutoResizeTextEnabled(bool enable)
{
	m_autoResizeText = enable;
	if(!m_output->isEnabled())
		return;
	Output::OutputType x = m_output->outputType();
	if(x == Output::Screen || x == Output::Custom || x == Output::Preview)
	{
		m_viewer->setAutoResizeTextEnabled(enable);
	}
	else
	{
		if(m_outputServer)
			m_outputServer->cmdSetAutoResizeTextEnabled(enable);
	}
}


void OutputInstance::setFadeSpeed(int value)
{
	m_fadeSpeed = value;
	if(!m_output->isEnabled())
		return;
	Output::OutputType x = m_output->outputType();
	if(x == Output::Screen || x == Output::Custom || x == Output::Preview)
	{
		m_viewer->setFadeSpeed(value);
	}
	else
	{
		if(m_outputServer)
			m_outputServer->cmdSetFadeSpeed(value);
	}
}


void OutputInstance::setFadeQuality(int value)
{
	m_fadeQuality = value;
	if(!m_output->isEnabled())
		return;
	Output::OutputType x = m_output->outputType();
	if(x == Output::Screen || x == Output::Custom || x == Output::Preview)
	{
		m_viewer->setFadeQuality(value);
	}
	else
	{
		if(m_outputServer)
			m_outputServer->cmdSetFadeQuality(value);
	}
}


Slide * OutputInstance::setSlide(int x)
{
	//qDebug() << "OutputInstance::setSlide: ["<<m_output->name()<<"] Setting slide#:"<<x;
	if(x >= m_sortedSlides.size() || x<0)
	{
		//qDebug() << "OutputInstance::setSlide: ["<<m_output->name()<<"] "<<x<<" is out of range, size: "<<m_sortedSlides.size();
		return 0;
	}
		
	return setSlide(m_sortedSlides.at(x));	
}

Slide * OutputInstance::setSlide(Slide *slide)
{
	if(!m_output->isEnabled())
		return 0;
		
	m_slideNum = m_sortedSlides.indexOf(slide);
	if(m_slideNum >-1 )
	{
		//qDebug() << "OutputInstance::setSlide: ["<<m_output->name()<<"] emit slideChanged("<<m_slideNum<<")";
		emit slideChanged(m_slideNum);
	}
	else
	{
		//qDebug() << "OutputInstance::setSlide: ["<<m_output->name()<<"] Slide ptr given isn't in my list of m_sortedSlides!";
	}
	
	Output::OutputType x = m_output->outputType();
	if(x == Output::Screen || x == Output::Custom || x == Output::Preview)
	{
		//qDebug() << "OutputInstance::setSlide: ["<<m_output->name()<<"] Setting slide#"<<m_slideNum;
		m_viewer->setSlide(slide);
		
	}
	else
	{
		if(m_outputServer)
		{
			qDebug() << "OutputInstance::m_outputServer: Sending slide to server, num:"<<m_slideNum;
			m_outputServer->cmdSetSlide(m_slideNum);
		}
	}
	
	return slide;
}


Slide * OutputInstance::nextSlide()
{
	m_slideNum ++;
	if(m_slideNum >= m_sortedSlides.size())
	{
		if(m_slideGroup->autoChangeGroup())
		{
			//m_slideNum = m_sortedSlides.size() - 1;
			emit nextGroup();
			return 0;
		}
		else
		{
			m_slideNum = 0;
		}
	}
	
	return setSlide(m_slideNum);
}

Slide * OutputInstance::prevSlide()
{
	m_slideNum --;
	if(m_slideNum < 0)
		m_slideNum = 0;
	return setSlide(m_slideNum);
}

void OutputInstance::fadeBlackFrame(bool enable)
{
	if(!m_output->isEnabled())
		return;
		
	if(m_sortedSlides.size() <= 0)
		return;
		
	Output::OutputType outType = m_output->outputType();
	if(outType == Output::Screen || outType == Output::Custom || outType == Output::Preview)
	{
		m_viewer->fadeBlackFrame(enable);
	}
	else
	{
		if(m_outputServer)
			m_outputServer->cmdFadeBlack(enable);
	}
}

void OutputInstance::fadeClearFrame(bool enable)
{
	if(!m_output->isEnabled())
		return;
		
	Output::OutputType outType = m_output->outputType();
	if(outType == Output::Screen || outType == Output::Custom || outType == Output::Preview)
	{
		m_viewer->fadeClearFrame(enable);
	}
	else
	{
		if(m_outputServer)
			m_outputServer->cmdFadeClear(enable);
	}
}

void OutputInstance::setLiveBackground(const QFileInfo &info, bool waitForNextSlide)
{
	if(!m_output->isEnabled())
		return;
		
	Output::OutputType outType = m_output->outputType();
	if(outType == Output::Screen || outType == Output::Custom || outType == Output::Preview)
	{
		m_viewer->setLiveBackground(info,waitForNextSlide);
	}
	else
	{
		if(m_outputServer)
			m_outputServer->cmdSetLiveBackground(info.absoluteFilePath(),waitForNextSlide);
	}
}
