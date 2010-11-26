#include "GLTextDrawable.h"

#include "RichTextRenderer.h"


GLTextDrawable::GLTextDrawable(QString text, QObject *parent)
	: GLImageDrawable("",parent)
{
	m_renderer = new RichTextRenderer(this);
	connect(m_renderer, SIGNAL(textRendered(QImage)), this, SLOT(setImage(const QImage&)));
	
	m_renderer->setTextWidth(1000); // just a guess
	setImage(QImage("dot.gif"));
	
	if(!text.isEmpty())
		setText(text);
	
	QTimer::singleShot(1500, this, SLOT(testXfade()));
}
	
void GLTextDrawable::testXfade()
{
	qDebug() << "GLTextDrawable::testXfade(): loading text #2";
	setText("Friday 2010-11-26");
}

void GLTextDrawable::setText(const QString& text)
{
	m_text = text;
	qDebug() << "GLTextDrawable::setText(): text:"<<text;
	
	m_renderer->setHtml(text);
	m_renderer->changeFontSize(40);
}


void GLTextDrawable::drawableResized(const QSizeF& newSize)
{
	m_renderer->setTextWidth((int)newSize.width());
	GLVideoDrawable::drawableResized(newSize);
}
