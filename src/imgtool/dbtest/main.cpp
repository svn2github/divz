#include <QtGui/QApplication>
#include <QCoreApplication>
#include <QtSql>
#include <QDebug>
// #include <QSqlTableModel>
// #include <QTableView>
#include <QListView>

#include "ImageRecord.h"
#include "ImageRecordListModel.h"

//  Q_DECLARE_METATYPE(ImageRecord*);

int main(int argc, char **argv)
{
	QApplication app(argc, argv);		
	//QCoreApplication app(argc, argv);
	
	if(0)
	{
		ImageRecord rec("foobar.jpg");
		ImageRecord::addRecord(&rec);
		qDebug() << "New Record:"<<rec;
	}
	if(1)
	{
		QListView * lv = new QListView;
		lv->setWindowTitle(IMAGEDB_FILE);
		lv->setModel(ImageRecordListModel::instance());
		lv->show();
	}
	else
	{
		ImageRecord * rec = ImageRecord::retrieve(1);
		//rec->setFile("test.jpg");
		qDebug() << "Record#1: "<<rec;
	}

// 	ImageRecord::db().close();
	
	
// 	return -1;
	return app.exec();
}

