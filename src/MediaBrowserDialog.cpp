#include "MediaBrowserDialog.h"
#include <QDialog>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QSettings>

#include "MediaBrowser.h"

MediaBrowserDialog::MediaBrowserDialog(const QString & prevPathKey, QWidget *parent)
	: QDialog(parent)
	, m_browser(0)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	
	m_browser = new MediaBrowser();	
	QVBoxLayout * vbox = dynamic_cast<QVBoxLayout*>(m_browser->layout());
	if(vbox)
		vbox->setMargin(0);
	m_browser->setBackgroundActionsEnabled(false);
	m_browser->setViewMode(QListView::IconMode);
	
	//m_browser->setIconSize(QSize(64,64));
	
	if(!prevPathKey.isEmpty())
	{
		m_browser->setPreviousPathKey(prevPathKey);
		if(prevPathKey.toLower() == "videos")
		{
			QStringList filters;
			filters << tr("Video Files (*.wmv *.mpeg *.mpg *.avi *.wmv *.flv *.mov *.mp4 *.m4a *.3gp *.3g2 *.mj2 *.mjpeg *.ipod *.m4v *.gsm *.gif *.swf *.dv *.dvd *.asf *.mtv *.roq *.aac *.ac3 *.aiff *.alaw *.iif)");
			filters << tr("Any File (*.*)");
			m_browser->setFileTypeFilterList(filters);
		}
		else
		if(prevPathKey.toLower() == "images")
		{
			QStringList filters;
			filters << tr("Image Files (*.png *.jpg *.bmp *.svg *.xpm)");
			filters << tr("Any File (*.*)");
			m_browser->setFileTypeFilterList(filters);
		}
	}
	
	layout->addWidget(m_browser);
	
	connect(m_browser, SIGNAL(fileSelected(const QFileInfo&)), this, SLOT(fileSelected(const QFileInfo&)));
	connect(m_browser, SIGNAL(fileDoubleClicked(const QFileInfo&)), this, SLOT(slotFileDoubleClicked(const QFileInfo&)));
	
	QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	layout->addWidget(buttonBox);
	
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	
	m_settingsKey = prevPathKey.toLower();
	if(m_settingsKey.isEmpty())
		m_settingsKey = "media-browser-dialog";
	
	QSettings settings;
	QSize sz = settings.value("media-browser-dialog/size").toSize();
	if(sz.isValid())
		resize(sz);
	QPoint p = settings.value("media-browser-dialog/pos").toPoint();
	if(!p.isNull())
		move(p);
	m_browser->restoreState(settings.value("media-browser-dialog/browser").toByteArray());
	
	
	m_browser->setSplitterOrientation(Qt::Horizontal);
	
}

MediaBrowserDialog::~MediaBrowserDialog()
{
	QSettings settings;
	settings.setValue("media-browser-dialog/size",size());
	settings.setValue("media-browser-dialog/pos",pos());
	settings.setValue("media-browser-dialog/browser",m_browser->saveState());
	
	delete m_browser;
	m_browser = 0;
}

void MediaBrowserDialog::slotFileDoubleClicked(const QFileInfo &info)
{
	emit fileDoubleClicked(info);
}

MediaBrowser *MediaBrowserDialog::browser()
{
	return m_browser;
}
	
void MediaBrowserDialog::fileSelected(const QFileInfo &file)
{
	m_selectedFile = file;
}
