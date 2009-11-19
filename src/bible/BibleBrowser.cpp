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

#include "BibleModel.h"
#include "BibleGatewayConnector.h"

#include "model/Slide.h"
#include "model/SlideGroup.h"
#include "model/Document.h"
#include "model/TextBoxItem.h"
#include "model/ItemFactory.h"
#include "MainWindow.h"


BibleBrowser::BibleBrowser(QWidget *parent)
	: QWidget(parent)
	, m_showVerseNumbers(true)
	//, m_showFullRefEachSlide(false)
	, m_showFullRefAtStart(true)
	, m_showFullRefAtEnd(false)
	, m_showResponsiveReadingLabels(false)
{
	setObjectName("BibleBrowser");
	
	
	QSettings s;
	m_showVerseNumbers	= s.value("biblebrowser/show-verse-numbers",true).toBool();
	//m_showFullRefEachSlide	= s.value("biblebrowser/show-full-ref-eachslide",false).toBool();
	m_showFullRefAtStart	= s.value("biblebrowser/show-full-ref-at-start",true).toBool();
	m_showFullRefAtEnd	= s.value("biblebrowser/show-full-ref-at-end",false).toBool();
	m_showResponsiveReadingLabels = s.value("biblebrowser/show-responsive-reading-lables",false).toBool();
	
	setupUI();
	
	m_bible = new BibleGatewayConnector();
	connect(m_bible, SIGNAL(referenceAvailable(const BibleVerseRef& , const BibleVerseList &)), this, SLOT(referenceAvailable(const BibleVerseRef& , const BibleVerseList &)));
}
	
BibleBrowser::~BibleBrowser() 
{
}

void BibleBrowser::saveSettings()
{
	qDebug() << "BibleBrowser::saveSettings()";
	
	QSettings s;
	s.setValue("biblebrowser/show-verse-numbers",			m_showVerseNumbers);
	//s.setValue("biblebrowser/show-full-ref-eachslide",		m_showFullRefEachSlide);
	s.setValue("biblebrowser/show-full-ref-at-start",		m_showFullRefAtStart);
	s.setValue("biblebrowser/show-full-ref-at-end",			m_showFullRefAtEnd);
	s.setValue("biblebrowser/show-responsive-reading-lables",	m_showResponsiveReadingLabels);
}

void BibleBrowser::closeEvent(QCloseEvent*)
{
	
}

#define SET_MARGIN(layout,margin) \
	layout->setContentsMargins(margin,margin,margin,margin);

void BibleBrowser::setShowVerseNumbers(bool x) 			{ m_showVerseNumbers = x; saveSettings(); }
//void BibleBrowser::setShowFullRefEachSlide(bool x) 		{  m_showFullRefEachSlide = x; saveSettings(); }
void BibleBrowser::setShowFullRefAtStart(bool x) 		{ m_showFullRefAtStart = x; saveSettings(); }
void BibleBrowser::setShowFullRefAtEnd(bool x) 			{ m_showFullRefAtEnd = x; saveSettings(); }
void BibleBrowser::setShowResponsiveReadingLabels(bool x) 	{ m_showResponsiveReadingLabels = x; saveSettings(); }

