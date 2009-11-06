#include <QApplication>
#include "PPTLoader.h"

#include <QDebug>
int main(int argc, char ** argv)
{
    QApplication a(argc, argv);

	PPTLoader ppt;

    QString showFile = "c:/test.ppt";
    ppt.open(showFile);
    ppt.show();


    return a.exec();
}
//! [0]
