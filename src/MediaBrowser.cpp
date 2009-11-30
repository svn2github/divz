#include "MediaBrowser.h"

#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QFileSystemModel>
#include <QFileInfo>
#include <QListView>
#include <QDebug>
#include <QComboBox>
#include <QPixmapCache>
#include <QFileIconProvider>
#include <QDir>
#include <QGraphicsView>
#include <QSplitter>

#include "qvideo/QVideoProvider.h"
#include "3rdparty/md5/md5.h"
#include "AppSettings.h"

#include "SlideGroupViewer.h"
#include "model/SlideGroup.h"
#include "model/Slide.h"

#include "DirectoryListModel.h"

#include "DeepProgressIndicator.h"

#define CACHE_DIR "dviz-imageiconcache"


/* We reimplement QListView's keyPressEvent to detect
  selection changes on key press events in QListView::ListMode.
  Aparently, in list mode the selection model's currentChanged()
  signal doesn't get fired on keypress, but in IconMode it does.
  We use IconMode by default in the ViewControl below, but the
  SongSlideGroupViewControl uses ListMode - this allows either
  icon or list mode to change slides just by pressing up or down
*/
class MediaBrowserQListView : public QListView
{
public:
	MediaBrowserQListView(QWidget * ctrl) : QListView(ctrl) {}
	void keyPressEvent(QKeyEvent *event)
	{
		QModelIndex oldIdx = currentIndex();
		QListView::keyPressEvent(event);
		QModelIndex newIdx = currentIndex();
		if(oldIdx.row() != newIdx.row())
		{
			emit clicked(newIdx);
		}
	}
};


QRegExp MediaBrowser::videoRegexp = QRegExp("(wmv|mpeg|mpg|avi|wmv|flv|mov|mp4|m4a|3gp|3g2|mj2|mjpeg|ipod|m4v|gsm|swf|dv|dvd|asf|mtv|roq|aac|ac3|aiff|alaw|iif)",Qt::CaseInsensitive);
QRegExp MediaBrowser::imageRegexp = QRegExp("(png|jpg|bmp|svg|xpm|gif)",Qt::CaseInsensitive);

bool MediaBrowser::isVideo(const QString &extension) { return extension.indexOf(videoRegexp) == 0; }
bool MediaBrowser::isImage(const QString &extension) { return extension.indexOf(imageRegexp) == 0; }

QPixmap MediaBrowser::iconForImage(const QString & file, const QSize & size)
{
	QPixmap cache;
	QDir path(QString("%1/%2").arg(AppSettings::cachePath()).arg(CACHE_DIR));
	if(!path.exists())
		QDir(AppSettings::cachePath()).mkdir(CACHE_DIR);

	QString cacheFile = QString("%1/%2/%3-%4x%5")
				.arg(AppSettings::cachePath())
				.arg(CACHE_DIR)
				.arg(MD5::md5sum(file))
				.arg(size.width())
				.arg(size.height());


	//qDebug() << "MediaBrowser::iconForImage: file:"<<file<<", size:"<<size<<", cacheFile: "<<cacheFile;
	//QPixmap orig(file);
	//orig = orig.scaled(size,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
	//orig.detach();

	//return orig;

	if(!QPixmapCache::find(cacheFile,cache))
	{
		if(QFile(cacheFile).exists())
		{
			cache.load(cacheFile);
			QPixmapCache::insert(cacheFile,cache);
			//qDebug() << "MediaBrowser::iconForImage: file:"<<file<<", size:"<<size<<": hit DISK (loaded scaled from disk cache)";
		}
		else
		{
			QPixmap orig(file);
			if(orig.isNull())
			{
				cache = QPixmap();
				QPixmapCache::insert(cacheFile,cache);
				qDebug() << "MediaBrowser::iconForImage: file:"<<file<<", size:"<<size<<": load INVALID (Can't load original)";
			}
			else
			{
				cache = orig.scaled(size,Qt::KeepAspectRatio,Qt::SmoothTransformation);
				cache.save(cacheFile,"PNG");
				////qDebug() << "MyQFileIconProvider::icon(): image file: caching to:"<<cacheFile<<" for "<<file;
				QPixmapCache::insert(cacheFile,cache);
				//qDebug() << "MediaBrowser::iconForImage: file:"<<file<<", size:"<<size<<": load GOOD (loaded original and scaled)";
				//QApplication::processEvents();
			}
		}
	}
	else
	{
		//qDebug() << "MediaBrowser::iconForImage: file:"<<file<<", size:"<<size<<": hit RAM (scaled image already in ram)";
	}

	//qDebug() << "MediaBrowser::data: iconForImage: file:"<<file<<", cacheKey:"<<cache.cacheKey();

	return cache;
}

