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
	
	virtual bool findReference(const BibleVerseRef& reference);
	
	virtual BibleVerseList loadReference(const BibleVerseRef& reference);
	
	virtual void downloadReference(const BibleVerseRef& reference);
	
	virtual QString urlForReference(const BibleVerseRef& reference) = 0;
	
signals:
	void referenceAvailable(const BibleVerseRef& reference, const BibleVerseList & list);

protected slots:
	void downloadFinished(QNetworkReply *reply);
	
protected:
	QString html2text(const QString &tmp);
	
	virtual BibleVerseList parseHtmlReply(QByteArray &) = 0;
	
	virtual BibleVerseList loadCached(const BibleVerseRef&);
	virtual void cacheList(const BibleVerseList &);
	virtual bool isCached(const BibleVerseRef&);
	
	QNetworkAccessManager * m_net;
	QHash<QString, BibleVerse> m_cache; // cache per-verse (per version)
	QHash<QNetworkReply*, BibleVerseRef> m_inProgress;
	
};

#endif
