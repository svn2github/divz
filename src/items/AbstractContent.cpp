#include "AbstractContent.h"
#include "ButtonItem.h"
#include "CornerItem.h"
#include "MirrorItem.h"
#include "RenderOpts.h"
#include "frames/FrameFactory.h"
#include "AppSettings.h"
#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPainter>
#include <QTimer>
#include <QUrl>
#include <math.h>
#include <assert.h>
#include <QDebug>
#include <QPixmapCache>

#if QT_VERSION >= 0x040600
	#include <QGraphicsDropShadowEffect>

	#define QT46_SHADOW_ENAB 0
#endif


#define DEBUG_ABSTRACTCONTENT 0

AbstractContent::AbstractContent(QGraphicsScene * scene, QGraphicsItem * parent, bool noRescale)
    : AbstractDisposeable(parent, true)
    , m_contentsRect(-100, -75, 200, 150)
    , m_frame(0)
    , m_frameTextItem(0)
    , m_controlsVisible(false)
    , m_dirtyTransforming(false)
    , m_transformRefreshTimer(0)
    , m_gfxChangeTimer(0)
    , m_dontSyncToModel(true)
    , m_mirrorItem(0)
    , m_xRotationAngle(0)
    , m_yRotationAngle(0)
    , m_zRotationAngle(0)
    , m_modelItem(0)
    , m_modelItemIsChanging(false)
    , m_hovering(false)
    , m_sourceOffsetTL(0,0)
    , m_sourceOffsetBR(0,0)
{
	// the buffered graphics changes timer
	m_gfxChangeTimer = new QTimer(this);
	m_gfxChangeTimer->setInterval(0);
	m_gfxChangeTimer->setSingleShot(true);

	// customize item's behavior
	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsSelectable);
	#if QT46_SHADOW_ENAB
		setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	#endif
	// allow some items (eg. the shape controls for text) to be shown
	setFlag(QGraphicsItem::ItemClipsChildrenToShape, false);
	setAcceptHoverEvents(true);

	// create child controls
	createCorner(CornerItem::TopLeftCorner, noRescale);
	createCorner(CornerItem::TopRightCorner, noRescale);
	createCorner(CornerItem::BottomLeftCorner, noRescale);
	createCorner(CornerItem::BottomRightCorner, noRescale);
	
	// create midpoint handles
	createCorner(CornerItem::MidTop, noRescale);
	createCorner(CornerItem::MidLeft, noRescale);
	createCorner(CornerItem::MidRight, noRescale);
	createCorner(CornerItem::MidBottom, noRescale);
	

	//ButtonItem * bFront = new ButtonItem(ButtonItem::Control, Qt::blue, QIcon(":/data/action-order-front.png"), this);
	//bFront->setToolTip(tr("Raise"));
	//connect(bFront, SIGNAL(clicked()), this, SLOT(slotStackRaise()));
	//addButtonItem(bFront);

	ButtonItem * bConf = new ButtonItem(ButtonItem::Control, Qt::green, QIcon(":/data/action-configure.png"), this);
	bConf->setToolTip(tr("Change properties..."));
	connect(bConf, SIGNAL(clicked()), this, SLOT(slotConfigure()));
	addButtonItem(bConf);

// 	#if QT_VERSION >= 0x040500
// 	    ButtonItem * bPersp = new ButtonItem(ButtonItem::Control, Qt::red, QIcon(":/data/action-perspective.png"), this);
// 	    bPersp->setToolTip(tr("Drag around to change the perspective.\nHold SHIFT to move faster.\nUse CTRL to cancel the transformations."));
// 	    connect(bPersp, SIGNAL(dragging(const QPointF&,Qt::KeyboardModifiers)), this, SLOT(slotPerspective(const QPointF&,Qt::KeyboardModifiers)));
// 	    connect(bPersp, SIGNAL(doubleClicked()), this, SLOT(slotClearPerspective()));
// 	    addButtonItem(bPersp);
// 	#endif

	ButtonItem * bDelete = new ButtonItem(ButtonItem::Control, Qt::red, QIcon(":/data/action-delete.png"), this);
	bDelete->setSelectsParent(false);
	bDelete->setToolTip(tr("Remove"));
	connect(bDelete, SIGNAL(clicked()), this, SIGNAL(deleteItem()));
	addButtonItem(bDelete);

	// create default frame
	Frame * frame = FrameFactory::defaultPictureFrame();
	setFrame(frame);

	// hide and layoutChildren buttons
	layoutChildren();

	// apply context hints for behaviour based on if we're in an editor, live view, preview, etc
	if(scene)
		applySceneContextHint((dynamic_cast<MyGraphicsScene*>(scene))->contextHint());

	// add to the scene
	//scene->addItem(this);

	// display mirror
	setMirrorEnabled(false); //RenderOpts::LastMirrorEnabled);

	m_dontSyncToModel = false;
}

