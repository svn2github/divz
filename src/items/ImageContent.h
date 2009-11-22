#ifndef IMAGECONTENT_H
#define IMAGECONTENT_H

#include "AbstractContent.h"

class QSvgRenderer;

/// \brief The ImageContent class represents a image item in a slide.
class ImageContent : public AbstractContent
{
	Q_OBJECT

public:
	ImageContent(QGraphicsScene * scene, QGraphicsItem * parent = 0);
	~ImageContent();

public Q_SLOTS:

public:
	// ::AbstractContent
	QString contentName() const { return tr("Image"); }
	QWidget * createPropertyWidget();
	bool fromXml(QDomElement & parentElement);
	void toXml(QDomElement & parentElement) const;
	QPixmap renderContent(const QSize & size, Qt::AspectRatioMode ratio) const;
	int contentHeightForWidth(int width) const;
	
	void syncFromModelItem(AbstractVisualItem*);
	AbstractVisualItem * syncToModelItem(AbstractVisualItem*);
	
	void dirtyCache();
	bool hasSourceOffsets() { return true; }
	
	// ::QGraphicsItem
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
	

private slots:
	void renderSvg();
	
private:
	void drawForeground(QPainter *painter, bool screenTranslation = true);
	
	void loadFile(const QString&);
	void loadSvg(const QString&);
	void setPixmap(const QPixmap &);
	void checkSize();
	void disposeSvgRenderer();
	
	QPixmap m_pixmap;
	QSize m_imageSize;
	QSvgRenderer * m_svgRenderer;
	
	void updateShadowClipPath();
	
	QPainterPath m_shadowClipPath;
	bool m_shadowClipDirty;
	
	bool m_fileLoaded;
	QString m_fileName;
	QString m_fileLastModified;
	
	quint32 m_lastModelRev;
};

#endif
