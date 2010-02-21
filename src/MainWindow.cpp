#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QSettings>
#include <QFile>
#include <QDebug>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextEdit>
#include <QCheckBox>
#include <QFile>
#include <QTextStream>

#include "DeepProgressIndicator.h"
#include "AppSettings.h"
#include "AppSettingsDialog.h"
#include "DocumentSettingsDialog.h"
#include "SlideGroupSettingsDialog.h"
#include "model/Output.h"
#include "model/Slide.h"
#include "model/TextBoxItem.h"

#include "model/Document.h"
#include "DocumentListModel.h"
#include "SlideEditorWindow.h"
#include "SlideGroupViewer.h"
#include "OutputViewer.h"
#include "OutputSetupDialog.h"
#include "ImageImportDialog.h"
#include "ImportGroupDialog.h"

#include "groupplayer/GroupPlayerSlideGroup.h"

#include "model/ItemFactory.h"
#include "model/SlideGroupFactory.h"

#include "ppt/PPTSlideGroup.h"
#include "phonon/VideoSlideGroup.h"

#include "songdb/SongSlideGroup.h"
#include "songdb/SongRecord.h"
#include "songdb/SongBrowser.h"
#include "bible/BibleBrowser.h"

#include "MediaBrowser.h"

#include "OutputInstance.h"
#include "OutputControl.h"

#include "http/ControlServer.h"

MainWindow * MainWindow::static_mainWindow = 0;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::MainWindow),
	m_liveView(0),
	m_docModel(0),
	m_doc(0),
	m_viewControl(0),
	m_editWin(0),
	m_autosaveTimer(0),
	m_controlServer(0)

{
	static_mainWindow = this;

	m_autosaveTimer = new QTimer(this);
	connect(m_autosaveTimer, SIGNAL(timeout()), this, SLOT(autosave()));

	setWindowIcon(QIcon(":/data/icon-d.png"));

	m_ui->setupUi(this);

	m_docModel = new DocumentListModel();

	QString lastOpenFile = AppSettings::previousPath("last-dviz-file");

	if(!openFile(lastOpenFile))
		actionNew();

	// init the editor win AFTER load/new so that editor win starts out with the correct aspect ratio
	m_editWin = new SlideEditorWindow();

	setupOutputViews();
	setupHotkeys();
	setupCentralWidget();
	setupSongList();
	setupBibleBrowser();
	setupMediaBrowser();
	setupSlideLibrary();

	// Restore state
	loadWindowState();



	//m_ui->actionEdit_Slide_Group->setEnabled(false);
	QList<QAction*> actionList;

	m_ui->actionOpen->setIcon(QIcon(":data/stock-open.png"));
	actionList << m_ui->actionOpen;
	m_ui->actionOpen->setToolTip(m_ui->actionOpen->text() + "(" + m_ui->actionOpen->shortcut().toString() + ")");
	connect(m_ui->actionOpen, SIGNAL(triggered()), this, SLOT(actionOpen()));

	m_ui->actionSave->setIcon(QIcon(":data/stock-save.png"));
	actionList << m_ui->actionSave;
	connect(m_ui->actionSave, SIGNAL(triggered()), this, SLOT(actionSave()));

	m_ui->actionSave_As->setIcon(QIcon(":data/stock-save.png"));
	connect(m_ui->actionSave_As, SIGNAL(triggered()), this, SLOT(actionSaveAs()));
	actionList << m_ui->actionSave_As;

	m_ui->actionNew->setIcon(QIcon(":data/stock-new.png"));
	connect(m_ui->actionNew, SIGNAL(triggered()), this, SLOT(actionNew()));
	actionList << m_ui->actionNew;

	m_ui->actionNew_Slide_Group->setIcon(QIcon(":data/stock-add.png"));
	connect(m_ui->actionNew_Slide_Group, SIGNAL(triggered()), this, SLOT(actionNewGroup()));
	actionList << m_ui->actionNew_Slide_Group;

	m_ui->actionEdit_Slide_Group->setIcon(QIcon(":data/stock-edit.png"));
	m_ui->actionEdit_Slide_Group->setEnabled(false);
	connect(m_ui->actionEdit_Slide_Group, SIGNAL(triggered()), this, SLOT(actionEditGroup()));
	actionList << m_ui->actionEdit_Slide_Group;

	m_ui->actionSlide_Group_Properties->setIcon(QIcon(":data/stock-properties.png"));
	m_ui->actionSlide_Group_Properties->setEnabled(false);
	connect(m_ui->actionSlide_Group_Properties, SIGNAL(triggered()), this, SLOT(actionGroupProperties()));
	actionList << m_ui->actionSlide_Group_Properties;

	m_ui->actionDelete_Slide_Group->setIcon(QIcon(":data/stock-delete.png"));
	m_ui->actionDelete_Slide_Group->setEnabled(false);
	connect(m_ui->actionDelete_Slide_Group, SIGNAL(triggered()), this, SLOT(actionDelGroup()));
	actionList << m_ui->actionDelete_Slide_Group;


	m_ui->actionApp_Settings->setIcon(QIcon(":data/stock-preferences.png"));
	connect(m_ui->actionApp_Settings, SIGNAL(triggered()), this, SLOT(actionAppSettingsDialog()));
	actionList << m_ui->actionApp_Settings;

	m_ui->actionDoc_Settings->setIcon(QIcon(":data/stock-properties.png"));
	connect(m_ui->actionDoc_Settings, SIGNAL(triggered()), this, SLOT(actionDocSettingsDialog()));
	actionList << m_ui->actionDoc_Settings;

	connect(m_ui->actionAbout_DViz, SIGNAL(triggered()), this, SLOT(actionAboutDviz()));
	connect(m_ui->actionVisit_DViz_Website, SIGNAL(triggered()), this, SLOT(actionDvizWebsite()));

	m_ui->actionImages_Import_Tool->setIcon(QIcon(":data/insert-image-24.png"));
	connect(m_ui->actionImages_Import_Tool, SIGNAL(triggered()), this, SLOT(imageImportTool()));

	m_ui->actionText_Import_Tool->setIcon(QIcon(":data/insert-text-24.png"));
	connect(m_ui->actionText_Import_Tool, SIGNAL(triggered()), this, SLOT(textImportTool()));
	
	//m_ui->actionImport_Slide_Group->setIcon(QIcon(":data/insert-text-24.png"));
	connect(m_ui->actionImport_Slide_Group, SIGNAL(triggered()), this, SLOT(importSlideGroup()));
	
	connect(m_ui->actionAdd_Group_Player, SIGNAL(triggered()), this, SLOT(addGroupPlayer()));

	connect(m_ui->actionToggle_Live_Output, SIGNAL(triggered()), this, SLOT(actionToggleLiveOutput()));


	#ifdef WIN32_PPT_ENABLED
		m_ui->actionAdd_PowerPoint_File->setIcon(QIcon(":data/insert-ppt-24.png"));
		connect(m_ui->actionAdd_PowerPoint_File, SIGNAL(triggered()), this, SLOT(actionAddPPT()));
	#else
		m_ui->actionAdd_PowerPoint_File->setVisible(false);
	#endif
	
	connect(m_ui->actionAdd_Video_File, SIGNAL(triggered()), this, SLOT(actionAddVideo()));


	foreach(QAction *action, actionList)
	{
		QString shortcut = action->shortcut().toString();
		if(!shortcut.trimmed().isEmpty())
		{
			QString text = action->text();
			text.replace("&","");
			action->setText(QString("%1 (%2)").arg(text).arg(shortcut));
		}
	}

	//connect(m_groupView, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(slotListContextMenu(const QPoint &)));

	connect(m_ui->actionExit,SIGNAL(triggered()), this, SLOT(close()));

	if(AppSettings::httpControlEnabled())
		m_controlServer = new ControlServer(AppSettings::httpControlPort(),this);
}


