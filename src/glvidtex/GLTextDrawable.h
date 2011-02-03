#ifndef GLTextDrawable_h
#define GLTextDrawable_h

#include "GLImageDrawable.h"

#include <QXmlStreamReader>
#include <QtNetwork>

class RichTextRenderer;

class GLTextDrawable : public GLImageDrawable
{
	Q_OBJECT
	
	Q_PROPERTY(QString plainText READ plainText WRITE setPlainText);
	Q_PROPERTY(QString text READ text WRITE setText USER true);
	
	Q_PROPERTY(bool isCountdown READ isCountdown WRITE setIsCountdown);
	Q_PROPERTY(QDateTime targetDateTime READ targetDateTime WRITE setTargetDateTime);
	
	Q_PROPERTY(bool isClock READ isClock WRITE setIsClock);
	Q_PROPERTY(QString clockFormat READ clockFormat WRITE setClockFormat);
	
	Q_PROPERTY(bool isScroller READ isScroller WRITE setIsScroller);
	Q_PROPERTY(double scrollerSpeed READ scrollerSpeed WRITE setScrollerSpeed);
	Q_PROPERTY(QString iconFile READ iconFile WRITE setIconFile);
	Q_PROPERTY(QUrl rssUrl READ rssUrl WRITE setRssUrl);
	Q_PROPERTY(int rssRefreshTime READ rssRefreshTime  WRITE setRssRefreshTime);
	
public:
	GLTextDrawable(QString text="", QObject *parent=0);
	~GLTextDrawable();
	
	QString plainText();
	QString text() { return m_text; }
	bool isCountdown() { return m_isCountdown; }
	QDateTime targetDateTime() { return m_targetTime; }
	bool isClock() { return m_isClock; }
	QString clockFormat() { return m_clockFormat; }
	
	bool isScroller() { return m_isScroller; }
	double scrollerSpeed() { return m_scrollerSpeed; }
	QString iconFile() { return m_iconFile; }
	QUrl rssUrl() { return m_rssUrl; }
	int rssRefreshTime() { return m_rssRefreshTime; }
	
	static QString htmlToPlainText(const QString&);
	
	QSize findNaturalSize(int atWidth);
	void changeFontSize(double);
	double findFontSize();
	
	virtual void loadPropsFromMap(const QVariantMap&, bool onlyApplyIfChanged = false);
	virtual QVariantMap propsToMap();

public slots:
	void setPlainText(const QString&, bool replaceNewlineSlash=true);
	void setText(const QString&);
	void setIsCountdown(bool);
	void setTargetDateTime(const QDateTime&);
	void setIsClock(bool);
	void setClockFormat(const QString&);
	void setIsScroller(bool);
	void setScrollerSpeed(double);
	void setIconFile(const QString&);
	void setRssUrl(const QUrl&);
	void setRssRefreshTime(int);
	
signals:
	void textChanged(const QString& html);
	void plainTextChanged(const QString& text);
	
protected:
	virtual void drawableResized(const QSizeF& /*newSize*/);
	virtual void updateRects(bool secondSource=false);
	
private slots:
	void testXfade();
	
	void countdownTick();
	void clockTick();
	void scrollerTick();
	void playlistChanged();
	void rssReadData(const QHttpResponseHeader &);
	void parseRssXml();
	void reloadRss();
	
private:
	QString formatTime(double time);
	
	QString m_text;
	RichTextRenderer *m_renderer;
	
	bool m_isCountdown;
	QTimer m_countdownTimer;
	QDateTime m_targetTime;
	
	bool m_isClock;
	QString m_clockFormat;
	QTimer m_clockTimer;
	
	bool m_isScroller;
	double m_scrollerSpeed;
	QString m_iconFile;
	QUrl m_rssUrl;
	QTimer m_scrollerTimer;
	QImage m_scrollerImage;
	double m_scrollPos;
// 	int m_firstItem;
// 	int m_lastItem;
	QHttp m_rssHttp;
	QXmlStreamReader m_rssXml;
	int m_rssRefreshTime;
	QTimer m_rssRefreshTimer;
	QString m_rssTextTemplate;
	bool m_dataReceived;
	bool m_lockScrollerRender;
		
	
	bool m_lockSetPlainText;
	
	QString m_cachedImageText;
	QImage m_cachedImage;
};

#endif
