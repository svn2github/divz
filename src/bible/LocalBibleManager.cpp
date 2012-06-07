#include "LocalBibleManager.h"

#include <QtGui>

LocalBibleManager *LocalBibleManager::static_inst = 0;
	
LocalBibleManager::LocalBibleManager()
{
	scanBiblesFolder();
}
	
	
LocalBibleManager *LocalBibleManager::inst()
{
	if(!static_inst)
		static_inst = new LocalBibleManager();
	return static_inst;
}

QList<BibleData*> LocalBibleManager::localBibles()
{
	return m_localBibles.values();
}

BibleData *LocalBibleManager::bibleData(QString versionCode)
{
	if(m_localBibles.contains(versionCode))
		return m_localBibles.value(versionCode);
	
	return 0;
}
	
void LocalBibleManager::scanBiblesFolder()
{
	QStringList loadedFiles;
	foreach(BibleData *data, m_localBibles.values())
		loadedFiles << data->file;
	
	QString folder = "bibles";
	
	QDir bibleDir(folder);
	QStringList bibles = bibleDir.entryList(QStringList() << "*.dzb" << "*.dvizbible", QDir::Files);
	foreach(QString file, bibles)
	{
		if(!loadedFiles.contains(file))
		{
			BibleData *data = loadBible(tr("%1/%2").arg(folder).arg(file));
			m_localBibles[data->code] = data;
			qDebug() << "LocalBibleMananger::scanBiblesFolder: Loaded bible "<<data->name<<" from "<<file;
		}
	}
}

BibleData *LocalBibleManager::loadBible(QString fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly)) 
	{
		qDebug() << "LocalBibleManager::loadBible(): Unable to read "<<fileName;
		return 0;
	}
	
	QByteArray array = file.readAll();
		
	QDataStream stream(&array, QIODevice::ReadOnly);
	QVariantMap map;
	stream >> map;
	
	BibleData *bibleData = new BibleData();
	bibleData->file = fileName;
	bibleData->name = map["name"].toString();
	bibleData->code = map["code"].toString();
	
	// convert book info to a QHash
	QVariantMap bookInfoMap = map["bookNames"].toMap();
	foreach(QString key, bookInfoMap.keys())
	{
		QVariantMap data = bookInfoMap[key].toMap();
		
		BibleDataBookInfo info;
		info.englishName = data["EnglishName"].toString();
		info.displayName = data["DisplayName"].toString();
		info.chapCount   = data["ChapterCount"].toInt();
		
		bibleData->bookInfo[key] = info;
	}
	
	// load actual bible text from map
	QVariantMap bibleTextMap = map["bibleText"].toMap();
	foreach(QString book, bibleTextMap.keys())
	{
		QVariantMap bookMap = bibleTextMap[book].toMap();
		
		QHash<int, QHash<int, QString > > bookHash;
		foreach(QString chapStr, bookMap.keys())
		{
			QVariantMap chapMap = bookMap[chapStr].toMap();
			
			QHash<int, QString > chapHash;
			foreach(QString verseStr, chapMap.keys())
				chapHash[verseStr.toInt()] = chapMap[verseStr].toString();
				
			bookHash[chapStr.toInt()] = chapHash;
		}
		
		bibleData->bibleText[book] = bookHash;
	}

	return bibleData;
}