MainWindow::~MainWindow()
{
	// done in close event now
	//actionSave();

	clearAllOutputs();

	foreach(OutputInstance *inst, m_outputInstances)
		delete inst;

	delete m_previewInstance;

	m_outputInstances.clear();

	delete m_ui;

	//delete m_liveView;
	//m_liveView = 0;

	delete m_docModel;
	delete m_doc;
	delete m_editWin;
}


void MainWindow::autosave()
{
	//qDebug() << "Spawning autosave...";
	if(!m_doc->filename().isEmpty())
	{
		statusBar()->showMessage(tr("Auto-saving document..."));
		DocumentSaveThread * thread = new DocumentSaveThread(m_doc);
		thread->start();
		connect(thread, SIGNAL(finished()), this, SLOT(saveFinished()));
	}
	//qDebug() << "Autosave started.";
}

void MainWindow::saveFinished()
{
	statusBar()->showMessage(QString(tr("Document saved as %1.")).arg(m_doc->filename()),1500);
}

void MainWindow::actionToggleLiveOutput()
{
	Output * live = AppSettings::taggedOutput("live");
	live->setIsEnabled(!live->isEnabled());

	emit appSettingsChanged();
	setupOutputViews();
	setupOutputList();
	setupOutputControls();
}

void MainWindow::actionAddPPT()
{
	QString lastDir = AppSettings::previousPath("ppt");
	QString file = QFileDialog::getOpenFileName(this,"Add PowerPoint File",lastDir,"PowerPoint Files (*.ppt *.pptx)");
	if(!file.isEmpty())
	{
		AppSettings::setPreviousPath("ppt",file);

		PPTSlideGroup * group = new PPTSlideGroup();
		group->setFile(file);

		m_doc->addGroup(group);
		//if(!liveInst()->slideGroup())
		//	setLiveGroup(group);
		QModelIndex idx = m_docModel->indexForGroup(group);
		m_groupView->setCurrentIndex(idx);
	}
}

void MainWindow::actionAddVideo()
{
	QString lastDir = AppSettings::previousPath("video");
	QString file = QFileDialog::getOpenFileName(this,"Add Video File",lastDir,tr("Video Files (*.wmv *.mpeg *.mpg *.avi *.wmv *.flv *.mov *.mp4 *.m4a *.3gp *.3g2 *.mj2 *.mjpeg *.ipod *.m4v *.gsm *.swf *.dv *.dvd *.asf *.mtv *.roq *.aac *.ac3 *.aiff *.alaw *.iif);;Any File (*.*)"));
	if(!file.isEmpty())
	{
		AppSettings::setPreviousPath("video",file);

		VideoSlideGroup * group = new VideoSlideGroup();
		group->setFile(file);

		m_doc->addGroup(group);
		//if(!liveInst()->slideGroup())
		//	setLiveGroup(group);
		QModelIndex idx = m_docModel->indexForGroup(group);
		m_groupView->setCurrentIndex(idx);
	}
}

void MainWindow::showEvent(QShowEvent *evt)
{
	evt->accept();
	//qDebug()<< "MainWindow::showEvent: pos:"<<pos()<<", size:"<<size();

	emit appSettingsChanged();

	if(AppSettings::autosaveTime() > 0)
		m_autosaveTimer->start(AppSettings::autosaveTime() * 1000);

	raise(); // raise above output instances
}

OutputInstance * MainWindow::liveInst() { return outputInst(AppSettings::taggedOutput("live")->id()); }
SlideGroupViewControl * MainWindow::liveCtrl() { return viewControl(AppSettings::taggedOutput("live")->id()); }

void MainWindow::loadWindowState()
{
	QDesktopWidget *d = QApplication::desktop();
	QRect primary = d->screenGeometry(d->primaryScreen());

	QSettings settings;
	QSize sz = settings.value("mainwindow/size").toSize();
	if(sz.isValid())
		resize(sz);

	QPoint p = settings.value("mainwindow/pos").toPoint();
	if(p.isNull())
		p = primary.topLeft();
	if(p.y() < 0)
		p.setY(1);
	move(p);
	restoreState(settings.value("mainwindow/state").toByteArray());
	m_splitter->restoreState(settings.value("mainwindow/splitter_state").toByteArray());
	m_splitter2->restoreState(settings.value("mainwindow/grouplistsplitter_state").toByteArray());
	m_mediaBrowser->restoreState(settings.value("mainwindow/mediabrowser_state").toByteArray());
	m_songBrowser->restoreState(settings.value("mainwindow/songbrowser_state").toByteArray());

}

void MainWindow::saveWindowState()
{
	QSettings settings;
	settings.setValue("mainwindow/size",size());
	settings.setValue("mainwindow/pos",pos());
	settings.setValue("mainwindow/state",saveState());
	settings.setValue("mainwindow/splitter_state",m_splitter->saveState());
	settings.setValue("mainwindow/grouplistsplitter_state",m_splitter2->saveState());
	settings.setValue("mainwindow/mediabrowser_state",m_mediaBrowser->saveState());
	settings.setValue("mainwindow/songbrowser_state",m_songBrowser->saveState());
}

void MainWindow::clearAllOutputs()
{
// 	m_liveView->clear();
	//m_previewWidget->clear();
	//qDebug() << "MainWindow::clearAllOutputs: Releasing preview slides\n";

	foreach(SlideGroupViewControl *ctrl, m_viewControls)
		ctrl->releaseSlideGroup();
}

