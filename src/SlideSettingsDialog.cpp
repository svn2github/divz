#include "SlideSettingsDialog.h"
#include "ui_SlideSettingsDialog.h"
#include "model/Slide.h"
#include "model/SlideGroup.h"
#include "model/BackgroundItem.h"
#include "model/ImageItem.h"
#include "model/TextBoxItem.h"
#include "items/BackgroundConfig.h"
#include "items/BackgroundContent.h"
#include "SlideGroupListModel.h"
#include <QMessageBox>
#include "ui_GenericItemConfig.h"


SlideSettingsDialog::SlideSettingsDialog(Slide *slide, QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::SlideSettingsDialog),
	m_slide(slide),
	m_primaryGroupModel(0)
{
	m_ui->setupUi(this);
	m_ui->slideChangeTime->setValue(slide->autoChangeTime());
	connect(m_ui->slideChangeTime, SIGNAL(valueChanged(double)), this, SLOT(setAutoChangeTime(double)));
	
	connect(m_ui->btnNever, SIGNAL(clicked()), this, SLOT(slotNever()));
	connect(m_ui->btnGuess, SIGNAL(clicked()), this, SLOT(slotGuess()));
	
	BackgroundContent *bg = dynamic_cast<BackgroundContent*>(dynamic_cast<AbstractVisualItem*>(m_slide->background())->createDelegate());
	BackgroundConfig *config = new BackgroundConfig(bg);
	
	m_ui->tabWidget->addTab(config->m_commonUi->backgroundTab,"Background");
	//config->setWindowModality(Qt::WindowModal);
	
	setWindowTitle("Slide Settings");
	setWindowIcon(QIcon(":/data/icon-d.png"));
	
	m_ui->slideChangeTime->setFocus(Qt::OtherFocusReason);
	
	if(m_slide->inheritFadeSettings())
		m_ui->btnUseGroup->setChecked(true);
	else
		m_ui->btnUseSlide->setChecked(true);
	
	m_ui->speedBox->setValue(m_slide->crossFadeSpeed());
	m_ui->qualityBox->setValue(m_slide->crossFadeQuality());
	m_ui->slideName->setText(m_slide->slideName());
	
	m_ui->slideName->setFocus();
	
	m_ui->primarySlideBox->setVisible(false);
	
	connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(slotAccepted()));
}

void SlideSettingsDialog::setPrimaryGroup(SlideGroup *primary)
{
	m_ui->primarySlideBox->setVisible(true);
	
	m_primaryGroupModel = new SlideGroupListModel();
	m_ui->primarySlideList->setModel(m_primaryGroupModel);
	m_primaryGroupModel->setSlideGroup(primary);
	
	Slide *primarySlide = primary->slideById(m_slide->primarySlideId());
	QModelIndex modelIndex = m_primaryGroupModel->indexForSlide(primarySlide);
	qDebug() << "SlideSettingsDialog::setPrimaryGroup: m_slide->primarySlideId():"<<m_slide->primarySlideId()<<",primarySlide:"<<primarySlide<<",modelIndex:"<<modelIndex; 
	if(modelIndex.row() > -1)
		m_ui->primarySlideList->setCurrentIndex(modelIndex.row());
}

void SlideSettingsDialog::slotAccepted()
{
	m_slide->setInheritFadeSettings(m_ui->btnUseGroup->isChecked());
	m_slide->setCrossFadeSpeed(m_ui->speedBox->value());
	m_slide->setCrossFadeQuality(m_ui->qualityBox->value());
	m_slide->setSlideName(m_ui->slideName->text());
	
	if(m_primaryGroupModel)
	{
		int idx = m_ui->primarySlideList->currentIndex();
		if(idx >= 0)
		{
			Slide *slide = m_primaryGroupModel->slideAt(idx);
			m_slide->setPrimarySlideId(slide->slideId());
			
			qDebug() << "SlideSettingsDialog::slotAccepted(): idx:"<<idx<<", slide:"<<slide<<",slide->slideId():"<<slide->slideId();  
		}
		else
		{
			QMessageBox::critical(this,"Select a Primary Slide","No primary slide selected - choose one and try again.");
			return;
		}
	}
	else
	{
		qDebug() << "SlideSettingsDialog::slotAccepted(): not alternate group";
	}
	
	close();
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
	m_ui->slideChangeTime->setValue(m_slide->guessTimeout());
}

void SlideSettingsDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) 
	{
		case QEvent::LanguageChange:
			m_ui->retranslateUi(this);
			break;
		default:
			break;
	}
}
