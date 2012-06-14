#include "LocalBibleManager.h"
#include <QProgressDialog>
#include <QtGui>

// The max # of Bibles to keep loaded in RAM at one time
#define BIBLE_LOADED_STACK_MAX_SIZE 3

// The number of minutes to keep Bible data loaded in RAM before freeing
#define BIBLE_DATA_RELEASE_TIMER 10

// Location of the list of files online
#define BIBLE_INDEX_DOWNLOAD_URL "http://www.mybryanlife.com/dviz-bibles/index.txt"

// Location where Bibles are stored online - append a "/englishkjv.dzb", for example, and you got the Bible file you're looking for
#define BIBLE_DOWNLOAD_URL_BASE "http://www.mybryanlife.com/dviz-bibles"


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
	
	QList<BibleDataPlaceholder *> notDisabled;
	foreach(BibleDataPlaceholder *data, bibles)
		if(!data->disabled())
			notDisabled << data;
		
	qSort(notDisabled.begin(), notDisabled.end(), LocalBibleManager_compare_titles);
	return notDisabled;
}

BibleData *LocalBibleManager::bibleData(QString versionCode)
{
	if(m_localBibles.contains(versionCode))
		return m_localBibles.value(versionCode)->data();
	
	return 0;
}

///! scanBiblesFolder() - Scan the './bibles' folder for '*.dzb' files.
// .dzb files store 3 pieces of data for the Bible: the 'header' (name/code),
// the book index, and the actual text. We really only need the header (name/code)
// to display to the user and to make up the BibleDataPlaceholder. Only when the
// user *actually* requests the version ('code') is the book index and actual text
// actually needed in RAM.
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
	
	QSettings settings;
	settings.beginGroup("localbibles");
	
	bool listChanged = false;
	
	foreach(QString file, bibles)
	{
		progress.setValue(progressCounter++);
		progress.setLabelText(tr("Reading Bible from %1...").arg(file));
		QApplication::processEvents();
		
		QString absFile = tr("%1/%2").arg(folder).arg(file);
			
		settings.beginGroup(absFile);
		
		if(!loadedFiles.contains(absFile))
		{
			bool disabled = settings.value("disabled", false).toBool();
			bool indexed  = settings.value("indexed",  false).toBool();
			
			QString name, code;
			
			if(!indexed)
			{
				// If not already indexed, load the data from the disk
				// and store the name/code into QSettings for later use.
				BibleData *data = loadBible(absFile, false, true); // false = show progress, true = header only
				name = data->name;
				code = data->code;
				delete data;
				
				settings.setValue("indexed", true);
				settings.setValue("name",    name);
				settings.setValue("code",    code);
			}
			else
			{
				name = settings.value("name").toString();
				code = settings.value("code").toString();
			}
			
			BibleDataPlaceholder *placeholder = new BibleDataPlaceholder(this, absFile, name, code, disabled);
			m_localBibles[code] = placeholder;
			
			qDebug() << "LocalBibleMananger::scanBiblesFolder(): Bible "<<name<<" stored in "<<file;
			
			// Free data until actually accessed by the user
			placeholder->releaseData();
			
			listChanged = true;
		}
		else
		{
			bool disabled = settings.value("disabled", false).toBool();
			QString code  = settings.value("code").toString();
			qDebug() << "LocalBibleMananger::scanBiblesFolder(): Updating code "<<code<<": Disabled: "<<disabled;
			if(m_localBibles[code]->disabled() != disabled)
			{
				listChanged = true;
				m_localBibles[code]->setDisabled(disabled);
			}
		}
		
		settings.endGroup(); // group: absFile
	}
	
	settings.endGroup(); // group: "localbibles"
	
	if(listChanged)
		emit bibleListChanged();
}

BibleData *LocalBibleManager::loadBible(QString fileName, bool showProgress, bool headerOnly)
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
	
	if(headerOnly)
	{
		if(showProgress)
			delete progress;
			
		return bibleData;
	}
	
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

void LocalBibleManager::getMoreBibles()
{
	BibleDownloadDialog dialog;
	dialog.exec();
	
	if(dialog.listChanged())
		scanBiblesFolder();
}

/// 

BibleDataPlaceholder::BibleDataPlaceholder(LocalBibleManager *mgr, BibleData *bible, bool flag)
		: QObject()
		, m_mgr(mgr)
		, m_file(bible->file)
		, m_name(bible->name)
		, m_code(bible->code)
		, m_data(bible)
		, m_disabled(flag)
{
	setupTimer();
}