void MainWindow::actionOpen()
{
	QString curFile = m_doc->filename();
	if(curFile.trimmed().isEmpty())
		curFile = AppSettings::previousPath("last-dviz-file");

	QString fileName = QFileDialog::getOpenFileName(this, tr("Select DViz File"), curFile, tr("DViz File (*.dviz *.dvz);;DViz XML File (*.dvizx *.dvx *.xml);;Any File (*.*)"));
	if(fileName != "")
	{
		AppSettings::setPreviousPath("last-dviz-file",fileName);
		if(openFile(fileName))
		{
			return;
		}
		else
		{
			QMessageBox::critical(this,tr("File Does Not Exist"),tr("Sorry, but the file you chose does not exist. Please try again."));
		}
	}
}

bool MainWindow::actionSave()
{
	if(m_doc->filename().isEmpty())
		return actionSaveAs();
	else
		saveFile();
	return true;

}

bool MainWindow::actionSaveAs()
{
	QString curFile = m_doc->filename();
	if(curFile.trimmed().isEmpty())
		curFile = AppSettings::previousPath("last-dviz-file");

	QString fileName = QFileDialog::getSaveFileName(this, tr("Choose a Filename"), curFile, tr("DViz File (*.dviz *.dvz);;DViz XML File (*.dvizx *.dvx *.xml);;Any File (*.*)"));
	if(fileName != "")
	{
		QFileInfo info(fileName);
		//if(info.suffix().isEmpty())
			//fileName += ".dviz";
		AppSettings::setPreviousPath("last-dviz-file",fileName);
		saveFile(fileName);
		return true;
	}

	return false;
}

void MainWindow::actionNew()
{
	if(m_doc)
	{
		clearAllOutputs();

		m_docModel->releaseDocument();
		// Not deleting because of not deleting in open file
		//delete m_doc;
	}

	m_doc = new Document();

	Slide * slide = new Slide();
	SlideGroup *g = new SlideGroup();
	g->addSlide(slide);
	m_doc->addGroup(g);

	m_docModel->setDocument(m_doc);

	setWindowTitle(tr("DViz - New File"));
}




void MainWindow::imageImportTool()
{
	if(!m_doc)
		return;
	ImageImportDialog d(m_doc);
	d.exec();
}


void MainWindow::importSlideGroup()
{
	if(!m_doc)
		return;
	ImportGroupDialog d;
	d.exec();
}

void MainWindow::addGroupPlayer()
{
	SlideGroup *g = new GroupPlayerSlideGroup();
	m_doc->addGroup(g);
	
	openSlideEditor(g);
}


void MainWindow::textImportTool()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Text File"),
		AppSettings::previousPath("text"),
		tr("Text Files (*.txt);;Any File (*.*)")
	);


	if(!fileName.isEmpty())
	{
		int MinTextSize = 48;

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

		QSize fitSize = standardSceneRect().size();

		SlideGroup *group = new SlideGroup();
		group->setGroupTitle(AbstractItem::guessTitle(QFileInfo(fileName).baseName()));

		int slideNum = 0;

		QString blockPrefix = "<span style='font-family:Calibri,Tahoma,Arial,Sans-Serif;font-weight:800'><b>";
		QString blockSuffix = "</b></span>";

		TextBoxItem * tmpText = 0;
		QStringList tmpList;
		for(int x=0; x<lines.size(); x++)
		{
			if(tmpList.isEmpty() &&
			  lines[x].trimmed().isEmpty())
				continue;

			tmpList.append(lines[x]);

			if(!tmpText)
			{
				tmpText = new TextBoxItem();
				tmpText->setItemId(ItemFactory::nextId());
				tmpText->setItemName(QString("TextBoxItem%1").arg(tmpText->itemId()));
			}

			tmpText->setText(QString("%1%2%3")
					    .arg(blockPrefix)
					    .arg(tmpList.join("\n"))
					    .arg(blockSuffix));

			int realHeight = tmpText->fitToSize(fitSize,MinTextSize);
			if(realHeight < 0)
			{
				if(tmpList.size() > 1)
				{
					// return last line to the file buffer
					lines.prepend(tmpList.takeFirst());

					tmpText->setText(QString("%1%2%3")
							    .arg(blockPrefix)
							    .arg(tmpList.join("\n"))
							    .arg(blockSuffix));
					realHeight = tmpText->fitToSize(fitSize,MinTextSize);
				}

				Slide * slide = new Slide();
				AbstractVisualItem * bg = dynamic_cast<AbstractVisualItem*>(slide->background());

				qDebug() << "Slide "<<slideNum<<": [\n"<<tmpList.join("\n")<<"\n]";;

				bg->setFillType(AbstractVisualItem::Solid);
				bg->setFillBrush(Qt::blue);

				// Center text on screen
				qreal y = fitSize.height()/2 - realHeight/2;
				//qDebug() << "SongSlideGroup::textToSlides(): centering: boxHeight:"<<boxHeight<<", textRect height:"<<textRect.height()<<", centered Y:"<<y;
				tmpText->setContentsRect(QRectF(0,y,fitSize.width(),realHeight));

				// Outline pen for the text
				QPen pen = QPen(Qt::black,1.5);
				pen.setJoinStyle(Qt::MiterJoin);

				tmpText->setPos(QPointF(0,0));
				tmpText->setOutlinePen(pen);
				tmpText->setOutlineEnabled(true);
				tmpText->setFillBrush(Qt::white);
				tmpText->setFillType(AbstractVisualItem::Solid);
				tmpText->setShadowEnabled(true);
				tmpText->setShadowBlurRadius(6);

				slide->addItem(tmpText);
				tmpText = 0;

				slide->setSlideNumber(slideNum);
				group->addSlide(slide);
				slideNum++;

				tmpList.clear();



			}
		}

		m_doc->addGroup(group);
	}
}

bool MainWindow::openFile(const QString & file)
{
	if(!QFile(file).exists())
		return false;

	double oldAspect = 0;

	if(m_doc)
	{
		oldAspect = m_doc->aspectRatio();
		if(!m_doc->filename().isEmpty())
			m_doc->save();

		clearAllOutputs();

		m_docModel->releaseDocument();

		// HACK this causes segflt if deleted - WHY???
		//delete m_doc;
	}

	m_doc = new Document(file);

	AppSettings::setPreviousPath("last-dviz-file",file);

	//m_doc->load(file);
	//r.readSlide(m_slide);
	QString fileName = QFileInfo(file).fileName();
	setWindowTitle(QString(tr("DViz - %1")).arg(fileName));

	//qDebug()<<"MainWindow::open(): "<<file<<", oldAspect:"<<oldAspect<<", new:"<<m_doc->aspectRatio();
	if(oldAspect != m_doc->aspectRatio())
	{
		//qDebug()<<"MainWindow::open(): emitting aspectRatioChanged()";
		emit aspectRatioChanged(m_doc->aspectRatio());
	}

// 	qDebug() << "MainWindow::open(): m_docModel->setDocument() - start";
// 	printf("MainWindow::open(): m_docModel ptr: %p\n",m_docModel);
	DeepProgressIndicator *d = new DeepProgressIndicator(m_docModel,this);
	d->setText(QString(tr("Loading %1...")).arg(fileName));
	d->setTitle(QString(tr("Loading %1")).arg(fileName));
	d->setSize(m_doc->numGroups());

	m_docModel->setDocument(m_doc);

	d->close();
//	qDebug() << "MainWindow::open(): m_docModel->setDocument() - end";

	return true;
}

