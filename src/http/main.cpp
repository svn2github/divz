#include "HttpServer.h"

#include <QCoreApplication>
int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	
	HttpServer s(8080);
	
	return app.exec();
	//return 1;
}

