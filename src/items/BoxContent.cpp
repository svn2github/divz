#include "BoxContent.h"
#include "frames/Frame.h"
// #include "items/BezierCubicItem.h"
#include "items/TextProperties.h"
#include "model/BoxItem.h"
#include "items/CornerItem.h"
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

BoxContent::BoxContent(QGraphicsScene * scene, QGraphicsItem * parent)
    : AbstractContent(scene, parent, false)
//     , m_text(0)
//     , m_textRect(0, 0, 0, 0)
//     , m_textMargin(4)
{
	m_dontSyncToModel = true;
	
	setFrame(0);
	setFrameTextEnabled(false);
	setToolTip(tr("Rectangle - right click for options."));
	
// 	setText("Welcome");
	for(int i=0;i<m_cornerItems.size();i++)
		m_cornerItems.at(i)->setDefaultLeftOp(CornerItem::Scale);
	
	m_dontSyncToModel = false;
}

BoxContent::~BoxContent()
{
// 	delete m_shapeEditor;
// 	delete m_text;
}

QWidget * BoxContent::createPropertyWidget()
{
/*	TextProperties * p = new TextProperties();
	
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
	
	return p;*/
	
	return 0;
}

void BoxContent::syncFromModelItem(AbstractVisualItem *model)
{
        m_dontSyncToModel = true;
	setModelItem(model);
	
	QFont font;
	BoxItem * boxmodel = dynamic_cast<BoxItem*>(model);
	
// 	setText(textModel->text());
// 	
// 	font.setFamily(textModel->fontFamily());
// 	font.setPointSize((int)textModel->fontSize());
// 	setFont(font);
	
	AbstractContent::syncFromModelItem(model);
	
        m_dontSyncToModel = false;
}

AbstractVisualItem * BoxContent::syncToModelItem(AbstractVisualItem *model)
{
	setModelItemIsChanging(true);
	
	BoxItem * boxModel = dynamic_cast<BoxItem*>(AbstractContent::syncToModelItem(model));
	
	if(!boxModel)
	{
		setModelItemIsChanging(false);
                //qDebug("BoxContent::syncToModelItem: textModel is null, cannot sync\n");
		return 0;
	}
        //qDebug("TextContent:syncToModelItem: Syncing to model! Yay!");
// 	textModel->setText(text());
// 	textModel->setFontFamily(font().family());
// 	textModel->setFontSize(font().pointSize());
	
	setModelItemIsChanging(false);
	
	return model;
}

QPixmap BoxContent::renderContent(const QSize & size, Qt::AspectRatioMode /*ratio*/) const
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
	//m_text->drawContents(&pixPainter);
// 	pixPainter.drawText(0,0,m_text);
	pixPainter.end();
	return pix;
}

int BoxContent::contentHeightForWidth(int width) const
{
// 	// if no text size is available, use default
// 	if (m_textRect.width() < 1 || m_textRect.height() < 1)
		return AbstractContent::contentHeightForWidth(width);
//	return (m_textRect.height() * width) / m_textRect.width();
}

void BoxContent::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	// paint parent
	AbstractContent::paint(painter, option, widget);
	
	QRect cRect = contentsRect();
// 	QRect sRect = m_textRect;
	painter->save();
	painter->translate(cRect.topLeft());
// 	if (sRect.width() > 0 && sRect.height() > 0)
// 	{
// 		qreal xScale = (qreal)cRect.width() / (qreal)sRect.width();
// 		qreal yScale = (qreal)cRect.height() / (qreal)sRect.height();
// 		if (!qFuzzyCompare(xScale, 1.0) || !qFuzzyCompare(yScale, 1.0))
// 		painter->scale(xScale, yScale);
// 	}
	
	QPen pen;
 	pen.setWidthF(3);
 	pen.setColor(QColor(0,0,0,255));
 
 	QBrush brush(QColor(255,0,0,255));
 	painter->setPen(pen);
 	painter->setBrush(brush);
	painter->drawRect(QRect(QPoint(0,0),cRect.size()));
	
	painter->restore();
}