void MainWindow::saveFile(const QString & file)
{
	m_doc->save(file.isEmpty() ? m_doc->filename() : file);

	setWindowTitle(QString(tr("DViz - %1")).arg(QFileInfo(m_doc->filename()).fileName()));

	saveWindowState();

}

void MainWindow::setupSongList()
{
	QVBoxLayout * baseLayout = new QVBoxLayout(m_ui->tabSongs);
	baseLayout->setContentsMargins(0,0,0,0);

	m_songBrowser = new SongBrowser(m_ui->tabSongs);
	baseLayout->addWidget(m_songBrowser);

	connect(m_songBrowser, SIGNAL(songSelected(SongRecord*)), this, SLOT(songSelected(SongRecord*)));
}
	
void MainWindow::setupBibleBrowser()
{
	QVBoxLayout * baseLayout = new QVBoxLayout(m_ui->tabBible);
	baseLayout->setContentsMargins(0,0,0,0);

	BibleBrowser * bible = new BibleBrowser(m_ui->tabBible);
	baseLayout->addWidget(bible);
}

void MainWindow::setupSlideLibrary()
{
	QVBoxLayout * baseLayout = new QVBoxLayout(m_ui->tabSlides);
	baseLayout->setContentsMargins(0,0,0,0);

// 	SlideLibraryBrowser * browser = new SlideLibraryBrowser(m_ui->tabSlides);
// 	baseLayout->addWidget(browser);
}

void MainWindow::songSelected(SongRecord *song)
{
	SongSlideGroup *group = new SongSlideGroup();
	group->setSong(song);
	m_doc->addGroup(group);
	if(!liveInst()->slideGroup())
		setLiveGroup(group);
	QModelIndex idx = m_docModel->indexForGroup(group);
	m_groupView->setCurrentIndex(idx);
}


void MainWindow::setupMediaBrowser()
{
	QVBoxLayout * mediaBrowserLayout = new QVBoxLayout(m_ui->tabMedia);
	mediaBrowserLayout->setContentsMargins(0,0,0,0);

	m_mediaBrowser = new MediaBrowser("",this);
	mediaBrowserLayout->addWidget(m_mediaBrowser);

	foreach(OutputInstance *inst, m_outputInstances)
		connect(m_mediaBrowser, SIGNAL(setLiveBackground(const QFileInfo&, bool)), inst, SLOT(setLiveBackground(const QFileInfo&, bool)));

	connect(m_mediaBrowser, SIGNAL(setSelectedBackground(const QFileInfo&)), this, SLOT(setSelectedBackground(const QFileInfo&)));
	connect(m_mediaBrowser, SIGNAL(fileSelected(const QFileInfo&)), this, SLOT(fileSelected(const QFileInfo&)));

}

void MainWindow::fileSelected(const QFileInfo &info)
{
	if(!SlideGroup::canUseBackground(info))
	{
		QMessageBox::warning(this,tr("Unknown File Type"),tr("I'm not sure how to handle that file. Sorry!"));
		return;
	}

	Slide * slide = new Slide();
	SlideGroup *group = new SlideGroup();
	group->addSlide(slide);

	group->changeBackground(info);

	m_doc->addGroup(group);
	if(!liveInst()->slideGroup())
		setLiveGroup(group);
	QModelIndex idx = m_docModel->indexForGroup(group);
	m_groupView->setCurrentIndex(idx);
}

void MainWindow::setSelectedBackground(const QFileInfo &info)
{
	QModelIndex idx = m_groupView->currentIndex();
	if(idx.isValid())
	{
		if(!SlideGroup::canUseBackground(info))
		{
			QMessageBox::warning(this,tr("Unknown File Type"),tr("I'm not sure how to handle that file. Sorry!"));
			return;
		}

		SlideGroup *group = m_docModel->groupFromIndex(idx);
		if(group)
		{
			group->changeBackground(info);
			//qDebug() << "Finished setting "<<abs<<" as background on group "<<group->groupTitle();
		}
	}
	else
	{
		QMessageBox::warning(this,tr("No Slide Group Selected"),tr("First, click a group in the list THEN click the 'Set Selected Background' button in the media browser!"));
	}
}

void MainWindow::setupOutputViews()
{
	QList<Output*> allOut = AppSettings::outputs();

	foreach(Output *out, allOut)
	{
		if(!m_outputInstances.contains(out->id()))
		{
			OutputInstance *inst = new OutputInstance(out);
			connect(inst, SIGNAL(nextGroup()), this, SLOT(nextGroup()));
			connect(inst, SIGNAL(jumpToGroup(int)), this, SLOT(jumpToGroup(int)));

			m_outputInstances[out->id()] = inst;
		}
	}
}

bool MainWindow::isTransitionActive()
{
	foreach(OutputInstance *inst, m_outputInstances)
		if(inst->isTransitionActive())
			return true;
	return false;
}

void MainWindow::groupsDropped(QList<SlideGroup*> list)
{
	QModelIndex idx = m_docModel->indexForGroup(list.first());
	m_groupView->setCurrentIndex(idx);
}


