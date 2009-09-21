#include <QtGui/QApplication>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QtOpenGL/QGLWidget>

class BoxItem : public QGraphicsItem
{
public:

	QRectF m_contentsRect;
	QPen pen;
	QBrush brush;

	BoxItem(QGraphicsScene * scene, QGraphicsItem * parent) : QGraphicsItem(parent, scene),
		m_contentsRect(0,0,500,500), brush(QColor(255,0,0,255))
	{
		setFlags(QGraphicsItem::ItemIsMovable);
		pen.setWidthF(3);
		pen.setColor(QColor(0,0,0,255));
	}

	QRectF boundingRect() const
	{
		return m_contentsRect;
	}

	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
	{
		painter->setPen(pen);
		painter->setBrush(brush);
		painter->drawRect(m_contentsRect);
	}
};

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	QGraphicsView *graphicsView = new QGraphicsView();
	graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	graphicsView->setRenderHint( QPainter::Antialiasing, true );

	QGraphicsScene * scene = new QGraphicsScene();

	graphicsView->setScene(scene);
	scene->setSceneRect(0,0,800,600);
	graphicsView->resize(800,600);
	graphicsView->setWindowTitle("Test");

	for(int i=0;i<100;i++)
	{
		BoxItem *x = new BoxItem(scene,0);
		x->setPos(qrand() % 800, qrand() % 600);
	}

	graphicsView->show();

	return app.exec();
}

