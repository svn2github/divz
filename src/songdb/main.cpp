#include <QtGui/QApplication>
#include <QCoreApplication>
#include <QtSql>
#include <QDebug>
#include <QSqlTableModel>
#include <QTableView>

#include "Song.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);		
	//QCoreApplication app(argc, argv);		
// 	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
// 	//db.setHostName("bigblue");
// 	db.setDatabaseName("songs.db");
// 	//db.setUserName("acarlson");
// 	//db.setPassword("1uTbSbAs");
// 	bool ok = db.open();
// 	qDebug()<<"Ok?"<<ok;
	/*
	QSqlQuery query;
	query.exec("SELECT title, number FROM songs"); 
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
			if(number == 0)
				continue;
			// check value(1).isNull() before using
			qDebug() << title << number;
		}
	}
	*/
	
	
	QSqlTableModel * tbl = new QSqlTableModel(0,Song::db());
	tbl->setTable("songs");
	
	#define tr
	tbl->select();
	//tbl->removeColumn(0); // don't show the ID
	tbl->setHeaderData(0, Qt::Horizontal, tr("SongID"));
	tbl->setHeaderData(1, Qt::Horizontal, tr("Number"));
	tbl->setHeaderData(2, Qt::Horizontal, tr("Title"));
	tbl->setHeaderData(3, Qt::Horizontal, tr("Text"));
	tbl->setHeaderData(4, Qt::Horizontal, tr("Author"));
	tbl->setHeaderData(5, Qt::Horizontal, tr("Copyright"));
	tbl->setHeaderData(6, Qt::Horizontal, tr("Last Used"));
	
	QTableView *view = new QTableView;
	view->setModel(tbl);
	view->setWindowTitle("songs.db");
	view->show();
	
	
	//return -1;
	return app.exec();
}

