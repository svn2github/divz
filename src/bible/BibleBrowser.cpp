#include "BibleBrowser.h"

#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QStringList>
#include <QMovie>
#include <QComboBox>
#include <QMessageBox>
#include <QAction>
#include <QMenu>
#include <QProgressDialog>
#include <QCompleter>
#include <QApplication> // for processEvents

#include "BibleModel.h"
#include "BibleGatewayConnector.h"
#include "LocalBibleManager.h"

#include "model/Slide.h"
#include "model/SlideGroup.h"
#include "model/Document.h"
#include "model/TextBoxItem.h"
#include "model/ItemFactory.h"
#include "MainWindow.h"
#include "AppSettings.h"
#include "model/Output.h"
#include "OutputInstance.h"
#include "model/SlideTemplateManager.h"
#include "ChooseGroupDialog.h"

using namespace SlideTemplateUtilities;


BibleBrowser::BibleBrowser(QWidget *parent)
	: QWidget(parent)
	, m_showEachVerseOnSeperateSlide(false)
	, m_showVerseNumbers(true)
	, m_showFullRefTopEachSlide(false)
 	, m_showFullRefBottomEachSlide(false)
 	, m_showFullRefAtFirstTop(false)
 	, m_showFullRefAtBottomLast(false)
 	, m_showFullRefAtStart(true)
	, m_showFullRefAtEnd(false)
	, m_showResponsiveReadingLabels(false)
	, m_appendToExistingGroup(false)
	, m_attemptAutoLive(false)
	, m_template(0)
	, m_lastDownloadAutomated(false)
	, m_downloadComplete(false)
	, m_tempDest(0)
{
	setObjectName("BibleBrowser");
	
	QSettings s;
	
	m_showEachVerseOnSeperateSlide	= s.value("biblebrowser/show-each-verse-on-seperate-slide",false).toBool();
	
	m_showVerseNumbers		= s.value("biblebrowser/show-verse-numbers",true).toBool();
	
	m_showFullRefTopEachSlide	= s.value("biblebrowser/show-full-ref-top-each-slide",false).toBool();
 	m_showFullRefBottomEachSlide	= s.value("biblebrowser/show-full-ref-bottom-each-slide",false).toBool();
 	
 	m_showFullRefAtFirstTop		= s.value("biblebrowser/show-full-ref-at-first-top",false).toBool();
 	m_showFullRefAtBottomLast 	= s.value("biblebrowser/show-full-ref-at-bottom-last",false).toBool();
 	
	m_showFullRefAtStart		= s.value("biblebrowser/show-full-ref-at-start",true).toBool();
	m_showFullRefAtEnd		= s.value("biblebrowser/show-full-ref-at-end",false).toBool();
	m_showResponsiveReadingLabels	= s.value("biblebrowser/show-responsive-reading-lables",false).toBool();
	
	m_appendToExistingGroup		= s.value("biblebrowser/append-to-existing-group",false).toBool();
	m_allowSplitVersesInAltGroups	= s.value("biblebrowser/allow-split",false).toBool();
	
	
	
	int templateId = s.value("biblebrowser/template-id",0).toInt();
	if(templateId > 0)
		m_template = SlideTemplateManager::instance()->findTemplate(SlideTemplateManager::Bible,templateId);
	
	setupUI();
	
	// Set version index AFTER setupUI so m_versionCombo can get created
	int idx = s.value("biblebrowser/current-version",-1).toInt();
	if(idx > -1)
		m_versionCombo->setCurrentIndex(idx);
	
	// Likewise, set the search text after setupUI so that m_search is created
	m_search->setText(s.value("biblebrowser/last-reference","John 3:16").toString());
	m_search->selectAll();
	
	m_bible = new BibleGatewayConnector();
	connect(m_bible, SIGNAL(referenceAvailable(const BibleVerseRef& , const BibleVerseList &)), this, SLOT(referenceAvailable(const BibleVerseRef& , const BibleVerseList &)));
	
	connect(LocalBibleManager::inst(), SIGNAL(bibleListChanged()), this, SLOT(setupVersionCombo()));
}
	
BibleBrowser::~BibleBrowser() 
{
	saveSettings();
}

void BibleBrowser::saveSettings()
{
//	qDebug() << "BibleBrowser::saveSettings()";
	
	QSettings s;
	
	s.setValue("biblebrowser/show-each-verse-on-seperate-slide",	m_showEachVerseOnSeperateSlide);
	
	s.setValue("biblebrowser/show-verse-numbers",			m_showVerseNumbers);
	
	s.setValue("biblebrowser/show-full-ref-top-each-slide",		m_showFullRefTopEachSlide);
 	s.setValue("biblebrowser/show-full-ref-bottom-each-slide",	m_showFullRefBottomEachSlide);
 	
 	s.setValue("biblebrowser/show-full-ref-at-first-top",		m_showFullRefAtFirstTop);
 	s.setValue("biblebrowser/show-full-ref-at-bottom-last",		m_showFullRefAtBottomLast);
 	
	s.setValue("biblebrowser/show-full-ref-at-start",		m_showFullRefAtStart);
	s.setValue("biblebrowser/show-full-ref-at-end",			m_showFullRefAtEnd);
	s.setValue("biblebrowser/show-responsive-reading-lables",	m_showResponsiveReadingLabels);
	
	s.setValue("biblebrowser/current-version",			m_versionCombo->currentIndex());
	s.setValue("biblebrowser/last-reference",			m_search->text());
	s.setValue("biblebrowser/template-id",				m_template ? m_template->groupId() : 0);
	
	s.setValue("biblebrowser/append-to-existing-group",		m_appendToExistingGroup);
	
	s.setValue("biblebrowser/allow-split",				m_allowSplitVersesInAltGroups);
}

void BibleBrowser::closeEvent(QCloseEvent*)
{
}

#define SET_MARGIN(layout,margin) \
	layout->setContentsMargins(margin,margin,margin,margin);

void BibleBrowser::setShowEachVerseOnSeperateSlide(bool x)	{ m_showEachVerseOnSeperateSlide = x; saveSettings(); }

void BibleBrowser::setShowVerseNumbers(bool x) 			{ m_showVerseNumbers = x; saveSettings(); }
void BibleBrowser::setShowFullRefTopEachSlide(bool x) 		{ m_showFullRefTopEachSlide = x; saveSettings(); }
void BibleBrowser::setShowFullRefBottomEachSlide(bool x)	{ m_showFullRefBottomEachSlide = x; saveSettings(); }
void BibleBrowser::setShowFullRefAtFirstTop(bool x)		{ m_showFullRefAtFirstTop = x; saveSettings(); }
void BibleBrowser::setShowFullRefAtBottomLast(bool x)		{ m_showFullRefAtBottomLast = x; saveSettings(); }


void BibleBrowser::setShowFullRefAtStart(bool x) 		{ m_showFullRefAtStart = x; saveSettings(); }
void BibleBrowser::setShowFullRefAtEnd(bool x) 			{ m_showFullRefAtEnd = x; saveSettings(); }
void BibleBrowser::setShowResponsiveReadingLabels(bool x) 	{ m_showResponsiveReadingLabels = x; saveSettings(); }

void BibleBrowser::setAppendToExistingGroup(bool x) 		{ m_appendToExistingGroup = x; saveSettings(); }

void BibleBrowser::setAllowSplitVersesInAltGroups(bool x)	{ m_allowSplitVersesInAltGroups = x; saveSettings(); }

