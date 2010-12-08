#ifndef EditorWindow_H
#define EditorWindow_H

#include <QtGui>

class GLEditorGraphicsScene;
class GLSceneGroup;
class GLScene;
class GLDrawable;
class RichTextEditorDialog;
class EditorGraphicsView;

class EditorWindow : public QMainWindow
{
	Q_OBJECT
public:
	EditorWindow(QWidget *parent=0);
	~EditorWindow();
	
	GLSceneGroup * group();
	GLScene *scene();
	
	const QSizeF& canvasSize() { return m_canvasSize; }
	
public slots:
	void setGroup(GLSceneGroup*, GLScene *currentScene=0);
	void setCurrentScene(GLScene *);
	
	void addVideoInput();
	void addVideoLoop();
	void addVideoFile();
	void addImage();
	void addText(const QString& text="Hello, World");
	
	void addScene();
	void delScene();
	void dupScene();
	
	void setCanvasSize(const QSizeF&);
	
protected slots:
	void slideSelected(const QModelIndex &);
	void currentSlideChanged(const QModelIndex &idx,const QModelIndex &);
	
	void drawableSelected(GLDrawable *);
	
	void drawableAdded(GLDrawable*);
	void drawableRemoved(GLDrawable*);
	
	void rtfEditorSave();
	
protected:
	void closeEvent(QCloseEvent *event);
	
	void createUI();
	void readSettings();
	void writeSettings();
	
	void addDrawable(GLDrawable*);
	
	QWidget *createPropertyEditors(GLDrawable*);
	
	QSizeF m_canvasSize;
	GLEditorGraphicsScene *m_graphicsScene;

	GLSceneGroup *m_group;
	GLScene *m_scene;
	
	QListView *m_slideList;
	QListView *m_layoutList;
	
	QSplitter *m_mainSplitter;
	QSplitter *m_centerSplitter;
	
	QScrollArea *m_controlArea;	
	QWidget *m_controlBase;
	QWidget *m_currentLayerPropsEditor;
	
	RichTextEditorDialog *m_rtfEditor;
	GLDrawable *m_currentDrawable;

	EditorGraphicsView *m_graphicsView;
	
	QString m_fileName;
};


#endif