AbstractContent::~AbstractContent()
{
	qDeleteAll(m_cornerItems);
	qDeleteAll(m_controlItems);
	delete m_mirrorItem;
	delete m_frameTextItem;
	delete m_frame;
	m_modelItem = 0;
}

void AbstractContent::applySceneContextHint(MyGraphicsScene::ContextHint hint)
{
	m_contextHint = hint;

	switch(hint)
	{
		case MyGraphicsScene::Live:
		case MyGraphicsScene::Preview:
		case MyGraphicsScene::Monitor:
			// customize item's behavior
			setFlag(QGraphicsItem::ItemIsMovable, false);
			setFlag(QGraphicsItem::ItemIsFocusable, false);
			setFlag(QGraphicsItem::ItemIsSelectable, false);
			setAcceptHoverEvents(false);

		case MyGraphicsScene::Editor:
		default:
			break;
	};
}

void AbstractContent::dispose(bool anim)
{
	// stick this item
	setFlags((GraphicsItemFlags)0x00);

	// fade out mirror too
	setMirrorEnabled(false);

	// little rotate animation
	#if QT_VERSION >= 0x040600
	if(anim)
	{
		QPropertyAnimation * ani = new QPropertyAnimation(this, "rotation");
		ani->setEasingCurve(QEasingCurve::InQuad);
		ani->setDuration(300);
		ani->setEndValue(-30.0);
		ani->start(QPropertyAnimation::DeleteWhenStopped);
	}
	#endif

	// standard disposition
	AbstractDisposeable::dispose(anim);
}

QRect AbstractContent::contentsRect() const
{
	return m_contentsRect;
}

void AbstractContent::resizeContents(const QRect & rect, bool keepRatio)
{
	if (!rect.isValid())
		return;
	
	prepareGeometryChange();

	m_contentsRect = rect;

	if (keepRatio)
	{
		int hfw = contentHeightForWidth(rect.width());
		if (hfw > 1)
		{
			m_contentsRect.setTop(-hfw / 2);
			m_contentsRect.setHeight(hfw);
		}
	}


	if (m_frame)
		m_frameRect = m_frame->frameRect(m_contentsRect);
	else
		m_frameRect = m_contentsRect;


	layoutChildren();
	update();
	GFX_CHANGED();
	emit resized();
}

void AbstractContent::resetContentsRatio()
{
	resizeContents(m_contentsRect, true);
}

void AbstractContent::delayedDirty(int ms)
{
	// tell rendering that we're changing stuff
	m_dirtyTransforming = true;

	// start refresh timer
	if (!m_transformRefreshTimer) {
		m_transformRefreshTimer = new QTimer(this);
		connect(m_transformRefreshTimer, SIGNAL(timeout()), this, SLOT(slotDirtyEnded()));
		m_transformRefreshTimer->setSingleShot(true);
	}
	m_transformRefreshTimer->start(ms);
}

void AbstractContent::setFrame(Frame * frame)
{
	delete m_frame;
	m_frame = frame;
	if (m_frame)
		FrameFactory::setDefaultPictureClass(m_frame->frameClass());
	resizeContents(m_contentsRect);
	syncToModelItem(modelItem());
	layoutChildren();
	update();
	GFX_CHANGED();
}

quint32 AbstractContent::frameClass() const
{
	if (!m_frame)
		return Frame::NoFrame;
	return m_frame->frameClass();
}

#include <QGraphicsTextItem>
class MyTextItem : public QGraphicsTextItem {
    public:
        MyTextItem(QGraphicsItem * parent = 0)
            : QGraphicsTextItem(parent)
        {
        }

