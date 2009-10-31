#ifndef MediaBrowserDialog_H
#define MediaBrowserDialog_H

#include <QDialog>
#include <QFileInfo>

#include "MediaBrowser.h"

class MediaBrowserDialog : public QDialog
{
	Q_OBJECT
public:
	MediaBrowserDialog(const QString &prevPathKey="", QWidget *parent=0);
	~MediaBrowserDialog();
	
	MediaBrowser *browser();
	
	QFileInfo selectedFile() { return m_selectedFile; }

signals:
	void fileDoubleClicked(const QFileInfo&);
	
private slots:
	void fileSelected(const QFileInfo&);
	void slotFileDoubleClicked(const QFileInfo&);

private:
	MediaBrowser *m_browser;
	QFileInfo m_selectedFile;
	QString m_settingsKey;

};




#endif