void BibleBrowser::setupUI()
{
	QVBoxLayout *vbox = new QVBoxLayout(this);
	//SET_MARGIN(vbox,0);
	
	
	QHBoxLayout *hboxTop = new QHBoxLayout();
	vbox->addLayout(hboxTop);
	
	m_versionCombo = new QComboBox(this);
	m_versionCombo->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
	setupVersionCombo();
	hboxTop->addWidget(m_versionCombo);
	
	connect(m_versionCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(searchReturnPressed()));
	
	QPushButton *editBtn = new QPushButton(QPixmap(":/data/stock-edit.png"),"");
	editBtn->setToolTip("Get More Bibles or Enable/Disable Bibles");
	connect(editBtn, SIGNAL(clicked()), LocalBibleManager::inst(), SLOT(getMoreBibles()));
	hboxTop->addWidget(editBtn);
	
	QPushButton *configBtn = new QPushButton(QPixmap(":/data/stock-preferences.png"),"");
	configBtn->setToolTip("Setup Slide Creator Options");
	connect(configBtn, SIGNAL(clicked()), this, SLOT(showConfigMenu()));
	
	QMenu *configMenu = new QMenu(); //configBtn);
	QAction * action;
	
	action = configMenu->addAction("Show Verse Numbers in Text");
	action->setCheckable(true);
	action->setChecked(showVerseNumbers());
	connect(action, SIGNAL(toggled(bool)), this, SLOT(setShowVerseNumbers(bool)));
	
	action = configMenu->addAction("Show Each Verse on a Seperate Slide");
	action->setCheckable(true);
	action->setChecked(showEachVerseOnSeperateSlide());
	connect(action, SIGNAL(toggled(bool)), this, SLOT(setShowEachVerseOnSeperateSlide(bool)));
	
	configMenu->addSeparator();
	
	action = configMenu->addAction("Show Responsive Reading Labels");
	action->setCheckable(true);
	action->setChecked(showResponsiveReadingLabels());
	connect(action, SIGNAL(toggled(bool)), this, SLOT(setShowResponsiveReadingLabels(bool)));
	
	configMenu->addSeparator()->setText("Reference Locations");
	
	action = configMenu->addAction("Show Reference at Top of Each Slide");
	action->setCheckable(true);
	action->setChecked(showFullRefTopEachSlide());
	connect(action, SIGNAL(toggled(bool)), this, SLOT(setShowFullRefTopEachSlide(bool)));
	
	action = configMenu->addAction("Show Reference at Bottom of Each Slide");
	action->setCheckable(true);
	action->setChecked(showFullRefBottomEachSlide());
	connect(action, SIGNAL(toggled(bool)), this, SLOT(setShowFullRefBottomEachSlide(bool)));
 	
 	configMenu->addSeparator();
 	
 	action = configMenu->addAction("Show Reference at Top of First Slide");
	action->setCheckable(true);
	action->setChecked(showFullRefAtFirstTop());
	connect(action, SIGNAL(toggled(bool)), this, SLOT(setShowFullRefAtFirstTop(bool)));
	
	action = configMenu->addAction("Show Reference at Bottom of Last Slide");
	action->setCheckable(true);
	action->setChecked(showFullRefAtBottomLast());
	connect(action, SIGNAL(toggled(bool)), this, SLOT(setShowFullRefAtBottomLast(bool)));
 	
 	configMenu->addSeparator();
 	
	action = configMenu->addAction("Show Reference at Group Start");
	action->setCheckable(true);
	action->setChecked(showFullRefAtStart());
	connect(action, SIGNAL(toggled(bool)), this, SLOT(setShowFullRefAtStart(bool)));
	
	action = configMenu->addAction("Show Reference at Group End");
	action->setCheckable(true);
	action->setChecked(showFullRefAtEnd());
	connect(action, SIGNAL(toggled(bool)), this, SLOT(setShowFullRefAtEnd(bool)));
	
	configMenu->addSeparator();
 	
	action = configMenu->addAction("Append Slides to Existing Group");
	action->setCheckable(true);
	action->setChecked(appendToExistingGroup());
	connect(action, SIGNAL(toggled(bool)), this, SLOT(setAppendToExistingGroup(bool)));
	
	configMenu->addSeparator();
 	
	action = configMenu->addAction("Allow Split Verses in Alternate Groups");
	action->setCheckable(true);
	action->setChecked(allowSplitVersesInAltGroups());
	connect(action, SIGNAL(toggled(bool)), this, SLOT(setAllowSplitVersesInAltGroups(bool)));
	
	//configBtn->setMenu(configMenu);
	m_configMenu = configMenu;
	hboxTop->addWidget(configBtn);
	
	m_configDialog = new QDialog();
	m_configDialog->setWindowTitle("Bible Browser Options");
	
	QVBoxLayout *dialogVbox = new QVBoxLayout(m_configDialog);
	dialogVbox->addWidget(m_configMenu);
	
	
	
	// Setup filter box at the top of the widget
	m_searchBase = new QWidget(this);
	
	QHBoxLayout *hbox = new QHBoxLayout(m_searchBase);
	SET_MARGIN(hbox,0);
	
	QLabel *label = new QLabel("Sea&rch:");
	m_search = new QLineEdit(m_searchBase);
	label->setBuddy(m_search);
	setFocusProxy(m_search);
	
	// add book name completion
	QCompleter *completer = new QCompleter(BibleVerseRef::bookCompleterList(), m_search);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setWrapAround(true);
	m_search->setCompleter(completer);
	
	QPushButton * btnSearch = new QPushButton(QPixmap(":/data/stock-find.png"),"");
	btnSearch->setToolTip("Seach BibleGateway.com for the verse reference entered on the left (ENTER)");
	
	m_liveBtn = new QPushButton(QPixmap(":/data/stock-fullscreen.png"),"");
	m_liveBtn->setToolTip("Send the selected verses to the output without adding to the document (Ctrl+E)");
	m_liveBtn->setVisible(false);
	m_liveBtn->setShortcut(tr("Ctrl+E"));
	
	m_addBtn = new QPushButton(QPixmap(":/data/stock-add.png"),"");
	m_addBtn->setToolTip("Add verses below as a slide group to current document (Ctrl+D)");
	m_addBtn->setVisible(false);
	m_addBtn->setShortcut(tr("Ctrl+D"));
	
	m_spinnerLabel = new QLabel();
	m_spinnerLabel->setVisible(false);
	
	m_spinnerLabel->setMovie(new QMovie(":/data/ajax-loader.gif",QByteArray(),this));
	m_spinnerLabel->setToolTip("Loading Verses...");
	
	hbox->addWidget(label);
	hbox->addWidget(m_search);
	hbox->addWidget(btnSearch);
	hbox->addWidget(m_liveBtn);
	hbox->addWidget(m_addBtn);
	hbox->addWidget(m_spinnerLabel);
	
	//connect(m_search, SIGNAL(textChanged(const QString &)), this, SLOT(loadVerses(const QString &)));
	connect(m_search, SIGNAL(returnPressed()), this, SLOT(searchReturnPressed()));
	connect(btnSearch, SIGNAL(clicked()), this, SLOT(searchReturnPressed()));
	connect(m_liveBtn, SIGNAL(clicked()), this, SLOT(sendVersesLive()));
	connect(m_addBtn, SIGNAL(clicked()), this, SLOT(addVersesToDocument()));
	
	// Add reference controls (btns to expand selection, etc)
	
	m_refBase = new QWidget(this);
	m_refBase->setVisible(false);

	QHBoxLayout *hbox2 = new QHBoxLayout(m_refBase);
	SET_MARGIN(hbox2,0);
	
	//m_referenceLabel = new QLabel(m_refBase);
	
	m_addPrevVerseBtn = new QPushButton(QPixmap(":/data/stock-go-back.png"),"");
	m_addPrevVerseBtn->setToolTip(tr("Add previous verse from this chapter"));
	
	m_addAnotherVerseBtn = new QPushButton(QPixmap(":/data/stock-go-forward.png"),"");
	m_addAnotherVerseBtn->setToolTip(tr("Add next verse from this chapter"));
	
	m_getChapterBtn = new QPushButton(QPixmap(":/data/stock-goto-top.png"),"");
	m_getChapterBtn->setToolTip(tr("Get entire chapter"));
	
	m_prevChapterBtn = new QPushButton(QPixmap(":/data/stock-goto-first.png"),"");
	m_prevChapterBtn->setToolTip(tr("Get previous chapter"));
	
	m_nextChapterBtn = new QPushButton(QPixmap(":/data/stock-goto-last.png"),"");
	m_nextChapterBtn ->setToolTip(tr("Get next chapter"));
	
	//hbox2->addWidget(m_referenceLabel);
	hbox2->addWidget(m_addPrevVerseBtn);
	hbox2->addWidget(m_addAnotherVerseBtn);
	hbox2->addWidget(m_getChapterBtn);
	hbox2->addWidget(m_prevChapterBtn);
	hbox2->addWidget(m_nextChapterBtn);
	hbox2->addStretch(1);
	
	
	connect(m_addPrevVerseBtn, SIGNAL(clicked()), this, SLOT(addPrevVerse()));
	connect(m_addAnotherVerseBtn, SIGNAL(clicked()), this, SLOT(addAnotherVerse()));
	connect(m_getChapterBtn, SIGNAL(clicked()), this, SLOT(getChapter()));
	connect(m_prevChapterBtn, SIGNAL(clicked()), this, SLOT(prevChapter()));
	connect(m_nextChapterBtn, SIGNAL(clicked()), this, SLOT(nextChapter()));
	
	// add text preview
	m_preview = new QTextEdit(this);
	m_preview->setReadOnly(true);

	QFont font;
	font.setFamily("Courier");
	font.setFixedPitch(true);
	font.setPointSize(12);
	m_preview->setFont(font);
	
	// create the divider line between the search box and verse nav controls
	QFrame * line = new QFrame();
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);

	// Setup the template selector widget
	m_tmplWidget = new TemplateSelectorWidget(SlideTemplateManager::Bible,"Style:",this);
	if(m_template)
		m_tmplWidget->setSelectedGroup(m_template);
	else
		m_template = m_tmplWidget->selectedGroup();
		
	connect(m_tmplWidget, SIGNAL(currentGroupChanged(SlideGroup*)), this, SLOT(templateChanged(SlideGroup*)));
	
	// Finally, add all the widgets to the vertical layout
	vbox->addWidget(m_searchBase);
	vbox->addWidget(line);
	vbox->addWidget(m_refBase);
	vbox->addWidget(m_preview);
	vbox->addWidget(m_tmplWidget);
	
	
}

