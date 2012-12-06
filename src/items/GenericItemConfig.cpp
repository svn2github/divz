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
#include <QDebug>
#include <QShortcut>
#include <QDockWidget>

#include <qtcolorpicker.h>

#include "GenericItemConfig.h"
#include "ui_GenericItemConfig.h"

#include "AppSettings.h"
#include <QFileInfo>

#include "model/AbstractVisualItem.h"

#include "MediaBrowserDialog.h"

#include <QDirModel>
#include <QCompleter>
static void GenericItemConfigDialog_setupGenericDirectoryCompleter(QLineEdit*lineEdit)
{
	QCompleter *completer = new QCompleter(lineEdit);
	QDirModel *dirModel = new QDirModel(completer);
	completer->setModel(dirModel);
	//completer->setMaxVisibleItems(10);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setWrapAround(true);
	lineEdit->setCompleter(completer);
}


static void setupColorPicker(QtColorPicker*p)
{
	p->setStandardColors();
}

	
class GenericItemConfigDialog : public QDialog
{
public:
	GenericItemConfigDialog(GenericItemConfig *configWidget, QWidget *parent) 
		: QDialog(parent)
	{
		QVBoxLayout *layout = new QVBoxLayout(this);
		layout->setContentsMargins(0,0,0,0);
		layout->addWidget(configWidget);
		/*
		QHBoxLayout *hbox = new QHBoxLayout();
		QPushButton *btn = new QPushButton("Close");
		connect(btn,SIGNAL(clicked()), this, SLOT(close()));
		hbox->addWidget(btn);
		
// 		QShortcut * shortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
// 		connect(shortcut, SIGNAL(activated()), this, SLOT(close()));
		
		layout->addLayout(hbox);*/
	}
};

QDialog * GenericItemConfig::toDialog(QWidget *parent)
{
	GenericItemConfigDialog * d = new GenericItemConfigDialog(this,parent);
	//d->setAttribute(Qt::WA_DeleteOnClose);
	return dynamic_cast<QDialog*>(d);
}


