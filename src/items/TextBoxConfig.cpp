#include "TextBoxConfig.h"
#include "3rdparty/richtextedit/richtexteditor_p.h"
#include "TextBoxContent.h"
#include "ui_GenericItemConfig.h"
#include "model/TextItem.h"

TextBoxConfig::TextBoxConfig(TextBoxContent * textContent, QGraphicsItem * parent)
    : GenericItemConfig(textContent)
    , m_textContent(textContent)
{
	// inject Text Editor
	m_editor = new RichTextEditorDialog();
	m_editor->setMinimumSize(425, 400);
	
	TextItem * textModel = dynamic_cast<TextItem*>(m_textContent->modelItem());
	m_editor->setText(textModel->text());
	//qDebug() << "Editing text: "<<textModel->text();
	m_editor->adjustSize();
	addTab(m_editor, tr("Text"), false, true);
	m_editor->focusEditor();	
	
	m_commonUi->bgOptImage->setVisible(false);
	m_commonUi->bgOptVideo->setVisible(false);
	
	resize(640,300);
}

TextBoxConfig::~TextBoxConfig()
{
}

void TextBoxConfig::slotOkClicked()
{
	//m_textContent->setHtml(m_editor->text(Qt::RichText));
	TextItem * textModel = dynamic_cast<TextItem*>(m_textContent->modelItem());
	textModel->setText(m_editor->text(Qt::RichText));
//	slotRequestClose();
}
