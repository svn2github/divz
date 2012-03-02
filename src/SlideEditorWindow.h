#ifndef SLIDEEDITORWINDOW_H
#define SLIDEEDITORWINDOW_H

#include <QMainWindow>
#include <QListView>

#include "model/SlideGroupFactory.h" 
class Document;

class SlideGroupFactory;
class SlideGroupListModel;
class AbstractItem;
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
class QDockWidget;
class GenericItemConfig;
class AbstractContent;
class QTabWidget;
class SlideEditorWindowListView;
class SlideEditorWindow;
class SlideItemListModel;

class QCheckBox;
class QSlider;
class QLabel;
class QTimer;

class SlideEditorWindowListView : public QListView
{ 
public:
	SlideEditorWindowListView(SlideEditorWindow * ctrl, QWidget *parent=0);
protected:
	void keyPressEvent(QKeyEvent *event);	
	SlideEditorWindow *ctrl;
};

#define TOOLBAR_TEXT_SIZE_INC 4

class QShowEvent;
class SlideEditorWindow : public AbstractSlideGroupEditor
{
	Q_OBJECT
public:
	SlideEditorWindow(SlideGroup *g=0, QWidget * parent = 0);
	~SlideEditorWindow();
	
	MyGraphicsScene * scene() { return m_scene; }
	SlideGroup * slideGroup() { return m_slideGroup; } 
	
	int iconSize() { return m_iconSize; }
	
public slots:
	void setSlideGroup(SlideGroup*g,Slide *curSlide=0);
	void setCurrentSlide(Slide *slide);
	Slide * nextSlide();
	Slide * prevSlide();

	void ignoreUndoChanged(bool);
	
signals:
	void closed();

protected:
	void closeEvent(QCloseEvent*);
	void showEvent(QShowEvent*);

protected slots:
	friend class SlideEditorWindowListView;
	void slideSelected(const QModelIndex &);
	void currentChanged(const QModelIndex &idx,const QModelIndex &);
	
	void itemSelected(const QModelIndex &);
	void currentItemChanged(const QModelIndex &idx,const QModelIndex &);
	
	void itemDoubleClicked(AbstractContent *);
	
private slots:
	void newTextItem();
	void newBoxItem();
	void newVideoItem();
	void newImageItem();
	void newOutputView();
	void newSlide();
	void delSlide();
	void slideProperties();
	void dupSlide();
	void groupProperties();
	
	//void textSizeChanged(double);
	void textSizeBoxChanged();
	void textPlus();
	void textMinus();
	void textFitToRect();
	void textNaturalBox();
	void textFitToSlide();
	
	void editMasterSlide();
	void masterSlideEditorClosed();
	
	void editAltGroups();
	//void altEditorClosed();
	
	void centerSelVert();
	void centerSelHorz();
	
	void slotConfigGrid();
	
	void slotConfigBackground();
	void setSlideTimeout(double);
	void zeroSlideTimeout();
	void guessSlideTimeout();
	
	void setInheritFade(bool);
	void setFadeSpeedPreset(int);
	
	
	void slidesDropped(QList<Slide*>);
	void itemsDropped(QList<AbstractItem*>);
	
	void appSettingsChanged();
	void aspectRatioChanged(double);
	
	void slideChanged(Slide *slide, QString slideOperation, AbstractItem *item, QString operation, QString fieldName, QVariant value);
	void slideItemChanged(AbstractItem *item, QString operation, QString fieldName, QVariant value, QVariant old);
	void releaseSlideGroup();
	
	void selectionChanged();
	
	void setCurrentSlideLive();
	
// 	void autosave();
	void setAutosaveOn(bool);
	
	void repaintSlideList();
	
	void setIconSize(int);
	
private:
	void setupSlideList();
	void setupViewportLines();
	void setupToolbar();
	void setupPropDock();
	void setupItemList();
	
	void updatePropDock(AbstractContent*);
	
	void addVpLineY(qreal y, qreal x1, qreal x2, bool inside=true);
	void addVpLineX(qreal x, qreal y1, qreal y2, bool inside=true);
	
	//Slide *m_slide;
	SlideGroup *m_slideGroup;
	SlideGroupListModel *m_slideModel;
	
	SlideEditorWindowListView *m_slideListView;
	QSplitter * m_splitter;
	
	QListView *m_itemListView;
	SlideItemListModel *m_itemModel;
	
	Document * m_doc;
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
	
	QAction * m_textNaturalBoxAction;
	QAction * m_textFitToBoxAction;
	QAction * m_textFitToSlideAction;
	
	QDoubleSpinBox * m_slideTimeout;
	
	QCheckBox * m_inheritFadeBox;
	QSlider * m_fadeSlider;
	QLabel * m_fadeSliderLabel;
	QLabel * m_fadeSliderLabel2;
	
	QList<TextBoxContent*> m_currentTextItems;
	
	QDockWidget * m_propDock;
	QWidget *m_propDockEmpty;
	GenericItemConfig *m_currentConfig;
	AbstractContent *m_currentConfigContent;
	
	QTabWidget * m_propDockBase;
	QWidget * m_currentPropWidget;
	
	QTimer * m_autosaveTimer;
	
	QAction * m_masterSlideAction;
	
	SlideEditorWindow *m_masterSlideEditor;
	
	QAction * m_altEditAction;
	SlideEditorWindow *m_altEditorWindow;
	
	int m_iconSize;
	QSlider * m_iconSizeSlider;
	bool m_lockIconSizeSetter;

};


#endif
