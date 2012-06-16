#include "CheckUpdatesDialog.h"

#include <QtGui>
#include <QDesktopServices>

#include "AppSettings.h"

#define UPDATE_INFO_URL "http://www.mybryanlife.com/dviz-update-info.txt"

CheckUpdatesDialogLauncher::CheckUpdatesDialogLauncher()
{
}

void CheckUpdatesDialogLauncher::start()
{
	QUrl url(UPDATE_INFO_URL);
	m_reply = m_manager.get(QNetworkRequest(url));
	connect(m_reply, SIGNAL(finished()),  this, SLOT(downloadFinished()));
	connect(m_reply, SIGNAL(readyRead()), this, SLOT(downloadReadyRead()));
}

void CheckUpdatesDialogLauncher::downloadFinished()
{
	if(m_reply->error() && m_reply->errorString() != "Connection closed")
	{
		//Download failed
		qDebug() << "CheckUpdatesDialogLauncher::downloadFinished(): Error downloading "<<m_reply->url().toString()<<": "<<m_reply->errorString();
		AppSettings::sendCheckin("/update/error",tr("Error downloading %1: %2").arg(m_reply->url().toString()).arg(m_reply->errorString()));
		emit updateCheckFinished();
		return;
	}
	
	
	// Process reply
	QString info(m_buffer);
	
	// Format:
	// - First line: Single integer denoting the latest release's build SVN rev
	// - Next line:  Direct link to the download 
	//	- If containing ',', pairs of OS:URL, eg. unix:http://...,win:http://...,macx:http://...
	// - Remaining lines to EOF: Text description of release (changes, etc)
	
	QStringList lines = info.split("\n");
	
	if(lines.size() < 3)
	{
		qDebug() << "CheckUpdatesDialogLauncher::downloadFinished(): Not enough info in " << UPDATE_INFO_URL << ": "<<info;
		AppSettings::sendCheckin("/update/error",tr("Not enough info in %1: %2").arg(UPDATE_INFO_URL).arg(info));
		emit updateCheckFinished();
		return;
	}
	
	int latestRev = lines.takeFirst().toInt();
	if(BUILD_SVN_REV >= latestRev) // BUILD_SVN_REV is defined by dviz.pro 
	{
		// Nothing to do here, return
		qDebug() << "CheckUpdatesDialogLauncher::downloadFinished(): current rev "<<BUILD_SVN_REV<<" newer than rev online: "<<latestRev;
		emit updateCheckFinished();
		return;
	}
	
	QString urlInfo = lines.takeFirst();
	QString diz = lines.join("\n");
	
	QString downloadUrl;
	if(urlInfo.contains(","))
	{
		QStringList downloads = urlInfo.split(",");
		
		QString osInfo;
		
		#ifdef Q_OS_LINUX
		osInfo = "unix";
		#endif
		
		#ifdef Q_OS_WIN
		osInfo = "win";
		#endif
		
		#ifdef Q_OS_MAC
		osInfo = "mac";
		#endif
		
		foreach(QString item, downloads)
		{
			QStringList itemInfo = item.split(":");
			
			QString itemOs = itemInfo.takeFirst();
			QString itemUrl = itemInfo.takeFirst();
			
			if(itemOs == osInfo)
				downloadUrl = itemUrl;
		}
		
		if(downloadUrl.isEmpty())
		{
			qDebug() << "CheckUpdatesDialogLauncher::downloadFinished(): Odd, this OS not found: osInfo:"<<osInfo<<", downloads avail:"<<downloads<<". Nothing to do here.";
			AppSettings::sendCheckin("/update/error",tr("OS not found: %1, avail: %2").arg(osInfo).arg(urlInfo));
			emit updateCheckFinished();
			return;
		}
	}
	else
	{
		downloadUrl = urlInfo;
		
		
		if(downloadUrl.isEmpty())
		{
			qDebug() << "CheckUpdatesDialogLauncher::downloadFinished(): Odd, no download URL in data received. Nothing to do here.";
			AppSettings::sendCheckin("/update/error",tr("No download URL in data"));
			emit updateCheckFinished();
			return;
		}
	}
	
	QMessageBox msgBox;
	msgBox.setWindowTitle("New Version of DViz Available");
	msgBox.setText(tr("New Version of DViz Available: Ver %1").arg(latestRev));
	msgBox.setInformativeText(tr("You can download version %1 of DViz from from the following location:<br><a href='%2'><b>%2</b></a><br><br> Do you want to open that URL now?").arg(latestRev).arg(downloadUrl));
	if(!diz.isEmpty())
		msgBox.setDetailedText(diz);
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Yes);
	msgBox.button(QMessageBox::Yes)->setText("&Download Update");
	msgBox.button(QMessageBox::Cancel)->setText("&Not Now");
	int ret = msgBox.exec();
	if(ret == QMessageBox::Yes)
	{
		qDebug() << "CheckUpdatesDialogLauncher::downloadFinished(): User accepted update message, opening URL: "<<downloadUrl;
		AppSettings::sendCheckin("/update/accepted",tr("%1?old_ver=%2").arg(downloadUrl).arg(BUILD_SVN_REV));
		QDesktopServices::openUrl(downloadUrl);
	}
	else
	{
		qDebug() << "CheckUpdatesDialogLauncher::downloadFinished(): User rejected update, NOT opening URL: "<<downloadUrl;
		AppSettings::sendCheckin("/update/rejected",tr("%1?old_ver=%2").arg(downloadUrl).arg(BUILD_SVN_REV));
	}
	
	m_reply->deleteLater();
	
	//qDebug() << "CheckUpdatesDialogLauncher::downloadFinished(): Done checking for updates";
	
	emit updateCheckFinished();
}

void CheckUpdatesDialogLauncher::downloadReadyRead()
{
	// Append to byte array
	m_buffer.append(m_reply->readAll());
}
	
//private:
//	QNetworkAccessManager m_manager;
//	QNetworkReply *m_reply;


