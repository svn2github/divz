#include "recorder.h"

#include <QDebug>

//#include "../audio/qaudiodeviceinfo.h"
#include <QAudioDeviceInfo>

Recorder::Recorder()
{
	qDebug() << "Listing...";
	foreach(const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
	{
     		qDebug() << "Device name: " << deviceInfo.deviceName();
		QStringList codecs = deviceInfo.supportedCodecs();
		qDebug() << "       Supported Codecs: "<<codecs;
	}
	qDebug() << "Done.";

	//outputFile.setFileName("/test.raw");
	//outputFile.open( QIODevice::WriteOnly | QIODevice::Truncate );
	
	QAudioFormat format;

	format.setFrequency(8000);
	format.setChannels(1);
	format.setSampleSize(8);

	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::UnSignedInt);
	
	qDebug() << "Going to create input...";
	audio = new QAudioInput(QAudioDeviceInfo::defaultInputDevice(), format, this); //, format, this);
	qDebug() << "Created input...";
	QTimer::singleShot(10000, this, SLOT(stopRecording()));

	//audio->start(&outputFile);
	connect(&sample, SIGNAL(timeout()), this, SLOT(sampleIO()));
	sample.start(50);

	buffer.setBuffer(&byteArray);
	buffer.open(QBuffer::ReadWrite);
	//connect(&buffer, SIGNAL(bytesWritten()), this, SLOT(sampleIO()));

	audio->start(&buffer);

	//io = audio->start();
	qDebug() << "Recording started.";
	// Records audio for 3000ms
} 

Recorder::~Recorder()
{
	delete audio;
	audio = 0;
}

void Recorder::sampleIO()
{
    /*
	char buf[4096];
	//int read = io->read((char*)&buf,sizeof(buf));
	//fwrite(buf, 8, read, stdout);
	int ba = io->bytesAvailable();
	//QByteArray b = io->read(4096);
	//qDebug() << "SampleIO: ba:"<<ba<<", b:"<<b;
	int read = io->read( (char*)&buf, 4096);
	qDebug() << "**************** Bytes Read: "<<read;
	*/

	//int ba = buffer.bytesAvailable();
	//QByteArray b = io->read(4096);
	//qDebug() << "SampleIO: data: "<<byteArray.size();
	//byteArray.chop(byteArray.size());
	int sz = byteArray.size();
	int sum = 0;
	char * data = byteArray.data();
	for(int i=0;i<sz;i++)
	{
	    sum += (int)data[i];
	}
	int avg = sz == 0 ? 0 : sum / sz;
	byteArray.clear();
	buffer.seek(0);

	qDebug() << "SampleIO: bytes: "<<sz<<", avg:"<<avg;


    /*
	char ch;
	buffer.seek(0);
	 buffer.getChar(&ch);  // ch == 'Q'
	 buffer.getChar(&ch);  // ch == 't'
	 buffer.getChar(&ch);  // ch == ' '
	 buffer.getChar(&ch);  // ch == 'r'
*/
}
void Recorder::stopRecording()
{
	audio->stop();
	//outputFile.close();
	exit(-1);
}