class MyQFileIconProvider : public QFileIconProvider
{
public:
	MyQFileIconProvider() : QFileIconProvider(), m_iconSize(MEDIABROWSER_LIST_ICON_SIZE)  {}

	QIcon icon(const QFileInfo& info) const
	{
		DeepProgressIndicator * d = DeepProgressIndicator::indicatorForObject(dynamic_cast<void*>(const_cast<MyQFileIconProvider*>(this)));
		if(d)
			d->step();


		QApplication::processEvents();
		if(MediaBrowser::isVideo(info.suffix()))
		{
			//qDebug() << "MyQFileIconProvider::icon(): video file:"<<info.absoluteFilePath();
			return QVideoProvider::iconForFile(info.absoluteFilePath());
			//return QFileIconProvider::icon(info);
		}
		else
		if(MediaBrowser::isImage(info.suffix()))
		{
			//qDebug() << "MyQFileIconProvider::icon(): image file:"<<info.absoluteFilePath();
			return MediaBrowser::iconForImage(info.absoluteFilePath(),m_iconSize);
		}
		else
		{
			return QFileIconProvider::icon(info);
		}
	}

	void setIconSize(QSize s) { m_iconSize = s; }
private:
	QSize m_iconSize;

};
/*
class MediaBrowserDirectoryModel : public DirectoryListModel
{
public:
	MediaBrowserDirectoryModel() : DirectoryListModel() {}
	~MediaBrowserDirectoryModel() {}
	
protected:
	QPixmap generatePixmap(const QFileInfo& info)
	{
	
	}

};*/


MediaBrowser::MediaBrowser(const QString &directory, QWidget *parent)
	: QWidget(parent)
	, m_currentDirectory("")
	, m_prevPathKey("media")
	, m_backgroundActionsEnabled(true)
	, m_iconSize(MEDIABROWSER_LIST_ICON_SIZE)
{
	setObjectName("MediaBrowser");
	setupUI();

	QStringList filters;

	filters << tr("Media Files (*.wmv *.mpeg *.mpg *.avi *.wmv *.flv *.mov *.mp4 *.m4a *.3gp *.3g2 *.mj2 *.mjpeg *.ipod *.m4v *.gsm *.gif *.swf *.dv *.dvd *.asf *.mtv *.roq *.aac *.ac3 *.aiff *.alaw *.iif *.png *.jpg *.bmp *.svg *.xpm)");
	filters << tr("Video Files (*.wmv *.mpeg *.mpg *.avi *.wmv *.flv *.mov *.mp4 *.m4a *.3gp *.3g2 *.mj2 *.mjpeg *.ipod *.m4v *.gsm *.swf *.dv *.dvd *.asf *.mtv *.roq *.aac *.ac3 *.aiff *.alaw *.iif)");
	filters << tr("Image Files (*.png *.jpg *.bmp *.svg *.xpm *.gif)");
	filters << tr("Any File (*.*)");

	setFileTypeFilterList(filters);

	setDirectory(directory.isEmpty() ? AppSettings::previousPath(m_prevPathKey) : directory);
}

