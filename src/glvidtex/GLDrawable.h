#ifndef GLDRAWABLE_H
#define GLDRAWABLE_H

#include <QtGui>

class QPropertyAnimation;
class GLWidget;

// This class exists because we want to be able to call start()
// from a slot AND have the animation automatically delete when done.
// However, since we cant pass hardcoded arguments thru a slot, AND
// connecting the animation's finished() signal to deleteLater()
// causes a SEGFLT, then this is the only way I can think of.
class QAutoDelPropertyAnimation : public QPropertyAnimation
{
	Q_OBJECT

public:
	QAutoDelPropertyAnimation(QObject * target, const QByteArray & propertyName, QObject * parent = 0) ;

public slots:
	void resetProperty();

private:
	QVariant m_originalPropValue;
};

class GLDrawable : public QObject
{
	Q_OBJECT
	//Q_PROPERTY(QString itemName READ itemName WRITE setItemName);
	Q_PROPERTY(QRectF rect READ rect WRITE setRect);
	Q_PROPERTY(double zIndex READ zIndex WRITE setZIndex);
	Q_PROPERTY(double opacity READ opacity WRITE setOpacity);
	Q_PROPERTY(double isVisible READ isVisible WRITE setVisible);
	
	Q_PROPERTY(double topPercent READ topPercent WRITE setTopPercent);
	Q_PROPERTY(double leftPercent READ leftPercent WRITE setLeftPercent);
	Q_PROPERTY(double bottomPercent READ bottomPercent WRITE setBottomPercent);
	Q_PROPERTY(double rightPercent READ rightPercent WRITE setRightPercent);

// 	Q_PROPERTY(bool showFullScreen READ showFullScreen WRITE setShowFullScreen);
	Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment);
	Q_PROPERTY(QPointF insetTopLeft READ insetTopLeft WRITE setInsetTopLeft);
	Q_PROPERTY(QPointF insetBottomRight READ insetBottomRight WRITE setInsetBottomRight);
	Q_PROPERTY(double alignedSizeScale READ alignedSizeScale WRITE setAlignedSizeScale);
	
	Q_PROPERTY(QVector3D translation READ translation WRITE setTranslation);
	Q_PROPERTY(QVector3D rotation READ rotation WRITE setRotation);
	// Expressed in a range of 0..1 as a percentage of the relevant size (e.g. (.5,.5,.5) means rotate around center, the default value)
	Q_PROPERTY(QVector3D rotationPoint READ rotationPoint WRITE setRotationPoint);
	
	// This is the usable area for arranging the drawable - if isNull, then it uses the GLWidget's canvas size
	Q_PROPERTY(QSizeF canvasSize READ canvasSize WRITE setCanvasSize);

	Q_ENUMS(AnimationType);

public:
	GLDrawable(QObject *parent=0);

	void updateGL();

	GLWidget *glWidget() { return m_glw; }

	const QRectF & rect() const { return m_rect; }
	double zIndex() { return m_zIndex; }

	double opacity() { return m_opacity; }


	typedef enum AnimationType
	{
		AnimNone = 0,

		AnimFade,
		AnimZoom,
		AnimSlideTop,
		AnimSlideBottom,
		AnimSlideLeft,
		AnimSlideRight,

		AnimUser = 1000
	};
	
	typedef enum AnimCondition
	{
		OnHide,
		OnShow,
	};

	typedef struct AnimParam
	{
		AnimCondition cond;
		AnimationType type;
		int startDelay;
		int length;
		QEasingCurve curve;

		QByteArray toByteArray();
		void fromByteArray(QByteArray);
	};

	void addAnimation(AnimParam);
	void removeAnimation(AnimParam);

	AnimParam & addShowAnimation(AnimationType type, int length=500);
	AnimParam & addHideAnimation(AnimationType type, int length=500);

	void resetAllAnimations();

	bool isVisible() { return m_isVisible; }

	bool animationsEnabled() { return m_animationsEnabled; }

