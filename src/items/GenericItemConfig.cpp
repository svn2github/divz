#include "AbstractContent.h"
#include "MyGraphicsScene.h"
#include "RenderOpts.h"
#include "StyledButtonItem.h"
#include "frames/FrameFactory.h"
#include <QGraphicsSceneMouseEvent>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QPainter>
#include <QPixmapCache>
#include <QPushButton>
#include <QStyle>
#include <QWidget>
#include <QSettings>

#include "GenericItemConfig.h"
#include "ui_GenericItemConfig.h"

GenericItemConfig::GenericItemConfig(AbstractContent * content, QWidget *parent) :
	QDialog(parent)
	, m_commonUi(new Ui::GenericItemConfig())
	, m_content(content)
	, m_closeButton(0)
	, m_okButton(0)
	, m_frame(FrameFactory::defaultPanelFrame())
{
	m_commonUi->setupUi(this);
	
	// read other properties
	if(m_content->mirrorEnabled())
	{
		m_commonUi->reflectionEnabled->setChecked(true);
	}
	else
	{
		m_commonUi->reflectionNone->setChecked(true);
	}
	
	m_commonUi->mirrorOffset->setValue(m_content->modelItem()->mirrorOffset());
	
	m_origOpacity = m_content->opacity();
	m_commonUi->opacityBox->setValue((int)( m_origOpacity * 100 ));
	
	m_origPos = m_content->pos();
	m_commonUi->locationX->setValue(m_origPos.x());
	m_commonUi->locationY->setValue(m_origPos.y());
	
	m_origSize = m_content->contentsRect().size();
	m_commonUi->contentWidth->setValue((double)m_origSize.width());
	m_commonUi->contentHeight->setValue((double)m_origSize.height());
	
	
	
	connect(m_commonUi->front, SIGNAL(clicked()), m_content, SLOT(slotStackFront()));
	connect(m_commonUi->raise, SIGNAL(clicked()), m_content, SLOT(slotStackRaise()));
	connect(m_commonUi->lower, SIGNAL(clicked()), m_content, SLOT(slotStackLower()));
	connect(m_commonUi->back, SIGNAL(clicked()), m_content, SLOT(slotStackBack()));
	connect(m_commonUi->background, SIGNAL(clicked()), m_content, SIGNAL(backgroundMe()));
	connect(m_commonUi->save, SIGNAL(clicked()), m_content, SLOT(slotSaveAs()));
	connect(m_commonUi->del, SIGNAL(clicked()), m_content, SIGNAL(deleteItem()), Qt::QueuedConnection);
	// autoconnection doesn't work because we don't do ->setupUi(this), so here we connect manually
// 	connect(m_commonUi->applyLooks, SIGNAL(clicked()), this, SLOT(on_applyLooks_clicked()));
// 	connect(m_commonUi->newFrame, SIGNAL(clicked()), this, SLOT(on_newFrame_clicked()));
// 	connect(m_commonUi->listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(on_listWidget_itemSelectionChanged()));
	connect(m_commonUi->reflectionEnabled, SIGNAL(toggled(bool)), this, SLOT(slotMirrorOn(bool)));
	//connect(m_commonUi->reflectionNone, SIGNAL(toggled(bool)), this, SLOT(slotMirrorOff(bool)));
	connect(m_commonUi->buttonBox, SIGNAL(accepted()), this, SLOT(slotOkClicked()));
	connect(m_commonUi->buttonBox, SIGNAL(accepted()), this, SLOT(slotClosed()));
	connect(m_commonUi->buttonBox, SIGNAL(rejected()), this, SLOT(slotClosed()));
	connect(m_commonUi->opacityBox, SIGNAL(valueChanged(int)), this, SLOT(slotOpacityChanged(int)));
	
	connect(m_commonUi->sizeReset,     SIGNAL(clicked()), this, SLOT(slotResetSize()));
	connect(m_commonUi->locationReset, SIGNAL(clicked()), this, SLOT(slotResetLocation()));
	connect(m_commonUi->opacityReset,  SIGNAL(clicked()), this, SLOT(slotResetOpacity()));
	
	connect(m_commonUi->locationX, SIGNAL(valueChanged(double)), this, SLOT(slotLocationChanged(double)));
	connect(m_commonUi->locationY, SIGNAL(valueChanged(double)), this, SLOT(slotLocationChanged(double)));
	
	connect(m_commonUi->contentWidth, SIGNAL(valueChanged(double)), this, SLOT(slotSizeChanged(double)));
	connect(m_commonUi->contentHeight, SIGNAL(valueChanged(double)), this, SLOT(slotSizeChanged(double)));
}

