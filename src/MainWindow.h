#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>

#include "model/Document.h"

#include "SlideGroupListModel.h"

class MyGraphicsScene;
class SlideGroup;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget * parent = 0);
	~MainWindow();
	
public slots:
	void setSlideGroup(SlideGroup*g,Slide *curSlide=0);

private slots:
	void newTextItem();
	void newBoxItem();
	void newVideoItem();
	void newSlide();
	
	void slideSelected(const QModelIndex &);
private:
	void setupSlideGroupDockWidget();
	
	//Slide *m_slide;
	SlideGroup *m_slideGroup;
	SlideGroupListModel *m_slideModel;
	
	QListView *m_slideListView;
	
	Document m_doc;
	MyGraphicsScene *m_scene;
	
	

};


#endif
