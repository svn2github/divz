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

#include "BibleModel.h"
#include "BibleGatewayConnector.h"

BibleBrowser::BibleBrowser(QWidget *parent)
	: QWidget(parent)
{
	setObjectName("BibleBrowser");
	setupUI();
	
	m_bible = new BibleGatewayConnector();
	connect(m_bible, SIGNAL(referenceAvailable(const BibleVerseRef& , const BibleVerseList &)), this, SLOT(referenceAvailable(const BibleVerseRef& , const BibleVerseList &)));
}
	
BibleBrowser::~BibleBrowser() {}
#define SET_MARGIN(layout,margin) \
	layout->setContentsMargins(margin,margin,margin,margin);

void BibleBrowser::setupUI()
{
	QVBoxLayout *vbox = new QVBoxLayout(this);
	//SET_MARGIN(vbox,0);
	
	// Setup filter box at the top of the widget
	m_searchBase = new QWidget(this);
	
	QHBoxLayout *hbox = new QHBoxLayout(m_searchBase);
	SET_MARGIN(hbox,0);
	
	m_versionCombo = new QComboBox(m_searchBase);
	setupVersionCombo();
	
	QLabel *label = new QLabel("Searc&h:");
	m_search = new QLineEdit(m_searchBase);
	label->setBuddy(m_search);
	
	m_clearSearchBtn = new QPushButton("C&lear");
	m_clearSearchBtn->setVisible(false);
	
	m_spinnerLabel = new QLabel();
	m_spinnerLabel->setVisible(false);
	
	QMovie * movie = new QMovie(":/data/ajax-loader.gif",QByteArray(),this);
	movie->start();
	m_spinnerLabel->setMovie(movie);
	m_spinnerLabel->setToolTip("Loading Verses...");
	
	hbox->addWidget(m_versionCombo);
	hbox->addWidget(label);
	hbox->addWidget(m_search);
	hbox->addWidget(m_clearSearchBtn);
	hbox->addWidget(m_spinnerLabel);
	
	//connect(m_search, SIGNAL(textChanged(const QString &)), this, SLOT(loadVerses(const QString &)));
	connect(m_search, SIGNAL(returnPressed()), this, SLOT(searchReturnPressed()));
	connect(m_clearSearchBtn, SIGNAL(clicked()), this, SLOT(clearSearch()));
	
		// add text preview
	m_preview = new QTextEdit(this);
	m_preview->setReadOnly(true);

	QFont font;
	font.setFamily("Courier");
	font.setFixedPitch(true);
	font.setPointSize(8);
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
	m_clearSearchBtn->setVisible(!text.isEmpty());
	m_spinnerLabel->setVisible(!text.isEmpty());
	if(!text.isEmpty())
	{
		QString versionCode = m_versionCombo->itemData(m_versionCombo->currentIndex()).toString();
		BibleVerseRef ref = BibleVerseRef::normalize(text, BibleVersion(versionCode,versionCode));
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
		m_preview->setPlainText("");
	}
	
// 	QModelIndex idx = m_songListModel->indexForRow(0);
// 	if(idx.isValid())
// 		m_songList->setCurrentIndex(idx);
}

void BibleBrowser::referenceAvailable(const BibleVerseRef& reference, const BibleVerseList & list)
{
	m_spinnerLabel->setVisible(false);
	QStringList listText;
	foreach(BibleVerse verse, list)
	{
		listText << QString("<sup>%1</sup>%2").arg(verse.verseNumber()).arg(verse.text());
	}
	
	m_preview->setHtml(QString("<h1>%1</h1><p>%2</p>").arg(reference.cacheKey(),listText.join("\n")));
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
	QHash<QString,QString> versionMap;
	
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
	
	foreach(QString code, versionMap)
		m_versionCombo->addItem(versionMap.value(code), QVariant(code));
	
	m_versionCombo->setCurrentIndex(0);
}
