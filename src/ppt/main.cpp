#include <QApplication>
#include "PPTLoader.h"
#include <windows.h>

#include <QTimer>
#include <QDebug>
int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

    PPTLoaderTest * t = new PPTLoaderTest();
    QTimer::singleShot( 500, t, SLOT(test1()));
    QTimer::singleShot(2000, t, SLOT(test2()));

    //ppt.showRun(0,0,320,240);
    return app.exec();
    //return -1;
}
//! [0]

