#include "SlideTemplateManager.h"

#include "AppSettings.h"
#include "Document.h"
#include "DocumentListModel.h"
#include "MainWindow.h"
#include "Slide.h"
#include "SlideGroup.h"
#include "SlideEditorWindow.h"
#include "TextBoxItem.h"

#include <QFile>
#include <QDir>
#include <QTextDocument>
#include <QTextEdit>
	
	
namespace SlideTemplateUtilities
{
	void textToDocument(QTextDocument& doc, const QString& text)
	{
		if (Qt::mightBeRichText(text))
			doc.setHtml(text);
		else
			doc.setPlainText(text);
	}
	
	TextBoxItem *findTextItem(Slide *slide, const QString& textKey)
	{
		QList<AbstractItem *> items = slide->itemList();
		
		//TextBoxItem * text = 0;
	
		QTextDocument doc;
				
		foreach(AbstractItem * item, items)
		{
			//AbstractVisualItem * newVisual = dynamic_cast<AbstractVisualItem*>(item);
			//if(DEBUG_TEXTOSLIDES)
			//	qDebug()<<"addSlide(): item list: "<<newVisual->itemName();
			
			if(item->itemClass() == TextBoxItem::ItemClass)
			{
				TextBoxItem *text = dynamic_cast<TextBoxItem*>(item);
				if(!text)
					continue;
					
				QTextDocument doc;
				textToDocument(doc, text->text());
				
				if(doc.toPlainText().indexOf("#verses") >= 0)
				{
					//if(DEBUG_TEXTOSLIDES)
					qDebug()<<"findTextItem(): Found textbox from template for key "<<textKey<<", name:"<<text->itemName();
					return dynamic_cast<TextBoxItem*>(item);
				}
			}
		}
		
		return 0;
	}
	
	
	QString mergeTextItem(const QString &destTemplate, const QString &source)
	{
		QTextDocument doc;
		textToDocument(doc, destTemplate);
		QTextCursor cursor(&doc);
		cursor.select(QTextCursor::Document);
		
		QTextCharFormat charFormat = cursor.charFormat();
		QTextBlockFormat blockFormat = cursor.blockFormat();
		QTextCharFormat blockCharFormat = cursor.blockCharFormat();
		
		QTextDocument doc2;
		textToDocument(doc2, source);
		QTextCursor cursor2(&doc2);
		cursor2.select(QTextCursor::Document);
		
		cursor2.mergeBlockCharFormat(blockCharFormat);
		cursor2.mergeBlockFormat(blockFormat);
		cursor2.mergeCharFormat(charFormat);
		
		return doc2.toHtml();
	}
	
	void mergeTextItem(TextBoxItem *dest, const QString &source)
	{
		dest->setText(mergeTextItem(dest->text(), source));
	}
	
	void mergeTextItem(TextBoxItem *dest, TextBoxItem *source)
	{
		mergeTextItem(dest, source->text());
	}
	
	void addSlideWithText(SlideGroup *group, Slide *slide, TextBoxItem *text)
	{
		int slideNbr = group->numSlides();
		
		slide->setSlideNumber(slideNbr);
		group->addSlide(slide);
		
		if(text)
		{
			// Delay warming the visual cache to increase UI responsiveness when quickly adding slides
			int delay = slideNbr * 5000;
			//qDebug()<<"SlideTemplateUtilities::addSlideWithText(): slideNbr:"<<slideNbr<<": delaying "<<delay<<"ms before warmVisualCache()";
			QTimer::singleShot(delay, text, SLOT(warmVisualCache()));
		}
	}
	
	
	void intelligentCenterTextbox(TextBoxItem *textbox)
	{
		QRect slideRect = AppSettings::adjustToTitlesafe(MainWindow::mw()->standardSceneRect());
		QRect textRect = textbox->contentsRect().toRect();
		
		// Attempt to enter text on screen
		int heightDifference = abs(slideRect.height() - textRect.height());
		
		// Arbitrary magic number to force centering for small amounts of differences.
		// We test the difference here because don't want to force-center the textbox if it came from the template
		// and was intentionally located off-center. But if the user tried to get it to fill the screen and missed
		// by a few pixels (<20), then go ahead and center it for the user.
		//qDebug()<<"BibleBrowser::addSlide(): slideNbr: "<<slideNbr<<": heightDifference: "<<heightDifference;
		
		QSize natSize = textbox->findNaturalSize(textRect.width()); // 20 = buffer for shadow on right
			
		if(heightDifference < 20 &&
		   natSize.height() > -1)
		{
			qreal y = qMax(0, slideRect.height()/2 - natSize.height()/2);
			
			QRectF centeredRect(slideRect.x(),slideRect.y() + y,textRect.width(),natSize.height());
			
			textbox->setPos(QPointF(0,0));
			textbox->setContentsRect(centeredRect);
			
			//qDebug()<<"BibleBrowser::addSlide(): [templated] slideNbr: "<<slideNbr<<": centeredRect: "<<centeredRect<<", templateTextbox:"<<templateTextbox<<", realHeight:"<<realHeight; 
		}
		else
		if(heightDifference > 20)
		{
			// Center it within its own contents rect
			qreal y = qMax(0, textRect.height()/2 - natSize.height()/2);
			
			QRectF centeredRect(textRect.x(),textRect.y() + y,textRect.width(),natSize.height());
			
			//textbox->setPos(QPointF(0,0));
			textbox->setContentsRect(centeredRect);
		}
	}

};

