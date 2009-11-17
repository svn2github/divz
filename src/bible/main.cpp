#include "BibleGatewayConnector.h"

#include <QCoreApplication>
int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	
	BibleGatewayConnector c;
	
	c.downloadReference(BibleVerseRef::normalize("Romans 6:23"));
	
// 	qDebug() << BibleVerseRef::normalize("Romans 6");
// 	qDebug() << BibleVerseRef::normalize("Romans 6:23");
// 	qDebug() << BibleVerseRef::normalize("Romans 6:23-25");
// 	qDebug() << BibleVerseRef::normalize("1 Cor 3:16");
	
	
// 	BibleVerseList v = c.download("John 3:16");
// 	qDebug() << v;
	
	return app.exec();
	//return 1;
}

