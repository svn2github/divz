#include "OutputInstance.h"
#include "model/SlideGroup.h"
#include "MainWindow.h"
#include "AppSettings.h"
#include "MyGraphicsScene.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QVBoxLayout>
#include <QDomDocument>
#include <QTextStream>
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
#define OWNED_SLIDE_BUFFER 10

bool OuputInstance_slide_num_compare(Slide *a, Slide *b)
{
	return (a && b) ? a->slideNumber() < b->slideNumber() : true;
}

OutputInstance::OutputInstance(Output *out, bool /*startHidden*/, QWidget *parent)
	: QWidget(parent)
	, m_output(out)
	, m_viewer(0)
	, m_slideGroup(0)
	, m_slideNum(-1)
	, m_slide(0)
	, m_isFoldback(false)
	, m_jpegServer(0)
	, m_outputServer(0)
	, m_clearEnabled(false)
	, m_blackEnabled(false)
	, m_overrideEndAction(false)
	, m_groupEndAction(SlideGroup::Stop)
	, m_forceTransmitRawSlide(false)
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
	m_viewer->setOutputId(out->id());
	connect(m_viewer, SIGNAL(nextGroup()), this, SLOT(slotNextGroup()));
	connect(m_viewer, SIGNAL(jumpToGroup(int)), this, SLOT(slotJumpToGroup(int)));
	
	if(out->outputType() == Output::Preview)
	{
		// ::Preview is generally used for the preview widget under the slide group list or for the media browser preview widget
		m_viewer->setSceneContextHint(MyGraphicsScene::Preview);
	}
	else
	if(out->outputType() == Output::Viewer)
	{
		// Output::Viewer's are the dock widgets on the right that show what the live output should be showing for that widget
		m_viewer->setSceneContextHint(MyGraphicsScene::Monitor);
	}
	else
	{
		// ::Live is, well, live...
		m_viewer->setSceneContextHint(MyGraphicsScene::Live);
	}
	
	layout->addWidget(m_viewer);

	QPalette p;
	p.setColor(QPalette::Window, Qt::black);
	setPalette(p);
	
	//m_grabTimer = new QTimer();
	//connect(m_grabTimer, SIGNAL(timeout()), this, SLOT(slotGrabPixmap()));
	
	if(out->tags().toLower().indexOf("live") >= 0 ||
	   out->name().toLower().indexOf("live") >= 0)
		m_viewer->setSharedMemoryImageWriterEnabled(true,"dviz/live");
			
	applyOutputSettings(/*startHidden*/);
	
}

OutputInstance::~OutputInstance() 
{
// 	qDebug() << "OutputInstance::~OutputInstance: Destroying "<<(void*)this;
	while(m_ownedSlides.size())
	{	
		QPointer<Slide> slide = m_ownedSlides.takeFirst();
		if(slide)
		{
			//Slide *slidePtr = (Slide*)slide;
			//delete slidePtr;
		}
	}
			
	if(!m_imgBuffer.isEmpty())
	{
		qDeleteAll(m_imgBuffer);
		m_imgBuffer.clear();
	}
}

void OutputInstance::addMirror(OutputInstance *inst)
{
	//qDebug() << "OutputInstance::addMirror: ["<<m_output->name()<<"] Adding mirror instance ptr"<<inst<<", mirror inst output name: "<<inst->output()->name();
	if(!m_mirrors.contains(inst))
		m_mirrors << inst;
}

void OutputInstance::removeMirror(OutputInstance *inst)
{
	if(!inst)
		return;
// 	if(inst->output())
// 		qDebug() << "OutputInstance::removeMirror: ["<<m_output->name()<<"] Removing mirror instance ptr"<<inst<<", mirror inst output name: "<<inst->output()->name();
	m_mirrors.removeAll(inst);
}