void BibleBrowser::showConfigMenu()
{
	m_configDialog->exec();
	//m_configMenu->exec(QCursor::pos());
}

void BibleBrowser::templateChanged(SlideGroup* group)
{
	if(m_template && group->property("-auto-template").toBool())
	{
		// we auto-created this template in getTemplate() - delete it since we dont need it now
		delete m_template;
		m_template = 0;
	}
	
	m_template = group;
	saveSettings();
}

void BibleBrowser::searchReturnPressed() 
{
	m_search->selectAll();
// 	QModelIndex idx = m_songListModel->indexForRow(0);
// 	if(idx.isValid())
// 		songDoubleClicked(idx);
	searchTextChanged(m_search->text());
}

void BibleBrowser::searchTextChanged(const QString &text)
{
	//m_songListModel->filter(text);
	saveSettings();
	
	m_spinnerLabel->setVisible(!text.isEmpty());
	if(!text.isEmpty())
	{
		m_spinnerLabel->movie()->start();
		QString versionCode = m_versionCombo->itemData(m_versionCombo->currentIndex()).toString();
		BibleVerseRef ref = BibleVerseRef::normalize(text, BibleVersion(versionCode, versionCode));
		if(!ref.valid())
		{
			m_spinnerLabel->movie()->stop();
			m_spinnerLabel->setVisible(false);
			m_search->selectAll();
			m_preview->setPlainText("");
			QMessageBox::warning(this,"Invalid Reference",QString(tr("Sorry, but %1 doesn't seem to be a valid bible reference.")).arg(text));
			return;
		}
		
		qDebug() << "BibleBrowser::searchTextChanged: "<<text;
		if(m_bible->findReference(ref))
		{
			referenceAvailable(ref, m_bible->loadReference(ref));
		}
		else
		{
			m_bible->downloadReference(ref);
		}
		
		AppSettings::sendCheckin("/bible/search",ref.toString(true));
	}
	else
	{
		m_spinnerLabel->movie()->stop();
		m_preview->setPlainText("");
		m_addBtn->setVisible(false);
		m_liveBtn->setVisible(false);
		m_refBase->setVisible(false);
	}
	
// 	QModelIndex idx = m_songListModel->indexForRow(0);
// 	if(idx.isValid())
// 		m_songList->setCurrentIndex(idx);
}

void BibleBrowser::referenceAvailable(const BibleVerseRef& reference, const BibleVerseList & list)
{
	m_currentList = list;
	m_currentRef = reference;
	
	if(m_lastDownloadAutomated)
	{
		m_downloadComplete = true;
		m_lastDownloadAutomated = false; // reset for next DL
		return;
	}
	
	m_spinnerLabel->movie()->stop();
	m_spinnerLabel->setVisible(false);
	if(list.isEmpty())
	{
		m_preview->setHtml(QString(tr("<font color='red'>Sorry, \"<b>%1</b>\" was not found!</font>")).arg(reference.toString(true)));
	}
	else
	{
		m_addBtn->setVisible(true);
		m_liveBtn->setVisible(true);
		m_refBase->setVisible(true);

		QStringList listText;
		foreach(BibleVerse verse, list)
		{
			listText << QString("<sup>%1</sup>%2").arg(verse.verseNumber()).arg(verse.text());
		}
		
		//m_referenceLabel->setText(QString("<h2>%1</h2>").arg(reference.toString()));
		
		//m_preview->setHtml(QString("<p>%2</p>").arg(listText.join("\n")));
		m_preview->setHtml(QString("<h2>%1</h2><p>%2</p>").arg(reference.toString()).arg(listText.join("\n")));
	}
	
	if(isLastGeneratedGroupStillLive())
		sendVersesLive();
}

void BibleBrowser::addVersesToGroup(const QString& text, SlideGroup *dest)
{
	// Normalize the requested ref
	QString versionCode = m_versionCombo->itemData(m_versionCombo->currentIndex()).toString();
	BibleVerseRef ref = BibleVerseRef::normalize(text, BibleVersion(versionCode,versionCode));
	if(!ref.valid())
	{
		qDebug() << "BibleBrowser::addVersesToGroup: Invalid ref: "<<text<<", not added";
		return;
	}
	
	// Lookup in cache, if present, just process, otherwise initate download
	if(m_bible->findReference(ref))
	{
		m_currentRef = ref;
		m_currentList = m_bible->loadReference(ref);
	}
	else
	{
		m_lastDownloadAutomated = true;
		m_downloadComplete = false;
		
		m_bible->downloadReference(ref);
		
		while(!m_downloadComplete)
		{
			// Allow download to complete and signals processed,
			// but block execution because this slot must execute async
			qApp->processEvents();
		}
		
		m_lastDownloadAutomated = false;
		m_downloadComplete = false;
	}
	
	m_tempDest = dest;
	if(!dest)
		m_tempDest = new SlideGroup();
	
	// Do the bulk of the work here
	createSlideGroup(getTemplate(), true);
	
	if(!dest)
		MainWindow::mw()->currentDocument()->addGroup(m_tempDest);
	
	m_tempDest = 0; // reset so UI doesn't behave oddly
	
	// That's all, folks!
}

void BibleBrowser::loadVerses(const QString & filter)
{
	searchTextChanged(filter);
	m_search->setText(filter);
}

void BibleBrowser::addVersesToDocument()
{
	SlideGroup *group = createSlideGroup(getTemplate(), true);
	if(group && !appendToExistingGroup())
		MainWindow::mw()->currentDocument()->addGroup(group);
	m_search->selectAll();
	
	AppSettings::sendCheckin("/bible/add-verses-to-document",m_currentRef.toString());
}

void BibleBrowser::sendVersesLive()
{
	SlideGroup *group = createSlideGroup(getTemplate());
	if(group)
	{
		MainWindow::mw()->setLiveGroup(group);
		cullGeneratedGroups();
		m_generatedGroups << group;
		
		AppSettings::sendCheckin("/bible/live",m_currentRef.toString());
	}
	m_search->selectAll();
}
	
void BibleBrowser::addPrevVerse()
{
	//qDebug() << "m_currentRef.verseNumber():"<<m_currentRef.verseNumber()<<", m_currentRef.verseRange(): "<<m_currentRef.verseRange();
	if(m_currentRef.verseRange() > 0 && m_currentRef.verseRange() != m_currentRef.verseNumber())
		m_currentRef.setVerseNumber(m_currentRef.verseNumber() - 1);
	else
	{
		m_currentRef.setVerseNumber(m_currentRef.verseNumber() - 1);
		m_currentRef.setVerseRange(-1);
	}
	AppSettings::sendCheckin("/bible/prev-verse",m_currentRef.toString());
	
	if(m_currentRef.verseNumber() < 1)
		m_currentRef.setVerseNumber(1);
	m_attemptAutoLive = true;
	loadVerses(m_currentRef.toString());
}

