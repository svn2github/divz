#ifndef GLRectDrawable_H
#define GLRectDrawable_H

#include "GLDrawable.h"

class GLRectDrawable : public GLDrawable
{
	Q_OBJECT
	
	Q_PROPERTY(QColor fillColor READ fillColor WRITE setFillColor);
	Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor);
	Q_PROPERTY(double borderWidth READ borderWidth WRITE setBorderWidth);
	Q_PROPERTY(double cornerRounding READ cornerRounding WRITE setCornerRounding);
public:
	GLRectDrawable(QObject *parent=0);
	
	QColor fillColor() { return m_fillColor; }
	QColor borderColor() { return m_borderColor; }
	double borderWidth() { return m_borderWidth; }
	double cornerRounding() { return m_cornerRounding; }
	
public slots:
	void setFillColor(QColor c);
	void setBorderColor(QColor c);
	void setBorderWidth(double d);
	void setCornerRounding(double d);

protected:
	virtual void paintGL();
	virtual void initGL();
	
	// For compat with QGraphicsItem
	virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

protected:
	QColor m_fillColor;
	QColor m_borderColor;
	double m_borderWidth;
	double m_cornerRounding;	

};

#endif

