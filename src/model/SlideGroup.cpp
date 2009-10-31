#include "SlideGroup.h"

#include <assert.h>
#include <QFileInfo>

#include "Slide.h"
#include "MediaBrowser.h"

SlideGroup::SlideGroup() :
	m_groupNumber(-1)
	, m_groupId(1)
	, m_groupType(Generic)
	, m_groupTitle("")
	, m_iconFile("")
	, m_autoChangeGroup(false)
	, m_inheritFadeSettings(true)
	, m_crossFadeSpeed(250)
	, m_crossFadeQuality(15)
	, m_masterSlide(0)
{

}

SlideGroup::~SlideGroup()
{
	qDeleteAll(m_slides);
	m_slides.clear();
	if(m_masterSlide)
	{
		delete m_masterSlide;
		m_masterSlide = 0;
	}
}

QList<Slide *> SlideGroup::slideList() { return m_slides; }

void SlideGroup::addSlide(Slide *slide)
{
	assert(slide != NULL);
	m_slides.append(slide);
	sortSlides();
	connect(slide,SIGNAL(slideItemChanged(AbstractItem *, QString, QString, QVariant, QVariant)),this,SLOT(slideItemChanged(AbstractItem *, QString, QString, QVariant, QVariant)));

	//qDebug("SlideGroup:: slide ADDED");
	emit slideChanged(slide, "add", 0, "", "", QVariant());
}

void SlideGroup::removeSlide(Slide *slide)
{
	assert(slide != NULL);
	disconnect(slide,0,this,0);
	m_slides.removeAll(slide);
	sortSlides();
	//qDebug("SlideGroup:: slide REMOVED");
	emit slideChanged(slide, "remove", 0, "", "", QVariant());

}

void SlideGroup::slideItemChanged(AbstractItem *item, QString operation, QString fieldName, QVariant value, QVariant /*oldValue*/)
{
	Slide * slide = dynamic_cast<Slide *>(sender());
	if(fieldName == "slideNumber")
		sortSlides();
	//qDebug("SlideGroup:: slide item changed");
	emit slideChanged(slide, "change", item, operation, fieldName, value);
}

void SlideGroup::setGroupNumber(int x)	   { m_groupNumber = x; }
void SlideGroup::setGroupId(int x)	   { m_groupId = x; }
void SlideGroup::setGroupType(GroupType t) { m_groupType = t; }
void SlideGroup::setGroupTitle(QString s)
{
	m_groupTitle = s;
	emit slideChanged(0, "change", 0, "change", "groupTitle", s);
}
void SlideGroup::setIconFile(QString s)    { m_iconFile = s; }
void SlideGroup::setAutoChangeGroup(bool s){ m_autoChangeGroup = s; }
void SlideGroup::setInheritFadeSettings(bool x){ m_inheritFadeSettings = x; }
void SlideGroup::setCrossFadeSpeed(double x){ m_crossFadeSpeed = x; }
void SlideGroup::setCrossFadeQuality(double x){ m_crossFadeQuality = x; }

void SlideGroup::loadGroupAttributes(QDomElement & pe)
{
	setGroupNumber(pe.attribute("number").toInt());
	
	setGroupId(pe.attribute("id").toInt());
	setGroupType((GroupType)pe.attribute("type").toInt());
	setGroupTitle(pe.attribute("title"));
	setIconFile(pe.attribute("icon"));
	setAutoChangeGroup((bool)pe.attribute("auto").toInt());
	
	//qDebug()<<"SlideGroup::fromXml: number:"<<groupNumber()<<", title:"<<groupTitle();
	
	QVariant inherit = pe.attribute("inherit-fade");
	setInheritFadeSettings(inherit.isNull() ? true : (bool)inherit.toInt());
	setCrossFadeSpeed(pe.attribute("fade-speed").toDouble());
	setCrossFadeQuality(pe.attribute("fade-quality").toDouble());
}