MediaBrowser::~MediaBrowser() 
{
	QFileIconProvider * p = m_fsModel->iconProvider();
	if(dynamic_cast<MyQFileIconProvider *>(p))
		delete p;
	m_fsModel->setIconProvider(0);	
	delete p;
}
#define SET_MARGIN(layout,margin) \
	layout->setContentsMargins(margin,margin,margin,margin);

void MediaBrowser::setupUI()
{
	QVBoxLayout *vbox0 = new QVBoxLayout(this);
	m_splitter = new QSplitter(this);
	m_splitter->setOrientation(Qt::Vertical);
	vbox0->addWidget(m_splitter);

	QWidget *browser = new QWidget(m_splitter);
	QVBoxLayout *vbox = new QVBoxLayout(browser);
	SET_MARGIN(vbox,0);

	// Setup filter box at the top of the widget
	m_searchBase = new QWidget(browser);

	QHBoxLayout *hbox = new QHBoxLayout(m_searchBase);
	SET_MARGIN(hbox,0);

	m_btnBack = new QPushButton(QIcon(":/data/stock-go-back.png"),"");
	m_btnBack->setEnabled(false);
	connect(m_btnBack, SIGNAL(clicked()), this, SLOT(goBack()));

	m_btnForward = new QPushButton(QIcon(":/data/stock-go-forward.png"),"");
	m_btnForward->setEnabled(false);
	connect(m_btnForward, SIGNAL(clicked()), this, SLOT(goForward()));

	m_btnUp = new QPushButton(QIcon(":/data/stock-go-up.png"),"");
	m_btnUp->setEnabled(false);
	connect(m_btnUp, SIGNAL(clicked()), this, SLOT(goUp()));

	QLabel *label = new QLabel("Se&arch:");
	m_searchBox = new QLineEdit(m_searchBase);
	label->setBuddy(m_searchBox);

	m_clearSearchBtn = new QPushButton(QIcon(":/data/stock-clear.png"),"");
	m_clearSearchBtn->setVisible(false);

	hbox->addWidget(m_btnBack);
	hbox->addWidget(m_btnForward);
	hbox->addWidget(m_btnUp);
	hbox->addWidget(label);
	hbox->addWidget(m_searchBox);
	hbox->addWidget(m_clearSearchBtn);

	connect(m_searchBox, SIGNAL(textChanged(const QString &)), this, SLOT(filterChanged(const QString &)));
	connect(m_searchBox, SIGNAL(returnPressed()), this, SLOT(filterReturnPressed()));
	connect(m_clearSearchBtn, SIGNAL(clicked()), this, SLOT(clearFilter()));
	
	// Now for the list itself
	m_listView = new MediaBrowserQListView(browser);
	m_listView->setAlternatingRowColors(true);
	m_listView->setIconSize(m_iconSize);
	//m_listView->setWrapping(true);
	m_listView->setWordWrap(true);
	//m_listView->setGridSize(MEDIABROWSER_LIST_ICON_SIZE);
	m_listView->setLayoutMode(QListView::Batched);
	//m_listView->setFlow(QListView::LeftToRight);
	m_listView->setResizeMode(QListView::Adjust);
	//m_listView->setSelectionMode(QAbstractItemView::SingleSelection);

	// below doesnt seem to be enough
	//m_listView->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);

	m_fsModel = new DirectoryListModel(browser); //QFileSystemModel(browser);
	m_fsModel->setIconProvider(new MyQFileIconProvider());
	//m_fsModel->setNameFilterDisables(false);

	m_listView->setModel(m_fsModel);

	connect(m_listView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(indexDoubleClicked(const QModelIndex &)));
	connect(m_listView,       SIGNAL(clicked(const QModelIndex &)), this, SLOT(indexSingleClicked(const QModelIndex &)));

	// Add action buttons
	m_btnBase = new QWidget(browser);
	QHBoxLayout *hbox3 = new QHBoxLayout(m_btnBase);
	SET_MARGIN(hbox3,0);

	m_btnSetAsBgLive = new QPushButton(QIcon(":/data/stock-apply-ffwd.png"),"");
	m_btnSetAsBgLive->setToolTip("Set selected file as background for the current live slide");
	hbox3->addWidget(m_btnSetAsBgLive);

	m_btnAddToSchedue = new QPushButton(QIcon(":/data/stock-add.png"),"");
	m_btnAddToSchedue->setToolTip("Add selected file to schedule");
	hbox3->addWidget(m_btnAddToSchedue);

	m_btnSetAsBgLater = new QPushButton(QIcon(":/data/stock-apply-next.png"),"");
	m_btnSetAsBgLater->setToolTip("Set selected file as background for the NEXT slide to go live");
	hbox3->addWidget(m_btnSetAsBgLater);

	m_btnSetAsBgCurrent = new QPushButton(QIcon(":/data/stock-apply-pause.png"),"");
	m_btnSetAsBgCurrent->setToolTip("Set selected file as background for the selected slide group");
	hbox3->addWidget(m_btnSetAsBgCurrent);

	connect(m_btnAddToSchedue, SIGNAL(clicked()), this, SLOT(slotAddToSchedule()));
	connect(m_btnSetAsBgCurrent, SIGNAL(clicked()), this, SLOT(slotSetAsBgCurrent()));
	connect(m_btnSetAsBgLater, SIGNAL(clicked()), this, SLOT(slotSetAsBgLater()));
	connect(m_btnSetAsBgLive, SIGNAL(clicked()), this, SLOT(slotSetAsBgLive()));

	// enabled by indexSingleClicked()
	m_btnBase->setEnabled(false);

	// Add the directory box and filter box at bottom
	m_folderBoxBase = new QWidget(browser);
	QHBoxLayout *hbox2 = new QHBoxLayout(m_folderBoxBase);
	SET_MARGIN(hbox2,0);

	m_filterBox = new QComboBox(m_folderBoxBase);
	hbox2->addWidget(m_filterBox);

	m_dirBox = new QLineEdit(m_folderBoxBase);
	hbox2->addWidget(m_dirBox);

	connect(m_dirBox, SIGNAL(returnPressed()), this, SLOT(dirBoxReturnPressed()));
	connect(m_filterBox, SIGNAL(currentIndexChanged(int)), this, SLOT(fileTypeChanged(int)));


	QFrame * line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
	
	vbox->addWidget(m_searchBase);
	vbox->addWidget(line);
	vbox->addWidget(m_folderBoxBase);
	vbox->addWidget(m_listView);
	vbox->addWidget(m_btnBase);
	
	m_splitter->addWidget(browser);


	m_viewer = new SlideGroupViewer(m_splitter);
	m_viewer->setCanZoom(true);

	Slide * slide = new Slide();
	AbstractVisualItem * bg = dynamic_cast<AbstractVisualItem*>(slide->background());
	bg->setFillType(AbstractVisualItem::Solid);
	bg->setFillBrush(Qt::black);

	SlideGroup *group = new SlideGroup();
	group->addSlide(slide);
	m_viewer->setSlideGroup(group);

	m_splitter->addWidget(m_viewer);
}

