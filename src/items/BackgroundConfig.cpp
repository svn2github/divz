#include "BackgroundConfig.h"
#include "BackgroundContent.h"

#include "ui_GenericItemConfig.h"

BackgroundConfig::BackgroundConfig(BackgroundContent * v, QWidget * parent)
    : GenericItemConfig(v,parent)
    , m_bg(v)
{
// 	m_commonUi->bgOptGradient->setVisible(false);
// 	m_commonUi->bgOptColor->setVisible(false);
// 	m_commonUi->bgOptImage->setVisible(false);
// 	m_commonUi->bgOptNoBg->setVisible(false);
// 	m_commonUi->bgVideo->setVisible(false);
	
	m_commonUi->tabWidget->removeTab(m_commonUi->tabWidget->indexOf(m_commonUi->outlineTab));
	m_commonUi->tabWidget->removeTab(m_commonUi->tabWidget->indexOf(m_commonUi->generalTab));
	m_commonUi->tabWidget->removeTab(m_commonUi->tabWidget->indexOf(m_commonUi->shadowTab));
	//m_commonUi->tabWidget->removeTab(m_commonUi->tabWidget->indexOf(m_commonUi->reflectionTab));
	m_commonUi->gbMirror->setVisible(false);
	
	int idx = m_commonUi->tabWidget->indexOf(m_commonUi->backgroundTab);
	//m_commonUi->tabWidget->setTabText(idx,"Video File");
	m_commonUi->tabWidget->setCurrentIndex(idx);
	//m_commonUi->videoFilenameBox->setFocus();
	
	
}

BackgroundConfig::~BackgroundConfig()
{
}

void BackgroundConfig::slotOkClicked()
{
}
