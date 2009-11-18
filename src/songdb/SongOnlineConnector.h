#ifndef SongOnlineConnector_H
#define SongOnlineConnector_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>

class SongOnlineConnector

class SongOnlineSearchResult
{
public:
	SongOnlineSearchResult(const QString& name, const QString& text, SongOnlineConnector* con=0) :
		title(name), lyrics(text), source(con) {}
	
	QString title;
	QString lyrics;
	SongOnlineConnector *source;
};

typedef QList<SongOnlineSearchResult> SongResultList;

class SongOnlineConnector : public QObject
{
	Q_OBJECT
public:
	SongOnlineConnector(QObject *parent = 0);
	virtual ~SongOnlineConnector();
	
	virtual void searchSong(const QString& title);
	
	virtual QString searchUrl(const QString& title) = 0;
	
signals:
	void songResultsAvailable(const QString& title, const SongResultList& list);

protected slots:
	void downloadFinished(QNetworkReply *reply);
	
protected:
	QString html2text(const QString &tmp);
	
	virtual SongResultList parseHtmlReply(QByteArray &) = 0;
	
	virtual SongResultList loadCached(const BibleVerseRef&);
	virtual void cacheList(const SongResultList &);
	virtual bool isCached(const SongResultList&);
	
	QNetworkAccessManager * m_net;
	QHash<QString, SongOnlineSearchResult> m_cache; 
	QHash<QNetworkReply*, QString> m_inProgress;
	
};

#endif
