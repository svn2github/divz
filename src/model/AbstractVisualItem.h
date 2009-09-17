#ifndef ABSTRACTVISUALITEM_H
#define ABSTRACTVISUALITEM_H

#include "AbstractItem.h"

#include <QPointF>
#include <QRectF>

class QGraphicsScene;
class AbstractContent;

class AbstractVisualItem : public AbstractItem
{
	Q_OBJECT
	
	Q_PROPERTY(QPointF 	pos          	READ pos 		WRITE setPos);
	Q_PROPERTY(QRectF  	contentsRect 	READ contentsRect 	WRITE setContentsRect);
	Q_PROPERTY(bool    	isVisible    	READ isVisible		WRITE setIsVisible);
	Q_PROPERTY(double  	zValue       	READ zValue 		WRITE setZValue);
	Q_PROPERTY(double  	opacity      	READ opacity   		WRITE setOpacity);
	Q_PROPERTY(double  	xRotation    	READ xRotation 		WRITE setXRotation);
	Q_PROPERTY(double  	yRotation    	READ yRotation 		WRITE setYRotation);
	Q_PROPERTY(double  	zRotation    	READ zRotation 		WRITE setZRotation);
	
	Q_PROPERTY(quint32 	frameClass   	READ frameClass 	WRITE setFrameClass);
	
	Q_PROPERTY(bool	   	fillEnabled 	READ fillEnabled 	WRITE setFillEnabled);
	Q_PROPERTY(QBrush  	fillBrush    	READ fillBrush 		WRITE setFillBrush);
	
	Q_PROPERTY(bool	   	outlineEnabled 	READ outlineEnabled 	WRITE setOutlineEnabled);
	Q_PROPERTY(QPen    	outlinePen   	READ outlinePen 	WRITE setOutlinePen);
	
	Q_PROPERTY(bool	   	mirrorEnabled 	READ mirrorEnabled 	WRITE setMirrorEnabled);
	Q_PROPERTY(double	mirrorOffset 	READ mirrorOffset 	WRITE setMirrorOffset);
	
	Q_PROPERTY(bool		shadowEnabeld	READ shadowEnabled	WRITE setShadowEnabled);
	Q_PROPERTY(double	shadowOffset 	READ shadowOffset	WRITE setShadowOffset);
	Q_PROPERTY(QBrush	shadowBrush 	READ shadowBrush	WRITE setShadowBrush);
	
	
	
	// Brush has:
	// - Gradient OR Color OR Image/Pixmap
	// Gradient has Types: Linear/Radial/Conical
	// Color has: RGBA
	// Image has filename
	
	// Pen has:
	// - Style, Width, brush, cap, join
	// Style is one of: NoPen (0), Solid (1), Dash (2), Dot (3), DashDot (4), DashDotDot (5), CustomDash (6) - not supported i dont think for us
	// Width is a float
	// Brush is (above)
	// Cap is one of: Flat (0), Square (0x10), Roud (0x20)
	// Join is one of: Miter Join (0), Belve (0x40), Round (0x80), SvgMiterJoin (0x100)
	
	
public:
	AbstractVisualItem();
// 	~AbstractVisualItem();
	
	// ::AbstractItem
	virtual quint32 itemClass() const = 0;
	enum { AbstractVisual = 1 };
	
	ItemType itemType() const { return AbstractItem::Visual; }
	
	virtual bool fromXml(QDomElement & parentElement);
        virtual void toXml(QDomElement & parentElement) const;
        
        static void brushToXml(QDomElement & element, QBrush brush);
        static QBrush brushFromXml(QDomElement & element);
        
        static void penToXml(QDomElement & element, QPen pen);
        static QPen penFromXml(QDomElement & element);

	
	// Convert the attributes here into an AbstractContent object
	virtual AbstractContent * createDelegate(QGraphicsScene*);
	
	// Scene position
	QPointF pos() const { return m_pos; }
	void setPos(QPointF);
	
	// Contents rectangle
	QRectF contentsRect() const { return m_contentsRect; }
	void setContentsRect(QRectF);
	
	// Basic attributes
	bool isVisible() const { return m_isVisible; }
	void setIsVisible(bool);
	
	double zValue() const { return m_zValue; }
	void setZValue(double);
	
	double opacity() const { return m_opacity; }
	void setOpacity(double);
	
	// Not sure if I'll use this...should be an effect instead...
	quint32 frameClass() const { return m_frameClass; }
	void setFrameClass(quint32);
	
	// Basic transformations to simulate 3d 
	double xRotation() { return m_xRotation; }
	void setYRotation(double);
	
	double yRotation() { return m_yRotation; }
	void setXRotation(double);
	
	double zRotation() { return m_zRotation; }
	void setZRotation(double);
	
	
	ITEM_PROPDEF(FillEnabled,	bool,	fillEnabled);
	ITEM_PROPDEF(FillBrush,		QBrush,	fillBrush);
	
	ITEM_PROPDEF(OutlineEnabled,	bool,	outlineEnabled);
	ITEM_PROPDEF(OutlinePen,	QPen,	outlinePen);
	
	ITEM_PROPDEF(MirrorEnabled,	bool,	mirrorEnabled);
	ITEM_PROPDEF(MirrorOffset,	double,	mirrorOffset);
	
	ITEM_PROPDEF(ShadowEnabled,	bool,	shadowEnabled);
	ITEM_PROPDEF(ShadowOffset,	double,	shadowOffset);
	ITEM_PROPDEF(ShadowBrush,	QBrush,	shadowBrush);
	
	
private:
	// Fields
	QPointF		m_pos;		// position on the canvas in sceen coordinates
	QRectF		m_contentsRect;	// contents rectangle of this item
	
	bool		m_isVisible;
	double		m_zValue;
	
        double		m_opacity;
	
	quint32		m_frameClass;
	
	double		m_xRotation;
	double		m_yRotation;
	double		m_zRotation;
	
	bool		m_fillEnabled;
	QBrush		m_fillBrush;
	
	bool		m_outlineEnabled;
	QPen		m_outlinePen;
	
	bool		m_mirrorEnabled;
	double		m_mirrorOffset;
	
	bool		m_shadowEnabled;
	double		m_shadowOffset;
	QBrush		m_shadowBrush;
};

#endif
