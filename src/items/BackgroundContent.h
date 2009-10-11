#ifndef BACKGROUNDCONTENT_H
#define BACKGROUNDCONTENT_H

#include "AbstractContent.h"
class QVideoProvider;

/// \brief TODO
class BackgroundContent : public AbstractContent
{
    Q_OBJECT

    public:
	BackgroundContent(QGraphicsScene * scene, QGraphicsItem * parent = 0);
	~BackgroundContent();

    public Q_SLOTS:

    public:
	// ::AbstractContent
	QString contentName() const { return tr("Background"); }
	QWidget * createPropertyWidget();
	bool fromXml(QDomElement & parentElement);
	void toXml(QDomElement & parentElement) const;
	QPixmap renderContent(const QSize & size, Qt::AspectRatioMode ratio) const;
	int contentHeightForWidth(int width) const;
	QRectF boundingRect() const;
	
	void syncFromModelItem(AbstractVisualItem*);
	AbstractVisualItem * syncToModelItem(AbstractVisualItem*);
	
	// ::QGraphicsItem
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

    private slots:
    	void setPixmap(const QPixmap & pixmap);
    	void sceneRectChanged(const QRectF &);
    	
    private:
    	void setVideoFile(const QString &name);
    	
	QPixmap m_pixmap;
	QSize m_imageSize;
	QVideoProvider * m_videoProvider;
	bool m_still;
	
	bool m_sceneSignalConnected;
};

#endif
