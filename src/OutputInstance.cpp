#include "OutputInstance.h"
#include "model/SlideGroup.h"
#include "MainWindow.h"
#include "AppSettings.h"
#include "MyGraphicsScene.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QVBoxLayout>

#include "itemlistfilters/SlideTextOnlyFilter.h"

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
{
	out->setInstance(this);
	
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
	
	applyOutputSettings();
}

OutputInstance::~OutputInstance() {}

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
			
			setWindowFlags(Qt::FramelessWindowHint);
		}
		else
		{
			geom = m_output->customRect();
			setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
		}

		resize(geom.width(),geom.height());
		move(geom.left(),geom.top());
		
		setVisible(m_output->isEnabled());
		
		setWindowTitle(QString("%1 Output - DViz").arg(m_output->name()));
		setWindowIcon(QIcon(":/data/icon-d.png"));
	}
	else
	//if(x == Output::Network)
	{
		qDebug("Warning: Output to network not supported yet. Still to be written.");
		setVisible(false);
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


void OutputInstance::setSlideGroup(SlideGroup *group, Slide * startSlide)
{
	Output::OutputType x = m_output->outputType();
	if(x == Output::Screen || x == Output::Custom)
	{
		m_viewer->setSlideGroup(group,startSlide);
	}
	else
	{
		// TODO
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
	if(outType == Output::Screen || outType == Output::Custom)
	{
		m_viewer->clear();
		
	}
	else
	{
		// TODO
	}
}

void OutputInstance::setBackground(QColor color)
{
	Output::OutputType outType = m_output->outputType();
	if(outType == Output::Screen || outType == Output::Custom)
	{
		m_viewer->setBackground(color);
	}
	else
	{
		// TODO
	}
}

void OutputInstance::setSceneContextHint(MyGraphicsScene::ContextHint hint)
{
	Output::OutputType outType = m_output->outputType();
	if(outType == Output::Screen || outType == Output::Custom)
	{
		m_viewer->setSceneContextHint(hint);
	}
	else
	{
		// TODO
	}
}

void OutputInstance::setOverlaySlide(Slide * newSlide)
{
	m_overlaySlide = newSlide;
	if(!m_output->isEnabled())
		return;
	Output::OutputType x = m_output->outputType();
	if(x == Output::Screen || x == Output::Custom)
	{
		m_viewer->setOverlaySlide(newSlide);
	}
	else
	{
		// TODO
	}
}

void OutputInstance::setOverlayEnabled(bool enable)
{
	m_overlayEnabled = enable;
	if(!m_output->isEnabled())
		return;
	Output::OutputType x = m_output->outputType();
	if(x == Output::Screen || x == Output::Custom)
	{
		m_viewer->setOverlayEnabled(enable);
	}
	else
	{
		// TODO
	}
}

void OutputInstance::setTextOnlyFilterEnabled(bool enable)
{
	m_textOnlyFilter = enable;
	if(!m_output->isEnabled())
		return;
	Output::OutputType x = m_output->outputType();
	if(x == Output::Screen || x == Output::Custom)
	{
		m_viewer->setTextOnlyFilterEnabled(enable);
	}
	else
	{
		// TODO
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
	if(x == Output::Screen || x == Output::Custom)
	{
		m_viewer->addFilter(filter);
	}
	else
	{
		// TODO
	}
}

void OutputInstance::removeFilter(AbstractItemFilter *filter)
{
	m_slideFilters.removeAll(filter);
	//applySlideFilters();
	
	if(!m_output->isEnabled())
		return;
	Output::OutputType x = m_output->outputType();
	if(x == Output::Screen || x == Output::Custom)
	{
		m_viewer->removeFilter(filter);
	}
	else
	{
		// TODO
	}
}

void OutputInstance::setAutoResizeTextEnabled(bool enable)
{
	m_autoResizeText = enable;
	if(!m_output->isEnabled())
		return;
	Output::OutputType x = m_output->outputType();
	if(x == Output::Screen || x == Output::Custom)
	{
		m_viewer->setAutoResizeTextEnabled(enable);
	}
	else
	{
		// TODO
	}
}


void OutputInstance::setFadeSpeed(int value)
{
	m_fadeSpeed = value;
	if(!m_output->isEnabled())
		return;
	Output::OutputType x = m_output->outputType();
	if(x == Output::Screen || x == Output::Custom)
	{
		m_viewer->setFadeSpeed(value);
	}
	else
	{
		// TODO
	}
}


void OutputInstance::setFadeQuality(int value)
{
	m_fadeQuality = value;
	if(!m_output->isEnabled())
		return;
	Output::OutputType x = m_output->outputType();
	if(x == Output::Screen || x == Output::Custom)
	{
		m_viewer->setFadeQuality(value);
	}
	else
	{
		// TODO
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
	if(x == Output::Screen || x == Output::Custom)
	{
		m_viewer->setSlide(slide);
		
	}
	else
	{
		// TODO
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
	if(outType == Output::Screen || outType == Output::Custom)
	{
		m_viewer->fadeBlackFrame(enable);
	}
	else
	{
		// TODO
	}
}

void OutputInstance::fadeClearFrame(bool enable)
{
	if(!m_output->isEnabled())
		return;
		
	Output::OutputType outType = m_output->outputType();
	if(outType == Output::Screen || outType == Output::Custom)
	{
		m_viewer->fadeClearFrame(enable);
	}
	else
	{
		// TODO
	}
}

void OutputInstance::setLiveBackground(const QFileInfo &info, bool waitForNextSlide)
{
	if(!m_output->isEnabled())
		return;
		
	Output::OutputType outType = m_output->outputType();
	if(outType == Output::Screen || outType == Output::Custom)
	{
		m_viewer->setLiveBackground(info,waitForNextSlide);
	}
	else
	{
		// TODO
	}
}
