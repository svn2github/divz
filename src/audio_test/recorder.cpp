#include "recorder.h"

Recorder::Recorder()
{
	outputFile.setFileName("/test.raw");
	outputFile.open( QIODevice::WriteOnly | QIODevice::Truncate );
	
	QAudioFormat format;
	// set up the format you want, eg.
	format.setFrequency(8000);
	format.setChannels(1);
	format.setSampleSize(8);
	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::UnSignedInt);
	
	audio = new QAudioInput(format, this);
	QTimer::singleShot(3000, this, SLOT(stopRecording()));
	audio->start(&outputFile);
	// Records audio for 3000ms
} 

Recorder::~Recorder()
{
	delete audio;
	audio = 0;
}

void Recorder::stopRecording()
{
	audio->stop();
	outputFile.close();
}
