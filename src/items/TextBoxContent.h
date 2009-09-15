#ifndef TEXTBOXCONTENT_H
#define TEXTBOXCONTENT_H

#include "AbstractContent.h"
class BezierCubicItem;
class QTextDocument;

#include <QtGui/QTextFragment>
#include <QPointF>

class TextLineSpec
{
	TextLineSpec(QTextFragment tf,QRect r,QString txt) 
	{ 
		frag=tf; 
		rect=r; 
		text=txt;
	}
protected:
	friend class TextBoxContent;
	
	QTextFragment frag;
	QString text;
	QRect rect;
};

/// \brief TODO
class TextBoxContent : public AbstractContent
{
    Q_OBJECT
    Q_PROPERTY(QString html READ toHtml WRITE setHtml)
    Q_PROPERTY(bool hasShape READ hasShape NOTIFY notifyHasShape)
    Q_PROPERTY(bool shapeEditing READ isShapeEditing WRITE setShapeEditing NOTIFY notifyShapeEditing)
    Q_PROPERTY(QPainterPath shapePath READ shapePath WRITE setShapePath)
    public:
        TextBoxContent(QGraphicsScene * scene, QGraphicsItem * parent = 0);
        ~TextBoxContent();

    public Q_SLOTS:
        QString toHtml() const;
        void setHtml(const QString & htmlCode);

        bool hasShape() const;
        void clearShape();

        bool isShapeEditing() const;
        void setShapeEditing(bool enabled);

        QPainterPath shapePath() const;
        void setShapePath(const QPainterPath & path);

    Q_SIGNALS:
        void notifyHasShape(bool);
        void notifyShapeEditing(bool);

    public:
        // ::AbstractContent
        QString contentName() const { return tr("Text"); }
        QWidget * createPropertyWidget();
        bool fromXml(QDomElement & parentElement);
        void toXml(QDomElement & parentElement) const;
        QPixmap renderContent(const QSize & size, Qt::AspectRatioMode ratio) const;
        int contentHeightForWidth(int width) const;
        void selectionChanged(bool selected);
        
        void syncFromModelItem(AbstractVisualItem*);
        AbstractVisualItem * syncToModelItem(AbstractVisualItem*);

        // ::QGraphicsItem
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
        void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

    private slots:
        // ::AbstractContent
        void contentsResized();
        void delayContentsResized();
        
    private:
        void updateTextConstraints();
        void updateCache();

        // text document, layouting & rendering
        QTextDocument * m_text;
        QList<QRect> m_blockRects;
        QList<TextLineSpec> m_lineSpecs;
        QRect m_textRect;
        int m_textMargin;

        // shape related stuff
        BezierCubicItem * m_shapeEditor;
        QPainterPath m_shapePath;
        QRect m_shapeRect;
};

#endif
