#include "GLDrawable.h"

#include "GLWidget.h"
#include "GLEditorGraphicsScene.h"
#include "GLVideoDrawable.h"
#include "GLSceneGroup.h"

QAutoDelPropertyAnimation::QAutoDelPropertyAnimation(QObject * target, const QByteArray & propertyName, QObject * parent)
	: QPropertyAnimation(target,propertyName,parent)
{
	m_originalPropValue = target->property(propertyName.constData());
// 	qDebug() << "QAutoDelPropertyAnimation: Anim STARTED for "<<propertyName.constData()<<", got orig value: "<<m_originalPropValue;
	//connect(this, SIGNAL(finished()), this, SLOT(resetProperty()));
}

void QAutoDelPropertyAnimation::resetProperty()
{
// 	qDebug() << "QAutoDelPropertyAnimation: Anim FINISHED for "<<propertyName().constData()<<", setting value: "<<m_originalPropValue;
	targetObject()->setProperty(propertyName().constData(), m_originalPropValue);
}


// class GLDrawable
GLDrawable::GLDrawable(QObject *parent)
	: QObject(parent)
	, QGraphicsItem()
	, m_glw(0)
	, m_controlsVisible(false)
	, m_zIndex(0)
	, m_zIndexModifier(0)
	, m_opacity(1)
	, m_isVisible(true)
	, m_animFinished(true)
	, m_originalOpacity(-1)
	, m_animationsEnabled(true)
	, m_alignment(Qt::AlignAbsolute)
	, m_inAlignment(false)
	, m_alignedSizeScale(1.)
	, m_animPendingGlWidget(false)
	, m_rotationPoint(.5,.5,.5) // rotate around center by default
	, m_lockVisibleSetter(false)
	, m_id(-1)
	, m_idLoaded(false)
	, m_isUserControllable(false)
	, m_selected(false)
	, m_fadeIn(true)
	, m_fadeOut(true)
	, m_fadeInLength(300)
	, m_fadeOutLength(300)
	, m_playlist(0)
	, m_scene(0)
{
	// QGraphicsItem
	{
// 		// customize item's behavior
 		setFlag(QGraphicsItem::ItemIsFocusable, true);
		#if QT_VERSION >= 0x040600
 			setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
		#endif
// 		// allow some items (eg. the shape controls for text) to be shown
 		setFlag(QGraphicsItem::ItemClipsChildrenToShape, false);
		setAcceptHoverEvents(true);

		bool noRescale=false;

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

		//setControlsVisible(false);
		editingModeChanged(false);
	}

	updateAnimValues();

	m_playlist = new GLDrawablePlaylist(this);
}

GLDrawable::~GLDrawable()
{
	qDeleteAll(m_cornerItems);
	m_cornerItems.clear();
}

void GLDrawable::editingModeChanged(bool flag)
{
	if(!flag)
	{
		setFlag(QGraphicsItem::ItemIsMovable, false);
		setFlag(QGraphicsItem::ItemIsSelectable, false);
		setControlsVisible(false);
	}
	else
	{
		setFlag(QGraphicsItem::ItemIsMovable, true);
		setFlag(QGraphicsItem::ItemIsSelectable, true);
	}

	updateGL();
}

void GLDrawable::updateAnimValues()
{
	resetAllAnimations();

	if(m_fadeIn)
		addShowAnimation(GLDrawable::AnimFade,m_fadeInLength);

	if(m_fadeOut)
		addHideAnimation(GLDrawable::AnimFade,m_fadeOutLength);
}

void GLDrawable::setFadeIn(bool f)
{
	m_fadeIn = f;
	updateAnimValues();
}

void GLDrawable::setFadeOut(bool f)
{
	m_fadeOut = f;
	updateAnimValues();
}

void GLDrawable::setFadeInLength(int f)
{
	m_fadeInLength = f;
	updateAnimValues();
}

void GLDrawable::setFadeOutLength(int f)
{
	m_fadeOutLength = f;
	updateAnimValues();
}

int GLDrawable::id()
{
	if(m_id < 0)
	{
		QSettings set;
		m_id = set.value("gldrawable/id-counter",0).toInt() + 1;
		set.setValue("gldrawable/id-counter",m_id);
	}

	return m_id;
}

void GLDrawable::setItemName(const QString& name)
{
	emit itemNameChanging(name);
	m_itemName = name;
	setObjectName(qPrintable(name));
	propertyWasChanged("itemName",name);
}

void GLDrawable::setUserControllable(bool flag)
{
	m_isUserControllable = flag;
	propertyWasChanged("userControllable",flag);
}

void GLDrawable::propertyWasChanged(const QString& propName, const QVariant& value)
{
	emit propertyChanged(propName,value);
}

void GLDrawable::updateGL(bool now)
{
	if(m_glw)
		m_glw->updateGL(now);
	else
		update(); // compat with QGraphicsItem
}

void GLDrawable::show()
{
	setVisible(true);
}

void GLDrawable::hide()
{
	setVisible(false);
}

void GLDrawable::setHidden(bool flag)
{
	setVisible(!flag);
}

void GLDrawable::setVisible(bool flag)
{
	//qDebug() << "QGraphicsItem::setVisible: "<<flag;
	QGraphicsItem::setVisible(flag);
	//qDebug() << "GLDrawable::setVisible(): "<<(QObject*)this<<": Flag:"<<flag<<" mark1";

	if(m_lockVisibleSetter)
		return;
	m_lockVisibleSetter = true;
	
	//qDebug() << "GLDrawable::setVisible(): "<<(QObject*)this<<": Flag:"<<flag<<" mark2";	

	//if(m_animFinished &&
	if(m_isVisible == flag)
	{
		//qDebug() << "GLDrawable::setVisible(): "<<(QObject*)this<<": Flag:"<<flag<<" mark3";
		m_lockVisibleSetter = false;
		emit isVisible(flag);
		return;
	}

	//qDebug() << "GLDrawable::setVisible(): "<<(QObject*)this<<": Flag:"<<flag<<" mark4";
	if(flag)
		emit isVisible(flag);
	forceStopAnimations();

	m_animDirection = flag;

	if(!m_animations.isEmpty())
		startAnimations();
	else
	{
		m_isVisible = flag;
		if(!flag)
			emit isVisible(flag);
	}
	
	//qDebug() << "GLDrawable::setVisible(): "<<(QObject*)this<<": Flag:"<<flag<<" mark5";

	m_lockVisibleSetter = false;

}

