#ifndef OUTPUTVIEWER_H
#define OUTPUTVIEWER_H

#include <QGraphicsView>

#include <QList>
#include <QGraphicsView>
#include <QTimer>

class MyGraphicsScene;
class SlideGroupViewer;

class OutputViewer : public QWidget
{
public:
	OutputViewer(SlideGroupViewer *output=0, QWidget *parent=0);
	~OutputViewer();

	void setViewer(SlideGroupViewer*);
	

protected:
	void resizeEvent(QResizeEvent *);
	void adjustViewScaling();

private slots:
	void appSettingsChanged();
	//void aspectRatioChanged(double);

private:
	SlideGroupViewer * m_output;
	MyGraphicsScene * m_scene;
	QGraphicsView * m_view;
	bool m_usingGL;

};

#endif // OUTPUTVIEWER_H
