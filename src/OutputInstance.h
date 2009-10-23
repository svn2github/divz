#ifndef OUTPUTINSTANCE_H
#define OUTPUTINSTANCE_H

#include <QGraphicsView>

#include <QList>
#include <QGraphicsView>
#include <QTimer>
#include <QCloseEvent>
#include <QFileInfo>

class SlideGroupViewer;
class SlideGroup;

#include "MyGraphicsScene.h"

#include <QWidget>
#include "model/Output.h"

class OutputInstance : public QWidget
{
	Q_OBJECT
public:
	OutputInstance(Output *output, QWidget *parent=0);
	~OutputInstance();
	
	void setSlideGroup(SlideGroup*, Slide *slide = 0);
	SlideGroup * slideGroup();
	
	int numSlides();
	void clear();
	
	void setBackground(QColor);
	void setSceneContextHint(MyGraphicsScene::ContextHint);
	
signals:
	void nextGroup();

public slots:
	Slide * setSlide(Slide *);
	Slide * setSlide(int);
	Slide * nextSlide();
	Slide * prevSlide();
	
	void fadeBlackFrame(bool);
	void fadeClearFrame(bool);
	
	void setLiveBackground(const QFileInfo &, bool waitForNextSlide);

private slots:
	//void appSettingsChanged();
	//void aspectRatioChanged(double);
	void applyOutputSettings();
	
	void slotNextGroup();
	
protected:
	//void closeEvent(QCloseEvent *);

private:
	Output *m_output;
	SlideGroupViewer *m_viewer;

	SlideGroup * m_slideGroup;
	QList<Slide*> m_sortedSlides;
//	QHash<int,QPixmap> m_pixmaps;

	int m_slideNum;
};

#endif // SLIDEGROUPVIEWER_H