void GLDrawable::forceStopAnimations()
{
	if(!m_animFinished)
	{
		foreach(QAutoDelPropertyAnimation *ani, m_runningAnimations)
		{
			ani->stop();
			//qDebug() << (QObject*)this<<"GLDrawable::setVisible while anim running, resetting property on "<<ani->propertyName().constData();
			ani->resetProperty();
			ani->deleteLater();
		}
		m_runningAnimations.clear();
		animationFinished();
	}
}

void GLDrawable::startAnimations()
{
	if(!m_glw)
	{
		m_animPendingGlWidget = true;
		return;
	}

	//qDebug() << "GLDrawable::startAnimations(): "<<(QObject*)this<<": At start, rect:"<<rect()<<", opacity:"<<opacity()<<", m_animDirection:"<<m_animDirection<<", m_animationsEnabled:"<<m_animationsEnabled;

	if(m_animDirection)
		m_isVisible = m_animDirection;

	m_animFinished = false;

	bool hasFade = false;
	bool hasAnimation = false;
	if(m_animationsEnabled)
	{
		foreach(GLDrawable::AnimParam p, m_animations)
			if(( m_animDirection && p.cond == GLDrawable::OnShow) ||
			   (!m_animDirection && p.cond == GLDrawable::OnHide))
			{
				if(p.type == GLDrawable::AnimFade)
					hasFade = true;
				startAnimation(p);
				hasAnimation = true;
			}
	}

	//if(m_animDirection && m_isVisible && !hasFade)
	//	setOpacity(1.0);

	if(!hasAnimation)
	{
		m_isVisible = m_animDirection;
		if(!m_animDirection)
		{
			m_lockVisibleSetter = true;
			emit isVisible(m_animDirection);
			m_lockVisibleSetter = false;
		}
	}

}

bool GLDrawable::setAnimationsEnabled(bool flag)
{
	bool oldValue = m_animationsEnabled;
	m_animationsEnabled = flag;
	return oldValue;
}

void GLDrawable::resetAllAnimations()
{
	m_animations.clear();
}

GLDrawable::AnimParam & GLDrawable::addShowAnimation(AnimationType value, int length)
{
	GLDrawable::AnimParam p;
	p.cond = GLDrawable::OnShow;
	p.type = value;
	p.startDelay = 0;
	p.length = length;
	p.curve = value == GLDrawable::AnimFade ? QEasingCurve::Linear : QEasingCurve::OutCubic;
	m_animations << p;

	//qDebug() << "GLDrawable::addShowAnimation:"<<(QObject*)this<<value<<", len:"<<length;
	return m_animations.last();
}

GLDrawable::AnimParam & GLDrawable::addHideAnimation(AnimationType value, int length)
{
	GLDrawable::AnimParam p;
	p.cond = GLDrawable::OnHide;
	p.type = value;
	p.startDelay = 0;
	p.length = length;
	p.curve = value == GLDrawable::AnimFade ? QEasingCurve::Linear : QEasingCurve::InCubic;
	m_animations << p;

	//qDebug() << "GLDrawable::addHideAnimation:"<<(QObject*)this<<value<<", len:"<<length;
	return m_animations.last();
}

void GLDrawable::removeAnimation(GLDrawable::AnimParam p)
{
	m_animations.removeAll(p);
}


bool operator==(const GLDrawable::AnimParam&a, const GLDrawable::AnimParam&b)
{
	return a.cond == b.cond &&
		a.type == b.type &&
		a.startDelay == b.startDelay &&
		a.length == b.length &&
		a.curve == b.curve;
};

void GLDrawable::startAnimation(const GLDrawable::AnimParam& p)
{
	QSizeF viewport = canvasSize();
	bool inFlag = m_animDirection;

	QAutoDelPropertyAnimation *ani = 0;

	switch(p.type)
	{
		case GLDrawable::AnimFade:
// 			if(m_originalOpacity>-1)
// 				setOpacity(m_originalOpacity);
// 			else
				m_originalOpacity = opacity();
			//qDebug() << "GLDrawable::startAnimation(): m_originalOpacity:"<<m_originalOpacity;
			ani = new QAutoDelPropertyAnimation(this, "opacity");
			ani->setStartValue(inFlag ? 0.0 : opacity());
			ani->setEndValue(inFlag ?   opacity() : 0.0);
			break;

		case GLDrawable::AnimZoom:
			ani = setupRectAnimation(m_rect.adjusted(-viewport.width(),-viewport.height(),viewport.width(),viewport.height()),inFlag);
			break;
		case GLDrawable::AnimSlideTop:
			ani = setupRectAnimation(m_rect.adjusted(0,-viewport.height(),0,-viewport.height()),inFlag);
			break;
		case GLDrawable::AnimSlideBottom:
			ani = setupRectAnimation(m_rect.adjusted(0,viewport.height(),0,viewport.height()),inFlag);
			break;
		case GLDrawable::AnimSlideLeft:
			ani = setupRectAnimation(m_rect.adjusted(-viewport.width(),0,-viewport.width(),0),inFlag);
			break;
		case GLDrawable::AnimSlideRight:
			ani = setupRectAnimation(m_rect.adjusted(viewport.width(),0,viewport.width(),0),inFlag);
			break;

		default:
			break;
	}

	if(ani)
	{
		ani->setEasingCurve(p.curve); //inFlag ? QEasingCurve::OutCubic : QEasingCurve::InCubic);

		int len = p.length;
		if(GLVideoDrawable *vid = dynamic_cast<GLVideoDrawable*>(this))
			len = vid->xfadeLength();

		ani->setDuration(len);

		//qDebug() << "GLDrawable::startAnimation: "<<(QObject*)this<<" type:"<<p.type<<", length:"<<len<<", curve:"<<p.curve.type()<<", delay:"<<p.startDelay;

		connect(ani, SIGNAL(finished()), this, SLOT(animationFinished()));

		if(p.startDelay > 0)
		{
			QTimer *timer = new QTimer();
			timer->setInterval(p.startDelay);

			connect(timer, SIGNAL(timeout()), ani, SLOT(start()));
			connect(timer, SIGNAL(timeout()), timer, SLOT(deleteLater()));
			timer->setSingleShot(true);
			timer->start();
		}
		else
		{
			ani->start();
		}

		//qDebug() << "GLDrawable::startAnimation(): "<<this<<": added animation:"<<ani;

		m_runningAnimations << ani;
	}
}