QByteArray MediaBrowser::saveState()
{
	return m_splitter->saveState();
}

bool MediaBrowser::restoreState(const QByteArray &state)
{
	return m_splitter->restoreState(state);
}

void MediaBrowser::setPreviousPathKey(const QString& key)
{
	m_prevPathKey = key;
	setDirectory(AppSettings::previousPath(m_prevPathKey));
}

void MediaBrowser::setViewMode(QListView::ViewMode mode)
{
	m_listView->setViewMode(mode);
	if(mode == QListView::ListMode)
		m_listView->setAlternatingRowColors(true);
	else
		m_listView->setAlternatingRowColors(false);
		
}

QListView::ViewMode MediaBrowser::viewMode()
{
	return m_listView->viewMode();
}

void MediaBrowser::setIconSize(const QSize & size)
{
	DeepProgressIndicator *d = new DeepProgressIndicator(m_fsModel->iconProvider(),this);
	d->setText(QString("Updating Icons..."));
	d->setTitle("Updating Icons");
	d->setSize(100);

	m_fsModel->setIconSize(size);
	m_listView->setIconSize(size);
	MyQFileIconProvider * p = dynamic_cast<MyQFileIconProvider*>(m_fsModel->iconProvider());
	if(p)
		p->setIconSize(size);

	d->close();
	//d->deleteLater();
	d->dialog()->close();
	d->close();
	delete d;

}