void BibleBrowser::setupUI()
{
	QVBoxLayout *vbox = new QVBoxLayout(this);
	//SET_MARGIN(vbox,0);
	
	
	QHBoxLayout *hboxTop = new QHBoxLayout();
	vbox->addLayout(hboxTop);
	
	m_versionCombo = new QComboBox(this);
	setupVersionCombo();
	hboxTop->addWidget(m_versionCombo);
	
	connect(m_versionCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(searchReturnPressed()));
	
	QPushButton *configBtn = new QPushButton(QPixmap(":/data/stock-preferences.png"),"");
	configBtn->setToolTip("Setup Slide Creator Options");
	
	QMenu *configMenu = new QMenu(configBtn);
	QAction * action;
	
	action = configMenu->addAction("Show Verse Numbers in Text");
	action->setCheckable(true);
	action->setChecked(showVerseNumbers());
	connect(action, SIGNAL(toggled(bool)), this, SLOT(setShowVerseNumbers(bool)));
	
// 	action = configMenu->addAction("Show Full Verse Ref Each Slide");
// 	action->setCheckable(true);
// 	action->setChecked(showFullRefEachSlide());
// 	connect(action, SIGNAL(toggled(bool)), this, SLOT(setShowFullRefEachSlide(bool)));
// 	
	action = configMenu->addAction("Show Full Verse Ref At Group Start");
	action->setCheckable(true);
	action->setChecked(showFullRefAtStart());
	connect(action, SIGNAL(toggled(bool)), this, SLOT(setShowFullRefAtStart(bool)));
	
	action = configMenu->addAction("Show Full Verse Ref At Group End");
	action->setCheckable(true);
	action->setChecked(showFullRefAtEnd());
	connect(action, SIGNAL(toggled(bool)), this, SLOT(setShowFullRefAtEnd(bool)));
	
	action = configMenu->addAction("Show Responsive Reading Labels");
	action->setCheckable(true);
	action->setChecked(showResponsiveReadingLabels());
	connect(action, SIGNAL(toggled(bool)), this, SLOT(setShowResponsiveReadingLabels(bool)));
	
	configBtn->setMenu(configMenu);
	hboxTop->addWidget(configBtn);
	
	
	// Setup filter box at the top of the widget
	m_searchBase = new QWidget(this);
	
	QHBoxLayout *hbox = new QHBoxLayout(m_searchBase);
	SET_MARGIN(hbox,0);
	
	QLabel *label = new QLabel("Searc&h:");
	m_search = new QLineEdit(m_searchBase);
	label->setBuddy(m_search);
	
	// add book name completion
	QCompleter *completer = new QCompleter(BibleVerseRef::bookCompleterList(), m_search);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setWrapAround(true);
	m_search->setCompleter(completer);
	
	QPushButton * btnSearch = new QPushButton(QPixmap(":/data/stock-find.png"),"");
	btnSearch->setToolTip("Seach BibleGateway.com for the verse reference entered on the left.");
	
	m_addBtn = new QPushButton(QPixmap(":/data/stock-add.png"),"");
	m_addBtn->setToolTip("Add verses below as a slide group to current document");
	m_addBtn->setVisible(false);
	
	m_spinnerLabel = new QLabel();
	m_spinnerLabel->setVisible(false);
	
	m_spinnerLabel->setMovie(new QMovie(":/data/ajax-loader.gif",QByteArray(),this));
	m_spinnerLabel->setToolTip("Loading Verses...");
	
	hbox->addWidget(label);
	hbox->addWidget(m_search);
	hbox->addWidget(btnSearch);
	hbox->addWidget(m_addBtn);
	hbox->addWidget(m_spinnerLabel);
	
	//connect(m_search, SIGNAL(textChanged(const QString &)), this, SLOT(loadVerses(const QString &)));
	connect(m_search, SIGNAL(returnPressed()), this, SLOT(searchReturnPressed()));
	connect(btnSearch, SIGNAL(clicked()), this, SLOT(searchReturnPressed()));
	connect(m_addBtn, SIGNAL(clicked()), this, SLOT(createSlideGroup()));
	
	// add text preview
	m_preview = new QTextEdit(this);
	m_preview->setReadOnly(true);

	QFont font;
	font.setFamily("Courier");
	font.setFixedPitch(true);
	font.setPointSize(12);
	m_preview->setFont(font);

	vbox->addWidget(m_searchBase);
	vbox->addWidget(m_preview);
	
	
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
	m_spinnerLabel->setVisible(!text.isEmpty());
	if(!text.isEmpty())
	{
		m_spinnerLabel->movie()->start();
		QString versionCode = m_versionCombo->itemData(m_versionCombo->currentIndex()).toString();
		BibleVerseRef ref = BibleVerseRef::normalize(text, BibleVersion(versionCode,versionCode));
		if(!ref.valid())
		{
			m_spinnerLabel->movie()->stop();
			m_spinnerLabel->setVisible(false);
			m_search->selectAll();
			m_preview->setPlainText("");
			QMessageBox::warning(this,"Invalid Reference",QString(tr("Sorry, but %1 doesn't seem to be a valid bible reference.")).arg(text));
			return;
		}
		
		if(m_bible->findReference(ref))
		{
			referenceAvailable(ref, m_bible->loadReference(ref));
		}
		else
		{
			m_bible->downloadReference(ref);
		}
	}
	else
	{
		m_spinnerLabel->movie()->stop();
		m_preview->setPlainText("");
		m_addBtn->setVisible(false);
	}
	
// 	QModelIndex idx = m_songListModel->indexForRow(0);
// 	if(idx.isValid())
// 		m_songList->setCurrentIndex(idx);
}