        void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 )
        {
            painter->save();
            painter->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
            QGraphicsTextItem::paint(painter, option, widget);
            painter->restore();
        }

        // prevent the TextItem from listening to global shortcuts
        bool eventFilter(QObject * object, QEvent * event)
        {
            if (event->type() == QEvent::Shortcut || event->type() == QEvent::ShortcutOverride) {
                if (!object->inherits("QGraphicsView")) {
                    event->accept();
                    return true;
                }
            }
            return false;
        }

    protected:
        void focusInEvent(QFocusEvent * event)
        {
            QGraphicsTextItem::focusInEvent(event);
            qApp->installEventFilter(this);
        }

        void focusOutEvent(QFocusEvent * event)
        {
            QGraphicsTextItem::focusOutEvent(event);
            qApp->removeEventFilter(this);
        }
};

void AbstractContent::setFrameTextEnabled(bool enabled)
{
	// create the Text Item, if enabled...
	if (enabled && !m_frameTextItem)
	{
		m_frameTextItem = new MyTextItem(this);
		m_frameTextItem->setTextInteractionFlags(Qt::TextEditorInteraction);
		QFont f("Sans Serif");
		//f.setPointSizeF(7.5);
		m_frameTextItem->setFont(f);
		m_frameTextItem->setZValue(1.0);
		layoutChildren();
	}

	// ...or destroy it if disabled
	else if (!enabled && m_frameTextItem)
	{
		delete m_frameTextItem;
		m_frameTextItem = 0;
	}
}

bool AbstractContent::frameTextEnabled() const
{
	return m_frameTextItem;
}

void AbstractContent::setFrameText(const QString & text)
{
	if (!m_frameTextItem)
		return;
	m_frameTextItem->setPlainText(text);
}

QString AbstractContent::frameText() const
{
	if (!m_frameTextItem)
		return QString();
	return m_frameTextItem->toPlainText();
}

void AbstractContent::addButtonItem(ButtonItem * button)
{
	m_controlItems.append(button);
	button->setVisible(m_controlsVisible);
	button->setZValue(3.0);
	layoutChildren();
}

void AbstractContent::setRotation(double angle, Qt::Axis axis)
{
	switch (axis)
	{
		case Qt::XAxis: if (m_xRotationAngle == angle) return; m_xRotationAngle = angle; break;
		case Qt::YAxis: if (m_yRotationAngle == angle) return; m_yRotationAngle = angle; break;
		case Qt::ZAxis: if (m_zRotationAngle == angle) return; m_zRotationAngle = angle; break;
	}
	syncToModelItem(modelItem());
	applyRotations();
}

double AbstractContent::rotation(Qt::Axis axis) const
{
	switch (axis)
	{
		case Qt::XAxis: return m_xRotationAngle;
		case Qt::YAxis: return m_yRotationAngle;
		case Qt::ZAxis: return m_zRotationAngle;
	}
	// suppress warnings, can't reach here
	return 0.0;
}

void AbstractContent::setMirrorEnabled(bool enabled)
{
	if (m_mirrorItem && !enabled)
	{
		m_mirrorItem->dispose();
		m_mirrorItem = 0;
	}
	if (enabled && !m_mirrorItem)
	{
		m_mirrorItem = new MirrorItem(this);
		connect(m_gfxChangeTimer, SIGNAL(timeout()), m_mirrorItem, SLOT(sourceChanged()));
		connect(this, SIGNAL(destroyed()), m_mirrorItem, SLOT(deleteLater()));
	}
}

bool AbstractContent::mirrorEnabled() const
{
	return m_mirrorItem;
}

void AbstractContent::ensureVisible(const QRectF & rect)
{
	// keep the center inside the scene rect
	QPointF center = pos();
	if (!rect.contains(center))
	{
		center.setX(qBound(rect.left(), center.x(), rect.right()));
		center.setY(qBound(rect.top(), center.y(), rect.bottom()));
		setPos(center);
	}
}

bool AbstractContent::beingTransformed() const
{
	return m_dirtyTransforming;
}

QWidget * AbstractContent::createPropertyWidget()
{
	return 0;
}

