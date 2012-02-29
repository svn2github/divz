#include "SlideEditorWindow.h"
#include "MyGraphicsScene.h"
#include "RenderOpts.h"

#include <QAction>
#include <QApplication>
#include <QDir>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFile>
#include <QImageReader>
#include <QInputDialog>
#include <QMenu>
/*#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>*/
#include <QPushButton>
#include <QSettings>
#include <QTimer>
#include <QVBoxLayout>
#include <QGraphicsSimpleTextItem>
#include <QLabel>
#include <QStyleOption>
#include <QCheckBox>
#ifndef QT_NO_OPENGL
# include <QtOpenGL/QGLWidget>
#endif
#include <QDockWidget>
#include <QListView>

#include <QWheelEvent>
#include <QToolBar>
#include <QSplitter>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QTimer>
#include <QMessageBox>

#include <QDebug>
#include <assert.h>

#include <QUndoView>
#include <QUndoStack>

#include <QApplication>

#include <QGraphicsLineItem>

#include "model/ItemFactory.h"
#include "model/Slide.h"
#include "model/TextItem.h"
#include "model/BackgroundItem.h"
#include "MainWindow.h"
#include "AppSettings.h"
#include "items/TextBoxContent.h"
#include "model/Document.h"
#include "model/SlideGroupFactory.h"
#include "SlideGroupListModel.h"

#include "items/OutputViewConfig.h"
#include "items/OutputViewContent.h"

#include "items/GenericItemConfig.h"
#include "items/AbstractContent.h"
/*#include "items/PictureContent.h"
#include "items/PictureConfig.h"*/
#include "items/TextContent.h"
// #include "items/WebContentSelectorItem.h"
// #include "items/WebcamContent.h"
#include "items/TextContent.h"
#include "items/TextBoxConfig.h"
#include "items/TextBoxContent.h"
#include "items/VideoFileContent.h"
#include "items/VideoFileConfig.h"
#include "items/BackgroundContent.h"
#include "items/BackgroundConfig.h"
#include "items/BoxConfig.h"
#include "items/BoxContent.h"
#include "items/ImageConfig.h"
#include "items/ImageContent.h"

#include "GridDialog.h"

#include "SlideSettingsDialog.h"
#include "SlideGroupSettingsDialog.h"

#include "SlideItemListModel.h"

#define DEBUG_MODE 0


#include <QCommonStyle>
class RubberBandStyle : public QCommonStyle
{
	public:
		void drawControl(ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0) const
		{
			if (element == CE_RubberBand)
			{
				painter->save();
				QColor color = option->palette.color(QPalette::Highlight);
				painter->setPen(color);
				color.setAlpha(80); painter->setBrush(color);
				painter->drawRect(option->rect.adjusted(0,0,-1,-1));
				painter->restore();
				return;
			}
			return QCommonStyle::drawControl(element, option, painter, widget);
		}

		int styleHint(StyleHint hint, const QStyleOption * option, const QWidget * widget, QStyleHintReturn * returnData) const
		{
			if (hint == SH_RubberBand_Mask)
				return false;
			return QCommonStyle::styleHint(hint, option, widget, returnData);
		}
};

#include <QGraphicsView>
class MyGraphicsView : public QGraphicsView
{
	public:
		MyGraphicsView(QWidget * parent)
			: QGraphicsView(parent)
			, m_desk(0)
			, m_editorWindow(dynamic_cast<SlideEditorWindow*>(parent))
		{
			// customize widget
// 			setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
// 			setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
			setInteractive(true);
			setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
			setDragMode(QGraphicsView::RubberBandDrag);
			//setAcceptDrops(true);
			//setFrameStyle(QFrame::NoFrame);

			setRenderHint( QPainter::TextAntialiasing, true);
			setRenderHint( QPainter::Antialiasing, true );

			setCacheMode(QGraphicsView::CacheBackground);
			setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
			setOptimizationFlags(QGraphicsView::DontSavePainterState);
			setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);


			setTransformationAnchor(AnchorUnderMouse);
			setResizeAnchor(AnchorViewCenter);

		//             // don't autofill the view with the Base brush
		//             QPalette pal;
		//             pal.setBrush(QPalette::Base, Qt::NoBrush);
		//             setPalette(pal);

			// use own style for drawing the RubberBand (opened on the viewport)
			viewport()->setStyle(new RubberBandStyle);

			// can't activate the cache mode by default, since it inhibits dynamical background picture changing
			//setCacheMode(CacheBackground);
		}

		~MyGraphicsView() { delete viewport()->style(); }

		void setMyScene(MyGraphicsScene * desk)
		{
			setScene(desk);
			m_desk = desk;
		}


	protected:
		void resizeEvent(QResizeEvent * event)
		{
// 			if (m_desk)
// 				m_desk->resize(contentsRect().size());
			QGraphicsView::resizeEvent(event);
		}


		void keyPressEvent(QKeyEvent *event)
		{
			if(event->modifiers() & Qt::ControlModifier)
			{

				switch (event->key())
				{
					case Qt::Key_Plus:
						scaleView(qreal(1.2));
						break;
					case Qt::Key_Minus:
                                        case Qt::Key_Equal:
						scaleView(1 / qreal(1.2));
						break;
					default:
						QGraphicsView::keyPressEvent(event);
				}
			}
			else
			{
				switch(event->key())
				{
					case Qt::Key_PageUp:
						if(!m_editorWindow->prevSlide())
							QApplication::beep();
						break;

					case Qt::Key_PageDown:
						if(!m_editorWindow->nextSlide())
							QApplication::beep();
						break;

					default:
						m_desk->keyPressEvent(event);
						break;
				}

			}
		}


		void wheelEvent(QWheelEvent *event)
		{
                        scaleView(pow((double)2, event->delta() / 240.0));
		}


		void scaleView(qreal scaleFactor)
		{
			qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
			if (factor < 0.07 || factor > 100)
				return;

			scale(scaleFactor, scaleFactor);
		}


	private:
		MyGraphicsScene * m_desk;
		SlideEditorWindow * m_editorWindow;
};




SlideEditorWindow::SlideEditorWindow(SlideGroup *group, QWidget * parent)
    : AbstractSlideGroupEditor(group,parent), m_usingGL(false),
    m_ignoreUndoPropChanges(false),
    m_slideGroup(0),
    m_propDockEmpty(0),
    m_currentConfigContent(0),
    m_currentConfig(0),
    m_itemListView(0),
    m_masterSlideEditor(0),
    m_iconSize(192),
    m_iconSizeSlider(0),
    m_lockIconSizeSetter(false)
{

	m_scene = new MyGraphicsScene(MyGraphicsScene::Editor,this);
	m_view = new MyGraphicsView(this);


	QRect sceneRect(0,0,1024,768);

	if(MainWindow::mw())
	{
		MainWindow *mw = MainWindow::mw();
		sceneRect = mw->standardSceneRect();

		connect(mw, SIGNAL(appSettingsChanged()),       this, SLOT(appSettingsChanged()));
 		connect(mw, SIGNAL(aspectRatioChanged(double)), this, SLOT(aspectRatioChanged(double)));
	}

	m_view->setMyScene(m_scene);
	m_scene->setSceneRect(sceneRect);
	setCentralWidget(m_view);

	resize(sceneRect.width(),sceneRect.height());


	appSettingsChanged();

        m_undoStack = new QUndoStack();

	setupSlideList();
	setupToolbar();
	setupUndoView();
	setupPropDock();
	setupItemList();
	setupViewportLines();




	if(DEBUG_MODE)
	{
		if(QFile("test.xml").exists())
		{
			m_doc = new Document();
			m_doc->load("test.xml");
			//r.readSlide(m_slide);

			qDebug("Loaded test.xml");

		}
		else
		{
			Slide * slide = new Slide();
			SlideGroup *g = new SlideGroup();
			g->addSlide(slide);
			m_doc->addGroup(g);
			m_scene->setSlide(slide);
		}

		QList<SlideGroup*> glist = m_doc->groupList();
		if(glist.size() >2)
		{
			SlideGroup *g = glist[0];
			assert(g);

			setSlideGroup(g);
		}
		else
		{
			qDebug("Error: No groups in test.xml");
		}
	}

	if(group)
		setSlideGroup(group);

}

SlideEditorWindow::~SlideEditorWindow()
{
	delete m_slideListView->model();

 	if(DEBUG_MODE)
 	{
 		if(m_doc)
 		{
 			m_doc->save("test.xml");
 			delete m_doc;
 		}
	}
	
	if(m_masterSlideEditor)
	{
		m_masterSlideEditor->close();
		delete m_masterSlideEditor;
		m_masterSlideEditor = 0;
	}

	delete m_undoStack;
}

