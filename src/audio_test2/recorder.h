
//#include <QAudio>
#include <QAudioFormat>
#include <QAudioInput>

#include <QTimer>
#include <QFile>
#include <QIODevice>
#include <QByteArray>
#include <QBuffer>

class Recorder : public QObject
{
	Q_OBJECT
public:
	Recorder();
	~Recorder();

public slots:
	void stopRecording();
	void sampleIO();

private:
	QAudioInput * audio;
	QFile outputFile;
	QIODevice *io;
	QTimer sample;
	QByteArray byteArray;
	QBuffer buffer;
};
