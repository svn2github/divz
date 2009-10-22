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

#include "AppSettings.h"

MediaBrowser::MediaBrowser(QWidget *parent)
	: QWidget(parent)
{
	setObjectName("MediaBrowser");
	setupUI();
	setDirectory("/home/josiah");
	//setDirectory(AppSettings::previousPath("videos"));
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
	
	QLabel *label = new QLabel("Fi&lter:");
	m_searchBox = new QLineEdit(m_searchBase);
	label->setBuddy(m_searchBox);
	
	m_clearSearchBtn = new QPushButton("Clear");
	m_clearSearchBtn->setVisible(false);
	
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

void MediaBrowser::setDirectory(const QString &path)
{
	qDebug() << "setDirectory(): setting folder path:"<<path;
	QModelIndex root = m_fsModel->setRootPath(path);
	m_listView->setRootIndex(root);
}

void MediaBrowser::filterChanged(const QString &text)
{
	QStringList list;
	list << text;
	m_fsModel->setNameFilters(list);
	m_clearSearchBtn->setVisible(!text.isEmpty());
// 	QModelIndex idx = m_fsModel->indexForRow(0);
// 	if(idx.isValid())
// 		m_listView->setCurrentIndex(idx);
}

void MediaBrowser::setFilter(const QString & filter)
{

	filterChanged(filter);
	m_searchBox->setText(filter);
}