void SlideEditorWindow::setAutosaveOn(bool flag)
{
	if(MainWindow::mw())
		MainWindow::mw()->setAutosaveEnabled(flag);
}

void SlideEditorWindow::showEvent(QShowEvent *evt)
{
	evt->accept();

	// Restore state
	QSettings settings;
	QSize sz = settings.value("slideeditor/size").toSize();
	if(sz.isValid())
		resize(sz);
	QPoint p = settings.value("slideeditor/pos").toPoint();
	if(!p.isNull())
	{
		if(p.y() < 0)
			p.setY(1);
		move(p);
	}


	restoreState(settings.value("slideeditor/state").toByteArray());



	float sx = ((float)m_view->width()) / m_scene->width();
	float sy = ((float)m_view->height()) / m_scene->height();

	float scale = qMin(sx,sy);
	m_view->setTransform(QTransform().scale(scale,scale));
        //qDebug("Scaling: %.02f x %.02f = %.02f",sx,sy,scale);
	m_view->update();

	//if(AppSettings::autosaveTime() > 0)
	//	m_autosaveTimer->start(AppSettings::autosaveTime() * 1000);
}

void SlideEditorWindow::closeEvent(QCloseEvent *evt)
{
	QSettings settings;
	settings.setValue("slideeditor/size",size());
	settings.setValue("slideeditor/pos",pos());
	settings.setValue("slideeditor/state",saveState());

	//m_autosaveTimer->stop();
	evt->accept();
	emit closed();
}