void SlideGroup::loadSlideList(QDomElement & pe)
{
	qDeleteAll(m_slides);
	m_slides.clear();

	// for each slide
	for (QDomElement element = pe.firstChildElement(); !element.isNull(); element = element.nextSiblingElement())
	{
		if(element.tagName() == "slide")
		{
			Slide *s = new Slide();
			addSlide(s);

			// restore the item, and delete it if something goes wrong
			if (!s->fromXml(element))
			{
				removeSlide(s);
				delete s;
				continue;
			}
		}
		else
		if(element.tagName() == "master")
		{
			Slide *s = new Slide();
			s->setSlideNumber(-1);
			
			// restore the item, and delete it if something goes wrong
			if (!s->fromXml(element))
			{
				removeSlide(s);
				delete s;
				continue;
			}
			
			m_masterSlide = s;
		}
	}

	sortSlides();
}

void SlideGroup::saveGroupAttributes(QDomElement & pe) const
{
	pe.setAttribute("number",groupNumber());
	pe.setAttribute("id",groupId());
	pe.setAttribute("type",(int)groupType());
	pe.setAttribute("title",groupTitle());
	pe.setAttribute("icon",iconFile());
	pe.setAttribute("auto",(int)autoChangeGroup());
	pe.setAttribute("inherit-fade",(int)m_inheritFadeSettings);
	pe.setAttribute("fade-speed",m_crossFadeSpeed);
	pe.setAttribute("fade-quality",m_crossFadeQuality);
}

void SlideGroup::saveSlideList(QDomElement & pe) const
{
	QDomDocument doc = pe.ownerDocument();

	foreach (Slide * slide, m_slides)
	{
		QDomElement element = doc.createElement("slide");
		pe.appendChild(element);
		slide->toXml(element);
	}
	
	if(m_masterSlide)
	{
		QDomElement element = doc.createElement("master");
		pe.appendChild(element);
		m_masterSlide->toXml(element);
	}
}

bool SlideGroup::fromXml(QDomElement & pe)
{
	loadGroupAttributes(pe);
	loadSlideList(pe);
	return true;
}

void SlideGroup::toXml(QDomElement & pe) const
{
	saveGroupAttributes(pe);
	saveSlideList(pe);
}


bool SlideGroup_slide_num_compare(Slide *a, Slide *b)
{
	return (a && b) ? a->slideNumber() < b->slideNumber() : true;
}

void SlideGroup::sortSlides()
{
	qSort(m_slides.begin(), m_slides.end(), SlideGroup_slide_num_compare);
}

bool SlideGroup::canUseBackground(const QFileInfo & info)
{
	QString ext = info.suffix();
	return MediaBrowser::isVideo(ext) || MediaBrowser::isImage(ext);
}

bool SlideGroup::changeBackground(const QFileInfo & info, Slide *onlyThisSlide)
{
	QString ext = info.suffix();
	
	QString abs = info.absoluteFilePath();
	
	AbstractVisualItem::FillType type = AbstractVisualItem::None;
	if(MediaBrowser::isVideo(ext))
		type = AbstractVisualItem::Video;
	else
	if(MediaBrowser::isImage(ext))
		type = AbstractVisualItem::Image;
	
	if(type!=AbstractVisualItem::None)
	{
		changeBackground(type,abs,onlyThisSlide);
		return true;
	}
	
	return false;
}

void SlideGroup::changeBackground(AbstractVisualItem::FillType fillType, QVariant fillValue, Slide *onlyThisSlide)
{
	QList<Slide *> slides;
	if(onlyThisSlide)
		slides.append(onlyThisSlide);
	else
		slides = slideList();
		
	foreach(Slide * slide, slides)
	{
		AbstractVisualItem * bg = dynamic_cast<AbstractVisualItem*>(slide->background());
		
		bg->setFillType(fillType);
		if(fillType == AbstractVisualItem::Video)
		{
			bg->setFillVideoFile(fillValue.toString());
		}
		else
		if(fillType == AbstractVisualItem::Image)
		{
			bg->setFillImageFile(fillValue.toString());
		}
		else
		if(fillType == AbstractVisualItem::Solid)
		{
			bg->setFillBrush(QColor(fillValue.toString()));
		}
	}
}

Slide * SlideGroup::masterSlide()
{
	if(!m_masterSlide)
	{
		m_masterSlide = new Slide();
		m_masterSlide->setSlideNumber(-1);
	}
	
//	qDebug() << "SlideGroup::masterSlide: accessor for ptr: "<<QString().sprintf("%p",m_masterSlide);
	
	return m_masterSlide;
}
