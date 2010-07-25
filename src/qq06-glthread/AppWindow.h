#ifndef AppWindow_H
#define AppWindow_H

#include <QWorkspace>
#include <QMainWindow>

class AppWindow: public QMainWindow
{
	Q_OBJECT
public:
	AppWindow();

protected:
	void closeEvent(QCloseEvent *evt);

private slots:
	void newThread();
	void killThread();
	
private:
	QWorkspace *ws;
};


#endif
