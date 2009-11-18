#include "OutputViewContent.h"
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
#include <QPixmapCache>
#include <QGraphicsProxyWidget>

#include "model/OutputViewItem.h"
#include "OutputInstance.h"
#include "AppSettings.h"
#include "MainWindow.h"
#include "model/Output.h"

#if QT_VERSION >= 0x040600
	#define QT46_SHADOW_ENAB 0
#endif

OutputViewContent::OutputViewContent(QGraphicsScene * scene, QGraphicsItem * parent)
    : AbstractContent(scene, parent, false)
    , m_inst(0)
{
	m_dontSyncToModel = true;
	
	setFrame(0);
	setFrameTextEnabled(false);
	setToolTip(tr("Output Viewer - right click for options."));
	
// 	for(int i=0;i<m_cornerItems.size();i++)
// 		m_cornerItems.at(i)->setDefaultLeftOp(CornerItem::Scale);

	m_viewer = new OutputInstance(Output::widgetInstance());
	m_viewer->setCanZoom(false);
	
	m_widgetProxy = new QGraphicsProxyWidget(this);
	m_widgetProxy->setWidget(m_viewer);
	
	m_dontSyncToModel = false;
}

OutputViewContent::~OutputViewContent()
{
}

QWidget * OutputViewContent::createPropertyWidget()
{
	return 0;
}

void OutputViewContent::syncFromModelItem(AbstractVisualItem *model)
{
	if(!modelItem())
		setModelItem(model);

	AbstractContent::syncFromModelItem(model);
	
	setOutputId(dynamic_cast<OutputViewItem*>(model)->outputId());
	
}



AbstractVisualItem * OutputViewContent::syncToModelItem(AbstractVisualItem *model)
{	
	return AbstractContent::syncToModelItem(model);
}

QPixmap OutputViewContent::renderContent(const QSize & size, Qt::AspectRatioMode /*ratio*/) const
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

int OutputViewContent::contentHeightForWidth(int width) const
{
	return AbstractContent::contentHeightForWidth(width);
}


void OutputViewContent::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	// paint parent
	AbstractContent::paint(painter, option, widget);
}

void OutputViewContent::setOutputId(int id)
{
	OutputInstance *inst = MainWindow::mw()->outputInst(id);
	if(m_inst)
		m_inst->removeMirror(m_viewer);
		
	if(inst)
	{
		qDebug() << "OutputViewContent::setOutputId("<<id<<"): Initalizing inst for output"<<inst->output()->name();
		m_inst = inst;
		m_inst->addMirror(m_viewer);
	}
	else
	{
		m_inst = 0;
		qDebug() << "OutputViewContent::setOutputId("<<id<<"): Could not find output instance for id:"<<id;
	}
}
