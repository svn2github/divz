#ifndef TEXTITEM_H
#define TEXTITEM_H

#include "AbstractVisualItem.h"
#include <QCache>

class TextItem : public AbstractVisualItem
{
	Q_OBJECT
	
	Q_PROPERTY(QString text         READ text         WRITE setText);
	Q_PROPERTY(QString fontFamily   READ fontFamily   WRITE setFontFamily);
	Q_PROPERTY(double  fontSize     READ fontSize     WRITE setFontSize);
	Q_PROPERTY(bool    shapeEnabled READ shapeEnabled WRITE setShapeEnabled);
	Q_PROPERTY(QPointF shapePoint1  READ shapePoint1  WRITE setShapePoint1);
	Q_PROPERTY(QPointF shapePoint2  READ shapePoint2  WRITE setShapePoint2);
	Q_PROPERTY(QPointF shapePoint3  READ shapePoint3  WRITE setShapePoint3);
	Q_PROPERTY(QPointF shapePoint4  READ shapePoint4  WRITE setShapePoint4);
	Q_PROPERTY(Qt::Alignment xTextAlign READ xTextAlign WRITE setXTextAlign);
	Q_PROPERTY(Qt::Alignment yTextAlign READ yTextAlign WRITE setYTextAlign);
	
public:
	TextItem();
	~TextItem();
	
	QString text() { return m_text; }
	void setText(QString);
	
	// Default Font settings
	QString fontFamily() const { return m_fontFamily; }
	void setFontFamily(QString);
	
	double fontSize() const { return m_fontSize; }
	void setFontSize(double);
	void changeFontSize(double); // force change the html font size
	double findFontSize();
	int fitToSize(const QSize &, int minimumFontSize = 0, int maximumFontSize = 500);
	QSize findNaturalSize(int atWidth = -1);
	
	// Shape (Beizer)
	bool shapeEnabled() const { return m_shapeEnabled; }
	void setShapeEnabled(bool);
	
	QPointF shapePoint1() const { return m_shapePoint1; }
	QPointF shapePoint2() const { return m_shapePoint2; }
	QPointF shapePoint3() const { return m_shapePoint3; }
	QPointF shapePoint4() const { return m_shapePoint4; }
	
	void setShapePoint1(QPointF);
	void setShapePoint2(QPointF);
	void setShapePoint3(QPointF);
	void setShapePoint4(QPointF);
	
	Qt::Alignment xTextAlign() const { return m_xTextAlign; }
	Qt::Alignment yTextAlign() const { return m_yTextAlign; }
	
	void setXTextAlign(Qt::Alignment);
	void setYTextAlign(Qt::Alignment);
	
	// ::AbstractVisualItem
	AbstractContent * createDelegate(QGraphicsScene*scene=0,QGraphicsItem*parent=0);
	
	// ::AbstractItem
	
	// class identification
	quint32 itemClass() const { return 0x0001; }
	
	// loading/saving
	bool fromXml(QDomElement & parentElement);
        void toXml(QDomElement & parentElement) const;

private:
	QString m_text;
	QString m_fontFamily;
	double  m_fontSize;
	bool m_shapeEnabled;
	QPointF m_shapePoint1;
	QPointF m_shapePoint2;
	QPointF m_shapePoint3;
	QPointF m_shapePoint4;
	Qt::Alignment m_xTextAlign;
	Qt::Alignment m_yTextAlign;
	
	static QCache<QString,double> static_autoTextSizeCache;

	

};


#endif
