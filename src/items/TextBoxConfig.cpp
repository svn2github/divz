#include "TextBoxConfig.h"
#include "3rdparty/richtextedit/richtexteditor_p.h"
#include "TextBoxContent.h"

TextBoxConfig::TextBoxConfig(TextBoxContent * textContent, QGraphicsItem * parent)
    : GenericItemConfig(textContent)
    , m_textContent(textContent)
{
	// inject Text Editor
	m_editor = new RichTextEditorDialog();
	m_editor->setMinimumSize(425, 400);
	m_editor->setText(m_textContent->toHtml());
	m_editor->adjustSize();
	addTab(m_editor, tr("Text"), false, true);
	m_editor->focusEditor();	
}

TextBoxConfig::~TextBoxConfig()
{
}

void TextBoxConfig::slotOkClicked()
{
	m_textContent->setHtml(m_editor->text(Qt::RichText));
//	slotRequestClose();
}
