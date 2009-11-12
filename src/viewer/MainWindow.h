#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#define FALLBACK_SCREEN_RECT QRectF(0,0,1024,768)

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
#include <QAbstractSocket>

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
	
	/** static */
	static MainWindow * static_mainWindow;	
};

#endif // MAINWINDOW_H