void SlideEditorWindow::setupToolbar()
{
	QList<QToolBar*> toolbars;
	
	QToolBar *toolbar;
	 
	/// New Objects Toolbar
	toolbar = addToolBar(tr("New Objects"));
	toolbars<<toolbar;

	QAction  *newAction = toolbar->addAction(QIcon(":/data/insert-text-24.png"), tr("New Text Item"));
	newAction->setShortcut(QString(tr("CTRL+SHIFT+T")));
	connect(newAction, SIGNAL(triggered()), this, SLOT(newTextItem()));

	QAction  *newBox = toolbar->addAction(QIcon(":/data/insert-rect-24.png"), tr("New Box Item"));
	newBox->setShortcut(QString(tr("CTRL+SHIFT+B")));
	connect(newBox, SIGNAL(triggered()), this, SLOT(newBoxItem()));

	QAction  *newVideo = toolbar->addAction(QIcon(":/data/stock-panel-multimedia.png"), tr("New Video Item"));
	newVideo->setShortcut(QString(tr("CTRL+SHIFT+V")));
	connect(newVideo, SIGNAL(triggered()), this, SLOT(newVideoItem()));

	QAction  *newImage = toolbar->addAction(QIcon(":/data/insert-image-24.png"), tr("New Image Item"));
	newImage->setShortcut(QString(tr("CTRL+SHIFT+I")));
	connect(newImage, SIGNAL(triggered()), this, SLOT(newImageItem()));

	QAction  *newOutputView = toolbar->addAction(QIcon(":/data/stock-execute.png"), tr("New Output Viewer Item"));
	newOutputView->setShortcut(QString(tr("CTRL+SHIFT+O")));
	connect(newOutputView, SIGNAL(triggered()), this, SLOT(newOutputView()));

	/// Arrange Items
	toolbar = addToolBar(tr("Arrange Items"));
	toolbars<<toolbar;

	QAction  *centerHor = toolbar->addAction(QIcon(":/data/obj-center-hor.png"), tr("Center Items Horizontally"));
	centerHor->setShortcut(QString(tr("CTRL+SHIFT+H")));
	connect(centerHor, SIGNAL(triggered()), this, SLOT(centerSelHorz()));

	QAction  *centerVer = toolbar->addAction(QIcon(":/data/obj-center-ver.png"), tr("Center Items Vertically"));
	centerVer->setShortcut(QString(tr("CTRL+SHIFT+V")));
	connect(centerVer, SIGNAL(triggered()), this, SLOT(centerSelVert()));

	/// Slide Operations
	toolbar = addToolBar(tr("Slide Operations"));
	toolbars<<toolbar;

	QAction  *newSlide = toolbar->addAction(QIcon(":/data/stock-add.png"), tr("New Slide"));
	newSlide->setShortcut(QString(tr("CTRL+M")));
	connect(newSlide, SIGNAL(triggered()), this, SLOT(newSlide()));

	QAction  *dupSlide = toolbar->addAction(QIcon(":/data/stock-convert.png"), tr("Duplicate Slide"));
	dupSlide->setShortcut(QString(tr("CTRL+D")));
	connect(dupSlide, SIGNAL(triggered()), this, SLOT(dupSlide()));

	toolbar->addSeparator();

	QAction  *configBg = toolbar->addAction(QIcon(":/data/stock-insert-image.png"), tr("Setup Slide Background"));
	configBg->setShortcut(QString(tr("CTRL+SHIFT+B")));
	connect(configBg, SIGNAL(triggered()), this, SLOT(slotConfigBackground()));

	m_masterSlideAction = toolbar->addAction(QIcon(":/data/master-slide.png"), tr("Edit Master Slide"));
	m_masterSlideAction->setShortcut(QString(tr("CTRL+SHIFT+M")));
	connect(m_masterSlideAction, SIGNAL(triggered()), this, SLOT(editMasterSlide()));

	toolbar->addSeparator();

	QAction  *delSlide = toolbar->addAction(QIcon(":/data/stock-delete.png"), tr("Delete Slide"));
	connect(delSlide, SIGNAL(triggered()), this, SLOT(delSlide()));


	/// Text Size Toolbar
	toolbar = addToolBar(tr("Text Size"));
	toolbars<<toolbar;


	m_textBase = new QWidget(toolbar);
	QHBoxLayout * layout = new QHBoxLayout(m_textBase);
	QLabel * label = new QLabel(tr("Text: "));
	layout->addWidget(label);

	m_textSizeBox = new QDoubleSpinBox(m_textBase);
	m_textSizeBox->setSuffix(tr("pt"));
	m_textSizeBox->setMinimum(1);
	m_textSizeBox->setValue(38);
	m_textSizeBox->setDecimals(1);
	m_textSizeBox->setMaximum(5000);
	//connect(m_textSizeBox, SIGNAL(returnPressed()), this, SLOT(textSizeChanged(double)));
	connect(m_textSizeBox, SIGNAL(editingFinished()), this, SLOT(textSizeBoxChanged()));

	m_textBase->setEnabled(false);

	layout->addWidget(m_textSizeBox);
	m_textBase->setLayout(layout);

	toolbar->addWidget(m_textBase);

	m_textPlusAction = toolbar->addAction(QIcon(":/data/stock-sort-descending.png"), tr("Increase Font Size"));
	m_textPlusAction->setShortcut(QString(tr("CTRL+SHFIT++")));
	connect(m_textPlusAction, SIGNAL(triggered()), this, SLOT(textPlus()));

	m_textMinusAction = toolbar->addAction(QIcon(":/data/stock-sort-ascending.png"), tr("Decrease Font Size"));
	m_textMinusAction->setShortcut(QString(tr("CTRL+SHFIT+-")));
	connect(m_textMinusAction, SIGNAL(triggered()), this, SLOT(textMinus()));

	m_textPlusAction->setEnabled(false);
	m_textMinusAction->setEnabled(false);

	m_textFitToSlideAction = toolbar->addAction(QIcon(":/data/stock-fit-rect.png"), tr("Fit Text to Slide"));
	m_textFitToSlideAction->setShortcut(QString(tr("CTRL+SHFIT+S")));
	connect(m_textFitToSlideAction, SIGNAL(triggered()), this, SLOT(textFitToSlide()));

	m_textFitToBoxAction = toolbar->addAction(QIcon(":/data/stock-fit-out.png"), tr("Fit Text to Box"));
	m_textFitToBoxAction->setShortcut(QString(tr("CTRL+SHFIT+F")));
	connect(m_textFitToBoxAction, SIGNAL(triggered()), this, SLOT(textFitToRect()));

	m_textNaturalBoxAction = toolbar->addAction(QIcon(":/data/stock-fit-in.png"), tr("Fit Box to Text Naturally"));
	m_textNaturalBoxAction->setShortcut(QString(tr("CTRL+SHFIT+N")));
	connect(m_textNaturalBoxAction, SIGNAL(triggered()), this, SLOT(textNaturalBox()));

	m_textNaturalBoxAction->setEnabled(false);
	m_textFitToSlideAction->setEnabled(false);
	m_textFitToBoxAction->setEnabled(false);

	connect(m_scene, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

	/// Undo/Redo
	toolbar = addToolBar(tr("Undo/Redo"));
	toolbars<<toolbar;

	QAction *action = m_undoStack->createUndoAction(this);
	action->setIcon(QIcon(":/data/stock-undo.png"));
	action->setShortcut(QString(tr("CTRL+Z")));
	toolbar->addAction(action);

	action = m_undoStack->createRedoAction(this);
	action->setIcon(QIcon(":/data/stock-redo2.png"));
	action->setShortcut(QString(tr("CTRL+SHIFT+Z")));
	toolbar->addAction(action);


	/// Slide Timeout Toolbar
	toolbar = addToolBar(tr("Slide Timeout"));
	toolbars<<toolbar;

	QWidget * base2 = new QWidget(toolbar);
	QHBoxLayout * layout2 = new QHBoxLayout(base2);
	QLabel * label2 = new QLabel(tr("Change Slides In: "));
	layout2->addWidget(label2);

	m_slideTimeout = new QDoubleSpinBox(m_textBase);
	m_slideTimeout->setSuffix(tr(" sec"));
	m_slideTimeout->setSpecialValueText(tr("(Never)"));
	m_slideTimeout->setMinimum(0);
	m_slideTimeout->setValue(0);
	m_slideTimeout->setDecimals(2);
	m_slideTimeout->setMaximum(5000);
	connect(m_slideTimeout, SIGNAL(valueChanged(double)), this, SLOT(setSlideTimeout(double)));
	layout2->addWidget(m_slideTimeout);
	base2->setLayout(layout2);
	toolbar->addWidget(base2);

	QAction * zeroTimeout = toolbar->addAction(QIcon(":/data/stock-clear.png"), tr("Set change time to zero (Never change)"));
	zeroTimeout->setShortcut(QString(tr("CTRL+SHIFT+N")));
	connect(zeroTimeout, SIGNAL(triggered()), this, SLOT(zeroSlideTimeout()));

	QAction * guessTimeout = toolbar->addAction(QIcon(":/data/stock-about.png"), tr("Guess an appropriate change time based on slide contents"));
	guessTimeout->setShortcut(QString(tr("CTRL+SHIFT+G")));
	connect(guessTimeout, SIGNAL(triggered()), this, SLOT(guessSlideTimeout()));

	//toolbar->addSeparator();
	toolbar = addToolBar(tr("Live"));
	toolbars<<toolbar;

	QAction  *liveAction  = toolbar->addAction(QIcon(":/data/stock-fullscreen.png"), tr("Send the Current Slide to the Live Output"));
	liveAction->setShortcut(QString(tr("F5")));
	connect(liveAction, SIGNAL(triggered()), this, SLOT(setCurrentSlideLive()));



// 	toolbar = addToolBar(tr("Fade Speed"));
// 	toolbars<<toolbar;
// 
// 
// 	QWidget * base3 = new QWidget(toolbar);
// 	QHBoxLayout * layout3 = new QHBoxLayout(base3);
// 
// 	m_fadeSliderLabel = new QLabel(tr("Fade Speed: (+)"));
// 	layout3->addWidget(m_fadeSliderLabel);
// 
// 	m_fadeSlider = new QSlider(Qt::Horizontal,base3);
// 	m_fadeSlider->setMinimum(0);
// 	m_fadeSlider->setValue(1);
// 	m_fadeSlider->setMaximum(100);
// 	m_fadeSlider->setMaximum(100);
// 	m_fadeSlider->setSingleStep(5);
// 	m_fadeSlider->setTickInterval(5);
// 	m_fadeSlider->setTickPosition(QSlider::TicksBelow);
// 	connect(m_fadeSlider, SIGNAL(valueChanged(int)), this, SLOT(setFadeSpeedPreset(int)));
// 	layout3->addWidget(m_fadeSlider);
// 
// 	m_fadeSliderLabel2 = new QLabel(tr("(-) "));
// 	layout3->addWidget(m_fadeSliderLabel2);
// 
// 	m_inheritFadeBox = new QCheckBox(tr("Use Default"),base3);
// 	m_inheritFadeBox->setToolTip(tr("Use the fade speed and quality settings for this slide group or application settings"));
// 	connect(m_inheritFadeBox, SIGNAL(toggled(bool)), this, SLOT(setInheritFade(bool)));
// 	layout3->addWidget(m_inheritFadeBox);
// 
// 	base3->setLayout(layout3);
// 	toolbar->addWidget(base3);

	/// Slide Setup toolbar
	toolbar = addToolBar(tr("Slide Setup"));
	toolbars << toolbar;

	QAction  *slideProp = toolbar->addAction(QIcon(":/data/stock-properties.png"), tr("Slide Properties"));
	slideProp->setShortcut(QString(tr("F2")));
	connect(slideProp, SIGNAL(triggered()), this, SLOT(slideProperties()));

	toolbar->addSeparator();

	QAction  *groupProp = toolbar->addAction(QIcon(":/data/stock-preferences.png"), tr("Slide Group Properties"));
	groupProp->setShortcut(QString(tr("SHIFT+F2")));
	connect(groupProp, SIGNAL(triggered()), this, SLOT(groupProperties()));

	/// Editor Setup Toolbar
	toolbar = addToolBar(tr("Editor Setup"));
	toolbars<<toolbar;


	QAction  *configGrid = toolbar->addAction(QIcon(":/data/config-grid.png"), tr("Setup Grid and Guidelines"));
	connect(configGrid, SIGNAL(triggered()), this, SLOT(slotConfigGrid()));

	QAction  *autosaveOn = toolbar->addAction(QIcon(":/data/stock-history.png"), tr("Enable/Disable Autosave"));
	autosaveOn->setCheckable(true);
	autosaveOn->setChecked(true);
	connect(autosaveOn, SIGNAL(toggled(bool)), this, SLOT(setAutosaveOn(bool)));


	// Yes, this seems a bit hackish - anybody know a better way to update the action text with the shortcut description
	foreach(QToolBar *tb, toolbars)
	{
		tb->setObjectName((char*)tb->windowTitle().constData());
		foreach(QAction *action, tb->actions())
		{
			QString shortcut = action->shortcut().toString();
			if(!shortcut.trimmed().isEmpty())
			{
				QString text = action->text();
				text.replace("&","");
				action->setText(QString("%1 (%2)").arg(text).arg(shortcut));
			}
		}
	}



}

void SlideEditorWindow::setInheritFade(bool flag)
{
// 	m_scene->slide()->setInheritFadeSettings(flag);
// 	m_fadeSlider->setEnabled(!flag);
// 	m_fadeSliderLabel->setEnabled(!flag);
// 	m_fadeSliderLabel2->setEnabled(!flag);
}

void SlideEditorWindow::setFadeSpeedPreset(int value)
{
// 	double percent = ((double)value) / 100.0;
// 	double speed = 3000.0 * percent;
// 	if(speed<1)
// 		speed = 1;
// 	double quality = 0.04*speed; // "standard" quality is 10 frames every 250 ms
// 	double qualityMax = speed<500 ? 20 :
// 			    speed<1000 ? 30 :
// 			    speed<2000 ? 45 :
// 			    speed<=3000 ? 60 : 30;
// 	quality = qMin(qualityMax,quality);
// 	if(quality<1)
// 		quality=1;
// 	//qDebug() << "SlideEditorWindow::setFadeSpeedPreset: value:"<<value<<" ("<<percent<<"), speed:"<<speed<<", quality:"<<quality<<" ( qualityMax:"<<qualityMax<<")";
// 	m_fadeSlider->setToolTip(QString(tr("%1 % - %2 ms / %3 fames")).arg(value).arg(speed).arg(quality));
// 	m_scene->slide()->setCrossFadeSpeed(speed);
// 	m_scene->slide()->setCrossFadeQuality(quality);
}

void SlideEditorWindow::guessSlideTimeout()
{
	double time = m_scene->slide()->guessTimeout();
	m_slideTimeout->setValue(time);
	m_scene->slide()->setAutoChangeTime(time);
}

void SlideEditorWindow::zeroSlideTimeout()
{
	m_slideTimeout->setValue(0);
}

void SlideEditorWindow::setSlideTimeout(double d)
{
	m_scene->slide()->setAutoChangeTime(d);
}

void SlideEditorWindow::slotConfigBackground()
{
	BackgroundItem * bg = dynamic_cast<BackgroundItem*>(m_scene->slide()->background());
	BackgroundContent *bgContent = dynamic_cast<BackgroundContent*>(m_scene->findVisualDelegate(bg));
	if(bgContent)
	{
		BackgroundConfig * bgConfig = new BackgroundConfig(bgContent);
		bgConfig->show();
	}
	else
	{
		QMessageBox::critical(this,tr("No Background"),tr("Internal Error: Could not find background object to configure!"));
	}
}


void SlideEditorWindow::slotConfigGrid()
{
	GridDialog d(this);
	d.exec();

	setupViewportLines();
}

void SlideEditorWindow::setCurrentSlideLive()
{
	if(!MainWindow::mw())
		return;
	if(!m_slideGroup)
		return;
	Slide * slide = m_scene->slide();
	double changeTime = slide->autoChangeTime();
	if(changeTime > 0)
	{
		if(QMessageBox::question(this,
			tr("Allow Slide to Auto-Change?"),
			QString(tr("This slide is set to automatically change to the next slide after %1 seconds. Do you want to ALLOW this slide to automatically change (click Yes) or do you want to CANCEL the timer, just for this viewing?")).arg(changeTime),
			QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes
			) == QMessageBox::Cancel)
			slide->setAutoChangeTime(0);
	}

	MainWindow::mw()->setLiveGroup(m_slideGroup,slide,false); // false = dont show progress dialog

	if(changeTime > 0 && slide->autoChangeTime() == 0)
	{
		slide->setAutoChangeTime(changeTime);
	}
}


void SlideEditorWindow::centerSelHorz()
{
	QList<QGraphicsItem *> selection = m_scene->selectedItems();
	QRectF scene = m_scene->sceneRect();

	qreal halfX = scene.width()/2;
	qreal halfY = scene.height()/2;
	foreach(QGraphicsItem *item, selection)
	{
		AbstractContent * content = dynamic_cast<AbstractContent *>(item);
		if(content)
		{
			QRectF r = content->boundingRect();
			content->setPos( halfX - r.width()/2 - r.left() + scene.left(), content->pos().y() );
			content->syncToModelItem(0);
		}
	}
}


void SlideEditorWindow::centerSelVert()
{
	QList<QGraphicsItem *> selection = m_scene->selectedItems();
	QRectF scene = m_scene->sceneRect();

	qreal halfX = scene.width()/2;
	qreal halfY = scene.height()/2;
	foreach(QGraphicsItem *item, selection)
	{
		AbstractContent * content = dynamic_cast<AbstractContent *>(item);
		if(content)
		{
			QRectF r = content->boundingRect();
			content->setPos( content->pos().x(), halfY - r.height()/2 - r.top() + scene.top());
			content->syncToModelItem(0);
		}
	}
}

void SlideEditorWindow::selectionChanged()
{
	m_currentTextItems.clear();
	QList<QGraphicsItem *> selection = m_scene->selectedItems();
	bool foundText = false;
	double sizeSum = 0.0;
	AbstractContent * firstContent = 0;
	foreach(QGraphicsItem *item, selection)
	{
		if(!firstContent)
			firstContent = dynamic_cast<AbstractContent*>(item);
		TextBoxContent * tmp = dynamic_cast<TextBoxContent*>(item);
		if(tmp)
		{
			m_currentTextItems << tmp;
			foundText = true;
			sizeSum += dynamic_cast<TextItem*>(tmp->modelItem())->findFontSize();
		}
	}

	if(foundText)
		m_textSizeBox->setValue(sizeSum / m_currentTextItems.size());

	m_textBase->setEnabled(foundText);
	m_textPlusAction->setEnabled(foundText);
	m_textMinusAction->setEnabled(foundText);
	m_textNaturalBoxAction->setEnabled(foundText);
	m_textFitToSlideAction->setEnabled(foundText);
	m_textFitToBoxAction->setEnabled(foundText);

	// dont update if no firstContent so we dont clear out the prop dock
	if(firstContent)
		updatePropDock(firstContent);

	if(firstContent && m_itemListView)
	{
		QModelIndex idx = m_itemModel->indexForItem(firstContent->modelItem());
		if(idx.isValid())
			m_itemListView->setCurrentIndex(idx);
	}
}

void SlideEditorWindow::textPlus()
{
	if(m_currentTextItems.size() <= 0)
		return;

	double value = m_textSizeBox->value();
	value += TOOLBAR_TEXT_SIZE_INC;
	m_textSizeBox->setValue(value);
	textSizeBoxChanged();

}

void SlideEditorWindow::textMinus()
{
	if(m_currentTextItems.size() <= 0)
		return;

	double value = m_textSizeBox->value();
	value -= TOOLBAR_TEXT_SIZE_INC;
	m_textSizeBox->setValue(value);
	textSizeBoxChanged();

}

//void SlideEditorWindow::textSizeChanged(double pt)
void SlideEditorWindow::textSizeBoxChanged()
{
	if(m_currentTextItems.size() <= 0)
		return;
	double pt = m_textSizeBox->value();
	foreach(TextBoxContent *text, m_currentTextItems)
		dynamic_cast<TextItem*>(text->modelItem())->changeFontSize(pt);
	textNaturalBox();
}


void SlideEditorWindow::textFitToRect()
{
	if(m_currentTextItems.size() <= 0)
		return;

	foreach(TextBoxContent *text, m_currentTextItems)
		dynamic_cast<TextItem*>(text->modelItem())->fitToSize(text->contentsRect().size(),8);

}


void SlideEditorWindow::textFitToSlide()
{
	if(m_currentTextItems.size() <= 0)
		return;

	QRectF rect = m_scene->sceneRect();
	QSize size = rect.size().toSize();
	foreach(TextBoxContent *text, m_currentTextItems)
	{
		dynamic_cast<TextItem*>(text->modelItem())->fitToSize(size,8);
		text->modelItem()->setPos(QPointF(0,0));
		text->modelItem()->setContentsRect(QRectF(QPointF(0,0),rect.size()));
	}

	textNaturalBox();
	centerSelVert();
	centerSelHorz();
}


void SlideEditorWindow::textNaturalBox()
{
	if(m_currentTextItems.size() <= 0)
		return;

	foreach(TextBoxContent *text, m_currentTextItems)
	{
		QSize natural = dynamic_cast<TextItem*>(text->modelItem())->findNaturalSize(m_scene->sceneRect().width());
		QRectF newRect = QRectF(text->modelItem()->contentsRect().topLeft(),QSizeF(natural));
		//qDebug() << "SlideEditorWindow::textNaturalBox: "<<text->modelItem()->itemName()<<": natural:"<<natural<<", newRect:"<<newRect;
		text->modelItem()->setContentsRect(newRect);
	}
}

void SlideEditorWindow::itemDoubleClicked(AbstractContent *item)
{
	// The flag_fromMaster property is set by MyGraphicsScene for master slide items
	QVariant masterFlag = item->property("flag_fromMaster");
	if(!masterFlag.isNull() && masterFlag.toBool())
		editMasterSlide();
}

void SlideEditorWindow::editMasterSlide()
{
	if(!m_slideGroup)
		return;

	if(m_masterSlideEditor)
	{
		m_masterSlideEditor->show();
		m_masterSlideEditor->raise();
	}
	else
	{
		m_masterSlideEditor = new SlideEditorWindow(0,this);
		SlideGroup *tmpGroup = new SlideGroup();
		tmpGroup->setProperty("isMasterSlide",true);
		tmpGroup->setGroupTitle(QString(tr("Master Slide for %1")).arg(m_slideGroup->groupTitle().isEmpty() ? QString(tr("Group %1")).arg(m_slideGroup->groupNumber()) : m_slideGroup->groupTitle()));
		tmpGroup->addSlide(m_slideGroup->masterSlide());
		m_masterSlideEditor->setSlideGroup(tmpGroup,m_slideGroup->masterSlide());
		connect(m_masterSlideEditor, SIGNAL(closed()), this, SLOT(masterSlideEditorClosed()));

		m_masterSlideEditor->show();
	}
}

void SlideEditorWindow::masterSlideEditorClosed()
{
	// TODO anything needed to do here?
}

void SlideEditorWindow::setupUndoView()
{
	QDockWidget *dock = new QDockWidget(tr("Undo/Redo"), this);
	dock->setObjectName("undoViewDock");
	m_undoView = new QUndoView(m_undoStack,dock);
	dock->setWidget(m_undoView);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
}

void SlideEditorWindow::setupPropDock()
{
	m_propDock = new QDockWidget(tr("Item Properties"), this);
	m_propDock->setObjectName("itemPropDock");

	updatePropDock(0);

	//m_propDock->setWidget(m_undoView);
	addDockWidget(Qt::RightDockWidgetArea, m_propDock);
}

void SlideEditorWindow::updatePropDock(AbstractContent *content)
{
	if(!content)
	{
		if(!m_propDockEmpty)
		{
			m_propDockEmpty = new QWidget();
			QVBoxLayout *layout = new QVBoxLayout(m_propDockEmpty);
			QLabel *label = new QLabel(tr("<center><font color='gray'><b>No item selected</b></font></center>"));
			layout->addWidget(label);
		}

		m_propDock->setWidget(m_propDockEmpty);
		m_propDockEmpty->show();

		m_propDock->setWindowTitle(tr("Item Settings"));
	}
	else
	{
		int previousIdx = -1;
		QString previousTitle = "";

		if(m_currentConfig && m_currentConfigContent != content)
		{
			previousIdx = m_currentConfig->tabWidget()->currentIndex();
			previousTitle = m_currentConfig->tabWidget()->tabText(previousIdx);

			m_currentConfig->close();
			delete m_currentConfig;
			m_currentConfig = 0;
			m_currentConfigContent = 0;
		}

		// TODO this is a duplicate of code in MyGraphicsScene
		GenericItemConfig * p = 0;

		if (TextBoxContent * text = dynamic_cast<TextBoxContent *>(content))
			p = new TextBoxConfig(text);
		else
		if (OutputViewContent * text = dynamic_cast<OutputViewContent *>(content))
			p = new OutputViewConfig(text);
		else
		if (VideoFileContent * vid = dynamic_cast<VideoFileContent *>(content))
			p = new VideoFileConfig(vid);
		else
		if (BackgroundContent * bg = dynamic_cast<BackgroundContent *>(content))
			p = new BackgroundConfig(bg);
		else
		if (BoxContent * box = dynamic_cast<BoxContent *>(content))
			p = new BoxConfig(box);
		else
		if (ImageContent * box = dynamic_cast<ImageContent *>(content))
			p = new ImageConfig(box);

		// generic config
		if (!p)
			p = new GenericItemConfig(content);

		m_propDock->setWidget(p);
		p->show();
		p->adjustSize();
		m_propDock->adjustSize();

		if(previousIdx > -1 &&
			p->tabWidget()->tabText(previousIdx) == previousTitle)
			p->tabWidget()->setCurrentIndex(previousIdx);

    		m_currentConfigContent = content;
    		m_currentConfig = p;

    		m_propDock->setWindowTitle(QString(tr("Settings for %1")).arg(AbstractItem::guessTitle(content->modelItem()->itemName())));
	}
}

void SlideEditorWindow::appSettingsChanged()
{
	if(AppSettings::useOpenGL() && !m_usingGL)
	{
		m_usingGL = true;
		m_view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
		//qDebug("SlideEditorWindow::appSettingsChanged(): Loaded OpenGL Viewport");
	}
	else
	if(!AppSettings::useOpenGL() && m_usingGL)
	{
		m_usingGL = false;
		m_view->setViewport(new QWidget());
		//qDebug("SlideEditorWindow::appSettingsChanged(): Loaded Non-GL Viewport");
	}

	setupViewportLines();
}

void SlideEditorWindow::aspectRatioChanged(double x)
{
	//qDebug("SlideEditorWindow::aspectRatioChanged(): New aspect ratio: %.02f",x);
	if(MainWindow::mw())
	{
		m_scene->setSceneRect(MainWindow::mw()->standardSceneRect());
		setupViewportLines();
	}
}



SlideEditorWindowListView::SlideEditorWindowListView(SlideEditorWindow * ctrl, QWidget *parent) : QListView(ctrl), ctrl(ctrl) {}
void SlideEditorWindowListView::keyPressEvent(QKeyEvent *event)
{

	QModelIndex oldIdx = currentIndex();
	QListView::keyPressEvent(event);
	QModelIndex newIdx = currentIndex();
	if(oldIdx.row() != newIdx.row())
	{
		ctrl->slideSelected(newIdx);
	}
}



void SlideEditorWindow::setupSlideList()
{
	QDockWidget *dock = new QDockWidget(tr("Slide List"), this);
	dock->setObjectName("slideListDock");
	
	QWidget * base = new QWidget(dock);
	QVBoxLayout * layout = new QVBoxLayout(base);
	layout->setMargin(0);
	
	m_slideListView = new SlideEditorWindowListView(this,base);
	m_slideListView->setViewMode(QListView::ListMode);
	//m_slideListView->setViewMode(QListView::IconMode);
	m_slideListView->setMovement(QListView::Free);
	m_slideListView->setWordWrap(true);
	m_slideListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_slideListView->setDragEnabled(true);
	m_slideListView->setAcceptDrops(true);
	m_slideListView->setDropIndicatorShown(true);

	connect(m_slideListView,SIGNAL(activated(const QModelIndex &)),this,SLOT(slideSelected(const QModelIndex &)));
	connect(m_slideListView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(slideSelected(const QModelIndex &)));

	// deleting old selection model per http://doc.trolltech.com/4.5/qabstractitemview.html#setModel
	QItemSelectionModel *m = m_slideListView->selectionModel();

	m_slideModel = new SlideGroupListModel();
	m_slideListView->setModel(m_slideModel);
	connect(m_slideModel, SIGNAL(repaintList()), this, SLOT(repaintSlideList()));
	connect(m_slideModel, SIGNAL(slidesDropped(QList<Slide*>)), this, SLOT(slidesDropped(QList<Slide*>)));

	if(m)
	{
		delete m;
		m=0;
	}

	QItemSelectionModel *currentSelectionModel = m_slideListView->selectionModel();
	connect(currentSelectionModel, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(currentChanged(const QModelIndex &, const QModelIndex &)));
	
	layout->addWidget(m_slideListView);
	
	QWidget *hboxBase = new QWidget(base);

	QHBoxLayout *hbox = new QHBoxLayout(hboxBase);
	
	QLabel * label = new QLabel("Zoom:");
	hbox->addWidget(label);
	
	m_iconSizeSlider= new QSlider(Qt::Horizontal);
	m_iconSizeSlider->setMinimum(16);
	m_iconSizeSlider->setMaximum(480);
	m_iconSizeSlider->setTickInterval(16);
	m_iconSizeSlider->setSingleStep(16);
	m_iconSizeSlider->setPageStep(32);
	m_iconSizeSlider->setTickPosition(QSlider::TicksBelow);
	hbox->addWidget(m_iconSizeSlider,1); 
	
	connect(m_iconSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(setIconSize(int)));
	
	QSettings s;
	setIconSize(s.value(QString("slideeditorwindow/icon-size"),m_iconSize).toInt());
	
	layout->addWidget(hboxBase);


	//dock->setWidget(m_slideListView);
	dock->setWidget(base);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
	//viewMenu->addAction(dock->toggleViewAction());
}


void SlideEditorWindow::setIconSize(int size)
{
	if(m_lockIconSizeSetter)
		return;
	m_lockIconSizeSetter = true;
	int old = size;
	size = (int)(size / 16) * 16;
	//qDebug() << "setIconSize: old:"<<old<<", size:"<<size;
	if(m_iconSizeSlider->value() != size)
		m_iconSizeSlider->setValue(size);
// 	if(m_spinBox->value() != size)
// 		m_spinBox->setValue(size);
		
	QSettings s;
	s.setValue(QString("slideeditorwindow/icon-size"),size);
	m_iconSize = size;
		
	// the model will automatically correct aspect ratio as needed
	m_slideModel->setIconSize(QSize(size,size));
	m_slideListView->reset();
	m_lockIconSizeSetter = false;
}



void SlideEditorWindow::repaintSlideList()
{
	m_slideListView->clearFocus();
	m_slideListView->setFocus();
	m_slideListView->update();
}

void SlideEditorWindow::setupItemList()
{
	QDockWidget *dock = new QDockWidget(tr("Item List"), this);
	dock->setObjectName("slideItemDock");
	m_itemListView = new QListView(dock);
	m_itemListView->setViewMode(QListView::ListMode);
	//m_slideListView->setViewMode(QListView::IconMode);
	m_itemListView->setMovement(QListView::Free);
	m_itemListView->setWordWrap(true);
	m_itemListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_itemListView->setDragEnabled(true);
	m_itemListView->setAcceptDrops(true);
	m_itemListView->setDropIndicatorShown(true);

	connect(m_itemListView,SIGNAL(activated(const QModelIndex &)),this,SLOT(itemSelected(const QModelIndex &)));
	connect(m_itemListView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(itemSelected(const QModelIndex &)));

	// deleting old selection model per http://doc.trolltech.com/4.5/qabstractitemview.html#setModel
	//QItemSelectionModel *m = m_itemListView->selectionModel();

	m_itemModel = new SlideItemListModel();
	m_itemListView->setModel(m_itemModel);
	connect(m_itemModel, SIGNAL(itemsDropped(QList<AbstractItem*>)), this, SLOT(itemsDropped(QList<AbstractItem*>)));

// 	if(m)
// 	{
// 		delete m;
// 		m=0;
// 	}

// 	QItemSelectionModel *currentSelectionModel = m_itemListView->selectionModel();
// 	connect(currentSelectionModel, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(currentItemChanged(const QModelIndex &, const QModelIndex &)));
//
	dock->setWidget(m_itemListView);
	addDockWidget(Qt::RightDockWidgetArea, dock);
	//viewMenu->addAction(dock->toggleViewAction());
}




void SlideEditorWindow::currentChanged(const QModelIndex &idx,const QModelIndex &)
{
	if(idx.isValid())
		slideSelected(idx);
}

void SlideEditorWindow::currentItemChanged(const QModelIndex &idx,const QModelIndex &)
{
	if(idx.isValid())
		itemSelected(idx);
}


class MyLine : public QGraphicsLineItem
{
public:
	MyLine ( qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem * parent = 0 ) :
		QGraphicsLineItem (x1,y1,x2,y2,parent) {}

	void paint(QPainter*p, const QStyleOptionGraphicsItem*, QWidget*)
	{
		p->save();
		//p->setCompositionMode(QPainter::CompositionMode_Difference);
		p->setPen(pen());
		p->drawLine(line());
		p->restore();
	}
};

void SlideEditorWindow::addVpLineX(qreal x, qreal y1, qreal y2, bool /*in*/)
{
	static QPen pw(Qt::gray, 0,Qt::DotLine);
	static QPen pb(Qt::black, 0,Qt::DotLine);

	qreal z1 = 0; //in ? 2:0;
//	qreal z2 = in ? 0:2;

	QGraphicsLineItem * g;
	g = new MyLine(x,y1-z1,x,y2-z1);
	g->setPen(pw);
	g->setZValue(9999);
	m_scene->addItem(g);
	m_viewportLines << g;
/*
	g = new QGraphicsLineItem(x,y1-z2,x,y2-z2);
	g->setPen(pw);
	g->setZValue(9999);
	m_scene->addItem(g);
	m_viewportLines << g;
	*/
}

void SlideEditorWindow::addVpLineY(qreal y, qreal x1, qreal x2, bool /*in*/)
{
	static QPen pw(Qt::gray, 0,Qt::DotLine);
	static QPen pb(Qt::black, 0,Qt::DotLine);

	qreal z1 = 0; //in ? 2:0;
	//qreal z2 = in ? 0:2;

	QGraphicsLineItem * g;
	g = new MyLine(x1-z1,y,x2-z1,y);
	g->setPen(pw);
	g->setZValue(9999);
	m_scene->addItem(g);
	m_viewportLines << g;
/*
	g = new QGraphicsLineItem(x1-z2,y,x2-z2,y);
	g->setPen(pw);
	g->setZValue(9999);
	m_scene->addItem(g);
	m_viewportLines << g;
	*/
}


void SlideEditorWindow::setupViewportLines()
{
	bool showThirds = AppSettings::thirdGuideEnabled();

	QRectF r = m_scene->sceneRect();

	foreach(QGraphicsLineItem *l, m_viewportLines)
	{
		m_scene->removeItem(l);
		delete l;
		l=0;
	}

	m_viewportLines.clear();

	// top line
	addVpLineY(r.y(),r.x(),r.width(),false);

	// bottom line
	addVpLineY(r.height(),r.x(),r.width());

	// left line
	addVpLineX(r.x(), r.y(), r.height(),false);

	// right line
	addVpLineX(r.width(), r.y(), r.height());

	if(showThirds)
	{
		// third lines
		qreal x3 = r.width() / 3;
		qreal y3 = r.height() / 3;

		qreal x1 = x3;
		qreal x2 = x3+x3;

		qreal y1 = y3;
		qreal y2 = y3+y3;

		// draw top horizontal third
		addVpLineY(y1,r.x(),r.width());

		// bottom horiz. third
		addVpLineY(y2,r.x(),r.width());

		// left vert third
		addVpLineX(x1,r.y(),r.height());

		// right vert third
		addVpLineX(x2,r.y(), r.height());
	}
	
	bool showTitlesafeGuide = true;
	if(showTitlesafeGuide)
	{
		int xMargin = r.width()  * AppSettings::titlesafeAmount();
		int yMargin = r.height() * AppSettings::titlesafeAmount();
		
		// add top/bottom lines
		addVpLineY(yMargin,xMargin,r.width()-xMargin);
		addVpLineY(r.height()-yMargin,xMargin,r.width()-xMargin);

		// add left/right lines
		addVpLineX(xMargin,yMargin,r.height()-yMargin);
		addVpLineX(r.width()-xMargin,yMargin,r.height()-yMargin);
	}
}

void SlideEditorWindow::slidesDropped(QList<Slide*> list)
{
	QModelIndex idx = m_slideModel->indexForSlide(list.first());
	m_slideListView->setCurrentIndex(idx);
}

void SlideEditorWindow::itemsDropped(QList<AbstractItem*> list)
{
	QModelIndex idx = m_itemModel->indexForItem(list.first());
	m_itemListView->setCurrentIndex(idx);
}

void SlideEditorWindow::setSlideGroup(SlideGroup *group, Slide *curSlide)
{
	if(m_slideGroup)
		disconnect(m_slideGroup,0,this,0);

	if(m_masterSlideEditor)
	{
		m_masterSlideEditor->close();
		delete m_masterSlideEditor;
		m_masterSlideEditor = 0;
	}


	m_slideGroup = group;
	m_slideModel->setSlideGroup(group);

	QVariant masterFlag = group->property("isMasterSlide");
	if(masterFlag.isValid() && masterFlag.toBool())
		m_masterSlideAction->setVisible(false);
	else
		m_scene->setMasterSlide(m_slideGroup->masterSlide());


	// Trigger slideItemChange slot connections, but not an undo entry
	m_ignoreUndoPropChanges = true;
	QList<Slide*> slist = group->slideList();
	foreach(Slide *slide, slist)
		slideChanged(slide, "add", 0, "", "", QVariant());
	m_ignoreUndoPropChanges = false;

	connect(group,SIGNAL(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)),this,SLOT(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)));
	connect(group,SIGNAL(destroyed(QObject*)), this, SLOT(releaseSlideGroup()));

	setWindowTitle(QString(tr("%1 - Slide Editor")).arg(group->groupTitle().isEmpty() ? QString(tr("Group %1")).arg(group->groupNumber()) : group->groupTitle()));
	setWindowIcon(QIcon(":/data/icon-d.png"));
	//m_slideListView->setModel(m_slideModel);

	if(curSlide)
	{
		setCurrentSlide(curSlide);
	}
	else
	{
		QList<Slide*> slist = group->slideList();
		if(slist.size() > 0)
			setCurrentSlide(m_slideModel->slideAt(0));
		else
			qDebug("SlideEditorWindow::setSlideGroup: Group[0] has 0 slides");
	}


}

