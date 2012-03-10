#include "SongEditorWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QCheckBox>
#include <QPalette>

#include "SlideEditorWindow.h"
#include "SongSlideGroup.h"

#include "ArrangementListModel.h"

// Neede to access the openSlideEditor() function
#include "MainWindow.h"

class MyQTextEdit : public QTextEdit
{
public:
	MyQTextEdit() : QTextEdit() {}
protected:
	void insertFromMimeData ( const QMimeData * source )
	{
		if(source)
		{
			//QMimeData data = *source;
			qDebug() << "MyQTextEdit::insertFromMimeData(): Formats dropped: "<<source->formats();
			
			QTextEdit::insertFromMimeData(source);
			
		}
		else
		{
			qDebug() << "MyQTextEdit::insertFromMimeData(): No *source ptr provided";
		}
	}
};

SongEditorWindow::SongEditorWindow(SlideGroup *g, QWidget *parent) : 
	AbstractSlideGroupEditor(g,parent),
	m_slideGroup(0),
	m_editWin(0),
	m_syncToDatabase(true),
	m_arrModel(0)
{
	setAttribute(Qt::WA_DeleteOnClose,true);
	
	QWidget *centerWidget = new QWidget(this);
	
	QVBoxLayout * vbox = new QVBoxLayout(centerWidget);
	
	// Title editor
	QHBoxLayout * hbox1 = new QHBoxLayout();
	QLabel *label = new QLabel("&Title: ");
	hbox1->addWidget(label);
	
	m_title = new QLineEdit();
	
	label->setBuddy(m_title);
	hbox1->addWidget(m_title);
	
	vbox->addLayout(hbox1);
	
	// Default arrangement
	QHBoxLayout *hbox2 = new QHBoxLayout();
	label = new QLabel("Default Arrangement: ");
	hbox2->addWidget(label);
	
	m_defaultArrangement = new QLineEdit();
	m_defaultArrangement->setReadOnly(true);
	
	QPalette p = m_defaultArrangement->palette();
	p.setColor(QPalette::Base, Qt::lightGray);
	m_defaultArrangement->setPalette(p);

	hbox2->addWidget(m_defaultArrangement);
	
	vbox->addLayout(hbox2);

	// My arrangement
	QHBoxLayout *hbox3 = new QHBoxLayout();
	label = new QLabel("Current &Arrangement:");
	hbox3->addWidget(label);
	
	m_arrangement = new QLineEdit();
	m_arrangement->setReadOnly(false);
	connect(m_arrangement, SIGNAL(textChanged(const QString&)), this, SLOT(arrTextChanged(const QString&)));
	label->setBuddy(m_arrangement);
	hbox3->addWidget(m_arrangement);
	
	QPushButton *btn = new QPushButton("Default");
	btn->setIcon(QIcon(":/data/stock-undo.png"));
	btn->setToolTip("Reset arrangement to default arrangement (above)");
	connect(btn, SIGNAL(clicked()), this, SLOT(resetArr()));
	hbox3->addWidget(btn);
	
	
	btn = new QPushButton("List");
	btn->setCheckable(true);
	btn->setIcon(QIcon(":/data/stock-find-and-replace.png"));
	btn->setToolTip("Show/hide the arrangement drag-and-drop list");
	connect(btn, SIGNAL(toggled(bool)), this, SLOT(arrListViewToggled(bool)));
	hbox3->addWidget(btn);
	
	QPushButton *listBtn = btn;
	
	vbox->addLayout(hbox3);


	setWindowIcon(QIcon(":/data/icon-d.png"));
	
	QFont font;
	font.setFamily("Courier");
	font.setFixedPitch(true);
	font.setPointSize(10);
	
	//QHBoxLayout *editorHBox = new QHBoxLayout();
	//editorHBox->setContentsMargins(0,0,0,0);
	QSplitter *editorHBox = new QSplitter();
	
	m_editor = new MyQTextEdit;
	m_editor->setFont(font);
	editorHBox->addWidget(m_editor);
	connect(m_editor, SIGNAL(textChanged()), this, SLOT(editTextChanged()));
	//m_editor->setAcceptRichText(false);
	
	m_highlighter = new SongEditorHighlighter(m_editor->document());
	
	// Arrangement preview box
	m_arrangementPreview = new MyQTextEdit;
	m_arrangementPreview->setFont(font);
	m_arrangementPreview->setReadOnly(true);
	
	QPalette p2 = m_arrangementPreview->palette();
	p2.setColor(QPalette::Base, Qt::lightGray);
	m_arrangementPreview->setPalette(p2);

	editorHBox->addWidget(m_arrangementPreview);
	//m_editor->setAcceptRichText(false);
	
	(void*)new SongEditorHighlighter(m_arrangementPreview->document());
	
	// List view
	m_arrModel = new ArrangementListModel();
	connect(m_arrModel, SIGNAL(blockListChanged(QStringList)), this, SLOT(arrModelChange(QStringList)));
	
	QListView *arrListView = new QListView();
	
	arrListView->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));
	arrListView->setViewMode(QListView::ListMode);
	arrListView->setWrapping(false);
	arrListView->setFlow(QListView::TopToBottom);
	
	arrListView->setMovement(QListView::Free);
	arrListView->setWordWrap(true);
	arrListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	arrListView->setDragEnabled(true);
	arrListView->setAcceptDrops(true);
	arrListView->setDropIndicatorShown(true);
	
	arrListView->setModel(m_arrModel);
	m_arrListView = arrListView;
	
	editorHBox->addWidget(arrListView);
	
	editorHBox->setStretchFactor(0,10);
	editorHBox->setStretchFactor(1,10);
	editorHBox->setStretchFactor(2,1);
	
	//vbox->addWidget(m_editor);
	//vbox->addLayout(editorHBox);
	vbox->addWidget(editorHBox);
	
	QHBoxLayout * hboxBottom = new QHBoxLayout();
	
	m_tmplEditButton = new QPushButton("Edit &Template...");
	m_tmplEditButton->setIcon(QIcon(":data/stock-select-color.png"));
	connect(m_tmplEditButton, SIGNAL(clicked()), this, SLOT(editSongTemplate()));
	hboxBottom->addWidget(m_tmplEditButton);
	
	hboxBottom->addStretch();
	
	m_syncBox = new QCheckBox("S&ync Changes to DB");
	m_syncBox->setToolTip("If checked, saving changes will update the master song database as well as the copy of the song in this document");
	m_syncBox->setChecked(true);
	connect(m_syncBox, SIGNAL(toggled(bool)), this, SLOT(setSyncToDatabase(bool)));
	
	hboxBottom->addWidget(m_syncBox);
	
	QCheckBox *showArrPreviewCb = new QCheckBox("Arrangement Preview");
	showArrPreviewCb->setToolTip("Show/hide arrangement preview window");
	showArrPreviewCb->setChecked(true);
	connect(showArrPreviewCb, SIGNAL(toggled(bool)), this, SLOT(showArrPreview(bool)));
	
	hboxBottom->addWidget(showArrPreviewCb);
	
	
	btn = new QPushButton("&Save Song");
	btn->setIcon(QIcon(":data/stock-save.png"));
	connect(btn, SIGNAL(clicked()), this, SLOT(accepted()));
	hboxBottom->addWidget(btn);
	
	btn = new QPushButton("&Cancel");
	connect(btn, SIGNAL(clicked()), this, SLOT(close()));
	hboxBottom->addWidget(btn);
	
	vbox->addLayout(hboxBottom);
	
	//setLayout(vbox);
	setCentralWidget(centerWidget);
	
	resize(500,600);
	
	QSettings set;
	bool flag = set.value("songdb/arrlistview",true).toBool();
	arrListViewToggled(flag);
	listBtn->setChecked(flag);
	
	flag = set.value("songdb/arrpreview",true).toBool();
	showArrPreviewCb->setChecked(flag);
	showArrPreview(flag);
	
	
	
	
		
