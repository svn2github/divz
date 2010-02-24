#include "WebSlideGroupFactory.h"

#include "WebSlideGroup.h"

#include "NativeViewerWebKit.h"

#include "OutputInstance.h"
#include <QMessageBox>

#include <assert.h>
#include "MainWindow.h"

/** WebSlideGroupFactory:: **/

WebSlideGroupFactory::WebSlideGroupFactory() : SlideGroupFactory() {}
/*WebSlideGroupFactory::~WebSlideGroupFactory()
{
}*/
	
SlideGroup * WebSlideGroupFactory::newSlideGroup()
{
	return dynamic_cast<SlideGroup*>(new WebSlideGroup());
}

AbstractSlideGroupEditor * WebSlideGroupFactory::newEditor()
{
	// In futre: Call out to the system to launch native video editor with the file as argument?
	return 0;
}

NativeViewer * WebSlideGroupFactory::newNativeViewer()
{
	return new NativeViewerWebKit();
}

SlideGroupViewControl * WebSlideGroupFactory::newViewControl()
{
	return new WebSlideGroupViewControl();
}


/// WebSlideGroupViewControl Implementation
#include <QtGui>
#include <QtWebKit>

WebSlideGroupViewControl::WebSlideGroupViewControl(OutputInstance *inst, QWidget *parent)
    : SlideGroupViewControl(inst,parent,false)
{
	//QNetworkProxyFactory::setUseSystemConfiguration(true);
	
	QVBoxLayout * masterLayout = new QVBoxLayout(this);
	masterLayout->setContentsMargins(0,0,0,0);
	
	m_splitter = new QSplitter(this);
	m_splitter->setOrientation(Qt::Vertical);
	masterLayout->addWidget(m_splitter);
	
	
	QWidget * baseWidget = new QWidget(m_splitter);
	QVBoxLayout * layout = new QVBoxLayout(baseWidget);
	layout->setContentsMargins(0,0,0,0);
	
	m_splitter->addWidget(baseWidget);
	m_splitter->addWidget(new QWidget(m_splitter)); // blank filler

	m_view = new QWebView(baseWidget);
	//m_view->load(QUrl("http://www.google.com/"));
	
	connect(m_view, SIGNAL(loadFinished(bool)),    SLOT(adjustLocation()));
	//connect(m_view, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
	connect(m_view, SIGNAL(loadProgress(int)),     SLOT(setProgress(int)));
	connect(m_view, SIGNAL(loadFinished(bool)),    SLOT(finishLoading(bool)));
	
	m_locationEdit = new QLineEdit(this);
	m_locationEdit->setSizePolicy(QSizePolicy::Expanding, m_locationEdit->sizePolicy().verticalPolicy());
	connect(m_locationEdit, SIGNAL(returnPressed()), SLOT(changeLocation()));
	
	m_toolBar = new QToolBar(tr("Navigation"),baseWidget);
	m_toolBar->addAction(m_view->pageAction(QWebPage::Back));
	m_toolBar->addAction(m_view->pageAction(QWebPage::Forward));
	m_toolBar->addAction(m_view->pageAction(QWebPage::Reload));
	m_toolBar->addAction(m_view->pageAction(QWebPage::Stop));
	m_toolBar->addWidget(m_locationEdit);
	
	m_progress = new QProgressBar(baseWidget);
	
	QFrame * line = new QFrame();
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);
	
	layout->addWidget(m_toolBar);
	layout->addWidget(line);
	layout->addWidget(m_view);
	layout->addWidget(m_progress);
	
	m_controlBase = baseWidget;
}

WebSlideGroupViewControl::~WebSlideGroupViewControl()
{
// 	if(m_webGroup && m_webGroup->nativeViewer())
// 		m_webGroup->nativeViewer()->renderWidget()->setWidget(0);
}

void WebSlideGroupViewControl::resizeEvent(QResizeEvent*)
{
	QRect r = MainWindow::mw()->standardSceneRect();
	qreal ar = (qreal)r.height() / (qreal)r.width();
	int newWebHeight = width() * ar + m_toolBar->height() + 20; // Magic fudge
	m_splitter->setSizes(QList<int>() << newWebHeight << height() - newWebHeight);
}

void WebSlideGroupViewControl::setIsPreviewControl(bool flag)
{
	SlideGroupViewControl::setIsPreviewControl(flag);
	m_controlBase->setVisible(!flag);
	if(flag)
	    setMaximumHeight(0);
}

void WebSlideGroupViewControl::setSlideGroup(SlideGroup *g, Slide *curSlide, bool allowProgressDialog)
{
	SlideGroupViewControl::setSlideGroup(g);

	if(m_releasingSlideGroup)
		return;

	m_slideViewer->setSlideGroup(m_group);
	
	m_webGroup = dynamic_cast<WebSlideGroup*>(g);
	if(!m_webGroup)
	{
		qDebug() << "WebSlideGroupViewControl::setSlideGroup(): Unable to cast group to WebSlideGroup";
		return;
	}
	
	//m_view->load(m_webGroup->url());
	m_locationEdit->setText(m_webGroup->url().toString());
	changeLocation();

	if(!m_webGroup->nativeViewer())
	{
		//qDebug() << "WebSlideGroupViewControl::setSlideGroup(): No native viewer on video group yet, cannot init slots";
		return;
	}
	
	if(!m_isPreviewControl)
		m_webGroup->nativeViewer()->renderWidget()->setWidget(m_view);
	
}

void WebSlideGroupViewControl::adjustLocation()
{
	m_locationEdit->setText(m_view->url().toString());
	m_locationEdit->selectAll();
}

void WebSlideGroupViewControl::changeLocation()
{
	QString txt = m_locationEdit->text();
	if(txt.indexOf("://") < 0)
		txt = "http://" + txt;
	QUrl url = QUrl(txt);
	m_view->load(url);
	m_view->setFocus();
}

void WebSlideGroupViewControl::setProgress(int p)
{
	if(p < 0 || p > 100)
	{
		//m_progress->setVisible(false);
		finishLoading(true);
	}
	else
	{
		if(!m_progress->isVisible())
			m_progress->setVisible(true);
		m_progress->setValue(p);
	}
}

void WebSlideGroupViewControl::finishLoading(bool)
{
	m_progress->setVisible(false);
	
	if(m_isPreviewControl)
		return;
	
	if(!m_webGroup->nativeViewer())
	{
		//qDebug() << "WebSlideGroupViewControl::finishLoading(): no native viewer, cant save image";
		return;
	}

	QPixmap cache = m_webGroup->nativeViewer()->snapshot();
	
	QString cacheFile = m_webGroup->cacheFile();
	cache.save(cacheFile,"PNG");
	m_webGroup->changeBackground(AbstractVisualItem::Image,QVariant(cacheFile),0);
	
// 	qDebug() << "WebSlideGroupViewControl::finishLoading(): Saving image to "<<cacheFile;
}