void MainWindow::setupCentralWidget()
{

	m_splitter = new QSplitter(this);
	//m_splitter->setOrientation(Qt::Vertical);
	setCentralWidget(m_splitter);

	// left side
	m_splitter2 = new QSplitter(m_splitter);
	m_splitter2->setOrientation(Qt::Vertical);

	QWidget * leftBase = new QWidget(this);
	QVBoxLayout * leftLayout = new QVBoxLayout(leftBase);
	leftLayout->setMargin(0);


	QWidget * ouputChoiceBase = new QWidget(leftBase);
	QHBoxLayout * ouputChoiceBaseLayout = new QHBoxLayout(ouputChoiceBase);
	ouputChoiceBaseLayout->setMargin(0);

	ouputChoiceBaseLayout->addStretch(1);

	// Output checkbox list
	m_outputCheckboxBase = new QWidget(ouputChoiceBase);
	QHBoxLayout * outputCheckboxBaseLayout = new QHBoxLayout(m_outputCheckboxBase);
	outputCheckboxBaseLayout->setMargin(0);
	ouputChoiceBaseLayout->addWidget(m_outputCheckboxBase);

	setupOutputList();

	// Send to output button
	m_btnSendOut = new QPushButton(QIcon(":/data/stock-fullscreen.png"),tr("Send to Output"),ouputChoiceBase);
	ouputChoiceBaseLayout->addWidget(m_btnSendOut);
	connect(m_btnSendOut, SIGNAL(clicked()), this, SLOT(slotSendToOutputs()));

	leftLayout->addWidget(ouputChoiceBase);
	leftLayout->setSpacing(2);

	// List of groups
	m_groupView = new QListView(leftBase);

	m_groupView->setViewMode(QListView::ListMode);
	//m_groupView->setViewMode(QListView::IconMode);
	m_groupView->setMovement(QListView::Free);
	m_groupView->setWordWrap(true);
	m_groupView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_groupView->setDragEnabled(true);
	m_groupView->setAcceptDrops(true);
	m_groupView->setDropIndicatorShown(true);
	m_groupView->setEditTriggers(QAbstractItemView::EditKeyPressed);

	m_groupView->setModel(m_docModel);
	m_groupView->setContextMenuPolicy(Qt::ActionsContextMenu);
	m_groupView->insertAction(0,m_ui->actionEdit_Slide_Group);
	m_groupView->insertAction(0,m_ui->actionSlide_Group_Properties);
	m_groupView->insertAction(0,m_ui->actionNew_Slide_Group);
	m_groupView->insertAction(0,m_ui->actionDelete_Slide_Group);

	leftLayout->addWidget(m_groupView);

	connect(m_docModel, SIGNAL(groupsDropped(QList<SlideGroup*>)), this, SLOT(groupsDropped(QList<SlideGroup*>)));

	connect(m_groupView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(groupSelected(const QModelIndex &)));
	connect(m_groupView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(groupDoubleClicked(const QModelIndex &)));

	m_splitter2->addWidget(leftBase);

	//m_splitter3 = new QSplitter(m_splitter2);
	//m_splitter3->setOrientation(Qt::Vertical);

	// preview widget
	//m_previewControlBase
	// TODO Later

	m_previewInstance = new OutputInstance(Output::previewInstance());
	m_previewInstance->setCanZoom(true);
	m_previewInstance->setSceneContextHint(MyGraphicsScene::Preview);

	m_previewControlBase = new QWidget(this);
	QVBoxLayout * leftLayout3 = new QVBoxLayout(m_previewControlBase);
	leftLayout3->setMargin(0);

	m_previewControl = new SlideGroupViewControl(0,this);
	m_previewControl->setIsPreviewControl(true);
	m_previewControl->setOutputView(m_previewInstance);
	connect(m_previewControl, SIGNAL(slideDoubleClicked(Slide *)), this, SLOT(previewSlideDoubleClicked(Slide *)));

	leftLayout3->addWidget(m_previewControl);

	m_splitter2->addWidget(m_previewControlBase);
	m_splitter2->addWidget(m_previewInstance);

	//m_splitter2->addWidget(m_splitter3);

	m_splitter->addWidget(m_splitter2);




	// live view control on right side of splitter
	m_outputTabs = new QTabWidget();

	setupOutputControls();
	m_splitter->addWidget(m_outputTabs);




}

void MainWindow::slotSendToOutputs()
{
	QModelIndex idx = m_groupView->currentIndex();
	if(idx.isValid())
		groupDoubleClicked(idx);
}

void MainWindow::setupOutputList()
{
	foreach(QCheckBox *box, m_outputCheckboxes)
	{
		if(box)
		{
			box->setVisible(false);
			m_outputCheckboxBase->layout()->removeWidget(box);
			delete box;
		}
	}

	m_outputCheckboxes.clear();

	foreach(Output*out, AppSettings::outputs())
	{
		if(out->isEnabled())
		{
			QCheckBox *box = new QCheckBox(out->name());
			m_outputCheckboxes[out->id()] = box;

			box->setChecked(true);
			m_outputCheckboxBase->layout()->addWidget(box);
		}
	}
}


void MainWindow::actionAboutDviz()
{
	QString ver = "";

#ifdef VER
	ver = QString(", Ver %1").arg(VER);
#endif

	QMessageBox msgBox(QMessageBox::NoIcon,tr("About DViz"),QString(tr("DViz%1\n\"Tasty Breakfast\" Release\n(c) 2009 Josiah Bryan")).arg(ver));
	msgBox.exec();
}

void MainWindow::actionDvizWebsite()
{
	QMessageBox msgBox(QMessageBox::Information,tr("Visit DViz Website"),tr("For more info, go to http://code.google.com/p/dviz"));
	msgBox.exec();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	m_autosaveTimer->stop();

	if(m_doc->filename().isEmpty())
	{
		switch(QMessageBox::question(this,tr("File Not Saved"),tr("This file has not yet been saved - do you want to give it a file name? Click 'Save' to save the document to a file, click 'Discard' to continue closing and loose all changes, or click 'Cancel' to cancel closing and return to the program."),
			QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel))
		{
			case QMessageBox::Save:
				if(!actionSaveAs())
					event->ignore();
				else
				{
					event->accept();
					//m_liveView->hide();
					foreach(OutputInstance *inst, m_outputInstances)
						inst->hide();
				}
				return;
			case QMessageBox::Discard:
				event->accept();
				//m_liveView->hide();
				foreach(OutputInstance *inst, m_outputInstances)
					inst->hide();
				return;
			default:
				event->ignore();
				return;
		};
	}
	else
	if(actionSave())
	{
		event->accept();
		//m_liveView->hide();
		foreach(OutputInstance *inst, m_outputInstances)
			inst->hide();
		//close();
	}
	else
	{
		event->ignore();
	}

	//deleteLater();
}


/*
void MainWindow::slotListContextMenu(const QPoint &pos)
{
	QModelIndex idx = m_groupView->currentIndex();
	SlideGroup *s = m_docModel.groupFromIndex(idx);
	qDebug() << "MainWindow::slotListContextMenu(): selected group#:"<<s->groupNumber()<<", title:"<<s->groupTitle()<<", pos:"<<pos<<", mapToGlobal:"<<mapToGlobal(pos);

	QMenu contextMenu(tr("Context menu"), this);
	contextMenu.addAction(new QAction(tr("Hello"), this));
	contextMenu.exec(mapToGlobal(pos));
}
*/

void MainWindow::actionAppSettingsDialog()
{
	AppSettingsDialog *d = new AppSettingsDialog(this);
	d->exec();

	emit appSettingsChanged();
	setupOutputViews();
	setupOutputList();
	setupOutputControls();
	setupHotkeys();


	// reapply autosave time
	if(m_autosaveTimer && m_autosaveTimer->isActive())
	{
		m_autosaveTimer->stop();
		if(AppSettings::autosaveTime() > 0)
			m_autosaveTimer->start(AppSettings::autosaveTime() * 1000);
	}
	
	// update control server
	if(m_controlServer)
		delete m_controlServer;
	
	if(AppSettings::httpControlEnabled())
		m_controlServer = new ControlServer(AppSettings::httpControlPort(),this);

}