//#define qGrep(LIST_TYPE, NEW_LIST, OLD_LIST, IT_CLAUSE)
//	foreach(LIST_TYPE _it, OLD_LIST) if(LIST_CLAUSE) NEW_LIST.append(_it);

#define qGrep(IT_VAR, OLD_LIST, CLAUSE)  \
	({typedef Q_TYPEOF(OLD_LIST) OLD_LIST_T; OLD_LIST_T newList; foreach(IT_VAR, OLD_LIST) {if(CLAUSE) {newList.append(_it);}}; newList; })


#define XY(X,Y) X##Y
#define MakeNameXY(FX,LINE) XY(FX,LINE)
#define MakeName(FX) MakeNameXY(FX,__LINE__)

// from http://okmij.org/ftp/cpp-digest/Functional-Cpp.html#LinAlg
#define Lambda(args,ret_type,body) \
	class MakeName(__Lambda___) { \
		public: ret_type operator() args { body; } }

//sorted = qSortX(Lambda((AbstractContent*a,AbstractContent*b),bool, return (a && b) ? a->zValue() < b->zValue()), nonBg);
#define qSortX(OLD_LIST, TYPE_SPEC, COMPARE_BODY) \
	({ QList<TYPE_SPEC> newList = OLD_LIST; \
		qSort(  newList.begin(),  \
			newList.end(),    \
			Lambda((TYPE_SPEC a, TYPE_SPEC b), bool, return (a && b) ? COMPARE_BODY : true) ); newList; });