void BibleBrowser::addAnotherVerse()
{
	//qDebug() << "m_currentRef.verseNumber():"<<m_currentRef.verseNumber()<<", m_currentRef.verseRange(): "<<m_currentRef.verseRange();
	if(m_currentRef.verseRange() > 0 && m_currentRef.verseRange() != m_currentRef.verseNumber())
		m_currentRef.setVerseRange(m_currentRef.verseRange() + 1);
	else
	{
		m_currentRef.setVerseNumber(m_currentRef.verseNumber() + 1);
		m_currentRef.setVerseRange(-1);
	}
	AppSettings::sendCheckin("/bible/next-verse",m_currentRef.toString());
	
	m_attemptAutoLive = true;
	loadVerses(m_currentRef.toString());
}

void BibleBrowser::getChapter()
{
	QString versionCode = m_versionCombo->itemData(m_versionCombo->currentIndex()).toString();
	AppSettings::sendCheckin("/bible/get-chapter",m_currentRef.toString());
	
	loadVerses(QString("%1 %2").arg(m_currentRef.book().name()).arg(m_currentRef.chapter().chapterNumber()));
}

void BibleBrowser::prevChapter()
{
	QString versionCode = m_versionCombo->itemData(m_versionCombo->currentIndex()).toString();
	AppSettings::sendCheckin("/bible/prev-chapter",m_currentRef.toString());
	
	int prev = m_currentRef.chapter().chapterNumber()-1;
	if(prev > 0)
		loadVerses(QString("%1 %2").arg(m_currentRef.book().name()).arg(prev));
}

void BibleBrowser::nextChapter()
{
	QString versionCode = m_versionCombo->itemData(m_versionCombo->currentIndex()).toString();
	AppSettings::sendCheckin("/bible/next-chapter",m_currentRef.toString());
	
	int next = m_currentRef.chapter().chapterNumber()+1;
	loadVerses(QString("%1 %2").arg(m_currentRef.book().name()).arg(next));
}

bool BibleBrowser::isLastGeneratedGroupStillLive()
{
	if(m_generatedGroups.isEmpty())
		return false;
	return isGroupLive(m_generatedGroups.last());
}

void BibleBrowser::cullGeneratedGroups()
{
	QList<SlideGroup*> deleted;
	
	// This arbitrary number (5) below is here for this reason:
	// Even though isGroupLive() may be false, this only indicates
	// the group is not the LIVE group - the group still may be the "Fade out" group in a transition.
	// Therefore, we leave a buffer of 5 groups sitting around till we go and delete them.
	
		
	foreach(SlideGroup * group, m_generatedGroups)
		if(!isGroupLive(group))
			deleted << group;
	foreach(SlideGroup * group, deleted)
	{
		if(m_generatedGroups.size() < 5)
			return;
			
		group->deleteLater();
		m_generatedGroups.removeAll(group);
	}
}

bool BibleBrowser::isGroupLive(SlideGroup* group)
{
	QList<Output*> outputs = AppSettings::outputs();
	foreach(Output* output, outputs)
		if(MainWindow::mw()->outputInst(output->id())->slideGroup() == group)
			return true;	
	return false;
}


#ifndef QStringPair
typedef QPair<QString,QString> QStringPair;
#endif

void BibleBrowser::setupVersionCombo()
{
	int currentVersionIndex = m_versionCombo->currentIndex();
	QString currentVersionCode = m_versionCombo->itemData(currentVersionIndex).toString();
	
	m_versionCombo->clear();
	
	QList<QStringPair> list;
	
	QList<BibleDataPlaceholder*> localBibles = LocalBibleManager::inst()->biblePlaceholders();
	
	foreach(BibleDataPlaceholder *data, localBibles)
		list.append(qMakePair(data->code(),QString("%1").arg(data->name())));
		
	// Add in Bibles from BibleGateway.com - feel free to add more codes here if desired
	list.append(qMakePair(QString("NIV1984"), QString(tr("BibleGateway.com: NIV 1984 "))));
	list.append(qMakePair(QString("NIV"),     QString(tr("BibleGateway.com: NIV Latest"))));
	list.append(qMakePair(QString("NIRV"),    QString(tr("BibleGateway.com: New International Reader's Version"))));
	list.append(qMakePair(QString("NIVUK"),   QString(tr("BibleGateway.com: NIV - UK"))));

	#define MAX_VERSION_NAME_LENGTH 32
	foreach(QStringPair pair, list)
	{
		QString code = pair.first;
		QString text = pair.second;
		m_versionCombo->addItem(QString("%1%2").arg(text.left(MAX_VERSION_NAME_LENGTH)).arg(text.length() > MAX_VERSION_NAME_LENGTH ? "..." : ""), code);
	}
	
	// Default bible shipped with DViz is KJV
	if(currentVersionIndex > -1)
		m_versionCombo->setCurrentIndex(m_versionCombo->findData(currentVersionCode));
	else
		m_versionCombo->setCurrentIndex(m_versionCombo->findData("ENGLISHKJV"));
}

void BibleBrowser::setupTextBox(TextBoxItem *tmpText)
{
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
}

SlideGroup * BibleBrowser::getTemplate()
{
	if(!m_template)
	{
		SlideGroup *group = new SlideGroup();
		Slide *slide = new Slide();
	
		AbstractVisualItem * bg = dynamic_cast<AbstractVisualItem*>(slide->background());

		bg->setFillType(AbstractVisualItem::Solid);
		bg->setFillBrush(Qt::blue);
		
		group->addSlide(slide); // first slide, used for "show ref at start" and "show ref at end"
		group->addSlide(slide->clone()); // second slide, used for the individual verse slides
		
		// Allow this template to be deleted if user selects one using the SlideTemplateManager UI
		group->setProperty("-auto-template",true);
		
		// We dont setup the individual text boxes - those will be created as needed in getTemplateSlide(SlideGroup *, int number)
		
		m_template = group;
	}
	
	return m_template;
}

