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

static QString trimLeft(QString str)
{
	static QRegExp white("\\s");
	while(white.indexIn(str) == 0)
	{
		str = str.right(str.length()-1);
	}
	return str;
}


TextBoxContent::TextBoxContent(QGraphicsScene * scene, QGraphicsItem * parent)
    : AbstractContent(scene, parent, false)
    , m_text(0)
    , m_shadowText(0)
    , m_textRect(0, 0, 0, 0)
    , m_textMargin(4)
    , m_shapeEditor(0)
    , m_xTextAlign(Qt::AlignLeft)
    , m_yTextAlign(Qt::AlignTop)
    , m_cacheScaleX(-1)
    , m_cacheScaleY(-1)
    , m_textCache(0)
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
	
	// shape editor
// 	m_shapeEditor = new BezierCubicItem(this);
// 	m_shapeEditor->setVisible(false);
// 	m_shapeEditor->setControlPoints(QList<QPointF>() << QPointF(-100, -50) << QPointF(-10, 40) << QPointF(100, -50) << QPointF(100, 50));
// 	connect(m_shapeEditor, SIGNAL(shapeChanged(const QPainterPath &)), this, SLOT(setShapePath(const QPainterPath &)));
	
	
	//connect(this, SIGNAL(resized()), this, SLOT(delayContentsResized()));
	
	for(int i=0;i<m_cornerItems.size();i++)
		m_cornerItems.at(i)->setDefaultLeftOp(CornerItem::Scale);
		
	m_dontSyncToModel = false;
}

TextBoxContent::~TextBoxContent()
{
	//delete m_shapeEditor;
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
}

void TextBoxContent::setXTextAlign(Qt::Alignment x)
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
	
bool TextBoxContent::hasShape() const
{
	return !m_shapePath.isEmpty();
}

void TextBoxContent::clearShape()
{
	setShapePath(QPainterPath());
	setShapeEditing(false);
	emit notifyHasShape(false);
}

bool TextBoxContent::isShapeEditing() const
{
	//return m_shapeEditor->isVisible();
	return false;
}

void TextBoxContent::setShapeEditing(bool enabled)
{
// 	if (enabled) 
// 	{
// 		// shape editor on
// 		if (!m_shapeEditor->isVisible()) 
// 		{
// 			m_shapeEditor->show();
// 			emit notifyShapeEditing(true);
// 		}
// 	
// 		// begin new shape
// 		if (!hasShape()) 
// 		{
// 			// use caching only when drawing shaped [disabled because updates are wrong when cached!]
// 			//setCacheMode(enabled ? QGraphicsItem::DeviceCoordinateCache : QGraphicsItem::NoCache);
// 		
// 			// use new shape
// 			setShapePath(m_shapeEditor->shape());
// 			emit notifyHasShape(true);
// 		}
// 	} 
// 	else 
// 	{
// 		// shape editor off
// 		if (m_shapeEditor->isVisible()) 
// 		{
// 			m_shapeEditor->hide();
// 			emit notifyShapeEditing(false);
// 		}
// 	}
}

QWidget * TextBoxContent::createPropertyWidget()
{
	TextProperties * p = new TextProperties();
	
	// common properties
	connect(p->bFront, SIGNAL(clicked()), this, SLOT(slotStackFront()));
	connect(p->bRaise, SIGNAL(clicked()), this, SLOT(slotStackRaise()));
	connect(p->bLower, SIGNAL(clicked()), this, SLOT(slotStackLower()));
	connect(p->bBack, SIGNAL(clicked()), this, SLOT(slotStackBack()));
	connect(p->bDel, SIGNAL(clicked()), this, SIGNAL(deleteItem()), Qt::QueuedConnection);
	
	// shape properties
	p->bEditShape->setChecked(isShapeEditing());
	connect(this, SIGNAL(notifyShapeEditing(bool)), p->bEditShape, SLOT(setChecked(bool)));
	connect(p->bEditShape, SIGNAL(toggled(bool)), this, SLOT(setShapeEditing(bool)));
	p->bClearShape->setVisible(hasShape());
	connect(this, SIGNAL(notifyHasShape(bool)), p->bClearShape, SLOT(setVisible(bool)));
	connect(p->bClearShape, SIGNAL(clicked()), this, SLOT(clearShape()));
	
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
		m_text->setPlainText(textModel->text());
		textModel->setText(m_text->toHtml());
	}
	setHtml(textModel->text());
	
	font.setFamily(textModel->fontFamily());
	font.setPointSize((int)textModel->fontSize());
	m_text->setDefaultFont(font);
	
	
