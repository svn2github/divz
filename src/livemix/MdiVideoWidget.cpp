
#include "MdiVideoWidget.h"

#include <QCompleter>
#include <QDirModel>
#include <QFileDialog>
#include <QPushButton>

#include "VideoThread.h"

#include <QCDEStyle>

MdiVideoWidget::MdiVideoWidget(QWidget *parent)
	: MdiVideoChild(parent)
{
	
	QHBoxLayout *layout = new QHBoxLayout();
	
	m_fileInput = new QLineEdit(this);
	connect(m_fileInput, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
		
	layout->addWidget(m_fileInput);
	
	QCompleter *completer = new QCompleter(m_fileInput);
	QDirModel *dirModel = new QDirModel(completer);
	completer->setModel(dirModel);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setWrapAround(true);
	m_fileInput->setCompleter(completer);
	
	QPushButton *btn = new QPushButton("...",this);
	connect(btn, SIGNAL(clicked()), this, SLOT(browseButtonClicked()));
	layout->addWidget(btn);
	
	btn->setStyle(new QCDEStyle());
	
	m_layout->addLayout(layout);
	
	resize(200,sizeHint().height());
	
	
}
	
void MdiVideoWidget::setVideoFile(const QString& file)
{
	if(m_videoFile == file)
		return;
		
	if(m_fileInput->text() != file)
		m_fileInput->setText(file);
	
	if(m_videoSource)
	{
		m_videoSource->quit();
		m_videoSource->deleteLater();
	}
	
	VideoThread *videoSource = new VideoThread();
	videoSource->setVideo(file);
	videoSource->start();
	//videoSource->pause();
	
	setVideoSource(videoSource);
}

void MdiVideoWidget::browseButtonClicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Video"), m_fileInput->text(), tr("Video Files (*.wmv *.mpeg *.mpg *.avi *.wmv *.flv *.mov *.mp4 *.m4a *.3gp *.3g2 *.mj2 *.mjpeg *.ipod *.m4v *.gsm *.swf *.dv *.dvd *.asf *.mtv *.roq *.aac *.ac3 *.aiff *.alaw *.iif);;Any File (*.*)"));
	if(fileName != "")
	{
		setVideoFile(fileName);
// 		QString abs = fileName.startsWith("http://") ? fileName : QFileInfo(fileName).absolutePath();
// 		m_commonUi->videoFilenameBox->setText(abs);
// 		AppSettings::setPreviousPath("videos",abs);
	}
}

void MdiVideoWidget::returnPressed()
{
	setVideoFile(m_fileInput->text());
}
