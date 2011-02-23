#ifndef GLSceneTypeNewsFeed_H
#define GLSceneTypeNewsFeed_H

#include "GLSceneGroupType.h"

#include <QNetworkReply>

/** \class GLSceneTypeNewsFeed
	A simple news feed based on the Google News Data API. Shows one news item every time the slide is displayed, looping through the list it downloads from google.
	
	Parameters:
		- UpdateTime - Time in minutes to wait between reloading the weather data from the server
	 
	Fields Required:
		- Title
			- Text
			- Example: Workers continue Egypt strikes
			- Required
		- Text
			- Text
			- Example: Doctors and lawyers among thousands of workers joining strike as anti-Mubarak demonstrations enter 17th day. Egyptian labour unions have held nationwide strikes for a second day, adding momentum to the pro-democracy demonstrations in Cairo and other ..
			- Required
		- Source
			- Text
			- Example: Aljazeera.net
			- Optional
		- Date
			- Text
			- Example: 13 minutes ago
			- Optional
		- QRCode
			- Image
			- Displays a QR Code with a link to the news item 
			- Optional
*/			


class RssParser : public QObject
{
	Q_OBJECT
	
	Q_PROPERTY(QString url READ url WRITE setUrl);
	Q_PROPERTY(int updateTime READ updateTime WRITE setUpdateTime);
	
public:
	RssParser(QString url="", QObject *parent=0);
	
	class RssItem
	{
	public:
		QString title;
		QString text;
		QString source;
		QString url;
		QString date;
	};
	
	QString url() { return m_url; }
	int updateTime() { return m_updateTime; }

	QList<RssItem> items() { return m_items; }

signals:
	void itemsAvailable(QList<RssParser::RssItem>);

	
public slots:
	/** Set the time to wait between updates to \a minutes */
	void setUpdateTime(int minutes);
		
	/** Reload the data from server*/
	void reloadData();
	
	void setUrl(const QString&);

private slots:
	void loadUrl(const QString &url);
	void handleNetworkData(QNetworkReply *networkReply);
	void parseData(const QString &data);

private:
	QTimer m_reloadTimer;
		
	QString m_url;
	int m_updateTime;
	QList<RssItem> m_items;

};



class GLSceneTypeNewsFeed : public GLSceneType
{
	Q_OBJECT
	
	Q_PROPERTY(int updateTime READ updateTime WRITE setUpdateTime);
	
public:
	GLSceneTypeNewsFeed(QObject *parent=0);
	virtual ~GLSceneTypeNewsFeed() {};
	
	virtual QString id()		{ return "0aa1083e-1aea-46c0-a77e-c8c5d8576f6a"; }
	virtual QString title()		{ return "News Feed"; }
	virtual QString description()	{ return "Displays a news feed from the Google News Data API"; }
	
	/** Returns the current update time parameter value.
		\sa setUpdateTime() */
	int updateTime() { return m_params["updateTime"].toInt(); }
	
public slots:
	virtual void setLiveStatus (bool flag=true);
	
	/** Overridden to intercept changes to the 'updateTime' parameter. */
	virtual void setParam(QString param, QVariant value);
	
	/** Set the time to wait between updates to \a minutes */
	void setUpdateTime(int minutes) { setParam("updateTime", minutes); }
		
	/** Reload the data from Google */
	void reloadData();
	
private slots:
	void requestData(const QString &location);
	void handleNetworkData(QNetworkReply *networkReply);
	void parseData(const QString &data);
	
	void showNextItem();
	
	void itemsAvailable(QList<RssParser::RssItem>);

private:
	QTimer m_reloadTimer;
	
	//typedef RssParser::RssItem NewsItem;
	class NewsItem
	{
	public:
		QString title;
		QString text;
		QString source;
		QString url;
		QString date;
	};
	
	QList<NewsItem> m_news;
	int m_currentIndex;

	RssParser *m_parser;
};

#endif
