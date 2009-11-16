#ifndef GroupPlayerSlideGroup_H
#define GroupPlayerSlideGroup_H

class Document;
#include "model/SlideGroup.h"

/// \brief: GroupPlayerSlideGroup sequences slide groups either from the current document or external documents.
/// GroupPlayerSlideGroup allows you to specify the sequence of slide groups to play. It's a container
/// for other groups - it doesn't actually contain slides of its own.
class GroupPlayerSlideGroup : public SlideGroup
{
private:
	Q_OBJECT 
// 	Q_PROPERTY(QString numGroups READ numGroups WRITE setText);
// 	Q_PROPERTY(QString isTextDiffFromDb READ isTextDiffFromDb);

public:
	GroupPlayerSlideGroup();
	~GroupPlayerSlideGroup();
	
	typedef enum { GroupType = 4 };
	int groupType() const { return GroupType; }
	
	// SlideGroup::
	virtual bool fromXml(QDomElement & parentElement);
        virtual void toXml(QDomElement & parentElement) const;
	
	//void changeBackground(AbstractVisualItem::FillType fillType, QVariant fillValue, Slide *onlyThisSlide);
	
	// GroupPlayerSlideGroup::
	typedef enum GroupSource
	{
		SameDocument,
		ExternalDocument,
	};
	
	typedef struct GroupMember
	{
		int 		groupId;	// the id (stored for reference to the group when loading from disk)
		GroupSource 	source;		// where to look for groupId
		QString 	externalDoc;	// filename of the external source (if source == ExternalDocument)
		SlideGroup 	*group;		// the pointer to the group (will be set when loaded from byte array or xml)
		int 		sequenceNumber; // the sequence in which it should play
	};
	
	GroupMember addGroup(SlideGroup *, const QString& document="");
	GroupMember addGroup(SlideGroup *, Document *);
	void addGroup(GroupMember);
	void removeGroup(SlideGroup *);
	void removeGroup(GroupMember);
	
	QList<SlideGroup *> groupList();
	QList<GroupMember> members() { return m_groups; }
	void setMembers(QList<GroupMember>);
	
	int numGroups() { return m_groups.size(); }
	SlideGroup * at(int idx) { return m_groups.isEmpty() ? 0 : m_groups.at(idx).group; }
	GroupMember memberInfo(int idx) { return m_groups.isEmpty() ? GroupMember() : m_groups.at(idx); }
	
	void resequence(GroupMember mem, int op);
	void resequence(int index, int op);
	
/*	
signals:
	void slideGroupChanged(SlideGroup *g, QString groupOperation, Slide *slide, QString slideOperation, AbstractItem *item, QString operation, QString fieldName, QVariant value);

private slots:
	void slideChanged(Slide *slide, QString slideOperation, AbstractItem *item, QString operation, QString fieldName, QVariant value);
*/

protected:
	void removeAllSlides();
	void removeAllMembers();
	
	void fromVariantMap(QVariantMap &);
	void   toVariantMap(QVariantMap &) const;

protected slots:
//	void aspectRatioChanged(double x);

protected:
	void addGroupSlides(SlideGroup*);
	void removeGroupSlides(int);
	
	void loadGroupMember(GroupMember);
	
	QVariantMap memberToVariantMap(GroupMember) const;
	GroupMember memberFromVariantMap(QVariantMap);
	
	QVariantList memberListToVariantList() const;
	void memberListFromVariantList(QVariantList);

	
	QList<GroupMember> m_groups;

};

Q_DECLARE_METATYPE(GroupPlayerSlideGroup*);
Q_DECLARE_METATYPE(GroupPlayerSlideGroup::GroupMember);
bool operator==(const GroupPlayerSlideGroup::GroupMember& a, const GroupPlayerSlideGroup::GroupMember& b);
bool  operator<(const GroupPlayerSlideGroup::GroupMember& a, const GroupPlayerSlideGroup::GroupMember& b);

#endif
