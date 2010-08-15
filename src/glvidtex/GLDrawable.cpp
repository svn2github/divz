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
	, m_opacity(1)
	, m_isVisible(false)
	, m_animFinished(true)
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

void GLDrawable::setVisible(bool flag)
{
	emit isVisible(flag);
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
	
	m_animDirection = flag;
	startAnimations();
}

void GLDrawable::startAnimations()
{
	if(m_animDirection)
		m_isVisible = m_animDirection;
	
	m_animFinished = false;
	
	bool hasFade  = false;
	foreach(GLDrawable::AnimParam p, m_animations)
		if(( m_animDirection && p.cond == GLDrawable::OnShow) ||
		   (!m_animDirection && p.cond == GLDrawable::OnHide))
		{
			if(p.type == GLDrawable::AnimFade)
				hasFade = true; 
			startAnimation(p);
		}
	if(m_isVisible && !hasFade)
		setOpacity(1.0);
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
	QRectF viewport = m_glw ? m_glw->viewport() : QRectF(0,0,1000,750);
	bool inFlag = m_animDirection;
	
	QAutoDelPropertyAnimation *ani = 0;
	
	switch(p.type)
	{
		case GLDrawable::AnimFade:
			m_originalOpacity = m_opacity;
			ani = new QAutoDelPropertyAnimation(this, "opacity");
			ani->setStartValue(inFlag ? 0.0 : 1.0);
			ani->setEndValue(inFlag ?   1.0 : 0.0);
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
		}
		
	}
	else
	{
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
	m_rect = rect;
	//qDebug() << "GLDrawable::setRect: "<<rect;
	drawableResized(rect.size());
	emit drawableResized(rect.size());
	updateGL();
}
	
void GLDrawable::setZIndex(double z)
{
	m_zIndex = z;
	emit zIndexChanged(z);
}

void GLDrawable::setOpacity(double o)
{
	//qDebug() << "GLDrawable::setOpacity: "<<o;
	m_opacity = o;
	updateGL();
}

void GLDrawable::setGLWidget(GLWidget* w)
{
	m_glw = w;
	if(!w)
		return;
		
	if(m_rect.isNull())
		m_rect = w->rect();
}

void GLDrawable::viewportResized(const QSize& /*newSize*/)
{
	// NOOP
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
	curve.setType((QEasingCurve::Type)curveType);
}