// 	m_editWin = new SlideEditorWindow();
// 	connect(m_editWin, SIGNAL(closed()), this, SLOT(editorWindowClosed()));
//	}
	
}

void SongEditorWindow::arrListViewToggled(bool flag)
{
	m_arrListView->setVisible(flag);
	
	QSettings().setValue("songdb/arrlistview",flag);
}

void SongEditorWindow::showArrPreview(bool flag)
{
	m_arrangementPreview->setVisible(flag);
	
	QSettings().setValue("songdb/arrpreview",flag);
}

void SongEditorWindow::showSyncOption(bool flag)
{
	m_syncBox->setVisible(flag);
}

void SongEditorWindow::editSongTemplate()
{
	if(!m_slideGroup)
		return;
	
	SongSlideGroup * songGroup = dynamic_cast<SongSlideGroup*>(m_slideGroup);
	if(!songGroup)
		return;
	
	
	SlideGroup * tmpl = songGroup->slideTemplates();
	if(!tmpl)
	{
		tmpl = songGroup->createDefaultTemplates();
		songGroup->setSlideTemplates(tmpl);
	}
	
	// set slide group twice like MainWindow does
// 	m_editWin->setSlideGroup(tmpl);
// 	m_editWin->show();
// 	m_editWin->setSlideGroup(tmpl);

//	m_editWin = MainWindow::mw()->openSlideEditor(tmpl);
	if((m_editWin = MainWindow::mw()->openSlideEditor(tmpl)) != 0)
//	if(m_editWin)
		connect(m_editWin, SIGNAL(closed()), this, SLOT(editorWindowClosed()));
	
}

