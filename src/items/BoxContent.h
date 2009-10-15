#ifndef BOXCONTENT_H
#define BOXCONTENT_H

#include "AbstractContent.h"

/// \brief The BoxContent class represents a single rectanglar item in a slide.
/// It can have a shadow, outline, fill, and, in the future, rounded corners.
class BoxContent : public AbstractContent
{
	Q_OBJECT

public:
	BoxContent(QGraphicsScene * scene, QGraphicsItem * parent = 0);
	~BoxContent();

public Q_SLOTS:

public:
	// ::AbstractContent
	QString contentName() const { return tr("Box"); }
	QWidget * createPropertyWidget();
	bool fromXml(QDomElement & parentElement);
	void toXml(QDomElement & parentElement) const;
	QPixmap renderContent(const QSize & size, Qt::AspectRatioMode ratio) const;
	int contentHeightForWidth(int width) const;
	
	void syncFromModelItem(AbstractVisualItem*);
	AbstractVisualItem * syncToModelItem(AbstractVisualItem*);
	
	// ::QGraphicsItem
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

private:
	void drawForeground(QPainter *painter);

	void updateShadowClipPath();
	
	QPainterPath m_shadowClipPath;
	bool m_shadowClipDirty;
	quint32 m_lastModelRev;

};

#endif
