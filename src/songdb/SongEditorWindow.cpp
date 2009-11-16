#include "SongEditorWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QCheckBox>

#include "SlideEditorWindow.h"
#include "SongSlideGroup.h"

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
	m_syncToDatabase(true)
{
	setAttribute(Qt::WA_DeleteOnClose,true);
	
	QWidget *centerWidget = new QWidget(this);
	
	QVBoxLayout * vbox = new QVBoxLayout(centerWidget);
	
	QHBoxLayout * hbox1 = new QHBoxLayout();
	QLabel *label = new QLabel("&Title: ");
	hbox1->addWidget(label);
	
	m_title = new QLineEdit();
	
	label->setBuddy(m_title);
	hbox1->addWidget(m_title);

	
	vbox->addLayout(hbox1);
	
	setWindowIcon(QIcon(":/data/icon-d.png"));
	
	QFont font;
	font.setFamily("Courier");
	font.setFixedPitch(true);
	font.setPointSize(10);
	
	m_editor = new MyQTextEdit;
	m_editor->setFont(font);
	//m_editor->setAcceptRichText(false);
	
	m_highlighter = new SongEditorHighlighter(m_editor->document());
	
	vbox->addWidget(m_editor);
	
	
	QHBoxLayout * hbox2 = new QHBoxLayout();
	
	m_tmplEditButton = new QPushButton("Edit &Background && Templates...");
	m_tmplEditButton->setIcon(QIcon(":data/stock-select-color.png"));
	connect(m_tmplEditButton, SIGNAL(clicked()), this, SLOT(editSongTemplate()));
	hbox2->addWidget(m_tmplEditButton);
	
	hbox2->addStretch();
	
	m_syncBox = new QCheckBox("S&ync Changes to Database");
	m_syncBox->setToolTip("If checked, saving changes will update the master song database as well as the copy of the song in this document");
	m_syncBox->setChecked(true);
	connect(m_syncBox, SIGNAL(toggled(bool)), this, SLOT(setSyncToDatabase(bool)));
	
	hbox2->addWidget(m_syncBox);
	
	QPushButton *btn;
	btn = new QPushButton("&Save Song");
	btn->setIcon(QIcon(":data/stock-save.png"));
	connect(btn, SIGNAL(clicked()), this, SLOT(accepted()));
	hbox2->addWidget(btn);
	
	btn = new QPushButton("&Cancel");
	connect(btn, SIGNAL(clicked()), this, SLOT(close()));
	hbox2->addWidget(btn);
	
	vbox->addLayout(hbox2);
	
	//setLayout(vbox);
	setCentralWidget(centerWidget);
	
	resize(500,600);
	
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
		
	if(!m_editWin)
	{
		m_editWin = new SlideEditorWindow();
		connect(m_editWin, SIGNAL(closed()), this, SLOT(editorWindowClosed()));
	}
	
	SlideGroup * tmpl = songGroup->slideTemplates();
	if(!tmpl)
	{
		tmpl = songGroup->createDefaultTemplates();
		songGroup->setSlideTemplates(tmpl);
	}
	
	// set slide group twice like MainWindow does
	m_editWin->setSlideGroup(tmpl);
	m_editWin->show();
	m_editWin->setSlideGroup(tmpl);
	
}

void SongEditorWindow::editorWindowClosed()
{
	SongSlideGroup * songGroup = dynamic_cast<SongSlideGroup*>(m_slideGroup);
	if(!songGroup)
		return;

	songGroup->setSlideTemplates(songGroup->slideTemplates());
}

SongEditorWindow::~SongEditorWindow() 
{
	if(m_editWin)
		delete m_editWin;
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
			
}

void SongEditorWindow::setSlideGroup(SlideGroup *g,bool syncToDatabase) 
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

void SongEditorWindow::closeEvent(QCloseEvent *evt)
{
// 	if(m_syncToDatabase)
// 	{
// 		delete songGroup->song();
// 		delete songGroup;
// 	}
	evt->accept();
}




SongEditorHighlighter::SongEditorHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
	HighlightingRule rule;
				
	tagFormat.setFontWeight(QFont::Bold);
	tagFormat.setForeground(Qt::white);
	tagFormat.setBackground(Qt::red);
	rule.pattern = QRegExp("\\s*(Verse|Chorus|Tag|Bridge|End(ing)?|Intro(duction)?)(\\s+\\d+)?(\\s*\\(.*\\))?\\s*");
	rule.format = tagFormat;
	highlightingRules.append(rule);
	
	rearFormat.setFontWeight(QFont::Bold);
	rearFormat.setForeground(Qt::white);
	rearFormat.setBackground(Qt::blue);
	rule.pattern = QRegExp("\\s*((?:B:|R:|C:|T:|G:|\\[|\\|).*)");
	rule.format = rearFormat;
	highlightingRules.append(rule);
}
//! [6]

//! [7]
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