// 	bool shapeEnabled = textModel->shapeEnabled();
// 	if (shapeEnabled)
// 	{
// 		QList<QPointF> points;
// 		QStringList strPoint;
// 		points << textModel->shapePoint1();
// 		points << textModel->shapePoint2();
// 		points << textModel->shapePoint3();
// 		points << textModel->shapePoint4();
// 		m_shapeEditor->setControlPoints(points);
// 	}
	
	setXTextAlign(textModel->xTextAlign());
	setYTextAlign(textModel->yTextAlign());
	
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
	textModel->setText(m_text->toHtml());
	textModel->setFontFamily(m_text->defaultFont().family());
	textModel->setFontSize(m_text->defaultFont().pointSize());
 	textModel->setShapeEnabled(hasShape());
// 	if (hasShape()) 
// 	{
// 		QList<QPointF> cp = m_shapeEditor->controlPoints();
// 		textModel->setShapePoint1(cp[0]);
// 		textModel->setShapePoint2(cp[2]);
// 		textModel->setShapePoint3(cp[3]);
// 		textModel->setShapePoint4(cp[4]);
// 	}
// 	
	textModel->setXTextAlign(xTextAlign());
	textModel->setYTextAlign(yTextAlign());
	
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
      	return sz.height();
}

void TextBoxContent::selectionChanged(bool selected)
{
	// hide shape editing controls
	if (!selected && isShapeEditing())
		setShapeEditing(false);
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
		
		if(modelItem()->shadowEnabled())
		{
			textPainter.save();
			
			textPainter.translate(modelItem()->shadowOffsetX(),modelItem()->shadowOffsetY());
			m_shadowText->documentLayout()->draw(&textPainter, pCtx);
			
			textPainter.restore();
		}
	
		m_text->documentLayout()->draw(&textPainter, pCtx);
	}
	
	painter->drawPixmap(0,0,*m_textCache);
		
	painter->restore();
}

void TextBoxContent::updateShadowClipPath()
{
// 	QPainterPath clipPath;
// 	clipPath.addRect(QRectF(0,0,contentsRect().width(),contentsRect().height()));
// 	
// 	m_shadowClipPath = clipPath.subtracted(m_textPath);
}

QPainterPath TextBoxContent::shapePath() const
{
	return m_shapePath;
}

void TextBoxContent::setShapePath(const QPainterPath & path)
{
// 	if (path == m_shapePath)
// 		return;
// 	
// 	// invalidate rectangles
// 	m_textRect = QRect();
// 	m_shapeRect = QRect();
// 	
// 	// set new path
// 	m_shapePath = path;
// 	
// 	// regenerate text layouting
// 	updateTextConstraints();
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

	if(1)
	{
		// Adjust the bounding rect *height* to our document wrapped height, but leave
		// the width alone.
		bool changed = false;
		QRect newRect = contentsRect();
		if(m_textRect.height() != newRect.height())
		{
			newRect.setHeight(m_textRect.height());
			changed = true;
		}
	// 	if(m_textRect.width() > newRect.width())
	// 	{
	// 		newRect.setWidth(m_textRect.width());
	// 		changed = true;
	// 	}
		if(changed)
		{
			AbstractContent::resizeContents(newRect);
		}
	
	}
	
	updateShadowClipPath();
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

void TextBoxContent::updateCache()
{
    /*
    m_cachePixmap = QPixmap(contentsRect().size());
    m_cachePixmap.fill(QColor(0, 0, 0, 0));
    QPainter painter(&m_cachePixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    ...
    */
}