void MainWindow::setupHotkeys()
{
	if(!m_hotkeyActions.contains("black"))
	{
		QAction * action = new QAction("Enable/Disable Black",this);
		action->setShortcutContext(Qt::ApplicationShortcut);
		addAction(action);
		connect(action, SIGNAL(triggered()), this, SLOT(hotkeyBlack()));
		m_hotkeyActions.insert("black",action);
	}
	m_hotkeyActions["black"]->setShortcut(AppSettings::hotkeySequence("black"));
	
	if(!m_hotkeyActions.contains("clear"))
	{
		QAction * action = new QAction("Enable/Disable Clear",this);
		action->setShortcutContext(Qt::ApplicationShortcut);
		addAction(action);
		connect(action, SIGNAL(triggered()), this, SLOT(hotkeyClear()));
		m_hotkeyActions.insert("clear",action);
	}
	m_hotkeyActions["clear"]->setShortcut(AppSettings::hotkeySequence("clear"));	
	
	if(!m_hotkeyActions.contains("logo"))
	{
		QAction * action = new QAction("Enable/Disable Logo",this);
		action->setShortcutContext(Qt::ApplicationShortcut);
		addAction(action);
		connect(action, SIGNAL(triggered()), this, SLOT(hotkeyLogo()));
		m_hotkeyActions.insert("logo",action);
	}
	
	m_hotkeyActions["logo"]->setShortcut(AppSettings::hotkeySequence("logo"));
	
	if(!m_hotkeyActions.contains("next-slide"))
	{
		QAction * action = new QAction("Goto Next Slide",this);
		action->setShortcutContext(Qt::ApplicationShortcut);
		addAction(action);
		connect(action, SIGNAL(triggered()), this, SLOT(hotkeyNextSlide()));
		m_hotkeyActions.insert("next-slide",action);
	}
	m_hotkeyActions["next-slide"]->setShortcut(AppSettings::hotkeySequence("next-slide"));	
	
	if(!m_hotkeyActions.contains("prev-slide"))
	{
		QAction * action = new QAction("Goto Previous Slide",this);
		action->setShortcutContext(Qt::ApplicationShortcut);
		addAction(action);
		connect(action, SIGNAL(triggered()), this, SLOT(hotkeyPrevSlide()));
		m_hotkeyActions.insert("prev-slide",action);
	}
	m_hotkeyActions["prev-slide"]->setShortcut(AppSettings::hotkeySequence("prev-slide"));	
	
	if(!m_hotkeyActions.contains("next-group"))
	{
		QAction * action = new QAction("Goto Next Group",this);
		action->setShortcutContext(Qt::ApplicationShortcut);
		addAction(action);
		connect(action, SIGNAL(triggered()), this, SLOT(nextGroup()));
		m_hotkeyActions.insert("next-group",action);
	}
	m_hotkeyActions["next-group"]->setShortcut(AppSettings::hotkeySequence("next-group"));
	
	if(!m_hotkeyActions.contains("prev-group"))
	{
		QAction * action = new QAction("Goto Prev Group",this);
		action->setShortcutContext(Qt::ApplicationShortcut);
		addAction(action);
		connect(action, SIGNAL(triggered()), this, SLOT(prevGroup()));
		m_hotkeyActions.insert("prev-group",action);
	}	
	m_hotkeyActions["prev-group"]->setShortcut(AppSettings::hotkeySequence("prev-group"));
}

void MainWindow::hotkeyBlack()
{
	int liveId = AppSettings::taggedOutput("live")->id();
	OutputControl * ctrl = outputControl(liveId);
	ctrl->fadeBlackFrame(!ctrl->isBlackToggled());
}

void MainWindow::hotkeyClear()
{
	int liveId = AppSettings::taggedOutput("live")->id();
	OutputControl * ctrl = outputControl(liveId);
	ctrl->fadeClearFrame(!ctrl->isClearToggled());
}

void MainWindow::hotkeyLogo()
{
	QMessageBox::information(this,"Not Implemented Yet","Sorry, the logo feature isn't implemented yet.");
}

void MainWindow::hotkeyNextSlide()
{
	int liveId = AppSettings::taggedOutput("live")->id();
	SlideGroupViewControl *ctrl = viewControl(liveId);
	ctrl->nextSlide();
}

void MainWindow::hotkeyPrevSlide()
{
	int liveId = AppSettings::taggedOutput("live")->id();
	SlideGroupViewControl *ctrl = viewControl(liveId);
	ctrl->prevSlide();
}

void MainWindow::setAutosaveEnabled(bool flag)
{
	if(flag)
	{
		if(AppSettings::autosaveTime() > 0)
			m_autosaveTimer->start(AppSettings::autosaveTime() * 1000);
	}
	else
	{
		if(m_autosaveTimer->isActive())
			m_autosaveTimer->stop();
	}
}



void MainWindow::setupOutputControls()
{
	QList<Output*> allOut = AppSettings::outputs();

	foreach(Output *output, allOut)
	{
		if(m_outputControls.contains(output->id()))
		{
			OutputControl * outCtrl = m_outputControls[output->id()];

			int idx = m_outputTabs->indexOf(outCtrl);

			m_outputTabs->setTabEnabled(idx,output->isEnabled());
		}
		else
		{
			OutputInstance *inst = outputInst(output->id());
			//SlideGroupViewControl *ctrl = viewControl(output->id());

			OutputControl * outCtrl = new OutputControl();
			outCtrl->setOutputInstance(inst);
			m_outputControls[output->id()] = outCtrl;

			int idx = m_outputTabs->addTab(outCtrl, output->name());

			m_outputTabs->setTabEnabled(idx,output->isEnabled());

			SlideGroupFactory *factory = SlideGroupFactory::factoryForType(SlideGroup::GroupType);

			if(factory)
			{
				SlideGroupViewControl * ctrl = factory->newViewControl();
				ctrl->setOutputView(inst);
				outCtrl->setViewControl(ctrl);

				outCtrl->setCustomFilters(factory->customFiltersFor(outCtrl->outputInstance()));

				m_viewControls[output->id()] = ctrl;

				//m_outputTabs->addTab(ctrl,output->name());
			}
		}

		if(m_outputViewDocks.contains(output->id()))
		{
			QDockWidget * dock = m_outputViewDocks[output->id()];
			dock->setVisible(output->isEnabled());
		}
		else
		{
			QDockWidget *dock = new QDockWidget(QString(tr("%1 View")).arg(output->name()), this);
			dock->setObjectName(output->name());

			OutputInstance *inst = outputInst(output->id());
			OutputViewer *view = new OutputViewer(inst,dock);
			dock->setWidget(view);
			addDockWidget(Qt::RightDockWidgetArea, dock);

			m_outputViewDocks[output->id()] = dock;

			dock->setVisible(output->isEnabled());
		}
	}
}


