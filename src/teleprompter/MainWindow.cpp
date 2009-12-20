#include "MainWindow.h"
#include ".build/ui_MainWindow.h"

#include "AppSettings.h"
#include "OutputInstance.h"
#include "SingleOutputSetupDialog.h"
#include "OutputViewer.h"
#include "model/Output.h"

#include "model/TextBoxItem.h"
#include "model/Slide.h"
#include "model/SlideGroup.h"
#include "model/ItemFactory.h"

#include <QCloseEvent>
#include <QDockWidget>
#include <QMessageBox>
#include <QApplication>
#include <QSettings>
#include <QFileDialog>

#define RECONNECT_WAIT_TIME 1000 * 2

MainWindow * MainWindow::static_mainWindow = 0;

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, m_ui(new Ui::MainWindow)
	, m_inst(0)
	, m_aspect(-1)
	, m_preview(0)
	, m_previewDock(0)
	, m_posBoxLock(false)
	, m_accelBoxLock(false)
	, m_pos(0)
	, m_setPosLock(false)
{
	static_mainWindow = this;
	m_ui->setupUi(this);

	setWindowTitle("Teleprompter - DViz");
	setWindowIcon(QIcon(":/data/icon-d.png"));

	connect(m_ui->actionOpen_Text_File, SIGNAL(triggered()), this, SLOT(slotOpen()));
	connect(m_ui->actionSave, SIGNAL(triggered()), this, SLOT(slotSave()));
	connect(m_ui->actionSave_As, SIGNAL(triggered()), this, SLOT(slotSaveAs()));
	connect(m_ui->actionSetup_Outputs, SIGNAL(triggered()), this, SLOT(slotOutputSetup()));
	connect(m_ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
	
	m_ui->playBtn->setIcon(QIcon(":/data/action-play.png"));
	connect(m_ui->playBtn, SIGNAL(clicked()), this, SLOT(slotTogglePlay()));
	
	connect(m_ui->posBox, SIGNAL(valueChanged(int)), this, SLOT(slotPosBoxChanged(int)));
	connect(m_ui->accelBox, SIGNAL(valueChanged(int)), this, SLOT(slotAccelBoxChanged(int)));
	connect(m_ui->accelResetBtn, SIGNAL(clicked()), this, SLOT(slotResetAccel()));
	m_ui->accelResetBtn->setIcon(QIcon(":/data/stock-undo.png"));
	m_ui->accelResetBtn->setToolTip("Reset to Normal Speed (CTRL+SHIFT+Z)");
	m_ui->accelResetBtn->setShortcut(QString(tr("CTRL+SHIFT+Z")));
	
	connect(&m_animTimer, SIGNAL(timeout()), this, SLOT(animate()));
	m_animTimer.setInterval(ANIMATE_BASE_MS);
	
	connect(m_ui->textEdit, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
	
	m_ui->actionSetup_Outputs->setIcon(QIcon(":data/stock-preferences.png"));

	QSettings s;
	bool flag = s.value("teleprompter/firstrun",true).toBool();
	if(flag)
	{
		s.setValue("teleprompter/firstrun",false);
		slotOutputSetup();
	}
	
	AppSettings::setGridEnabled(false);
	

	Output * output = AppSettings::outputs().first();
	m_previewDock = new QDockWidget(QString(tr("%1 Preiew")).arg(output->name()), this);
	m_previewDock->setObjectName(output->name());

	m_preview = new OutputViewer(0,m_previewDock);
	m_previewDock->setWidget(m_preview);
	addDockWidget(Qt::BottomDockWidgetArea, m_previewDock);
	
	m_group = new SlideGroup();
	m_group->setGroupTitle("Group"); //AbstractItem::guessTitle(QFileInfo(fileName).baseName()));
		
	m_slide = new Slide();
	
	m_textbox = new TextBoxItem();
	m_textbox->setItemId(ItemFactory::nextId());
	m_textbox->setItemName(QString("TextBoxItem%1").arg(m_textbox->itemId()));



	AbstractVisualItem * bg = dynamic_cast<AbstractVisualItem*>(m_slide->background());

// 	qDebug() << "Slide "<<slideNum<<": [\n"<<tmpList.join("\n")<<"\n]";;

	bg->setFillType(AbstractVisualItem::Solid);
	bg->setFillBrush(Qt::black);


	// Outline pen for the text
	QPen pen = QPen(Qt::black,1.5);
	pen.setJoinStyle(Qt::MiterJoin);

	m_textbox->setPos(QPointF(0,0));
	m_textbox->setOutlinePen(pen);
	m_textbox->setOutlineEnabled(false);
	m_textbox->setFillBrush(Qt::white);
	m_textbox->setFillType(AbstractVisualItem::Solid);
	m_textbox->setShadowEnabled(false);
// 	tmpText->setShadowBlurRadius(6);

	m_slide->addItem(m_textbox);
	
	m_slide->setSlideNumber(0);
	m_group->addSlide(m_slide);
	
	openOutput();
	
	m_inst->setSlideGroup(m_group);
	
	openTextFile("test.txt");
	
}

void MainWindow::slotResetAccel()
{
	m_ui->accelBox->setValue(0);
}

void MainWindow::slotOpen()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Text File"),
		AppSettings::previousPath("text"),
		tr("Text Files (*.txt);;Any File (*.*)")
	);
	
	if(!fileName.isEmpty())
	{
// 		int MinTextSize = 48;
		openTextFile(fileName);
		
		/*slotTextChanged();*/		
	}	
}

