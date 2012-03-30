#ifndef UserEventActions_H
#define UserEventActions_H

#include "QStorableObject.h"
#include <QtGui>

class EventActionItem : public QStorableObject
{
	Q_OBJECT
	Q_PROPERTY(ActionType actionType READ actionType WRITE setActionType);
	Q_PROPERTY(QString action READ action WRITE setAction); 
	
public:
	enum ActionType { Invalid=-1, URL=0, Exec };
	EventActionItem(QString action="", ActionType type=Invalid);
	
	bool isValid();
	
	ActionType actionType() { return m_actionType; }
	QString action() { return m_action; }
	
	void setActionType(ActionType type);
	void setAction(QString name);
	
private:
	QString m_action;
	ActionType m_actionType;
};
Q_DECLARE_METATYPE(EventActionItem::ActionType)

class UserEventAction : public QStorableObject
{
	Q_OBJECT
	Q_PROPERTY(QString event READ event WRITE setEvent);
	
public:
	UserEventAction(QString event="", QList<EventActionItem*> list=QList<EventActionItem*>());
	
	QString event() { return m_event; }
	QList<EventActionItem*> actions() { return m_list; }
	
	void setEvent(QString);
	void setActions(QList<EventActionItem*>);

	// QStorableObject::
	virtual bool fromVariantMap(const QVariantMap&, bool onlyApplyIfChanged = false);
	virtual QVariantMap toVariantMap();
	
	// Utility for slide group factories or whomever else cares
	static QByteArray listToByteArray(QList<UserEventAction*>);
	static QList<UserEventAction*> listFromByteArray(QByteArray);

private:
	QString m_event;
	QList<EventActionItem*> m_list;
};

typedef QList<UserEventAction*> UserEventActionList;

class UserEventActionController
{
//	Q_OBJECT
public:
	static QStringList availableActions();


};

#endif
