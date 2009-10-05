#include "../audio/qaudio.h"
#include "../audio/qaudioformat.h"
#include "../audio/qaudioinput.h"

#include <QTimer>
#include <QFile>

class Recorder : public QObject
{
	Q_OBJECT
public:
	Recorder();
	~Recorder();

public slots:
	void stopRecording();
private:
	QAudioInput * audio;
	QFile outputFile;
};