void SongEditorWindow::editorWindowClosed()
{
	SongSlideGroup * songGroup = dynamic_cast<SongSlideGroup*>(m_slideGroup);
	if(!songGroup)
		return;

	songGroup->setSlideTemplates(songGroup->slideTemplates());
	
// 	m_editWin->deleteLater();
// 	m_editWin = 0;
	if(m_editWin)
		disconnect(m_editWin, 0, this, 0);
	
	//raise();
	setFocus();
	qDebug() << "SongEditorWindow::editorWindowClosed(): setting focus to self:"<<this<<", m_editWin="<<m_editWin;
}

SongEditorWindow::~SongEditorWindow() 
{
// 	if(m_editWin)
// 		m_editWin->deleteLater();
}

void SongEditorWindow::setSlideGroup(SlideGroup *g,Slide */*curSlide*/) 
{
	m_slideGroup = g;
	SongSlideGroup * songGroup = dynamic_cast<SongSlideGroup*>(m_slideGroup);
	if(!songGroup)
		return;
	QString text = songGroup->text().replace("\r\n","\n");
	m_editor->setPlainText(text);
	m_title->setText(songGroup->groupTitle());
	setWindowTitle(songGroup->groupTitle() + " - Song Editor");
	m_editor->setFocus(Qt::OtherFocusReason);
	
	m_defaultArrangement->setText(SongSlideGroup::findDefaultArragement(songGroup->text()).join(", "));
	m_arrangement->setText(songGroup->arrangement().join(", "));
	m_arrangementPreview->setText(SongSlideGroup::rearrange(songGroup->text(), songGroup->arrangement()));
	
	if(m_arrModel)
		m_arrModel->setBlockList(songGroup->arrangement());
}

void SongEditorWindow::arrTextChanged(const QString& newArr)
{
	QStringList newArrList = newArr.split(QRegExp("\\s*,\\s*"));
	
	//qDebug() << "SongEditorWindow::arrTextChanged: "<<newArrList<<" from "<<newArr;
	
	arrModelChange(newArrList, false);
	
}

void SongEditorWindow::resetArr()
{
	SongSlideGroup * songGroup = dynamic_cast<SongSlideGroup*>(m_slideGroup);
	if(!songGroup)
		return;
	
	arrModelChange(SongSlideGroup::findDefaultArragement(songGroup->text()));
}

