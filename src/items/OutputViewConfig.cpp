#include "OutputViewConfig.h"
#include "OutputViewContent.h"
#include "model/OutputViewItem.h"
#include "AppSettings.h"
#include "model/Output.h"
#include <QComboBox>
#include "ui_GenericItemConfig.h"

OutputViewConfig::OutputViewConfig(OutputViewContent * v, QGraphicsItem * parent)
    : GenericItemConfig(v)
{
	m_commonUi->bgOptImage->setVisible(false);
	m_commonUi->bgImage->setVisible(false);
	m_commonUi->bgVideo->setVisible(false);
	m_commonUi->bgOptVideo->setVisible(false);
	
	OutputViewItem * model = dynamic_cast<OutputViewItem*>(v->modelItem());
	
	QWidget * base = new QWidget();
	
	QVBoxLayout *vbox = new QVBoxLayout(base);
	
	QHBoxLayout *layout = new QHBoxLayout();
	QLabel *label = new QLabel("Show Output: ");
	layout->addWidget(label);
	
	vbox->addLayout(layout);
	vbox->addStretch(2);
	
	m_syncWithBox = new QComboBox();
	layout->addWidget(m_syncWithBox);
	connect(m_syncWithBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotOutputChanged(int)));
	
	m_syncWithBox->clear();
	QList<Output*> allOut = AppSettings::outputs();
	
	foreach(Output *out, allOut)
	{
		if(out->isEnabled()) // && (m_inst ? out->id() != m_inst->output()->id() : true))
		{
			m_syncWithBox->addItem(out->name(), QVariant(out->id()));
			qDebug() << "OutputViewConfig: outputId "<<out->id()<<"="<<out->name();
		}
	}
	
	int idx = m_syncWithBox->findData(model->outputId());
	if(idx>-1)
		m_syncWithBox->setCurrentIndex(idx);
	
	addTab(base, tr("Output"), false, true);
	
 	//int idx = m_commonUi->tabWidget->indexOf(m_commonUi->backgroundTab);
// 	m_commonUi->tabWidget->setTabText(idx,"Video File");
 	//m_commonUi->tabWidget->setCurrentIndex(idx);
// 	m_commonUi->videoFilenameBox->setFocus();
	
}

void OutputViewConfig::slotOutputChanged(int idx)
{
	if(idx<0)
	{	
		qDebug() << "slotOutputChanged: indvalid idx , < 0";
		return;
	}
		
	int outputId = m_syncWithBox->itemData(idx).toInt();
	OutputViewItem * model = dynamic_cast<OutputViewItem*>(content()->modelItem());
	model->setOutputId(outputId);
}

OutputViewConfig::~OutputViewConfig()
{
}

void OutputViewConfig::slotOkClicked()
{
}
