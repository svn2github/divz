#ifndef ABSTRACTVISUALITEM_H
#define ABSTRACTVISUALITEM_H

#include "AbstractItem.h"

#include <QPointF>
#include <QRectF>

class QGraphicsScene;
class AbstractContent;
class QGraphicsItem;

class AbstractVisualItem : public AbstractItem
{
	Q_OBJECT
	
	Q_PROPERTY(QPointF 	pos          	READ pos 		WRITE setPos);
	Q_PROPERTY(QRectF  	contentsRect 	READ contentsRect 	WRITE setContentsRect);
	Q_PROPERTY(QPointF	sourceOffsetTL 	READ sourceOffsetTL	WRITE setSourceOffsetTL);
	Q_PROPERTY(QPointF	sourceOffsetBR 	READ sourceOffsetBR	WRITE setSourceOffsetBR);
	Q_PROPERTY(bool    	isVisible    	READ isVisible		WRITE setIsVisible);
	Q_PROPERTY(double  	zValue       	READ zValue 		WRITE setZValue);
	Q_PROPERTY(double  	opacity      	READ opacity   		WRITE setOpacity);
	Q_PROPERTY(double  	xRotation    	READ xRotation 		WRITE setXRotation);
	Q_PROPERTY(double  	yRotation    	READ yRotation 		WRITE setYRotation);
	Q_PROPERTY(double  	zRotation    	READ zRotation 		WRITE setZRotation);
	
	Q_PROPERTY(quint32 	frameClass   	READ frameClass 	WRITE setFrameClass);
	
	Q_PROPERTY(FillType 	fillType 	READ fillType		WRITE setFillType);
	Q_PROPERTY(QBrush  	fillBrush    	READ fillBrush 		WRITE setFillBrush);
	Q_PROPERTY(QString  	fillImageFile	READ fillImageFile	WRITE setFillImageFile);
	Q_PROPERTY(QString	fillVideoFile	READ fillVideoFile	WRITE setFillVideoFile);
	
	Q_PROPERTY(bool	   	outlineEnabled 	READ outlineEnabled 	WRITE setOutlineEnabled);
	Q_PROPERTY(QPen    	outlinePen   	READ outlinePen 	WRITE setOutlinePen);
	
	Q_PROPERTY(bool	   	mirrorEnabled 	READ mirrorEnabled 	WRITE setMirrorEnabled);
	Q_PROPERTY(double	mirrorOffset 	READ mirrorOffset 	WRITE setMirrorOffset);
	
	Q_PROPERTY(bool		shadowEnabled	READ shadowEnabled	WRITE setShadowEnabled);
	Q_PROPERTY(double 	shadowBlurRadius READ shadowBlurRadius	WRITE setShadowBlurRadius);
	Q_PROPERTY(double	shadowOffsetX 	READ shadowOffsetX	WRITE setShadowOffsetX);
	Q_PROPERTY(double	shadowOffsetY 	READ shadowOffsetY	WRITE setShadowOffsetY);
	Q_PROPERTY(QBrush	shadowBrush 	READ shadowBrush	WRITE setShadowBrush);
	
	Q_ENUMS(FillType);
	
	
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
	typedef enum FillType { None, Solid, Gradient, Image, Video };

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
	virtual AbstractContent * createDelegate(QGraphicsScene*scene=0,QGraphicsItem*parent=0);
	virtual void warmVisualCache() {}
	
	// Scene position
	QPointF pos() const { return m_pos; }
	void setPos(QPointF);
	
	// Contents rectangle
	QRectF contentsRect() const { return m_contentsRect; }
	void setContentsRect(QRectF);
	
	// Source rectangle (for images, etc)
	QPointF sourceOffsetTL() const { return m_sourceOffsetTL; }
	void setSourceOffsetTL(QPointF);
	QPointF sourceOffsetBR() const { return m_sourceOffsetBR; }
	void setSourceOffsetBR(QPointF);
	
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
	
	virtual bool hasSourceOffsets() { return false; }
	
	
	ITEM_PROPDEF(FillType,		FillType, 	fillType);
	ITEM_PROPDEF(FillBrush,		QBrush,		fillBrush);
	ITEM_PROPDEF(FillImageFile,	QString,	fillImageFile);
	ITEM_PROPDEF(FillVideoFile,	QString,	fillVideoFile);
	
	ITEM_PROPDEF(OutlineEnabled,	bool,	outlineEnabled);
	ITEM_PROPDEF(OutlinePen,	QPen,	outlinePen);
	
	ITEM_PROPDEF(MirrorEnabled,	bool,	mirrorEnabled);
	ITEM_PROPDEF(MirrorOffset,	double,	mirrorOffset);
	
	ITEM_PROPDEF(ShadowEnabled,	bool,	shadowEnabled);
	ITEM_PROPDEF(ShadowBlurRadius,	double,	shadowBlurRadius);
	ITEM_PROPDEF(ShadowOffsetX,	double,	shadowOffsetX);
	ITEM_PROPDEF(ShadowOffsetY,	double,	shadowOffsetY);
	ITEM_PROPDEF(ShadowBrush,	QBrush,	shadowBrush);
	
	
private:
	// Fields
	QPointF		m_pos;		// position on the canvas in sceen coordinates
	QRectF		m_contentsRect;	// contents rectangle of this item
	QPointF		m_sourceOffsetTL;	// source rect - used by images & videos
	QPointF		m_sourceOffsetBR;	// source rect - used by images & videos
	
	bool		m_isVisible;
	double		m_zValue;
	
        double		m_opacity;
	
	quint32		m_frameClass;
	
	double		m_xRotation;
	double		m_yRotation;
	double		m_zRotation;
	
	FillType	m_fillType;
	QBrush		m_fillBrush;
	QString		m_fillImageFile;
	QString		m_fillVideoFile;
	
	bool		m_outlineEnabled;
	QPen		m_outlinePen;
	
	bool		m_mirrorEnabled;
	double		m_mirrorOffset;
	
	bool		m_shadowEnabled;
	int		m_shadowBlurRadius;
	double		m_shadowOffsetX;
	double		m_shadowOffsetY;
	QBrush		m_shadowBrush;
	
	//Q_DECLARE_METATYPE(FillType);
};

Q_DECLARE_METATYPE(AbstractVisualItem::FillType);

	
	
#endif
