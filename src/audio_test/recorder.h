#include "../audio/qaudio.h"
#include "../audio/qaudioformat.h"
#include "../audio/qaudioinput.h"

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
