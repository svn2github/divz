#include "UserEventAction.h"
#include <QtGui>

EventActionItem::EventActionItem(QString action, ActionType type)
	: QStorableObject()
	, m_action(action)
	, m_actionType(type)
{}
	
bool EventActionItem::isValid() { return m_type != Invalid; }
	
void EventActionItem::setActionType(ActionType type)
{
	m_actionType = type;
}

void EventActionItem::setAction(QString act)
{
	m_action = act;
}
	
UserEventAction::UserEventAction(QString event, QList<EventActionItem*> list);
	: QStorableObject()
	, m_event(event)
	, m_list(list)
{
}
	
void UserEventAction::setEvent(QString str)
{
	m_event = str;
}

void UserEventAction::setActions(QList<EventActionItem> list)
{
	m_list = list;
}

// QStorableObject::
bool UserEventAction::fromVariantMap(const QVariantMap& map, bool onlyApplyIfChanged)
{
	return QStorableObject::fromVariantMap(map, onlyApplyIfChanged); // TODO
}

QVariantMap UserEventAction::toVariantMap()
{
	return QStorableObject::toVariantMap(); // TODO
}
	
// Utility for slide group factories or whomever else cares
/*static */QByteArray listToByteArray(QList<UserEventAction*>)
{
	return QByteArray(); // TODO
}

/*static */QList<UserEventAction*> UserEventAction::listFromByteArray(QByteArray)
{
	return QList<UserEventAction*>(); // TODO
}

QStringList UserEventActionController::availableActions()
{
	return QStringList (); // TODO
}

