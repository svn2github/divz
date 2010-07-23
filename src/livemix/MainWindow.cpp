#include <QtGui>

#include "MainWindow.h"
#include "MdiChild.h"
#include "MdiMjpeg.h"
#include "MdiCamera.h"
#include <QCDEStyle>

MainWindow::MainWindow()
{
	mdiArea = new QMdiArea;
	mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setCentralWidget(mdiArea);
	connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
		this, SLOT(updateMenus()));
	windowMapper = new QSignalMapper(this);
	connect(windowMapper, SIGNAL(mapped(QWidget*)),
		this, SLOT(setActiveSubWindow(QWidget*)));
	
	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
	updateMenus();
	
	readSettings();
	
	setWindowTitle(tr("LiveMix"));
	setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::newCamera()
{
	addNewWindow(new MdiCamera);
}

void MainWindow::addNewWindow(QWidget *child)
{
	QMdiSubWindow *window = new QMdiSubWindow;
	window->setStyle(new QCDEStyle());
	child->setStyle(QApplication::style());
	window->setWidget(child);
	window->setAttribute(Qt::WA_DeleteOnClose);
	window->resize(child->sizeHint()); 
	mdiArea->addSubWindow(window);
	
	child->show();
// 	MdiVideoSource *vid = dynamic_cast<MdiVideoSource*>(child);
// 	if(vid)
// 		vid->videoWidget()->resize(160,120);
//	child->adjustSize();
}

void MainWindow::newMjpeg()
{
	addNewWindow(new MdiMjpeg);
}
void MainWindow::newProgram()
{
}
void MainWindow::newVideo()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	mdiArea->closeAllSubWindows();
	if (mdiArea->currentSubWindow()) 
	{
		event->ignore();
	} 
	else 
	{
		writeSettings();
		event->accept();
	}
}

void MainWindow::newFile()
{
// 	MdiChild *child = createMdiChild();
// 	child->newFile();
// 	child->show();
}

void MainWindow::open()
{
// 	QString fileName = QFileDialog::getOpenFileName(this);
// 	if (!fileName.isEmpty()) 
// 	{
// 		QMdiSubWindow *existing = findMdiChild(fileName);
// 		if (existing) 
// 		{
// 			mdiArea->setActiveSubWindow(existing);
// 			return;
// 		}
// 	
// 		MdiChild *child = createMdiChild();
// 		if (child->loadFile(fileName)) 
// 		{
// 			statusBar()->showMessage(tr("File loaded"), 2000);
// 			child->show();
// 		} 
// 		else 
// 		{
// 			child->close();
// 		}
// 	}
}

