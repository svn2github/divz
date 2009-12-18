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

	// This m_rescanTimer and related slot really is a bit of a "hack". You see, if you
	// create the group player BEFORE you create the groups that you add to the group player, then when the document
	// is loaded again from disk the next time, the member groups won't be loaded from disk till AFTER the group
	// player. Therefore, they won't exist in the document at the time we try to find them. So, we use this timer to
	// re-check for the member groups, assuming that in 1ms (when the event loop resumes) the member groups will have
	// been loaded. I know it seems a bit hackish - but it seems "cleaner" than changing the SlideGroup API right now.
	connect(&m_rescanTimer, SIGNAL(timeout()), this, SLOT(rescanGroupIds()));
	m_rescanTimer.setSingleShot(true);
	m_rescanTimer.setInterval(1);
}

GroupPlayerSlideGroup::~GroupPlayerSlideGroup()
{
	foreach(GroupMember mem, m_groups)
	{
		if(mem.source == GroupPlayerSlideGroup::ExternalDocument)
		{
			delete (SlideGroup*)mem.group;
			mem.group = 0;
		}
	}
}

void GroupPlayerSlideGroup::rescanGroupIds()
{
	QList<GroupMember> newList;
	bool changed = false;

	foreach(GroupMember mem, m_groups)
	{
		if(mem.group && mem.group->groupId() != mem.groupId)
		{
			qDebug() << "GroupPlayerSlideGroup::rescanGroupIds(): Found mismatch in expected groupids:"<<mem.group->groupId()<<"!="<<mem.groupId;
			SlideGroup *testGroup =  loadGroupMember(mem);
			if(testGroup)
			{
				qDebug() << "GroupPlayerSlideGroup::rescanGroupIds(): Resolved mismatch, loaded group:"<<testGroup->assumedName();
				mem.group = testGroup;
				changed = true;
			}
			else
			{
				qDebug() << "GroupPlayerSlideGroup::rescanGroupIds(): Unable to find correct group.";
			}
		}

		newList.append(mem);
	}

	//setupSlides();
	if(changed)
		setMembers(newList);

}

GroupPlayerSlideGroup::GroupMember GroupPlayerSlideGroup::addGroup(SlideGroup *group, const QString& document)
{
	GroupMember mem;
	mem.groupId 		= group->groupId();
	mem.group 		= document.isEmpty() ? group : group->clone();
	mem.source 		= document.isEmpty() ? 
				GroupPlayerSlideGroup::SameDocument : 
				GroupPlayerSlideGroup::ExternalDocument;
	mem.externalDoc 	= document;
	mem.sequenceNumber 	= m_groups.size();
	m_groups.append(mem);
	
	//addGroupSlides(group);
	setupSlides();
	
	return mem;
}

GroupPlayerSlideGroup::GroupMember GroupPlayerSlideGroup::addGroup(SlideGroup *group, Document *doc)
{
	return addGroup(group,doc ? doc->filename() : "");
}

void GroupPlayerSlideGroup::addGroup(GroupMember mem)
{
	if(!mem.group)
		mem.group = loadGroupMember(mem);
		
	if(!mem.group)
	{
		// hackish method to put *something* as the group - right now, if we leave group null, then we segflt - i dont have time to trace the crash, but I can patch around it here.
		Document *doc = MainWindow::mw()->currentDocument();
		if(!doc) // loading, therefore SlideGroup::document() should contain current doc
			doc = document();
		if(doc && !doc->groupList().isEmpty())
			mem.group = doc->groupList().first();
		//return;
	}

	if(!mem.group)
	{
		qDebug() << "GroupPlayerSlideGroup::addGroup: No 'group' defined in GroupMember # "<<mem.sequenceNumber<<", not adding member to player list - could not patch with hack above.";
		return;
	}
	
	m_groups.append(mem);
	//addGroupSlides(mem.group);
	setupSlides();
	//qDebug() << "GroupPlayerSlideGroup::addGroup: Adding GroupMember seq# "<<mem.sequenceNumber<<", group name: "<<mem.group->assumedName()<<", new size: "<<m_groups.size()<<", slides:"<<numSlides();

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
	//removeGroupSlides(mem.groupId);
	m_groups.removeAll(mem);
	setupSlides();
}

void GroupPlayerSlideGroup::removeGroup(SlideGroup *group)
{
	QList<GroupMember> toBeRemoved;
	foreach(GroupMember mem, m_groups)
		if(((SlideGroup*)mem.group) == group)
			toBeRemoved.append(mem);
			
	foreach(GroupMember mem, toBeRemoved)
	{
		//removeGroupSlides(mem.groupId);
		m_groups.removeAll(mem);
	}
	setupSlides();
}

