#ifndef BACKGROUNDCONTENT_H
#define BACKGROUNDCONTENT_H

#include "AbstractContent.h"
class QVideoProvider;
class QSvgRenderer;

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
    	void renderSvg();
    	
    private:
    	void setVideoFile(const QString &name);
    	void setImageFile(const QString&);
	void loadSvg(const QString&);
	void disposeSvgRenderer();
    	
	QPixmap m_pixmap;
	QSize m_imageSize;
	QVideoProvider * m_videoProvider;
	bool m_still;
	
	bool m_sceneSignalConnected;
	
	QSvgRenderer * m_svgRenderer;
	
	bool m_fileLoaded;
	QString m_fileName;
	QString m_fileLastModified;
};

#endif
