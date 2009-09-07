#ifndef MYGRAPHICSSCENE_H
#define MYGRAPHICSSCENE_H


#include <QGraphicsScene>
#include <QDataStream>
#include <QPainter>
#include <QPixmap>
#include <QRect>
#include <QTime>


// class AbstractContent;
// class AbstractConfig;

class MyGraphicsScene : public QGraphicsScene
{
	Q_OBJECT
	public:
	//         friend class XmlRead;
	//         friend class XmlSave;
	
		MyGraphicsScene(QObject * parent = 0);
		~MyGraphicsScene();
};

#endif
