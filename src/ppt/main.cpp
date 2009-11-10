#include <QApplication>
#include "PPTLoader.h"
#include <windows.h>

#include <QTimer>
#include <QDebug>
int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

    //PPTLoaderTest * t = new PPTLoaderTest();
    //QTimer::singleShot( 500, t, SLOT(test1()));
    //QTimer::singleShot(2000, t, SLOT(test2()));
    PPTLoader ppt;
    ppt.openFile("c:/test.ppt");

    ppt.runShow();
    ppt.setWindowRect(0,0,320,240);
    ppt.setState(PPTLoader::Black);
    return app.exec();
    //return -1;
}
//! [0]