void OutputInstance::slotGrabPixmap()
{
/*	if(!m_output->mjpegServerEnabled())
		return;
		
	if(!m_jpegServer ||
	    m_jpegServer->serverPort() != m_output->mjpegServerPort())
	{
		if(m_jpegServer)
			delete m_jpegServer;
		
		m_jpegServer = new JpegServer();
		m_jpegServer->setProvider(this, SIGNAL(imageReady(QImage*)));
		
		if (!m_jpegServer->listen(QHostAddress::Any,m_output->mjpegServerPort())) 
		{
			qDebug() << "JpegServer could not start: "<<m_jpegServer->errorString();
		}
		else
		{
			//qDebug() << "OutputInstance"<<output()->name()<<": JpegServer listening on "<<m_jpegServer->myAddress();
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
	
	//qDebug("Frame: %d, Elapsed: %.02f, Avg: %.02f, FPmS: %.02f, Port: %d", frameCounter, elapsed, frameTimeSum / frameCounter, frameCounter / frameTimeSum, m_jpegServer->serverPort());*/
}

void OutputInstance::applyOutputSettings(bool startHidden)
{
	Output::OutputType x = m_output->outputType();
	m_viewer->setIsPreviewViewer(false); // reset in case changed
	if(x == Output::Screen || x == Output::Custom)
	{
		
		QRect geom;
		if(x == Output::Screen)
		{
			QDesktopWidget *d = QApplication::desktop();	
			int screenNum = m_output->screenNum();
			geom = d->screenGeometry(screenNum);
			
			int mws = d->screenNumber(MainWindow::mw());
// 			if(m_output->isEnabled())
// 				qDebug() <<"OutputInstance::applyOutputSettings: ["<<m_output->name()<<"] mws:"<<mws<<", screenNum:"<<screenNum;
			if(d->numScreens() == 1 || screenNum == mws)
			{
				setWindowFlags(Qt::FramelessWindowHint);
// 				if(m_output->isEnabled())
// 					qDebug() <<"OutputInstance::applyOutputSettings: ["<<m_output->name()<<"] no tooltip!";
			}
			else
				setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
		}
		else
		{
			geom = m_output->customRect();
			if(m_output->stayOnTop())
				setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::ToolTip);
			else
				setWindowFlags(Qt::FramelessWindowHint);
		}

		resize(geom.width(),geom.height());
		move(geom.left(),geom.top());
		
// 		int ows = d->screenNumber(this);
// 		if(m_output->isEnabled())
// 			qDebug() <<"OutputInstance::applyOutputSettings: ["<<m_output->name()<<"] enabled: "<<m_output->isEnabled()<<", geom:"<<geom<<", mw geom:"<<MainWindow::mw()->pos()<<"-"<<MainWindow::mw()->size()<<", ows:"<<ows;
		
		if(startHidden)
			setVisible(false);
		else
			setVisible(m_output->isEnabled());
		
		setWindowTitle(QString("%1 Output - DViz").arg(m_output->name()));
		setWindowIcon(QIcon(":/data/icon-d.png"));
		
// 		if(m_grabTimer->isActive())
// 			m_grabTimer->stop();
			
// 		if(m_output->isEnabled() &&
// 		   m_output->mjpegServerEnabled())
// 			m_grabTimer->start(1000 / m_output->mjpegServerFPS());
		
		m_viewer->setMjpegServerEnabled(m_output->mjpegServerEnabled(), m_output->mjpegServerPort(), m_output->mjpegServerFPS());
		
		m_viewer->setIgnoreAspectRatio(m_output->ignoreAR()); 

// 		if(!m_jpegServer ||
// 		m_jpegServer->serverPort() != m_output->mjpegServerPort())
// 		{
// 			if(m_jpegServer)
// 				delete m_jpegServer;
// 			
// 			m_jpegServer = new JpegServer();
// 			m_jpegServer->setProvider(this, SIGNAL(imageReady(QImage*)));
// 			
// 			if (!m_jpegServer->listen(QHostAddress::Any,m_output->mjpegServerPort())) 
// 			{
// 				qDebug() << "JpegServer could not start: "<<m_jpegServer->errorString();
// 			}
// 			else
// 			{
// 				//qDebug() << "OutputInstance"<<output()->name()<<": JpegServer listening on "<<m_jpegServer->myAddress();
// 			}
// 		}
	}
	else
	if(x == Output::Preview || x == Output::Viewer)
	{
		m_viewer->setCursor(Qt::ArrowCursor);
		m_viewer->setIsPreviewViewer(true);
	}
	else
	if(x == Output::Widget)
	{
		//m_viewer->setCursor(Qt::ArrowCursor);
	}
	else
	if(x == Output::Network)
	{
		setVisible(false);
			
		if(m_output->isEnabled() &&
		  (!m_outputServer ||
		    m_outputServer->serverPort() != m_output->port()))
		{
			if(m_outputServer)
			{
				m_outputServer->close();
				delete m_outputServer;
			}
			
			m_outputServer = new OutputServer();
			m_outputServer->setInstance(this);
			
			if (!m_outputServer->listen(QHostAddress::Any,m_output->port())) 
			{
				qDebug() << "OutputInstance"<<output()->name()<<": OutputServer could not start: "<<m_outputServer->errorString();
			}
			else
			{
				qDebug() << "OutputInstance"<<output()->name()<<": OutputServer listening on "<<m_outputServer->myAddress();
			}
		}
	}
	
	if((x != Output::Network || !m_output->isEnabled())
	    && m_outputServer)
	{
		m_outputServer->close();
		delete m_outputServer;
	}
}

