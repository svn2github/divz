#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#define FALLBACK_SCREEN_RECT QRectF(0,0,1024,768)

#include <QTimer>
class QCloseEvent;

namespace Ui {
	class MainWindow;
}

class Document;
class OutputInstance;
class NetworkClient;
class SlideGroup;
class Slide;
class QDockWidget;
class OutputViewer;

class TextBoxItem;


#include "model/Output.h"
#include "OutputInstance.h"

#include <QAbstractSocket>

#define ANIMATE_BASE_MS 25
#define ANIMATE_BASE_PX 35

class AbstractSlideGroupEditor;

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	static MainWindow * mw() { return static_mainWindow; }
	
	MainWindow(QWidget *parent = 0);
	~MainWindow();
	
	QRect standardSceneRect(double aspectRatio = -1);
	Document * currentDocument() { return 0; }

	void log(const QString&, int severity=-1);
	
	void setLiveGroup(SlideGroup *newGroup, Slide *currentSlide,bool);

	void setAutosaveEnabled(bool);
	
	// These functions are specific to dviz-viewer
	bool isNetworkViewer() { return false; }
	QString networkHost() { return ""; }
	
	// Here for compat with dviz MainWindow source
	OutputInstance * outputInst(int id) { return id == m_inst->output()->id() ? m_inst : 0; }

	AbstractSlideGroupEditor * openSlideEditor(SlideGroup *g,Slide *slide=0) {}
	
signals:
// 	void documentChanged(Document*);
 	void docSettingsChanged(Document*);
	void aspectRatioChanged(double);
	void appSettingsChanged();


protected slots:
	void slotOpen();
	void slotSave();
	void slotSaveAs();
	void slotOutputSetup();
	void slotExit();
	void slotTogglePlay();
	void slotPosBoxChanged(int);
	void slotAccelBoxChanged(int);
	void slotResetAccel();
	
	void aspectChanged(double);
	
	void animate();
	void setPos(double);
	
	void slotTextChanged();
	
	void saveTextFile(const QString&);
	void openTextFile(const QString&);

protected:
	void changeEvent(QEvent *e);
	void closeEvent(QCloseEvent *event);
	
	
private:
	void openOutput();
	
	Ui::MainWindow *m_ui;
	
	OutputInstance *m_inst;
	
	double m_aspect;
	
	QDockWidget * m_previewDock;
	OutputViewer * m_preview;
	
	QString m_filename;
	
	TextBoxItem * m_textbox;
	Slide * m_slide;
	SlideGroup *m_group;
	
	QTimer m_animTimer;
	
	double m_pos;
	double m_inc;
	double m_incOrig;
	
	bool m_posBoxLock;
	bool m_accelBoxLock;
	bool m_setPosLock;
	
	/** static */
	static MainWindow * static_mainWindow;	
};

#endif // MAINWINDOW_H
