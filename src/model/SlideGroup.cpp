#include "SlideGroup.h"

#include <assert.h>

SlideGroup::SlideGroup() :
	m_groupNumber(0)
	, m_groupId(1)
	, m_groupType(Generic)
	, m_groupTitle("")
	, m_iconFile("")
	
{
		
}

SlideGroup::~SlideGroup() 
{
	qDeleteAll(m_slides);
}

QList<Slide *> SlideGroup::slideList() { return m_slides; }

void SlideGroup::addSlide(Slide *slide)
{
	assert(slide != NULL);
	m_slides.append(slide);
	sortSlides();
	emit slideChanged(slide, "add", 0, "", "", QVariant());
	connect(slide,SIGNAL(slideItemChanged(AbstractItem *, QString, QString, QVariant)),this,SLOT(slideItemChanged(AbstractItem *, QString, QString, QVariant)));

}

void SlideGroup::removeSlide(Slide *slide)
{
	assert(slide != NULL);
	disconnect(slide,0,this,0);
	m_slides.removeAll(slide);
	sortSlides();
	emit slideChanged(slide, "remove", 0, "", "", QVariant());

}

void SlideGroup::slideItemChanged(AbstractItem *item, QString operation, QString fieldName, QVariant value)
{
	Slide * slide = dynamic_cast<Slide *>(sender());
	sortSlides();
	emit slideChanged(slide, "change", item, operation, fieldName, value);
}

void SlideGroup::setGroupNumber(int x)	   { m_groupNumber = x; }
void SlideGroup::setGroupId(int x)	   { m_groupId = x; } 
void SlideGroup::setGroupType(GroupType t) { m_groupType = t; }
void SlideGroup::setGroupTitle(QString s)  { m_groupTitle = s; }
void SlideGroup::setIconFile(QString s)    { m_iconFile = s; }

bool SlideGroup::fromXml(QDomElement & pe)
{
	qDeleteAll(m_slides);
	m_slides.clear();

	setGroupNumber(pe.attribute("number").toInt());
	setGroupId(pe.attribute("id").toInt());
	setGroupType((GroupType)pe.attribute("type").toInt());
	setGroupTitle(pe.attribute("title"));
	setIconFile(pe.attribute("icon"));
	
	// for each slide
	for (QDomElement element = pe.firstChildElement(); !element.isNull(); element = element.nextSiblingElement()) 
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
	
	return true;
}

void SlideGroup::toXml(QDomElement & pe) const
{
	pe.setAttribute("number",groupNumber());
	pe.setAttribute("id",groupId());
	pe.setAttribute("type",(int)groupType());
	pe.setAttribute("title",groupTitle());
	pe.setAttribute("icon",iconFile());
	
	QDomDocument doc = pe.ownerDocument();
	
	foreach (Slide * slide, m_slides) 
	{
		QDomElement element = doc.createElement("slide");
		pe.appendChild(element);
		slide->toXml(element);	
	}
}


bool SlideGroup_slide_num_compare(Slide *a, Slide *b)
{
	return (a && b) ? a->slideNumber() < b->slideNumber() : true;
}

void SlideGroup::sortSlides()
{
	qSort(m_slides.begin(), m_slides.end(), SlideGroup_slide_num_compare);
}