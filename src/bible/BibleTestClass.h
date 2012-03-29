#ifndef BibleTestClass_H
#define BibleTestClass_H
	
#include <QtGui>

class BibleTestClass : public QObject
{
	Q_OBJECT
private slots:
	void normalizeRef();
	void normalizeRef_data();
};



#endif