Slide *BibleBrowser::getTemplateSlide(SlideGroup *templateGroup, int slideNum)
{
	// This will be adjusted below if we were told to add responsive reading labels
	QRect fitRect = AppSettings::adjustToTitlesafe(MainWindow::mw()->standardSceneRect());
	// Used for placement of labels around the edges of the slide
	QRect slideRect = AppSettings::adjustToTitlesafe(MainWindow::mw()->standardSceneRect());
	
	

	if(slideNum <= (showFullRefAtStart() ? 0:-1))
	{
		Slide *slide = templateGroup->at(0);
		if(!slide)
		{
			slide = new Slide();
		
			AbstractVisualItem * bg = dynamic_cast<AbstractVisualItem*>(slide->background());
	
			bg->setFillType(AbstractVisualItem::Solid);
			bg->setFillBrush(Qt::blue);
			
			templateGroup->addSlide(slide);
		}
		
		// We'll need more than one slide, but the user didn't add a second slide to the template group,
		// so, assuming they've set the background they want on this first slide, 
		// duplicate and add it to the template before adding our textbox
		// (Basically, this is for backwards compatibility with background-only templates)
		if(showFullRefAtStart() && templateGroup->numSlides() == 1)
		{
			templateGroup->addSlide(slide->clone());
		}
		
		TextBoxItem *tmpText = findTextItem(slide, "#text");
		
		if(!tmpText)
		{
			tmpText = new TextBoxItem();
			tmpText->setItemId(ItemFactory::nextId());
			tmpText->setItemName(QString("TextBoxItem%1").arg(tmpText->itemId()));
			
			tmpText->setText("<center><span style='font-family:Constantia,Times New Roman,Serif;font-weight:800'><b>#text</b></span></center>");
			tmpText->changeFontSize(72);
			tmpText->setContentsRect(fitRect);
			setupTextBox(tmpText);
			
			slide->addItem(tmpText);
		}
		
		return slide;
	}
	else
	{
		// Here we prep the responsive reading label object and adjust the "fitRect" (from above) to allow for space at the top of each slide
		TextBoxItem * labelItem = 0;
		QSize labelSize;
		
		if(   showResponsiveReadingLabels()
		   || showFullRefAtFirstTop()
		   || showFullRefTopEachSlide()
		   || showFullRefBottomEachSlide()
		   || showFullRefAtBottomLast())
		{
			labelItem = new TextBoxItem();
			labelItem->setItemId(ItemFactory::nextId());
			labelItem->setItemName(QString("TextBoxItem%1").arg(labelItem->itemId()));
			
			// Note: We're not actually *using* this label at the top - we're just using this string to get the *size* of the label
			labelItem->setText(QString("<span style='font-family:Calibri,Tahoma,Arial,Sans-Serif;font-weight:800'><b>Congregation</b></span></center>"));;
			labelItem->changeFontSize(40); // TODO...configurable?
			
			labelSize = labelItem->findNaturalSize();
			
			setupTextBox(labelItem);
			labelItem->setFillBrush(Qt::yellow);
		}
		
		
		Slide *slide = templateGroup->at(1);
		
		if(!slide && !showFullRefAtStart())
			slide = templateGroup->at(0);
			
		if(!slide)
		{
			slide = new Slide();
		
			AbstractVisualItem * bg = dynamic_cast<AbstractVisualItem*>(slide->background());
	
			bg->setFillType(AbstractVisualItem::Solid);
			bg->setFillBrush(Qt::blue);
			
			templateGroup->addSlide(slide);
		}
		
		
		QRect currentFitRect = fitRect;
		
		if((showFullRefAtFirstTop() && slideNum==(showFullRefAtStart() ? 1:0)) || showFullRefTopEachSlide())
			currentFitRect.setY(currentFitRect.y() + labelSize.height());
		
		if(showResponsiveReadingLabels())
			// move down to make space for the label
			currentFitRect.setY(currentFitRect.y() + labelSize.height());
			
		if(showFullRefBottomEachSlide() || showFullRefAtBottomLast()) // HACK to test for showFullRefAtBottomLast here!
			currentFitRect.setBottom( currentFitRect.bottom() - labelSize.height());
			
		TextBoxItem *tmpText = findTextItem(slide, "#verses");
		if(!tmpText)
		{
			qDebug() << "BibleBrowser::getTemplateSlide(): slide:"<<slideNum<<", #verses"; 
			tmpText = new TextBoxItem();
			tmpText->setItemId(ItemFactory::nextId());
			tmpText->setItemName(QString("TextBox%1").arg(tmpText->itemId()));
			tmpText->setZValue(99);
			tmpText->setText("<span style='font-family:Calibri,Tahoma,Arial,Sans-Serif;font-weight:800'><b>#verses</b></span>");
			tmpText->changeFontSize(48);
			tmpText->setContentsRect(currentFitRect);
			slide->addItem(tmpText);
			setupTextBox(tmpText);
			
		}
		
		if(m_showResponsiveReadingLabels || showFullRefAtFirstTop() || showFullRefTopEachSlide())
		{
			int responsiveYPos = 0;
			if((showFullRefAtFirstTop() && slideNum==(showFullRefAtStart() ? 1:0)) || showFullRefTopEachSlide())
			{
				TextBoxItem *labelText = findTextItem(slide, "#ref");
				if(!labelText)
				{
					
					// Clone the label so we dont have to re-create it
					TextBoxItem * label = dynamic_cast<TextBoxItem*>(labelItem->clone());
					label->setFillBrush(Qt::white);
					label->setText("<span style='font-family:Calibri,Tahoma,Arial,Sans-Serif;font-weight:800'><b>#ref</b></span></center>");
					label->changeFontSize(40);
					
					// Adjust the size of the label
					QSize sz = label->findNaturalSize(fitRect.width());
					label->setContentsRect(QRectF(slideRect.x(),slideRect.y(),fitRect.width(),sz.height()));
					responsiveYPos = sz.height() + slideRect.y();
					
					//qDebug() << "responsive reading: #"<<labelCounter<<", labelText:"<<labelText<<", sz:"<<sz<<", rect:"<<label->contentsRect();
					
					slide->addItem(label);
				}
			}
			
			if(showResponsiveReadingLabels())
			{
				TextBoxItem *labelText = findTextItem(slide, "#reading");
				if(!labelText)
				{
					// Clone the label so we dont have to re-create it
					TextBoxItem * label = dynamic_cast<TextBoxItem*>(labelItem->clone());
					label->setText("<span style='font-family:Calibri,Tahoma,Arial,Sans-Serif;font-weight:800'><b>#reading</b></span></center>");
					label->changeFontSize(40);
					
					// Adjust the size of the label
					QSize sz = label->findNaturalSize(fitRect.width());
					label->setContentsRect(QRectF(slideRect.x(),responsiveYPos,fitRect.width(),sz.height()));
					
					//qDebug() << "responsive reading: #"<<labelCounter<<", labelText:"<<labelText<<", sz:"<<sz<<", rect:"<<label->contentsRect();
					
					slide->addItem(label);
				}
			}
		}
		
		if(showFullRefBottomEachSlide())
		{
			TextBoxItem *labelText = findTextItem(slide, "#ref-bottom");
			if(!labelText)
			{
				// Clone the label so we dont have to re-create it
				TextBoxItem * label = dynamic_cast<TextBoxItem*>(labelItem->clone());
				label->setFillBrush(Qt::white);
				label->setText("<span style='font-family:Calibri,Tahoma,Arial,Sans-Serif;font-weight:800'><b>#ref-bottom</b></span></center>");
				label->changeFontSize(40);
				
				// Adjust the size of the label
				QSize sz = label->findNaturalSize(fitRect.width());
				label->setContentsRect(QRectF(slideRect.x(),slideRect.bottom() - sz.height(),fitRect.width(),sz.height()));
				
				//qDebug() << "responsive reading: #"<<labelCounter<<", labelText:"<<labelText<<", sz:"<<sz<<", rect:"<<label->contentsRect();
				
				slide->addItem(label);
			}
		}
			
		delete labelItem;
		
		return slide;
	}
	
}

Slide *BibleBrowser::createTitleSlide(SlideGroup *templateGroup, SlideGroup *group, bool isFirst)
{
	QString refString = m_currentRef.toString();
		
	// Add basic text to first slide indicating it's responsive reading
	if(showResponsiveReadingLabels())
		if(isFirst)
			refString = tr("Please stand as we read together<br>%1").arg(refString);
		else
			refString = tr("Please be seated.<br><hr size=1>%1").arg(refString);
	
	Slide *slide = getTemplateSlide(templateGroup, 0)->clone();
	TextBoxItem *text = findTextItem(slide, "#text");
	if(text)
	{
		//int currentMinTextSize = text->findFontSize();
		mergeTextItem(text, refString);
		//text->fitToSize(text->contentsRect().size().toSize(), currentMinTextSize, 99);
		intelligentCenterTextbox(text);
		addSlideWithText(group, slide, text);
		
		slide->setSlideName(m_currentRef.toString());
		
		return slide;
	}
	else
	{
		// Couldn't find #text (even though getTemplateSlide() should've created it!
		delete slide; 
		slide = 0;
	}
	
	return 0;
}

void BibleBrowser::setupOptionalLabels(Slide *currentSlide, int slideNumber)
{
	int MinLabelSize = 40;
	
	// In the future, these labels should be an option in the UI - for now, we'll hardcocde them
	const QString leaderLabel  = tr("Leader:");
	const QString readingLabel = tr("Congregation:");

	if((showFullRefAtFirstTop() && slideNumber==1)
		|| showFullRefTopEachSlide())
	{
		TextBoxItem *label = findTextItem(currentSlide, "#ref");
		mergeTextItem(label, m_currentRef.toString());
		conditionallyFitAndAlign(label,MinLabelSize);
	}
	else
	{
		TextBoxItem *label = findTextItem(currentSlide, "#ref");
		if(label)
			label->setOpacity(0.);
	}
	
	if(showResponsiveReadingLabels())
	{
		QString labelText = slideNumber % 2 == 0 ? leaderLabel : readingLabel;
		
		TextBoxItem *label = findTextItem(currentSlide, "#reading");
		mergeTextItem(label, labelText);
		conditionallyFitAndAlign(label,MinLabelSize);
	}
	else
	{
		TextBoxItem *label = findTextItem(currentSlide, "#reading");
		if(label)
			label->setOpacity(0.);
	}

	if(showFullRefBottomEachSlide())
	{
		TextBoxItem *label = findTextItem(currentSlide, "#ref-bottom");
		mergeTextItem(label, m_currentRef.toString());
		conditionallyFitAndAlign(label,MinLabelSize);
	}
	else
	{
		TextBoxItem *label = findTextItem(currentSlide, "#ref-bottom");
		if(label)
			label->setOpacity(0.);
	}
}