BibleDataPlaceholder::BibleDataPlaceholder(LocalBibleManager *mgr, QString file, QString name, QString code, bool flag)
		: QObject()
		, m_mgr(mgr)
		, m_file(file)
		, m_name(name)
		, m_code(code)
		, m_data(0)
		, m_disabled(flag)
{
	setupTimer();
}

BibleDataPlaceholder::~BibleDataPlaceholder()
{
	releaseData();
}

void BibleDataPlaceholder::setupTimer()
{
	connect(&m_expireTimer, SIGNAL(timeout()), this, SLOT(releaseData()));
	// Release Bible data after 10 minutes
	m_expireTimer.setInterval(60 * BIBLE_DATA_RELEASE_TIMER * 1000);
	m_expireTimer.setSingleShot(true);
}

BibleData *BibleDataPlaceholder::data()
{
	m_expireTimer.start(); // restart if active
	if(!m_data)
	{
		qDebug() << "BibleDataPlaceholder::data(): Re-loading data for: "<<name() << "("<<code()<<")";
		m_data = m_mgr->loadBible(file(), true); // true = show progress
		m_mgr->justLoaded(this); // inform the manager we just loaded our data so it can remove older data from other Bibles, if present
// 		qDebug() << "BibleDataPlaceholder::data(): m_data:"<<m_data;
// 		qDebug() << "BibleDataPlaceholder::data(): bookInfo test:" << m_data->bookInfo["John"].displayName;
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

///

BibleDownloadDialog::BibleDownloadDialog()
	: QDialog()
	, m_file(0)
	, m_progressDialog(0)
	, m_reply(0)
	, m_downloadRequestAborted(false)
	, m_indexDownloadMode(false)
{
	setWindowTitle("Bibles Available");
	
	setupUi();
	m_progressDialog = new QProgressDialog(this);

	QTimer::singleShot(0, this, SLOT(startDownloadingIndex()));
}

void BibleDownloadDialog::startDownloadingIndex()
{
	QUrl url(BIBLE_INDEX_DOWNLOAD_URL);

	m_file = NULL;
	m_indexDownloadMode = true;

	m_downloadRequestAborted = false;
	m_reply = m_manager.get(QNetworkRequest(url));
	connect(m_reply, SIGNAL(finished()),  this, SLOT(downloadFinished()));
	connect(m_reply, SIGNAL(readyRead()), this, SLOT(downloadReadyRead()));
	connect(m_reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
	connect(m_progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));

	m_listWidget->setEnabled(false);

	m_progressDialog->setWindowTitle("Downloading Bible List");
	m_progressDialog->setLabelText(tr("Downloading list of available bibles..."));
	m_progressDialog->show();
}

void BibleDownloadDialog::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	if(m_downloadRequestAborted)
		return;
	
	m_progressDialog->setMaximum(bytesTotal);
	m_progressDialog->setValue(bytesReceived);
}

void BibleDownloadDialog::downloadReadyRead()
{
	if(!m_reply || !m_reply->bytesAvailable())
	    return;

	if(m_indexDownloadMode)
		m_indexBuffer.append(m_reply->readAll());
	else
	if(m_file)
		m_file->write(m_reply->readAll());
}

void BibleDownloadDialog::cancelDownload()
{
	qDebug() << "BibleDownloadDialog::cancelDownload()";
	m_downloadRequestAborted = true;
	m_reply->abort();
	
	if(m_indexDownloadMode)
	{
		reject();
		//deleteLater();
	}
		
	m_listWidget->setEnabled(true);
}

void BibleDownloadDialog::downloadFinished()
{
	//qDebug() << "BibleDownloadDialog::downloadFinished(): Signal for: "<<m_reply->url().toString()<<", processing...";
	m_progressDialog->hide();

	// Make sure we've read all the data from the socket
	downloadReadyRead();

	if(m_file)
	{
		m_file->flush();
		m_file->close();
	}

	m_listWidget->setEnabled(true);

	if(m_downloadRequestAborted)
	{
		if(m_file)
		{
			m_file->remove();
			delete m_file;
			m_file = NULL;
		}
		m_reply->deleteLater();
		return;
	}

	if(m_reply->error() && m_reply->errorString() != "Connection closed")
	{
		//Download failed

		qDebug() << "BibleDownloadDialog::downloadFinished(): Error downloading "<<m_reply->url().toString()<<": "<<m_reply->errorString();
		if(m_indexDownloadMode)
		{
			qDebug() << "BibleDownloadDialog::downloadFinished(): m_indexDownloadMode, closing dialog";
			reject();
		}

		QMessageBox::information(this, "Download failed", tr("Error getting %2:\n%1").arg(m_reply->errorString()).arg(m_reply->url().toString()));
	}
	else
	if(m_indexDownloadMode)
	{
		m_indexDownloadMode = false;
		processBibleIndex();

		m_indexBuffer.clear();
	}

	qDebug() << "BibleDownloadDialog::downloadFinished(): Done downloading: "<<m_reply->url().toString();

	#ifndef Q_WS_WIN
	/// NOTE - YES, this DOES leak memory if not enabled - but
	/// for some WIERD reason, this causes a crash on Windows - so disabling for now.
	if(m_reply)
	{
		m_reply->deleteLater();
		m_reply = NULL;
	}
	#endif


	if(m_file)
	{
		delete m_file;
		m_file = NULL;
	}

}


bool BibleDownloadDialog_compare_titles(BibleIndexData a, BibleIndexData b)
{
	return a.name < b.name;
}

void BibleDownloadDialog::processBibleIndex()
{
	/*
	QString indexBuffer = "afrikaans.dzb,Afrikaans Bible,AFRIKAANS\n"
				"albanian.dzb,Albanian Bible,ALBANIAN\n"
				"croatian.dzb,Croatian Bible,CROATIAN\n"
				"czech.dzb,Czech  Bible,CZECH\n"
				"czechcep.dzb,Czech Cep Bible,CZECHCEP\n"
				"czechekumenicka.dzb,Czech Ekumenicka Bible,CZECHEKUMENICKA\n"
				"danish.dzb,Danish Bible,DANISH\n"
				"dari.dzb,Dari Bible,DARI\n"
				"dutch.dzb,Dutch Bible,DUTCH\n";
	*/
	QString indexBuffer = QString(m_indexBuffer);

	QStringList lines = indexBuffer.split("\n");
	
	QSettings settings;
	settings.beginGroup("localbibles");
	
	QString folder = "bibles";

	// We'll add data to this list below
	m_indexList.clear();

	foreach(QString entry, lines)
	{
		if(entry.isEmpty())
			continue;
			
		// Parse the line by spltting on commas
		QStringList fields = entry.split(",");
		if(fields.size() < 3)
		{
			qDebug() << "BibleDownloadDialog::processBibleIndex(): Problem parsing line:"<<entry<<": Too few fields";
			continue;
		}
		 
		QString file = fields[0];
		QString name = fields[1];
		QString code = fields[2];
		
		// Add/update the values in QSettings for this entry
		QString absFile = tr("%1/%2").arg(folder).arg(file);
		settings.beginGroup(absFile);
		
		settings.setValue("indexed", true);
		settings.setValue("name",    name);
		settings.setValue("code",    code);
		settings.setValue("url",     tr("%1/%2").arg(BIBLE_DOWNLOAD_URL_BASE).arg(file));
		
		bool exists   = QFileInfo(absFile).exists();
		bool disabled = settings.value("disabled", exists ? false : true).toBool();
		
		settings.endGroup();
		
		//qDebug() << "BibleDownloadDialog::processBibleIndex(): Indexed: "<<absFile<<": "<<name;
		
		/*
		// Add the item to the QListWidget
		QListWidgetItem *item = new QListWidgetItem(name);
		item->setCheckState(disabled ? Qt::Unchecked : Qt::Checked);
		item->setData(Qt::UserRole, absFile); 
		
		m_listWidget->addItem(item);
		*/

		BibleIndexData indexItem;
		indexItem.name = name;
		indexItem.disabled = disabled || !exists;
		indexItem.absFile = absFile;
		m_indexList << indexItem;
	}

	qSort(m_indexList.begin(), m_indexList.end(), BibleDownloadDialog_compare_titles);

	populateList();
}

void BibleDownloadDialog::populateList(QString filter)
{
	// Make lowercase for case-insensitive matching
	filter = filter.toLower();

	m_listWidget->clear();
	foreach(BibleIndexData indexItem, m_indexList)
	{
		if(!filter.isEmpty() &&
		   !indexItem.name.toLower().contains(filter))
		   continue;

		// Add the item to the QListWidget
		QListWidgetItem *item = new QListWidgetItem(indexItem.name);
		item->setCheckState(indexItem.disabled ? Qt::Unchecked : Qt::Checked);
		item->setData(Qt::UserRole, indexItem.absFile);

		m_listWidget->addItem(item);
	}
}


void BibleDownloadDialog::listItemChanged(QListWidgetItem * item)
{
	// extract value for enabled/disabled
	bool checked = item->checkState() == Qt::Checked;
	
	// Load bible settings
	QSettings settings;
	settings.beginGroup("localbibles");
	
	// Grab the filename for this list item and start that settings group
	QString absFile = item->data(Qt::UserRole).toString();
	settings.beginGroup(absFile);
	
	// Check to see if the file exists and if its disabled currently
	bool exists = QFileInfo(absFile).exists();
		
	// m_listChanged is used by LocalBibleManager to know if it should rescan the Bibles folder and/or emit a signal
	m_listChanged = true;
	
	bool disabled = !checked;
	settings.setValue("disabled", disabled);
	
	//qDebug() << "BibleDownloadDialog::listItemChanged(): "<<absFile<<": disabled:"<<disabled<<", exists:"<<exists<<", delete:"<<m_deleteDisabledFiles;
	
	if(checked && exists)
	{
		//qDebug() << "BibleDownloadDialog::listItemChanged(): "<<absFile<<": checked && exists, nothing to do here";
		// nothing to do here, just update settings (above)
		return;
	}
	
	if(checked && !exists)
	{
		//qDebug() << "BibleDownloadDialog::listItemChanged(): "<<absFile<<": checked && !exists, downloading";
		startDownload(settings.value("url").toString());
		return;
	}
	
	if(!checked && exists && m_deleteDisabledFiles)
	{
		//qDebug() << "BibleDownloadDialog::listItemChanged(): "<<absFile<<": !checked && exists && delete: Removing "<<absFile;
		QFile::remove(absFile);
		return;
	}
}

void BibleDownloadDialog::startDownload(QString urlString)
{
	QUrl url(urlString);
	QString shortFilename = QFileInfo(url.path()).fileName();
	QString filename = tr("bibles/%2").arg(shortFilename);
	
	qDebug() << "BibleDownloadDialog::startDownload(): "<<urlString;
 
	if(QFile::exists(filename))
	{
		if(QMessageBox::question(this, tr("Downloader"),
			tr("There already exists a file called %1 in "
				"the 'bibles' directory. Overwrite?").arg(filename),
				QMessageBox::Yes|QMessageBox::No, QMessageBox::No)
				== QMessageBox::No)
				return;
		QFile::remove(filename);
	}
 
	m_file = new QFile(filename);
	if(!m_file->open(QIODevice::WriteOnly))
	{
		QMessageBox::information(this, "Downloader", 
			tr("Unable to save the file %1: %2.")
			.arg(filename).arg(m_file->errorString()));
		delete m_file;
		m_file = NULL;
		return;
	}
 
	m_downloadRequestAborted = false;
	m_reply = m_manager.get(QNetworkRequest(url));
	connect(m_reply, SIGNAL(finished()),  this, SLOT(downloadFinished()));
	connect(m_reply, SIGNAL(readyRead()), this, SLOT(downloadReadyRead()));
	connect(m_reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
	connect(m_progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));
 
	m_progressDialog->setWindowTitle(tr("Downloading %1").arg(shortFilename));
	m_progressDialog->setLabelText(tr("Downloading %1...").arg(shortFilename));
	m_listWidget->setEnabled(false);
	m_progressDialog->show();
}

void BibleDownloadDialog::deleteWhenDisabledChanged(bool flag)
{
	m_deleteDisabledFiles = flag;
	
	QSettings settings;
	settings.beginGroup("localbibles");
	
	settings.setValue("deleteDisabledFiles", flag);
}

void BibleDownloadDialog::setupUi()
{
	QVBoxLayout *vbox = new QVBoxLayout(this);
	
	vbox->addWidget(new QLabel("Select Bibles from the list below and\nthey will be downloaded automatically."));

	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(new QLabel("Search: "));

	QLineEdit *searchBox = new QLineEdit();
	hbox->addWidget(searchBox);
	connect(searchBox, SIGNAL(textChanged(QString)), this, SLOT(populateList(QString)));

	vbox->addLayout(hbox);
	
	m_listWidget = new QListWidget();
	connect(m_listWidget, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(listItemChanged(QListWidgetItem *)));
	vbox->addWidget(m_listWidget);
	
	QSettings settings;
	settings.beginGroup("localbibles");
	
	m_deleteDisabledFiles = settings.value("deleteDisabledFiles", false).toBool();
	
	QCheckBox *cb = new QCheckBox("Delete Bibles from disk when disabled\n(Will be downloaded again as needed.)");
	cb->setChecked(m_deleteDisabledFiles);
	connect(cb, SIGNAL(toggled(bool)), this, SLOT(deleteWhenDisabledChanged(bool)));
	vbox->addWidget(cb);
}
