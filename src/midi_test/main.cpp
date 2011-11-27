#include <QCoreApplication>
#include <QDebug>
#include "3rdparty/rtmidi/RtMidi.h"

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);

 	/** output test **/
 	if(1)
 	{
 		qDebug("MidiTest: Testing output...");
		RtMidiOut midiout;
		
		std::vector<unsigned char> message(3);
		// Open first available port.
		midiout.openPort( 0 );
		// Compose a Note On message.
		message[0] = 144;
		message[1] = 64;
		message[2] = 90;
		
		// Send the message immediately.
		midiout.sendMessage( &message );
		
		qDebug("MidiTest: Done testing output.");
	}
	
	/** input test **/
	if(1)
	{
		qDebug("MidiTest: Testing input...");
		
		RtMidiIn midiin;
		std::vector<unsigned char> message;
		int nBytes;
		double stamp;
		// Open first available port.
		midiin.openPort( 0 );
	
		// Periodically check input queue.
		bool done = false;
		while ( !done ) 
		{
			stamp = midiin.getMessage( &message );
			nBytes = message.size();
			if ( nBytes > 0 ) 
			{
				// Do something with MIDI data.
				//done = true;
			}
			qDebug() << "Bytes in:"<<nBytes;
			// Sleep for 10 milliseconds.
			sleep( 1 );
			
			//done = true;
		}
		
		qDebug("MidiTest: Done testing input!");
	}

	return app.exec();
}

