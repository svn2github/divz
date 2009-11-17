#ifndef BibleGatewayConnector_H
#define BibleGatewayConnector_H

#include "BibleModel.h"
#include "BibleConnector.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>

class BibleGatewayConnector : public BibleConnector
{
	Q_OBJECT
public:
	BibleGatewayConnector(QObject *parent = 0);
	
	QString urlForReference(const BibleVerseRef& reference);
	
protected:
	BibleVerseList parseHtmlReply(QByteArray &);
	
};

#endif
