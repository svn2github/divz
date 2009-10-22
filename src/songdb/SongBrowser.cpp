#include "SongBrowser.h"

#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLabel>

#include "SongRecordListModel.h"
#include "SongSlideGroup.h"
#include "SongEditorWindow.h"
#include "SongRecord.h"


class MyQListView : public QListView
{
public:
	MyQListView(SongBrowser * ctrl) : QListView(ctrl), ctrl(ctrl) {}
protected:
	void keyPressEvent(QKeyEvent *event)
	{
		QModelIndex oldIdx = currentIndex();
		QListView::keyPressEvent(event);
		QModelIndex newIdx = currentIndex();
		if(oldIdx.row() != newIdx.row())
		{
			ctrl->songSingleClicked(newIdx);
		}
	}
	
	SongBrowser * ctrl;
};


SongBrowser::SongBrowser(QWidget *parent)
	: QWidget(parent)
	, m_editingEnabled(true)
	, m_filteringEnabled(true)
	, m_previewEnabled(true)
{
	setObjectName("SongBrowser");
	setupUI();
}
	
SongBrowser::~SongBrowser() {}
#define SET_MARGIN(layout,margin) \
	layout->setContentsMargins(margin,margin,margin,margin);

void SongBrowser::setupUI()
{
	QVBoxLayout *vbox = new QVBoxLayout();
	SET_MARGIN(vbox,0);
	
	// Setup filter box at the top of the widget
	m_searchBase = new QWidget(this);
	
	QHBoxLayout *hbox = new QHBoxLayout(m_searchBase);
	SET_MARGIN(hbox,0);
	
	QLabel *label = new QLabel("Searc&h:");
	m_songSearch = new QLineEdit(m_searchBase);
	label->setBuddy(m_songSearch);
	
	m_clearSearchBtn = new QPushButton("Clear");
	m_clearSearchBtn->setVisible(false);
	
	hbox->addWidget(label);
	hbox->addWidget(m_songSearch);
	hbox->addWidget(m_clearSearchBtn);
	
	connect(m_songSearch, SIGNAL(textChanged(const QString &)), this, SLOT(songFilterChanged(const QString &)));
	connect(m_songSearch, SIGNAL(returnPressed()), this, SLOT(songSearchReturnPressed()));
	connect(m_clearSearchBtn, SIGNAL(clicked()), this, SLOT(songFilterReset()));
	
	// Now for the song list itself
	m_songList = new MyQListView(this);
	m_songList->setAlternatingRowColors(true);
	
	m_songListModel = SongRecordListModel::instance();
	m_songList->setModel(m_songListModel);
	
	// setup buttons at bottom
	m_editingButtons = new QWidget(this);
	QHBoxLayout *hbox2 = new QHBoxLayout(m_editingButtons);
	SET_MARGIN(hbox2,0);
	QPushButton *btn;
	
	btn = new QPushButton(QIcon(":/data/stock-new.png"),"&New Song");
	connect(btn, SIGNAL(clicked()), this, SLOT(addNewSong()));
	hbox2->addWidget(btn);
	
	btn = new QPushButton(QIcon(":/data/stock-edit.png"),"Edi&t");
	connect(btn, SIGNAL(clicked()), this, SLOT(editSongInDB()));
	hbox2->addWidget(btn);
	
	btn = new QPushButton(QIcon(":/data/stock-delete.png"),"&Delete");
	connect(btn, SIGNAL(clicked()), this, SLOT(deleteCurrentSong()));
	hbox2->addWidget(btn);
	
	// initalize splitter
	QVBoxLayout * baseLayout = new QVBoxLayout(this);
	
	m_splitter = new QSplitter(Qt::Vertical);
	baseLayout->addWidget(m_splitter);
	
	// add song list to splitter
	QWidget * topBase = new QWidget();
	vbox->addWidget(m_searchBase);
	vbox->addWidget(m_songList);
	vbox->addWidget(m_editingButtons);
	topBase->setLayout(vbox);
	
	m_splitter->addWidget(topBase);

	// add song text preview
	m_songTextPreview = new QTextEdit;
	m_songTextPreview->setReadOnly(true);

	QFont font;
	font.setFamily("Courier");
	font.setFixedPitch(true);
	font.setPointSize(8);
	m_songTextPreview->setFont(font);

	new SongEditorHighlighter(m_songTextPreview->document());
	m_splitter->addWidget(m_songTextPreview);
	
	
	connect(m_songList, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(songDoubleClicked(const QModelIndex &)));
	connect(m_songList,       SIGNAL(clicked(const QModelIndex &)), this, SLOT(songSingleClicked(const QModelIndex &)));
}

