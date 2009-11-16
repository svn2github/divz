#include "GroupPlayerSlideGroup.h"

#include "model/ItemFactory.h"
#include "model/Slide.h"
#include "model/SlideGroup.h"
#include "model/Document.h"
#include "MainWindow.h"

#include "3rdparty/qjson/parser.h"
#include "3rdparty/qjson/serializer.h"

#include <QBuffer>

#define DEBUG_TEXTOSLIDES 0

GroupPlayerSlideGroup::GroupPlayerSlideGroup() : SlideGroup()
{
// 	if(MainWindow::mw())
// 		connect(MainWindow::mw(), SIGNAL(aspectRatioChanged(double)), this, SLOT(aspectRatioChanged(double)));
}

GroupPlayerSlideGroup::~GroupPlayerSlideGroup()
{
	foreach(GroupMember mem, m_groups)
		if(mem.source == GroupPlayerSlideGroup::ExternalDocument)
		{
			delete mem.group;
			mem.group = 0;
		}
}

GroupPlayerSlideGroup::GroupMember GroupPlayerSlideGroup::addGroup(SlideGroup *group, const QString& document)
{
	GroupMember mem;
	mem.groupId = group->groupId();
	mem.source = document.isEmpty() ? GroupPlayerSlideGroup::SameDocument : GroupPlayerSlideGroup::ExternalDocument;
	mem.externalDoc = document;
	mem.group = mem.source == GroupPlayerSlideGroup::ExternalDocument ? group->clone() : group;
	mem.sequenceNumber = m_groups.size();
	m_groups.append(mem);
	
	addGroupSlides(group);
	
	return mem;
}

GroupPlayerSlideGroup::GroupMember GroupPlayerSlideGroup::addGroup(SlideGroup *group, Document *doc)
{
	return addGroup(group,doc ? doc->filename() : "");
}

void GroupPlayerSlideGroup::addGroup(GroupMember mem)
{
	m_groups.append(mem);
	addGroupSlides(mem.group);
}

bool operator==(const GroupPlayerSlideGroup::GroupMember& a, const GroupPlayerSlideGroup::GroupMember& b)
{
	return a.sequenceNumber == b.sequenceNumber;
}

bool  operator<(const GroupPlayerSlideGroup::GroupMember& a, const GroupPlayerSlideGroup::GroupMember& b)
{
	return a.sequenceNumber < b.sequenceNumber;
}

void GroupPlayerSlideGroup::removeGroup(GroupMember mem)
{
	removeGroupSlides(mem.groupId);
	m_groups.removeAll(mem);
}

void GroupPlayerSlideGroup::removeGroup(SlideGroup *group)
{
	QList<GroupMember> toBeRemoved;
	foreach(GroupMember mem, m_groups)
		if(mem.group == group)
			toBeRemoved.append(mem);
			
	foreach(GroupMember mem, toBeRemoved)
	{
		removeGroupSlides(mem.groupId);
		m_groups.removeAll(mem);
	}
}

void GroupPlayerSlideGroup::addGroupSlides(SlideGroup *group)
{
	int groupId = group->groupId();
	QList<Slide*> slides = group->slideList();
	foreach(Slide *slide, slides)
	{
		slide->setProperty("GroupPlayerSlideGroup.groupId", groupId);
		addSlide(slide);
	}
}

void GroupPlayerSlideGroup::removeGroupSlides(int groupId)
{
	foreach(Slide *slide, m_slides)
	{
		QVariant id = slide->property("GroupPlayerSlideGroup.groupId");
		if(id.toInt() == groupId)
		{
			disconnect(slide,0,this,0);
			m_slides.removeAll(slide);
			emit slideChanged(slide, "remove", 0, "", "", QVariant());
		}
	}
}

QList<SlideGroup *> GroupPlayerSlideGroup::groupList()
{
	QList<SlideGroup *> list;
	foreach(GroupMember mem, m_groups)
		list.append(mem.group);
	return list;
}


/* protected */
void GroupPlayerSlideGroup::removeAllSlides()
{
	foreach(Slide *slide, m_slides)
	{
		disconnect(slide,0,this,0);
		//m_slides.removeAll(slide);
		emit slideChanged(slide, "remove", 0, "", "", QVariant());
	}

	//qDeleteAll(m_slides);
	m_slides.clear();
}

void GroupPlayerSlideGroup::removeAllMembers()
{
	removeAllSlides();
	m_groups.clear();
}