GenericItemConfig::GenericItemConfig(AbstractContent * content, QWidget *parent) :
	QWidget(parent)
	, m_commonUi(new Ui::GenericItemConfig())
	, m_content(content)
	, m_closeButton(0)
	, m_okButton(0)
	//, m_frame(FrameFactory::defaultPanelFrame())
	, m_updateShadowDistanceAndDirection_changing(false)
	, m_inItemChangedSlot(false)
{
	m_commonUi->setupUi(this);
	
	AbstractVisualItem * model = m_content->modelItem();
	
	connect(model, SIGNAL(itemChanged(QString, QVariant, QVariant)), this, SLOT(itemChanged(QString, QVariant, QVariant)));
	
	// read other properties
	if(m_content->mirrorEnabled())
		m_commonUi->reflectionEnabled->setChecked(true);
	else
		m_commonUi->reflectionNone->setChecked(true);
	
	m_commonUi->mirrorOffset->setValue(model->mirrorOffset());
	
	m_origOpacity = model->opacity();
	m_commonUi->opacityBox->setValue((int)( m_origOpacity * 100 ));
	
	m_origPos = model->pos();
	m_commonUi->locationX->setValue(m_origPos.x());
	m_commonUi->locationY->setValue(m_origPos.y());
	
	m_origSize = model->contentsRect().size();
	m_commonUi->contentWidth->setValue((double)m_origSize.width());
	m_commonUi->contentHeight->setValue((double)m_origSize.height());
	
	QtColorPicker * bgColorPicker = new QtColorPicker;
	setupColorPicker(bgColorPicker);
	bgColorPicker->setCurrentColor(model->fillBrush().color());
	connect(bgColorPicker, SIGNAL(colorChanged(const QColor &)), this, SLOT(setBgColor(const QColor &)));
	m_commonUi->bgColorPickerLayout->addWidget(bgColorPicker);
	
	QBoxLayout *lineColorLayout = new QHBoxLayout;
	QtColorPicker * lineColorPicker = new QtColorPicker;
	setupColorPicker(lineColorPicker);
	lineColorPicker->setCurrentColor(model->outlinePen().color());
	connect(lineColorPicker, SIGNAL(colorChanged(const QColor &)), this, SLOT(setLineColor(const QColor &)));
	lineColorLayout->addWidget(lineColorPicker);
	m_commonUi->outlineColorPlaceholder->setLayout(lineColorLayout);
	
	QBoxLayout *shadowColorLayout = new QHBoxLayout;
	m_shadowColorPicker = new QtColorPicker;
	setupColorPicker(m_shadowColorPicker);
	m_shadowColorPicker->setCurrentColor(model->shadowBrush().color());
	connect(m_shadowColorPicker, SIGNAL(colorChanged(const QColor &)), this, SLOT(setShadowColor(const QColor &)));
	shadowColorLayout->addWidget(m_shadowColorPicker);
	m_commonUi->shadowColorGb->setLayout(shadowColorLayout);
	
	if(model->shadowEnabled())
		m_commonUi->shadowSolid->setChecked(true);
		
	
	connect(m_commonUi->blurBox, SIGNAL(valueChanged(int)), this, SLOT(setShadowBlur(int)));
	m_commonUi->blurBox->setValue(model->shadowBlurRadius());
	
	m_commonUi->endActionCombo->setCurrentIndex((int)model->videoEndAction());
	connect(m_commonUi->endActionCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(videoEndActionChanged(int)));
	
	
	
	AbstractVisualItem::FillType t = model->fillType();
	QAbstractButton *b = 0;
	switch(t)
	{
		case AbstractVisualItem::None: 		b = m_commonUi->bgNone;		break;
		case AbstractVisualItem::Solid:		b = m_commonUi->bgColor;	break;
		case AbstractVisualItem::Gradient:	b = m_commonUi->bgGradient;	break;
		case AbstractVisualItem::Image:		b = m_commonUi->bgImage;	break;
		case AbstractVisualItem::Video:		b = m_commonUi->bgVideo; 	break;
		default: break;
	}
	if(b)
		b->setChecked(true);
	
	m_commonUi->videoFilenameBox->setText(model->fillVideoFile());
	m_commonUi->imageFilenameBox->setText(model->fillImageFile());
	
	
	if(model->outlineEnabled())
	{
		m_commonUi->outlineSolid->setChecked(true);
	}
	else
	{
		m_commonUi->outlineNone->setChecked(true);
	}
	
	m_commonUi->outlineBox->setValue(model->outlinePen().widthF());
	
	m_commonUi->shadowXOffsetBox->setValue(model->shadowOffsetX());
	m_commonUi->shadowYOffsetBox->setValue(model->shadowOffsetY());
	
	// foce UI update by toggeling state
	m_commonUi->zoomEnabled->setChecked(!model->zoomEffectEnabled());
	m_commonUi->zoomEnabled->setChecked(model->zoomEffectEnabled());
	if(model->zoomAnchorPoint() == AbstractVisualItem::ZoomCenter)
		m_commonUi->anchorCenter->setChecked(true);
	else
	if(model->zoomAnchorPoint() == AbstractVisualItem::ZoomAnchorRandom)
		m_commonUi->anchorRandom->setChecked(true);
	else
	{
		m_commonUi->anchorOther->setChecked(true);
		
		int value = (int)model->zoomAnchorPoint();
		int idx = value - 7;
		if(idx < 0)
			idx = idx + 9 - 1;
		
		qDebug() << "combo init: "<<value<<"="<<idx;
		m_commonUi->zoomAnchorCombo->setCurrentIndex(idx);
	}
		
	if(model->zoomDirection() == AbstractVisualItem::ZoomIn)
		m_commonUi->zoomDirIn->setChecked(true);
	else
	if(model->zoomDirection() == AbstractVisualItem::ZoomOut)
		m_commonUi->zoomDirOut->setChecked(true);
	else
		m_commonUi->zoomDirRandom->setChecked(true);
		
	m_commonUi->zoomSpeedBox->setValue(model->zoomSpeed());
	m_commonUi->zoomLoop->setChecked(model->zoomLoop());
	m_commonUi->zoomFactor->setValue(model->zoomFactor());
	
	
	//updateShadowDistanceAndDirection();
	m_commonUi->shadowDirectionBox->setVisible(false);
	
	
	
	connect(m_commonUi->zoomFactor, SIGNAL(valueChanged(double)), this, SLOT(zoomFactorChanged(double)));
	connect(m_commonUi->zoomLoop, SIGNAL(toggled(bool)), this, SLOT(zoomLoopChanged(bool)));
	connect(m_commonUi->zoomEnabled, SIGNAL(toggled(bool)), this, SLOT(zoomEffectEnabled(bool)));
	connect(m_commonUi->anchorCenter, SIGNAL(toggled(bool)), this, SLOT(setZoomAnchorCenter(bool)));
	connect(m_commonUi->anchorRandom, SIGNAL(toggled(bool)), this, SLOT(setZoomAnchorRandom(bool)));
	connect(m_commonUi->zoomAnchorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setZoomAnchorPoint(int)));
	
	connect(m_commonUi->zoomDirIn, SIGNAL(clicked()), this, SLOT(updateZoomDirection()));
	connect(m_commonUi->zoomDirOut, SIGNAL(clicked()), this, SLOT(updateZoomDirection()));
	connect(m_commonUi->zoomDirRandom, SIGNAL(clicked()), this, SLOT(updateZoomDirection()));
	
	connect(m_commonUi->zoomSpeedBox, SIGNAL(valueChanged(int)), this, SLOT(zoomSpeedChanged(int)));
	
	
	connect(m_commonUi->directionBox, SIGNAL(valueChanged(int)), this, SLOT(shadowDirectionBoxChanged(int)));
 	connect(m_commonUi->distanceBox, SIGNAL(valueChanged(int)), this, SLOT(shadowDistanceBoxChanged(int)));
	
	
	
	connect(m_commonUi->front, SIGNAL(clicked()), m_content, SLOT(slotStackFront()));
	connect(m_commonUi->raise, SIGNAL(clicked()), m_content, SLOT(slotStackRaise()));
	connect(m_commonUi->lower, SIGNAL(clicked()), m_content, SLOT(slotStackLower()));
	connect(m_commonUi->back, SIGNAL(clicked()), m_content, SLOT(slotStackBack()));
	//connect(m_commonUi->background, SIGNAL(clicked()), m_content, SIGNAL(backgroundMe()));
	connect(m_commonUi->save, SIGNAL(clicked()), m_content, SLOT(slotSaveAs()));
	connect(m_commonUi->del, SIGNAL(clicked()), m_content, SIGNAL(deleteItem()), Qt::QueuedConnection);
	// autoconnection doesn't work because we don't do ->setupUi(this), so here we connect manually