QByteArray SongBrowser::saveState() 
{
	return m_splitter->saveState();
}

bool SongBrowser::restoreState(const QByteArray &state) 
{
	return m_splitter->restoreState(state);
}


void SongBrowser::setEditingEnabled(bool flag)
{
	m_editingEnabled = flag;
	m_editingButtons->setVisible(flag);
}

void SongBrowser::setFilteringEnabled(bool flag)
{
	m_filteringEnabled = flag;
	m_searchBase->setVisible(flag);
}

void SongBrowser::setPreviewEnabled(bool flag)
{
	m_previewEnabled = flag;
	m_songTextPreview->setVisible(flag);
}

void SongBrowser::songSearchReturnPressed() 
{
	QModelIndex idx = m_songListModel->indexForRow(0);
	if(idx.isValid())
		songDoubleClicked(idx);
}

void SongBrowser::addNewSong()
{
	if(!m_editingEnabled)
		return;
		
	SongRecord * song = new SongRecord();
	SongSlideGroup * group = new SongSlideGroup();
	group->setSong(song);
	
	SongEditorWindow * editor = new SongEditorWindow();
	editor->setSlideGroup(group,true);
	
	connect(editor, SIGNAL(songCreated(SongRecord*)), this, SLOT(songCreated(SongRecord*)));
	
	editor->show();
}

void SongBrowser::songCreated(SongRecord *song)
{
	QModelIndex idx = m_songListModel->indexForSong(song);
	if(idx.isValid())
		m_songList->setCurrentIndex(idx);

}


void SongBrowser::deleteCurrentSong()
{
	if(!m_editingEnabled)
		return;
		
	QModelIndex idx = m_songList->currentIndex();
	SongRecord *song = m_songListModel->songFromIndex(idx);
	if(QMessageBox::warning(this,"Really Delete Song?","Are you sure you want to delete this song? This cannot be undone.",QMessageBox::Ok | QMessageBox::Cancel) 
		== QMessageBox::Ok)
	{
		SongRecord::deleteSong(song);
	}
}

void SongBrowser::editSongInDB() 
{
	if(!m_editingEnabled)
		return;
		
	QModelIndex idx = m_songList->currentIndex();
	if(idx.isValid())
	{
		SongRecord *song = m_songListModel->songFromIndex(idx);
		
		SongSlideGroup * group = new SongSlideGroup();
		group->setSong(song);
		
		SongEditorWindow * editor = new SongEditorWindow();
		editor->setSlideGroup(group,true);
		editor->show();
		
		connect(editor, SIGNAL(songSaved()), this, SLOT(editSongAccepted()));
	}
}

void SongBrowser::editSongAccepted(/*SongRecord**/)
{

}

void SongBrowser::songDoubleClicked(const QModelIndex &idx)
{
	SongRecord * song = m_songListModel->songFromIndex(idx);
	emit songSelected(song);
}

void SongBrowser::songSingleClicked(const QModelIndex &idx)
{
	SongRecord * song = m_songListModel->songFromIndex(idx);
	m_songTextPreview->setPlainText(
		    QString("%1\n\n%2")
			.arg(song->title())
			.arg(song->text())
		);
}


void SongBrowser::songFilterChanged(const QString &text)
{
	m_songListModel->filter(text);
	m_clearSearchBtn->setVisible(!text.isEmpty());
	QModelIndex idx = m_songListModel->indexForRow(0);
	if(idx.isValid())
		m_songList->setCurrentIndex(idx);
}

void SongBrowser::songFilterReset()
{
	setFilter("");
}

void SongBrowser::setFilter(const QString & filter)
{
	if(!m_filteringEnabled)
		return;
		
	songFilterChanged(filter);
	m_songSearch->setText(filter);
}


