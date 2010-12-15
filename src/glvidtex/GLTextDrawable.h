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
	
public:
	GLTextDrawable(QString text="", QObject *parent=0);
	
	QString plainText();
	QString text() { return m_text; }
	bool isCountdown() { return m_isCountdown; }
	QDateTime targetDateTime() { return m_targetTime; }
	
	static QString htmlToPlainText(const QString&);

public slots:
	void setPlainText(const QString&, bool replaceNewlineSlash=true);
	void setText(const QString&);
	void setIsCountdown(bool);
	void setTargetDateTime(const QDateTime&);
	
signals:
	void textChanged(const QString& html);
	void plainTextChanged(const QString& text);
	
protected:
	virtual void drawableResized(const QSizeF& /*newSize*/);
	virtual void updateRects(bool secondSource=false);
	
private slots:
	void testXfade();
	
	void countdownTick();
	
private:
	QString formatTime(double time);
	
	QString m_text;
	RichTextRenderer *m_renderer;
	
	bool m_isCountdown;
	QTimer m_countdownTimer;
	QDateTime m_targetTime;
	
	bool m_lockSetPlainText;
};

#endif
