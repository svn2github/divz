#ifndef GLTextDrawable_h
#define GLTextDrawable_h

#include "GLImageDrawable.h"

class RichTextRenderer;

class GLTextDrawable : public GLImageDrawable
{
	Q_OBJECT
	
	Q_PROPERTY(QString text READ text WRITE setText);
	
public:
	GLTextDrawable(QString text="", QObject *parent=0);
	
	QString text() { return m_text; }
	
public slots:
	void setText(const QString&);
	
	
protected:
	virtual void drawableResized(const QSizeF& /*newSize*/);
	virtual void updateRects(bool secondSource=false);
	
private slots:
	void testXfade();
	
private:
	QString m_text;
	RichTextRenderer *m_renderer;
};

#endif
