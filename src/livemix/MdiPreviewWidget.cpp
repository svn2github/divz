#include "MdiPreviewWidget.h"

#include <QMessageBox>
#include <QDesktopWidget>

//#include "VideoOutputWidget.h"
//#include "../qq06-glthread/GLWidget.h"

#include <QApplication>
#include <QSettings>

#include "VideoWidget.h"

MdiPreviewWidget::MdiPreviewWidget(QWidget *parent)
	: MdiVideoChild(parent)
	, m_textInput(new QLineEdit(this))
{
	m_outputWidget = new VideoWidget();
	m_outputWidget->setFadeLength(2000);
		
	m_layout->addWidget(m_textInput);
	connect(m_textInput, SIGNAL(returnPressed()), this, SLOT(textReturnPressed()));

	
/*	m_outputWidget = new GLWidget(0);
	m_outputWidget->show();
	m_outputWidget->startRendering();*/
	
// 	GLWidget *widget = new GLWidget(0);
// 	widget->setWindowTitle("Thread0");
// 	widget->show();
//  	widget->startRendering();
	
	
// 	m_outputWidget->setVisible(true);
// 	m_outputWidget->applyGeometry(QRect(0,0,320,240));


	m_screenBox = new QComboBox(this);
	m_layout->addWidget(m_screenBox);

	QDesktopWidget *desktopWidget = QApplication::desktop();
	
	QStringList items;
	
	items << "No Output";
	m_screenList << QRect(0,0,320,240);
	
	int ourScreen = desktopWidget->screenNumber(this);
			
	int numScreens = desktopWidget->numScreens();
	for(int screenNum = 0; screenNum < numScreens; screenNum ++)
	{
// 		if(screenNum != ourScreen)
		{
			QRect geom = desktopWidget->screenGeometry(screenNum);
	// 		QString geomStr = QString("%1 x %2 at (%3,%4)")
	// 					.arg(geom.width())
	// 					.arg(geom.height())
	// 					.arg(geom.left())
	// 					.arg(geom.top());
			
			items << QString("Screen %1 at (%2 x %3)")
				.arg(screenNum + 1)
				.arg(geom.left())
				.arg(geom.top());
			
			m_screenList << geom;
		}
	}
	
	m_screenBox->addItems(items);
	
	connect(m_screenBox, SIGNAL(currentIndexChanged(int)), this, SLOT(screenBoxChanged(int)));
	if(numScreens == 1)
		m_screenBox->setEnabled(false);
	else
	{
		QSettings settings;
		int idx = settings.value("mdipreviewwidget/last-screen-index",0).toInt();
		//screenBoxChanged(0);
		m_screenBox->setCurrentIndex(idx);
	}
		
	setWindowTitle("Output Preview");
	
	videoWidget()->setFps(15);
}

	
void MdiPreviewWidget::textReturnPressed()
{
	m_videoWidget->setOverlayText(m_textInput->text());
	m_outputWidget->setOverlayText(m_textInput->text());
	m_textInput->selectAll();
}


void MdiPreviewWidget::screenBoxChanged(int idx)
{
	QDesktopWidget *desktopWidget = QApplication::desktop();
	if(idx < 0 || idx >= m_screenList.size())
		return;
		
	QSettings settings;
	settings.setValue("mdipreviewwidget/last-screen-index",idx);
		
	QRect geom = m_screenList[idx];
	
	//m_outputWidget->applyGeometry(geom);
	//m_outputWidget->setVisible(true);
	
	//qDebug() << "VideoOutputWidget::applyGeometry(): rect: "<<rect;
 	//if(isFullScreen)
 		m_outputWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
//  	else
//  		m_outputWidget->setWindowFlags(Qt::FramelessWindowHint);
				
				
	m_outputWidget->resize(geom.width(),geom.height());
	m_outputWidget->move(geom.left(),geom.top());
		
	m_outputWidget->show();
	
	m_outputWidget->setWindowTitle(QString("Video Output - LiveMix")/*.arg(m_output->name())*/);
	m_outputWidget->setWindowIcon(QIcon(":/data/icon-d.png"));
}

void MdiPreviewWidget::takeSource(MdiVideoChild *child)
{
	//m_outputWidget->setOverlayText(child->videoWidget()->overlayText());
	m_outputWidget->setVideoSource(child->videoSource());
	setVideoSource(child->videoSource());
}

void MdiPreviewWidget::takeSource(VideoSource *source)
{
	m_outputWidget->setVideoSource(source);
	setVideoSource(source);
}