QAutoDelPropertyAnimation * GLDrawable::setupRectAnimation(const QRectF& otherRect, bool inFlag)
{
	QAutoDelPropertyAnimation *ani = new QAutoDelPropertyAnimation(this, "rect");

	m_originalRect = m_rect; // will be restored when anim done

	ani->setEndValue(inFlag   ? m_rect : otherRect);
	ani->setStartValue(inFlag ? otherRect : m_rect);

	//qDebug() << "GLDrawable::startRectAnimation: start:"<<(inFlag ? otherRect : m_rect)<<", end:"<<(inFlag   ? m_rect : otherRect)<<", other:"<<otherRect<<", duration:"<<duration<<", inFlag:"<<inFlag;
	return ani;
}

void GLDrawable::animationFinished()
{
	QAutoDelPropertyAnimation *ani = dynamic_cast<QAutoDelPropertyAnimation*>(sender());
	//qDebug() << "GLDrawable::animationFinished(): "<<this<<": animation finished:"<<ani;
	if(ani)
	{
// 		qDebug() << "GLDrawable::animationFinished(): Got ANI";
		m_runningAnimations.removeAll(ani);
		m_finishedAnimations.append(ani);
		if(m_runningAnimations.isEmpty())
		{
// 			qDebug() << "GLDrawable::animationFinished(): All animations finished";
			m_isVisible = m_animDirection;
			m_runningAnimations.clear();
			m_animFinished = true;
			updateGL();

			foreach(QAutoDelPropertyAnimation *ani, m_finishedAnimations)
			{
				ani->resetProperty();
				//ani->deleteLater();
			}
			qDeleteAll(m_finishedAnimations);
			m_finishedAnimations.clear();

			if(!m_isVisible)
			{
				m_lockVisibleSetter = true;
				emit isVisible(m_isVisible);
				m_lockVisibleSetter = false;
			}
		}
		else
		{
			//qDebug() << "GLDrawable::startAnimation(): "<<this<<": "<<ani<<": not all empty: "<<m_runningAnimations.size();
		}

	}
	else
	{
		m_runningAnimations.clear();
		foreach(QAutoDelPropertyAnimation *ani, m_finishedAnimations)
		{
			ani->resetProperty();
			//ani->deleteLater();
		}
		qDeleteAll(m_finishedAnimations);
		m_finishedAnimations.clear();
		m_animFinished = true;
		m_isVisible = m_animDirection;
		updateGL();
	}
}

void GLDrawable::setRect(const QRectF& rect)
{
	if(m_rect == rect)
		return;

	// Notify QGraphicsItem of upcoming change
	prepareGeometryChange();

	m_rect = rect;
	if(m_rect.width()<0)
		m_rect.setWidth(0);
	if(m_rect.height()<0)
		m_rect.setHeight(0);
 	//qDebug() << "GLDrawable::setRect(): "<<this<<", pos:"<<rect.topLeft();
	//qDebug() << "GLDrawable::setRect: "<<this<<rect;
	//qDebug() << "GLDrawable::setRect: "<<rect;
// 	qDebug() << "GLDrawable::setRect: size: "<<rect.size();

	drawableResized(rect.size());
	emit drawableResized(rect.size());
// 	if(!m_inAlignment)
// 		updateAlignment();
	updateGL();
	layoutChildren();

	setPos(rect.topLeft());
}

void GLDrawable::setZIndexModifier(double mod)
{
	m_zIndexModifier = mod;
	emit zIndexChanged(zIndex());
}

double GLDrawable::zIndex()
{
	if(m_zIndexModifier != 0)
	{
		double fract = m_zIndex / (Z_MAX/2) + 0.5;
		double final = fract * m_zIndexModifier;
		//qDebug() << "GLDrawable::zIndex: m_zIndexModifier:"<<m_zIndexModifier<<", m_zIndex:"<<m_zIndex<<", final: "<<final;

		return final;
	}

	return m_zIndex;
}

double GLDrawable::zIndexModifier()
{
	return m_zIndexModifier;
}

double GLDrawable::opacity()
{
	if(m_scene)
		return m_opacity * m_scene->opacity();
	else
		return m_opacity;
}

void GLDrawable::setZIndex(double z)
{
	//qDebug() << "GLDrawable::setZIndex: "<<this<<", zIndex:"<<z;
	m_zIndex = z;
	setZValue((int)z); // for QGraphicsItem compat

	emit zIndexChanged(z);
	updateGL();
	propertyWasChanged("zIndex",z);
}

void GLDrawable::setOpacity(double o)
{
	//qDebug() << "GLDrawable::setOpacity: "<<this<<", opacity:"<<o;
	m_opacity = o;
	updateGL();
	propertyWasChanged("opacity",o);
}

void GLDrawable::setAlignment(Qt::Alignment value, bool animate, int animLength, QEasingCurve animCurve)
{
	m_alignment = value;
  	//qDebug() << "GLDrawable::setAlignment(): "<<this<<", m_alignment:"<<m_alignment;
	updateAlignment(animate, animLength, animCurve);
}

void GLDrawable::setAlignment(int value)
{
	m_alignment = (Qt::Alignment)value;
// 	qDebug() << "GLDrawable::setAlignment(int): "<<this<<objectName()<<", m_alignment:"<<m_alignment;
	updateAlignment();
}

void GLDrawable::setInsetTopLeft(const QPointF& value)
{
	m_insetTopLeft = value;
// 	qDebug() << "GLDrawable::setInsetTopLeft(): "<<value;
	updateAlignment();
}

void GLDrawable::setInsetBottomRight(const QPointF& value)
{
	m_insetBottomRight = value;
// 	qDebug() << "GLDrawable::setInsetBottomRight(): "<<value;
	updateAlignment();
}

void GLDrawable::setAlignedSizeScale(qreal scale)
{
	m_alignedSizeScale = scale;
	//qDebug() << "GLDrawable::setAlignedSizeScale: New scale: "<<m_alignedSizeScale;
	updateAlignment();
}