SlideTemplateManager * SlideTemplateManager::m_staticInstance = 0;

void SlideTemplateManager::setupTemplateManager()
{
	m_staticInstance = new SlideTemplateManager();
}
	
SlideTemplateManager::SlideTemplateManager()
{
	m_docHash[Generic] = loadOrCreate(Generic);
	m_docHash[Bible]   = loadOrCreate(Bible);
	m_docHash[Songs]   = loadOrCreate(Songs);
	m_docHash[Logo]   = loadOrCreate(Logo);
}

SlideTemplateManager::~SlideTemplateManager()
{
	templateDocument(Generic)->save();
	templateDocument(Bible)->save();
	templateDocument(Songs)->save();
	templateDocument(Logo)->save();
}

Document * SlideTemplateManager::loadOrCreate(TemplateType type)
{
	QString file = typeToFile(type);
	qDebug() << "SlideTemplateManager::loadOrCreate("<<type<<"): file:"<<file;
	if(QFile(file).exists())
		return new Document(file);
	else
		return templateDocument(type); // auto-creates document and saves
}

Document * SlideTemplateManager::templateDocument(TemplateType type)
{
	if(m_docHash[type])
	{
// 		qDebug() << "SlideTemplateManager::templateDocument("<<type<<"): m_docHash has type, returning doc ptr";
		return m_docHash[type];
	}
	
// 	qDebug() << "SlideTemplateManager::templateDocument("<<type<<"): doc not loaded, creating new document";
	Document * doc = new Document();
	QString file = typeToFile(type);
	doc->save(file);
// 	qDebug() << "\t saving new document to "<<file;
	m_docHash[type] = doc;
	return doc;
}

SlideGroup * SlideTemplateManager::findTemplate(TemplateType type, int groupId) 
{ 
	return templateDocument(type)->groupById(groupId); 
}


QString SlideTemplateManager::typeToFile(TemplateType type)
{
	return QString("%1/%2")
		.arg(AppSettings::templateStorageFolder())
		.arg(
			type == Bible ? "DVizBibleTemplates.dviz" :
			type == Songs ? "DVizSongTemplates.dviz"  :
			type == Logo  ? "DVizLogoTemplates.dviz"  :
			"DVizUserTemplates.dviz"
		);
}

QString SlideTemplateManager::typeToString(TemplateType type)
{
	return  type == Bible ? "Bible": 
		type == Songs ? "Song" :
		type == Logo ? "Logo" : 
		"User";
	
}

/// TemplateSelectorWidget
#include <QHBoxLayout>
#include <QLabel>
TemplateSelectorWidget::TemplateSelectorWidget(SlideTemplateManager::TemplateType type, const QString & labelText, QWidget *parent)
	: QWidget(parent)
	, m_labelText(labelText)
	, m_type(type)
	, m_doc(0)
	, m_model(new DocumentListModel)
	, m_selected(0)
	, m_editWin(0)
{
	m_doc = SlideTemplateManager::instance()->templateDocument(type);
	m_model->setDocument(m_doc);
	setupUI();
}
TemplateSelectorWidget::~TemplateSelectorWidget() {}
	
SlideGroup * TemplateSelectorWidget::selectedGroup()
{
	if(!m_selected)
		m_selected = m_doc->at(0);
		
	return m_selected;
}
	
	
void TemplateSelectorWidget::setSelectedGroup(SlideGroup* group)
{
	m_selected = group;
	QModelIndex idx = m_model->indexForGroup(group);
	if(idx.isValid())
		m_templateBox->setCurrentIndex(idx.row());
}