// 	connect(m_commonUi->applyLooks, SIGNAL(clicked()), this, SLOT(on_applyLooks_clicked()));
// 	connect(m_commonUi->newFrame, SIGNAL(clicked()), this, SLOT(on_newFrame_clicked()));
// 	connect(m_commonUi->listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(on_listWidget_itemSelectionChanged()));
	connect(m_commonUi->reflectionEnabled, SIGNAL(toggled(bool)), this, SLOT(slotMirrorOn(bool)));
	//connect(m_commonUi->reflectionNone, SIGNAL(toggled(bool)), this, SLOT(slotMirrorOff(bool)));
	connect(m_commonUi->btnSave, SIGNAL(clicked()), this, SLOT(slotOkClicked()));
	connect(m_commonUi->btnSave, SIGNAL(clicked()), this, SLOT(slotClosed()));
// 	connect(m_commonUi->buttonBox, SIGNAL(accepted()), this, SLOT(slotClosed()));
// 	connect(m_commonUi->buttonBox, SIGNAL(rejected()), this, SLOT(slotClosed()));
	connect(m_commonUi->opacityBox, SIGNAL(valueChanged(int)), this, SLOT(slotOpacityChanged(int)));
	
	connect(m_commonUi->sizeReset,     SIGNAL(clicked()), this, SLOT(slotResetSize()));
	connect(m_commonUi->locationReset, SIGNAL(clicked()), this, SLOT(slotResetLocation()));
	connect(m_commonUi->opacityReset,  SIGNAL(clicked()), this, SLOT(slotResetOpacity()));
	
	connect(m_commonUi->videoBrowseButton,  SIGNAL(clicked()), this, SLOT(slotVideoBrowse()));
	connect(m_commonUi->imageBrowseButton,  SIGNAL(clicked()), this, SLOT(slotImageBrowse()));
	
	GenericItemConfigDialog_setupGenericDirectoryCompleter(m_commonUi->videoFilenameBox);
	GenericItemConfigDialog_setupGenericDirectoryCompleter(m_commonUi->imageFilenameBox);
	
	
	
	connect(m_commonUi->locationX, SIGNAL(valueChanged(double)), this, SLOT(slotLocationChanged(double)));
	connect(m_commonUi->locationY, SIGNAL(valueChanged(double)), this, SLOT(slotLocationChanged(double)));
	
	connect(m_commonUi->contentWidth, SIGNAL(valueChanged(double)), this, SLOT(slotSizeChanged(double)));
	connect(m_commonUi->contentHeight, SIGNAL(valueChanged(double)), this, SLOT(slotSizeChanged(double)));
	
	connect(m_commonUi->shadowXOffsetBox, SIGNAL(valueChanged(double)), this, SLOT(slotShadowXOffsetChanged(double)));
	connect(m_commonUi->shadowYOffsetBox, SIGNAL(valueChanged(double)), this, SLOT(slotShadowYOffsetChanged(double)));
	connect(m_commonUi->outlineBox, SIGNAL(valueChanged(double)), this, SLOT(slotLineSizeChanged(double)));
	connect(m_commonUi->mirrorOffset, SIGNAL(valueChanged(double)), this, SLOT(slotMirrorOffsetChanged(double)));
	
	
	connect(m_commonUi->outlineSolid, SIGNAL(toggled(bool)), this, SLOT(slotOutlineEnabled(bool)));
	connect(m_commonUi->shadowSolid, SIGNAL(toggled(bool)), this, SLOT(slotShadowEnabled(bool)));
	
	connect(m_commonUi->bgNone, SIGNAL(toggled(bool)), this, SLOT(slotBgNone(bool)));
	connect(m_commonUi->bgColor, SIGNAL(toggled(bool)), this, SLOT(slotBgColor(bool)));
	connect(m_commonUi->bgGradient, SIGNAL(toggled(bool)), this, SLOT(slotBgGradient(bool)));
	connect(m_commonUi->bgImage, SIGNAL(toggled(bool)), this, SLOT(slotBgImage(bool)));
	connect(m_commonUi->bgVideo, SIGNAL(toggled(bool)), this, SLOT(slotBgVideo(bool)));
	
	connect(m_commonUi->imageFilenameBox, SIGNAL(textChanged(const QString&)), this, SLOT(slotImageFileChanged(const QString& )));
	connect(m_commonUi->videoFilenameBox, SIGNAL(textChanged(const QString&)), this, SLOT(slotVideoFileChanged(const QString& )));
	
	connect(m_commonUi->shadowOffsetPreset0, SIGNAL(clicked()), this, SLOT(shadowOffsetPreset0()));
	connect(m_commonUi->shadowOffsetPresetB, SIGNAL(clicked()), this, SLOT(shadowOffsetPresetB()));
	connect(m_commonUi->shadowOffsetPresetBL, SIGNAL(clicked()), this, SLOT(shadowOffsetPresetBL()));
	connect(m_commonUi->shadowOffsetPresetBR, SIGNAL(clicked()), this, SLOT(shadowOffsetPresetBR()));
	connect(m_commonUi->shadowOffsetPresetL, SIGNAL(clicked()), this, SLOT(shadowOffsetPresetL()));
	connect(m_commonUi->shadowOffsetPresetR, SIGNAL(clicked()), this, SLOT(shadowOffsetPresetR()));
	connect(m_commonUi->shadowOffsetPresetT, SIGNAL(clicked()), this, SLOT(shadowOffsetPresetT()));
	connect(m_commonUi->shadowOffsetPresetTL, SIGNAL(clicked()), this, SLOT(shadowOffsetPresetTL()));
	connect(m_commonUi->shadowOffsetPresetTR, SIGNAL(clicked()), this, SLOT(shadowOffsetPresetTR()));
	
	connect(m_commonUi->alphaBox, SIGNAL(valueChanged(int)), this, SLOT(slotShadowAlphaChanged(int)));
	
	
	m_commonUi->gbMirror->setVisible(false);
	
	m_commonUi->tabWidget->setCurrentIndex(0);
	
	// hide till used in later revisions
	m_commonUi->videoPlayButton->setVisible(false);
	//m_commonUi->distanceSlider->setVisible(false);
	
	setShadowAlphaBox();
}

