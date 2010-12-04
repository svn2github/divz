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
	
	//QTimer::singleShot(1500, this, SLOT(testXfade()));
	
	foreach(CornerItem *corner, m_cornerItems)
		corner->setDefaultLeftOp(CornerItem::Scale);
		
	//setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
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
	if(m_renderer->textWidth() != (int)newSize.width())
		m_renderer->setTextWidth((int)newSize.width());
	GLVideoDrawable::drawableResized(newSize);
}

void GLTextDrawable::updateRects(bool secondSource)
{
	QRectF sourceRect = m_frame.rect;
	
	updateTextureOffsets();
	
	// Honor croping since the rendering will honor croping - but not really needed for text.
	QRectF adjustedSource = sourceRect.adjusted(
		m_displayOpts.cropTopLeft.x(),
		m_displayOpts.cropTopLeft.y(),
		m_displayOpts.cropBottomRight.x(),
		m_displayOpts.cropBottomRight.y());
		
	QRectF targetRect = QRectF(rect().topLeft(),adjustedSource.size()); 
	
	if(!secondSource)
	{
		//qDebug() << "GLTextDrawable::updateRects: source:"<<sourceRect<<", target:"<<targetRect;
		m_sourceRect = sourceRect;
		m_targetRect = targetRect;
	}
	else
	{
		m_sourceRect2 = sourceRect;
		m_targetRect2 = targetRect;
	}
}
