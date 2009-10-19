#include "VideoFileConfig.h"
#include "VideoFileContent.h"

#include "ui_GenericItemConfigBase.h"

VideoFileConfig::VideoFileConfig(VideoFileContent * v, QGraphicsItem * parent)
    : GenericItemConfig(v)
    , m_vid(v)
{
	m_commonUi->bgOptGradient->setVisible(false);
	m_commonUi->bgGradient->setVisible(false);
	m_commonUi->bgOptColor->setVisible(false);
	m_commonUi->bgColor->setVisible(false);
	m_commonUi->bgOptImage->setVisible(false);
	m_commonUi->bgImage->setVisible(false);
	m_commonUi->bgOptNoBg->setVisible(false);
	m_commonUi->bgNone->setVisible(false);
	m_commonUi->bgVideo->setVisible(false);
	
	int idx = m_commonUi->tabWidget->indexOf(m_commonUi->backgroundTab);
	m_commonUi->tabWidget->setTabText(idx,"Video File");
	m_commonUi->tabWidget->setCurrentIndex(idx);
	m_commonUi->videoFilenameBox->setFocus();
	
}

VideoFileConfig::~VideoFileConfig()
{
}

void VideoFileConfig::slotOkClicked()
{
}
