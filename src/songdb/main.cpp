#include <QtGui/QApplication>
#include <QCoreApplication>
#include <QtSql>
#include <QDebug>

int main(int argc, char **argv)
{
	//QApplication app(argc, argv);		
	QCoreApplication app(argc, argv);		
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	//db.setHostName("bigblue");
	db.setDatabaseName("songs.db");
	//db.setUserName("acarlson");
	//db.setPassword("1uTbSbAs");
	bool ok = db.open();
	qDebug()<<"Ok?"<<ok;
	
	QSqlQuery query;
	query.exec("SELECT title, number FROM songs"); // WHERE salary > 50000");
	if (query.lastError().isValid())
	{
		qDebug() << query.lastError();
	}
	else
	{
		while (query.next()) 
		{
			QString title = query.value(0).toString();
			int number = query.value(1).toInt();
			// check value(1).isNull() before using
			qDebug() << title << number;
		}
	}
	
	return -1;
}

