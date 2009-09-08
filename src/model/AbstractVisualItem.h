#ifndef ABSTRACTVISUALITEM_H
#define ABSTRACTVISUALITEM_H

#include "AbstractItem.h"

#include <QPointF>
#include <QRectF>

class AbstractVisualItem : public AbstractItem
{
	Q_OBJECT
	
	Q_PROPERTY(QPointF pos          READ pos WRITE setPos);
	Q_PROPERTY(QRectF  contentsRect READ contentsRect WRITE setContentsRect);
	Q_PROPERTY(bool    isVisible    READ isVisible WRITE setIsVisible);
	Q_PROPERTY(double  zValue       READ zValue WRITE setZValue);
	Q_PROPERTY(quint32 frameClass   READ frameClass WRITE setFrameClass);
	Q_PROPERTY(double  xRotation    READ xRotation WRITE setXRotation);
	Q_PROPERTY(double  yRotation    READ yRotation WRITE setYRotation);
	Q_PROPERTY(double  zRotation    READ zRotation WRITE setZRotation);
	Q_PROPERTY(double  opacity      READ opacity   WRITE setOpacity);

public:
	AbstractVisualItem();
// 	~AbstractVisualItem();
	
	// Class ID
	virtual quint32 itemClass() const = 0;
	enum { AbstractVisual = 1 };
	
	ItemType itemType() const { return AbstractItem::Visual; }
	
	virtual bool fromXml(QDomElement & parentElement);
        virtual void toXml(QDomElement & parentElement) const;
	
	QPointF pos() const { return m_pos; }
	void setPos(QPointF);
	
	QRectF contentsRect() const { return m_contentsRect; }
	void setContentsRect(QRectF);
	
	bool isVisible() const { return m_isVisible; }
	void setIsVisible(bool);
	
	double zValue() const { return m_zValue; }
	void setZValue(double);
	
	quint32 frameClass() const { return m_frameClass; }
	void setFrameClass(quint32);
	
	double xRotation() { return m_xRotation; }
	void setYRotation(double);
	
	double yRotation() { return m_yRotation; }
	void setXRotation(double);
	
	double zRotation() { return m_zRotation; }
	void setZRotation(double);
	
	double opacity() const { return m_opacity; }
	void setOpacity(double);

private:
	// Fields
	QPointF		m_pos;		// position on the canvas in sceen coordinates
	QRectF		m_contentsRect;	// contents rectangle of this item
	
	bool		m_isVisible;
	double		m_zValue;
	
	quint32		m_frameClass;
	
	double		m_xRotation;
        double		m_yRotation;
        double		m_zRotation;
        
        double		m_opacity;
        
// 	QList<AbstractVisualEffect *> m_effects;
};

#endif