// proxy methods
void OutputInstance::slotNextGroup()
{
	emit nextGroup();
}

void OutputInstance::slotJumpToGroup(int x)
{
	emit jumpToGroup(x);
}


void OutputInstance::slideChanged(Slide *slide, QString slideOperation, AbstractItem */*item*/, QString /*operation*/, QString /*fieldName*/, QVariant /*value*/)
{
	//qDebug() << "OutputInstance::slideChanged: ["<<m_output->name()<<"] slideOperation:"<<slideOperation;
	if(!m_slideGroup)
		return;
	if(!slide)
		return;
		
	if(slideOperation == "add" || slideOperation == "remove")
	{
		QList<Slide*> slist = m_slideGroup->slideList();
		qSort(slist.begin(), slist.end(), OuputInstance_slide_num_compare);
		m_sortedSlides = slist;
		
// 		Output::OutputType x = m_output->outputType();
// 		if(x == Output::Network)
// 		{
// 			setSlideGroup(m_slideGroup, m_slideNum);
// 			m_forceTransmitRawSlide = true;
// 			setSlide(slide);
// 			m_forceTransmitRawSlide = false;
// 		}
		
		if(m_slideNum >= m_sortedSlides.size())
			m_slideNum = 0;
	}
	else
	{
		Output::OutputType x = m_output->outputType();
		if(x == Output::Network)
		{

			int idx = m_sortedSlides.indexOf(slide);
			//if(idx > -1)
// 			{
				// TODO do we need to support idx<0, e.g. changes to a slide not in slide group - rather, should changes to slide not in group make slide live - becausae slide wouldnt be live right now,.....would it?
				if(/*idx < 0 || */idx == m_slideNum)
				{
					m_forceTransmitRawSlide = true;
					setSlide(idx);
					m_forceTransmitRawSlide = false;
				}
// 			}
// 			else
				//qDebug() << "OutputInstance::slideChanged: ["<<m_output->name()<<"] setting slide ptr";
			//m_forceTransmitRawSlide = true;
// 				setSlide(slide);
			//m_forceTransmitRawSlide = false;
		}
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
	//qDebug() << "OutputInstance::setSlideGroup: ["<<m_output->name()<<"] group name:"<<group->assumedName()<<", startSlide#:"<<startSlide;
	setSlideGroup(group, group->at(startSlide));
}

void OutputInstance::setSlideGroup(SlideGroup *group, Slide * startSlide)
{
// 	qDebug() << "OutputInstance::setSlideGroup: ["<<m_output->name()<<"] emitting slideGroupChanged(), group:"<<group->assumedName();
	emit slideGroupChanged(group,startSlide);
	foreach(OutputInstance *m, m_mirrors)
		m->setSlideGroup(group,startSlide);
	
	if(isLocal())
	{
// 		qDebug() << "OutputInstance::setSlideGroup: ["<<m_output->name()<<"] Calling m_viewer->setSlideGroup(), group:"<<group->assumedName()<<", startSlide:"<<startSlide;
		//setVisible(m_output->isEnabled());
		if(startSlide)
			m_slideNum = m_sortedSlides.indexOf(startSlide);
		else
			m_slideNum = 0;

		bool foundAlt = false;
		if(group)
		{
			SlideGroup *altGroup = group->altGroupForOutput(m_output);
			if(altGroup)
			{
				foundAlt = true;
				
				// resolve the start slide for the alt group - just match by index
				Slide *newStart = 0;
				int startIdx = group->indexOf(startSlide);
				if(startIdx > -1)
					newStart = altGroup->at(startIdx);
				
				m_viewer->setSlideGroup(altGroup,newStart);
			}
		}
		
		if(!foundAlt)
			m_viewer->setSlideGroup(group,startSlide);
	}
	else
	{
		//qDebug() << "OutputInstance::setSlideGroup: ["<<m_output->name()<<"] Calling m_outputServer->sendCommand(), group:"<<group->assumedName()<<",  startSlide:"<<startSlide;
		if(startSlide)
			m_slideNum = m_sortedSlides.indexOf(startSlide);
		else
			m_slideNum = 0;
		
		if(m_outputServer)
		{
			// send it to the client
			m_outputServer->sendCommand(OutputServer::SetSlideGroup,group->toByteArray(),m_slideNum);
		}
		else
		{
			qDebug() << "OutputInstance::setSlideGroup: ["<<m_output->name()<<"] Cannot send to m_outputServer because m_outputServer is NULL";
		}
	}
	
	
	// Replicate the OutputInstance logic internally.
	// Right now, not needed. But for a network viewer, we'll do this to reduce network logic & trafic needed
	
	setSlideGroupInternal(group,startSlide);
}

Slide * OutputInstance::setSlideGroupInternal(SlideGroup *group, Slide * startSlide)
{
	if(m_slideGroup == group)
		return startSlide;
		
	m_slideNum = 0;

	if(m_slideGroup)
		disconnect(m_slideGroup,0,this,0);
	
	connect(group,SIGNAL(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)),this,SLOT(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)));

	m_slideGroup = group;
	
	QList<Slide*> slist = group->slideList();
	qSort(slist.begin(), slist.end(), OuputInstance_slide_num_compare);
	m_sortedSlides = slist;
	
	if(startSlide)
	{
		setSlideInternal(startSlide);
// 		m_slideNum = m_sortedSlides.indexOf(startSlide);
		return startSlide;
	}
	else
	{
		QList<Slide*> slist = group->slideList();
		if(slist.size() > 0)
			m_slideNum = 0;
		else
			qDebug("OutputInstance::setSlideGroup: Group[0] has 0 slides");
			
		Slide * first = slist.first();
		setSlideInternal(first);
		
		return first;
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
	foreach(OutputInstance *m, m_mirrors)
		m->clear();
	if(isLocal())
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

void OutputInstance::setViewerState(SlideGroupViewer::ViewerState state)
{
	foreach(OutputInstance *m, m_mirrors)
		m->setViewerState(state);
	if(isLocal())
	{
		m_viewer->setViewerState(state);

	}
	else
	{
		//if(m_outputServer)
		//	setViewerState();
		// TODO
	}
}

void OutputInstance::setBackground(QColor color)
{
	foreach(OutputInstance *m, m_mirrors)
		m->setBackground(color);
	if(isLocal())
	{
		m_viewer->setBackground(color);
	}
	else
	{
		if(m_outputServer)
			m_outputServer->sendCommand(OutputServer::SetBackgroundColor,color.name());
	}
}

void OutputInstance::setCanZoom(bool flag)
{
	if(isLocal())
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

void OutputInstance::forceGLDisabled(bool flag)
{
	if(isLocal())
	{
		m_viewer->forceGLDisabled(flag);
	}
	else
	{
		//if(m_outputServer)
		//	cmdSetSceneContextHint(hint);
		// TODO
	}
}

bool OutputInstance::isTransitionActive()
{
	if(!isLocal())
		return false;
		
	return m_viewer->isTransitionActive();
}

void OutputInstance::setSceneContextHint(MyGraphicsScene::ContextHint hint)
{
	if(isLocal())
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
	foreach(OutputInstance *m, m_mirrors)
		m->setOverlaySlide(newSlide);
	m_overlaySlide = newSlide;
	if(!m_output->isEnabled())
		return;
	if(isLocal())
	{
		m_viewer->setOverlaySlide(newSlide); 
	}
	else
	{
		if(m_outputServer)
		{
		/*
			QString xmlString;
			QDomDocument doc;
			QTextStream out(&xmlString);
			
			// This element contains all the others.
			QDomElement rootElement = doc.createElement("slide");
		
			newSlide->toXml(rootElement);
			
			// Add the root (and all the sub-nodes) to the document
			doc.appendChild(rootElement);
			
			//Add at the begining : <?xml version="1.0" ?>
			QDomNode noeud = doc.createProcessingInstruction("xml","version=\"1.0\" ");
			doc.insertBefore(noeud,doc.firstChild());
			//save in the file (4 spaces indent)
			doc.save(out, 4);*/
			
			m_outputServer->sendCommand(OutputServer::SetOverlaySlide, newSlide->toByteArray());
		}
	}
}

void OutputInstance::setOverlayEnabled(bool enable)
{
	foreach(OutputInstance *m, m_mirrors)
		m->setOverlayEnabled(enable);
	m_overlayEnabled = enable;
	if(!m_output->isEnabled())
		return;
	if(isLocal())
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
	foreach(OutputInstance *m, m_mirrors)
		m->setTextOnlyFilterEnabled(enable);
	m_textOnlyFilter = enable;
	if(!m_output->isEnabled())
		return;
	if(isLocal())
	{
		m_viewer->setTextOnlyFilterEnabled(enable);
	}
	else
	{
		// handled in addFilter for network
	}
	
	m_textOnlyFilter = enable;
	if(enable)
		addFilter(SlideTextOnlyFilter::instance());
	else
		removeFilter(SlideTextOnlyFilter::instance());
}

void OutputInstance::addFilter(AbstractItemFilter * filter)
{
	foreach(OutputInstance *m, m_mirrors)
		m->addFilter(filter);
		
	if(!m_slideFilters.contains(filter))
		m_slideFilters.append(filter);
	//applySlideFilters();
	
	if(!m_output->isEnabled())
		return;
	if(isLocal())
	{
		m_viewer->addFilter(filter);
	}
	else
	{
		if(m_outputServer)
			m_outputServer->sendCommand(OutputServer::AddFilter, filter->filterId());
	}
}

void OutputInstance::removeFilter(AbstractItemFilter *filter)
{
	foreach(OutputInstance *m, m_mirrors)
		m->removeFilter(filter);
		
	m_slideFilters.removeAll(filter);
	//applySlideFilters();
	
	if(!m_output->isEnabled())
		return;
	if(isLocal())
	{
		m_viewer->removeFilter(filter);
	}
	else
	{
		if(m_outputServer)
			m_outputServer->sendCommand(OutputServer::DelFilter, filter->filterId());
	}
}

void OutputInstance::removeAllFilters()
{
	//qDebug() << "OutputInstance::setSlide: ["<<m_output->name()<<"] Updating mirrors - remove all filters, # mirrors:"<<m_mirrors.size();
	foreach(OutputInstance *m, m_mirrors)
	{
		//qDebug() << "OutputInstance::setSlide: ["<<m_output->name()<<"] MirrorPtr: "<<(void*)m;
		m->removeAllFilters();
	}
	
	if(!m_output->isEnabled())
		return;
	if(isLocal())
	{
		m_viewer->removeAllFilters();
	}
	else
	{
 		 if(m_outputServer)
			foreach(AbstractItemFilter *filter, m_slideFilters)
 				m_outputServer->sendCommand(OutputServer::DelFilter, filter->filterId());
	}
	
	m_slideFilters.clear();
}

void OutputInstance::setAutoResizeTextEnabled(bool enable)
{
	foreach(OutputInstance *m, m_mirrors)
		m->setAutoResizeTextEnabled(enable);
	
	m_autoResizeText = enable;
	if(!m_output->isEnabled())
		return;
	if(isLocal())
	{
		m_viewer->setAutoResizeTextEnabled(enable);
	}
	else
	{
		if(m_outputServer)
			m_outputServer->sendCommand(OutputServer::SetTextResize,enable);
	}
}


void OutputInstance::setFadeSpeed(int value)
{
	foreach(OutputInstance *m, m_mirrors)
		m->setFadeSpeed(m->output() == Output::previewInstance() ||
		                m->output() == Output::viewerInstance()   ? 0 : value);
	
	m_fadeSpeed = value;
	if(!m_output->isEnabled())
		return;
	if(isLocal())
	{
		m_viewer->setFadeSpeed(value);
	}
	else
	{
		if(m_outputServer)
			m_outputServer->sendCommand(OutputServer::SetFadeSpeed,value);
	}
}


void OutputInstance::setFadeQuality(int value)
{
	foreach(OutputInstance *m, m_mirrors)
		m->setFadeQuality(m->output() == Output::previewInstance() ||
		                  m->output() == Output::viewerInstance()   ? 0 : value);
	
	m_fadeQuality = value;
	if(!m_output->isEnabled())
		return;
	if(isLocal())
	{
		m_viewer->setFadeQuality(value);
	}
	else
	{
		if(m_outputServer)
			m_outputServer->sendCommand(OutputServer::SetFadeQuality,value);
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

void OutputInstance::setSlideInternal(Slide *slide)
{
	m_slideNum = m_sortedSlides.indexOf(slide);
	
// 	qDebug() << "OutputInstance::setSlide: ["<<m_output->name()<<"] emitting slideChanged() for slide="<<slide;
	emit slideChanged(slide);
	if(m_slideNum > -1)
		emit slideChanged(m_slideNum);
		
	m_slide = slide;
}

Slide * OutputInstance::setSlide(Slide *slide, bool takeOwnership)
{
	//qDebug() << "OutputInstance::setSlide: ["<<m_output->name()<<"] Updating mirrors to slidePtr"<<slide;
	foreach(OutputInstance *m, m_mirrors)
		m->setSlide(slide);
		
	if(!m_output->isEnabled())
		return 0;
		
	if(isLocal())
	{
		//qDebug() << "OutputInstance::setSlide: ["<<m_output->name()<<"] Setting slide#"<<m_slideNum;
		//setVisible(m_output->isEnabled());
		
		if(slide)
			m_slideNum = m_sortedSlides.indexOf(slide);
		else
			m_slideNum = 0;
		
		bool foundAlt = false;
		if(!takeOwnership && 
		    m_slideGroup)
		{
			SlideGroup *altGroup = m_slideGroup->altGroupForOutput(m_output);
			if(altGroup)
			{
				//foundAlt = true;
				
				// resolve the start slide for the alt group - just match by index
				Slide *newSlide = 0;
				
				QList<Slide*> slist = altGroup->slideList();
				qSort(slist.begin(), slist.end(), OuputInstance_slide_num_compare);
				newSlide = slist.isEmpty() ? 0 :
					m_slideNum < slist.size() && m_slideNum > -1 ? slist.at(m_slideNum) : 0;
				
// 				int startIdx = group->indexOf(slide);
// 				if(startIdx > -1)
// 					newStart = altGroup->at(startIdx);
				
				if(newSlide)
				{
					m_viewer->setSlide(newSlide); // NOT takeOwnership here....checked above...
					foundAlt = true;
				}
			}
		}
		
		if(!foundAlt)
			m_viewer->setSlide(slide,takeOwnership);
		
		// 20100216: Moved setSlideInternal() to AFTER the call to m_viewer inorder for proper controlWidgets()
		// functionality. In MyGraphicsScene, the parentItem() is checked and only items with the "live root"
		// as the parent are queried for control widgets. By placing setSlideInternal() after m_viewer->setSlide()
		// this ensures that the slideChanged() signal will be emitted, well, *after* the slide is changed, ensuring
		// that the parentItem has had a change to be changed before controlWidgets() is queried for each item 
		// in the scene.
		setSlideInternal(slide);
	}
	else
	{
		setSlideInternal(slide);
	
		if(m_outputServer)
		{
			//qDebug() << "OutputInstance::m_outputServer: Sending slide to server, num:"<<m_slideNum;
			
			// Try to save network bandwidth and time by transmitting just the slide index if possible.
			// If the slide is not in the slide group, then transmit it as a byte array.
			if(slide && (m_slideNum < 0 || m_forceTransmitRawSlide))
				m_outputServer->sendCommand(OutputServer::SetSlideObject, slide->toByteArray());
			else
				m_outputServer->sendCommand(OutputServer::SetSlide,m_slideNum);
			//else
			//	qDebug() << "OutputInstance::setSlide: ["<<m_output->name()<<"] Cannot send slide to server instance because 'slide' is NULL";
		}
		
		if(takeOwnership)
			m_ownedSlides.append(QPointer<Slide>(slide));
		
		while(m_ownedSlides.size() > OWNED_SLIDE_BUFFER)
		{	
			QPointer<Slide> slide = m_ownedSlides.takeFirst();
			if(slide)
			{
				//Slide *slidePtr = (Slide*)slide;
				//delete slidePtr;
			}
		}
		
		// TODO handle ownership of slides sent to output server
			
		// TODO handle slides not in slide group sent to output server			
	}
	
	return slide;
}

void OutputInstance::setEndActionOverrideEnabled(bool flag) 
{
	m_overrideEndAction = flag;
}

void OutputInstance::setEndGroupAction(SlideGroup::EndOfGroupAction act)
{
	m_groupEndAction = act;
}
	
Slide * OutputInstance::nextSlide()
{
	if(!m_slideGroup)
		return 0;
		
	m_slideNum ++;
	if(m_slideNum >= m_sortedSlides.size())
	{
		SlideGroup::EndOfGroupAction action = m_slideGroup->endOfGroupAction();
		if(isEndActionOverrideEnabled())
			action = groupEndAction();
			
		if(action == SlideGroup::GotoNextGroup)
		{
			//m_slideNum = m_sortedSlides.size() - 1;
			emit nextGroup();
			return 0;
		}
		else
		if(action == SlideGroup::GotoGroupIndex)
		{
			//m_slideNum = m_sortedSlides.size() - 1;
			emit jumpToGroup(m_slideGroup->jumpToGroupIndex());
			return 0;
		}
		else
		if(action == SlideGroup::LoopToStart)
		{
			m_slideNum = 0;
		}
		else
		{
			emit endOfGroup();
			return 0;
		}
	}
	
	return setSlide(m_slideNum);
}

Slide * OutputInstance::prevSlide()
{
	if(!m_slideGroup)
		return 0;
		
	m_slideNum --;
	if(m_slideNum < 0)
		m_slideNum = 0;
	return setSlide(m_slideNum);
}

void OutputInstance::fadeBlackFrame(bool enable)
{
	foreach(OutputInstance *m, m_mirrors)
		m->fadeBlackFrame(enable);
	
	if(!m_output->isEnabled())
		return;
		
	// set the flag only if the output is enabled inorder to reflect the true status of the output
	m_blackEnabled = enable;
	
	if(isLocal())
	{
		m_viewer->fadeBlackFrame(enable);
	}
	else
	{
		if(m_outputServer)
			m_outputServer->sendCommand(OutputServer::FadeBlack,enable);
	}
	
// 	if(m_sortedSlides.size() <= 0)
// 		return;
}

// void OutputInstance::fadeClearFrame(bool enable)
// {
// 	foreach(OutputInstance *m, m_mirrors)
// 		m->fadeClearFrame(enable);
// 		
// 	if(!m_output->isEnabled())
// 		return;
// 	
// 	// set the flag only if the output is enabled inorder to reflect the true status of the output
// 	m_clearEnabled = enable;
// 	
// 	Output::OutputType outType = m_output->outputType();
// 	if(outType == Output::Screen || outType == Output::Custom || outType == Output::Preview || outType == Output::Widget)
// 	{
// 		m_viewer->fadeClearFrame(enable);
// 	}
// 	else
// 	{
// 		if(m_outputServer)
// 			m_outputServer->sendCommand(OutputServer::FadeClear,enable);
// 	}
// }

void OutputInstance::setLiveBackground(const QFileInfo &info, bool waitForNextSlide)
{
// 	foreach(OutputInstance *m, m_mirrors)
// 		m->setLiveBackground(info,waitForNextSlide);
		
	if(!m_output->isEnabled())
		return;
		
	if(isLocal())
	{
		m_viewer->setLiveBackground(info,waitForNextSlide);
	}
	else
	{
		if(m_outputServer)
			m_outputServer->sendCommand(OutputServer::SetLiveBackground,info.absoluteFilePath(),waitForNextSlide);
	}
}


bool OutputInstance::isLocal()
{
	Output::OutputType outType = m_output->outputType();
	return (outType == Output::Screen || outType == Output::Custom || outType == Output::Preview || outType == Output::Widget || outType == Output::Viewer);
}

QList<QWidget*> OutputInstance::controlWidgets()
{
	if(!isLocal())
		return QList<QWidget*>();
	return m_viewer->controlWidgets();
}
