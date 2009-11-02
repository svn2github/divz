#ifndef OutputServer_H
#define OutputServer_H

#include <QTcpServer>
#include <QThread>
#include <QTcpSocket>
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

public slots:
	void cmdSetSlideGroup(SlideGroup *, int);
	void cmdSetSlide(int);
	void cmdAddFilter(int);
	void cmdDelFilter(int);
	void cmdFadeClear(bool);
	void cmdFadeBlack(bool);
	void cmdSetBackroundColor(const QString&);
	void cmdSetOverlaySlide(Slide*);
	void cmdSetLiveBackground(const QString&,bool);
	void cmdSetAutoResizeTextEnabled(bool);
	void cmdSetFadeSpeed(int);
	void cmdSetFadeQuality(int);
	void cmdSetAspectRatio(double);

signals:
	void _cmdSetSlideGroup(SlideGroup *, int);
	void _cmdSetSlide(int);
	void _cmdAddFilter(int);
	void _cmdDelFilter(int);
	void _cmdFadeClear(bool);
	void _cmdFadeBlack(bool);
	void _cmdSetBackroundColor(const QString&);
	void _cmdSetOverlaySlide(Slide*);
	void _cmdSetLiveBackground(const QString&,bool);
	void _cmdSetAutoResizeTextEnabled(bool);
	void _cmdSetFadeSpeed(int);
	void _cmdSetFadeQuality(int);
	void _cmdSetAspectRatio(double);

protected:
	void incomingConnection(int socketDescriptor);
	
	OutputInstance *m_inst;
};


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
	void cmdSetSlideGroup(SlideGroup *, int startSlide);
	void cmdSetSlide(int);
	void cmdAddFilter(int);
	void cmdDelFilter(int);
	void cmdFadeClear(bool);
	void cmdFadeBlack(bool);
	void cmdSetBackroundColor(const QString&);
	void cmdSetOverlaySlide(Slide*);
	void cmdSetLiveBackground(const QString&,bool);
	void cmdSetAutoResizeTextEnabled(bool);
	void cmdSetFadeSpeed(int);
	void cmdSetFadeQuality(int);
	void cmdSetAspectRatio(double);

private:
	void sendMap(const QVariantMap &);
	void sendCmd(const QString &cmd, const QString &arg, QVariant value, const QString &arg2="", QVariant value2= QVariant());
	
	QJson::Serializer * m_stringy;
	int m_socketDescriptor;
	QTcpSocket * m_socket;
	
};


#endif //JpegServer_H