void BibleBrowser::referenceAvailable(const BibleVerseRef& reference, const BibleVerseList & list)
{
	m_currentList = list;
	m_currentRef = reference;
	
	m_spinnerLabel->movie()->stop();
	m_spinnerLabel->setVisible(false);
	if(list.isEmpty())
	{
		m_preview->setHtml(QString(tr("<font color='red'>Sorry, \"<b>%1</b>\" was not found!</font>")).arg(reference.toString(true)));
	}
	else
	{
		m_addBtn->setVisible(true);
		
		QStringList listText;
		foreach(BibleVerse verse, list)
		{
			listText << QString("<sup>%1</sup>%2").arg(verse.verseNumber()).arg(verse.text());
		}
		
		m_preview->setHtml(QString("<h2>%1</h2><p>%2</p>").arg(reference.toString(true),listText.join("\n")));
	}
}

void BibleBrowser::loadVerses(const QString & filter)
{

	searchTextChanged(filter);
	m_search->setText(filter);
}

#ifndef QCharPair
typedef QPair<QString,QString> QStringPair;
#endif

void BibleBrowser::setupVersionCombo()
{
	QList<QStringPair> list;
	
	list.append(qMakePair(QString("NIV"),QString(tr("New International Version"))));
	list.append(qMakePair(QString("NASB"),QString(tr("New American Standard Bible"))));
	list.append(qMakePair(QString("MSG"),QString(tr("The Message"))));
	list.append(qMakePair(QString("AMP"),QString(tr("Amplified Bible"))));
	list.append(qMakePair(QString("NLT"),QString(tr("New Living Translation"))));
	list.append(qMakePair(QString("KJV"),QString(tr("King James Version"))));
	list.append(qMakePair(QString("ESV"),QString(tr("English Standard Version"))));
	list.append(qMakePair(QString("CEV"),QString(tr("Contemporary English Version"))));
	
	list.append(qMakePair(QString("NKJV"),QString(tr("New King James Version"))));
	list.append(qMakePair(QString("NCV"),QString(tr("New Century Version"))));
	list.append(qMakePair(QString("KJ21"),QString(tr("21st Century King James Version"))));
	list.append(qMakePair(QString("ASV"),QString(tr("American Standard Version"))));
	list.append(qMakePair(QString("YLT"),QString(tr("Young's Literal Translation"))));
	list.append(qMakePair(QString("DARBY"),QString(tr("Darby Translation"))));
	list.append(qMakePair(QString("HCSB"),QString(tr("Holman Christian Standard Bible"))));
	list.append(qMakePair(QString("NIRV"),QString(tr("New International Reader's Version"))));
	//list.append(qMakePair(QString("WYC"),QString(tr("Wycliffe New Testament"))));
	
	
	//list.append(qMakePair(QString("AMU"),QString(tr("Amuzgo de Guerrero"))));
	list.append(qMakePair(QString("ALAB"),QString(tr("Arabic Life Application Bible"))));
	list.append(qMakePair(QString("BULG"),QString(tr("Bulgarian Bible"))));
	list.append(qMakePair(QString("BG1940"),QString(tr("1940 Bulgarian Bible"))));
	//list.append(qMakePair(QString("CCO"),QString(tr("Chinanteco de Comaltepec"))));
	//list.append(qMakePair(QString("CKW"),QString(tr("Cakchiquel Occidental"))));
	
	list.append(qMakePair(QString("HCV"),QString(tr("Haitian Creole Version"))));
	//list.append(qMakePair(QString("SNC"),QString(tr("Slovo na cestu"))));
	list.append(qMakePair(QString("DN1933"),QString(tr("Dette er Biblen pÃ¥ dansk"))));
	//list.append(qMakePair(QString("HOF"),QString(tr("Hoffnung für Alle"))));
	list.append(qMakePair(QString("LUTH1545"),QString(tr("Luther Bibel 1545"))));
	
	
	//list.append(qMakePair(QString("WE"),QString(tr("Worldwide English (New Testament)"))));
	list.append(qMakePair(QString("NIVUK"),QString(tr("New International Version - UK"))));
	list.append(qMakePair(QString("TNIV"),QString(tr("Today's New International Version"))));
	list.append(qMakePair(QString("RVR1960"),QString(tr("Reina-Valera 1960"))));
	list.append(qMakePair(QString("NVI"),QString(tr("Nueva Versión Internacional"))));
	list.append(qMakePair(QString("RVR1995"),QString(tr("Reina-Valera 1995"))));
	//list.append(qMakePair(QString("CST"),QString(tr("Castilian"))));
	list.append(qMakePair(QString("RVA"),QString(tr("Reina-Valera Antigua"))));
	
	//list.append(qMakePair(QString("BLS"),QString(tr("Biblia en Lenguaje Sencillo"))));
	list.append(qMakePair(QString("LBLA"),QString(tr("La Biblia de las Américas"))));
	list.append(qMakePair(QString("LSG"),QString(tr("Louis Segond"))));
	list.append(qMakePair(QString("BDS"),QString(tr("La Bible du Semeur"))));
	//list.append(qMakePair(QString("WHNU"),QString(tr("1881 Westcott-Hort New Testament"))));
	//list.append(qMakePair(QString("TR1550"),QString(tr("1550 Stephanus New Testament"))));
	//list.append(qMakePair(QString("TR1894"),QString(tr("1894 Scrivener New Testament"))));
	
	//list.append(qMakePair(QString("WLC"),QString(tr("The Westminster Leningrad Codex"))));
	//list.append(qMakePair(QString("HLGN"),QString(tr("Hiligaynon Bible"))));
	//list.append(qMakePair(QString("CRO"),QString(tr("Croatian Bible"))));
	list.append(qMakePair(QString("KAR"),QString(tr("Hungarian KÃ¡roli"))));
	
	list.append(qMakePair(QString("ICELAND"),QString(tr("Icelandic Bible"))));
	list.append(qMakePair(QString("LND"),QString(tr("La Nuova Diodati"))));
	//list.append(qMakePair(QString("LM"),QString(tr("La Parola è Vita"))));
	//list.append(qMakePair(QString("JAC"),QString(tr("Jacalteco, Oriental"))));
	//list.append(qMakePair(QString("KEK"),QString(tr("Kekchi"))));
	
	list.append(qMakePair(QString("KOREAN"),QString(tr("Korean Bible"))));
	list.append(qMakePair(QString("MAORI"),QString(tr("Maori Bible"))));
	//list.append(qMakePair(QString("MNT"),QString(tr("Macedonian New Testament"))));
	//list.append(qMakePair(QString("MVC"),QString(tr("Mam, Central"))));
	//list.append(qMakePair(QString("MVJ"),QString(tr("Mam de Todos Santos Chuchumatán"))));
	
	//list.append(qMakePair(QString("REIMER"),QString(tr("Reimer 2001"))));
	//list.append(qMakePair(QString("NGU"),QString(tr("Náhuatl de Guerrero"))));
	list.append(qMakePair(QString("HTB"),QString(tr("Het Boek"))));
	list.append(qMakePair(QString("DNB1930"),QString(tr("Det Norsk Bibelselskap 1930"))));
	//list.append(qMakePair(QString("LB"),QString(tr("Levande Bibeln"))));
	
	list.append(qMakePair(QString("OL"),QString(tr("O Livro"))));
	list.append(qMakePair(QString("AA"),QString(tr("João Ferreira de Almeida Atualizada"))));
	//list.append(qMakePair(QString("QUT"),QString(tr("Quiché, Centro Occidental"))));
	list.append(qMakePair(QString("RMNN"),QString(tr("Romanian"))));
	//list.append(qMakePair(QString("TLCR"),QString(tr("Romanian"))));
	
	list.append(qMakePair(QString("RUSV"),QString(tr("Russian Synodal Version"))));
	//list.append(qMakePair(QString("SZ"),QString(tr("Slovo Zhizny"))));
	//list.append(qMakePair(QString("NPK"),QString(tr("Nádej pre kazdého"))));
	list.append(qMakePair(QString("ALB"),QString(tr("Albanian Bible"))));
	list.append(qMakePair(QString("SVL"),QString(tr("Levande Bibeln"))));
	list.append(qMakePair(QString("SV1917"),QString(tr("Svenska 1917"))));
	
	//list.append(qMakePair(QString("SNT"),QString(tr("Swahili New Testament"))));
	//list.append(qMakePair(QString("SND"),QString(tr("Ang Salita ng Diyos"))));
	list.append(qMakePair(QString("UKR"),QString(tr("Ukrainian Bible"))));
	//list.append(qMakePair(QString("USP"),QString(tr("Uspanteco"))));
	list.append(qMakePair(QString("VIET"),QString(tr("1934 Vietnamese Bible"))));
	list.append(qMakePair(QString("CUVS"),QString(tr("Chinese Union Version (Simplified)"))));
	list.append(qMakePair(QString("CUV"),QString(tr("Chinese Union Version (Traditional)"))));
	
	#define MAX_VERSION_NAME_LENGTH 32
	foreach(QStringPair pair, list)
	{
		QString code = pair.first;
		QString text = pair.second;
		m_versionCombo->addItem(QString("%1%2").arg(text.left(MAX_VERSION_NAME_LENGTH)).arg(text.length() > MAX_VERSION_NAME_LENGTH ? "..." : ""), code);
	}
	
	m_versionCombo->setCurrentIndex(m_versionCombo->findData("NIV"));
}

