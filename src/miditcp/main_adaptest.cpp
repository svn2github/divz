#include <QApplication>

#include "MidiInputAdapter.h"
#include "MidiInputSettingsDialog.h"

int main(int argc, char *argv[])
{
	QApplication app(argc,argv);
	
	qApp->setApplicationName("adaptest");
	qApp->setOrganizationName("Josiah Bryan");
	qApp->setOrganizationDomain("mybryanlife.com");
	
	MidiInputAdapter *adap = new MidiInputAdapter();
	
// 	// Setup "Special 2" key mapping to "Hello World" action
// 	QHash<int,MidiInputAction*> hash;
// 	hash[48] = adap->availableActions().first();
// 	
// 	adap->setMappings(hash);
	
	MidiInputSettingsDialog *d = new MidiInputSettingsDialog(adap);
	d->exec();
	
	return app.exec();
}