void SongEditorWindow::arrModelChange(QStringList newArrList, bool changeLineEdit)
{
	//qDebug() << "SongEditorWindow::arrModelChange: "<<newArrList;
	if(newArrList != m_currArrPreviewList)
	{
		m_currArrPreviewList = newArrList;
		
		// Update arr text
		if(changeLineEdit)
			m_arrangement->setText(newArrList.join(", "));
		
		if(m_arrModel)
			m_arrModel->setBlockList(newArrList);
		m_arrangementPreview->setText(SongSlideGroup::rearrange(m_editor->toPlainText(), newArrList));
	}
}

void SongEditorWindow::editTextChanged()
{
	QString plainText = m_editor->toPlainText();
	m_defaultArrangement->setText(SongSlideGroup::findDefaultArragement(plainText).join(", "));
	m_arrangementPreview->setText(SongSlideGroup::rearrange(plainText, m_currArrPreviewList));
}

void SongEditorWindow::setSlideGroup(SlideGroup *g, bool syncToDatabase) 
{
	setSlideGroup(g);
	if(syncToDatabase)
	{
		if(!dynamic_cast<SongSlideGroup*>(g)->song()->songId())
		{
			setWindowTitle("New Song - Song Editor");
			m_title->setText("");
			m_title->setFocus(Qt::OtherFocusReason);
		}
	}
	
	m_syncToDatabase = syncToDatabase;
	
	m_tmplEditButton->setVisible(!syncToDatabase);
			
}

void SongEditorWindow::setSyncToDatabase(bool sync)
{
	m_syncToDatabase = sync;
}

void SongEditorWindow::accepted()
{
	if(m_slideGroup)
	{
		SongSlideGroup * songGroup = dynamic_cast<SongSlideGroup*>(m_slideGroup);
		if(songGroup)
		{
			songGroup->setGroupTitle(m_title->text());
			//songGroup->song()->setText(m_editor->toPlainText());
			songGroup->setText(m_editor->toPlainText());
			
			QStringList newArrList = m_arrangement->text().split(QRegExp("\\s*,\\s*"));
			songGroup->setArrangement(newArrList);
			
			if(m_syncToDatabase)
			{
				
				SongRecord * song = songGroup->song();
				song->setTitle(m_title->text());
				song->setText(m_editor->toPlainText());
				
				if(!songGroup->song()->songId())
				{
					SongRecord::addSong(song);
					emit songCreated(song);
				}
			}
			
			emit songSaved();
		}
	}
	close();
}

// void SongEditorWindow::closeEvent(QCloseEvent *evt)
// {
// // 	if(m_syncToDatabase)
// // 	{
// // 		delete songGroup->song();
// // 		delete songGroup;
// // 	}
// 	evt->accept();
// }




SongEditorHighlighter::SongEditorHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
	HighlightingRule rule;
				
	tagFormat.setFontWeight(QFont::Bold);
	tagFormat.setForeground(Qt::white);
	tagFormat.setBackground(Qt::red);
	rule.pattern = QRegExp(tr("^\\s*(%1)(\\s+\\d+)?(\\s*\\(.*\\))?\\s*.*$").arg(SongSlideGroup::songTagRegexpList()));
	rule.format = tagFormat;
	highlightingRules.append(rule);
	
	rearFormat.setFontWeight(QFont::Bold);
	rearFormat.setForeground(Qt::white);
	rearFormat.setBackground(Qt::blue);
	rule.pattern = QRegExp("^\\s*((?:B:|R:|C:|T:|G:|\\[|\\|).*).*$");
	rule.format = rearFormat;
	highlightingRules.append(rule);
}

void SongEditorHighlighter::highlightBlock(const QString &text)
{
	foreach (const HighlightingRule &rule, highlightingRules) 
	{
		QRegExp expression(rule.pattern);
		int index = expression.indexIn(text);
		while (index >= 0) 
		{
			int length = expression.matchedLength();
			setFormat(index, length, rule.format);
			index = expression.indexIn(text, index + length);
		}
	}
}