void GLDrawable::updateAlignment(bool animateRect, int animLength, QEasingCurve animCurve)
{
	m_inAlignment = true;

	QSizeF size = naturalSize();
// 	if(!size.isValid())
// 		size = m_rect.size();

	if(!m_glw)
	{
// 		qDebug() << "GLDrawable::updateAlignment(): "<<this<<objectName()<<", no m_glw, can't align";
		m_inAlignment = false;
		return;
	}

	//qDebug() << "GLDrawable::updateAlignment(): "<<this<<", m_animFinished:"<<m_animFinished<<", m_animDirection:"<<m_animDirection;;
// 	bool restartAnimations = false;
	if(!m_animFinished && m_animDirection)
	{
		// This will force a stop of the animations, reset properties.
		// We'll restart animations at the end
		forceStopAnimations();
		//qDebug() << "GLDrawable::updateAlignment(): restoring opacity:"<<m_originalOpacity;
		if(m_originalOpacity>-1)
			setOpacity(m_originalOpacity);
		//restartAnimations = true;
	}


//   	qDebug() << "GLDrawable::updateAlignment(): "<<this<<", size:"<<size<<", m_alignment:"<<m_alignment;

	QRectF rect;
	qreal   x = m_rect.left(),
	        y = m_rect.top(),
		w = m_rect.width(),
		h = m_rect.height();
	qreal vw = canvasSize().width(),
	      vh = canvasSize().height();

	//qDebug() << "GLDrawable::updateAlignment: w:"<<w<<", h:"<<h<<", alignedSizeScale:"<<alignedSizeScale();

	//qDebug() << "GLDrawable::updateAlignment(): "<<this<<" m_alignment: "<<m_alignment ;

	if(!m_alignment || (m_alignment & Qt::AlignAbsolute) == Qt::AlignAbsolute)
	{
// 		x = vw * m_leftPercent;
// 		y = vh * m_topPercent;
// // 		qDebug() << "GLDrawable::updateAlignment(): "<<this<<objectName()<<", absolute: m_leftPercent:"<<m_leftPercent<<", x:"<<x;
  		//qDebug() << "GLDrawable::updateAlignment(): "<<this<<", [AlignAbsolute]: m_rect:"<<m_rect;
//
// 		double b = vh * m_bottomPercent;
// 		double r = vw * m_rightPercent;
// 		h = b - y;
// 		w = r - x;

// 		x = m_rect.left();
// 		y = m_rect.top();
// 		w = m_rect.width();
// 		h = m_rect.height();
	}
	else
	{
		x = 0;
		y = 0;
		w = size.width()  * alignedSizeScale();
		h = size.height() * alignedSizeScale();

		//qDebug() << "GLDrawable::updateAlignment(): "<<this<<", [NON ABSOLUTE]";

		if ((m_alignment & Qt::AlignHCenter) == Qt::AlignHCenter)
		{
			//qDebug() << "GLDrawable::updateAlignment(): "<<this<<", [AlignHCenter]";
			x = (vw - w)/2 + m_insetTopLeft.x() - m_insetBottomRight.x();
// 			qDebug() << "GLDrawable::updateAlignment(): "<<this<<objectName()<<", ALIGN: H Center, x:"<<x;

		}

		if ((m_alignment & Qt::AlignVCenter) == Qt::AlignVCenter)
		{
			//qDebug() << "GLDrawable::updateAlignment(): "<<this<<", [AlignVCenter]";
			y = (vh - h)/2 + m_insetTopLeft.y() - m_insetBottomRight.y();
// 			qDebug() << "GLDrawable::updateAlignment(): "<<this<<objectName()<<", ALIGN: V Center, y:"<<y;
		}

		if ((m_alignment & Qt::AlignBottom) == Qt::AlignBottom)
		{
			//qDebug() << "GLDrawable::updateAlignment(): "<<this<<", [AlignBottom]";
			y = vh - h - m_insetBottomRight.y();
// 			qDebug() << "GLDrawable::updateAlignment(): "<<this<<objectName()<<", ALIGN: Bottom, y:"<<y;
		}

		if ((m_alignment & Qt::AlignRight) == Qt::AlignRight)
		{
			//qDebug() << "GLDrawable::updateAlignment(): "<<this<<", [AlignRight]";
			x = vw - w - m_insetBottomRight.x();
// 			qDebug() << "GLDrawable::updateAlignment(): "<<this<<objectName()<<", ALIGN: Right, x:"<<x;
		}

		if ((m_alignment & Qt::AlignTop) == Qt::AlignTop)
		{
			//qDebug() << "GLDrawable::updateAlignment(): "<<this<<", [AlignTop]";
			y = m_insetTopLeft.y();
// 			qDebug() << "GLDrawable::updateAlignment(): "<<this<<objectName()<<", ALIGN: Top, y:"<<y;
		}

		if ((m_alignment & Qt::AlignLeft) == Qt::AlignLeft)
		{
			//qDebug() << "GLDrawable::updateAlignment(): "<<this<<", [AlignLeft]";
			x = m_insetTopLeft.x();
// 			qDebug() << "GLDrawable::updateAlignment(): "<<this<<objectName()<<", ALIGN: Left, x:"<<x;
		}
	}

	rect = QRectF(x,y,w,h);

	if((m_alignment & Qt::AlignAbsolute) == Qt::AlignAbsolute)
	{
		//qDebug() << "GLDrawable::updateAlignment(): "<<this<<", final rect: "<<rect;
	}

	if(QPropertyAnimation *ani = propAnim("rect"))
	{
// 		qDebug() << "GLDrawable::updateAlignment(): "<<this<<", Killing running rect animation...";
		ani->stop();
		ani->deleteLater();
		m_propAnims.remove("rect");
// 		qDebug() << "GLDrawable::updateAlignment(): "<<this<<", done killing.";
	}

	if(animateRect)
	{
		QPropertyAnimation *animation = new QPropertyAnimation(this, "rect");
		animation->setDuration(animLength);
		animation->setEasingCurve(animCurve);
		animation->setEndValue(rect);
		animation->start(QAbstractAnimation::DeleteWhenStopped);

		registerPropAnim("rect", animation);
// 		qDebug() << "GLDrawable::updateAlignment(): "<<this<<", setting rect, animating change to:"<<rect;
	}
	else
	{
// 		qDebug() << "GLDrawable::updateAlignment(): "<<this<<", setting rect, no anim to:"<<rect;
		setRect(rect);
	}

	m_inAlignment = false;

// 	if(restartAnimations)
// 	{
// 		startAnimations();
// 	}
}

void GLDrawable::setGLWidget(GLWidget* w)
{
	//qDebug() << "GLDrawable::setGLWidget(): "<<this<<", w:"<<w;
	m_glw = w;
	if(!w)
		return;

	if(m_rect.isNull())
		m_rect = QRectF(QPointF(0,0),canvasSize());

	updateAlignment();

	if(m_animPendingGlWidget)
	{
		m_animPendingGlWidget = false;
		startAnimations();
	}
}

