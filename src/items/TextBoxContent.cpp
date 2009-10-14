#include "TextBoxContent.h"
#include "frames/Frame.h"
#include "items/BezierCubicItem.h"
#include "items/TextProperties.h"
#include "model/TextItem.h"
// #include "CPixmap.h"
#include "RenderOpts.h"
#include <QDebug>
#include <QFileInfo>
#include <QGraphicsScene>
#include <QMimeData>
#include <QPainter>
#include <QTextDocument>
#include <QTextFrame>
#include <QUrl>
#include <QAbstractTextDocumentLayout>
#include <QDebug>
#include <QTextOption>
#include <QTimer>
#include <QPainterPathStroker>
#include "items/CornerItem.h"

#define DEBUG_LAYOUT 0

#if QT_VERSION >= 0x040600
	#define QT46_SHADOW_ENAB 1
#endif

// static QString trimLeft(QString str)
// {
// 	static QRegExp white("\\s");
// 	while(white.indexIn(str) == 0)
// 	{
// 		str = str.right(str.length()-1);
// 	}
// 	return str;
// }
//

TextBoxContent::TextBoxContent(QGraphicsScene * scene, QGraphicsItem * parent)
    : AbstractContent(scene, parent, false)
    , m_textCache(0)
    , m_cacheScaleX(-1)
    , m_cacheScaleY(-1)
    , m_text(0)
    , m_shadowText(0)
    , m_textRect(0, 0, 0, 0)
    , m_textMargin(4)
//     , m_xTextAlign(Qt::AlignLeft)
//     , m_yTextAlign(Qt::AlignTop)
{
	m_dontSyncToModel = true;

	setFrame(0);
	setFrameTextEnabled(false);
	setToolTip(tr("Right click to Edit the text"));

	// create a text document
	m_text = new QTextDocument(this);
	// for drawing the shadow
	m_shadowText = new QTextDocument(this);

	#if QT_VERSION >= 0x040500
		m_textMargin = (int)m_text->documentMargin();
	#endif

	// template text
	QFont font;
	#ifdef Q_OS_WIN
		font.setFamily("Arial");
	#endif
	font.setPointSize(16);
	m_text->setDefaultFont(font);
	m_text->setPlainText(tr("right click to edit..."));
	setHtml(m_text->toHtml());

	//connect(this, SIGNAL(resized()), this, SLOT(delayContentsResized()));

	for(int i=0;i<m_cornerItems.size();i++)
		m_cornerItems.at(i)->setDefaultLeftOp(CornerItem::Scale);

	m_dontSyncToModel = false;
}

TextBoxContent::~TextBoxContent()
{
	delete m_text;
	delete m_shadowText;
	if(m_textCache)
		delete m_textCache;
}

QString TextBoxContent::toHtml()
{
	TextItem * textModel = dynamic_cast<TextItem*>(modelItem());
	return textModel ? textModel->text() : "";
}



void TextBoxContent::setHtml(const QString & htmlCode)
{
        //qDebug("Setting HTML... [%s]",htmlCode.toAscii().constData());
	m_text->setHtml(htmlCode);
	m_shadowText->setHtml(htmlCode);
	//qDebug()<<"TextBoxContent::setHtml: (not shown)";
	updateTextConstraints();
        //qDebug("Calling syncToModelItem");
	syncToModelItem(0);

	// Apply outline pen to the html
	QTextCursor cursor(m_text);
	cursor.select(QTextCursor::Document);

	QTextCharFormat format;

	QPen p = modelItem() ? modelItem()->outlinePen() : QPen(Qt::black,1.5);
	p.setJoinStyle(Qt::MiterJoin);

	format.setTextOutline(p);
	format.setForeground(modelItem() ? modelItem()->fillBrush() : Qt::white);

	cursor.mergeCharFormat(format);

	#if QT46_SHADOW_ENAB == 0
	// Setup the shadow text formatting if enabled
	if(modelItem() && modelItem()->shadowEnabled())
	{
		QTextCursor cursor(m_shadowText);
		cursor.select(QTextCursor::Document);

		QTextCharFormat format;
		format.setTextOutline(Qt::NoPen);
		format.setForeground(modelItem() ? modelItem()->shadowBrush() : Qt::black);

		cursor.mergeCharFormat(format);
	}
	#endif

	delete m_textCache;
	m_textCache = 0;

	update();
}

