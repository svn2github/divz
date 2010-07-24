#include <QtGui/QApplication>
#include <QDebug>

#include "problem.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	Receiver w;
	w.show();
	
	return app.exec();
}