GenericItemConfig::~GenericItemConfig()
{
	delete m_commonUi;
}


void GenericItemConfig::slotShadowDistanceChanged(int x)
{

}

void GenericItemConfig::slotShadowAlphaChanged(int x)
{
	QColor c = m_content->modelItem()->shadowBrush().color();
	
	int alpha = (int)((100.0 - ((double)x))/100.0 * 255.0);
	c.setAlpha(alpha);
	m_shadowColorPicker->setCurrentColor(c);
	m_content->modelItem()->setShadowBrush(c);
}

void GenericItemConfig::setShadowAlphaBox()
{
	QColor c = m_content->modelItem()->shadowBrush().color();
	int alpha = 100 - (int)(((double)c.alpha())/255.0 * 100.0);
	m_commonUi->alphaBox->setValue(alpha);
}
		
QTabWidget * GenericItemConfig::tabWidget() { return m_commonUi->tabWidget; }

void GenericItemConfig::slotClosed()
{
//     qDebug() << "GenericItemConfig::slotClosed()! ";
//      if(scene())
//      {
//      	qDebug() << "GenericItemConfig::slotClosed(): mark1 ";
// 	MyGraphicsScene * desk = static_cast<MyGraphicsScene*>(scene());
// 	qDebug() << "GenericItemConfig::slotClosed(): mark2 ";
// 	desk->slotDeleteConfig(this);
// 	qDebug() << "GenericItemConfig::slotClosed(): mark3 ";
//      }
//      else
//      {
     	//qDebug() << "GenericItemConfig::slotClosed(): mark4 ";
     	if(parentWidget() && dynamic_cast<QDockWidget*>(parentWidget()))
     	{
     		//qDebug() << "GenericItemConfig::slotClosed(): mark5 ";
     		return;
     	}
     	else
     	{
     		//qDebug() << "GenericItemConfig::slotClosed(): mark6 ";
		close();
		deleteLater();
	}
 //    }
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
	QWidget::changeEvent(e);
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
	m_content->modelItem()->setOpacity((double)o / 100);
}

