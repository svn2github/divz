#include "BibleGatewayConnector.h"

#include <QCoreApplication>
int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	
	BibleGatewayConnector c;
	
	//c.downloadReference("Romans 6");
	
	BibleVerseRef::normalize("Romans 6");
	BibleVerseRef::normalize("Romans 6:23"); 
	BibleVerseRef::normalize("Romans 6:23-25");
	
	
// 	BibleVerseList v = c.download("John 3:16");
// 	qDebug() << v;
	
	return app.exec();
	//return 1;
}

