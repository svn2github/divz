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
class QUndoCommand;
class QDoubleSpinBox;
class QAction;
class TextBoxContent;

#define TOOLBAR_TEXT_SIZE_INC 4

class SlideEditorWindow : public AbstractSlideGroupEditor
{
	Q_OBJECT
public:
	SlideEditorWindow(SlideGroup *g=0, QWidget * parent = 0);
	~SlideEditorWindow();
	
	MyGraphicsScene * scene() { return m_scene; }
	SlideGroup * slideGroup() { return m_slideGroup; } 
	
public slots:
	void setSlideGroup(SlideGroup*g,Slide *curSlide=0);

	void ignoreUndoChanged(bool);
	
signals:
	void closed();

protected:
	void closeEvent(QCloseEvent*);

private slots:
	void newTextItem();
	void newBoxItem();
	void newVideoItem();
	void newImageItem();
	void newSlide();
	void delSlide();
	void slideProperties();
	void dupSlide();
	void groupProperties();
	
	void textSizeChanged(double);
	void textPlus();
	void textMinus();
	
	void slideSelected(const QModelIndex &);
	
	void slidesDropped(QList<Slide*>);
	
	void appSettingsChanged();
	void aspectRatioChanged(double);
	
	void slideChanged(Slide *slide, QString slideOperation, AbstractItem *item, QString operation, QString fieldName, QVariant value);
	void slideItemChanged(AbstractItem *item, QString operation, QString fieldName, QVariant value, QVariant old);
	void releaseSlideGroup();
	
	void selectionChanged();
	
	void setCurrentSlideLive();
	
private:
	void setupSlideList();
	void setupViewportLines();
	void setupToolbar();
	
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

	bool m_ignoreUndoPropChanges;
	
	
	QAction * m_textPlusAction;
	QAction * m_textMinusAction;
	QDoubleSpinBox * m_textSizeBox;
	QWidget * m_textBase;
	
	QList<TextBoxContent*> m_currentTextItems;

};


#endif
