#ifndef LocalBibleManager_H
#define LocalBibleManager_H

#include <QObject>
#include <QHash>
#include <QByteArray>
#include <QTimer>

#include <QtGui>
#include <QtNetwork>

class BibleDataBookInfo
{
public:
	QString englishName;
	QString displayName;
	int chapCount;
};

// Inherit QObject so we can use QPointer if desired
class BibleData : public QObject
{
	Q_OBJECT
public:
	QString file; // file from which this data was read
	
	QString name; // displayed in ComboBox in BibleBrowser in DViz
	QString code; // used to index this version and to locate this version in BibleConnector.cpp
	
	//   english book and a local book as key
	QHash<QString, BibleDataBookInfo> bookInfo;
		
	//   book           chap       verse verseText
	QHash<QString, QHash<int, QHash<int, QString > > > bibleText;
};

class LocalBibleManager;
class BibleDataPlaceholder : public QObject
{
	Q_OBJECT
public:
	BibleDataPlaceholder(LocalBibleManager *mgr, BibleData *bible, bool disabled = false);
	BibleDataPlaceholder(LocalBibleManager *mgr, QString file, QString name, QString code, bool disabled = false);
	virtual ~BibleDataPlaceholder();
	
	QString file() { return m_file; } // file from which data is read
	QString name() { return m_name; } // cache name for sorting
	QString code() { return m_code; }; // cache code just in case
	bool disabled() { return m_disabled; }
	void setDisabled(bool flag) { m_disabled = flag; }
	
	// Record time accessed
	BibleData *data();
	
public slots:
	void releaseData();
	
private:
	void setupTimer();
	
	LocalBibleManager *m_mgr; // for loading data as needed
	QString m_file;
	QString m_name;
	QString m_code;
	BibleData *m_data; // actual data;
	QTimer m_expireTimer;
	bool m_disabled;
};

class LocalBibleManager : public QObject
{
	Q_OBJECT
public:
	static LocalBibleManager *inst();
	QList<BibleDataPlaceholder*> biblePlaceholders();
	BibleData *bibleData(QString versionCode);
	
public slots:
	void scanBiblesFolder();
	void getMoreBibles();
	
signals:
	void bibleListChanged();
	
protected:
	friend class BibleDataPlaceholder;
	
	BibleData *loadBible(QString file, bool showProgress = false, bool headerOnly = false);
	void justLoaded(BibleDataPlaceholder*); // add this placeholder to the stack and remove (and releaseData()) the one on the bottom of the stack
	
	LocalBibleManager();
	static LocalBibleManager *static_inst;
	
	QHash<QString, BibleDataPlaceholder*> m_localBibles;
	QList<BibleDataPlaceholder*> m_loadedStack;
public:
	virtual ~LocalBibleManager(){};
};

class BibleDownloadDialog : public QDialog
{
	Q_OBJECT
public:
	BibleDownloadDialog();
	virtual ~BibleDownloadDialog() {}
	
	bool listChanged() { return m_listChanged; }
	
protected slots:
	void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	void downloadFinished();
	void downloadReadyRead();
	void cancelDownload();
	
	void listItemChanged(QListWidgetItem * item);
	
	void deleteWhenDisabledChanged(bool flag);

protected:
	void setupUi();
	void processBibleIndex();
	void startDownload(QString);
	
	QNetworkAccessManager m_manager;
	QFile *m_file;
	QProgressDialog *m_progressDialog;
	QNetworkReply *m_reply;
	bool m_downloadRequestAborted;
	
	bool m_indexDownloadMode;
	QByteArray m_indexBuffer;

	QListWidget *m_listWidget;
	
	bool m_deleteDisabledFiles;
	
	bool m_listChanged;
	
};

#endif