void GenericItemConfig::slotResetOpacity()
{
	m_content->modelItem()->setOpacity(m_origOpacity);
	m_commonUi->opacityBox->setValue((int)(m_origOpacity * 100));
	m_commonUi->opacityBox->selectAll();
}

void GenericItemConfig::slotResetLocation()
{
	m_content->modelItem()->setPos(m_origPos);
	m_commonUi->locationX->setValue(m_origPos.x());
	m_commonUi->locationY->setValue(m_origPos.y());
	m_commonUi->locationX->selectAll();
	m_commonUi->locationY->selectAll();
}

void GenericItemConfig::slotResetSize()
{
	QRectF cr = m_content->modelItem()->contentsRect();
	cr.setSize(m_origSize);
	m_content->modelItem()->setContentsRect(cr);
	
	m_commonUi->contentWidth->setValue((double)m_origSize.width());
	m_commonUi->contentHeight->setValue((double)m_origSize.height());
	m_commonUi->contentWidth->selectAll();
	m_commonUi->contentHeight->selectAll();
}


	
void GenericItemConfig::slotSizeChanged(double)
{
	if(m_inItemChangedSlot)
		return;
		
	QSize s;
	s.setWidth((int)m_commonUi->contentWidth->value());
	s.setHeight((int)m_commonUi->contentHeight->value());
	
	QRectF cr = m_content->modelItem()->contentsRect();
	cr.setSize(s);//m_origSize);
	m_content->modelItem()->setContentsRect(cr);
}


