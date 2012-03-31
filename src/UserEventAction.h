#ifndef UserEventActions_H
#define UserEventActions_H

#include <QtGui>

typedef QHash<QString,QStringList> QStringListHash;

class UserEventActionUtilities
{
//	Q_OBJECT
public:
	#define UserEventAction_GroupGoLive	"Group Go-Live"
	#define UserEventAction_GroupNotLive	"Group Not-Live"
	#define UserEventAction_SlideGoLive	"Slide Go-Live"
	#define UserEventAction_SlideNotLive	"Slide Not-Live"
	
	static QStringList availableEvents();

	// Utility for slide group factories or whomever else cares
	static QVariantMap toVariantMap(QStringListHash);
	static QStringListHash fromVariantMap(QVariantMap);

};

#endif
