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

#include "AppSettings.h"

MediaBrowser::MediaBrowser(QWidget *parent)
	: QWidget(parent)
	, m_fileTypeFilter("")
	, m_currentDirectory("")
{
	setObjectName("MediaBrowser");
	setupUI();
	//setDirectory("/home/josiah",false);
	setDirectory(AppSettings::previousPath("media"));
}
	
MediaBrowser::~MediaBrowser() {}
#define SET_MARGIN(layout,margin) \
	layout->setContentsMargins(margin,margin,margin,margin);

void MediaBrowser::setupUI()
{
	QVBoxLayout *vbox = new QVBoxLayout(this);
	//SET_MARGIN(vbox,0);
	
	// Setup filter box at the top of the widget
	m_searchBase = new QWidget(this);
	
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
	
	QLabel *label = new QLabel("Fi&lter:");
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
	m_listView = new QListView(this);
	m_listView->setAlternatingRowColors(true);
	
	m_fsModel = new QFileSystemModel(this);
	m_fsModel->setNameFilterDisables(false);
	
	m_listView->setModel(m_fsModel);

	vbox->addWidget(m_searchBase);
	vbox->addWidget(m_listView);
	
	connect(m_listView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(indexDoubleClicked(const QModelIndex &)));
}


void MediaBrowser::filterReturnPressed() 
{
// 	QModelIndex idx = m_fsModel->indexForRow(0);
// 	if(idx.isValid())
// 		indexDoubleClicked(idx);
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
		emit fileSelected(info);
	}
}

void MediaBrowser::setDirectory(const QString &path, bool addToHistory)
{
	qDebug() << "setDirectory(): setting folder path:"<<path;
	QModelIndex root = m_fsModel->setRootPath(path);
	m_listView->setRootIndex(root);
	
	if(addToHistory && !m_currentDirectory.isEmpty())
	{
		pushBackward(m_currentDirectory);
		clearForward();
	}
	
	AppSettings::setPreviousPath("media",path);
	
	m_currentDirectory = path;
	checkCanGoUp();
}

bool MediaBrowser::checkCanGoUp()
{
	QFileInfo info(m_currentDirectory);
	QString can = info.canonicalFilePath();
	QStringList split = can.split("/");
	
	if(split.size() <= 1)
	{
		qDebug() << "checkCanGoUp(): False, can't go up from:"<<can;
		m_btnUp->setEnabled(false);
		return false;
	}
	else
	{
		qDebug() << "checkCanGoUp(): True, can go up from:"<<can;
		m_btnUp->setEnabled(true);
		return true;
	}
}

void MediaBrowser::goUp()
{
	if(!checkCanGoUp())
		return;
		
	QFileInfo info(m_currentDirectory);
	QString can = info.canonicalFilePath();
	QStringList split = can.split("/");
	split.takeLast();
	QString newPath = split.join("/");
	
	setDirectory(newPath);
	
	qDebug() << "goUp(): newPath:"<<newPath;
}

void MediaBrowser::goBack()
{
	QString path = popBackward();
	if(!m_currentDirectory.isEmpty())
		unshiftForward(m_currentDirectory);
	setDirectory(path,false);
	
	qDebug() << "goBack(): path:"<<path;
}

void MediaBrowser::goForward()
{
	QString path = shiftForward();
	if(!m_currentDirectory.isEmpty())
		pushBackward(m_currentDirectory);
	setDirectory(path,false);
	
	qDebug() << "goForward(): path:"<<path;
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
	qDebug() << "pushBackward(): path:"<<path<<", list:"<<m_pathsBackward;
}

QString MediaBrowser::popBackward()
{
	QString path = m_pathsBackward.takeLast();
	m_btnBack->setEnabled(m_pathsBackward.size() > 0);
	qDebug() << "popBackward(): path:"<<path<<", list:"<<m_pathsBackward;
	return path;
}

void MediaBrowser::unshiftForward(const QString &path)
{
	m_pathsForward.prepend(path);
	m_btnForward->setEnabled(true);
	qDebug() << "unshiftForward(): path:"<<path<<", list:"<<m_pathsForward;
}

QString MediaBrowser::shiftForward()
{
	QString path = m_pathsForward.takeFirst();
	m_btnForward->setEnabled(m_pathsForward.size() > 0);
	qDebug() << "shiftBackward(): path:"<<path<<", list:"<<m_pathsForward;
	return path;
}


void MediaBrowser::filterChanged(const QString &text)
{
	QStringList filter = makeModelFilterList(text);
	qDebug() << "filterChanged(): text:"<<text<<", filter:"<<filter;
	m_fsModel->setNameFilters(filter);
	m_clearSearchBtn->setVisible(!text.isEmpty());
// 	QModelIndex idx = m_fsModel->indexForRow(0);
// 	if(idx.isValid())
// 		m_listView->setCurrentIndex(idx);
}

QStringList MediaBrowser::makeModelFilterList(const QString& text)
{
	QString filter = text;
	
	if(m_fileTypeFilter == "")
		m_fileTypeFilter = "*.*";
	
	// if user doesnt include any wildcard, start it out for them correctly
	if(filter.indexOf("*")<0)
		filter = QString("*%1").arg(filter);
	
	// if user doesnt specify an extension, include the one from m_fileTypeFilter
	if(filter.indexOf(".")<0)
		filter = QString("%1%2").arg(filter).arg(m_fileTypeFilter);
	else
		filter = QString("%1*").arg(filter);
	
	QStringList list;
	list << filter;
	
	return list;
}

void MediaBrowser::setFilter(const QString & filter)
{
	filterChanged(filter);
	m_searchBox->setText(filter);
}
