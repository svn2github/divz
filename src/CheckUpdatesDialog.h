#ifndef CheckUpdatesDialog_H
#define CheckUpdatesDialog_H

#include <QtGui>
#include <QtNetwork>

class CheckUpdatesDialogLauncher : public QObject
{
	Q_OBJECT
public:
	CheckUpdatesDialogLauncher();
	virtual ~CheckUpdatesDialogLauncher() {};
	
public slots:
	void start();
	
signals:
	void updateCheckFinished();

private slots:
	void downloadFinished();
	void downloadReadyRead();
	
private:
	QNetworkAccessManager m_manager;
	QNetworkReply *m_reply;
	QByteArray m_buffer;
	
};

#endif //CheckUpdatesDialog_H