SlideGroup * BibleBrowser::createSlideGroup(SlideGroup *templateGroup, bool allowAppend)
{
	BibleVerseList verseList = m_currentList;
	
	if(verseList.isEmpty())
	{
		QMessageBox::warning(this,"No Verses Found","Sorry, no verses were found!");
		return 0;
	}
	
	// Adding entire chapters can take 10 - 30 seconds, so a progress dialog makes the UI more "friendly"
	QProgressDialog progress;
	progress.setWindowIcon(QIcon(":/data/icon-d.png"));
	//.arg(m_currentRef.toString(true)
	progress.setWindowTitle(QString(tr("Creating Slides")));
	progress.setLabelText(QString(tr("Setting up %1...")).arg(m_currentRef.toString(true)));
	
	int MinLabelSize = 40;
	int MinTextSize = 48;
	int MaxTextSize = 72;

	// Add the verse numbers here because after this point,
	// all we have to work with is a blob of text, we loose the BibleVerse objects here.
	QStringList lines;
	QString prefix;
	foreach(BibleVerse verse, verseList)
	{
		prefix = m_showVerseNumbers ? 
			prefix = QString("<sup>%1</sup>").arg(verse.verseNumber())
			: "";
		
		lines << prefix + verse.text();
	}

	// If user did NOT request each verse on seperate slide, then combine the verses into a blob of text
	// and break them up at sentance markers and other break points to fit more text on each slide.
	// Of course, if the user wants a single verse per slide, than this is all irrelevant.
	if(!showEachVerseOnSeperateSlide())
	{
		QString blob = lines.join(" ");
		lines.clear();
		
		// Do a fancy split on various breaking points in the text so we can fit more
		// text per slide. If we just split on line-end (\n), we potentially could get 
		// a line that wouldn't fit on the slide, or have a lot of empty space on the
		// end of a slide. Using a variety of break points makes the text fit better.
		int pos = 0;
		int lastPos = 0;
		QRegExp rx("[-;,\n:\\.]");
		while((pos = rx.indexIn(blob,pos)) != -1)
		{
			lines.append(blob.mid(lastPos,pos-lastPos+1));
			pos += rx.matchedLength();
			lastPos = pos;
		}
	}
	
	progress.setMaximum(lines.size());

	// This will be adjusted below if we were told to add responsive reading labels
	QRect fitRect = AppSettings::adjustToTitlesafe(MainWindow::mw()->standardSceneRect());

	SlideGroup *group;
	 
	if(m_tempDest)
	{
		group = m_tempDest;
		if(group->groupTitle().isEmpty())
			group->setGroupTitle(m_currentRef.toString() + (showResponsiveReadingLabels() ? " - Responsive" : ""));
	}
	else
	if(allowAppend && appendToExistingGroup())
	{
		ChooseGroupDialog d;
		d.exec();
		group = d.selectedGroup();
		if(!group)
			return 0;
	}
	else
	{
		group = new SlideGroup();
		group->setGroupTitle(m_currentRef.toString() + (showResponsiveReadingLabels() ? " - Responsive" : ""));
	}
	

	TextBoxItem * tmpText = 0;
	int realHeight=0;
	
	// Prepare the alternate groups if present in template, only if not appending.
	// If appending, assume group already has appros alt groups if desired,
	// and from here on out (after this block), we only create alt slides on *our* group
	// if we have an alt group *already* on *our* group AND an alt group in the template for that output
	QList<Output*> allOut = AppSettings::outputs();
	//if(!appendToExistingGroup())
	{
		foreach(Output *out, allOut)
		{
			SlideGroup *outputTemplate = templateGroup->altGroupForOutput(out);
			if(outputTemplate)
			{
				//qDebug() << "BibleBrowser::createSlideGroup(): [prep] Creating alternate group from template for output: "<<out->name();
				
				SlideGroup *oldGroup = group->altGroupForOutput(out);
				if(!oldGroup)
				{
					SlideGroup *altGroup = new SlideGroup();
					altGroup->setGroupTitle(tr("Alternate %1 for %2").arg(m_currentRef.toString()).arg(out->name()));
							
					//qDebug() << "BibleBrowser::createSlideGroup(): [prep] * Added new alt group for: "<<out->name();
					group->setAltGroupForOutput(out, altGroup);
				}
				else
				{
					//qDebug() << "BibleBrowser::createSlideGroup(): [prep] * Alt group already existed for: "<<out->name();
				}
				
				//qDebug() << "BibleBrowser::createSlideGroup(): [prep] *DONE* creating alternate group from template for output: "<<out->name();
				
			}
		}
	}
	
	// If the user asked for it, add the verse reference as the first slide, centered on the screen.
// 	Slide * startSlide = 0;
	if(showFullRefAtStart())
	{
		Slide *primaryTitleSlide = 
			createTitleSlide(templateGroup, // using slide from this template
					group,          // add to this group
					true);          // first slide (as opposed to last/closing slide)
			
		foreach(Output *out, allOut)
		{
			SlideGroup *outputTemplate = templateGroup->altGroupForOutput(out);
			if(outputTemplate)
			{
				//qDebug() << "BibleBrowser::createSlideGroup(): [prep] Creating alternate group from template for output: "<<out->name();
				
				if(SlideGroup *altGroup = group->altGroupForOutput(out))
				{
					Slide *altTitleSlide = 
						createTitleSlide(outputTemplate, // using slide from this template
						         altGroup,       // add to this group
						         true);          // first slide (as opposed to last/closing slide)
					
					altTitleSlide->setPrimarySlideId(primaryTitleSlide->slideId());
				}
			}
		}
	}

	
	// used to alternate the responsive reading labels
// 	int labelCounter = 0;
	
	int slideNumber = group->numSlides();
	
	// hold a pointer to the current slide outside the loop
	// so that at the end of the loop we can add the reference to the bottom if showFullRefAtBottomLast() is enabled
	Slide * currentSlide = 0;
	
	// Main logic block - loop over all the "lines" in the verseList (really, text fragments),
	// and try to fit as many of them as we can on a slide
	QStringList tmpList;
	
	// Use a boolean flag to ask for 'tmpText' to be recreated, instead of setting tmpText to 0 at end of loop because
	// we need to use tmpText outside the loop at the end if showFullRefAtBottomLast() is true
	bool recreateTextBox = true;
	
	int currentMinTextSize = MinTextSize;
	
	for(int x=0; x<lines.size(); x++)
	{
		//qDebug() << "BibleBrowser::createSlideGroup(): loop start, slideNumber:"<<slideNumber;
		if(tmpList.isEmpty() &&
		   lines[x].trimmed().isEmpty())
		   continue;

		tmpList.append(lines[x]);
		
		progress.setValue(x);
		
		if(recreateTextBox)
		{
			//qDebug() << "BibleBrowser::createSlideGroup(): recreateTextBox, slideNumber:"<<slideNumber;
			currentSlide = getTemplateSlide(templateGroup, slideNumber)->clone();
			tmpText = findTextItem(currentSlide, "#verses");
			currentMinTextSize = (int)tmpText->findFontSize();
			
			//qDebug() << "BibleBrowser::createSlideGroup(): recreateTextBox, got currentSlide:"<<currentSlide<<", tmpText:"<<tmpText;
		}
		
		mergeTextItem(tmpText, tmpList.join(""));
		
		QRect currentFitRect = tmpText->contentsRect().toRect();
		
		//realHeight = tmpText->fitToSize(fitSize,MinTextSize,MaxTextSize);
		realHeight = tmpText->fitToSize(currentFitRect.size(), currentMinTextSize, MaxTextSize);
		//qDebug() << "x:"<<x<<", realHeight:"<<realHeight<<", currentFitRect:"<<currentFitRect;
		
		// If the 'realHeight' is <0, it means that it didnt fit on the slide.
		// Therefore, we've found the max # of text frags that fit on this slide
		// so we should create a new slide, add the text, and then start searching again.
		// This block also handles each verse on a seperate slide, in which case a single line in lines[] is a single verse.
		if(realHeight < 0 || realHeight > currentFitRect.height() || showEachVerseOnSeperateSlide())
		{
			// More than one line, so the last line is the line that made the slide overflow the screen - 
			// therefore take it off and return it to the buffer for the next slide to use.
			if(!showEachVerseOnSeperateSlide() &&
			    tmpList.size() > 1)
			{
				// return last line to the file buffer
				QString line = tmpList.takeLast();
				lines.prepend(line);
				
				mergeTextItem(tmpText, tmpList.join(""));
				
				realHeight = tmpText->fitToSize(currentFitRect.size(),currentMinTextSize,MaxTextSize);
			}

			// Verse slide
			intelligentCenterTextbox(tmpText);
			addSlideWithText(group, currentSlide, tmpText);
			
			//currentSlide = addSlide(group,tmpText,realHeight,currentFitRect,tmpList.join("\n"));
			QString slideName =
				showEachVerseOnSeperateSlide() ?
				(tr("%1v %2")
				.arg(appendToExistingGroup() ? tr("%1: ").arg(m_currentRef.toString()) : "")
				.arg(verseList[x].verseNumber())) :
				"";
			
			qDebug() << "BibleBrowser::createSlideGroup(): [verse] index "<<x<<", slideName: "<<slideName;
			
			if(showEachVerseOnSeperateSlide())
				currentSlide->setSlideName(slideName);
			
			slideNumber++;
			
			setupOptionalLabels(currentSlide, slideNumber);
			
			//int verseListIndex = x;
			foreach(Output *out, allOut)
			{
				SlideGroup *outputTemplate = templateGroup->altGroupForOutput(out);
				if(outputTemplate)
				{
					//qDebug() << "BibleBrowser::createSlideGroup(): [verse] Creating verse slide from template for output: "<<out->name();
					
					if(SlideGroup *altGroup = group->altGroupForOutput(out))
					{
						if(allowSplitVersesInAltGroups())
						{
							QStringList slideLines = tmpList;
							
							QString blob = slideLines.join(" ");
							slideLines.clear();
							
							// Do a fancy split on various breaking points in the text so we can fit more
							// text per slide. If we just split on line-end (\n), we potentially could get 
							// a line that wouldn't fit on the slide, or have a lot of empty space on the
							// end of a slide. Using a variety of break points makes the text fit better.
							int pos = 0;
							int lastPos = 0;
							QRegExp rx("[-;,\n:\\.]");
							while((pos = rx.indexIn(blob,pos)) != -1)
							{
								slideLines.append(blob.mid(lastPos,pos-lastPos+1));
								pos += rx.matchedLength();
								lastPos = pos;
							}
							
							//qDebug() << "[alt slide] slideLines:" <<slideLines;
							
							bool recreateTextBox = true;
							Slide *altSlide = 0;
							TextBoxItem *altText = 0;
							int altRealHeight = 0;
							int altSlideCount = 0;
							int currentMinTextSize = MinTextSize;
							
							
							QStringList slideTmpList;
							for(int x=0; x<slideLines.size(); x++)
							{
								//qDebug() << "BibleBrowser::createSlideGroup(): loop start, slideNumber:"<<slideNumber;
								if(slideTmpList.isEmpty() &&
								   slideLines[x].trimmed().isEmpty())
								   continue;
						
								slideTmpList.append(slideLines[x]);
								
								//progress.setValue(x);
						
								if(recreateTextBox)
								{
									//qDebug() << "BibleBrowser::createSlideGroup(): recreateTextBox, slideNumber:"<<slideNumber;
									Slide *tmpl = getTemplateSlide(outputTemplate, slideNumber);
									altSlide = tmpl->clone();
									
									//qDebug() << "BibleBrowser::createSlideGroup(): [clone] alt:"<<altSlide->slideId()<<", tmpl:"<<tmpl->slideId()<<", current:"<<currentSlide->slideId(); 
									
									altText = findTextItem(altSlide, "#verses");
									currentMinTextSize = (int)altText->findFontSize();
									
									//qDebug() << "BibleBrowser::createSlideGroup(): recreateTextBox, got currentSlide:"<<currentSlide<<", tmpText:"<<tmpText;
								}
								
								mergeTextItem(altText, slideTmpList.join("").trimmed());
								
								QRect currentFitRect = altText->contentsRect().toRect();
								
								//realHeight = tmpText->fitToSize(fitSize,MinTextSize,MaxTextSize);
								altRealHeight = altText->fitToSize(currentFitRect.size(), currentMinTextSize, MaxTextSize);
								//qDebug() << "x:"<<x<<", realHeight:"<<realHeight<<", currentFitRect:"<<currentFitRect;
								
								//qDebug() << "BibleBrowser::createSlideGroup(): [alt slide] Trying text: "<<slideTmpList.join("").trimmed()<<", altRealHeight:"<<altRealHeight<<", currentFitRect:"<<currentFitRect<<", currentMinTextSize:"<<currentMinTextSize;
								
								
								// If the 'altRealHeight' is <0, it means that it didnt fit on the slide.
								// Therefore, we've found the max # of text frags that fit on this slide
								// so we should create a new slide, add the text, and then start searching again.
								// This block also handles each verse on a seperate slide, in which case a single line in lines[] is a single verse.
								if(altRealHeight < 0
								   || altRealHeight > currentFitRect.height()
								   /*|| showEachVerseOnSeperateSlide()*/)
								{
									// More than one line, so the last line is the line that made the slide overflow the screen - 
									// therefore take it off and return it to the buffer for the next slide to use.
									if(slideTmpList.size() > 1)
									{
										// return last line to the file buffer
										QString line = slideTmpList.takeLast();
										slideLines.prepend(line);
										
										//qDebug() << "BibleBrowser::createSlideGroup(): [alt slide] Hit limit, removing last line, new text: "<<slideTmpList.join("").trimmed();
									}
									
									QString dots = x < slideLines.size()-1 ? "..." : "";
									mergeTextItem(altText, slideTmpList.join("").trimmed() + dots);
									altRealHeight = altText->fitToSize(currentFitRect.size(),currentMinTextSize,MaxTextSize);
						
									// Verse slide
									intelligentCenterTextbox(altText);
									addSlideWithText(altGroup, altSlide, altText);
							
									altSlideCount ++;
									
									altSlide->setPrimarySlideId(currentSlide->slideId());
									
									if(showEachVerseOnSeperateSlide())
										altSlide->setSlideName(tr("%1.%2").arg(slideName).arg(altSlideCount));
									//(appendToExistingGroup() ? m_currentRef.toString() + ": " : "") + QString("v %1.%2").arg(verseList[verseListIndex].verseNumber()).arg(altSlideCount));
									
									setupOptionalLabels(altSlide, slideNumber);
									
									//qDebug() << "BibleBrowser::createSlideGroup(): [verse] Output "<<out->name()<<": [split] Create alt slide "<<altSlideCount<<" for slide "<<slideNumber<<", name:"<<altSlide->slideName();
									//qDebug() << "BibleBrowser::createSlideGroup(): [verse] Output "<<out->name()<<": [split] Created: "<<altSlide->slideName()<<", linked: altSlide:"<<altSlide->slideId()<<" -> currentSlide:"<<currentSlide->slideId(); 
									
									//qDebug() << "bottom of loop, tmpText size:"<<tmpText->contentsRect();
									// this will get re-created above when we set it to 0
									//tmpText = 0;
									recreateTextBox = true;
									altRealHeight = 0;
									
									slideTmpList.clear();
								}
							}
							
							//qDebug() << "BibleBrowser::createSlideGroup(): loop end, slideNumber:"<<slideNumber;
							
							// This block handles the case where we never did hit the maximum # of text frags per slide in the search block above
							// which would mean we have a dangling blob of text that never got added - so add it here. (Such as the last slide in a long list of slides)
							if(altRealHeight>0 && recreateTextBox)
							{
								// Verse slide
								intelligentCenterTextbox(altText);
								addSlideWithText(altGroup, altSlide, altText);
								
								altSlideCount++;
								
								if(showEachVerseOnSeperateSlide())
									altSlide->setSlideName(tr("%1.%2").arg(slideName).arg(altSlideCount));
									//altSlide->setSlideName((appendToExistingGroup() ? m_currentRef.toString() + ": " : "") + QString("v %1.%2").arg(verseList[verseListIndex].verseNumber()).arg(altSlideCount));
								
								setupOptionalLabels(altSlide, slideNumber);
								
								altSlide->setPrimarySlideId(currentSlide->slideId());
								
								//qDebug() << "BibleBrowser::createSlideGroup(): [verse] Output "<<out->name()<<": [split] Created: "<<altSlide->slideName()<<", linked: altSlide:"<<altSlide->slideId()<<" -> currentSlide:"<<currentSlide->slideId();
							}
							
						}
						else
						{
							// squeze text block to fit in alt box
							
							Slide *altSlide = getTemplateSlide(outputTemplate, slideNumber)->clone();
							TextBoxItem *altText = findTextItem(altSlide, "#verses");
							int currentMinTextSize = (int)altText->findFontSize();
			
							mergeTextItem(altText, tmpList.join(""));
							
							conditionallyFitAndAlign(altText,currentMinTextSize,MaxTextSize);
							//intelligentCenterTextbox(altText);
							
							addSlideWithText(altGroup, altSlide, altText);
							
							altSlide->setPrimarySlideId(currentSlide->slideId());
							
							//qDebug() << "BibleBrowser::createSlideGroup(): [verse] Output "<<out->name()<<": [regular] Create alt slide for slide "<<slideNumber;
							
							if(showEachVerseOnSeperateSlide())
								altSlide->setSlideName(slideName); 
								//(appendToExistingGroup() ? m_currentRef.toString() + ": " : "") + QString("v %1").arg(verseList[verseListIndex].verseNumber()));
							
							setupOptionalLabels(altSlide, slideNumber);
						}
					}
				}
			}
				
			//qDebug() << "bottom of loop, tmpText size:"<<tmpText->contentsRect();
			// this will get re-created above when we set it to 0
			//tmpText = 0;
			recreateTextBox = true;
			realHeight = 0;
			
			tmpList.clear();
		}
		
		//qDebug() << "BibleBrowser::createSlideGroup(): loop end, slideNumber:"<<slideNumber;
	}
	
	// This block handles the case where we never did hit the maximum # of text frags per slide in the search block above
	// which would mean we have a dangling blob of text that never got added - so add it here. (Such as the last slide in a long list of slides)
 	if(realHeight>0 && recreateTextBox)
 	{
 		// Verse slide
		intelligentCenterTextbox(tmpText);
		addSlideWithText(group, currentSlide, tmpText);
		
		slideNumber++;
		
		setupOptionalLabels(currentSlide, slideNumber);
	}
	
	if(showFullRefAtBottomLast() && !showFullRefBottomEachSlide())
	{
		TextBoxItem *label = findTextItem(currentSlide, "#ref-bottom");
		mergeTextItem(label, m_currentRef.toString());
		conditionallyFitAndAlign(label,MinLabelSize);
	}
	else
	if(!showFullRefAtBottomLast() && !showFullRefBottomEachSlide())
	{
		TextBoxItem *label = findTextItem(currentSlide, "#ref-bottom");
		if(label)
			label->setOpacity(0.);
	}
	
	// Same thing as the starting slide - add an ending slide with the full text reference at the end if requested.
	if(showFullRefAtEnd())
	{
		Slide *primaryTitleSlide = 
			createTitleSlide(templateGroup, // using slide from this template
					group,          // add to this group
					false);         // last slide (as opposed to first slide)) 
		
		foreach(Output *out, allOut)
		{
			SlideGroup *outputTemplate = templateGroup->altGroupForOutput(out);
			if(outputTemplate)
			{
				qDebug() << "BibleBrowser::createSlideGroup(): [last] Creating last title slide from template for output: "<<out->name();
				
				if(SlideGroup *altGroup = group->altGroupForOutput(out))
				{
					Slide *altTitleSlide = 
						createTitleSlide(outputTemplate, // using slide from this template
						         altGroup,       // add to this group
						         false);          // last slide (as opposed to first slide)
					
					altTitleSlide->setPrimarySlideId(primaryTitleSlide->slideId());
				}
			}
		}
	}
	
	
	// close the dialog
	progress.setValue(progress.maximum());

	return group;

}

	
	

