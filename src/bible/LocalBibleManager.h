#ifndef LocalBibleManager_H
#define LocalBibleManager_H

#include <QObject>
#include <QHash>
#include <QByteArray>

class BibleDataBookInfo
{
public:
	QString englishName;
	QString displayName;
	int chapCount;
};

class BibleData
{
public:
	QString file; // file from which this data was read
	
	QString name; // displayed in ComboBox in BibleBrowser in DViz
	QString code; // used to index this version and to locate this version in BibleConnector.cpp
	
	//   english book and a local book as key
	QHash<QString, BibleDataBookInfo> bookInfo;
		
	//   book           chap       verse verseText
	QHash<QString, QHash<int, QHash<int, QString > > > bibleText;
};

class LocalBibleManager : public QObject
{
	Q_OBJECT
public:
	static LocalBibleManager *inst();
	QList<BibleData*> localBibles();
	BibleData *bibleData(QString versionCode);
	
public slots:
	void scanBiblesFolder();
	
private:
	BibleData *loadBible(QString file);
	
	LocalBibleManager();
	static LocalBibleManager *static_inst;
	
	QHash<QString, BibleData*> m_localBibles;
public:
	virtual ~LocalBibleManager(){};
};

#endif