void AbstractContent::setModelItemIsChanging(bool flag)
{
	m_modelItemIsChanging = flag;
}

void AbstractContent::modelItemChanged(QString fieldName, QVariant /*value*/, QVariant /*oldValue*/)
{
	if(DEBUG_ABSTRACTCONTENT)
		qDebug() << "AbstractContent::modelItemChanged(): fieldName:"<<fieldName;
	if(m_modelItem && !m_modelItemIsChanging)
	{
		if(DEBUG_ABSTRACTCONTENT)
			qDebug() << "AbstractContent::modelItemChanged(): syncing from model";
		syncFromModelItem(m_modelItem);
	}
	else
	{
		if(DEBUG_ABSTRACTCONTENT)
			qDebug() << "AbstractContent::modelItemChanged(): item:"<<(modelItem()?modelItem()->itemName() : "NULL")<<" Ignored signal because m_modelItemIsChanging:"<<m_modelItemIsChanging<<", or m_modelItem:"<<(m_modelItem?"<set>":"<not set>");
	}
}

void AbstractContent::setModelItem(AbstractVisualItem *model)
{
//	setModelItemIsChanging(true);

	if(m_modelItem != model)
	{
		if(DEBUG_ABSTRACTCONTENT)
			qDebug() << "AbstractContent::setModelItem(): Received new model item";

		if(m_modelItem != NULL)
		{
			disconnect(m_modelItem, 0, this, 0);
		}

		m_modelItem = model;

		connect(model, SIGNAL(itemChanged(QString, QVariant, QVariant)), this, SLOT(modelItemChanged(QString, QVariant,QVariant)));
	}
}

void AbstractContent::syncFromModelItem(AbstractVisualItem *model)
{
	m_dontSyncToModel = true;

	assert(model);

	if(!modelItem())
		setModelItem(model);

	if(DEBUG_ABSTRACTCONTENT)
		qDebug() << "AbstractContent::syncFromModelItem(): item:"<<(modelItem()?modelItem()->itemName() : "NULL")<<": doing sync";

	QRectF r = model->contentsRect();
	//qDebug() << "AbstractContent::syncFromModelItem(): Setting rect:"<<r;
	resizeContents(r.toRect());

	// Load position coordinates
	setPos(model->pos());
	//qDebug() << "AbstractContent::syncFromModelItem(): Setting pos:"<<model->pos();

	setZValue(model->zValue());
	//qDebug() << "AbstractContent::syncFromModelItem(): item:"<<(modelItem()?modelItem()->itemName() : "NULL")<<": model->zValue(): "<<model->zValue()<<", my zvalue: "<<zValue();
	
	setVisible(model->isVisible());

	quint32 frameClass = model->frameClass();
	setFrame(frameClass ? FrameFactory::createFrame(frameClass) : 0);

	m_xRotationAngle = model->xRotation();
	m_yRotationAngle = model->yRotation();
	m_zRotationAngle = model->zRotation();
	applyRotations();

	setMirrorEnabled(model->mirrorEnabled());

	setOpacity(model->opacity());

	#if QT46_SHADOW_ENAB > 0
	if(model->shadowEnabled())
	{
		QGraphicsDropShadowEffect * shadow = new QGraphicsDropShadowEffect(this);
		shadow->setBlurRadius(model->shadowBlurRadius());
		shadow->setXOffset(model->shadowOffsetX());
		shadow->setYOffset(model->shadowOffsetY());
		shadow->setColor(model->shadowBrush().color());
		setGraphicsEffect(shadow);

	}
	#endif
	
	setSourceOffsetTL(model->sourceOffsetTL());
	setSourceOffsetBR(model->sourceOffsetBR());


	m_dontSyncToModel = false;
}


QString AbstractContent::cacheKey()
{
        return QString().sprintf("%p",static_cast<void*>(modelItem()));
}

void AbstractContent::dirtyCache()
{
        QPixmapCache::remove(cacheKey());
}



void AbstractContent::setOpacity(double d)
{
// 	AbstractVisualItem * model = modelItem();
// 	if(model)
// 	{
// 		qDebug() << "AbstractContent::setOpacity(): Model:"<<model->itemName()<<", ID:"<<model->itemId()<<", my opacity right now:"<<opacity()<<", changing to:"<<model->opacity();
// 	}
	AbstractDisposeable::setOpacity(d);
	//syncToModelItem(modelItem());
}

