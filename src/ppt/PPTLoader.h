#ifndef PPTLOADER_H
#define PPTLOADER_H

#include "powerpointviewer.h"

#include <QTimer>
#include <QWidget>
#include <QShowEvent>
#include <QCLoseEvent>

class PPTLoader : public QWidget
{
	Q_OBJECT

public:
	PPTLoader();
	~PPTLoader();

	void open(const QString&);


protected:
	void showEvent(QShowEvent*);
	void closeEvent(QCloseEvent*);

private slots:
	void embedHwnd();
	void axException(int,QString,QString,QString);

private:


	QTimer m_timer;
	PowerPointViewer::Application *m_ppt;
	PowerPointViewer::SlideShowView *m_show;

	HWND m_pptWindow;

};
#endif