void MediaBrowser::setSplitterOrientation(Qt::Orientation o)
{
	m_splitter->setOrientation(o);
}

Qt::Orientation MediaBrowser::splitterOrientation()
{
	return m_splitter->orientation();
}

void MediaBrowser::setBackgroundActionsEnabled(bool flag)
{
	m_btnBase->setVisible(flag);
	m_backgroundActionsEnabled = flag;
}

void MediaBrowser::setFileTypeFilterList(QStringList list)
{
	static QRegExp parse1("(.*)\\s*\\(([^\\)]+)\\)");

	m_filterBox->clear();
	foreach(QString string, list)
	{
		if(parse1.indexIn(string)>-1)
		{
			QStringList cap = parse1.capturedTexts();
			m_filterBox->addItem(cap.at(1), cap.at(2));
		}
	}

	int idx = m_filterBox->currentIndex();
	if(idx<0)
		idx = 0;

	fileTypeChanged(idx);
}

void MediaBrowser::fileTypeChanged(int selectedIndex)
{
	QString type = m_filterBox->itemData(selectedIndex).toString();

	static QRegExp parse2("(\\*\\.\\w+)\\s*");

	QString list = type;

	m_currentTypeFilterList.clear();
	int pos=0;
	int count=0;
	while(pos>=0)
	{
		pos = parse2.indexIn(list,pos);
		if(pos>=0)
		{
			pos += parse2.matchedLength();
			m_currentTypeFilterList.append(parse2.capturedTexts().at(1));
			count ++;
		}
	}

// 		if(count == 0)
// 		{
// 			qDebug() << "MediaBrowser::fileTypeChanged: parse2 didnt match:"<<list;
// 		}
// 		else
// 		{
// 			qDebug() << "MediaBrowser::fileTypeChanged: parse2 matched:"<<m_currentTypeFilterList;
// 		}

	// re-apply the filters to the file model
	filterChanged(m_searchBox->text());

}

void MediaBrowser::slotAddToSchedule()
{
	QModelIndex idx = m_listView->currentIndex();
	if(idx.isValid())
	{
		QFileInfo info = m_fsModel->fileInfo(idx);
		emit fileSelected(info);
	}
}

void MediaBrowser::slotSetAsBgCurrent()
{
	QModelIndex idx = m_listView->currentIndex();
	if(idx.isValid())
	{
		QFileInfo info = m_fsModel->fileInfo(idx);
		emit setSelectedBackground(info);
	}
}

void MediaBrowser::slotSetAsBgLater()
{
	QModelIndex idx = m_listView->currentIndex();
	if(idx.isValid())
	{
		QFileInfo info = m_fsModel->fileInfo(idx);
		emit setLiveBackground(info,true);
	}
}


void MediaBrowser::slotSetAsBgLive()
{
	QModelIndex idx = m_listView->currentIndex();
	if(idx.isValid())
	{
		QFileInfo info = m_fsModel->fileInfo(idx);
		emit setLiveBackground(info,false);
	}
}



