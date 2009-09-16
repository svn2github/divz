#ifndef BOXCONTENT_H
#define BOXCONTENT_H

#include "AbstractContent.h"

/// \brief TODO
class BoxContent : public AbstractContent
{
    Q_OBJECT
    //Q_PROPERTY(QString text READ text WRITE setText)
    
    public:
        BoxContent(QGraphicsScene * scene, QGraphicsItem * parent = 0);
        ~BoxContent();

    public Q_SLOTS:
//         QString text() const;
//         void setText(const QString & text);

    public:
        // ::AbstractContent
        QString contentName() const { return tr("Box"); }
        QWidget * createPropertyWidget();
        bool fromXml(QDomElement & parentElement);
        void toXml(QDomElement & parentElement) const;
        QPixmap renderContent(const QSize & size, Qt::AspectRatioMode ratio) const;
        int contentHeightForWidth(int width) const;
//         void selectionChanged(bool selected);
        
        void syncFromModelItem(AbstractVisualItem*);
        AbstractVisualItem * syncToModelItem(AbstractVisualItem*);

        // ::QGraphicsItem
//         void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
        void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

    private:
//         void updateTextConstraints();
//         void updateCache();

        // text document, layouting & rendering
//         QString m_text;
//         QList<QRect> m_blockRects;
//         QRect m_textRect;
//         int m_textMargin;

};

#endif
