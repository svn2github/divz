#include <QtGui/QApplication>
#include <QCoreApplication>
#include <QtSql>
#include <QDebug>
#include <QSqlTableModel>
#include <QTableView>

#include "ImageRecord.h"
// #include "SongRecordListModel.h"

//  Q_DECLARE_METATYPE(ImageRecord*);

int main(int argc, char **argv)
{
	//QApplication app(argc, argv);		
	QCoreApplication app(argc, argv);
	
	ImageRecord rec("test.jpg");
	ImageRecord::addRecord(&rec);

	ImageRecord::db().close();
	
	
	return -1;
	//return app.exec();
}