// 	bool showFullScreen()		{ return m_showFullScreen; }
	Qt::Alignment alignment()	{ return m_alignment; }
	QPointF insetTopLeft()		{ return m_insetTopLeft; }
	QPointF insetBottomRight()	{ return m_insetBottomRight; }

	// This is the normal size of the content in pixels - independent of the specified rect().
	// E.g. if its an image, this is the size of the image, if this is text, then the size
	// of the text unscaled at natural resolution. Used for calculating alignment.
	virtual QSizeF naturalSize() { return QSizeF(0,0); }

	double alignedSizeScale() { return m_alignedSizeScale; }
	
	double topPercent() { return m_topPercent; }
	double leftPercent() { return m_leftPercent; }
	double bottomPercent() { return m_bottomPercent; }
	double rightPercent() { return m_rightPercent; }
	
	
	QVector3D translation() { return m_translation; }
	QVector3D rotation() { return m_rotation; }
	QVector3D rotationPoint() { return m_rotationPoint; }
	
	// If m_canvasSize isNull, returns the GLWidget's canvas size
	QSizeF canvasSize();

public slots:
	void setRect(const QRectF& rect);
	void setOpacity(double i);
	void setZIndex(double z);

	void show();
	void hide();
	void setVisible(bool flag);

	void setAnimationsEnabled(bool);

// 	void setShowFullScreen(bool flag, bool animate=false, int animLength=300, QEasingCurve animCurve = QEasingCurve::Linear);
	void setAlignment(Qt::Alignment value, bool animate=false, int animLength=300, QEasingCurve animCurve = QEasingCurve::Linear);
	void setAlignment(int value);
	void setInsetTopLeft(const QPointF& value);
	void setInsetBottomRight(const QPointF& value);
	
	void setTopPercent(double value);
	void setLeftPercent(double value);
	void setBottomPercent(double value);
	void setRightPercent(double value);

	void setAlignedSizeScale(double);

	void setTranslation(QVector3D value);
	void setRotation(QVector3D value);
	// Expressed in a range of 0..1 as a percentage of the relevant size (e.g. (.5,.5,.5) means rotate around center, the default value)
	void setRotationPoint(QVector3D value);
	
	void setCanvasSize(const QSizeF&);
	
signals:
	void zIndexChanged(double newZIndex);
	void drawableResized(const QSize& newSize);

	void isVisible(bool);

protected slots:
	virtual void animationFinished();
	
	// remove it from m_propAnims
	void propAnimFinished();

protected:
	virtual void updateAlignment(bool animateRect=false, int animLength=300, QEasingCurve animCurve = QEasingCurve::Linear);

	friend class GLWidget;
	virtual void setGLWidget(GLWidget*); // when update is called, it calls GLWidget::updateGL()

	virtual void viewportResized(const QSize& newSize);
	virtual void canvasResized(const QSizeF& newSize);
	virtual void drawableResized(const QSizeF& newSize);

	virtual void paintGL();
	virtual void initGL();

	virtual void startAnimation(const AnimParam & p);
	virtual void startAnimations();
	void forceStopAnimations();
	
	QPropertyAnimation *propAnim(const QString&);
	void registerPropAnim(const QString& prop, QPropertyAnimation *anim);

	GLWidget *m_glw;

private:
	QAutoDelPropertyAnimation * setupRectAnimation(const QRectF& other, bool animateIn);

	QRectF m_rect;
	double m_zIndex;
	double m_opacity;

	bool m_isVisible;
	bool m_animDirection;
	bool m_animFinished;
	QRectF m_originalRect;
	double m_originalOpacity;

	QList<GLDrawable::AnimParam> m_animations;
	QList<QAutoDelPropertyAnimation*> m_runningAnimations;
	QList<QAutoDelPropertyAnimation*> m_finishedAnimations;

	bool m_animationsEnabled;

// 	bool m_showFullScreen;
	Qt::Alignment m_alignment;
	QPointF m_insetTopLeft;
	QPointF m_insetBottomRight;

	bool m_inAlignment;

	double m_alignedSizeScale;
	
	bool m_animPendingGlWidget;
	bool m_alignmentPending;

	QVector3D m_translation;
	QVector3D m_rotation;
	QVector3D m_rotationPoint;
	
	double m_topPercent;
	double m_leftPercent;
	double m_bottomPercent;
	double m_rightPercent;
	
	QSizeF m_canvasSize;
	
	// Used by code to prevent two animations from running on same prop at same time
	QHash<QString,QPropertyAnimation*> m_propAnims;
};

bool operator==(const GLDrawable::AnimParam&a, const GLDrawable::AnimParam&b);
Q_DECLARE_METATYPE(GLDrawable::AnimationType);

#endif