Slide * BibleBrowser::addSlide(SlideGroup *group, TextBoxItem *tmpText, int realHeight, const QRect & fitRect, const QString & /*plain*/)
{
	Slide *slide = 0;
	
// 	int slideNum = group->numSlides();
	
	//bool textboxFromTemplate = false;;
	
	QRect slideRect = AppSettings::adjustToTitlesafe(MainWindow::mw()->standardSceneRect());
	QRect textRect = slideRect;
	
	TextBoxItem * templateTextbox = 0;
	
	//qDebug() << "Slide "<<slideNum<<": [\n"<<plain<<"\n]";
	if(m_template && m_template->numSlides() > 0)
	{
		slide = m_template->at(0)->clone();
		
		// Use the first textbox in the slide as the textbox
		// "first" as defined by ZValue
		TextBoxItem *text = findTextItem(slide, "#verses");
			
		// copy format from template to text box passed in, then use the resulting html in the template text box
		if(text) 
		{
			mergeTextItem(text, tmpText);
			
			templateTextbox = text;
			textRect = text->contentsRect().toRect();
		}
	}
	else
	{
		slide = new Slide();
	
		AbstractVisualItem * bg = dynamic_cast<AbstractVisualItem*>(slide->background());

		bg->setFillType(AbstractVisualItem::Solid);
		bg->setFillBrush(Qt::blue);
	}
	
	// get it up here early for debugging
// 	int slideNbr = group->numSlides();
	
	// Attempt to enter text on screen
	int heightDifference = abs(fitRect.height() - textRect.height());
	
	// Arbitrary magic number to force centering for small amounts of differences.
	// We test the difference here because don't want to force-center the textbox if it came from the template
	// and was intentionally located off-center. But if the user tried to get it to fill the screen and missed
	// by a few pixels (<20), then go ahead and center it for the user.
	//qDebug()<<"BibleBrowser::addSlide(): slideNbr: "<<slideNbr<<": heightDifference: "<<heightDifference;
	if(templateTextbox)
	{
		if(heightDifference < 20 &&
		   realHeight > -1)
		{
			QSize natSize = tmpText->findNaturalSize(fitRect.width()); // 20 = buffer for shadow on right
			//qreal y = qMax(slideRect.y(), fitRect.height()/2 - natSize.height()/2);
			int centerUsableHeight = qMin(realHeight,natSize.height());
			qreal y = qMax(0, fitRect.height()/2 - centerUsableHeight/2);
			
			QRectF centeredRect(fitRect.x(),fitRect.y() + y,textRect.width(),centerUsableHeight);
			
			templateTextbox->setPos(QPointF(0,0));
			templateTextbox->setContentsRect(centeredRect);
			
			//qDebug()<<"BibleBrowser::addSlide(): [templated] slideNbr: "<<slideNbr<<": centeredRect: "<<centeredRect<<", templateTextbox:"<<templateTextbox<<", realHeight:"<<realHeight; 
		}
	}
	else
	{
		
		//qDebug()<<"BibleBrowser::addSlide(): slideNbr: "<<slideNbr<<": processing sizes...";
		
		QSize natSize = tmpText->findNaturalSize(fitRect.width()); // 20 = buffer for shadow on right
		//qreal y = qMax(slideRect.y(), fitRect.height()/2 - natSize.height()/2);
		int centerUsableHeight = qMin(realHeight,natSize.height());
		qreal y = qMax(0, fitRect.height()/2 - centerUsableHeight/2);
		
		QRectF centeredRect(fitRect.x(),fitRect.y() + y,textRect.width(),centerUsableHeight);
		//QRectF centeredRect(slideRect.x(),y + slideRect.y(),natSize.width(),natSize.height());
		tmpText->setPos(QPointF(0,0));
		tmpText->setContentsRect(centeredRect);
		
		
		
// 	if(!templateTextbox)
// 	{
		setupTextBox(tmpText);
		slide->addItem(tmpText);
		
		//qDebug()<<"BibleBrowser::addSlide(): slideNbr: "<<slideNbr<<": centeredRect: "<<centeredRect<<", realHeight:"<<realHeight<<", fitRect:"<<fitRect<<", y:"<<y<<", nat:"<<natSize;
	}
	
	addSlideWithText(group, slide, tmpText);
	
	return slide;
}