void MainWindow::actionDocSettingsDialog()
{
	double ar = m_doc->aspectRatio();
	DocumentSettingsDialog *d = new DocumentSettingsDialog(m_doc,this);
	d->exec();
	if(ar != m_doc->aspectRatio())
		emit aspectRatioChanged(m_doc->aspectRatio());

}


void MainWindow::groupSelected(const QModelIndex &idx)
{
	SlideGroup *s = m_docModel->groupFromIndex(idx);
        //qDebug() << "MainWindow::groupSelected(): selected group#:"<<s->groupNumber()<<", title:"<<s->groupTitle();
	previewSlideGroup(s);
	m_ui->actionEdit_Slide_Group->setEnabled(true);
	m_ui->actionDelete_Slide_Group->setEnabled(true);
	m_ui->actionSlide_Group_Properties->setEnabled(true);

}

void MainWindow::prevGroup()
{
	QModelIndex idx = m_groupView->currentIndex();
	int nextRow = idx.row() - 1;
	if(nextRow < 0)
		nextRow = m_doc->numGroups() - 1;
// 	qDebug() << "MainWindow::prevGroup(): nextRow:"<<nextRow;
	jumpToGroup(nextRow);
}

void MainWindow::nextGroup()
{
	QModelIndex idx = m_groupView->currentIndex();
	int nextRow = idx.row() + 1;
	if(nextRow >= m_doc->numGroups())
		nextRow = 0;
// 	qDebug() << "MainWindow::nextGroup(): nextRow:"<<nextRow;
	jumpToGroup(nextRow);
}

void MainWindow::jumpToGroup(int nextRow)
{
// 	qDebug() << "MainWindow::jumpToGroup(): row:"<<nextRow;
	SlideGroup *nextGroup = m_docModel->groupAt(nextRow);
	QModelIndex nextIdx = m_docModel->indexForGroup(nextGroup);
	//groupDoubleClicked(nextIdx);
	
	m_groupView->setCurrentIndex(nextIdx);	
	setLiveGroup(nextGroup);

	
}


void MainWindow::previewSlideGroup(SlideGroup *newGroup)
{
	//qDebug() << "MainWindow::previewSlideGroup: Loading preview slide\n";

	SlideGroup * oldGroup = m_previewControl->slideGroup();

	//qDebug() << "MainWindow::setLiveGroup(): newGroup->groupType():"<<newGroup->groupType()<<", SlideGroup::GroupType:"<<SlideGroup::GroupType;
// 	if((oldGroup && oldGroup->groupType() != newGroup->groupType())
// 		|| newGroup->groupType() != SlideGroup::GroupType)
	if(1)
	{
		SlideGroupFactory *factory = SlideGroupFactory::factoryForType(newGroup->groupType());
		if(!factory)
		{
			//qDebug() << "MainWindow::setLiveGroup(): Factory fell thu for request, going to generic control";
			factory = SlideGroupFactory::factoryForType(SlideGroup::GroupType);
		}

		if(factory)
		{
			//qDebug() << "MainWindow::setLiveGroup(): got new factory, initalizing";
			if(m_previewControl)
			{
				m_previewControlBase->layout()->removeWidget(m_previewControl);
				disconnect(m_previewControl, 0, this, 0);
 				delete m_previewControl;
 				m_previewControl = 0;
			}


			m_previewControl = factory->newViewControl();
			m_previewControl->setIsPreviewControl(true);
			m_previewControl->setOutputView(m_previewInstance);
			m_previewControlBase->layout()->addWidget(m_previewControl);

			connect(m_previewControl, SIGNAL(slideDoubleClicked(Slide *)), this, SLOT(previewSlideDoubleClicked(Slide *)));
		}
	}

	// I think I've coded my own API under an incorrect assumption - that is, that I need to set the slide group on the instance
	// instead of just letting the SlideGroupViewControl set the slide group - which now seems the more logical method.
	// Therefore, I've removed direct calls to OutputInstance::setSlideGroup() from MainWindow::* methods, and instead
	// will rely on SlideGroupViewControl to set the slide group on the outpuTInstance that it is responsible for managing.
	m_previewControl->setSlideGroup(newGroup);
	//m_previewInstance->setSlideGroup(newGroup);
	m_previewControl->setFocus(Qt::OtherFocusReason);

}

void MainWindow::previewSlideDoubleClicked(Slide *slide)
{
	SlideGroup *group = m_previewInstance->slideGroup();
	setLiveGroup(group,slide);
}

void MainWindow::setLiveGroup(SlideGroup *newGroup, Slide *currentSlide, bool allowProgressDialog)
{
	//qDebug() << "MainWindow::setLiveGroup: newGroup:"<<newGroup<<", slide:"<<currentSlide;
	QList<Output*> selectedOutputs;
	QList<Output*> allOut = AppSettings::outputs();

	foreach(Output *out, allOut)
	{
		QCheckBox *box = m_outputCheckboxes[out->id()];
		if(box && box->isChecked())
			selectedOutputs.append(out);
	}

	QList<int> alreadyConsumed;

	foreach(OutputControl *outputCtrl, m_outputControls)
	{
		Output * output = outputCtrl->outputInstance()->output();
		if(selectedOutputs.contains(output))
		{
			if(outputCtrl->outputIsSynced())
			{
				//qDebug() << "MainWindow::setLiveGroup: preping synced output:"<<output->name();
				alreadyConsumed << output->id();

				sendGroupToOutput(output, newGroup, currentSlide, allowProgressDialog);
			}
		}
	}

	foreach(Output *output, selectedOutputs)
	{
		if(!alreadyConsumed.contains(output->id()))
		{
			//qDebug() << "MainWindow::setLiveGroup: setting non-synced output:"<<output->name();

			sendGroupToOutput(output, newGroup, currentSlide, allowProgressDialog);
		}
	}

}


