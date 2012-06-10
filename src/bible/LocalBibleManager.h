#ifndef LocalBibleManager_H
#define LocalBibleManager_H

#include <QObject>
#include <QHash>
#include <QByteArray>
#include <QTimer>

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
	BibleDataPlaceholder(BibleData *bible, LocalBibleManager *mgr);
	virtual ~BibleDataPlaceholder();
	
	QString file() { return m_file; } // file from which data is read
	QString name() { return m_name; } // cache name for sorting
	QString code() { return m_code; }; // cache code just in case
	
	// Record time accessed
	BibleData *data();
	
public slots:
	void releaseData();
	
private:
	LocalBibleManager *m_mgr; // for loading data as needed
	QString m_file;
	QString m_name;
	QString m_code;
	BibleData *m_data; // actual data;
	QTimer m_expireTimer;
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
	
protected:
	friend class BibleDataPlaceholder;
	
	BibleData *loadBible(QString file, bool showProgress = false);
	void justLoaded(BibleDataPlaceholder*); // add this placeholder to the stack and remove (and releaseData()) the one on the bottom of the stack
	
	LocalBibleManager();
	static LocalBibleManager *static_inst;
	
	QHash<QString, BibleDataPlaceholder*> m_localBibles;
	QList<BibleDataPlaceholder*> m_loadedStack;
public:
	virtual ~LocalBibleManager(){};
};

#endif