QSizeF GLDrawable::canvasSize()
{
	if(m_canvasSize.isNull() || !m_canvasSize.isValid())
		return m_glw ? m_glw->canvasSize() : QSizeF(1000.,750.);
	return m_canvasSize;
}

void GLDrawable::setCanvasSize(const QSizeF& size)
{
	m_canvasSize = size;
	canvasResized(size);
}

void GLDrawable::viewportResized(const QSize& /*newSize*/)
{
	updateAlignment();
}

void GLDrawable::canvasResized(const QSizeF& /*newSize*/)
{
	updateAlignment();
}

void GLDrawable::drawableResized(const QSizeF& /*newSize*/)
{
	// NOOP
}

void GLDrawable::paintGL()
{
	// NOOP
}

void GLDrawable::initGL()
{
	// NOOP
}

void GLDrawable::paint(QPainter * painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
	GLEditorGraphicsScene *castedScene = dynamic_cast<GLEditorGraphicsScene*>(scene());
	if(!castedScene)
		return;
	if(isSelected() && castedScene->isEditingMode())
	{
		painter->setRenderHint(QPainter::Antialiasing, true);
		painter->setPen(QPen(qApp->palette().color(QPalette::Highlight), 1.0));
		painter->setBrush(QBrush());
		// FIXME: this draws OUTSIDE (but inside the safe 2px area)
		painter->drawRect(QRectF(boundingRect()).adjusted(-0.5, -0.5, +0.5, +0.5));
	}
}


QByteArray GLDrawable::AnimParam::toByteArray()
{
	QByteArray array;
	QDataStream b(&array, QIODevice::WriteOnly);

	b << QVariant(cond);
	b << QVariant(type);
	b << QVariant(startDelay);
	b << QVariant(length);
	b << QVariant(curve.type());

	return array;
}

void GLDrawable::AnimParam::fromByteArray(QByteArray array)
{
	QDataStream b(&array, QIODevice::ReadOnly);
	QVariant x;

	b >> x; cond = (GLDrawable::AnimCondition)x.toInt();
	b >> x; type = (GLDrawable::AnimationType)x.toInt();
	b >> x; startDelay = x.toInt();
	b >> x; length = x.toInt();
	b >> x; int curveType = x.toInt();
	curve = (QEasingCurve::Type)curveType;
}

void GLDrawable::setTranslation(QVector3D value)
{
	m_translation = value;
	updateGL();
}

void GLDrawable::setRotation(QVector3D value)
{
	m_rotation = value;
	updateGL();
}

// Expressed in a range of 0..1 as a percentage of the relevant size (e.g. (.5,.5,.5) means rotate around center, the default value)
void GLDrawable::setRotationPoint(QVector3D value)
{
	m_rotationPoint = value;
	updateGL();
}

QPropertyAnimation *GLDrawable::propAnim(const QString& prop)
{
	if(m_propAnims.contains(prop))
		return m_propAnims[prop];
	return 0;
}

void GLDrawable::registerPropAnim(const QString& prop, QPropertyAnimation *anim)
{
	m_propAnims[prop] = anim;
	connect(anim, SIGNAL(finished()), this, SLOT(propAnimFinished()));
}

void GLDrawable::propAnimFinished()
{
	QPropertyAnimation *ani = dynamic_cast<QPropertyAnimation*>(sender());
	if(!ani)
		return;

	QString prop(ani->propertyName().constData());
	m_propAnims.remove(prop);
}

void GLDrawable::fromByteArray(QByteArray& array)
{
	bool animEnabled = setAnimationsEnabled(false);

	QDataStream stream(&array, QIODevice::ReadOnly);
	QVariantMap map;
	stream >> map;

	//qDebug() << "LiveScene::fromByteArray(): "<<map;
	if(map.isEmpty())
	{
		qDebug() << "Error: GLDrawable::fromByteArray(): Map is empty, unable to load item.";
		return;
	}

	loadPropsFromMap(map);

	setAnimationsEnabled(animEnabled);


}

void GLDrawable::loadPropsFromMap(const QVariantMap& map, bool onlyApplyIfChanged)
{
	bool vis = false;

	// So we dont have to engineer our own method of tracking
	// properties, just assume all inherited objects delcare the relevant
	// properties using Q_PROPERTY macro
	const QMetaObject *metaobject = metaObject();
	int count = metaobject->propertyCount();
	for (int i=0; i<count; ++i)
	{
		QMetaProperty metaproperty = metaobject->property(i);
		const char *name = metaproperty.name();
		QVariant value = map[name];

		//if(QString(name) == "rect")
			//qDebug() << "GLDrawable::loadPropsFromMap():"<<(QObject*)this<<": i:"<<i<<", count:"<<count<<", prop:"<<name<<", value:"<<value;


		// Hold setting visiblility flag till last so that way any properties that affect
		// animations are set BEFORE animations start!
		if(QString(name) == "isVisible")
		{
			vis = value.toBool();
			//vis = true;
		}
		else
		if(QString(name) == "id")
		{
			// m_layerId is only set ONCE by this method, overwriting any ID assigned at creation time
			if(!m_idLoaded && value.isValid())
			{
				m_idLoaded = true;
				m_id = value.toInt();
			}
		}
		else
		{

			if(value.isValid())
			{
				if(onlyApplyIfChanged)
				{
					if(property(name) != value)
					{
 						//qDebug() << "LiveLayer::loadPropsFromMap():"<<this<<": [onlyApplyIfChanged] i:"<<i<<", count:"<<count<<", prop:"<<name<<", value:"<<value;
						setProperty(name,value);
					}
				}
				else
				{
					//if(QString(name) == "alignment")
					//	qDebug() << "LiveLayer::loadPropsFromMap():"<<this<<": i:"<<i<<", count:"<<count<<", prop:"<<name<<", value:"<<value<<" (calling set prop)";

					setProperty(name,value);
					//m_props[name] = value;
				}
			}
			else
				qDebug() << "GLDrawable::loadPropsFromMap: Unable to load property for "<<name<<", got invalid property from map";
		}
	}

	QByteArray ba = map["playlist"].toByteArray();
	m_playlist->fromByteArray(ba);

	//qDebug() << "GLDrawable::loadPropsFromMap():"<<(QObject*)this<<": *** Setting visibility to "<<vis;
	if(!onlyApplyIfChanged || isVisible() != vis)
		setVisible(vis);
}

