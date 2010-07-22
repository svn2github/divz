
#include "MdiVideoSource.h"

MdiVideoSource::MdiVideoSource(QWidget *parent)
	: MdiChild(parent)
	, m_layout(new QVBoxLayout(this))
	, m_textInput(new QLineEdit(this))
	, m_videoSource(0)
	, m_videoWidget(new VideoWidget())
{
	connect(m_videoWidget, SIGNAL(clicked()), this, SIGNAL(clicked()));
	connect(m_textInput, SIGNAL(returnPressed()), this, SLOT(textReturnPressed()));
	setupGui();
}
	
void MdiVideoSource::setVideoSource(VideoSource* source)
{
	m_videoSource = source;
	m_videoWidget->setVideoSource(source);
}
	
void MdiVideoSource::textReturnPressed()
{
	m_videoWidget->setOverlayText(m_textInput->text());
}

void MdiVideoSource::setupGui()
{
	setupDefaultGui();
}

void MdiVideoSource::setupDefaultGui()
{
	m_layout->setContentsMargins(3,3,3,3);
	m_layout->addWidget(m_videoWidget);
	m_layout->addWidget(m_textInput);
}
