#include "SongEditorWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>

#include "SlideEditorWindow.h"
#include "SongSlideGroup.h"

SongEditorWindow::SongEditorWindow(SlideGroup *g, QWidget *parent) : 
	AbstractSlideGroupEditor(g,parent),
	m_slideGroup(0),
	m_editWin(0)
{
	setAttribute(Qt::WA_DeleteOnClose,true);
	
	QVBoxLayout * vbox = new QVBoxLayout(this);
	
	QHBoxLayout * hbox1 = new QHBoxLayout();
	QLabel *label = new QLabel("Title: ");
	hbox1->addWidget(label);
	
	m_title = new QLineEdit();
	hbox1->addWidget(m_title);

	
	vbox->addLayout(hbox1);
	
	
	QFont font;
	font.setFamily("Courier");
	font.setFixedPitch(true);
	font.setPointSize(10);
	
	m_editor = new QTextEdit;
	m_editor->setFont(font);
	
	m_highlighter = new SongEditorHighlighter(m_editor->document());
	
	vbox->addWidget(m_editor);
	
	
	QHBoxLayout * hbox2 = new QHBoxLayout();
	
	QPushButton * tmplEditBtn = new QPushButton("Edit Background && Templates...");
	connect(tmplEditBtn, SIGNAL(clicked()), this, SLOT(editSongTemplate()));
	hbox2->addWidget(tmplEditBtn);
	
	hbox2->addStretch();
	
	QPushButton *btn;
	btn = new QPushButton("Save Song");
	connect(btn, SIGNAL(clicked()), this, SLOT(accepted()));
	hbox2->addWidget(btn);
	
	btn = new QPushButton("Cancel");
	connect(btn, SIGNAL(clicked()), this, SLOT(close()));
	hbox2->addWidget(btn);
	
	vbox->addLayout(hbox2);
	
	//setLayout(vbox);
	
	resize(500,600);
	
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
		}
	}
	close();
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

