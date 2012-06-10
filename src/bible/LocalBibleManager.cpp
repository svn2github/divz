#include "LocalBibleManager.h"
#include <QProgressDialog>
#include <QtGui>

// The max # of Bibles to keep loaded in RAM at one time
#define BIBLE_LOADED_STACK_MAX_SIZE 3

// The number of minutes to keep Bible data loaded in RAM before freeing
#define BIBLE_DATA_RELEASE_TIMER 10



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

	
bool LocalBibleManager_compare_titles(BibleDataPlaceholder *a, BibleDataPlaceholder *b)
{
	return (a && b) ? a->name() < b->name() : true;
}

QList<BibleDataPlaceholder*> LocalBibleManager::biblePlaceholders()
{
	QList<BibleDataPlaceholder *> bibles = m_localBibles.values();
	qSort(bibles.begin(), bibles.end(), LocalBibleManager_compare_titles);
	return bibles;
}

BibleData *LocalBibleManager::bibleData(QString versionCode)
{
	if(m_localBibles.contains(versionCode))
		return m_localBibles.value(versionCode)->data();
	
	return 0;
}

void LocalBibleManager::scanBiblesFolder()
{
	QStringList loadedFiles;
	foreach(BibleDataPlaceholder *data, m_localBibles.values())
		loadedFiles << data->file();
	
	QProgressDialog progress(("Loading Bibles..."),"",0,0);
	progress.setWindowModality(Qt::WindowModal);
	progress.setCancelButton(0); // hide cancel button
	progress.setWindowIcon(QIcon(":/data/icon-d.png"));
	progress.setWindowTitle(tr("Loading Bibles"));
	progress.setValue(0);
	progress.show();
	QApplication::processEvents();
	
	QString folder = "bibles";
	
	int progressCounter = 0;
	
	QDir bibleDir(folder);
	QStringList bibles = bibleDir.entryList(QStringList() << "*.dzb" << "*.dvizbible", QDir::Files);
	
	progress.setMaximum(bibles.size());
	qDebug() << "LocalBibleManager::scanBiblesFolder(): Loading " << bibles.size() << " bibles";
	
	foreach(QString file, bibles)
	{
		progress.setValue(progressCounter++);
		progress.setLabelText(tr("Reading Bible from %1...").arg(file));
		QApplication::processEvents();
		
		if(!loadedFiles.contains(file))
		{
			BibleData *data = loadBible(tr("%1/%2").arg(folder).arg(file));
			BibleDataPlaceholder *placeholder = new BibleDataPlaceholder(data, this);
			m_localBibles[data->code] = placeholder;
			qDebug() << "LocalBibleMananger::scanBiblesFolder: Loaded bible "<<data->name<<" from "<<file;
			
			// Free data until actually accessed by the user
			placeholder->releaseData();
		}
	}
}

BibleData *LocalBibleManager::loadBible(QString fileName, bool showProgress)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly)) 
	{
		qDebug() << "LocalBibleManager::loadBible(): Unable to read "<<fileName;
		return 0;
	}
	
	QProgressDialog *progress = 0;
	
	if(showProgress)
	{
		progress = new QProgressDialog(tr("Reading %1").arg(fileName),"",0,0);
		progress->setWindowModality(Qt::WindowModal);
		progress->setCancelButton(0); // hide cancel button
		progress->setWindowIcon(QIcon(":/data/icon-d.png"));
		progress->setWindowTitle(tr("Reading %1").arg(fileName));
		progress->setValue(0);
		progress->show();
		QApplication::processEvents();
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
	
	if(showProgress)
		progress->setMaximum(bibleTextMap.keys().size());
	
	int progressCounter = 0;
	
	foreach(QString book, bibleTextMap.keys())
	{
		if(showProgress)
		{
			progress->setValue(progressCounter++);
			progress->setLabelText(tr("Loading %1").arg(book));
			QApplication::processEvents();
		}	
	
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
	
	if(showProgress)
		delete progress;

	return bibleData;
}

void LocalBibleManager::justLoaded(BibleDataPlaceholder *placeholder)
{
	// Dont list bible in stack more than once
	if(m_loadedStack.contains(placeholder))
		m_loadedStack.removeAll(placeholder);
	
	m_loadedStack.append(placeholder);
	
	if(m_loadedStack.size() > BIBLE_LOADED_STACK_MAX_SIZE)
	{
		BibleDataPlaceholder *bottom = m_loadedStack.takeFirst();
		qDebug() << "LocalBibleManager::justLoaded(): Loaded stack exceeded "<<BIBLE_LOADED_STACK_MAX_SIZE<<" items, releasing bottom item: "<<bottom->name()<<" ("<<bottom->code()<<")"; 
		bottom->releaseData();
	}
}

/// 

BibleDataPlaceholder::BibleDataPlaceholder(BibleData *bible, LocalBibleManager *mgr)
		: QObject()
		, m_mgr(mgr)
		, m_file(bible->file)
		, m_name(bible->name)
		, m_code(bible->code)
		, m_data(bible)
{
	connect(&m_expireTimer, SIGNAL(timeout()), this, SLOT(releaseData()));
	// Release Bible data after 10 minutes
	m_expireTimer.setInterval(60 * BIBLE_DATA_RELEASE_TIMER * 1000);
	m_expireTimer.setSingleShot(true);
}

BibleDataPlaceholder::~BibleDataPlaceholder()
{
	releaseData();
}

BibleData *BibleDataPlaceholder::data()
{
	m_expireTimer.start(); // restart if active
	if(!m_data)
	{
		qDebug() << "BibleDataPlaceholder::data(): Re-loading data for: "<<name() << "("<<code()<<")";
		m_data = m_mgr->loadBible(file(), true); // true =  show progress
		m_mgr->justLoaded(this); // inform the manager we just loaded our data so it can remove older data from other Bibles, if present
	}
	return m_data;
	
}

void BibleDataPlaceholder::releaseData()
{
	if(m_data)
	{
		qDebug() << "BibleDataPlaceholder::releaseData(): Releasing data for: "<<name() << "("<<code()<<")";
		delete m_data;
		m_data = 0;
	}
	m_expireTimer.stop();
}

