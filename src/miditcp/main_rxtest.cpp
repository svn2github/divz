#include <QCoreApplication>

#include "MidiReceiver.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc,argv);
	
	MidiReceiver reader;
	reader.start();
	
	qDebug() << "Started MIDI Receiver.";
	
	return app.exec();
}
