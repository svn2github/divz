#include "TestServer.h"

#include <QCoreApplication>
int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	
	TestServer s(8080);
	
	return app.exec();
	//return 1;
}

