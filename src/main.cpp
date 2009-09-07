#include "dviz.h"

#include <QtGui/QApplication>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	
	DViz d(argc,argv);
	d.show();
	
	return app.exec();
}

