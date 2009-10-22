#include <QtGui/QApplication>
#include <QCoreApplication>
#include <QtSql>
#include <QDebug>
#include <QSqlTableModel>
#include <QTableView>

#include "SongRecord.h"

int main(int argc, char **argv)
{
	//QApplication app(argc, argv);		
	QCoreApplication app(argc, argv);		
// 	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
// 	//db.setHostName("bigblue");
// 	db.setDatabaseName("songs.db");
// 	//db.setUserName("acarlson");
// 	//db.setPassword("1uTbSbAs");
// 	bool ok = db.open();
// 	qDebug()<<"Ok?"<<ok;
	SongRecord::db();
	
	
//	SongRecord * s = SongRecord::retrieve(388);
	SongRecord * s = new SongRecord();
 	s->setTitle("A bbbbbbb");
 	s->setText("bzzzzzzzzz");
 	SongRecord::addSong(s);
// 	if(s->title() != "A Bee Song")
// 	{
// 		s->setText("A Bee Song");
// 		qDebug(" ** Updated title");
// 	}
 	qDebug()<<"SongID:"<<s->songId()<<", title:"<<s->title()<<", author:"<<s->author();
/*	if(s && s->songId())
		qDebug("Got song # 388");
	else
		qDebug("Song # 388 does NOT exist");*/
	
	if(s && s->songId())
		SongRecord::deleteSong(s);
	
	
// /*
// 	QSqlQuery query;
// 	query.exec("SELECT author FROM songs where songid=16"); 
// 	if (query.lastError().isValid())
// 	{
// 		qDebug() << query.lastError();
// 	}
// 	else
// 	{
// 		query.next();
// 		qDebug() << "author:" << query.value(0).toString();
// 	}*/
// 	
// // 	QSqlQuery q2;
// // 	q2.exec("UPDATE songs SET author='' where songid=16");
// // 	if (q2.lastError().isValid())
// // 	{
// // 		qDebug() << q2.lastError();
// // 	}
// // // 	
// //  	return -1;
// 
// 
// // 	QSqlQuery query;
// // 	query.exec("SELECT title, number FROM songs"); 
// // 	if (query.lastError().isValid())
// // 	{
// // 		qDebug() << query.lastError();
// // 	}
// // 	else
// // 	{
// // 		while (query.next()) 
// // 		{
// // 			QString title = query.value(0).toString();
// // 			int number = query.value(1).toInt();
// // 			if(number == 0)
// // 				continue;
// // 			//check value(1).isNull() before using
// // 			qDebug() << title << number;
// // 		}
// // 	}
// 	
// 	
// 	//return -1;
// 	
// 	QSqlTableModel * tbl = new QSqlTableModel(0,SongRecord::db());
// 	tbl->setTable("songs");
// 	
// 	#define tr
// 	tbl->select();
// 	//tbl->removeColumn(0); // don't show the ID
// 	tbl->setHeaderData(0, Qt::Horizontal, tr("SongID"));
// 	tbl->setHeaderData(1, Qt::Horizontal, tr("Number"));
// 	tbl->setHeaderData(2, Qt::Horizontal, tr("Title"));
// 	tbl->setHeaderData(3, Qt::Horizontal, tr("Text"));
// 	tbl->setHeaderData(4, Qt::Horizontal, tr("Author"));
// 	tbl->setHeaderData(5, Qt::Horizontal, tr("Copyright"));
// 	tbl->setHeaderData(6, Qt::Horizontal, tr("Last Used"));
// 	
// 	QTableView *view = new QTableView;
// 	view->setModel(tbl);
// 	view->setWindowTitle("songs.db");
// 	view->show();
// 	
// 	// id=16, nbr=203
// 	SongRecord * s = SongRecord::songByNumber(203);
// 	//Song * s = Song::retrieve(16);
// 	
// 	if(s)
// 	{
// 		qDebug()<<"SongID:"<<s->songId()<<", title:"<<s->title()<<", author:"<<s->author();
// 		//s->setAuthor("asdf");
// 		
// // 		Song * s = Song::retrieve(16);
// // 		qDebug()<<"try2: SongID:"<<s->songId()<<", title:"<<s->title()<<", author:"<<s->author();
// 	}
// 	else
// 	{
// 		qDebug()<<"Can't load song 203 for some unknown reason";
// 	}
// 	
// 	delete s;
// 	s=0;
// 	
	SongRecord::db().close();
	
	
	return -1;
	//return app.exec();
}