void GenericItemConfig::slotLocationChanged(double)
{
	if(m_inItemChangedSlot)
		return;
		
	QPointF p;
	p.setX(m_commonUi->locationX->value());
	p.setY(m_commonUi->locationY->value());
	m_content->modelItem()->setPos(p);
	
}

void GenericItemConfig::itemChanged(QString fieldName, QVariant value, QVariant oldValue)
{
	m_inItemChangedSlot = true;
	/*
	if(fieldName == "pos")
	{
		QPointF pos = m_content->modelItem()->pos();
		m_commonUi->locationX->setValue(pos.x());
		m_commonUi->locationY->setValue(pos.y());
	}
	else
	if(fieldName == "contentsRect")
	{
		QRectF rect = m_content->modelItem()->contentsRect();
		m_commonUi->contentHeight->setValue(rect.height());
		m_commonUi->contentWidth->setValue(rect.width());
	}
	*/
	m_inItemChangedSlot = false;
}

void GenericItemConfig::setShadowColor(const QColor & c)
{
	QBrush b = m_content->modelItem()->shadowBrush();
	b.setColor(c);
	m_content->modelItem()->setShadowBrush(b);
}

void GenericItemConfig::setShadowBlur(int x)
{
	m_content->modelItem()->setShadowBlurRadius(x);
}

void GenericItemConfig::setLineColor(const QColor & c)
{
	QPen p = m_content->modelItem()->outlinePen();
	p.setColor(c);
	m_content->modelItem()->setOutlinePen(p);
}

void GenericItemConfig::setBgColor(const QColor & c)
{
	QBrush b = m_content->modelItem()->fillBrush();
	b.setColor(c);
	m_content->modelItem()->setFillBrush(b);
}

void GenericItemConfig::slotVideoBrowse()
{
	if(1)
	{
		QString text = m_commonUi->videoFilenameBox->text();
		if(text.trimmed().isEmpty())
		{
			text = AppSettings::previousPath("videos");
		}
	
		QString fileName = QFileDialog::getOpenFileName(this, tr("Select Video"), m_commonUi->videoFilenameBox->text(), tr("Video Files (*.wmv *.mpeg *.mpg *.avi *.wmv *.flv *.mov *.mp4 *.m4a *.3gp *.3g2 *.mj2 *.mjpeg *.ipod *.m4v *.gsm *.swf *.dv *.dvd *.asf *.mtv *.roq *.aac *.ac3 *.aiff *.alaw *.iif);;Any File (*.*)"));
		if(fileName != "")
		{
			slotVideoFileChanged(fileName);
			QString abs = fileName.startsWith("http://") ? fileName : QFileInfo(fileName).absolutePath();
			m_commonUi->videoFilenameBox->setText(abs);
			AppSettings::setPreviousPath("videos",abs);
		}
	}
	else
	{
		QString text = m_commonUi->videoFilenameBox->text();
		
		MediaBrowserDialog d("videos",this);
		if(!text.trimmed().isEmpty())
			d.browser()->setDirectory(text);
		
		d.setWindowTitle("Select Video");
		connect(&d, SIGNAL(fileDoubleClicked(const QFileInfo&)), this, SLOT(slotVideoDblClicked(const QFileInfo&)));
		
		if(d.exec())
			slotVideoFileChanged(d.selectedFile().absoluteFilePath());
			
		disconnect(&d, 0, this, 0);
	}

}


void GenericItemConfig::slotVideoDblClicked(const QFileInfo&info)
{
	QString abs = info.absoluteFilePath();
	slotVideoFileChanged(abs);
	m_commonUi->videoFilenameBox->setText(abs);
}

void GenericItemConfig::slotVideoFileChanged(const QString& fileName)
{
	QDir current = QDir::current();
	QString relative = fileName.startsWith("http://") ? fileName : current.absoluteFilePath(fileName);
	//m_commonUi->videoFilenameBox->setText(relative);
	m_commonUi->bgVideo->setChecked(true);
	m_content->modelItem()->setFillVideoFile(relative);
	m_content->modelItem()->setFillType(AbstractVisualItem::Video);
}