void GroupPlayerSlideGroup::setupSlides()
{
	if(m_groups.isEmpty())
	{
		while(!m_slides.isEmpty())
			removeSlide(m_slides.takeFirst());
		return;
	}
	
	SlideGroup *group = m_groups.first().group;
	if(!group)
		return;
	
	QList<Slide*> slides = group->slideList();
	if(slides.isEmpty())
		return;
	if(m_slides.isEmpty() || 
		m_slides.first() != slides.first())
	{
		if(!m_slides.isEmpty())
			removeSlide(m_slides.first());
		addSlide(slides.first());
	}
	
// 	int groupId = group->groupId();
// 	foreach(Slide *slide, slides)
// 	{
// 		slide->setProperty("GroupPlayerSlideGroup.groupId", groupId);
// 		addSlide(slide);
// 	}
}

// void GroupPlayerSlideGroup::removeGroupSlides(int groupId)
// {
// 	foreach(Slide *slide, m_slides)
// 	{
// 		QVariant id = slide->property("GroupPlayerSlideGroup.groupId");
// 		if(id.toInt() == groupId)
// 		{
// 			disconnect(slide,0,this,0);
// 			m_slides.removeAll(slide);
// 			emit slideChanged(slide, "remove", 0, "", "", QVariant());
// 		}
// 	}
// }

QList<SlideGroup *> GroupPlayerSlideGroup::groupList()
{
	QList<SlideGroup *> list;
	foreach(GroupMember mem, m_groups)
		list.append((SlideGroup*)mem.group);
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

	m_rescanTimer.start();
	
	return true;
}

void GroupPlayerSlideGroup::fromVariantMap(QVariantMap &map)
{
	loadProperties(map);
	QVariant var = map["members"];
	//qDebug() << "GroupPlayerSlideGroup::fromVariantMap()";
	memberListFromVariantList(var.toList());
	m_rescanTimer.start();
}
	
void GroupPlayerSlideGroup::toVariantMap(QVariantMap &map) const
{
	saveProperties(map);
	
	QVariantList list = memberListToVariantList();
	//qDebug() << "GroupPlayerSlideGroup::toVariantMap(): list.size: "<<list.size();
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
	
	//qDebug() << "GroupPlayerSlideGroup::memberToVariantMap: "<<map;
	//qDebug() << "GroupPlayerSlideGroup::memberToVariantMap: saved groupId:"<<map["groupId"]<<" at sequence#"<<map["seq"];
	/*
	if(mem.group)
		qDebug() << "GroupPlayerSlideGroup::memberToVariantMap: actual groupId is:"<<mem.group->groupId();
	else
		qDebug() << "GroupPlayerSlideGroup::memberToVariantMap: no group ptr on this member, so I have no idea what the real groupid is";
	*/
	
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
		
	//qDebug() << "GroupPlayerSlideGroup::memberListToVariantList: m_groups.size:"<<m_groups.size()<<", list.size:"<<list.size();
		
	return list;
}

SlideGroup * GroupPlayerSlideGroup::loadGroupMember(GroupMember mem)
{
	SlideGroup * group = 0;
	if(mem.source == GroupPlayerSlideGroup::SameDocument)
	{
		Document *doc = MainWindow::mw()->currentDocument();
		if(!doc) // loading, therefore SlideGroup::document() should contain current doc
			doc = document();
		if(!doc)
		{
			qDebug() << "GroupPlayerSlideGroup::loadGroupMember(): Unable to determine current document - cannot load SameDocument group#"<<mem.groupId;
			return 0;
		}
		
		//qDebug() << "GroupPlayerSlideGroup::loadGroupMember(): Attempting to find group ID "<<mem.groupId<<"...";
		group = doc->groupById(mem.groupId);
		if(!group)
		{
			qDebug() << "GroupPlayerSlideGroup::loadGroupMember(): Unable to find group#"<<mem.groupId<<"in this document!";
		}
		//qDebug() << "GroupPlayerSlideGroup::loadGroupMember(): Received group with ID:"<<group->groupId();
	}
	else
	if(mem.source == GroupPlayerSlideGroup::ExternalDocument)
	{
		Document * doc = new Document(mem.externalDoc);
		group = doc->groupById(mem.groupId)->clone();
		if(!group)
		{
			qDebug() << "GroupPlayerSlideGroup::loadGroupMember(): Unable to find group#"<<mem.groupId<<"in external document"<<mem.externalDoc;
		}
	}
	
	return group;
}

void GroupPlayerSlideGroup::memberListFromVariantList(QVariantList varlist)
{
	QList<GroupMember> tmpList;
	foreach(QVariant var, varlist)
		tmpList << memberFromVariantMap(var.toMap());
	
	qSort(tmpList);
	
	//qDebug() << "GroupPlayerSlideGroup::memberListFromVariantList(): tmpList.size:"<<tmpList.size()<<", varlist.size:"<<varlist.size();
	
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
