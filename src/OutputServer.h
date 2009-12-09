#ifndef OutputServer_H
#define OutputServer_H

#include <QTcpServer>
#include <QThread>
#include <QTcpSocket>
#include <QVariant>
#include "3rdparty/qjson/serializer.h"


class OutputInstance;
class SlideGroup;
class Slide;

class OutputServer : public QTcpServer
{
	Q_OBJECT

public:
	OutputServer(QObject *parent = 0);

	void setInstance(OutputInstance *);

	QString myAddress();

	typedef enum BlockType
	{
		Text,
		QImage,
	};

	typedef enum Command
	{
		SetSlideGroup,
		SetSlide,
		AddFilter,
		DelFilter,
		FadeClear,
		FadeBlack,
		SetBackgroundColor,
		SetOverlaySlide,
		SetLiveBackground,
		SetTextResize,
		SetFadeSpeed,
		SetFadeQuality,
		SetAspectRatio,
		SetSlideObject, // added to the end so that viewers that havn't been recompiled can still work with old cmds
	};

public slots:
	void sendCommand(Command,QVariant a= QVariant(),QVariant b= QVariant(),QVariant c= QVariant());

signals:
	void commandReady(OutputServer::Command,QVariant,QVariant,QVariant);

protected:
	void incomingConnection(int socketDescriptor);

	OutputInstance *m_inst;
};

Q_DECLARE_METATYPE(OutputServer::Command);


struct OutputServerCommandFrame
{
	OutputServer::Command cmd;
	QVariant a;
	QVariant b;
	QVariant c;
};

#include <QMutex>
#include <QTimer>

class OutputServerThread : public QThread
{
    Q_OBJECT

public:
	OutputServerThread(int socketDescriptor, QObject *parent = 0);
	~OutputServerThread();

	void run();

signals:
	void error(QTcpSocket::SocketError socketError);

public slots:
	void queueCommand(OutputServer::Command,QVariant,QVariant,QVariant);

private slots:
	void processCommandBuffer();
	void sendCommand(OutputServer::Command,QVariant,QVariant,QVariant);

private:
	void sendMap(const QVariantMap &);

	QJson::Serializer * m_stringy;
	int m_socketDescriptor;
	QTcpSocket * m_socket;

	QMutex m_bufferMutex;
	QList<OutputServerCommandFrame> m_commandBuffer;
	QTimer * m_dequeueTimer;

};


#endif //JpegServer_H

