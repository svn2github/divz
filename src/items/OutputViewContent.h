#ifndef OutputViewContent_H
#define OutputViewContent_H

#include "AbstractContent.h"

class OutputInstance;
class QGraphicsProxyWidget;

/// \brief The OutputViewContent displays a viewer for dviz output inside a slide.
class OutputViewContent : public AbstractContent
{
	Q_OBJECT
public:
	OutputViewContent(QGraphicsScene * scene, QGraphicsItem * parent = 0);
	~OutputViewContent();

public Q_SLOTS:

public:
	// ::AbstractContent
	QString contentName() const { return tr("OutputViewContent"); }
	QWidget * createPropertyWidget();
	QPixmap renderContent(const QSize & size, Qt::AspectRatioMode ratio) const;
	int contentHeightForWidth(int width) const;

	void syncFromModelItem(AbstractVisualItem*);
	AbstractVisualItem * syncToModelItem(AbstractVisualItem*);
	
	void setOutputId(int);
	
	// ::QGraphicsItem
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

private:
	
	OutputInstance * m_viewer;
	OutputInstance * m_inst;
	QGraphicsProxyWidget * m_widgetProxy;

};

#endif
