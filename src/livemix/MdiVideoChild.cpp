
#include "MdiVideoChild.h"

#include "VideoWidget.h"

MdiVideoChild::MdiVideoChild(QWidget *parent)
	: MdiChild(parent)
	, m_layout(new QVBoxLayout(this))
	, m_videoSource(0)
	, m_videoWidget(new VideoWidget())
	, m_configMenu(0)
{
	connect(m_videoWidget, SIGNAL(clicked()), this, SIGNAL(clicked()));
	setupDefaultGui();
}
	
void MdiVideoChild::setVideoSource(VideoSource* source)
{
	m_videoSource = source;
	m_videoWidget->setVideoSource(source);
}

void MdiVideoChild::setupDefaultGui()
{
	m_layout->setContentsMargins(3,3,3,3);
	m_layout->addWidget(m_videoWidget);
}


void MdiVideoChild::contextMenuEvent(QContextMenuEvent * event)
{
	if(m_configMenu)
		m_configMenu->popup(event->globalPos());
}