void MainWindow::openTextFile(const QString& fileName)
{
	m_filename = fileName;

	AppSettings::setPreviousPath("text",fileName);

	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(this,tr("Can't Read File"),QString(tr("Unable to open %1")).arg(fileName));
		return;
	}

	QStringList lines;
	QTextStream stream(&file);
	while( ! stream.atEnd() )
		lines << stream.readLine();
		
	m_ui->textEdit->setPlainText(lines.join("\n"));
}

void MainWindow::slotSave()
{
	if(m_filename.isEmpty())
	{
		slotSaveAs();
	}
	else
	{
		saveTextFile(m_filename);
	}
}

void MainWindow::saveTextFile(const QString& filename)
{
	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::warning(this,tr("Can't Write File"),QString(tr("Unable to open %1")).arg(filename));
		return;
	}

	QStringList lines;
	QTextStream stream(&file);
	QString txt = m_ui->textEdit->toPlainText();
	stream << txt;
	file.close();
}

void MainWindow::slotSaveAs()
{
	QString curFile = m_filename;
	if(curFile.trimmed().isEmpty())
		curFile = AppSettings::previousPath("text");

	QString fileName = QFileDialog::getSaveFileName(this, tr("Choose a Filename"), curFile, tr("Text Files (*.txt);;Any File (*.*)"));
	if(fileName != "")
	{
		QFileInfo info(fileName);
		//if(info.suffix().isEmpty())
			//fileName += ".dviz";
		AppSettings::setPreviousPath("text",fileName);
		saveTextFile(fileName);
		return;
	}

// 	return;
}

void MainWindow::slotTextChanged()
{
	m_textbox->setText(m_ui->textEdit->toHtml());
	
	
// 	QString blockPrefix = "<span style='font-family:Calibri,Tahoma,Arial,Sans-Serif;font-weight:800'><b>";
// 	QString blockSuffix = "</b></span>";
// 	
// 		
// 	tmpText->setText(QString("%1%2%3")
// 					    .arg(blockPrefix)
// 					    .arg(tmpList.join("\n"))
// 					    .arg(blockSuffix));

	int MinTextSize = 52;
	QSize fitSize = standardSceneRect().size();
	int realHeight = m_textbox->fitToSize(fitSize,MinTextSize);
	
	//// Center text on screen
	//qreal y = fitSize.height()/2 - realHeight/2;
	m_textbox->setContentsRect(QRectF(0,0,fitSize.width(),realHeight));
	
	m_incOrig = m_inc = ANIMATE_BASE_PX / (double)realHeight;
	qDebug() << "slotTextChanged(): fitsize:"<<fitSize<<", realHeight: "<<realHeight<<", rect: "<<m_textbox->contentsRect()<<", m_inc:"<<m_inc;
	
		
}

