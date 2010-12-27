#ifndef GLTextDrawable_h
#define GLTextDrawable_h

#include "GLImageDrawable.h"

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
	
public:
	GLTextDrawable(QString text="", QObject *parent=0);
	~GLTextDrawable();
	
	QString plainText();
	QString text() { return m_text; }
	bool isCountdown() { return m_isCountdown; }
	QDateTime targetDateTime() { return m_targetTime; }
	bool isClock() { return m_isClock; }
	QString clockFormat() { return m_clockFormat; }
	
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
	
	bool m_lockSetPlainText;
	
	QString m_cachedImageText;
	QImage m_cachedImage;
};

#endif