void TemplateSelectorWidget::showSample(SlideGroup* group)
{
	/// NOT IMPL YET
}
	
void TemplateSelectorWidget::selectionChanged(int idx)
{
	m_selected = m_doc->at(idx);
	emit currentGroupChanged(m_selected);
}

void TemplateSelectorWidget::showSample()
{
	showSample(selectedGroup());
}

void TemplateSelectorWidget::editTemplate()
{
// 	if(selectedGroup())
// 		MainWindow::mw()->editGroup(selectedGroup());
		
 	SlideGroup * tmpl = selectedGroup();
 	if(!tmpl)
 	{
 		newTemplate();
 		return;
 	}
 		
 	MainWindow::mw()->editGroup(selectedGroup());
	
	if(m_editWin)
	{
		m_editWin->show();
		m_editWin->raise();
		m_editWin->setSlideGroup(tmpl);
	}
	else
	if((m_editWin = MainWindow::mw()->openSlideEditor(tmpl)) != 0)
		connect(m_editWin, SIGNAL(closed()), this, SLOT(editorWindowClosed()));
}

void TemplateSelectorWidget::delTemplate()
{
// 	if(selectedGroup())
// 		MainWindow::mw()->editGroup(selectedGroup());
		
 	SlideGroup * tmpl = selectedGroup();
 	if(!tmpl)
 		return;
 		
 	SlideGroup *group = selectedGroup();
	
	int idx = m_doc->groupList().indexOf(group);
	m_doc->removeGroup(group);
	
	setSelectedGroup(m_doc->at(qMax(0,idx-1)));
	
	// force save
	SlideTemplateManager::instance()->templateDocument(m_type)->save();
}

void TemplateSelectorWidget::editorWindowClosed()
{
	
// 	if(m_editWin)
// 		disconnect(m_editWin, 0, this, 0);
	
	SlideTemplateManager::instance()->templateDocument(m_type)->save();
	
  	//QTimer::singleShot(1000,m_editWin,SLOT(deleteLater()));
  	//m_editWin->deleteL
//  	m_editWin = 0;
	
	//raise();
	//setFocus();
	//qDebug() << "SongEditorWindow::editorWindowClosed(): setting focus to self:"<<this<<", m_editWin="<<m_editWin;
}


void TemplateSelectorWidget::newTemplate()
{
	Slide * slide = new Slide();
	SlideGroup *g = new SlideGroup();
	g->setGroupTitle(QString("%2 %1").arg(m_doc->numGroups()+1).arg(SlideTemplateManager::typeToString(m_type)));
	g->addSlide(slide);
	m_doc->addGroup(g);
	
	setSelectedGroup(g);
	editTemplate();
}

void TemplateSelectorWidget::setupUI()
{
	QHBoxLayout * hbox = new QHBoxLayout(this);
	hbox->setContentsMargins(0,0,0,0);
	
	if(!m_labelText.isEmpty())
	{
		QLabel * label = new QLabel(m_labelText,this);
		hbox->addWidget(label);
	}
	
	m_templateBox = new QComboBox(this);
	m_templateBox->setModel(m_model);
	connect(m_templateBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectionChanged(int)));
	hbox->addWidget(m_templateBox);
	
// 	m_sampleButton = new QPushButton("Sample",this);
// 	connect(m_sampleButton, SIGNAL(clicked()), this, SLOT(showSample()));
// 	hbox->addWidget(m_sampleButton);
	
	m_editButton = new QPushButton(QPixmap(":/data/stock-edit.png"),"",this);
	m_editButton->setToolTip("Edit Selected Template");
	connect(m_editButton, SIGNAL(clicked()), this, SLOT(editTemplate()));
	hbox->addWidget(m_editButton);
	
	m_newButton = new QPushButton(QPixmap(":/data/stock-new.png"),"",this);
	m_newButton->setToolTip("Create a new template");
	connect(m_newButton, SIGNAL(clicked()), this, SLOT(newTemplate()));
	hbox->addWidget(m_newButton);
	
	m_delButton= new QPushButton(QPixmap(":/data/stock-delete.png"),"",this);
	m_delButton->setToolTip("Delete Selected template");
	connect(m_delButton, SIGNAL(clicked()), this, SLOT(delTemplate()));
	hbox->addWidget(m_delButton);
	
	hbox->addStretch(1);
	
}

