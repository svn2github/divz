#include <QApplication>
#include "PPTLoader.h"
#include <windows.h>

#include <QDebug>
int main(int argc, char ** argv)
{
    QApplication a(argc, argv);

    PPTLoader ppt;
    ppt.openFile("c:/test3.ppt");
    ppt.showRun(0,0,320,240);
    return a.exec();
}
//! [0]
