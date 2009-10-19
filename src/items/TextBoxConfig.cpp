#include "TextBoxConfig.h"
#include "3rdparty/richtextedit/richtexteditor_p.h"
#include "TextBoxContent.h"
#include "ui_GenericItemConfigBase.h"
#include "model/TextItem.h"

TextBoxConfig::TextBoxConfig(TextBoxContent * textContent, QGraphicsItem * parent)
    : GenericItemConfig(textContent)
    , m_textContent(textContent)
{
	 QWidget *base = new QWidget();
    
	QVBoxLayout *layout = new QVBoxLayout();
	layout->setContentsMargins(0,0,0,0);
	
	// inject Text Editor
	m_editor = new RichTextEditorDialog();
	m_editor->setMinimumSize(425, 400);
	
	TextItem * textModel = dynamic_cast<TextItem*>(m_textContent->modelItem());
	m_editor->setText(textModel->text());
	
	m_editor->adjustSize();
	//addTab(m_editor, tr("Text"), false, true);
	
	// insert on front/back
	//int idx = m_commonUi->tabWidget->insertTab(m_commonUi->tabWidget->count(), m_editor, "Text");
	int idx = m_commonUi->tabWidget->addTab(m_editor, "Text");
	qDebug() << "TextBoxConfig(): addTab(editor): idx:"<<idx;
	
	// show if requested
	m_commonUi->tabWidget->setCurrentIndex(idx);
	
	// adjust size after inserting the tab
	//if (m_commonUi->tabWidget->parentWidget())
	if(parentWidget())
		parentWidget()->adjustSize();
	else
		adjustSize();
		
		
	m_editor->focusEditor();	
	
	layout->addWidget(m_editor);
    
    
	QHBoxLayout *hbox = new QHBoxLayout();
	QPushButton *btn = new QPushButton("Apply Text");
	connect(btn,SIGNAL(clicked()), this, SLOT(applyTextSlot()));
	hbox->addWidget(btn);
	
	btn = new QPushButton("Reset");
	connect(btn,SIGNAL(clicked()), this, SLOT(resetTextSlot()));
	hbox->addWidget(btn);
	
	layout->addLayout(hbox);
	
	m_commonUi->bgOptImage->setVisible(false);
	m_commonUi->bgOptVideo->setVisible(false);
	
	resize(640,300);
}

TextBoxConfig::~TextBoxConfig()
{
}

void TextBoxConfig::applyTextSlot()
{
    m_textContent->setHtml(m_editor->text(Qt::RichText));
    //slotRequestClose();
}

void TextBoxConfig::resetTextSlot()
{
    m_editor->setText(m_textContent->toHtml());
}