QByteArray GLDrawable::toByteArray()
{
	QByteArray array;
	QDataStream stream(&array, QIODevice::WriteOnly);
	stream << propsToMap();
	return array;
}

QVariantMap GLDrawable::propsToMap()
{
	QVariantMap map;
	// So we dont have to engineer our own method of tracking
	// properties, just assume all inherited objects delcare the relevant
	// properties using Q_PROPERTY macro
	const QMetaObject *metaobject = metaObject();
	int count = metaobject->propertyCount();
	for (int i=0; i<count; ++i)
	{
		QMetaProperty metaproperty = metaobject->property(i);
		const char *name = metaproperty.name();
		QVariant value = property(name);

		//if(name == "rect")
			//qDebug() << "GLDrawable::propsToMap():"<<(QObject*)this<<": prop:"<<name<<", value:"<<value;

		map[name] = value;
	}

	map["playlist"] = m_playlist->toByteArray();

	return map;
}

QVariant GLDrawable::itemChange(GraphicsItemChange change, const QVariant & value)
{
// 	//if(change == ItemSceneChange)
// 		//qDebug() << "GLDrawable::itemChange: change:"<<change<<", value:"<<value;
// 
// 	QVariant retVal;
// 	bool retValOverride = false;
// // 	if(change != ItemFlagsChange &&
// // 		change != ItemFlagsHaveChanged &&
// // 		change != ItemChildAddedChange)
// // 		qDebug() << "GLDrawable::itemChange: change:"<<change<<", value:"<<value;
// 
// 	switch (change)
// 	{
// 		// notify about setPos
// 		case ItemPositionHasChanged:
// // 			if(DEBUG_ABSTRACTCONTENT)
// // 				qDebug() << "AbstractContent::itemChange: " << modelItem()->itemName() << " ItemPositionHasChanged:"<<value;
// 
// 			//syncToModelItem(modelItem());
// 			break;
// 
// 		// notify about graphics changes
// 		case ItemSelectedHasChanged:
// // 			if(DEBUG_ABSTRACTCONTENT)
// // 				qDebug() << "AbstractContent::itemChange: " << modelItem()->itemName() << " ItemSelectedHasChanged:"<<value;
// 			//setControlsVisible(value.toBool() ? true : false);
// 
// 		case ItemTransformHasChanged:
// 		case ItemEnabledHasChanged:
// 
// 		case ItemParentHasChanged:
// #if QT_VERSION >= 0x040500
// 		case ItemOpacityHasChanged:
// #endif
// 			break;
// 
// 		case ItemZValueHasChanged:
// 			break;
// 
// 		case ItemVisibleHasChanged:
// 			break;
// 		
// 		case ItemSceneChange:
// 			break;
// 		default:
// 			break;
// 	}
// 
// 	// ..or just apply the value
// 
// 	QVariant otherVal = QGraphicsItem::itemChange(change, value);
// 	return retValOverride ? retVal : otherVal;

	return QGraphicsItem::itemChange(change, value);
}


void GLDrawable::setControlsVisible(bool visible)
{
// 	if(m_contextHint != MyGraphicsScene::Editor)
// 		return;
	GLEditorGraphicsScene *castedScene = dynamic_cast<GLEditorGraphicsScene*>(scene());
	if(!castedScene || !castedScene->isEditingMode())
		visible = false;

	m_controlsVisible = visible;
	foreach (CornerItem * corner, m_cornerItems)
		corner->setVisible(visible);
}


void GLDrawable::createCorner(CornerItem::CornerPosition corner, bool noRescale)
{
	CornerItem * c = new CornerItem(corner, noRescale, this);
	c->setVisible(m_controlsVisible);
	c->setZValue(999.0);
	c->setToolTip(tr("Drag with Left or Right mouse button.\n - Hold down SHIFT for free resize\n - Hold down CTRL to allow rotation\n - Hold down ALT to snap rotation\n - Double click (with LMB/RMB) to restore the aspect ratio/rotation"));
	m_cornerItems.append(c);
}

void GLDrawable::layoutChildren()
{
	// layout corners
	foreach (CornerItem * corner, m_cornerItems)
		corner->relayout(boundingRect().toRect()); //m_contentsRect);
}

void GLDrawable::setSelected(bool flag)
{
	//qDebug() << "GLDrawable::setSelected: "<<(QObject*)this<<" flag:"<<flag;
	m_selected = flag;
	setControlsVisible(flag);
	if(flag)
	{
		GLEditorGraphicsScene *gls = dynamic_cast<GLEditorGraphicsScene*>(scene());
		if(gls)
			gls->itemSelected(this);
	}
}

void GLDrawable::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	//qDebug() << "GLDrawable::mousePressEvent";
	QGraphicsItem::mousePressEvent(event);
	if(!isSelected())
		setSelected(true);

}

void GLDrawable::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
	//qDebug() << "GLDrawable::mouseMoveEvent";
	QGraphicsItem::mouseMoveEvent(event);
	if(pos() != rect().topLeft())
	{
 		QSizeF sz(10.,10.);// = AppSettings::gridSize();
 		bool halfGrid = false;

		GLEditorGraphicsScene *gls = dynamic_cast<GLEditorGraphicsScene*>(scene());
		if(gls)
		{
			QList<GLDrawable*> sel = gls->selectedItems();
			foreach(GLDrawable *item, sel)
			{
				QPointF dif = item->property("-mouse-diff").toPointF();
				QPointF posDiff = event->scenePos() + dif;

				qreal x = sz.width()  / (halfGrid ? 2:1);
				qreal y = sz.height() / (halfGrid ? 2:1);
				posDiff.setX(((int)(posDiff.x() / x + .5)) * x);
				posDiff.setY(((int)(posDiff.y() / y + .5)) * y);
				item->setPos(posDiff);
			}
		}

	}
}