void GenericItemConfig::slotImageBrowse()
{
	if(1)
	{
		QString text = m_commonUi->imageFilenameBox->text();
		if(text.trimmed().isEmpty())
		{
			text = AppSettings::previousPath("images");
		}
	
		QString fileName = QFileDialog::getOpenFileName(this, tr("Select Image"), text, tr("Image Files (*.png *.jpg *.bmp *.svg *.xpm *.gif);;Any File (*.*)"));
		if(fileName != "")
		{
			slotImageFileChanged(fileName);
			AppSettings::setPreviousPath("images",QFileInfo(fileName).absolutePath());
		}
	}
	else
	{
		QString text = m_commonUi->imageFilenameBox->text();
		
		MediaBrowserDialog d("images",this);
		if(!text.trimmed().isEmpty())
			d.browser()->setDirectory(text);
		
		connect(&d, SIGNAL(fileDoubleClicked(const QFileInfo&)), this, SLOT(slotImageDblClicked(const QFileInfo&)));
		d.setWindowTitle("Select Image");
		if(d.exec())
			slotImageFileChanged(d.selectedFile().absoluteFilePath());
			
		disconnect(&d, 0, this, 0);
	}
}


void GenericItemConfig::slotImageDblClicked(const QFileInfo&info)
{
	slotImageFileChanged(info.absoluteFilePath());
}

void GenericItemConfig::slotImageFileChanged(const QString& fileName)
{
	QDir current = QDir::current();
	QString relative = current.absoluteFilePath(fileName);
	m_content->modelItem()->setFillType(AbstractVisualItem::Image);
	m_content->modelItem()->setFillImageFile(relative);
	m_commonUi->imageFilenameBox->setText(fileName);
	m_commonUi->bgImage->setChecked(true);
}

void GenericItemConfig::slotOutlineEnabled(bool flag)
{
	m_content->modelItem()->setOutlineEnabled(flag);
}

void GenericItemConfig::slotShadowEnabled(bool flag)
{
	m_content->modelItem()->setShadowEnabled(flag);
}

void GenericItemConfig::slotBgNone(bool)
{
	m_content->modelItem()->setFillType(AbstractVisualItem::None);
}

void GenericItemConfig::slotBgColor(bool)
{
	m_content->modelItem()->setFillType(AbstractVisualItem::Solid);
}
	
void GenericItemConfig::slotBgGradient(bool)
{
	m_content->modelItem()->setFillType(AbstractVisualItem::Gradient);
}

void GenericItemConfig::slotBgImage(bool)
{
	m_content->modelItem()->setFillType(AbstractVisualItem::Image);
}

void GenericItemConfig::slotBgVideo(bool)
{
	m_content->modelItem()->setFillType(AbstractVisualItem::Video);
}



void GenericItemConfig::slotMirrorOffsetChanged(double d)
{
	m_content->modelItem()->setMirrorOffset(d);
}

void GenericItemConfig::slotLineSizeChanged(double d)
{
	QPen p = m_content->modelItem()->outlinePen();
	p.setWidthF(d);
	m_content->modelItem()->setOutlinePen(p);
	
}

void GenericItemConfig::slotShadowXOffsetChanged(double d)
{
	m_content->modelItem()->setShadowOffsetX(d);
	//updateShadowDistanceAndDirection();
}

void GenericItemConfig::slotShadowYOffsetChanged(double d)
{
	m_content->modelItem()->setShadowOffsetY(d);
	//updateShadowDistanceAndDirection();
}

void GenericItemConfig::setShadowOffsets(double x, double y)
{
	m_commonUi->shadowXOffsetBox->setValue(x);
	m_commonUi->shadowYOffsetBox->setValue(y);
	
	m_content->modelItem()->setShadowOffsetX(x);
	m_content->modelItem()->setShadowOffsetY(y);
}

void GenericItemConfig::shadowOffsetPreset0()
{
	setShadowOffsets(0,0);	
}

void GenericItemConfig::shadowOffsetPresetB()
{
	setShadowOffsets(0,3);	
}

void GenericItemConfig::shadowOffsetPresetBL()
{
	setShadowOffsets(-3,3);	
}

void GenericItemConfig::shadowOffsetPresetBR()
{
	setShadowOffsets(3,3);	
}

void GenericItemConfig::shadowOffsetPresetL()
{
	setShadowOffsets(-3,0);	
}