/*void TextBoxContent::setXTextAlign(Qt::Alignment x)
{
	m_xTextAlign = x;
        //qDebug()<<"TextBoxContent::setXTextAlign: "<<x;
        updateTextConstraints();
        syncToModelItem(0);
}

void TextBoxContent::setYTextAlign(Qt::Alignment y)
{
	m_yTextAlign = y;
        //qDebug()<<"TextBoxContent::setYTextAlign: "<<y;
        updateTextConstraints();
        syncToModelItem(0);
}
*/

QWidget * TextBoxContent::createPropertyWidget()
{
	TextProperties * p = new TextProperties();

	// common properties
	connect(p->bFront, SIGNAL(clicked()), this, SLOT(slotStackFront()));
	connect(p->bRaise, SIGNAL(clicked()), this, SLOT(slotStackRaise()));
	connect(p->bLower, SIGNAL(clicked()), this, SLOT(slotStackLower()));
	connect(p->bBack, SIGNAL(clicked()), this, SLOT(slotStackBack()));
	connect(p->bDel, SIGNAL(clicked()), this, SIGNAL(deleteItem()), Qt::QueuedConnection);


	return p;
}

void TextBoxContent::syncFromModelItem(AbstractVisualItem *model)
{
        m_dontSyncToModel = true;
	if(!modelItem())
		setModelItem(model);

	static int x = 0;
	x++;
	//qDebug() << x<<": TextBoxContent::syncFromModelItem() mark";
	QFont font;
	TextItem * textModel = dynamic_cast<TextItem*>(model);

	if(textModel->text().indexOf('<') < 0)
	{
		//qDebug() << "TextBoxContent:: converting plain text from model item to html";
		m_text->setPlainText(textModel->text());
		textModel->setText(m_text->toHtml());
	}
	setHtml(textModel->text());

	font.setFamily(textModel->fontFamily());
	font.setPointSize((int)textModel->fontSize());
	m_text->setDefaultFont(font);


// 	setXTextAlign(textModel->xTextAlign());
// 	setYTextAlign(textModel->yTextAlign());

	AbstractContent::syncFromModelItem(model);

        m_dontSyncToModel = false;
}

AbstractVisualItem * TextBoxContent::syncToModelItem(AbstractVisualItem *model)
{
	TextItem * textModel = dynamic_cast<TextItem*>(AbstractContent::syncToModelItem(model));

	setModelItemIsChanging(true);

	if(!textModel)
	{
		setModelItemIsChanging(false);
                //qDebug("TextBoxContent::syncToModelItem: textModel is null, cannot sync\n");
		return 0;
	}
        //qDebug("TextBoxContent:syncToModelItem: Syncing to model! Yay!");
	//textModel->setText(m_text->toHtml());
	textModel->setFontFamily(m_text->defaultFont().family());
	//textModel->setFontSize(m_text->defaultFont().pointSize());

// 	textModel->setXTextAlign(xTextAlign());
// 	textModel->setYTextAlign(yTextAlign());

	setModelItemIsChanging(false);

	return model;
}


QPixmap TextBoxContent::renderContent(const QSize & size, Qt::AspectRatioMode /*ratio*/) const
{
	// get the base empty pixmap
	QSize textSize = boundingRect().size().toSize();
	const float w = size.width(),
			h = size.height(),
			tw = textSize.width(),
			th = textSize.height();
	if (w < 2 || h < 2 || tw < 2 || th < 2)
		return QPixmap();

	// draw text (centered, maximized keeping aspect ratio)
	float scale = qMin(w / (tw + 16), h / (th + 16));
	QPixmap pix(size);
	pix.fill(Qt::transparent);
	QPainter pixPainter(&pix);
	pixPainter.translate((w - (int)((float)tw * scale)) / 2, (h - (int)((float)th * scale)) / 2);
	pixPainter.scale(scale, scale);
	m_text->drawContents(&pixPainter);
	pixPainter.end();
	return pix;
}

int TextBoxContent::contentHeightForWidth(int width) const
{
	// if no text size is available, use default
	if (m_textRect.width() < 1 || m_textRect.height() < 1)
		return AbstractContent::contentHeightForWidth(width);


	m_text->setTextWidth(width);
	QSizeF sz = m_text->documentLayout()->documentSize();
      	m_text->setTextWidth(contentsRect().width());
      	return (int)sz.height();
}

