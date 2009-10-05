#include <QtGui/QApplication>
#include <QCoreApplication>
#include <QDebug>

#include "recorder.h"

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	
	Recorder r;
	
	return app.exec();
}