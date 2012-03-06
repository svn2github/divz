#include "SlideGroup.h"

#include <assert.h>
#include <QFileInfo>
#include <QDomDocument>
#include <QMessageBox>
#include <QMetaProperty>
#include <QSettings>

#include "Slide.h"
#include "MediaBrowser.h"
#include "songdb/SongSlideGroup.h"
#include "ppt/PPTSlideGroup.h"
#include "phonon/VideoSlideGroup.h"
#include "camera/CameraSlideGroup.h"
#include "webgroup/WebSlideGroup.h"
#include "groupplayer/GroupPlayerSlideGroup.h"
#include "model/Output.h"

#define ID_COUNTER_KEY "slidegroup/id-counter"

SlideGroup::SlideGroup() :
	m_groupNumber(-1)
	, m_groupId(1)
	, m_groupType(SlideGroup::GroupType)
	, m_groupTitle("")
	, m_iconFile("")
	, m_endOfGroupAction(SlideGroup::LoopToStart)
	, m_inheritFadeSettings(true)
	, m_crossFadeSpeed(250)
	, m_crossFadeQuality(15)
	, m_masterSlide(0)
	, m_filename("")
{
	QSettings s;
	m_groupId = s.value(ID_COUNTER_KEY,0).toInt() + 1;
	s.setValue(ID_COUNTER_KEY,m_groupId);
//	qDebug() << "SlideGroup: Init groupid: "<<m_groupId;
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

Slide *SlideGroup::slideById(int id)
{
	foreach(Slide *slide, m_slides)
		if(slide->slideId() == id)
			return slide;
	
	return 0;
}

QList<Slide*> SlideGroup::altSlides(Slide *primarySlide)
{
	if(!primarySlide)
		return QList<Slide*>();
	int primaryId = primarySlide->slideId();
	
	QList<Slide*> alts;
	foreach(Slide *slide, m_slides)
		if(slide->primarySlideId() == primaryId)
			alts << slide;
			
	return alts;
	
}

// QList<Slide*> SlideGroup::altSlides(int primarySlideId)
// {
// 	QList<Slide*> alts;
// 	foreach(Slide *slide, m_slides)
// 		if(slide->primarySlideId() == primarySlideId)
// 			alts << slide;
// 	
// 	return id;
// }


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
void SlideGroup::setGroupId(int x)	   { m_groupId = x; }// qDebug() << "SlideGroup::setGroupId:"<<x<<" for "<<assumedName(); }
void SlideGroup::setGroupType(int t)	   { m_groupType = t; }
void SlideGroup::setGroupTitle(QString s)
{
	m_groupTitle = s;
	emit slideChanged(0, "change", 0, "change", "groupTitle", s);
}
void SlideGroup::setIconFile(QString s)    { m_iconFile = s; }
void SlideGroup::setEndOfGroupAction(EndOfGroupAction s)
{
	m_endOfGroupAction = s; 
	emit slideChanged(0, "change", 0, "change", "endOfGroupAction", s);
}
void SlideGroup::setJumpToGroupIndex(int x){ m_jumpToGroupIndex = x; }
void SlideGroup::setInheritFadeSettings(bool x){ m_inheritFadeSettings = x; }
void SlideGroup::setCrossFadeSpeed(double x){ m_crossFadeSpeed = x; }
void SlideGroup::setCrossFadeQuality(double x){ m_crossFadeQuality = x; }

void SlideGroup::loadGroupAttributes(QDomElement & pe)
{
	setGroupNumber(pe.attribute("number").toInt());
	
	setGroupId(pe.attribute("id").toInt());
	setGroupType(pe.attribute("type").toInt());
	setGroupTitle(pe.attribute("title"));
	setIconFile(pe.attribute("icon"));
	setEndOfGroupAction((EndOfGroupAction)pe.attribute("end-action").toInt());
	setJumpToGroupIndex(pe.attribute("jumpto").toInt());
	
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
	pe.setAttribute("end-action",(int)endOfGroupAction());
	pe.setAttribute("jumpto",jumpToGroupIndex());
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

QByteArray SlideGroup::toByteArray() const
{
	QByteArray array;
	QDataStream stream(&array, QIODevice::WriteOnly);
	QVariantMap map;
	
	toVariantMap(map);
	
	map["SlideGroup.ClassName"] = metaObject()->className();
	
	//qDebug() << "SlideGroup::toByteArray(): "<<map;
	stream << map;
	return array; 
}

void SlideGroup::toVariantMap(QVariantMap& map) const
{
	saveProperties(map);
	saveSlideList(map);
}

void SlideGroup::saveProperties(QVariantMap& map) const
{
	// So we dont have to engineer our own method of tracking
	// properties, just assume all inherited objects delcare the relevant
	// properties using Q_PROPERTY macro
	const QMetaObject *metaobject = metaObject();
	int count = metaobject->propertyCount();
	for (int i=0; i<count; ++i)
	{
		QMetaProperty metaproperty = metaobject->property(i);
		const char *name = metaproperty.name();
		QVariant value = property(name);
		//qDebug() << "AbstractItem::clone():"<<itemName()<<": prop:"<<name<<", value:"<<value;
		//item->setProperty(name,value);
		map[name] = value;
	}
	
	
	if(m_masterSlide)
		map["master"] = m_masterSlide->toByteArray();
		
	QVariantMap alts;
	foreach(int outId, m_altGroupForOutput.keys())
	{
		QByteArray ba = m_altGroupForOutput.value(outId)->toByteArray();
		alts[QString("%1").arg(outId)] = ba;
	}
	map["alt"] = alts;
	
// 	if(!alts.isEmpty())
// 		qDebug() << "SlideGroup::saveProperties: "<<m_groupId<<" alts:"<<alts;
// 	else
// 		qDebug() << "SlideGroup::saveProperties: "<<m_groupId<<" No valid alts stored";
}

void SlideGroup::saveSlideList(QVariantMap& map)  const
{
	QVariantList list;
	if(m_slides.isEmpty())
		return;
	foreach (Slide * slide, m_slides)
		if(slide)
			list << slide->toByteArray();
	map["slides"] = list;

}

SlideGroup * SlideGroup::clone()
{
	QByteArray ba = toByteArray();
	SlideGroup * clone = fromByteArray(ba);
	clone->setGroupNumber(-1);
	return clone;
}

/* static */
SlideGroup * SlideGroup::fromByteArray(QByteArray &array, Document *context)
{
	QDataStream stream(&array, QIODevice::ReadOnly);
	QVariantMap map;
	stream >> map;
	
	//qDebug() << "SlideGroup::fromByteArray(): "<<map;
	if(map.isEmpty())
	{
		qDebug() << "Error: SlideGroup::fromByteArray(): Map is empty, not loading group.";
		return 0;
	}
	
	SlideGroup * group = 0;
	
	QString className = map["SlideGroup.ClassName"].toString();
	
	if (className == "SongSlideGroup")
	{
		//qDebug("cmdSetSlideGroup: Group type: Song");
		group = new SongSlideGroup();
	}
	else
	if(className == "PPTSlideGroup")
	{
		//qDebug("cmdSetSlideGroup: Group type: PPT");
		group = new PPTSlideGroup();
	}
	else
	if(className == "VideoSlideGroup")
	{
		//qDebug("cmdSetSlideGroup: Group type: Video");
		group = new VideoSlideGroup();
	}
	else
	if(className == "CameraSlideGroup")
	{
		//qDebug("cmdSetSlideGroup: Group type: Camera");
		group = new CameraSlideGroup();
	}
	else
	if(className == "GroupPlayerSlideGroup")
	{
		group = new GroupPlayerSlideGroup();
	}
	else
	if(className == "WebSlideGroup")
	{
		group = new WebSlideGroup();
	}
	else
	if(className == "SlideGroup")
	{
		//qDebug("cmdSetSlideGroup: Group type: Generic");
		group = new SlideGroup();
	}
	else
	{
		qWarning("SlideGroup::fromByteArray: Unknown class name '%s'", qPrintable(className));
		return 0;
	}
	
	group->setDocument(context);
	
	group->fromVariantMap(map);
	
	return group;
	
}

void SlideGroup::fromVariantMap(QVariantMap &map)
{
	loadProperties(map);
	loadSlides(map);	
}
	
void SlideGroup::loadProperties(QVariantMap &map)
{
	// So we dont have to engineer our own method of tracking
	// properties, just assume all inherited objects delcare the relevant
	// properties using Q_PROPERTY macro
	const QMetaObject *metaobject = metaObject();
	int count = metaobject->propertyCount();
	for (int i=0; i<count; ++i)
	{
		QMetaProperty metaproperty = metaobject->property(i);
		const char *name = metaproperty.name();
		QVariant value = map[name];
		//qDebug() << "AbstractItem::clone():"<<itemName()<<": prop:"<<name<<", value:"<<value;
		if(value.isValid())
			setProperty(name,value);
		else
			qDebug() << "SlideGroup::loadByteArray: Unable to load property for "<<name<<", got invalid property from map";
	}

	if(groupId() == 1)
	{
		// Assume invalid - groupIds should be higher than 1
		QSettings s;
		m_groupId = s.value(ID_COUNTER_KEY,0).toInt() + 1;
		s.setValue(ID_COUNTER_KEY,m_groupId);
		//qDebug() << "SlideGroup::loadProperties(): Re-init groupid: "<<m_groupId;
		//setGroupId(m_groupId);
	}
	
	QVariant master = map["master"];
	if(master.isValid())
	{
		m_masterSlide = new Slide();
		QByteArray ba = master.toByteArray();
		m_masterSlide->fromByteArray(ba);
	}
	
	QVariant alt = map["alt"];
	if(alt.isValid())
	{
		QVariantMap map = alt.toMap();
		foreach(QString outputIdString, map.keys())
		{
			int id = outputIdString.toInt();
			
			QVariant groupData = map.value(outputIdString);
			QByteArray ba = groupData.toByteArray();
			
			SlideGroup *group  = SlideGroup::fromByteArray(ba);
			
			//qDebug() << "SlideGroup: "<<m_groupId<<" Loading alt id:"<<outputIdString<<", group:"<<group;
			
			m_altGroupForOutput[id] = group;
		}
	}
	else
	{
		//qDebug() << "SlideGroup: "<<m_groupId<<" No valid alt found";
	}
}
	
void SlideGroup::loadSlides(QVariantMap &map)
{
	qDeleteAll(m_slides);
	m_slides.clear();

	QVariantList items = map["slides"].toList();
	foreach(QVariant var, items)
	{
		Slide * slide = new Slide();
		QByteArray ba = var.toByteArray();
		slide->fromByteArray(ba);
		addSlide(slide);
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

Slide * SlideGroup::masterSlide(bool autoCreate)
{
	if(!m_masterSlide && autoCreate)
	{
		m_masterSlide = new Slide();
		m_masterSlide->setSlideNumber(-1);
	}
	
//	qDebug() << "SlideGroup::masterSlide: accessor for ptr: "<<QString().sprintf("%p",m_masterSlide);
	
	return m_masterSlide;
}


void SlideGroup::load(const QString & s)
{
	m_filename = s;
	
	// Load the file
	QFile file(m_filename);
	if (!file.open(QIODevice::ReadOnly)) 
	{
		QMessageBox::critical(0, tr("Loading error"), tr("Unable to load file %1").arg(m_filename));
		throw(0);
		return;
	}
	
	// And create the XML document into memory (with nodes...)
	QString *error = new QString();
	QDomDocument doc;
	if (!doc.setContent(&file, false, error)) 
	{
		QMessageBox::critical(0, tr("Parsing error"), tr("Unable to parse file %1. The error was: %2").arg(m_filename, *error));
		file.close();
		throw(0);
		return;
	}
	file.close();
	
	delete error;
	error = 0;
	
	QDomElement root = doc.documentElement(); // The root node
	
	fromXml(root);
}


void SlideGroup::save(const QString & filename)
{
	QString tmp = filename;
	if(tmp.isEmpty())
		tmp = m_filename;
	else
		m_filename = tmp;
		
	QDomDocument doc;
	QFile file;
	QTextStream out;
	
	// Open file
	file.setFileName(tmp);
	if (!file.open(QIODevice::WriteOnly)) 
	{
		QMessageBox::warning(0, tr("File Error"), tr("Error saving to the file '%1'").arg(tmp));
		throw 0;
		return;
	}
	
	qDebug() << "SlideGroup::save: Writing to "<<tmp;
	
	out.setDevice(&file);
	
	// This element contains all the others.
	QDomElement rootElement = doc.createElement("group");

	toXml(rootElement);
	
	// Add the root (and all the sub-nodes) to the document
	doc.appendChild(rootElement);
	
	//Add at the begining : <?xml version="1.0" ?>
	QDomNode noeud = doc.createProcessingInstruction("xml","version=\"1.0\" ");
	doc.insertBefore(noeud,doc.firstChild());
	//save in the file (4 spaces indent)
	doc.save(out, 4);
	file.close();
	
	qDebug() << "SlideGroup::save: Done writing "<<tmp;
}

QString SlideGroup::assumedName()
{
	return groupTitle().isEmpty() ? QString(tr("Group %1")).arg(groupNumber()) : groupTitle();
}

void SlideGroup::setDocument(Document *doc)
{
	m_doc = doc;
}

SlideGroup *SlideGroup::altGroupForOutput(Output *output)
{
	if(m_altGroupForOutput.contains(output->id()))
		return m_altGroupForOutput.value(output->id());
	return 0;
}

void SlideGroup::setAltGroupForOutput(Output *output, SlideGroup *group)
{
	m_altGroupForOutput[output->id()] = group;
}