AbstractVisualItem * AbstractContent::syncToModelItem(AbstractVisualItem * model)
{
        if(m_dontSyncToModel)
	{
                //qDebug("AbstractContent::syncToModelItem: m_inConstructor is true, not syncing");
		return 0;
	}

	// catch any loops back in from the model firing signals about its values changing
	setModelItemIsChanging(true);

	if(!model)
	{
                model = modelItem();
	}

	if(!model)
	{
		setModelItemIsChanging(false);
                qDebug("AbstractContent::syncToModelItem: m_modelItem and 'model' arg was NULL, cannot sync");
		return 0;
	}

	//assert(model);

	if(DEBUG_ABSTRACTCONTENT)
		qDebug() << "AbstractContent::syncToModelItem(): item:"<<model->itemName()<<": doing sync";

	model->setContentsRect(m_contentsRect);

	if(DEBUG_ABSTRACTCONTENT)
		qDebug() << "AbstractContent::syncToModelItem(): item:"<<model->itemName()<<": setPos():"<<pos()<<" - START";
	model->setPos(pos());
	if(DEBUG_ABSTRACTCONTENT)
		qDebug() << "AbstractContent::syncToModelItem(): item:"<<model->itemName()<<": setPos():"<<pos()<<" - DONE";

	model->setZValue(zValue());
	model->setIsVisible(isVisible());
	model->setFrameClass(frameClass());
	model->setOpacity(opacity());

	//qDebug() << "AbstractContent::syncToModelItem: pos=" << model->pos() << ", rect=" << model->contentsRect();

	// save transformation
	const QTransform t = transform();
	if (!t.isIdentity())
	{
		model->setXRotation(m_xRotationAngle);
		model->setYRotation(m_yRotationAngle);
		model->setZRotation(m_zRotationAngle);
	}

	model->setMirrorEnabled(mirrorEnabled());

	model->setSourceOffsetTL(sourceOffsetTL());
	model->setSourceOffsetBR(sourceOffsetBR());
	
	setModelItemIsChanging(false);

	return model;

}
/*
QPixmap AbstractContent::renderContent(const QSize & size, Qt::AspectRatioMode ratio) const
{
    QSize realSize = size;
    if (ratio == Qt::KeepAspectRatio) {
        int hfw = contentHeightForWidth(size.width());
        if (hfw > 1)
            realSize.setHeight(hfw);
    }
    QPixmap pix(realSize);
    pix.fill(Qt::transparent);
    return pix;
}
*/
QRectF AbstractContent::boundingRect() const
{
	//return QRectF(QPointF(0,0),m_frameRect.size());

	double sx = 0;
	double sy = 0;
	qreal penWidth = m_modelItem && m_modelItem->outlineEnabled() ?
		m_modelItem->outlinePen().widthF() : 1.0;

	if(m_modelItem && m_modelItem->shadowEnabled())
	{
		sx = m_modelItem->shadowOffsetX();
		sy = m_modelItem->shadowOffsetY();

		// compensate for pen width - assum shadow position is contents+pen
		sx += sx == 0 ? 0 : sx>0 ? penWidth : -penWidth;
		sy += sy == 0 ? 0 : sy>0 ? penWidth : -penWidth;
	}

	return m_frameRect.adjusted(
		-penWidth/2 + sx<0?sx:0,
		-penWidth/2 + sy<0?sy:0,
		 penWidth   + sx>0?sx:0,
		 penWidth   + sy>0?sy:0
		);
}

void AbstractContent::paint(QPainter * painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
	const bool opaqueContent = contentOpaque();
	const bool drawSelection = RenderOpts::HQRendering ? false : isSelected();
	const QRect frameRect = m_frameRect.toRect();

	if (!m_frame)
	{
		// draw the selection only as done in EmptyFrame.cpp
		if (drawSelection)
		{
			painter->setRenderHint(QPainter::Antialiasing, true);
			painter->setPen(QPen(RenderOpts::hiColor, 1.0));
			// FIXME: this draws OUTSIDE (but inside the safe 2px area)
			painter->drawRect(QRectF(frameRect).adjusted(-0.5, -0.5, +0.5, +0.5));
		}
	}
	else
	{
		// draw the Frame
		m_frame->paint(painter, frameRect, drawSelection, opaqueContent);

		// use clip path for contents, if set
		if (m_frame->clipContents())
			painter->setClipPath(m_frame->contentsClipPath(m_contentsRect));
	}
}