void GLDrawable::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
	//qDebug() << "GLDrawable::mouseReleaseEvent";
	QGraphicsItem::mouseReleaseEvent(event);

	if(pos() != rect().topLeft())
	{
		QRectF newRect(pos(), rect().size());
		setRect(newRect);

		qDebug() << "GLDrawable::mouseReleaseEvent: Rect changed: "<<newRect;

		GLEditorGraphicsScene *gls = dynamic_cast<GLEditorGraphicsScene*>(scene());
		if(gls)
		{
			QList<GLDrawable*> sel = gls->selectedItems();
			foreach(GLDrawable *item, sel)
			{
				QRectF newRect(item->pos(), item->rect().size());
				if(item != this)
					item->setRect(newRect);
			}
		}
		//qDebug() << "GLDrawable::mouseReleaseEvent: Rect changed: "<<newRect;
	}
}

QRectF GLDrawable::boundingRect() const
{
	return QRectF(QPointF(0,0), rect().size());
}

void GLDrawable::setGLScene(GLScene *scene)
{
	if(m_scene == scene)
		return;
		
	if(m_scene)
 		m_scene->removeDrawable(this);
 		
 	m_scene = scene;
 	if(m_scene)
 		m_scene->addDrawable(this);
 }



//////////////////////////////////////////////////////////////////////////////

GLDrawablePlaylist::GLDrawablePlaylist(GLDrawable *drawable)
	: QAbstractItemModel(drawable)
	, m_drawable(drawable)
	, m_playTime(0)
	, m_timerTickLength(1./2.)
	, m_currentItemIndex(-1)
{
	connect(&m_tickTimer, SIGNAL(timeout()), this, SLOT(timerTick()));
	m_tickTimer.setInterval((int)(m_timerTickLength * 1000.));
}

GLDrawablePlaylist::~GLDrawablePlaylist()
{
	//disconnect(conn, 0, this, 0);
	m_itemLookup.clear();
	qDeleteAll(m_items);
	m_items.clear();
}

int GLDrawablePlaylist::rowCount(const QModelIndex &/*parent*/) const
{
	return m_items.size();
}

QModelIndex GLDrawablePlaylist::index(int row, int column, const QModelIndex&) const
{
	return createIndex(row,column);
}

QModelIndex GLDrawablePlaylist::parent(const QModelIndex&) const
{
	return QModelIndex();
}

int GLDrawablePlaylist::columnCount(const QModelIndex&) const
{
	return 2;
}

QVariant GLDrawablePlaylist::data( const QModelIndex & index, int role ) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= rowCount(QModelIndex()))
		return QVariant();

	GLPlaylistItem *d = m_items.at(index.row());
		
	if (role == Qt::DisplayRole || Qt::EditRole == role)
	{
		if(index.column() == 0)
		{
			QString value = d->title().isEmpty() ? QString("Item %1").arg(index.row()+1) : d->title();
			return value;
		}
		else
		{
			QString dur = QString().sprintf("%.02f", d->duration());
			return dur;
		}
	}
	else 
	if(index.column() == 1)
	{
		if(role == Qt::TextAlignmentRole)
		{
			return Qt::AlignRight;
		}
		else
		if(role == Qt::BackgroundRole)
		{
			if(d->autoDuration())
				return Qt::gray;
			else
				return QVariant();	
		}
	}
	
// 	else if(Qt::DecorationRole == role)
// 	{
// 		GLSceneLayout *lay = m_scene->m_layouts.at(index.row());
// 		return lay->pixmap();
// 	}
	
	return QVariant();
}

QVariant GLDrawablePlaylist::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation != Qt::Horizontal ||
		section < 0 || 
		section > 1 ||
		role != Qt::DisplayRole)
		return QVariant();
		
	
	return section == 0 ? "Item" : "Duration";
}

Qt::ItemFlags GLDrawablePlaylist::flags(const QModelIndex &index) const
{
	if (index.isValid())
		if(index.column() == 1)
			return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable; //| Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
		else
			return Qt::ItemIsEnabled | Qt::ItemIsSelectable; //| Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;// | Qt::ItemIsDropEnabled ;
}
bool GLDrawablePlaylist::setData(const QModelIndex &index, const QVariant & value, int /*role*/)
{
	if (!index.isValid())
		return false;

	if (index.row() >= rowCount(QModelIndex()))
		return false;

	GLPlaylistItem *d = m_items.at(index.row());
	qDebug() << "GLDrawablePlaylist::setData: "<<this<<" row:"<<index.row()<<", col:"<<index.column()<<", value:"<<value;
	
	if(value.isValid() && !value.isNull())
	{
		if(index.column() == 0)
		{	
			d->setTitle(value.toString());
		}
		else
		{
			double val = value.toDouble();
			if(!val)
			{
				/// TODO Need a way to re-calc the auto duration for this item
				d->setDuration(15.);
				d->setAutoDuration(true);
			}
			else
			{
				d->setDuration(val);
				d->setAutoDuration(false);
			}
			emit itemDurationEdited(d);
		}
		
		dataChanged(index,index);
		return true;
	}
	return false;
}

QModelIndex GLDrawablePlaylist::indexOf(GLPlaylistItem *item)
{
	int idx = m_items.indexOf(item);
	return createIndex(idx, 0);
}

void GLDrawablePlaylist::addItem(GLPlaylistItem *item, GLPlaylistItem *insertAfter)
{
	if(!item)
		return;

	item->setPlaylist(this);

	m_itemLookup[item->id()] = item;
	if(!insertAfter)
		m_items << item;
	else
	{
		int row = m_items.indexOf(insertAfter);
		if(row > -1)
			m_items.insert(row, item);
		else
			m_items.append(item);
	}
	
	connect(item, SIGNAL(playlistItemChanged()), this, SLOT(playlistItemChanged()));


	beginInsertRows(QModelIndex(),m_items.size()-1,m_items.size());

	QModelIndex top    = createIndex(m_items.size()-2, 0),
		    bottom = createIndex(m_items.size()-1, 0);
	dataChanged(top,bottom);

	endInsertRows();

	emit itemAdded(item);
}

void GLDrawablePlaylist::removeItem(GLPlaylistItem *item)
{
	if(!item)
		return;

	m_itemLookup.remove(item->id());
	m_items.removeAll(item);
	disconnect(item, 0, this, 0);

	beginRemoveRows(QModelIndex(),0,m_items.size()+1);

	int idx = m_items.indexOf(item);
	QModelIndex top    = createIndex(idx, 0),
		    bottom = createIndex(m_items.size(), 0);
	dataChanged(top,bottom);

	endRemoveRows();

	emit itemRemoved(item);
	
// 	qDebug() << "GLDrawablePlaylist::removeItem: Removed: "<<item;
// 	qDebug() << "GLDrawablePlaylist::removeItem: List is now:";
// 	foreach(GLPlaylistItem *x, m_items)
// 		qDebug() << "GLDrawablePlaylist::removeItem: \t "<<x->title();
// 	
// 	qDebug() << "GLDrawablePlaylist::removeItem: End of"<<m_items.size()<<"items";

}

