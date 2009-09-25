#ifndef SLIDEEDITORWINDOW_H
#define SLIDEEDITORWINDOW_H

#include <QMainWindow>
#include <QListView>

#include "model/Document.h"

#include "SlideGroupListModel.h"

class MyGraphicsScene;
class SlideGroup;

class SlideEditorWindow : public QMainWindow
{
	Q_OBJECT
public:
	SlideEditorWindow(SlideGroup *g=0, QWidget * parent = 0);
	~SlideEditorWindow();
	
public slots:
	void setSlideGroup(SlideGroup*g,Slide *curSlide=0);

private slots:
	void newTextItem();
	void newBoxItem();
	void newVideoItem();
	void newSlide();
	void delSlide();
	
	void slideSelected(const QModelIndex &);
	
	//void slideListModelChanged();
	
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