namespace SlideEditorWindowSortFunctions {
	bool sort_abscon_zvalue(AbstractContent *a, AbstractContent *b)
	{
		return (a && b) ? a->zValue() < b->zValue() : true;
	}
};

void SlideEditorWindow::setCurrentSlide(Slide *slide)
{
	if(!slide)
		return;

	QModelIndex idx = m_slideModel->indexForSlide(slide);
	if(idx.isValid() && idx.row() != m_slideListView->currentIndex().row())
	{
		m_slideListView->clearSelection();
		m_slideListView->setCurrentIndex(idx);
	}

	m_scene->setSlide(slide);
	setupViewportLines();

	QList<AbstractContent *> kids = m_scene->abstractContent();

	qSort(kids.begin(), kids.end(), SlideEditorWindowSortFunctions::sort_abscon_zvalue);

	m_scene->clearSelection();
	kids.last()->setSelected(true);

	m_slideTimeout->setValue(slide->autoChangeTime());

	double crossFadeSpeed = slide->crossFadeSpeed() / 3000;
	if(crossFadeSpeed>1)
		crossFadeSpeed = 1;

// 	m_fadeSlider->setValue((int)(crossFadeSpeed * 100));
// 	m_inheritFadeBox->setChecked(slide->inheritFadeSettings());
	setInheritFade(slide->inheritFadeSettings());

	m_itemModel->setSlide(slide);
	m_itemListView->reset();

// 	if(kids.last()->modelItem() && m_itemListView)
// 	{
// 		QModelIndex idx = m_itemModel->indexForItem(kids.last()->modelItem());
// 		m_itemListView->setCurrentIndex(idx);
// 	}
}

