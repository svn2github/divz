#include "SlideSettingsDialog.h"
#include "ui_SlideSettingsDialog.h"
#include "model/Slide.h"
#include "model/BackgroundItem.h"
#include "model/ImageItem.h"
#include "model/TextBoxItem.h"
#include "items/BackgroundConfig.h"
#include "items/BackgroundContent.h"
#include <QMessageBox>
#include "ui_GenericItemConfig.h"


SlideSettingsDialog::SlideSettingsDialog(Slide *slide, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SlideSettingsDialog),
    m_slide(slide)
{
    m_ui->setupUi(this);
    m_ui->slideChangeTime->setValue(slide->autoChangeTime());
    connect(m_ui->slideChangeTime, SIGNAL(valueChanged(double)), this, SLOT(setAutoChangeTime(double)));

    connect(m_ui->btnNever, SIGNAL(clicked()), this, SLOT(slotNever()));
    connect(m_ui->btnGuess, SIGNAL(clicked()), this, SLOT(slotGuess()));

    //// Not implemented yet, so hide from UI
    //m_ui->boxConfigBg->setVisible(false);
    //connect(m_ui->btnConfigBg, SIGNAL(clicked()), this, SLOT(configBg()));

    BackgroundContent * bg = dynamic_cast<BackgroundContent*>(dynamic_cast<AbstractVisualItem*>(m_slide->background())->createDelegate());
    BackgroundConfig *config = new BackgroundConfig(bg);
    
    m_ui->tabWidget->addTab(config->m_commonUi->backgroundTab,"Background");
    //config->setWindowModality(Qt::WindowModal);
    

    setWindowTitle("Slide Settings");
    m_ui->slideChangeTime->setFocus(Qt::OtherFocusReason);
    
    //adjustSize();
    //resize(577, 494);

}

SlideSettingsDialog::~SlideSettingsDialog()
{
    delete m_ui;
}

void SlideSettingsDialog::setAutoChangeTime(double d)
{
    m_slide->setAutoChangeTime(d);
}

void SlideSettingsDialog::slotNever()
{
	m_ui->slideChangeTime->setValue(0);
}

void SlideSettingsDialog::slotGuess()
{
	double guess = 0;
	
	double assumedImageArea = 1024 * 768;
	double perItemWeight = 0.1;
	double perImageWeightFactorOfArea = 1/(assumedImageArea / 2);
	double perTextWeightFactorOfLength = 60.0 / (300.0 * 5.0);
	// formula for text factor is: 60seconds / (300.0 avg words per minute for avg adult * 5 letters per avg word length)
	
	static QRegExp rxToText("<[^>]*>");
	
	bool special = false;
	QList<AbstractItem *> items = m_slide->itemList();
	foreach(AbstractItem *item, items)
	{
		AbstractVisualItem * vis = dynamic_cast<AbstractVisualItem*>(item);
		if(vis)
		{
			//qDebug() << "Processing visual:"<<vis->itemName();
			special = false;
			if(vis->itemClass() == BackgroundItem::ItemClass)
			{
				if(vis->fillType() == AbstractVisualItem::Image ||
				   vis->fillType() == AbstractVisualItem::Video)
				{
					
					guess += assumedImageArea * perImageWeightFactorOfArea;
					special = true;
				}
			}
			else
			if(vis->itemClass() == ImageItem::ItemClass)
			{
				QRectF cRect = vis->contentsRect();
				guess += cRect.width() * cRect.height() * perImageWeightFactorOfArea;
				special = true;
			}
			else
			if(vis->itemClass() == TextBoxItem::ItemClass)
			{
				TextBoxItem * textBox = dynamic_cast<TextBoxItem*>(vis);
				QString text = textBox->text().replace( rxToText, "" );
				double g = ((double)text.length()) * perTextWeightFactorOfLength;
				//qDebug() << "Found textbox:"<<vis->itemName()<<", length:"<<text.length()<<", g:"<<g<<", factor:"<<perTextWeightFactorOfLength;
				guess += g;
				special = true;
			}
			
			if(!special)
			{
				//qDebug() << "Visual:"<<vis->itemName()<<" not special";
				guess += perItemWeight;
			}
		}
	}

	
	m_ui->slideChangeTime->setValue(guess);
}

void SlideSettingsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
