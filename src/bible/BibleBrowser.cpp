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
		m_preview->setHtml(QString(tr("<font color='red'>Sorry, \"<b>%1</b>\" was not found!</font>")).arg(reference.cacheKey()));
	}
	else
	{
		m_addBtn->setVisible(true);
		
		QStringList listText;
		foreach(BibleVerse verse, list)
		{
			listText << QString("<sup>%1</sup>%2").arg(verse.verseNumber()).arg(verse.text());
		}
		
		m_preview->setHtml(QString("<h2>%1</h2><p>%2</p>").arg(reference.cacheKey(),listText.join("\n")));
	}
}

// void BibleBrowser::searchReset()
// {
// 	setFilter("");
// }

void BibleBrowser::loadVerses(const QString & filter)
{

	searchTextChanged(filter);
	m_search->setText(filter);
}


void BibleBrowser::setupVersionCombo()
{
	QMap<QString,QString> versionMap;
	
	versionMap["NIV"]="New International Version";
	versionMap["NASB"]="New American Standard Bible";
	versionMap["MSG"]="The Message";
	versionMap["AMP"]="Amplified Bible";
	versionMap["NLT"]="New Living Translation";
	versionMap["KJV"]="King James Version";
	versionMap["ESV"]="English Standard Version";
	versionMap["CEV"]="Contemporary English Version";
	
	versionMap["NKJV"]="New King James Version";
	versionMap["NCV"]="New Century Version";
	versionMap["KJ21"]="21st Century King James Version";
	versionMap["ASV"]="American Standard Version";
	versionMap["YLT"]="Young's Literal Translation";
	versionMap["DARBY"]="Darby Translation";
	versionMap["HCSB"]="Holman Christian Standard Bible";
	versionMap["NIRV"]="New International Reader's Version";
	versionMap["WYC"]="Wycliffe New Testament";
	
	
	versionMap["AMU"]="Amuzgo de Guerrero";
	versionMap["ALAB"]="Arabic Life Application Bible";
	versionMap["BULG"]="Bulgarian Bible";
	versionMap["BG1940"]="1940 Bulgarian Bible";
	versionMap["CCO"]="Chinanteco de Comaltepec";
	versionMap["CKW"]="Cakchiquel Occidental";
	
	versionMap["HCV"]="Haitian Creole Version";
	versionMap["SNC"]="Slovo na cestu";
	versionMap["DN1933"]="Dette er Biblen pÃ¥ dansk";
	versionMap["HOF"]="Hoffnung für Alle";
	versionMap["LUTH1545"]="Luther Bibel 1545";
	
	
	versionMap["WE"]="Worldwide English (New Testament)";
	versionMap["NIVUK"]="New International Version - UK";
	versionMap["TNIV"]="Today's New International Version";
	versionMap["RVR1960"]="Reina-Valera 1960";
	versionMap["NVI"]="Nueva Versión Internacional";
	versionMap["RVR1995"]="Reina-Valera 1995";
	versionMap["CST"]="Castilian";
	versionMap["RVA"]="Reina-Valera Antigua";
	
	versionMap["BLS"]="Biblia en Lenguaje Sencillo";
	versionMap["LBLA"]="La Biblia de las Américas";
	versionMap["LSG"]="Louis Segond";
	versionMap["BDS"]="La Bible du Semeur";
	versionMap["WHNU"]="1881 Westcott-Hort New Testament";
	versionMap["TR1550"]="1550 Stephanus New Testament";
	versionMap["TR1894"]="1894 Scrivener New Testament";
	
	versionMap["WLC"]="The Westminster Leningrad Codex";
	versionMap["HLGN"]="Hiligaynon Bible";
	versionMap["CRO"]="Croatian Bible";
	versionMap["KAR"]="Hungarian KÃ¡roli";
	
	versionMap["ICELAND"]="Icelandic Bible";
	versionMap["LND"]="La Nuova Diodati";
	versionMap["LM"]="La Parola è Vita";
	versionMap["JAC"]="Jacalteco, Oriental";
	versionMap["KEK"]="Kekchi";
	
	versionMap["KOREAN"]="Korean Bible";
	versionMap["MAORI"]="Maori Bible";
	versionMap["MNT"]="Macedonian New Testament";
	versionMap["MVC"]="Mam, Central";
	versionMap["MVJ"]="Mam de Todos Santos Chuchumatán";
	
	versionMap["REIMER"]="Reimer 2001";
	versionMap["NGU"]="Náhuatl de Guerrero";
	versionMap["HTB"]="Het Boek";
	versionMap["DNB1930"]="Det Norsk Bibelselskap 1930";
	versionMap["LB"]="Levande Bibeln";
	
	versionMap["OL"]="O Livro";
	versionMap["AA"]="João Ferreira de Almeida Atualizada";
	versionMap["QUT"]="Quiché, Centro Occidental";
	versionMap["RMNN"]="Romanian";
	versionMap["TLCR"]="Romanian";
	
	versionMap["RUSV"]="Russian Synodal Version";
	versionMap["SZ"]="Slovo Zhizny";
	versionMap["NPK"]="Nádej pre kazdého";
	versionMap["ALB"]="Albanian Bible";
	versionMap["SVL"]="Levande Bibeln";
	versionMap["SV1917"]="Svenska 1917";
	
	versionMap["SNT"]="Swahili New Testament";
	versionMap["SND"]="Ang Salita ng Diyos";
	versionMap["UKR"]="Ukrainian Bible";
	versionMap["USP"]="Uspanteco";
	versionMap["VIET"]="1934 Vietnamese Bible";
	versionMap["CUVS"]="Chinese Union Version (Simplified)";
	versionMap["CUV"]="Chinese Union Version (Traditional)";
	
	#define MAX_VERSION_NAME_LENGTH 32
	foreach(QString code, versionMap.keys())
	{
		QString text = versionMap.value(code);
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
	
	int MinTextSize = 48;

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
	
	int pos = 0;
	int lastPos = 0;
	QRegExp rx("[-;,\n:\\.]");
	while((pos = rx.indexIn(blob,pos)) != -1)
	{
		lines.append(blob.mid(lastPos,pos-lastPos+1));
		pos += rx.matchedLength();
		lastPos = pos;
	}
	

	QSize fitSize = MainWindow::mw()->standardSceneRect().size();

	SlideGroup *group = new SlideGroup();
	group->setGroupTitle(m_currentRef.toString());

	int slideNum = 0;

	QString blockPrefix = "<span style='font-family:Calibri,Tahoma,Arial,Sans-Serif;font-weight:800'><b>";
	QString blockSuffix = "</b></span>";
	
	TextBoxItem * tmpText = 0;
	int realHeight=0;
	
	Slide * startSlide = 0;
	if(showFullRefAtStart())
	{
		tmpText = new TextBoxItem();
		tmpText->setItemId(ItemFactory::nextId());
		tmpText->setItemName(QString("TextBoxItem%1").arg(tmpText->itemId()));
		
		tmpText->setText(QString("<center><span style='font-family:Constantina,Times New Roman,Serif;font-weight:800'><b>%1</b></span></center>").arg(m_currentRef.toString()));
		tmpText->changeFontSize(72);
		QSize size = tmpText->findNaturalSize();
		
		startSlide = addSlide(group,tmpText,size.height(),fitSize,m_currentRef.toString());
		
		tmpText = 0;
	}

	
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

	QString leaderLabel = tr("Leader:");
	QString readingLabel = tr("Congregation:");
	
	int labelCounter = 0;
	
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

		realHeight = tmpText->fitToSize(fitSize,MinTextSize);
		if(realHeight < 0)
		{
			if(tmpList.size() > 1)
			{
				// return last line to the file buffer
				QString line = tmpList.takeLast();
				lines.prepend(line);
				
				qDebug() << "Readding last line back into line buffer: "<<line; 

				tmpText->setText(QString("%1%2%3")
							.arg(blockPrefix)
							.arg(tmpList.join("\n"))
							.arg(blockSuffix));
				realHeight = tmpText->fitToSize(fitSize,MinTextSize);
			}

			Slide * slide = addSlide(group,tmpText,realHeight,fitSize,tmpList.join("\n"));
			
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
				
				qDebug() << "responsive reading: #"<<labelCounter<<", labelText:"<<labelText<<", sz:"<<sz<<", rect:"<<label->contentsRect();
				
				
				slide->addItem(label);
			}
				
			tmpText = 0;
			
			

			tmpList.clear();
		}
	}
	
 	if(realHeight>0 && tmpText)
 	{
 		Slide * slide = addSlide(group,tmpText,realHeight,fitSize,tmpList.join("\n"));
 		
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
			
			qDebug() << "responsive reading: #"<<labelCounter<<", labelText:"<<labelText<<", sz:"<<sz<<", rect:"<<label->contentsRect();
			
			
			slide->addItem(label);
		}
	}
		
	if(showFullRefAtEnd())
	{
		tmpText = new TextBoxItem();
		tmpText->setItemId(ItemFactory::nextId());
		tmpText->setItemName(QString("TextBoxItem%1").arg(tmpText->itemId()));
	
		tmpText->setText(QString("<center><span style='font-family:Constantina,Times New Roman,Serif;font-weight:800'><b>%1</b></span></center>").arg(m_currentRef.toString()));
		tmpText->changeFontSize(72);
		QSize size = tmpText->findNaturalSize();
		
		// get fitSize again because it may have been changed to accomodate reading labels
		QSize fitSize = MainWindow::mw()->standardSceneRect().size();
		addSlide(group,tmpText,size.height(),fitSize,m_currentRef.toString());
		
		tmpText = 0;
	}

	MainWindow::mw()->currentDocument()->addGroup(group);

}

Slide * BibleBrowser::addSlide(SlideGroup *group, TextBoxItem *tmpText, int realHeight, const QSize & fitSize, const QString & plain)
{
	Slide * slide = new Slide();
	AbstractVisualItem * bg = dynamic_cast<AbstractVisualItem*>(slide->background());

	int slideNum = group->numSlides();
	
	qDebug() << "Slide "<<slideNum<<": [\n"<<plain<<"\n]";;

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

