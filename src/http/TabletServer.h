#ifndef TabletServer_H

#include "HttpServer.h"
#include "3rdparty/qjson/serializer.h"
#include "3rdparty/qjson/parser.h"

class MainWindow;
class SongRecordListModel;

class TabletServer : public HttpServer
{
	Q_OBJECT
public:
	TabletServer(quint16 port, QObject* parent = 0);
	
protected:
	void dispatch(QTcpSocket *socket, const QStringList &pathElements, const QStringMap &query);
	void mainScreen(QTcpSocket *socket, const QStringList &pathElements, const QStringMap &query);
	//void screenLoadGroup(QTcpSocket *socket, const QStringList &pathElements, const QStringMap &query);
	
private:
	MainWindow * mw;
	QJson::Serializer m_toJson;
	QJson::Parser     m_fromJson;
	
	SongRecordListModel *m_songListModel;


};

#endif

