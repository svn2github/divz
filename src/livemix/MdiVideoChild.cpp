
#include "MdiVideoChild.h"

MdiVideoChild::MdiVideoChild(QWidget *parent)
	: MdiChild(parent)
	, m_layout(new QVBoxLayout(this))
	, m_textInput(new QLineEdit(this))
	, m_videoSource(0)
	, m_videoWidget(new VideoWidget())
{
	connect(m_videoWidget, SIGNAL(clicked()), this, SIGNAL(clicked()));
	connect(m_textInput, SIGNAL(returnPressed()), this, SLOT(textReturnPressed()));
	setupDefaultGui();
}
	
void MdiVideoChild::setVideoSource(VideoSource* source)
{
	m_videoSource = source;
	m_videoWidget->setVideoSource(source);
}
	
void MdiVideoChild::textReturnPressed()
{
	m_videoWidget->setOverlayText(m_textInput->text());
	m_textInput->selectAll();
}


void MdiVideoChild::setupDefaultGui()
{
	m_layout->setContentsMargins(3,3,3,3);
	m_layout->addWidget(m_videoWidget);
	m_layout->addWidget(m_textInput);
}