void GLDrawablePlaylist::playlistItemChanged()
{
	GLPlaylistItem *item = dynamic_cast<GLPlaylistItem *>(item);
	if(!item)
		return;

	int row = m_items.indexOf(item);
	if(row < 0)
		return;

	QModelIndex idx = createIndex(row, row);
	dataChanged(idx, idx);

	/// TODO - rerun calculations for automatic durations/schedules
}


QByteArray GLDrawablePlaylist::toByteArray()
{
	QByteArray array;
	QDataStream stream(&array, QIODevice::WriteOnly);

	QVariantList items;
	foreach(GLPlaylistItem *item, m_items)
		items << item->toByteArray();

	QVariantMap map;
	map["items"] = items;

	stream << map;

	return array;
}

void GLDrawablePlaylist::fromByteArray(QByteArray& array)
{
	QDataStream stream(&array, QIODevice::ReadOnly);
	QVariantMap map;
	stream >> map;

	if(map.isEmpty())
		return;

	if(!m_items.isEmpty())
	{
		qDeleteAll(m_items);
		m_items.clear();
	}

	QVariantList views = map["items"].toList();
	foreach(QVariant var, views)
	{
		QByteArray data = var.toByteArray();
		GLPlaylistItem *item = new GLPlaylistItem(data);
		m_items << item;
		m_itemLookup[item->id()] = item;
	}
}

void GLDrawablePlaylist::setIsPlaying(bool flag)
{
	if(flag)
		play();
	else
		stop();
}
void GLDrawablePlaylist::play(bool restart)
{
	if(restart)
		m_playTime = 0;
	m_tickTimer.start();
}
void GLDrawablePlaylist::stop()
{
	m_tickTimer.stop();
}

void GLDrawablePlaylist::playItem(GLPlaylistItem *item)
{
	int idx = m_items.indexOf(item);
	if(idx < 0)
		return;
	if(idx == m_currentItemIndex)
		return;

	emit currentItemChanged(item);
	m_currentItemIndex = idx;

	const char *propName = m_drawable->metaObject()->userProperty().name();

	//qDebug() << "GLDrawablePlaylist::playItem: Showing"<<propName<<": "<<item->value();

	m_drawable->setProperty(propName, item->value());
}

bool GLDrawablePlaylist::setPlayTime(double time)
{
	m_playTime = time;

	emit playerTimeChanged(time);

	double timeSum = 0;
	GLPlaylistItem *foundItem =0;
	foreach(GLPlaylistItem *item, m_items)
	{
		if(time >= timeSum && time < (timeSum + item->duration()))
		{
			foundItem = item;
			break;
		}
		timeSum += item->duration();
	}
	if(foundItem)
	{
		playItem(foundItem);
		return true;
	}
	else
	{
		qDebug() << "GLDrawablePlaylist::setPlayTime: "<<time<<": Could not find item at time "<<time;
		return false;
	}

}

void GLDrawablePlaylist::timerTick()
{
	if(!setPlayTime(m_playTime + m_timerTickLength))
		if(!setPlayTime(0))
			stop();
}


//////////////////////////////////////////////////////////////////////////////

GLPlaylistItem::GLPlaylistItem(GLDrawablePlaylist *list)
	: QObject(list)
	, m_playlist(list)
	, m_id(-1)
	, m_duration(5.)
	, m_autoDuration(true)
	, m_autoSchedule(true)
	{}

GLPlaylistItem::GLPlaylistItem(QByteArray& array, GLDrawablePlaylist *list)
	: QObject(list)
	, m_playlist(list)
	, m_duration(5.)
	, m_autoDuration(true)
	, m_autoSchedule(true)
{
	fromByteArray(array);
}

QByteArray GLPlaylistItem::toByteArray()
{
	QByteArray array;
	QDataStream stream(&array, QIODevice::WriteOnly);

	QVariantMap map;

	map["id"]	= id();
	map["title"]	= m_title;
	map["value"]	= m_value;
	map["dur"] 	= m_duration;
	map["autod"] 	= m_autoDuration;
	map["sched"] 	= m_scheduledTime;
	map["autos"]	= m_autoSchedule;

	stream << map;

	return array;
}

void GLPlaylistItem::fromByteArray(QByteArray& array)
{
	QDataStream stream(&array, QIODevice::ReadOnly);
	QVariantMap map;
	stream >> map;

	if(map.isEmpty())
		return;

	m_id		= map["id"].toInt();
	m_title		= map["title"].toString();
	m_value		= map["value"];
	m_duration	= map["dur"].toDouble();
	m_autoDuration	= map["autod"].toBool();
	m_scheduledTime	= map["sched"].toDateTime();
	m_autoSchedule	= map["autos"].toBool();
}

int GLPlaylistItem::id()
{
	if(m_id <= 0)
	{
		QSettings set;
		m_id = set.value("GLPlaylistItem/id-counter",100).toInt() + 1;
		set.setValue("GLPlaylistItem/id-counter",m_id);
	}

	return m_id;
}

void GLPlaylistItem::setTitle(const QString& title)
{
	m_title = title;
	emit playlistItemChanged();
}
void GLPlaylistItem::setValue(QVariant value)
{
	m_value = value;
	emit playlistItemChanged();
}
void GLPlaylistItem::setDuration(double duration)
{
	m_duration = duration;
	emit playlistItemChanged();
}
void GLPlaylistItem::setAutoDuration(bool flag)
{
	m_autoDuration = flag;
	emit playlistItemChanged();
}
void GLPlaylistItem::setScheduledTime(const QDateTime& value)
{
	m_scheduledTime = value;
	emit playlistItemChanged();
}
void GLPlaylistItem::setAutoSchedule(bool flag)
{
	m_autoSchedule = flag;
	emit playlistItemChanged();
}

void GLPlaylistItem::setPlaylist(GLDrawablePlaylist *playlist)
{
	m_playlist = playlist;
}

//	void playlistItemChanged();
/*
private:
	GLDrawablePlaylist *m_playlist;

	QVariant m_value;
	int m_duration;
	bool m_autoDuration;
	QDateTime m_scheduledTime;
	bool m_autoSchedule;
};*/

