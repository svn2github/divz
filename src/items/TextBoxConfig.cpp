#include "TextBoxConfig.h"
#include "3rdparty/richtextedit/richtexteditor_p.h"
#include "TextBoxContent.h"
#include "ui_GenericItemConfig.h"
#include "model/TextItem.h"

TextBoxConfig::TextBoxConfig(TextBoxContent * textContent, QGraphicsItem * parent)
    : GenericItemConfig(textContent)
    , m_textContent(textContent)
    , m_saving(false)
{
	// inject Text Editor
	m_editor = new RichTextEditorDialog();
	//m_editor->setMinimumSize(425, 400);
	
	TextItem * textModel = dynamic_cast<TextItem*>(m_textContent->modelItem());
	m_editor->setText(textModel->text());
	m_editor->initFontSize(textModel->findFontSize());
	
	m_editor->adjustSize();
	addTab(m_editor, tr("Text"), false, true);
	m_editor->focusEditor();	
	
	m_commonUi->bgOptImage->setVisible(false);
	m_commonUi->bgImage->setVisible(false);
	m_commonUi->bgOptVideo->setVisible(false);
	m_commonUi->bgVideo->setVisible(false);
	
	
	
	//resize(640,300);
	connect(textContent->modelItem(), SIGNAL(itemChanged(QString, QVariant, QVariant)), this, SLOT(itemChanged(QString, QVariant, QVariant)));
}

void TextBoxConfig::itemChanged(QString fieldName, QVariant value, QVariant)
{
	if(fieldName == "text" && !m_saving)
	{
		TextItem * textModel = dynamic_cast<TextItem*>(m_textContent->modelItem());
		m_editor->setText(textModel->text());
		m_editor->initFontSize(textModel->findFontSize());
	}
}

TextBoxConfig::~TextBoxConfig()
{
}

void TextBoxConfig::slotOkClicked()
{
	TextItem * textModel = dynamic_cast<TextItem*>(m_textContent->modelItem());
	m_saving = true;
	textModel->setText(m_editor->text(Qt::RichText));
	m_saving = false;
}
