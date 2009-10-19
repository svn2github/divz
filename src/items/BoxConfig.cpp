#include "BoxConfig.h"
#include "BoxContent.h"

#include "ui_GenericItemConfigBase.h"

BoxConfig::BoxConfig(BoxContent * v, QGraphicsItem * parent)
    : GenericItemConfig(v)
{
	m_commonUi->bgOptImage->setVisible(false);
	m_commonUi->bgImage->setVisible(false);
	m_commonUi->bgVideo->setVisible(false);
	m_commonUi->bgOptVideo->setVisible(false);
	
 	int idx = m_commonUi->tabWidget->indexOf(m_commonUi->backgroundTab);
// 	m_commonUi->tabWidget->setTabText(idx,"Video File");
 	m_commonUi->tabWidget->setCurrentIndex(idx);
// 	m_commonUi->videoFilenameBox->setFocus();
	
}

BoxConfig::~BoxConfig()
{
}

void BoxConfig::slotOkClicked()
{
}