void MediaBrowser::filterReturnPressed()
{
 	QModelIndex idx = m_fsModel->indexForRow(0); //,0);
 	if(idx.isValid())
 		indexDoubleClicked(idx);
}

void MediaBrowser::dirBoxReturnPressed()
{
 	QString dir = m_dirBox->text();
 	if(!dir.isEmpty())
 		setDirectory(dir);
}


void MediaBrowser::indexDoubleClicked(const QModelIndex &idx)
{
	QFileInfo info = m_fsModel->fileInfo(idx);
	if(info.isDir())
	{
		QString path = info.filePath();
		setDirectory(path);
	}
	else
	{
		if(m_backgroundActionsEnabled)
			emit setLiveBackground(info,false);
		else
			emit fileSelected(info);

		emit fileDoubleClicked(info);
	}
}

void MediaBrowser::indexSingleClicked(const QModelIndex &idx)
{
	QFileInfo info = m_fsModel->fileInfo(idx);
	m_btnBase->setEnabled(idx.isValid() && !info.isDir());

	if(SlideGroup::canUseBackground(info))
		m_viewer->slideGroup()->changeBackground(info);
	else
		m_viewer->slideGroup()->changeBackground(AbstractVisualItem::Solid,"#000");

	if(!m_backgroundActionsEnabled)
		emit fileSelected(info);
}

void MediaBrowser::setDirectory(const QString &path, bool addToHistory)
{
	//qDebug() << "setDirectory(): setting folder path:"<<path;

	QString directory = path;
	QString file = "";

	QFileInfo info(path);
	if(info.isFile())
	{
		directory = info.absolutePath();
		file = info.fileName();
	}

	//qDebug() << "MediaBrowser::setDirectory: path:"<<path<<", is file:"<<info.isFile()<<", directory:"<<directory<<", file:"<<file;

	QModelIndex idx = m_listView->currentIndex();
	if(idx.isValid() && !m_currentDirectory.isEmpty())
		m_lastIndexForPath[m_currentDirectory] = idx;

	DeepProgressIndicator *d = new DeepProgressIndicator(m_fsModel->iconProvider(),this);
	d->setText(QString("Loading %1...").arg(path));
	d->setTitle("Loading Folder");
	d->setSize(100);

	//QModelIndex root = 
	m_fsModel->setDirectory(directory);
	//m_listView->setRootIndex(root);

	d->close();
	//d->deleteLater();
	d->dialog()->close();
	d->close();
	delete d;


	if(m_lastIndexForPath.contains(directory))
	{
		QModelIndex idx = m_lastIndexForPath[directory];
		if(idx.isValid())
			m_listView->setCurrentIndex(idx);
	}

	if(addToHistory && !m_currentDirectory.isEmpty())
	{
		pushBackward(m_currentDirectory);
		clearForward();
	}

	AppSettings::setPreviousPath(m_prevPathKey,directory);

	m_currentDirectory = directory;
	checkCanGoUp();

	m_dirBox->setText(path);

	if(!file.isEmpty())
	{
		QModelIndex idx = m_fsModel->indexForFile(path);
		if(idx.isValid())
		{
			m_listView->setCurrentIndex(idx);
			indexSingleClicked(idx);
		}
	}

	m_listView->setFocus(Qt::OtherFocusReason);
}

bool MediaBrowser::checkCanGoUp()
{
	QFileInfo info(m_currentDirectory);
	QString path = info.canonicalFilePath();

	//QDir::separator()
	QString sep = "/"; // even on windows, canonicalFilePath() uses '/' as the separator

	QStringList folders = path.split(sep);

	if(folders.size() <= 1)
	{
		//qDebug() << "checkCanGoUp(): False, can't go up from:"<<path<<", folder list:"<<folders<<", sep:"<<QDir::separator();
		m_btnUp->setEnabled(false);
		return false;
	}
	else
	{
		//qDebug() << "checkCanGoUp(): True, can go up from:"<<path;
		m_btnUp->setEnabled(true);
		return true;
	}
}