void AbstractContent::selectionChanged(bool /*selected*/)
{
    // nothing to do here.. only used by subclasses
}

void AbstractContent::GFX_CHANGED() const
{
	if (m_gfxChangeTimer && m_mirrorItem)
	    m_gfxChangeTimer->start();
}

void AbstractContent::setControlsVisible(bool visible)
{
	if(m_contextHint != MyGraphicsScene::Editor)
		return;

	m_controlsVisible = visible;
	foreach (CornerItem * corner, m_cornerItems)
		corner->setVisible(visible);
	foreach (ButtonItem * button, m_controlItems)
		button->setVisible(visible);
}

QPixmap AbstractContent::ratioScaledPixmap(const QPixmap * source, const QSize & size, Qt::AspectRatioMode ratio) const
{
	QPixmap scaledPixmap = source->scaled(size, ratio, Qt::SmoothTransformation);
	if (scaledPixmap.size() != size) {
		int offX = (scaledPixmap.width() - size.width()) / 2;
		int offY = (scaledPixmap.height() - size.height()) / 2;
		if (ratio == Qt::KeepAspectRatio)
		{
			QPixmap rightSizePixmap(size);
			rightSizePixmap.fill(Qt::transparent);
			QPainter p(&rightSizePixmap);
			p.drawPixmap(-offX, -offY, scaledPixmap);
			p.end();
			return rightSizePixmap;
		}
		if (ratio == Qt::KeepAspectRatioByExpanding)
		{
			return scaledPixmap.copy(offX, offY, size.width(), size.height());
		}
	}
	return scaledPixmap;
}

int AbstractContent::contentHeightForWidth(int width) const
{
	return width;
}

bool AbstractContent::contentOpaque() const
{
	return false;
}

void AbstractContent::hoverEnterEvent(QGraphicsSceneHoverEvent * /*event*/)
{
	m_hovering = true;
	//setControlsVisible(true);
}

void AbstractContent::hoverLeaveEvent(QGraphicsSceneHoverEvent * /*event*/)
{
	m_hovering = false;
	//setControlsVisible(isSelected() ? true : false);
}

void AbstractContent::dragMoveEvent(QGraphicsSceneDragDropEvent * event)
{
	event->accept();
}

void AbstractContent::dropEvent(QGraphicsSceneDragDropEvent * event)
{
	event->accept();
}

void AbstractContent::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	QGraphicsItem::mousePressEvent(event);
	if (event->button() == Qt::RightButton) {
		setSelected(true);
		emit configureMe(event->scenePos().toPoint());
	}
}

void AbstractContent::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
	QGraphicsItem::mouseReleaseEvent(event);
	syncToModelItem(modelItem());
}






void AbstractContent::keyPressEvent(QKeyEvent * event)
{
	event->accept();
	QSizeF grid = AppSettings::gridSize();
	qreal x = grid.width();
	qreal y = grid.height();
	if(x<5)
		x = 5;
	if(y<5)
		y = 5;
	
	switch(event->key())
	{
		case Qt::Key_Delete:
			emit deleteItem();
			break;
		
		case Qt::Key_Up:
			moveBy(0,-y);
			break;
		case Qt::Key_Down:
			moveBy(0,+y);
			break;
		case Qt::Key_Left:
			moveBy(-x,0);
			break;
		case Qt::Key_Right:
			moveBy(+x,0);
			break;
			
		default:
			break;
	}
}

void AbstractContent::flagKeyboardMotivatedMovement() 
{
	m_kbdMotivated  = true;
}

