#include <QApplication>
#include "MjpegClient.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MjpegClient client;
    client.connectTo("cameras",8082);
    //client.connectTo("10.10.9.41",80,"/axis-cgi/mjpg/video.cgi?camera=1&1265143803069");

    return app.exec();
}