static void BibleBrowser_setupTextBox(TextBoxItem *tmpText)
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

void BibleBrowser::createSlideGroup()
{
	BibleVerseList list = m_currentList;
	
	if(list.isEmpty())
	{
		QMessageBox::warning(this,"No Verses Found","Sorry, no verses were found!");
		return;
	}
	
	// Adding entire chapters can take 10 - 30 seconds, so a progress dialog makes the UI more "friendly"
	QProgressDialog progress;
	progress.setWindowIcon(QIcon(":/data/icon-d.png"));
	progress.setWindowTitle(QString(tr("Adding %1")).arg(m_currentRef.toString(true)));
	progress.setLabelText(QString(tr("Adding %1...")).arg(m_currentRef.toString(true)));
	
	int MinTextSize = 48;

	// Add the verse numbers here because after this point,
	// all we have to work with is a blob of text, we loose the BibleVerse objects here.
	QStringList lines;
	QString prefix;
	foreach(BibleVerse verse, list)
	{
		prefix = m_showVerseNumbers ? 
			prefix = QString("<sup>%1</sup>").arg(verse.verseNumber())
			: "";
		
		lines << prefix + verse.text();
	}
	
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
	
	progress.setMaximum(lines.size());

	// This will be adjusted below if we were told to add responsive reading labels
	QSize fitSize = MainWindow::mw()->standardSceneRect().size();

	SlideGroup *group = new SlideGroup();
	group->setGroupTitle(m_currentRef.toString());

	QString blockPrefix = "<span style='font-family:Calibri,Tahoma,Arial,Sans-Serif;font-weight:800'><b>";
	QString blockSuffix = "</b></span>";
	
	TextBoxItem * tmpText = 0;
	int realHeight=0;
	
	// If the user asked for it, add the verse reference as the first slide, centered on the screen.
	Slide * startSlide = 0;
	if(showFullRefAtStart())
	{
		tmpText = new TextBoxItem();
		tmpText->setItemId(ItemFactory::nextId());
		tmpText->setItemName(QString("TextBoxItem%1").arg(tmpText->itemId()));
		
		tmpText->setText(QString("<center><span style='font-family:Constantia,Times New Roman,Serif;font-weight:800'><b>%1</b></span></center>").arg(m_currentRef.toString()));
		tmpText->changeFontSize(72);
		QSize size = tmpText->findNaturalSize();
		
		startSlide = addSlide(group,tmpText,size.height(),fitSize,m_currentRef.toString());
		
		tmpText = 0;
	}

	
	// Here we prep the responsive reading label object and adjust the "fitSize" (from above) to allow for space at the top of each slide
	TextBoxItem * labelItem = 0;
	QSize labelSize;
	if(m_showResponsiveReadingLabels)
	{
		labelItem = new TextBoxItem();
		labelItem->setItemId(ItemFactory::nextId());
		labelItem->setItemName(QString("TextBoxItem%1").arg(labelItem->itemId()));
		
		labelItem->setText(QString("<span style='font-family:Calibri,Tahoma,Arial,Sans-Serif;font-weight:800'><b>Congregation</b></span></center>"));;
		labelItem->changeFontSize(40);
		
		labelSize = labelItem->findNaturalSize();
		
		BibleBrowser_setupTextBox(labelItem);
		labelItem->setFillBrush(Qt::yellow);
	
		// resize usable area to allow for space at top of each slide for this label
		fitSize.setHeight(fitSize.height() - labelSize.height());
	}

	// In the future, these labels should be an option in the UI - for now, we'll hardcocde them
	QString leaderLabel = tr("Leader:");
	QString readingLabel = tr("Congregation:");
	
	// used to alternate the responsive reading labels
	int labelCounter = 0;
	
	// Main logic block - loop over all the "lines" in the list (really, text fragments),
	// and try to fit as many of them as we can on a slide
	QStringList tmpList;
	for(int x=0; x<lines.size(); x++)
	{
		if(tmpList.isEmpty() &&
			lines[x].trimmed().isEmpty())
			continue;

		tmpList.append(lines[x]);
		
		progress.setValue(x);

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

		realHeight = tmpText->fitToSize(fitSize,MinTextSize);
		
		// If the 'realHeight' is <0, it means that it didnt fit on the slide.
		// Therefore, we've found the max # of text frags that fit on this slide
		// so we should create a new slide, add the text, and then start searching again.
		if(realHeight < 0)
		{
			if(tmpList.size() > 1)
			{
				// return last line to the file buffer
				QString line = tmpList.takeLast();
				lines.prepend(line);
				
				tmpText->setText(QString("%1%2%3")
							.arg(blockPrefix)
							.arg(tmpList.join("\n"))
							.arg(blockSuffix));
				realHeight = tmpText->fitToSize(fitSize,MinTextSize);
			}

			Slide * slide = addSlide(group,tmpText,realHeight,fitSize,tmpList.join("\n"));
			
			if(m_showResponsiveReadingLabels)
			{
				
				// Resize the main text box by "pushing it down" to allow for room for the label at the top of the slide.
				QRectF rect = tmpText->contentsRect();
				tmpText->setContentsRect(QRectF(rect.x(),rect.y() + labelSize.height(),rect.width(),rect.height()));
				// dont adjust rect.height here because the rect should have been sized to fitSize, who's height was already adjusted to labelSize, above
				
				QString labelText = labelCounter ++ % 2 == 0 ? leaderLabel : readingLabel;
				
				// Clone the label so we dont have to re-create it
				TextBoxItem * label = dynamic_cast<TextBoxItem*>(labelItem->clone());
				label->setText(QString("<span style='font-family:Calibri,Tahoma,Arial,Sans-Serif;font-weight:800'><b>%1</b></span></center>").arg(labelText));
				label->changeFontSize(40);
				
				// Adjust the size of the label
				QSize sz = label->findNaturalSize(fitSize.width());
				label->setContentsRect(QRectF(0,0,sz.width(),sz.height()));
				
				//qDebug() << "responsive reading: #"<<labelCounter<<", labelText:"<<labelText<<", sz:"<<sz<<", rect:"<<label->contentsRect();
				
				slide->addItem(label);
			}
				
			// this will get re-created above when we set it to 0
			tmpText = 0;
			
			tmpList.clear();
		}
	}
	
	// This block handles the case where we never did hit the maximum # of text frags per slide in the search block above
	// which would mean we have a dangling blob of text that never got added - so add it here.
 	if(realHeight>0 && tmpText)
 	{
 		Slide * slide = addSlide(group,tmpText,realHeight,fitSize,tmpList.join("\n"));
 		
 		// This block is just an exact duplication of the label block above - but it seems more work than necessary right now to move it into
 		// it's own subroutine for now.
 		if(m_showResponsiveReadingLabels)
		{
			QRectF rect = tmpText->contentsRect();
			
			// dont adjust rect.height here because the rect should have been sized to fitSize, who's height was already adjusted to labelSize, above
			tmpText->setContentsRect(QRectF(rect.x(),rect.y() + labelSize.height(),rect.width(),rect.height()));
			
			QString labelText = labelCounter ++ % 2 == 0 ? leaderLabel : readingLabel;
			
			TextBoxItem * label = dynamic_cast<TextBoxItem*>(labelItem->clone());
			label->setText(QString("<span style='font-family:Calibri,Tahoma,Arial,Sans-Serif;font-weight:800'><b>%1</b></span></center>").arg(labelText));
			
			label->changeFontSize(40);
			
			QSize sz = label->findNaturalSize(fitSize.width());
			label->setContentsRect(QRectF(0,0,sz.width(),sz.height()));
			
			//qDebug() << "responsive reading: #"<<labelCounter<<", labelText:"<<labelText<<", sz:"<<sz<<", rect:"<<label->contentsRect();
			
			
			slide->addItem(label);
		}
	}
		
	// Same thing as the starting slide - add an ending slide with the full text reference at the end if requested.
	if(showFullRefAtEnd())
	{
		tmpText = new TextBoxItem();
		tmpText->setItemId(ItemFactory::nextId());
		tmpText->setItemName(QString("TextBoxItem%1").arg(tmpText->itemId()));
	
		tmpText->setText(QString("<center><span style='font-family:Constantia,Times New Roman,Serif;font-weight:800'><b>%1</b></span></center>").arg(m_currentRef.toString()));
		tmpText->changeFontSize(72);
		QSize size = tmpText->findNaturalSize();
		
		// get fitSize again because it may have been changed to accomodate reading labels
		QSize fitSize = MainWindow::mw()->standardSceneRect().size();
		addSlide(group,tmpText,size.height(),fitSize,m_currentRef.toString());
		
		tmpText = 0;
	}
	
	// close the dialog
	progress.setValue(progress.maximum());

	MainWindow::mw()->currentDocument()->addGroup(group);

}

Slide * BibleBrowser::addSlide(SlideGroup *group, TextBoxItem *tmpText, int realHeight, const QSize & fitSize, const QString & plain)
{
	Slide * slide = new Slide();
	AbstractVisualItem * bg = dynamic_cast<AbstractVisualItem*>(slide->background());

	int slideNum = group->numSlides();
	
	//qDebug() << "Slide "<<slideNum<<": [\n"<<plain<<"\n]";

	bg->setFillType(AbstractVisualItem::Solid);
	bg->setFillBrush(Qt::blue);

	// Center text on screen
	qreal y = fitSize.height()/2 - realHeight/2;
	//qDebug() << "SongSlideGroup::textToSlides(): centering: boxHeight:"<<boxHeight<<", textRect height:"<<textRect.height()<<", centered Y:"<<y;
	tmpText->setContentsRect(QRectF(0,y,fitSize.width(),realHeight));

	BibleBrowser_setupTextBox(tmpText);

	slide->addItem(tmpText);
	
	slide->setSlideNumber(slideNum);
	group->addSlide(slide);
	
	tmpText->warmVisualCache();
	
	return slide;
}