Slide * SlideEditorWindow::nextSlide()
{
	QModelIndex idx = m_slideListView->currentIndex();
	int max = m_slideModel->rowCount();
	if(idx.row() + 1 >= max)
		return 0;

	Slide * slide = m_slideModel->slideAt(idx.row() + 1);
	setCurrentSlide(slide);

	return slide;
}

Slide * SlideEditorWindow::prevSlide()
{
	QModelIndex idx = m_slideListView->currentIndex();
	if(idx.row() - 1 < 0)
		return 0;

	Slide * slide = m_slideModel->slideAt(idx.row() - 1);
	setCurrentSlide(slide);

	return slide;
}




 class UndoSlideItemChanged : public QUndoCommand
 {
 public:
	UndoSlideItemChanged(SlideEditorWindow *window, AbstractItem *item, QString field, QVariant value, QVariant oldValue)
		: m_window(window), m_item(item), m_field(field), m_value(value), m_oldValue(oldValue), redoCount(0)
		{
			setText(QString(qApp->translate("SlideEditorWindow","Change %2 of %1")).arg(AbstractItem::guessTitle(item->itemName())).arg(AbstractItem::guessTitle(field)));
		}

	virtual int id() const { return 0x001; }
	virtual void undo()
	{
		m_window->ignoreUndoChanged(true);
		//qDebug() << "UndoSlideItemChanged::undo: UNDO cmd for "<<m_item->itemName()<<", field:"<<m_field<<", oldValue:"<<m_oldValue<<", newValue:"<<m_value;
		m_item->setProperty(m_field.toLocal8Bit().constData(),m_oldValue);
		m_window->ignoreUndoChanged(false);
	}
	virtual void redo()
	{
		if(redoCount++ > 0)
		{
			m_window->ignoreUndoChanged(true);
			//qDebug() << "UndoSlideItemChanged::redo: REDO cmd for "<<m_item->itemName()<<", field:"<<m_field<<", oldValue:"<<m_oldValue<<", newValue:"<<m_value;
			m_item->setProperty(m_field.toLocal8Bit().constData(),m_value);
			m_window->ignoreUndoChanged(false);
		}
	}
	virtual bool mergeWidth(const QUndoCommand * other)
	{
		if(other->id() != id())
			return false;
		const UndoSlideItemChanged * cmd = static_cast<const UndoSlideItemChanged*>(other);
		if(cmd->m_field    == m_field &&
		   cmd->m_value    == m_value &&
		   cmd->m_oldValue == m_oldValue)
			return true;

		return false;
	}
private:
	SlideEditorWindow *m_window;
	AbstractItem *m_item;
	QString m_field;
	QVariant m_value;
	QVariant m_oldValue;
	int redoCount;
};

