#include <QtGui/QApplication>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QtOpenGL/QGLWidget>
#include <QDebug>

class BoxItem : public QGraphicsItem
{
public:

	QRectF rect;
	QBrush brush;
	QPen pen;

	BoxItem(QGraphicsScene * scene, QGraphicsItem * parent) 
		: QGraphicsItem(parent, scene)
		, rect(0,0,500,500)
		, brush(QColor(255,0,0,255))
		, pen(QColor(0,0,0,255), 3.0)
	{
		
	}

	QRectF boundingRect() const
	{
		return rect;
	}

	void paint(QPainter * painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
	{
		painter->setPen(pen);
		//painter->setPen(Qt::NoPen);
		painter->setBrush(brush);
		painter->drawRect(rect);
	}
};

int main(int argc, char **argv)
{
	QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
	QApplication app(argc, argv);

	QGraphicsView *graphicsView = new QGraphicsView();
	graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	graphicsView->setRenderHint( QPainter::Antialiasing, true );

	QGraphicsScene * scene = new QGraphicsScene();

	graphicsView->setScene(scene);
	scene->setSceneRect(0,0,800,600);
	graphicsView->resize(800,600);
	graphicsView->setWindowTitle("Test");

	BoxItem *root = new BoxItem(scene,0);
	root->setPos(10,10);
	root->setOpacity(.5);
	
	graphicsView->show();

	return app.exec();
}