GenericItemConfig::~GenericItemConfig()
{
	delete m_commonUi;
}

void GenericItemConfig::slotClosed()
{
     MyGraphicsScene * desk = static_cast<MyGraphicsScene*>(scene());
     desk->slotDeleteConfig(this);
//	done();
}


void GenericItemConfig::dispose()
{
/*#if QT_VERSION >= 0x040600
    // fade out animation, then delete
    QPropertyAnimation * ani = new QPropertyAnimation(this, "opacity");
    connect(ani, SIGNAL(finished()), this, SLOT(deleteLater()));
    ani->setEasingCurve(QEasingCurve::OutCubic);
    ani->setDuration(200);
    ani->setEndValue(0.0);
    ani->start(QPropertyAnimation::DeleteWhenStopped);
#else*/
    // delete this now
    deleteLater();
// #endif
}


void GenericItemConfig::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		m_commonUi->retranslateUi(this);
		break;
	default:
		break;
	}
}

AbstractContent * GenericItemConfig::content() const
{
    return m_content;
}


void GenericItemConfig::addTab(QWidget * widget, const QString & label, bool front, bool setCurrent)
{
	// insert on front/back
	int idx = m_commonUi->tabWidget->insertTab(front ? 0 : m_commonUi->tabWidget->count(), widget, label);
	
	// show if requested
	if (setCurrent)
		m_commonUi->tabWidget->setCurrentIndex(idx);
	
	// adjust size after inserting the tab
	if (m_commonUi->tabWidget->parentWidget())
		m_commonUi->tabWidget->parentWidget()->adjustSize();
}

// void GenericItemConfig::slotRequestClose()
// {
// 	MyGraphicsScene * desk = static_cast<MyGraphicsScene*>(scene());
// 	desk->slotDeleteConfig(this);
// 	//	done();
// }
// 
// 


void GenericItemConfig::slotMirrorOn(bool checked)
{
	RenderOpts::LastMirrorEnabled = checked;
	emit applyLook(m_content->frameClass(), checked, false);
}



void GenericItemConfig::slotOpacityChanged(int o)
{
	m_content->setOpacity((double)o / 100);
}

void GenericItemConfig::slotResetOpacity()
{
	m_content->setOpacity(m_origOpacity);
	m_commonUi->opacityBox->setValue((int)(m_origOpacity * 100));
	m_commonUi->opacityBox->selectAll();
}

void GenericItemConfig::slotResetLocation()
{
	m_content->setPos(m_origPos);
	m_commonUi->locationX->setValue(m_origPos.x());
	m_commonUi->locationY->setValue(m_origPos.y());
	m_commonUi->locationX->selectAll();
	m_commonUi->locationY->selectAll();
}

void GenericItemConfig::slotResetSize()
{
	QRect cr = m_content->contentsRect();
	cr.setSize(m_origSize);
	m_content->resizeContents(cr);
	
	m_commonUi->contentWidth->setValue((double)m_origSize.width());
	m_commonUi->contentHeight->setValue((double)m_origSize.height());
	m_commonUi->contentWidth->selectAll();
	m_commonUi->contentHeight->selectAll();
}


	
void GenericItemConfig::slotSizeChanged(double)
{
	QSize s;
	s.setWidth(m_commonUi->contentWidth->value());
	s.setHeight(m_commonUi->contentHeight->value());
	
	QRect cr = m_content->contentsRect();
	cr.setSize(m_origSize);
	m_content->resizeContents(cr);
}


void GenericItemConfig::slotLocationChanged(double)
{
	QPointF p;
	p.setX(m_commonUi->locationX->value());
	p.setY(m_commonUi->locationY->value());
	m_content->setPos(p);
	
}