void MainWindow::save()
{
// 	if (activeMdiChild() && activeMdiChild()->save())
// 		statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::saveAs()
{
// 	if (activeMdiChild() && activeMdiChild()->saveAs())
// 		statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::cut()
{
// 	if (activeMdiChild())
// 		activeMdiChild()->cut();
}

void MainWindow::copy()
{
// 	if (activeMdiChild())
// 		activeMdiChild()->copy();
}

void MainWindow::paste()
{/*
	if (activeMdiChild())
		activeMdiChild()->paste();*/
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About LiveMix"),
		tr("<b>LiveMix</b> is an open-source video mixer for live and recorded video."));
}

void MainWindow::updateMenus()
{
	bool hasMdiChild = (activeMdiChild() != 0);
	saveAct->setEnabled(hasMdiChild);
	saveAsAct->setEnabled(hasMdiChild);
	pasteAct->setEnabled(hasMdiChild);
	closeAct->setEnabled(hasMdiChild);
	closeAllAct->setEnabled(hasMdiChild);
	tileAct->setEnabled(hasMdiChild);
	cascadeAct->setEnabled(hasMdiChild);
	nextAct->setEnabled(hasMdiChild);
	previousAct->setEnabled(hasMdiChild);
	separatorAct->setVisible(hasMdiChild);
/*	
	bool hasSelection = (activeMdiChild() &&
				activeMdiChild()->textCursor().hasSelection());
	cutAct->setEnabled(hasSelection);
	copyAct->setEnabled(hasSelection);*/
}

void MainWindow::updateWindowMenu()
{
	windowMenu->clear();
	windowMenu->addAction(closeAct);
	windowMenu->addAction(closeAllAct);
	windowMenu->addSeparator();
	windowMenu->addAction(tileAct);
	windowMenu->addAction(cascadeAct);
	windowMenu->addSeparator();
	windowMenu->addAction(nextAct);
	windowMenu->addAction(previousAct);
	windowMenu->addAction(separatorAct);
	
	QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
	separatorAct->setVisible(!windows.isEmpty());
	
	for (int i = 0; i < windows.size(); ++i) 
	{
		MdiChild *child = qobject_cast<MdiChild *>(windows.at(i)->widget());
	
// 		QString text;
// 		if (i < 9) 
// 		{
// 			text = tr("&%1 %2").arg(i + 1)
// 					.arg(child->userFriendlyCurrentFile());
// 		} else {
// 			text = tr("%1 %2").arg(i + 1)
// 					.arg(child->userFriendlyCurrentFile());
// 		}
		QAction *action  = windowMenu->addAction(child->windowTitle());
		action->setCheckable(true);
		action ->setChecked(child == activeMdiChild());
		connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
		windowMapper->setMapping(action, windows.at(i));
	}
}

void MainWindow::createActions()
{
	newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
	newAct->setShortcuts(QKeySequence::New);
	newAct->setStatusTip(tr("Create a new file"));
	connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));
	
	openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("Open an existing file"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
	
	saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
	saveAct->setShortcuts(QKeySequence::Save);
	saveAct->setStatusTip(tr("Save the document to disk"));
	connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));
	
	saveAsAct = new QAction(tr("Save &As..."), this);
	saveAsAct->setShortcuts(QKeySequence::SaveAs);
	saveAsAct->setStatusTip(tr("Save the document under a new name"));
	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));
	
	//! [0]
	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcuts(QKeySequence::Quit);
	exitAct->setStatusTip(tr("Exit the application"));
	connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
	//! [0]
	
	cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
	cutAct->setShortcuts(QKeySequence::Cut);
	cutAct->setStatusTip(tr("Cut the current selection's contents to the "
				"clipboard"));
	connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));
	
	copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
	copyAct->setShortcuts(QKeySequence::Copy);
	copyAct->setStatusTip(tr("Copy the current selection's contents to the "
				"clipboard"));
	connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));
	
	pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
	pasteAct->setShortcuts(QKeySequence::Paste);
	pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
				"selection"));
	connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));
	
	closeAct = new QAction(tr("Cl&ose"), this);
	closeAct->setStatusTip(tr("Close the active window"));
	connect(closeAct, SIGNAL(triggered()),
		mdiArea, SLOT(closeActiveSubWindow()));
	
	closeAllAct = new QAction(tr("Close &All"), this);
	closeAllAct->setStatusTip(tr("Close all the windows"));
	connect(closeAllAct, SIGNAL(triggered()),
		mdiArea, SLOT(closeAllSubWindows()));
	
	tileAct = new QAction(tr("&Tile"), this);
	tileAct->setStatusTip(tr("Tile the windows"));
	connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));
	
	cascadeAct = new QAction(tr("&Cascade"), this);
	cascadeAct->setStatusTip(tr("Cascade the windows"));
	connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));
	
	nextAct = new QAction(tr("Ne&xt"), this);
	nextAct->setShortcuts(QKeySequence::NextChild);
	nextAct->setStatusTip(tr("Move the focus to the next window"));
	connect(nextAct, SIGNAL(triggered()),
		mdiArea, SLOT(activateNextSubWindow()));
	
	previousAct = new QAction(tr("Pre&vious"), this);
	previousAct->setShortcuts(QKeySequence::PreviousChild);
	previousAct->setStatusTip(tr("Move the focus to the previous "
					"window"));
	connect(previousAct, SIGNAL(triggered()),
		mdiArea, SLOT(activatePreviousSubWindow()));
	
	separatorAct = new QAction(this);
	separatorAct->setSeparator(true);
	
	aboutAct = new QAction(tr("&About"), this);
	aboutAct->setStatusTip(tr("Show the application's About box"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
	
	aboutQtAct = new QAction(tr("About &Qt"), this);
	aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	
	m_actNewCamera= new QAction(tr("New Camera"), this);
	connect(m_actNewCamera, SIGNAL(triggered()), this, SLOT(newCamera()));
	
	m_actNewVideo = new QAction(tr("New Video"), this);
	connect(m_actNewVideo, SIGNAL(triggered()), this, SLOT(newVideo()));
	
	m_actNewMjpeg = new QAction(tr("New MJPEG"), this);
	connect(m_actNewMjpeg, SIGNAL(triggered()), this, SLOT(newMjpeg()));
	
	m_actNewProgram = new QAction(tr("New Program"), this);
	connect(m_actNewProgram, SIGNAL(triggered()), this, SLOT(newProgram()));
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
// 	fileMenu->addAction(newAct);
// 	fileMenu->addAction(openAct);
// 	fileMenu->addAction(saveAct);
// 	fileMenu->addAction(saveAsAct);

	fileMenu->addAction(m_actNewCamera);
	fileMenu->addAction(m_actNewVideo);
	fileMenu->addAction(m_actNewMjpeg);
	fileMenu->addAction(m_actNewProgram);
	fileMenu->addSeparator();
	
	fileMenu->addAction(exitAct);
	
// 	editMenu = menuBar()->addMenu(tr("&Edit"));
// 	editMenu->addAction(cutAct);
// 	editMenu->addAction(copyAct);
// 	editMenu->addAction(pasteAct);
	
	windowMenu = menuBar()->addMenu(tr("&Window"));
	updateWindowMenu();
	connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));
	
	menuBar()->addSeparator();
	
	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
	fileToolBar = addToolBar(tr("Modules"));
	fileToolBar->addAction(m_actNewCamera);
	fileToolBar->addAction(m_actNewVideo);
	fileToolBar->addAction(m_actNewMjpeg);
	fileToolBar->addAction(m_actNewProgram);
	
// 	editToolBar = addToolBar(tr("Edit"));
// 	editToolBar->addAction(cutAct);
// 	editToolBar->addAction(copyAct);
// 	editToolBar->addAction(pasteAct);
}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
	QSettings settings;
	QPoint pos = settings.value("pos", QPoint(10, 10)).toPoint();
	QSize size = settings.value("size", QSize(640,480)).toSize();
	move(pos);
	resize(size);
}

void MainWindow::writeSettings()
{
	QSettings settings;
	settings.setValue("pos", pos());
	settings.setValue("size", size());
}

MdiChild *MainWindow::activeMdiChild()
{
	if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
		return qobject_cast<MdiChild *>(activeSubWindow->widget());
	return 0;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &name)
{
	foreach (QMdiSubWindow *window, mdiArea->subWindowList()) 
	{
		MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
		if (mdiChild->windowTitle() == name)
			return window;
	}
	return 0;
}


void MainWindow::setActiveSubWindow(QWidget *window)
{
	if (!window)
		return;
	mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}
