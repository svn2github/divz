#include "UserEventAction.h"
#include <QtGui>

// Utility for slide group factories or whomever else cares
/*static */QVariantMap UserEventActionUtilities::toVariantMap(QStringListHash hash)
{
	QVariantMap map;
	foreach(QString event, hash.keys())
	{
		QVariantList list;
		foreach(QString string, hash[event])
			list << string;
		map[event] = QVariant(list);
	}
	return map;
}

/*static */QStringListHash UserEventActionUtilities::fromVariantMap(QVariantMap map)
{
	QStringListHash output;
	foreach(QString event, map.keys())
	{
		QVariantList list = map[event].toList();
		
		QStringList stringList;
		foreach(QVariant var, list)
			stringList << var.toString();
		output[event] = stringList;
	}
	
	return output;
}

/*static */QStringList UserEventActionUtilities::availableEvents()
{
	return QStringList ()
		<< UserEventAction_GroupGoLive
		<< UserEventAction_GroupNotLive
		<< UserEventAction_SlideGoLive
		<< UserEventAction_SlideNotLive;
}