class UndoSlideItemAdded : public QUndoCommand
{
public:
	UndoSlideItemAdded(SlideEditorWindow *window, Slide *slide, AbstractItem *item)
		: m_window(window), m_slide(slide), m_item(item), redoCount(0)
		{
			setText(QString(qApp->translate("SlideEditorWindow","Added %1")).arg(AbstractItem::guessTitle(item->itemName().isEmpty() ? "New Item" : item->itemName())));
		}


	virtual void undo()
	{
		m_window->ignoreUndoChanged(true);
		// Todo: not sure why I have to explicitly call removeVisualDelegate here instead of relying on sig/slots to do it,
		// but removeItem() doesnt seem to work for this situation unless I call removeVisualDelegate() here as well
		m_window->scene()->removeVisualDelegate(m_item);
		m_slide->removeItem(m_item);
		m_window->ignoreUndoChanged(false);
	}
	virtual void redo()
	{
		if(redoCount++ > 0)
		{
			m_window->ignoreUndoChanged(true);
			m_slide->addItem(m_item);
			m_window->ignoreUndoChanged(false);
		}
	}
private:
	SlideEditorWindow *m_window;
	Slide *m_slide;
	AbstractItem *m_item;
	int redoCount;
};



 class UndoSlideItemRemoved : public QUndoCommand
 {
 public:
	UndoSlideItemRemoved(SlideEditorWindow *window, Slide *slide, AbstractItem *item)
		: m_window(window), m_slide(slide), m_item(item), redoCount(0)
		{
			setText(QString(qApp->translate("SlideEditorWindow","Removed %1")).arg(AbstractItem::guessTitle(item->itemName())));
		}

	virtual void undo()
	{
		m_window->ignoreUndoChanged(true);
		m_slide->addItem(m_item);
		m_window->ignoreUndoChanged(false);
	}
	virtual void redo()
	{
		if(redoCount++ > 0)
		{
			//qDebug() << "UndoSlideItemChanged::redo: REDO cmd for "<<m_item->itemName()<<", field:"<<m_field<<", oldValue:"<<m_oldValue<<", newValue:"<<m_value;
			m_window->ignoreUndoChanged(true);
			m_slide->removeItem(m_item);
			m_window->ignoreUndoChanged(false);
		}
	}
private:
	SlideEditorWindow *m_window;
	Slide *m_slide;
	AbstractItem *m_item;
	int redoCount;
};

class UndoSlideAdded : public QUndoCommand
{
public:
	UndoSlideAdded(SlideEditorWindow *window, Slide *slide)
		: m_window(window), m_slide(slide), redoCount(0)
		{
			setText(QString(qApp->translate("SlideEditorWindow","Added Slide# %1")).arg(slide->slideNumber()+1));
		}

	virtual void undo()
	{
		m_window->ignoreUndoChanged(true);
		m_window->slideGroup()->removeSlide(m_slide);
		m_window->ignoreUndoChanged(false);
	}
	virtual void redo()
	{
		if(redoCount++ > 0)
		{
			//qDebug() << "UndoSlideItemChanged::redo: REDO cmd for "<<m_item->itemName()<<", field:"<<m_field<<", oldValue:"<<m_oldValue<<", newValue:"<<m_value;
			m_window->ignoreUndoChanged(true);
			m_window->slideGroup()->addSlide(m_slide);
			m_window->ignoreUndoChanged(false);
		}
	}
private:
	SlideEditorWindow *m_window;
	Slide *m_slide;
	int redoCount;
};

// WARNING This command WILL crash dviz if you undo call redo() after undo(), then add an item to that slide - segfault!
// Need to fix! However, I've got to leave for the day so I'm going to commit this code and work later on it.

class UndoSlideRemoved : public QUndoCommand
{
public:
	UndoSlideRemoved(SlideEditorWindow *window, Slide *slide)
		: m_window(window), m_slide(slide), redoCount(0)
		{
			setText(QString(qApp->translate("SlideEditorWindow","Removed Slide# %1")).arg(slide->slideNumber()+1));
			//qDebug() << "UndoSlideRemoved::(): New cmd because you deleted slide#"<<m_slide->slideNumber();
		}


