#include "GLDrawable.h"

#include "GLWidget.h"
#include "GLEditorGraphicsScene.h"

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
{
	// QGraphicsItem
	{
		// customize item's behavior
		setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsSelectable);
		#if QT_VERSION >= 0x040600
			setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
		#endif
		// allow some items (eg. the shape controls for text) to be shown
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
		
		setControlsVisible(false);
	}
	
	updateAnimValues();
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
	m_itemName = name;
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

void GLDrawable::updateGL()
{
	if(m_glw)
		m_glw->updateGL();
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
	if(m_lockVisibleSetter)
		return;
	m_lockVisibleSetter = true;
	
	//if(m_animFinished && 
	if(m_isVisible == flag)
	{
		m_lockVisibleSetter = false;
		emit isVisible(flag);
		return;
	}
		
	//qDebug() << "GLDrawable::setVisible(): "<<this<<": Flag:"<<flag;
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
	
	m_lockVisibleSetter = false;

}

void GLDrawable::forceStopAnimations()
{
	if(!m_animFinished)
	{
		foreach(QAutoDelPropertyAnimation *ani, m_runningAnimations)
		{
			ani->stop();
			//qDebug() << "GLDrawable::setVisible while anim running, resetting property on "<<ani->propertyName().constData();
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
	
	//qDebug() << "GLDrawable::startAnimations(): "<<this<<": At start, rect:"<<rect()<<", opacity:"<<opacity();
	
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
		ani->setDuration(p.length);

// 		//qDebug() << "GLDrawable::startAnimation: type:"<<p.type<<", length:"<<p.length<<", curve:"<<p.curve.type();

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
	if(isSelected())
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
	return map;
}

QVariant GLDrawable::itemChange(GraphicsItemChange change, const QVariant & value)
{
	QVariant retVal;
	bool retValOverride = false;
	if(change != ItemFlagsChange &&
		change != ItemFlagsHaveChanged &&
		change != ItemChildAddedChange) 
		qDebug() << "GLDrawable::itemChange: change:"<<change<<", value:"<<value;
		
	// keep the AbstractContent's center inside the scene rect..
// 	if (change == ItemPositionChange && scene() && AppSettings::gridEnabled())
// 	{
// 		if(modelItem() && modelItem()->itemClass() == BackgroundItem::ItemClass)
// 		{
// 			retVal = QVariant(pos());
//  			retValOverride = true;
//  		}
//  		else
//  		{
// 				
// 			QPointF newPos = AppSettings::snapToGrid(value.toPointF(),m_kbdMotivated);
// 			
// 			// reset the keyboard flag - if another key press comes, it will be set again by the scene
// 			if(m_kbdMotivated)
// 				m_kbdMotivated = false;
// 	
// 			if (newPos != value.toPointF())
// 			{
// 				retVal = QVariant(newPos);
// 				retValOverride = true;
// 			}
// 		}
// 	}

	// tell subclasses about selection changes
// 	if (change == ItemSelectedHasChanged)
// 		selectionChanged(value.toBool());

	//qDebug() << "itemChange(): value:"<<value;

	// changes that affect the mirror item
	//if (m_mirrorItem) {
	switch (change)
	{
		// notify about setPos
		case ItemPositionHasChanged:
// 			if(DEBUG_ABSTRACTCONTENT)
// 				qDebug() << "AbstractContent::itemChange: " << modelItem()->itemName() << " ItemPositionHasChanged:"<<value;
			
			//syncToModelItem(modelItem());
			break;

		// notify about graphics changes
		case ItemSelectedHasChanged:
// 			if(DEBUG_ABSTRACTCONTENT)
// 				qDebug() << "AbstractContent::itemChange: " << modelItem()->itemName() << " ItemSelectedHasChanged:"<<value;
			setControlsVisible(value.toBool() ? true : false);

		case ItemTransformHasChanged:
		case ItemEnabledHasChanged:

		case ItemParentHasChanged:
#if QT_VERSION >= 0x040500
		case ItemOpacityHasChanged:
#endif
/*			if(DEBUG_ABSTRACTCONTENT)
				qDebug() << "AbstractContent::itemChange: " << modelItem()->itemName() << " ItemTransformHasChanged - ItemOpacityHasChanged:"<<value;
			//syncToModelItem(modelItem());
			GFX_CHANGED();*/
			break;

		case ItemZValueHasChanged:
/*			//syncToModelItem(modelItem());
			if(DEBUG_ABSTRACTCONTENT)
				qDebug() << "AbstractContent::itemChange: " << modelItem()->itemName() << " ItemZValueHasChanged:"<<value;
			if(m_mirrorItem)
				m_mirrorItem->setZValue(zValue());*/
			break;

		case ItemVisibleHasChanged:
/*			if(DEBUG_ABSTRACTCONTENT)
				qDebug() << "AbstractContent::itemChange: " << modelItem()->itemName() << " ItemVisibleHasChanged:"<<value;
			//syncToModelItem(modelItem());
			if(m_mirrorItem)
				m_mirrorItem->setVisible(isVisible());*/
			break;

		default:
			break;
	}

	// ..or just apply the value

	QVariant otherVal = QGraphicsItem::itemChange(change, value);
	return retValOverride ? retVal : otherVal;
}


void GLDrawable::setControlsVisible(bool visible)
{
// 	if(m_contextHint != MyGraphicsScene::Editor)
// 		return;

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
		//qDebug() << "New Pos:"<<pos();
		QSizeF sz(10.,10.);// = AppSettings::gridSize();
		bool halfGrid = false;
		QPointF newPos = pos();
		qreal x = sz.width()  / (halfGrid ? 2:1);
		qreal y = sz.height() / (halfGrid ? 2:1);
		newPos.setX(((int)(newPos.x() / x)) * x);
		newPos.setY(((int)(newPos.y() / y)) * y);
		if(newPos != pos())
		{
			setPos(newPos);
			
// 			QPointF posDiff = event->scenePos() - event->lastScenePos(); 
// 		
// 			GLEditorGraphicsScene *gls = dynamic_cast<GLEditorGraphicsScene*>(scene());
// 			if(gls)
// 			{
// 				//qDebug() << "posDiff: "<<posDiff;
// 				QList<GLDrawable*> sel = gls->selectedItems();
// 				foreach(GLDrawable *item, sel)
// 				{
// 					if(item != this)
// 						item->moveBy(posDiff.x(), posDiff.y());
// 				}
// 			}
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