void TextBoxContent::selectionChanged(bool /*selected*/)
{

}

void TextBoxContent::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
	emit backgroundMe();
	QGraphicsItem::mouseDoubleClickEvent(event);
}

void TextBoxContent::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	// paint parent
	AbstractContent::paint(painter, option, widget);

	painter->save();

	//TODO should we clip to the rect or FORCE resize the rect? probably clip...
	painter->setClipRect(contentsRect());
	painter->translate(contentsRect().topLeft()); // + QPoint(p.width(),p.width()));



	//	QAbstractTextDocumentLayout::PaintContext pCtx;
	//	m_text->documentLayout()->draw(painter, pCtx);

	bool pixmapReset = false;
	if(!m_textCache || m_textCache->size() != contentsRect().size())
	{
		if(m_textCache)
			delete m_textCache;
		m_textCache = new QPixmap(contentsRect().size());
		pixmapReset = true;
	}

	// The primary and only reason we cache the text rendering is inorder
	// to paint the text and shadow as a single unit (e.g. composite the
	// shadow+text BEFORE applying opacity rather than setting the opacity
	// before rendering the shaodw.) If we didnt cache the text as a pixmap
	// (e.g. render text directly) then when crossfading, the shadow
	// "apperas" to fade out last, after the text
	QTransform tx = painter->transform();
	if(pixmapReset || m_cacheScaleX != tx.m11() || m_cacheScaleY != tx.m22())
	{
		m_cacheScaleX = tx.m11();
		m_cacheScaleY = tx.m22();

		m_textCache->fill(Qt::transparent);
		QPainter textPainter(m_textCache);

		QAbstractTextDocumentLayout::PaintContext pCtx;

		#if QT46_SHADOW_ENAB == 0
		if(modelItem()->shadowEnabled())
		{
			textPainter.save();

			textPainter.translate(modelItem()->shadowOffsetX(),modelItem()->shadowOffsetY());
			m_shadowText->documentLayout()->draw(&textPainter, pCtx);

			textPainter.restore();
		}
		#endif

		m_text->documentLayout()->draw(&textPainter, pCtx);
	}

	painter->drawPixmap(0,0,*m_textCache);


	if(sceneContextHint() == MyGraphicsScene::Editor &&
		m_text->toPlainText().trimmed() == "")
	{
		QPen p = modelItem() ? modelItem()->outlinePen() : QPen(Qt::black,1.5);
		painter->setPen(p);
		painter->setBrush(Qt::NoBrush);

		painter->drawRect(QRect(QPoint(0,0),contentsRect().size()));
	}


	painter->restore();
}

void TextBoxContent::updateTextConstraints(int w)
{
	if(!m_text)
		return;

	int textWidth = w;
	if(w < 0)
		textWidth = contentsRect().width();

	if(DEBUG_LAYOUT)
		qDebug("updateTextConstraints() BEGIN (width: %d)",textWidth);


	m_text->setTextWidth(textWidth);
	m_shadowText->setTextWidth(textWidth);

	QSizeF sz = m_text->documentLayout()->documentSize();
      	m_textRect = QRect(QPoint(0,0),sz.toSize());

	// Adjust the bounding rect *height* to our document wrapped height, but leave
	// the width alone.
	bool changed = false;
	QRect newRect = contentsRect();
	if(m_textRect.height() > newRect.height())
	{
		// Changed from "!=" comparrison to ">" inorder to not mess up song slide templates
		// - JB 20091011
		newRect.setHeight(m_textRect.height());
		changed = true;
	}

	if(changed)
	{
		AbstractContent::resizeContents(newRect);
	}

}

void TextBoxContent::delayContentsResized()
{
	QTimer::singleShot(0,this,SLOT(contentsResized()));
}

void TextBoxContent::contentsResized()
{
 	updateTextConstraints();
 	update();
}

void TextBoxContent::resizeContents(const QRect & rect, bool keepRatio)
{
	updateTextConstraints(rect.width());
	AbstractContent::resizeContents(rect,keepRatio);
	update();
}
/*
void TextBoxContent::updateCache()
{
    m_cachePixmap = QPixmap(contentsRect().size());
    m_cachePixmap.fill(QColor(0, 0, 0, 0));
    QPainter painter(&m_cachePixmap);
    painter.setRenderHint(QPainter::Antialiasing);

}*/
