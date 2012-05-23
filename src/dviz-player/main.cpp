#include "MainWindow.h"
#include <QApplication>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	
	QString host = "localhost";
	if(argc >= 2)
		host = argv[1];
	
	int cmdPort = 9977;
        int vidPort = 8931;
	if(argc >= 3)
		cmdPort = QString(argv[2]).toInt();
	
	if(argc >= 4)
		vidPort = QString(argv[3]).toInt();
	
	qDebug() << "dviz-player: Using host: "<<host<<", cmdPort:"<<cmdPort<<", vidPort:"<<vidPort;
	
	MainWindow *mw = new MainWindow(host, cmdPort, vidPort);
	mw->show();
	
	return app.exec();
}
