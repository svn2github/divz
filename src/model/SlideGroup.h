#ifndef SLIDEGROUP_H
#define SLIDEGROUP_H

#include "Slide.h"

#include <QList>

class SlideGroup : public QObject
{
	Q_OBJECT
	
	Q_PROPERTY(int groupId READ groupId WRITE setGroupId);
	Q_PROPERTY(int groupNumber READ groupNumber WRITE setGroupNumber);
	Q_PROPERTY(GroupType groupType READ groupType WRITE setGroupType);
	Q_PROPERTY(QString groupTitle READ groupTitle WRITE setGroupTitle);
	Q_PROPERTY(QString iconFile READ iconFile WRITE setIconFile);
	
	
public:
	SlideGroup();
	~SlideGroup();
	
	typedef enum GroupType 
	{
		Generic,
		SectionHeader,
		Song,
	};
	
	ITEM_PROPDEF(GroupId,		int,		groupId);
	ITEM_PROPDEF(GroupNumber,	int,		groupNumber);
	ITEM_PROPDEF(GroupType,		GroupType,	groupType);
	ITEM_PROPDEF(GroupTitle,	QString,	groupTitle);
	ITEM_PROPDEF(IconFile,		QString,	iconFile);

	void addSlide(Slide *);
	QList<Slide *> slideList();
	
	void removeSlide(Slide *);
	
	bool fromXml(QDomElement & parentElement);
        void toXml(QDomElement & parentElement) const;
	
private:
	QList<Slide *> m_slides;
	int m_groupNumber;
	int m_groupId;
	GroupType m_groupType;
	QString m_groupTitle;
	QString m_iconFile;

};

#endif
