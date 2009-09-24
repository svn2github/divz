#ifndef OUTPUTVIEWER_H
#define OUTPUTVIEWER_H

#include <QGraphicsView>

#include <QList>
#include <QGraphicsView>
#include <QTimer>

#include "MyGraphicsScene.h"
#include "model/SlideGroup.h"
#include "SlideGroupViewer.h"

class OutputViewer : public QWidget
{
public:
	OutputViewer(SlideGroupViewer *output=0, QWidget *parent=0);
	~OutputViewer();

	void setViewer(SlideGroupViewer*);
	

protected:
	void resizeEvent(QResizeEvent *);
	void adjustViewScaling();

private:
	SlideGroupViewer * m_output;
	MyGraphicsScene * m_scene;
	QGraphicsView * m_view;

};

#endif // OUTPUTVIEWER_H