QVariant AbstractContent::itemChange(GraphicsItemChange change, const QVariant & value)
{
	QVariant retVal;
	bool retValOverride = false;
	// keep the AbstractContent's center inside the scene rect..
	if (change == ItemPositionChange && scene() && AppSettings::gridEnabled())
	{
		if(! (flags() & QGraphicsItem::ItemIsMovable) )
		{
			retVal = QVariant(pos());
 			retValOverride = true;
 		}
 		else
 		{
				
			QPointF newPos = AppSettings::snapToGrid(value.toPointF(),m_kbdMotivated);
			
			// reset the keyboard flag - if another key press comes, it will be set again by the scene
			if(m_kbdMotivated)
				m_kbdMotivated = false;
	
			if (newPos != value.toPointF())
			{
				retVal = QVariant(newPos);
				retValOverride = true;
			}
		}
	}

	// tell subclasses about selection changes
	if (change == ItemSelectedHasChanged)
		selectionChanged(value.toBool());

	//qDebug() << "itemChange(): value:"<<value;

	// changes that affect the mirror item
	//if (m_mirrorItem) {
	switch (change)
	{
		// notify about setPos
		case ItemPositionHasChanged:
			if(DEBUG_ABSTRACTCONTENT)
				qDebug() << "AbstractContent::itemChange: " << modelItem()->itemName() << " ItemPositionHasChanged:"<<value;
			if(m_mirrorItem)
				m_mirrorItem->sourceMoved();

			//syncToModelItem(modelItem());
			break;

		// notify about graphics changes
		case ItemSelectedHasChanged:
			if(DEBUG_ABSTRACTCONTENT)
				qDebug() << "AbstractContent::itemChange: " << modelItem()->itemName() << " ItemSelectedHasChanged:"<<value;
			setControlsVisible(value.toBool() ? true : false);

		case ItemTransformHasChanged:
		case ItemEnabledHasChanged:

		case ItemParentHasChanged:
#if QT_VERSION >= 0x040500
		case ItemOpacityHasChanged:
#endif
			if(DEBUG_ABSTRACTCONTENT)
				qDebug() << "AbstractContent::itemChange: " << modelItem()->itemName() << " ItemTransformHasChanged - ItemOpacityHasChanged:"<<value;
			//syncToModelItem(modelItem());
			GFX_CHANGED();
			break;

		case ItemZValueHasChanged:
			//syncToModelItem(modelItem());
			if(DEBUG_ABSTRACTCONTENT)
				qDebug() << "AbstractContent::itemChange: " << modelItem()->itemName() << " ItemZValueHasChanged:"<<value;
			if(m_mirrorItem)
				m_mirrorItem->setZValue(zValue());
			break;

		case ItemVisibleHasChanged:
			if(DEBUG_ABSTRACTCONTENT)
				qDebug() << "AbstractContent::itemChange: " << modelItem()->itemName() << " ItemVisibleHasChanged:"<<value;
			//syncToModelItem(modelItem());
			if(m_mirrorItem)
				m_mirrorItem->setVisible(isVisible());
			break;

		default:
			break;
	}

	// ..or just apply the value

	QVariant otherVal = QGraphicsItem::itemChange(change, value);
	return retValOverride ? retVal : otherVal;
}

void AbstractContent::slotConfigure()
{
    ButtonItem * item = dynamic_cast<ButtonItem *>(sender());
    if (!item)
        return;
    emit configureMe(item->scenePos().toPoint());
}

void AbstractContent::slotStackFront()
{

    emit changeStack(1);
    syncToModelItem(0);
}

void AbstractContent::slotStackRaise()
{
    emit changeStack(2);
    syncToModelItem(0);
}

void AbstractContent::slotStackLower()
{
    emit changeStack(3);
    syncToModelItem(0);
}

void AbstractContent::slotStackBack()
{
    emit changeStack(4);
    syncToModelItem(0);
}