void MainWindow::slotTogglePlay()
{
	if(!m_animTimer.isActive())
	{
		m_animTimer.start();
		m_ui->playBtn->setIcon(QIcon(":/data/action-pause.png"));
		m_ui->playBtn->setText("&Pause");	
	}
	else
	{
		m_animTimer.stop();
		m_ui->playBtn->setIcon(QIcon(":/data/action-play.png"));
		m_ui->playBtn->setText("&Play");
	}
}

void MainWindow::animate()
{
	m_pos += m_inc;
	
	setPos(m_pos);
	
	m_setPosLock = true;
	
	m_ui->posBox->setValue((int)m_pos);
	
	m_setPosLock = false;
}

void MainWindow::slotPosBoxChanged(int x)
{
	if(m_posBoxLock)
		return;
	m_posBoxLock = true;
	
	setPos((double)x);
	
	m_posBoxLock = false;
}

void MainWindow::setPos(double x)
{
	if(m_setPosLock)
		return;

	m_pos = x;
	m_ui->posBox->setValue(x);
	
	QRectF rect = m_textbox->contentsRect();
	QPointF pos = m_textbox->pos();
	
	double p = (double)x/100.0;
	
	
	QSize fitSize = standardSceneRect().size();
	
	double newTop = -1 * (rect.height() * p);
	
	qDebug() << "setPos(): x:"<<x<<", p:"<<p<<", newTop:"<<newTop<<", rect:"<<rect<<", m_inc:"<<m_inc; 
	
	
	m_textbox->setPos(QPointF(pos.x(),newTop));
}

#ifndef qabs
# define qabs(a) (a<0?-1*a:a)
#endif

void MainWindow::slotAccelBoxChanged(int x)
{
	if(m_accelBoxLock)
		return;
	m_accelBoxLock = true;
	
	double p = (double)x / 100.0;
	
	int sign = p < 0 ? -1:1;
	p = qabs(p);
	
	m_inc = m_incOrig * (1+p * 1.25);
	if(sign < 0)
		m_inc = qabs(m_inc)*-1;
	else
		m_inc = qabs(m_inc);
	
	if(p>1)
		p=1;
	int ms = ANIMATE_BASE_MS * (1-p) + 1;
	
		
	qDebug() << "slotAccelBoxChanged(): p:"<<p<<", ms:"<<ms;
	
	m_animTimer.setInterval(ms);
	
	m_accelBoxLock = false;
}

void MainWindow::openOutput()
{
	if(m_inst)
	{
		m_inst->setVisible(true);
		return;
	}
		
	//qDebug() << "MainWindow::openOutput()";
	//if(m_inst)
	//	delete m_inst;

	QList<Output*> outputs = AppSettings::outputs();

	Output * out = outputs.first();
	m_inst = new OutputInstance(out); // start hidden
	// not going to use this signal since it should be handeled from the server
	//connect(inst, SIGNAL(nextGroup()), this, SLOT(nextGroup()));

	// start hidden until connected
	//m_inst->setVisible(false);
	//m_previewDock->setVisible(false);
	m_preview->setOutputInstance(m_inst);
}

void MainWindow::aspectChanged(double d)
{
	qDebug() << "MainWindow::aspectChanged(): "<<d;
	m_aspect = d;
	emit aspectRatioChanged(d);
}

void MainWindow::slotOutputSetup()
{
	SingleOutputSetupDialog d(this);
	if(d.exec())
		emit appSettingsChanged();
}

void MainWindow::slotExit()
{
	qApp->quit();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
// 	slotDisconnect();
	if(m_inst)
		m_inst->close();
}


MainWindow::~MainWindow()
{
	delete m_inst;
	m_inst = 0;
	delete m_ui;
	
	delete m_slide;
}

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type())
	{
		case QEvent::LanguageChange:
			m_ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void MainWindow::setLiveGroup(SlideGroup *newGroup, Slide *currentSlide,bool)
{
	(void)0;
}

QRect MainWindow::standardSceneRect(double aspectRatio)
{
	if(aspectRatio < 0)
	{
		aspectRatio = AppSettings::liveAspectRatio();
	}

	int height = 768;
	return QRect(0,0,aspectRatio * height,height);
}

void MainWindow::setAutosaveEnabled(bool)
{
    (void)0;
}
