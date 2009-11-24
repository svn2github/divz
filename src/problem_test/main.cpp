#include <QtGui/QApplication>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QtOpenGL/QGLWidget>
#include <QDebug>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextCursor>


class BoxItem : public QGraphicsItem
{
public:

	QRectF rect;
	QBrush brush;
	QPen pen;
	QTextDocument doc;

	BoxItem(QGraphicsScene * scene, QGraphicsItem * parent) 
		: QGraphicsItem(parent, scene)
		, rect(0,0,500,500)
		, brush(QColor(255,0,0,255))
		, pen(QColor(0,0,0,255), 3.0)
	{
		doc.setPlainText("Hello, World");
		
		// Apply outline pen to the html
		QTextCursor cursor(&doc);
		cursor.select(QTextCursor::Document);
	
		QTextCharFormat format;
 		format.setTextOutline(pen);
 		format.setForeground(brush);
		format.setFontPointSize(88);
		cursor.mergeCharFormat(format);
	}

	QRectF boundingRect() const
	{
		return rect;
	}

	void paint(QPainter * painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
	{
		//painter->setPen(Qt::NoPen);
		QAbstractTextDocumentLayout::PaintContext pCtx;
		doc.documentLayout()->draw(painter, pCtx);
	}
};

int main(int argc, char **argv)
{
	//QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
	QApplication app(argc, argv);

	QGraphicsView *graphicsView = new QGraphicsView();
	graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	graphicsView->setRenderHint( QPainter::Antialiasing, true );

	QGraphicsScene * scene = new QGraphicsScene();

	graphicsView->setScene(scene);
	scene->setSceneRect(0,0,800,600);
	graphicsView->resize(750,190);
	graphicsView->setWindowTitle("Test");

	BoxItem *root = new BoxItem(scene,0);
	root->setPos(10,10);
	//root->setOpacity(.5);
	
	graphicsView->show();

	return app.exec();
}