void AbstractContent::slotSaveAs()
{
	QString fileName = QFileDialog::getSaveFileName(0, tr("Choose the Image file"), QString(), tr("Images (*.jpeg *.jpg *.png *.bmp *.tif *.tiff)"));
	if (fileName.isNull())
		return;
	if (QFileInfo(fileName).suffix().isEmpty())
		fileName += ".png";

	// find out the Transform chain to mirror a rotated item
	QRectF sceneRectF = mapToScene(boundingRect()).boundingRect();
	QTransform tFromItem = transform() * QTransform(1, 0, 0, 1, pos().x(), pos().y());
	QTransform tFromPixmap = QTransform(1, 0, 0, 1, sceneRectF.left(), sceneRectF.top());
	QTransform tItemToPixmap = tFromItem * tFromPixmap.inverted();

	// render on the image
	int iHeight = (int)sceneRectF.height();
	if (m_mirrorItem)
		iHeight += (int)m_mirrorItem->boundingRect().height();
	QImage image((int)sceneRectF.width(), iHeight, QImage::Format_ARGB32);
	image.fill(Qt::transparent);

	// enable hi-q rendering
	bool prevHQ = RenderOpts::HQRendering;
	RenderOpts::HQRendering = true;

	// draw the transformed item onto the pixmap
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setRenderHint(QPainter::SmoothPixmapTransform, true);
	p.setTransform(tItemToPixmap);
	paint(&p, 0, 0);
	if (m_mirrorItem)
	{
		p.resetTransform();
		p.translate(0, (qreal)((int)sceneRectF.height()));
		m_mirrorItem->paint(&p, 0, 0);
	}
	p.end();
	RenderOpts::HQRendering = prevHQ;

	// save image and check errors
	if (!image.save(fileName) || !QFile::exists(fileName))
	{
		QMessageBox::warning(0, tr("File Error"), tr("Error saving the Object to '%1'").arg(fileName));
		return;
	}
}

void AbstractContent::createCorner(CornerItem::CornerPosition corner, bool noRescale)
{
	CornerItem * c = new CornerItem(corner, noRescale, this);
	c->setVisible(m_controlsVisible);
	c->setZValue(999.0);
	c->setToolTip(tr("Drag with Left or Right mouse button.\n - Hold down SHIFT for free resize\n - Hold down CTRL to allow rotation\n - Hold down ALT to snap rotation\n - Double click (with LMB/RMB) to restore the aspect ratio/rotation"));
	m_cornerItems.append(c);
}

void AbstractContent::layoutChildren()
{
	// layout corners
	foreach (CornerItem * corner, m_cornerItems)
		corner->relayout(m_contentsRect);

	// layout buttons even if no frame
	if (!m_frame)
	{
		int right = (int)m_frameRect.right() - 12;
		int bottom = (int)m_frameRect.bottom() + 2; // if no frame, offset the buttons a little on bottom
		foreach (ButtonItem * button, m_controlItems)
		{
			button->setPos(right - button->width() / 2, bottom - button->height() / 2);
			right -= button->width() + 4;
		}
		return;
	}

	// layout all controls
	m_frame->layoutButtons(m_controlItems, m_frameRect.toRect());

	// layout text, if present
	if (m_frameTextItem)
		m_frame->layoutText(m_frameTextItem, m_frameRect.toRect());
}

void AbstractContent::applyRotations()
{
	setTransform(QTransform().rotate(m_yRotationAngle, Qt::XAxis).rotate(m_xRotationAngle, Qt::YAxis).rotate(m_zRotationAngle));
	layoutChildren();
	GFX_CHANGED();
}

void AbstractContent::slotPerspective(const QPointF & sceneRelPoint, Qt::KeyboardModifiers modifiers)
{
	if (modifiers & Qt::ControlModifier)
		return slotClearPerspective();

	double k = 0.2;
	if (modifiers != Qt::NoModifier)
		k = 0.5;
	m_xRotationAngle = qBound(-70.0, -k * sceneRelPoint.x(), 70.0);
	m_yRotationAngle = qBound(-70.0, -k * sceneRelPoint.y(), 70.0);
	applyRotations();
	syncToModelItem(0);
}

void AbstractContent::slotClearPerspective()
{
	m_xRotationAngle = 0;
	m_yRotationAngle = 0;
	applyRotations();
	syncToModelItem(0);
}

void AbstractContent::slotDirtyEnded()
{
	m_dirtyTransforming = false;
	update();
	GFX_CHANGED();
}

void AbstractContent::setSourceOffsetTL(QPointF x)
{
	m_sourceOffsetTL = x;
	update();
}

void AbstractContent::setSourceOffsetBR(QPointF x)
{
	m_sourceOffsetBR = x;
	update();
}
