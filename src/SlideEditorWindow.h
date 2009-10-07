#ifndef SLIDEEDITORWINDOW_H
#define SLIDEEDITORWINDOW_H

#include <QMainWindow>
#include <QListView>

#include "model/Document.h"
#include "model/SlideGroupFactory.h"

#include "SlideGroupListModel.h"

class MyGraphicsScene;
class SlideGroup;
class QGraphicsLineItem;
class MyGraphicsView;
class QUndoStack;
class QUndoView;
class QSplitter;

class SlideEditorWindow : public AbstractSlideGroupEditor
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
	
	void slidesDropped(QList<Slide*>);
	
	void appSettingsChanged();
	void aspectRatioChanged(double);
	
private:
	void setupSlideList();
	void setupViewportLines();
	void addVpLineY(qreal y, qreal x1, qreal x2, bool inside=true);
	void addVpLineX(qreal x, qreal y1, qreal y2, bool inside=true);
	
	//Slide *m_slide;
	SlideGroup *m_slideGroup;
	SlideGroupListModel *m_slideModel;
	
	QListView *m_slideListView;
	QSplitter * m_splitter;
	
	Document m_doc;
	MyGraphicsScene *m_scene;
	MyGraphicsView *m_view;

	QList<QGraphicsLineItem *> m_viewportLines;
	
	bool m_usingGL;	
	
	void setupUndoView();
	QUndoView * m_undoView;
	QUndoStack * m_undoStack;

};


#endif
