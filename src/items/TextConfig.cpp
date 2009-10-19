#include "TextConfig.h"
#include "3rdparty/richtextedit/richtexteditor_p.h"
#include "TextContent.h"
#include <QVBoxLayout>
#include <QPushButton>
#include "ui_GenericItemConfigBase.h"

TextConfig::TextConfig(TextContent * textContent, QGraphicsItem * parent)
    : AbstractConfig(textContent)
    , m_textContent(textContent)
{
    // inject Text Editor
    QWidget *base = new QWidget();
    
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    
    m_editor = new RichTextEditorDialog();
    m_editor->setMinimumSize(320, 200);
    m_editor->setText(m_textContent->toHtml());
    m_editor->adjustSize();
    addTab(m_editor, tr("Text"), false, true);
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

    // show the ok button
    //showOkButton(true);
}

TextConfig::~TextConfig()
{
}

void TextConfig::applyTextSlot()
{
    m_textContent->setHtml(m_editor->text(Qt::RichText));
    //slotRequestClose();
}

void TextConfig::resetTextSlot()
{
    m_editor->setText(m_textContent->toHtml());
}
