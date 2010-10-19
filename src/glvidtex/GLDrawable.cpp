#include "GLDrawable.h"

#include "GLWidget.h"

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
	, m_glw(0)
	, m_zIndex(0)
	, m_zIndexModifier(0)
	, m_opacity(1)
	, m_isVisible(false)
	, m_animFinished(true)
	, m_originalOpacity(-1)
	, m_animationsEnabled(true)
	, m_alignment(Qt::AlignAbsolute)
	, m_inAlignment(false)
	, m_alignedSizeScale(1.)
	, m_animPendingGlWidget(false)
	, m_rotationPoint(.5,.5,.5) // rotate around center by default
	, m_lockVisibleSetter(false)
// 	, m_topPercent(0.)
// 	, m_leftPercent(0.)
// 	, m_bottomPercent(1.)
// 	, m_rightPercent(1.)
{
}

void GLDrawable::updateGL()
{
	if(m_glw)
		m_glw->updateGL();
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

void GLDrawable::setAnimationsEnabled(bool flag)
{
	m_animationsEnabled = flag;
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
	emit zIndexChanged(z);
	updateGL();
}

void GLDrawable::setOpacity(double o)
{
	//qDebug() << "GLDrawable::setOpacity: "<<this<<", opacity:"<<o;
	m_opacity = o;
	updateGL();
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

// void GLDrawable::setTopPercent(double v)
// {
// 	m_topPercent = v;
// 	updateAlignment();
// }
// 
// void GLDrawable::setLeftPercent(double v)
// {
// // 	qDebug() << "GLDrawable::setLeftPercent(): "<<this<<", m_leftPercent:"<<v;
// 	m_leftPercent = v;
// 	updateAlignment();
// }
// 
// void GLDrawable::setBottomPercent(double v)
// {
// 	m_bottomPercent = v;
// 	updateAlignment();
// }
// 
// void GLDrawable::setRightPercent(double v)
// {
// 	m_rightPercent = v;
// 	updateAlignment();
// }

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

