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
class OutputControl;
class SlideGroupViewControl;

#include "model/Output.h"
#include "OutputInstance.h"

#include <QAbstractSocket>

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
	bool isNetworkViewer() { return true; }
	QString networkHost() { return m_host; }
	
	// Here for compat with dviz MainWindow source
	OutputInstance * outputInst(int id) { return id == m_inst->output()->id() ? m_inst : 0; }
	OutputControl * outputControl(int id) { return 0; } // network viewer does not use an output control
	SlideGroupViewControl * viewControl(int id) { return 0; } // therefore, nv does not use a view control
	
	AbstractSlideGroupEditor * openSlideEditor(SlideGroup *g,Slide *slide=0);

	bool isTransitionActive();
	void editGroup(SlideGroup*, Slide *slide=0) {}
	
signals:
// 	void documentChanged(Document*);
 	void docSettingsChanged(Document*);
	void aspectRatioChanged(double);
	void appSettingsChanged();


protected slots:
	void slotConnect();
	void slotDisconnect();
	void slotReconnect();
	void slotOutputSetup();
	void slotConnected();
	void slotExit();
	void socketError(QAbstractSocket::SocketError);
	void aspectChanged(double);

protected:
	void changeEvent(QEvent *e);
	void closeEvent(QCloseEvent *event);
	
	
private:
	void openOutput();
	
	Ui::MainWindow *m_ui;
	
	OutputInstance *m_inst;
	NetworkClient *m_client;
	
	double m_aspect;
	
	QString m_host;
	int m_port;
	bool m_reconnect;

	QDockWidget * m_previewDock;
	OutputViewer * m_preview;
	
	QTimer m_reconnectTimer;
	
	/** static */
	static MainWindow * static_mainWindow;	
};

#endif // MAINWINDOW_H