void MainWindow::sendGroupToOutput(Output *output, SlideGroup *newGroup, Slide *currentSlide, bool allowProgressDialog)
{
	if(!newGroup)
		return;

	OutputInstance *inst = outputInst(output->id());
	OutputControl *outputCtrl = outputControl(output->id());
	SlideGroupViewControl *ctrl = viewControl(output->id());

	SlideGroup * oldGroup = ctrl->slideGroup();

	//if(!inst->isVisible())
	//	inst->show();

	//qDebug() << "MainWindow::sendGroupToOutput(): newGroup->groupType():"<<newGroup->groupType()<<", SlideGroup::GroupType:"<<SlideGroup::GroupType<<"old: "<<(oldGroup ? oldGroup->groupType() : -1);
// 	if((oldGroup && oldGroup->groupType() != newGroup->groupType()) || newGroup->groupType() != SlideGroup::GroupType)
	
	SlideGroupFactory *factory = SlideGroupFactory::factoryForType(newGroup->groupType());
	if(!factory)
	{
		//qDebug() << "MainWindow::setLiveGroup(): Factory fell thu for request, going to generic control";
		factory = SlideGroupFactory::factoryForType(SlideGroup::GroupType);
	}

	if(factory)
	{
		//qDebug() << "MainWindow::setLiveGroup(): got new factory, initalizing";
		if(ctrl)
		{
			delete ctrl;
			ctrl = 0;
		}


		ctrl = factory->newViewControl();
		ctrl->setOutputView(inst);
		//m_outputTabs->addTab(ctrl,output->name());
		outputCtrl->setViewControl(ctrl);
		outputCtrl->setCustomFilters(factory->customFiltersFor(outputCtrl->outputInstance(),newGroup));
		
// 		qDebug() << "MainWindow::setLiveGroup: Loading into view control:"<<newGroup;
		ctrl->setSlideGroup(newGroup,currentSlide);
// 		qDebug() << "MainWindow::setLiveGroup: Loading into output instance:"<<newGroup;

		// I think I've coded my own API under an incorrect assumption - that is, that I need to set the slide group on the instance
		// instead of just letting the SlideGroupViewControl set the slide group - which now seems the more logical method.
		// Therefore, I've removed direct calls to OutputInstance::setSlideGroup() from MainWindow::* methods, and instead
		// will rely on SlideGroupViewControl to set the slide group on the outpuTInstance that it is responsible for managing.
		//inst->setSlideGroup(newGroup,currentSlide);
		 
		// hackish, I know - need a generic way for the slide group factory to set the text resized flag as well
		if(newGroup->groupType() == SongSlideGroup::GroupType && 
			(output->tags().toLower().indexOf("foldback") >= 0 ||
				output->name().toLower().indexOf("foldback") >= 0))
			outputCtrl->setTextResizeEnabled(true);

		m_viewControls[output->id()] = ctrl;
	}
	else
	{
		qDebug() << "MainWindow::setLiveGroup: [2] Loading into view control:"<<newGroup;
		ctrl->setSlideGroup(newGroup,currentSlide);
		qDebug() << "MainWindow::setLiveGroup: [2] Loading into output instance:"<<newGroup;
		//inst->setSlideGroup(newGroup,currentSlide);
			
	}
	
	//qDebug() << "MainWindow::setLiveGroup: Loading into LIVE output (done)";
	ctrl->setFocus(Qt::OtherFocusReason);
}


void MainWindow::groupDoubleClicked(const QModelIndex &idx)
{
	SlideGroup *g = m_docModel->groupFromIndex(idx);
        //qDebug() << "MainWindow::groupSelected(): double-clicked group#:"<<g->groupNumber()<<", title:"<<g->groupTitle();

        Slide *slide = 0;
	SlideGroup *previewGroup = m_previewInstance->slideGroup();
	if(previewGroup == g && m_previewControl)
		slide = m_previewControl->selectedSlide();

	setLiveGroup(g,slide);
}

void MainWindow::editGroup(SlideGroup *g, Slide *slide)
{
	statusBar()->showMessage(QString(tr("Loading %1...")).arg(g->groupTitle().isEmpty() ? QString(tr("Group %1")).arg(g->groupNumber()) : g->groupTitle()));
	openSlideEditor(g,slide);
	//m_previewWidget->clear();
	statusBar()->clearMessage();
}

void MainWindow::actionEditGroup()
{
	QModelIndex idx = m_groupView->currentIndex();
	if(!idx.isValid())
		return;
	SlideGroup *group = m_docModel->groupFromIndex(idx);
	Slide *slide = 0;
	if(liveInst()->slideGroup() == group)
		slide = liveCtrl()->selectedSlide();

	editGroup(group,slide);
}

void MainWindow::actionGroupProperties()
{
	QModelIndex idx = m_groupView->currentIndex();
	if(!idx.isValid())
		return;
	SlideGroup *s = m_docModel->groupFromIndex(idx);
	SlideGroupSettingsDialog d(s,this);
	d.adjustSize();
	d.exec();
}


void MainWindow::actionNewGroup()
{
	Slide * slide = new Slide();
	SlideGroup *g = new SlideGroup();
	g->addSlide(slide);
	m_doc->addGroup(g);

	QModelIndex idx = m_docModel->indexForGroup(g);
	m_groupView->setCurrentIndex(idx);
}

void MainWindow::actionDelGroup()
{
	QModelIndex idx = m_groupView->currentIndex();
	if(!idx.isValid())
		return;
	SlideGroup *s = m_docModel->groupFromIndex(idx);
	deleteGroup(s);
}

void MainWindow::deleteGroup(SlideGroup *s)
{
	QModelIndex idx = m_docModel->indexForGroup(s);

	m_doc->removeGroup(s);

	if(idx.row()>0)
		m_groupView->setCurrentIndex(m_docModel->indexForRow(idx.row()-1));
}


AbstractSlideGroupEditor * MainWindow::openSlideEditor(SlideGroup *group,Slide *slide)
{
	if(group->groupType() != SlideGroup::GroupType)
	{
		SlideGroupFactory *factory = SlideGroupFactory::factoryForType(group->groupType());
		if(!factory)
			factory = SlideGroupFactory::factoryForType(SlideGroup::GroupType);

		if(factory)
		{
			AbstractSlideGroupEditor * editor = factory->newEditor();
			if(!editor)
			{
				QMessageBox::information(this, QString(tr("Cannot Edit %1").arg(group->assumedName())), QString(tr("DViz has no way to edit slide groups with type # %1 at this time.").arg(group->groupType())));
				return 0;
			}

			editor->setSlideGroup(group,slide);
			editor->show();
			return editor;
		}
	}


	m_editWin->setSlideGroup(group,slide);
	m_editWin->show();
	m_editWin->setSlideGroup(group,slide);
	
	if(m_editWin->isMinimized())
	{
		m_editWin->showNormal();
		m_editWin->raise();
		m_editWin->activateWindow();
	}
	
	return m_editWin;
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


QRect MainWindow::standardSceneRect(double aspectRatio)
{
	if(aspectRatio < 0)
	{
		if(m_doc)
			aspectRatio = m_doc->aspectRatio();
		else
			aspectRatio = AppSettings::liveAspectRatio();
	}

	int height = 768;
	return QRect(0,0,aspectRatio * height,height);
}
