#ifndef TEXTBOXCONTENT_H
#define TEXTBOXCONTENT_H

#include "AbstractContent.h"
class QTextDocument;
#include <QAbstractTextDocumentLayout>

#include <QtGui/QTextFragment>
#include <QPointF>
#include <QPainterPath>
#include <QThread>
#include <QPointer>

class TextBoxWarmingThread : public QThread
{
	Q_OBJECT
public:
	TextBoxWarmingThread(AbstractVisualItem*);
	virtual ~TextBoxWarmingThread() {};
	void run();
signals:
	void renderDone(QImage *);
private:
	QPointer<AbstractVisualItem> m_model;
};

class TextBoxWarmingThreadManager : public QObject
{
	Q_OBJECT
public:
	TextBoxWarmingThreadManager(AbstractVisualItem*);
private slots:
	void renderDone(QImage*);
private:
	QPointer<AbstractVisualItem> m_model;
	TextBoxWarmingThread * m_thread;
};
	

/// \brief TODO
class TextBoxContent : public AbstractContent
{
    Q_OBJECT
    Q_PROPERTY(QString html READ toHtml WRITE setHtml)

    public:
	TextBoxContent(QGraphicsScene * scene, QGraphicsItem * parent = 0);
	~TextBoxContent();

    public Q_SLOTS:
	QString toHtml();
	void setHtml(const QString & htmlCode);
	
	static void warmVisualCache(AbstractVisualItem*);

//         Qt::Alignment xTextAlign() const { return m_xTextAlign; }
// 	Qt::Alignment yTextAlign() const { return m_yTextAlign; }
	
// 	void setXTextAlign(Qt::Alignment);
// 	void setYTextAlign(Qt::Alignment);
	
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
	
	void resizeContents(const QRect & rect, bool keepRatio = false);
	
	void syncFromModelItem(AbstractVisualItem*);
	AbstractVisualItem * syncToModelItem(AbstractVisualItem*);
	
	// ::QGraphicsItem
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
	
	QString cacheKey() { return cacheKey(modelItem()); }
	static QString cacheKey(TextBoxContent *box) { return cacheKey(box->modelItem()); }
	static QString cacheKey(AbstractVisualItem *);

    private slots:
	// ::AbstractContent
	void contentsResized();
	void delayContentsResized();
	
	void animateZoom();

    private:
	void updateTextConstraints(int w = -1);
	
	void renderShadow(QPainter *painter, QAbstractTextDocumentLayout::PaintContext *pCtx);
	
	//void updateCache();
	
	QPixmap *m_textCache;
	qreal m_cacheScaleX;
	qreal m_cacheScaleY;
	
	// text document, layouting & rendering
	QTextDocument * m_text;
	QTextDocument * m_shadowText;
	
	// TODO are these still used?
	QRect m_textRect;
	int m_textMargin;
	
	quint32 m_lastModelRev;
	
	QTimer * m_zoomAnimationTimer;
	
	QPointF m_zoomStep;
	QPointF m_zoomEndSize;
	QPointF m_zoomCurSize;
	QPointF m_zoomStartSize;
	QPointF m_zoomDestPoint;
	int m_zoomDir;
	bool m_zoomInit;
	bool m_zoomEnabled;

/*        
        Qt::Alignment m_xTextAlign;
        Qt::Alignment m_yTextAlign;*/
};

#endif