void MediaBrowser::goUp()
{
	if(!checkCanGoUp())
		return;

	QFileInfo info(m_currentDirectory);
	QString path = info.canonicalFilePath();

	QString sep = "/"; // even on windows, canonicalFilePath() uses '/' as the separator
	QStringList folders = path.split(sep); //QDir::separator());
	folders.takeLast();

	QString newPath = folders.join(sep); //QDir::separator());

	setDirectory(newPath);

	//qDebug() << "goUp(): newPath:"<<newPath;
}

void MediaBrowser::goBack()
{
	QString path = popBackward();
	if(!m_currentDirectory.isEmpty())
		unshiftForward(m_currentDirectory);
	setDirectory(path,false);

	//qDebug() << "goBack(): path:"<<path;
}

void MediaBrowser::goForward()
{
	QString path = shiftForward();
	if(!m_currentDirectory.isEmpty())
		pushBackward(m_currentDirectory);
	setDirectory(path,false);

	//qDebug() << "goForward(): path:"<<path;
}

void MediaBrowser::clearForward()
{
	m_pathsForward.clear();
	m_btnForward->setEnabled(false);
}

void MediaBrowser::pushBackward(const QString &path)
{
	m_pathsBackward << path;
	m_btnBack->setEnabled(true);
	//qDebug() << "pushBackward(): path:"<<path<<", list:"<<m_pathsBackward;
}

QString MediaBrowser::popBackward()
{
	QString path = m_pathsBackward.takeLast();
	m_btnBack->setEnabled(m_pathsBackward.size() > 0);
	//qDebug() << "popBackward(): path:"<<path<<", list:"<<m_pathsBackward;
	return path;
}

void MediaBrowser::unshiftForward(const QString &path)
{
	m_pathsForward.prepend(path);
	m_btnForward->setEnabled(true);
	//qDebug() << "unshiftForward(): path:"<<path<<", list:"<<m_pathsForward;
}

QString MediaBrowser::shiftForward()
{
	QString path = m_pathsForward.takeFirst();
	m_btnForward->setEnabled(m_pathsForward.size() > 0);
	//qDebug() << "shiftBackward(): path:"<<path<<", list:"<<m_pathsForward;
	return path;
}


void MediaBrowser::filterChanged(const QString &text)
{
	QStringList filter = makeModelFilterList(text);
	//qDebug() << "filterChanged(): text:"<<text<<", filter:"<<filter;
	//m_fsModel->setNameFilters(filter);
	m_fsModel->setFilters(filter);
	m_clearSearchBtn->setVisible(!text.isEmpty());
// 	QModelIndex idx = m_fsModel->indexForRow(0);
// 	if(idx.isValid())
// 		m_listView->setCurrentIndex(idx);
}

QStringList MediaBrowser::makeModelFilterList(const QString& text)
{
	QString filter = text;

	QStringList list;

	// if user doesnt include any wildcard, start it out for them correctly
	if(filter.indexOf("*")<0 && !filter.isEmpty())
		filter = QString("*%1").arg(filter);

	// if user doesnt specify an extension, include the one from m_fileTypeFilter
	if(filter.indexOf(".")>=0)
	{
		filter = QString("%1*").arg(filter);
		list << filter;
	}
	else
	{
		// process the list of extensions chosen in the combo box
		if(m_currentTypeFilterList.size()<=0)
			m_currentTypeFilterList << "*.*";

		foreach(QString typeFilter, m_currentTypeFilterList)
			list << QString("%1%2").arg(filter).arg(typeFilter);
	}

	//qDebug() << "MediaBrowser::makeModelFilterList: text:"<<text<<", list:"<<list;
	return list;
}

void MediaBrowser::setFilter(const QString & filter)
{
	filterChanged(filter);
	m_searchBox->setText(filter);
}
