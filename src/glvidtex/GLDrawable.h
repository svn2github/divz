#ifndef GLDRAWABLE_H
#define GLDRAWABLE_H

#include <QtGui>

#include "CornerItem.h"

class QPropertyAnimation;
class GLWidget;

// GLDrawable uses Z_MAX combined with the zIndexModifier to scale the zIndex to where zIndexModifier=<z<zIndexModifier
#define Z_MAX 10000

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

class GLDrawable : public QObject,
		   public QGraphicsItem
{
	Q_OBJECT
	Q_PROPERTY(int id READ id);
	Q_PROPERTY(bool userControllable READ isUserControllable WRITE setUserControllable);
	Q_PROPERTY(QString itemName READ itemName WRITE setItemName);
	
	Q_PROPERTY(QRectF rect READ rect WRITE setRect);
	Q_PROPERTY(double zIndex READ zIndex WRITE setZIndex);
	Q_PROPERTY(double opacity READ opacity WRITE setOpacity);
	Q_PROPERTY(double isVisible READ isVisible WRITE setVisible);

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
	
	Q_PROPERTY(bool fadeIn READ fadeIn WRITE setFadeIn);
	Q_PROPERTY(bool fadeOut READ fadeOut WRITE setFadeOut);
	
	Q_PROPERTY(int fadeInLength READ fadeInLength WRITE setFadeInLength);
	Q_PROPERTY(int fadeOutLength READ fadeOutLength WRITE setFadeOutLength);

	Q_ENUMS(AnimationType);

public:
	GLDrawable(QObject *parent=0);
	
	int id();
	QString itemName() { return m_itemName; }
	bool isUserControllable() { return m_isUserControllable; }

	GLWidget *glWidget() { return m_glw; }

	// Compat with QGraphgicsItem: boundingRect()
	QRectF boundingRect() const;// { return m_rect; }
	
	const QRectF & rect() const { return m_rect; }
	double zIndex();
	
	double zIndexModifier();

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

	// If the drawable is aligned to a corner or centered, the size isn't specified by the user - 
	// so they can use this property to "scale" the size of the object.
	double alignedSizeScale() { return m_alignedSizeScale; }
	
	// 3d translations that can be applied to the coordinates before redering
	QVector3D translation() { return m_translation; }
	QVector3D rotation() { return m_rotation; }
	QVector3D rotationPoint() { return m_rotationPoint; }
	
	// If m_canvasSize isNull, returns the GLWidget's canvas size
	QSizeF canvasSize();
	
	virtual void fromByteArray(QByteArray&);
	virtual QByteArray toByteArray();
	
	virtual void loadPropsFromMap(const QVariantMap&, bool onlyApplyIfChanged = false);
	virtual QVariantMap propsToMap();
	
	bool isSelected() { return m_selected; }
	
	bool fadeIn() { return m_fadeIn; }
	bool fadeOut() { return m_fadeOut; }
	
	int fadeInLength() { return m_fadeInLength; }
	int fadeOutLength() { return m_fadeOutLength; }

public slots:
	void updateGL();
	
	void setItemName(const QString&);
	void setUserControllable(bool);

	void setRect(const QRectF& rect);
	void moveBy(double x, double y) { setRect(m_rect.translated(x,y)); }
	void moveBy(const QPointF& pnt) { setRect(m_rect.translated(pnt)); }
	void setOpacity(double i);
	void setOpacity(int o) { setOpacity(((double)o)/100.0); }
	void setZIndex(double z);
	void setZIndex(int z) { setZIndex((double)z); } // for sig/slot compat
	void setZIndexModifier(double mod);

	void show();
	void hide();
	void setVisible(bool flag);
	void setHidden(bool flag);

	bool setAnimationsEnabled(bool);

// 	void setShowFullScreen(bool flag, bool animate=false, int animLength=300, QEasingCurve animCurve = QEasingCurve::Linear);
	void setAlignment(Qt::Alignment value, bool animate=false, int animLength=300, QEasingCurve animCurve = QEasingCurve::Linear);
	void setAlignment(int value);
	void setInsetTopLeft(const QPointF& value);
	void setInsetBottomRight(const QPointF& value);

	void setAlignedSizeScale(double);

	void setTranslation(QVector3D value);
	void setRotation(QVector3D value);
	// Expressed in a range of 0..1 as a percentage of the relevant size (e.g. (.5,.5,.5) means rotate around center, the default value)
	void setRotationPoint(QVector3D value);
	
	void setCanvasSize(const QSizeF&);
	
	void setSelected(bool selected=true);
	
	void setFadeIn(bool);
	void setFadeOut(bool);
	
	void setFadeInLength(int);
	void setFadeOutLength(int);
	
signals:
	void zIndexChanged(double newZIndex);
	void drawableResized(const QSize& newSize);

	void isVisible(bool);
	
	void propertyChanged(const QString& propName, const QVariant& value);

protected slots:
	virtual void animationFinished();
	
	// remove it from m_propAnims
	void propAnimFinished();

protected:
	void updateAnimValues();
	
	virtual void updateAlignment(bool animateRect=false, int animLength=300, QEasingCurve animCurve = QEasingCurve::Linear);

	friend class GLWidget;
	virtual void setGLWidget(GLWidget*); // when update is called, it calls GLWidget::updateGL()

	virtual void viewportResized(const QSize& newSize);
	virtual void canvasResized(const QSizeF& newSize);
	virtual void drawableResized(const QSizeF& newSize);

	virtual void paintGL();
	virtual void initGL();
	
	// For compat with QGraphicsItem
	virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
	// QGraphicsItem::
	virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value);
	
	// For corner items
	void createCorner(CornerItem::CornerPosition corner, bool noRescale);
	void layoutChildren();
	void setControlsVisible(bool visible);


	void propertyWasChanged(const QString& propName, const QVariant& value);
	
	virtual void startAnimation(const AnimParam & p);
	virtual void startAnimations();
	void forceStopAnimations();
	
	QPropertyAnimation *propAnim(const QString&);
	void registerPropAnim(const QString& prop, QPropertyAnimation *anim);
	
	
	GLWidget 	   *m_glw;
	QList<CornerItem *> m_cornerItems;
	bool                m_controlsVisible;

private:
	QAutoDelPropertyAnimation * setupRectAnimation(const QRectF& other, bool animateIn);

	QRectF m_rect;
	double m_zIndex;
	double m_zIndexModifier;
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
	
	QSizeF m_canvasSize;
	
	// Used by code to prevent two animations from running on same prop at same time
	QHash<QString,QPropertyAnimation*> m_propAnims;
	
	bool m_lockVisibleSetter;
	
	int m_id;
	bool m_idLoaded;
	//bool m_lockPropertyUpdates;
	
	QString m_itemName;
	bool m_isUserControllable;
	
	bool m_selected;
	
	bool m_fadeIn;
	bool m_fadeOut;
	int m_fadeInLength;
	int m_fadeOutLength;
};

bool operator==(const GLDrawable::AnimParam&a, const GLDrawable::AnimParam&b);
Q_DECLARE_METATYPE(GLDrawable::AnimationType);

#endif
