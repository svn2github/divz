#include "BibleConnector.h"
#include "BibleModel.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringList>
#include <QRegExp>


BibleConnector::BibleConnector(QObject *parent) 
	: QObject(parent)
	, m_net(0)
{
	m_net = new QNetworkAccessManager(this);
	connect(m_net, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
}

BibleConnector::~BibleConnector() {}

void BibleConnector::downloadReference(const QString& reference)
{
	BibleVerseList list;
	 
	QString urlString = urlForReference(reference);
	
	QUrl url(urlString);
	QNetworkReply * reply = m_net->get(QNetworkRequest(url));
	m_inProgress[reply] = reference;
}


void BibleConnector::downloadFinished(QNetworkReply *reply)
{
	QString reference = m_inProgress[reply];
	m_inProgress.remove(reply);
	
	QUrl url = reply->url();
	if (reply->error()) 
	{
		fprintf(stderr, "Download of %s failed: %s\n",
			url.toEncoded().constData(),
			qPrintable(reply->errorString()));
	} 
	else 
	{
		QByteArray ba = reply->readAll();
	
		//qDebug() << "Reply: "<<QString(ba);
		BibleVerseList list = parseHtmlReply(ba);
		m_cache[reference] = list;
		
		qDebug() << "BibleConnector::downloadFinished(): Downloaded: "<<list;
		
		emit referenceAvailable(reference,list);
	}
	
	reply->deleteLater();
}

QString BibleConnector::html2text(const QString &tmp)
{
	QString text = tmp;
	QRegExp rx = QRegExp("<(script|style)[^>]*>(.|\n)*</(script|style)>",Qt::CaseInsensitive);
	rx.setMinimal(true);
	text.replace(rx,"");
	
	rx = QRegExp("<sup[^>]*>(.|\n)*</sup>",Qt::CaseInsensitive);
	rx.setMinimal(true);
	text.replace(rx,"");
	
	text.replace(QRegExp("<!--(.|\n)*?-->",Qt::CaseInsensitive),"");
	text.replace(QRegExp("(<br>|</(p|div|blockquote)>)",Qt::CaseInsensitive),"\n");
	text.replace(QRegExp("</li><li>",Qt::CaseInsensitive)," ");
	text.replace(QRegExp("<[^>]+>",Qt::CaseInsensitive),"");
	text.replace("&amp;","&");
	text.replace("&nbsp;"," ");
	text.replace("&quot;","\"");
	text.replace("&mdash;","--");
	text.replace("&rsquo;","'");
	
	return text;
}

	
bool BibleConnector::findReference(const QString &ref)
{
	if(m_cache.contains(ref))
		return true;
	
	BibleVerseList list = loadCached(ref);
	if(list.isEmpty())
		return false;
	
	m_cache[ref] = list;
	
	emit referenceAvailable(ref, list);
	
	return true;
}

BibleVerseList BibleConnector::loadReference(const QString& ref)
{
	if(!findReference(ref))
		return BibleVerseList();
		
	return m_cache[ref];
}
	 

BibleVerseList BibleConnector::loadCached(const QString& /*ref*/)
{
	//TODO implement disk caching
	BibleVerseList list;
	BibleBook bJohn("John");
	BibleChapter bJohn3(bJohn, 3);
	list << BibleVerse(bJohn3, 16, "For God so loved the world, that He gave His only Son, that whosoever believes in Him should have everlasting life.");
		
	return list;
}

void BibleConnector::cacheList(const QString& /*reference*/, const BibleVerseList &/*list*/)
{
	//TODO implement disk caching
}
