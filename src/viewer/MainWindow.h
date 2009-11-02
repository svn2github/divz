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
class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	static MainWindow * mw() { return static_mainWindow; }
	
	MainWindow(QWidget *parent = 0);
	~MainWindow();
	
	QRect standardSceneRect(double aspectRatio = -1);
	Document * currentDocument() { return 0; }

	void log(const QString&, int severity=-1);
	
	void setLiveGroup(SlideGroup *newGroup, Slide *currentSlide);
	
signals:
// 	void documentChanged(Document*);
 	void docSettingsChanged(Document*);
	void aspectRatioChanged(double);
	void appSettingsChanged();

protected slots:
	void slotConnect();
	void slotDisconnect();
	void slotOutputSetup();
	void slotExit();
	void slotClientError(const QString&);
	void aspectChanged(double);

protected:
	void changeEvent(QEvent *e);
	void closeEvent(QCloseEvent *event);

private:
	Ui::MainWindow *m_ui;
	
	OutputInstance *m_inst;
	NetworkClient *m_client;
	
	double m_aspect;
	
	/** static */
	static MainWindow * static_mainWindow;	
};

#endif // MAINWINDOW_H
