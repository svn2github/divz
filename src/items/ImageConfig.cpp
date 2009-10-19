#include "ImageConfig.h"
#include "ImageContent.h"

#include "ui_GenericItemConfigBase.h"

ImageConfig::ImageConfig(ImageContent * v, QGraphicsItem * parent)
    : GenericItemConfig(v)
{
	m_commonUi->bgOptGradient->setVisible(false);
	m_commonUi->bgGradient->setVisible(false);
	m_commonUi->bgOptColor->setVisible(false);
	m_commonUi->bgColor->setVisible(false);
	//m_commonUi->bgOptImage->setVisible(false);
	m_commonUi->bgImage->setVisible(false);
	m_commonUi->bgOptNoBg->setVisible(false);
	m_commonUi->bgNone->setVisible(false);
	m_commonUi->bgOptVideo->setVisible(false);
	m_commonUi->bgVideo->setVisible(false);
	
	int idx = m_commonUi->tabWidget->indexOf(m_commonUi->backgroundTab);
	m_commonUi->tabWidget->setTabText(idx,"Image File");
	m_commonUi->tabWidget->setCurrentIndex(idx);
	m_commonUi->videoFilenameBox->setFocus();
	
}

ImageConfig::~ImageConfig()
{
}

void ImageConfig::slotOkClicked()
{
}
