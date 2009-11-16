#ifndef BibleConnector_H
#define BibleConnector_H

#include "BibleModel.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>

class BibleConnector : public QObject
{
	Q_OBJECT
public:
	BibleConnector(QObject *parent = 0);
	virtual ~BibleConnector();
	
	virtual bool findReference(const QString& reference);
	
	virtual BibleVerseList loadReference(const QString& reference);
	
	virtual void downloadReference(const QString& reference);
	
	virtual QString urlForReference(const QString& reference, const BibleVersion& v= BibleVersion()) = 0;
	
signals:
	void referenceAvailable(const QString& reference, const BibleVerseList & list);

protected slots:
	void downloadFinished(QNetworkReply *reply);
	
protected:
	QString html2text(const QString &tmp);
	
	virtual BibleVerseList parseHtmlReply(QByteArray &) = 0;
	
	virtual BibleVerseList loadCached(const QString& reference);
	virtual void cacheList(const QString& reference, const BibleVerseList &list);
	
	QNetworkAccessManager * m_net;
	QHash<QString, BibleVerseList> m_cache;
	QHash<QNetworkReply*, QString> m_inProgress;
	
};

#endif