	virtual void undo()
	{
		m_window->ignoreUndoChanged(true);
		m_window->slideGroup()->addSlide(m_slide);
		m_window->setCurrentSlide(m_slide);
		m_window->ignoreUndoChanged(false);
	}
	virtual void redo()
	{
 		if(redoCount++ > 0)
 		{
			m_window->ignoreUndoChanged(true);
			m_window->slideGroup()->removeSlide(m_slide);
			m_window->ignoreUndoChanged(false);
		}
	}
private:
	SlideEditorWindow *m_window;
	Slide *m_slide;
	int redoCount;
};


void SlideEditorWindow::ignoreUndoChanged(bool flag)
{
	m_ignoreUndoPropChanges = flag;
}


void SlideEditorWindow::slideChanged(Slide *slide, QString slideOperation, AbstractItem */*item*/, QString /*itemOperation*/, QString /*fieldName*/, QVariant /*value*/)
{
	if(slideOperation == "remove")
	{
		//qDebug()<<"SlideEditorWindow::slideChanged: (remove), disconnecting from slide#"<<slide->slideNumber();
		disconnect(slide,0,this,0);
		if(!m_ignoreUndoPropChanges)
		{
			m_undoStack->push(new UndoSlideRemoved(this,slide));
		}
	}
	else
	if(slideOperation == "add")
	{
		//qDebug()<<"SlideEditorWindow::slideChanged: (add), connecting to slide#"<<slide->slideNumber();
		// so we dont duplicate events in case we vet this signal twice
		disconnect(slide,0,this,0);
		connect(slide,SIGNAL(slideItemChanged(AbstractItem *, QString, QString, QVariant, QVariant)),this,SLOT(slideItemChanged(AbstractItem *, QString, QString, QVariant, QVariant)));

		if(m_ignoreUndoPropChanges)
		{
			QModelIndex idx = m_slideModel->indexForSlide(slide);
			m_slideListView->setCurrentIndex(idx);
		}
		else
		{
			m_undoStack->push(new UndoSlideAdded(this,slide));
		}
	}
	else
	if(slideOperation == "change")
	{
		// "change" would be an add/remove/change of an AbstractItem to the slide itself.
		// This will be procssed in slideItemChanged().
	}
}


void SlideEditorWindow::slideItemChanged(AbstractItem *item, QString operation, QString fieldName, QVariant value, QVariant oldValue)
{
// 	Slide * slide = dynamic_cast<Slide *>(sender());

	if(operation == "add")
	{
		if(!m_ignoreUndoPropChanges && ! dynamic_cast<BackgroundItem*>(item))
		{
			m_undoStack->push(new UndoSlideItemAdded(this,m_scene->slide(),item));
		}
	}
	else
	if(operation == "remove")
	{
		if(!m_ignoreUndoPropChanges)
		{
			m_undoStack->push(new UndoSlideItemRemoved(this,m_scene->slide(),item));
		}
	}
	else
	if(operation == "change")
	{
		if(item)
		{
			if(!m_ignoreUndoPropChanges)
			{
				if(value != oldValue)
				{
					QUndoCommand * changeCmd = new UndoSlideItemChanged(this,item,fieldName,value,oldValue);
					m_undoStack->push(changeCmd);

					//qDebug() << "SlideEditorWindow::slideItemChanged: New Cmd for "<<item->itemName()<<", field:"<<fieldName<<", oldValue:"<<oldValue<<", newValue:"<<value;
				}
				else
				{
					//qDebug() << "SlideEditorWindow::slideItemChanged: IGNORING Cmd for "<<item->itemName()<<", field:"<<fieldName<<", oldValue:"<<oldValue<<", newValue:"<<value;
				}
			}
			else
			{
				//qDebug() << "SlideEditorWindow::slideItemChanged: Ignoring prop change on "<<item->itemName();
			}
		}
		else
		{
			//qDebug() << "SlideEditorWindow::slideItemChanged: Item was null, no undo data recorded.";
		}

	}


}



void SlideEditorWindow::releaseSlideGroup()
{
	if(!m_slideGroup)
		return;

	disconnect(m_slideGroup,0,this,0);
	m_slideGroup = 0;

}


void SlideEditorWindow::slideSelected(const QModelIndex &idx)
{
	Slide *s = m_slideModel->slideFromIndex(idx);
	//qDebug() << "SlideEditorWindow::slideSelected(): selected slide#:"<<s->slideNumber();
	if(m_scene->slide() != s)
		setCurrentSlide(s);
}


void SlideEditorWindow::itemSelected(const QModelIndex &idx)
{

	AbstractItem * item = m_itemModel->itemFromIndex(idx);
	//qDebug() << "SlideEditorWindow::itemSelected(): selected item:"<<item->itemName();
	AbstractContent * content = m_scene->findVisualDelegate(item);
	if(content && !content->isSelected())
	{
		m_scene->clearSelection();
		content->setSelected(true);
	}
}


void SlideEditorWindow::newSlide()
{
	Slide * slide = new Slide();
	slide->setSlideNumber(m_slideGroup->numSlides());
	slide->setSlideId(m_slideGroup->numSlides());

	//qDebug() << "newSlide: ADDING "<<slide->slideNumber();
	m_slideGroup->addSlide(slide);
	//qDebug() << "newSlide: Added slide#"<<slide->slideNumber();

	setCurrentSlide(slide);
}

void SlideEditorWindow::dupSlide()
{
	Slide * oldSlide = m_scene->slide();
	Slide * slide = oldSlide->clone();

	slide->setSlideNumber(oldSlide->slideNumber() + 1);
	slide->setSlideId(m_slideGroup->numSlides());

	int oldIdx = m_slideGroup->indexOf(oldSlide);
	if(oldIdx > -1)
		for(int i=oldIdx + 1; i<m_slideGroup->numSlides(); i++)
			m_slideGroup->at(i)->setSlideNumber(i+1);

	m_slideGroup->addSlide(slide);

	setCurrentSlide(slide);
}

void SlideEditorWindow::slideProperties()
{
	Slide * slide = m_scene->slide();

	SlideSettingsDialog d(slide,this);
	//d.adjustSize();
	d.resize(400, 400);
	d.exec();
}

void SlideEditorWindow::groupProperties()
{
	SlideGroupSettingsDialog d(m_slideGroup,this);
	d.adjustSize();
	d.exec();
}

void SlideEditorWindow::delSlide()
{
	Slide * slide = m_scene->slide();
	//qDebug() << "delSlide: Removing slide#"<<slide->slideNumber();

	QModelIndex idx = m_slideModel->indexForSlide(slide);
	QModelIndex prev = m_slideModel->indexForRow(idx.row() - 1 > 0 ? idx.row() - 1 : 0);

	m_slideGroup->removeSlide(slide);
	//m_undoStack->push(new UndoSlideRemoved(this,slide));

	Slide *newSlide = 0;
	if(prev.isValid())
	{
		m_slideListView->setCurrentIndex(prev);
		newSlide = m_slideModel->slideAt(prev.row());
	}

// 	if(newSlide)
// 	{
// 		m_scene->setSlide(newSlide);
// 		setupViewportLines();
// 	}

	QList<Slide*> slides = m_slideGroup->slideList();
	int counter = 0;
	foreach(Slide *s, slides)
	{
		s->setSlideNumber(counter++);
	}

	setCurrentSlide(newSlide);

	// Dont delete here, delete when UndoSlideRemoved command is destroyed
	// change: go ahead, delete - we clone slide above
	// nope, try not to now
	//delete slide;
	//slide = 0;
}

// TODO shouldn't need the "m_itemModel->setSlide(slide)" calls!!!!
void SlideEditorWindow::newTextItem()
{
	m_scene->newTextItem();
	m_itemModel->setSlide(m_scene->slide());
}

void SlideEditorWindow::newBoxItem()
{
        m_scene->newBoxItem();
        m_itemModel->setSlide(m_scene->slide());
}

void SlideEditorWindow::newVideoItem()
{
        m_scene->newVideoItem();
        m_itemModel->setSlide(m_scene->slide());
}

void SlideEditorWindow::newImageItem()
{
        m_scene->newImageItem();
        m_itemModel->setSlide(m_scene->slide());
}

void SlideEditorWindow::newOutputView()
{
        m_scene->newOutputView();
        m_itemModel->setSlide(m_scene->slide());
}