bool GroupPlayerSlideGroup::fromXml(QDomElement & pe)
{
	QString memberJson = pe.attribute("members");
	QJson::Parser parser;
	
	bool ok;
	
	QBuffer buffer;
	buffer.open(QBuffer::ReadWrite);
	buffer.write(qPrintable(memberJson));
	buffer.seek(0);
	
	QVariant result = parser.parse(&buffer, &ok);
	if(!ok)
	{
		qDebug() << QString("[ERROR] Error in data at %1: %2\nData: %3").arg(parser.errorLine()).arg(parser.errorString()).arg(memberJson);
		return false;
	}
	
	memberListFromVariantList(result.toList());

	loadGroupAttributes(pe);
	
	return true;
}

void GroupPlayerSlideGroup::fromVariantMap(QVariantMap &map)
{
	loadProperties(map);
	memberListFromVariantList(map["members"].toList());
}
	
void GroupPlayerSlideGroup::toVariantMap(QVariantMap &map) const
{
	saveProperties(map);
	
	QVariantList list = memberListToVariantList();
	map["members"] = list;
}

void GroupPlayerSlideGroup::toXml(QDomElement & pe) const
{
	pe.setTagName("groupplayer");

	QVariantList list = memberListToVariantList();
	
	QJson::Serializer stringy;
	QByteArray json = stringy.serialize(list);
	pe.setAttribute("members",QString(json));
}


QVariantMap GroupPlayerSlideGroup::memberToVariantMap(GroupMember mem) const
{
	QVariantMap map;
	map["groupId"] = mem.groupId;
	map["source"]  = (int)mem.source;
	map["doc"]     = mem.externalDoc;
	map["seq"]     = mem.sequenceNumber;
	
	return map;
}

GroupPlayerSlideGroup::GroupMember GroupPlayerSlideGroup::memberFromVariantMap(QVariantMap map)
{
	GroupMember mem;
	mem.groupId		= map["groupId"].toInt();
	mem.source		= (GroupSource)map["source"].toInt();
	mem.externalDoc		= map["doc"].toString();
	mem.sequenceNumber	= map["seq"].toInt();
	
	return mem;
}

QVariantList GroupPlayerSlideGroup::memberListToVariantList() const
{
	QVariantList list;
	foreach(GroupMember mem, m_groups)
		list.append(memberToVariantMap(mem));
		
	return list;
}

void GroupPlayerSlideGroup::memberListFromVariantList(QVariantList varlist)
{
	QList<GroupMember> tmpList;
	foreach(QVariant var, varlist)
		tmpList << memberFromVariantMap(var.toMap());
	
	qSort(tmpList);
	
	foreach(GroupMember mem, tmpList)
		addGroup(mem);
}

void GroupPlayerSlideGroup::setMembers(QList<GroupMember> list)
{
	removeAllMembers();
	
	qSort(list);
	
	foreach(GroupMember mem, list)
		addGroup(mem);
	
}

void GroupPlayerSlideGroup::resequence(int index, int op)
{
	if(index < 0 || index >= m_groups.size())
		return;
	
	resequence(m_groups[index],op);
}

void GroupPlayerSlideGroup::resequence(GroupMember mem, int op)
{
	int size = m_groups.size();
	int index = m_groups.indexOf(mem);
	
	// find out insertion indexes over the stacked items
	int prevIndex = 0;
	int nextIndex = size - 1;
	foreach (GroupMember item, m_groups) 
	{
		// operate only on different members
		if (item == mem)
			continue;
	
		// refine previous/next indexes (close to 'index')
		int cIdx = m_groups.indexOf(item);
		if (cIdx < nextIndex && cIdx > index)
			nextIndex = cIdx;
		else if (cIdx > prevIndex && cIdx < index)
			prevIndex = cIdx;
	}
	
	// move items
	switch (op) 
	{
		case 1: // front
			m_groups.append(m_groups.takeAt(index));
			break;
		case 2: // raise
			if (index >= size - 1)
				return;
			m_groups.insert(nextIndex, m_groups.takeAt(index));
			break;
		case 3: // lower
			if (index <= 0)
				return;
			m_groups.insert(prevIndex, m_groups.takeAt(index));
			break;
		case 4: // back
			m_groups.prepend(m_groups.takeAt(index));
			break;
	}
	
	// reassign sequence numbers
	int z = 1;
	foreach (GroupMember mem, m_groups)
		mem.sequenceNumber = z++;
	
}

// void GroupPlayerSlideGroup::changeBackground(AbstractVisualItem::FillType fillType, QVariant fillValue, Slide *onlyThisSlide)
// {
// 	SlideGroup::changeBackground(fillType,fillValue,onlyThisSlide);
// 	
// }
