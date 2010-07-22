#include "MdiMjpeg.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QUrl>
#include <QMessageBox>

MdiMjpeg::MdiMjpeg(QWidget *parent)
	: MdiVideoSource(parent)
{
	m_thread = new MjpegThread();
	setVideoSource(m_thread);
	
	//qDebug() << "MdiMjpeg::ctor()";
	//setupDefaultGui();
	
	QHBoxLayout *layout = new QHBoxLayout();
	
	m_urlInput = new QLineEdit();
	connect(m_urlInput, SIGNAL(returnPressed()), this, SLOT(urlReturnPressed()));
		
	layout->addWidget(new QLabel("URL:"));
	layout->addWidget(m_urlInput);
	m_layout->addLayout(layout);
	
	setWindowTitle("MJPEG");	
}

// void MdiMjpeg::setupGui()
// {
// 	qDebug() << "MdiMjpeg::setupGui()";
// 	setupDefaultGui();
// 	
// 	QHBoxLayout *layout = new QHBoxLayout();
// 	
// 	m_urlInput = new QLineEdit();
// 	connect(m_urlInput, SIGNAL(returnPressed()), this, SLOT(urlReturnPressed()));
// 		
// 	layout->addWidget(new QLabel("URL:"));
// 	layout->addWidget(m_urlInput);
// 	m_layout->addLayout(layout);
// 	
// }

void MdiMjpeg::urlReturnPressed()
{
	qDebug() << "MdiMjpeg::urlReturnPressed(): "<<m_urlInput->text(); 
	
	setWindowTitle(m_urlInput->text());
	QUrl url(m_urlInput->text());
	
	if(!url.isValid())
	{
		QMessageBox::critical(this, "Invalid URL","Sorry, the URL you entered is not a properly-formatted URL. Please try again.");
		return;
	}
	
	if(!m_thread->connectTo(url.host(), url.port(), url.path(), url.userName(), url.password()))
	{
		QMessageBox::critical(this, "Connection Problem","Sorry, could not connect to the URL given!");
		return;
	}
}
