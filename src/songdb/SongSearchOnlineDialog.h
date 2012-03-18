#ifndef SONGSEARCHONLINEDIALOG_H
#define SONGSEARCHONLINEDIALOG_H

#include <QDialog>
#include <QProcess>
#include <QtGui>

namespace Ui {
    class SongSearchOnlineDialog;
}

#include "../3rdparty/qjson/parser.h"

class SongBrowser;
class SongSearchOnlineDialog : public QDialog 
{
	Q_OBJECT
	
public:
	SongSearchOnlineDialog(SongBrowser *sb, QWidget *parent = 0);
	~SongSearchOnlineDialog();

public slots:
	void setText(const QString&);

private:
	void setupUi();
	void setupProcs();
	
private slots:
	void currentRowChanged(int);
	void slotSaveClicked();
	void slotSearchClicked();
	
	void searchProcExit(int, QProcess::ExitStatus);
	void downloadProcExit(int, QProcess::ExitStatus);

protected:
	void changeEvent(QEvent *e);
	
	QVariant getJson(QProcess *, QString context);
	QVariant getJson(QByteArray &array, QString context);

private:
	Ui::SongSearchOnlineDialog *ui;
	
	QProcess m_searchProc;
	QProcess m_downloadProc;
	
	QVariantList m_resultset;
	
	QJson::Parser m_jsonParser;
	
	SongBrowser *m_sb;
};

#endif // SONGSEARCHONLINEDIALOG_H