void GenericItemConfig::shadowOffsetPresetR()
{
	setShadowOffsets(3,0);	
}
		
void GenericItemConfig::shadowOffsetPresetT()
{
	setShadowOffsets(0,-3);	
}

void GenericItemConfig::shadowOffsetPresetTL()
{
	setShadowOffsets(-3,-3);	
}

void GenericItemConfig::shadowOffsetPresetTR()
{
	setShadowOffsets(3,-3);	
}


void GenericItemConfig::shadowDirectionBoxChanged(int a)
{
	int r = m_commonUi->distanceBox->value();
	int x = r*cos(a);
	int y = r*sin(a);
	m_commonUi->shadowXOffsetBox->setValue(x);
	m_commonUi->shadowYOffsetBox->setValue(y);
}

void GenericItemConfig::shadowDistanceBoxChanged(int r)
{
	int a = m_commonUi->directionBox->value();
	int x = r*cos(a);
	int y = r*sin(a);
	m_commonUi->shadowXOffsetBox->setValue(x);
	m_commonUi->shadowYOffsetBox->setValue(y);
}

void GenericItemConfig::updateShadowDistanceAndDirection()
{
	if(m_updateShadowDistanceAndDirection_changing)
		return;
	m_updateShadowDistanceAndDirection_changing = true;
	
	double x = m_content->modelItem()->shadowOffsetX();
	double y = m_content->modelItem()->shadowOffsetY();
	double r = sqrt(x*x+y*y);
	double a = atan(x/y);
	
	m_commonUi->directionBox->setValue((int)a);
	m_commonUi->distanceBox->setValue((int)r);
	
	m_updateShadowDistanceAndDirection_changing = false;
}

void GenericItemConfig::zoomEffectEnabled(bool flag)
{
	m_content->modelItem()->setZoomEffectEnabled(flag);
}

void GenericItemConfig::setZoomAnchorCenter(bool flag)
{
	if(flag)
		m_content->modelItem()->setZoomAnchorPoint(AbstractVisualItem::ZoomCenter);
}

void GenericItemConfig::setZoomAnchorRandom(bool flag)
{
	if(flag)
		m_content->modelItem()->setZoomAnchorPoint(AbstractVisualItem::ZoomAnchorRandom);
}

void GenericItemConfig::setZoomAnchorPoint(int idx)
{
// 	0 bottom mid = 7
// 	1 bottom left  = 8
// 	2 left mid = 9
// 	
// 	3 top left = 2
// 	4 top mid = 3
// 	5 top right = 4
// 	6 right mid = 5
// 	7 bottom right = 6
// 	
// 	6 = 4
// 	and
// 	2 = 9
// 	
	if(idx < 0)
		idx = 0;
		
	// wrap around to match the enum
	int value = idx + 7;
	if(value > 9)
		value = value - 9 + 1;
		
	qDebug() << "GenericItemConfig::setZoomAnchorPoint("<<idx<<"): "<<value;
	
	m_content->modelItem()->setZoomAnchorPoint((AbstractVisualItem::ZoomAnchorPoint)value);
}


void GenericItemConfig::updateZoomDirection()
{
	AbstractVisualItem * model = m_content->modelItem();
	
			
	if(m_commonUi->zoomDirIn->isChecked())
		model->setZoomDirection(AbstractVisualItem::ZoomIn);
	else
	if(m_commonUi->zoomDirOut->isChecked())
		model->setZoomDirection(AbstractVisualItem::ZoomOut);
	else
		model->setZoomDirection(AbstractVisualItem::ZoomRandom);
}

void GenericItemConfig::zoomSpeedChanged(int value)
{
	m_content->modelItem()->setZoomSpeed(value);
}

void GenericItemConfig::zoomLoopChanged(bool value)
{
	m_content->modelItem()->setZoomLoop(value);
}


void GenericItemConfig::zoomFactorChanged(double value)
{
	m_content->modelItem()->setZoomFactor(value);
}

void GenericItemConfig::videoEndActionChanged(int value)
{
	m_content->modelItem()->setVideoEndAction((AbstractVisualItem::VideoEndAction)value);
}
