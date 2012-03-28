#include "SongSlideGroup.h"

#include "model/TextBoxItem.h"
#include "model/BackgroundItem.h"
#include "model/ItemFactory.h"
#include "model/Slide.h"
#include "model/SlideGroup.h"
#include "model/Output.h"
#include "MainWindow.h"
#include "AppSettings.h"

#include <QTextDocument>
#include <QTextBlock>
#include <QTextOption>
#include <QTextEdit>

#define DEBUG_TEXTOSLIDES 0

SongSlideGroup::SongSlideGroup() : SlideGroup(),
	m_song(0),
	m_text(""),
	m_isTextDiffFromDb(false),
	m_slideTemplates(0),
	m_lastAspectRatio(-1),
	m_syncToDatabase(true)
{
	setEndOfGroupAction(SlideGroup::GotoNextGroup);
	if(MainWindow::mw())
		connect(MainWindow::mw(), SIGNAL(aspectRatioChanged(double)), this, SLOT(aspectRatioChanged(double)));
	
	connect(&m_textRegenTimer, SIGNAL(timeout()), this, SLOT(textToSlides()));
	m_textRegenTimer.setSingleShot(true);
	m_textRegenTimer.setInterval(10);
}

SongSlideGroup::~SongSlideGroup()
{
	if(m_song)
		delete m_song;
}

void SongSlideGroup::setSyncToDatabase(bool flag)
{
	m_syncToDatabase = flag;
}

void SongSlideGroup::hitTextToSlides()
{
	if(m_textRegenTimer.isActive())
		m_textRegenTimer.stop();
	m_textRegenTimer.start();
	//qDebug() << "SongSlideGroup::hitTextToSlides()";
}

void SongSlideGroup::setSong(SongRecord *songRecord)
{
	removeAllSlides();

	m_song = songRecord;
	
	if(!songRecord)
		return;
		
	//qDebug() << "SongSlideGroup::setSong: songId:"<<m_song->songId()<<", title: "<<s->title();

	m_text = songRecord->text();
	
	SongArrangement *arr = songRecord->defaultArrangement();
	m_arrangement    = arr->arrangement();
	m_slideTemplates = arr->templateGroup();
	
	setGroupTitle(songRecord->title());

	// convert the text to slides
	//textToSlides();
	hitTextToSlides();
}

/* protected */
void SongSlideGroup::removeAllSlides()
{
	foreach(Slide *slide, m_slides)
	{
		disconnect(slide,0,this,0);
		//m_slides.removeAll(slide);
		emit slideChanged(slide, "remove", 0, "", "", QVariant());
	}

	//qDeleteAll(m_slides);
	m_slides.clear();
}

void SongSlideGroup::aspectRatioChanged(double newAr)
{
	if(newAr != m_lastAspectRatio)
	{
		removeAllSlides();
		//textToSlides();
		hitTextToSlides();
		
		m_lastAspectRatio = newAr;
		//qDebug() << "SongSlideGroup::aspectRatioChanged: Got new AR:"<<newAr;
	}
	else
	{
		//qDebug() << "SongSlideGroup::aspectRatioChanged: newAr not changed:"<<newAr;
	}
}

SlideGroup * SongSlideGroup::createDefaultTemplates()
{
	SlideGroup *group = new SlideGroup();

	Slide *slide = new Slide();

	BackgroundItem * bg = dynamic_cast<BackgroundItem*>(slide->background());
	if(bg)
	{
		bg->setFillType(AbstractVisualItem::Solid);
		bg->setFillBrush(Qt::blue);
	}
	else
	{
		qDebug() << "SongSlideGroup::createDefaultTemplates: Could not find background in slide.";
	}

	// Create the textbox to hold the slide lyrics
	TextBoxItem * text = new TextBoxItem();
	text->setItemId(ItemFactory::nextId());
	text->setItemName(QString("TextBox%1").arg(text->itemId()));

	QRectF textRect = AppSettings::adjustToTitlesafe(MainWindow::mw() ? MainWindow::mw()->standardSceneRect() : FALLBACK_SCREEN_RECT);

	text->setPos(QPointF(0,0));
	text->setContentsRect(textRect);

	QPen pen = QPen(Qt::black,1.5);
	pen.setJoinStyle(Qt::MiterJoin);
	text->setOutlinePen(pen);

	text->setOutlineEnabled(true);
	text->setShadowEnabled(true);
	text->setShadowBlurRadius(11);
	
	static QString slideHeader = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
				     "<html>"
				     "<head><meta name=\"qrichtext\" content=\"1\" />"
				     "<style type=\"text/css\">p, li { white-space: pre-wrap; }</style>"
				     "</head>"
				     "<body style=\"font-family:'Sans Serif'; font-size:32pt; font-weight:400; font-style:normal;\">";
	static QString linePrefix  = "<p align=\"center\" style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
				     "<span style=\" font-family:'Sans-Serif'; font-size:32pt; font-weight:800;\">";
	static QString lineSuffix =  "</span>"
				     "</p>";
	static QString slideFooter = "</body></html>";
	
	text->setText(QString("%1%2Template%3%4")
		.arg(slideHeader)
		.arg(linePrefix)
		.arg(lineSuffix)
		.arg(slideFooter));

	slide->addItem(text);
	group->addSlide(slide);

	return group;
}


bool SongSlideGroup_itemZCompare(AbstractItem *a, AbstractItem *b)
{
	AbstractVisualItem * va = dynamic_cast<AbstractVisualItem*>(a);
	AbstractVisualItem * vb = dynamic_cast<AbstractVisualItem*>(b);

	// Decending sort (eg. highest to lowest) because we want the highest
	// zvalue first n the list instead of last
	return (va && vb) ? va->zValue() > vb->zValue() : true;
}

void SongSlideGroup::textToSlides(SongTextFilter filter)
{
	if(DEBUG_TEXTOSLIDES)
		qDebug()<<"SongSlideGroup::textToSlides(): "<<(song() ? song()->title() : " (no song) ")<<": Start of text to slides";
	
	static QString slideHeader = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
				     "<html>"
				     "<head><meta name=\"qrichtext\" content=\"1\" />"
				     "<style type=\"text/css\">p, li { white-space: pre-wrap; }</style>"
				     "</head>"
				     "<body style=\"font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">";
	static QString linePrefix  = "<p align=\"center\" style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
				     "<span style=\" font-family:'Sans-Serif'; font-size:32pt; font-weight:800;\">";
	static QString lineSuffix =  "</span>"
				     "</p>";
	static QString slideFooter = "</body></html>";
	
	QString text = rearrange(m_text, m_arrangement);
	QStringList list = text.split("\n\n");
	
	//qDebug()<<"SongSlideGroup::textToSlides(): "<<(song() ? song()->title() : " (no song) ")<<": Using text: "<<list; 

	// Outline pen for the text
	QPen pen = QPen(Qt::black,1.5);
	pen.setJoinStyle(Qt::MiterJoin);

	// Used for centering and font size search
	QTextDocument doc;

	QRegExp excludeLineRegExp(
        		//filter == Standard ? "^\\s*(Verse|Chorus|Tag|Bridge|End(ing)?|Intro(duction)|B:|R:|C:|T:|G:)?)(\\s*\\(.*\\))?\\s*$" :
        		filter == Standard  ? tr("^\\s*(%1|B:|R:|C:|T:|G:|\\[|\\|)(\\s+\\d+)?(\\s*\\(.*\\))?\\s*.*$").arg(SongSlideGroup::songTagRegexpList()) :
        		filter == AllowRear ? tr("^\\s*(%1)(\\s+\\d+)?(\\s*\\(.*\\))?\\s*.*$").arg(SongSlideGroup::songTagRegexpList()) :
        		"",
        	Qt::CaseInsensitive);

	if(!slideTemplates() || !slideTemplates()->numSlides())
	{
		if(m_slideTemplates)
			delete m_slideTemplates;
		m_slideTemplates = createDefaultTemplates();
	}
	
        if(DEBUG_TEXTOSLIDES)
        	qDebug() << "SongSlideGroup::textToSlides(): filter int:"<<filter<<", using exclusion pattern:"<<excludeLineRegExp.pattern();

	SlideGroup * templates = slideTemplates();
	
	if(song())
	{
		if(DEBUG_TEXTOSLIDES)
			qDebug() << "SongSlideGroup::textToSlides(): "<<song()->title()<<": Checking for output tempaltes";
		
		QList<Output*> allOut = AppSettings::outputs();
		foreach(Output *out, allOut)
		{
			SlideGroup *outputTemplate = templates->altGroupForOutput(out);
			if(outputTemplate)
			{
				if(DEBUG_TEXTOSLIDES)
					qDebug() << "SongSlideGroup::textToSlides(): "<<song()->title()<<": Creating alternate group from template for output: "<<out->name();
				
				SongSlideGroup *group = new SongSlideGroup();
				group->setSong(song());
				group->setSlideTemplates(outputTemplate);
				
				if(SlideGroup *oldGroup = altGroupForOutput(out))
				{
					setAltGroupForOutput(out, 0); // just to be safe...
					if(SongSlideGroup *oldSongGroup = dynamic_cast<SongSlideGroup*>(oldGroup))
						oldSongGroup->setSong(0); // if we don't zero, it deletes song, below...
					delete oldGroup;
				}
						
				setAltGroupForOutput(out, group);
			}
		}
		
		if(DEBUG_TEXTOSLIDES)
			qDebug() << "SongSlideGroup::textToSlides(): "<<song()->title()<<": Done with alt templates";
	}
	
	if(DEBUG_TEXTOSLIDES)
		qDebug() << "SongSlideGroup::textToSlides(): slides:"<<list;
	int slideNbr = 0;
	foreach(QString passage, list)
	{
		Slide *slide = 0;
		TextBoxItem *text = 0;
		bool textboxFromTemplate = false;

		if(DEBUG_TEXTOSLIDES)
			qDebug()<<"SongSlideGroup::textToSlides(): Processing Slide # "<<slideNbr;

// 		if(DEBUG_TEXTOSLIDES)
// 			qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": Have templates, processing.";

		slide = templates->at(0)->clone();
		if(DEBUG_TEXTOSLIDES)
		{
			if(DEBUG_TEXTOSLIDES)
				qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": Cloned slide 0 (master)";
			AbstractItemList list = slide->itemList();
			foreach(AbstractItem *item, list)
			{
				if(DEBUG_TEXTOSLIDES)
					qDebug() << "SongSlideGroup::textToSlides(): \t Master Debug:"<<item->itemName();
			}
		}
			

		// If more than one slide, assum first slide is master slide and following slides are keyed to verses
		if(templates->numSlides() > 1 && slideNbr+1 < templates->numSlides())
		{
			// Since we've cloned a master slide and have another slide to clone, calc the max Z on the
			// master and rebase everything on secondary slide starting at the maxZ of the master slide
			double masterZValue = 0;
			BackgroundItem * masterBg = 0;
			QList<AbstractItem *> masterItems = slide->itemList();
			foreach(AbstractItem * item, masterItems)
			{
				AbstractVisualItem * visual = dynamic_cast<AbstractVisualItem*>(item);
				if(visual && visual->zValue() > masterZValue)
					masterZValue = visual->zValue();
				
				BackgroundItem * bgTmp = dynamic_cast<BackgroundItem*>(item);
				if(bgTmp && bgTmp->fillType() != AbstractVisualItem::None)
					masterBg = bgTmp;
			}
			if(DEBUG_TEXTOSLIDES)
				qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": masterZValue:"<<masterZValue;


			// Add items from the song slide to our master slide.
			// Don't clone the song slide since we've already cloned
			// the master slide - instead, we'll clone the items in
			// this slide, below.
			Slide *songSlide = templates->at(slideNbr+1);

			if(DEBUG_TEXTOSLIDES)
				qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": Cloned passage slide for #"<<slideNbr;

			bool secondaryBg = false;	
			QList<AbstractItem *> items = songSlide->itemList();
			foreach(AbstractItem * item, items)
			{
				BackgroundItem * bg = dynamic_cast<BackgroundItem*>(item);
				if(bg)
				{
					if(bg->fillType() == AbstractVisualItem::None)
					{
						if(DEBUG_TEXTOSLIDES)
							qDebug() << "Skipping inherited bg from seondary slide because exists and no fill";
						continue;
					}
					else
					{
						secondaryBg = true;
					}
				}
				

				AbstractItem * newItem = item->clone();
				AbstractVisualItem * newVisual = dynamic_cast<AbstractVisualItem*>(newItem);

				// rebase zvalue so everything in this slide is on top of the master slide
				if(newVisual && masterZValue!=0)
				{
					newVisual->setZValue(newVisual->zValue() + masterZValue);
					if(DEBUG_TEXTOSLIDES)
						qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": rebased" << newVisual->itemName() << "to new Z" << newVisual->zValue();
				}

				slide->addItem(newItem);
			}
			
			// If we have both a master and a child bg, remove the master bg
			if(secondaryBg && masterBg)
			{
				if(DEBUG_TEXTOSLIDES)
					qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": Found secondaryBg on slide"<<slideNbr<<", removing masterBg";
				slide->removeItem(masterBg);
			}
		}

		// Use the first textbox in the slide as the lyrics slide
		// "first" as defined by ZValue
		QList<AbstractItem *> items = slide->itemList();
		qSort(items.begin(), items.end(), SongSlideGroup_itemZCompare);

		foreach(AbstractItem * item, items)
		{
			AbstractVisualItem * newVisual = dynamic_cast<AbstractVisualItem*>(item);
			if(DEBUG_TEXTOSLIDES)
				qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": item list: "<<newVisual->itemName();
			if(!text && item->itemClass() == TextBoxItem::ItemClass)
			{
				text = dynamic_cast<TextBoxItem*>(item);
				textboxFromTemplate = true;
				if(DEBUG_TEXTOSLIDES)
					qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": Found textbox from template, name:"<<text->itemName();
			}
		}

		// If no textbox, we've got to create one!
		if(!text)
		{
			// Create the textbox to hold the slide lyrics
			text = new TextBoxItem();
			text->setItemId(ItemFactory::nextId());
			text->setItemName(QString("TextBox%1").arg(text->itemId()));

			if(DEBUG_TEXTOSLIDES)
				qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": No textbox in template, adding new box.";
		}


		BackgroundItem * bg = dynamic_cast<BackgroundItem*>(slide->background());
		if(bg)
		{
			if(DEBUG_TEXTOSLIDES)
				qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": Loaded background "<<bg->itemName()<<" in final slide";
		}
		else
		{
			if(DEBUG_TEXTOSLIDES)
				qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": No background in final slide!.";
		}
	

		// Run a basic algorithim to find the max font size to fit inside this text rectangle (textRect)
		QString htmlStr;
		QRectF screenRect = AppSettings::adjustToTitlesafe(MainWindow::mw() ? MainWindow::mw()->standardSceneRect() : FALLBACK_SCREEN_RECT);
		QRectF textRect = screenRect;
		int currentMinTextSize = 32;
		if(textboxFromTemplate)
		{
			textRect = text->contentsRect();
			
			currentMinTextSize = text->findFontSize();
			
			// Create the text for the lyrics
			QStringList filtered;
			QStringList lines = passage.split("\n");
			foreach(QString line, lines)
				if(filter == AllowAll || !line.contains(excludeLineRegExp))
					filtered << line;
					
			QString filteredPassage = filtered.join("\n");
			
			// First, grab the html from the text box from the template
			QTextDocument doc;
			if (Qt::mightBeRichText(text->text()))
				doc.setHtml(text->text());
			else
				doc.setPlainText(text->text());
			
			// Select the html in the template and extract the character format
			QTextCursor cursor(&doc);
			cursor.select(QTextCursor::Document);
			QTextCharFormat charFormat = cursor.charFormat();
			QTextBlockFormat blockFormat = cursor.blockFormat();
			QTextCharFormat blockCharFormat = cursor.blockCharFormat();
			
			// Replace the document with the lyrics text
			if (Qt::mightBeRichText(filteredPassage))
				doc.setHtml(filteredPassage);
			else
				doc.setPlainText(filteredPassage);
				
			// Merge back in the extracted character format
			QTextCursor cursor2(&doc);
			cursor2.select(QTextCursor::Document);
			
			cursor2.mergeBlockCharFormat(blockCharFormat);
			cursor2.mergeBlockFormat(blockFormat);
			cursor2.mergeCharFormat(charFormat);
			
			// Get the resulting HTML to apply to the text item
			htmlStr = doc.toHtml();
		}
		else
		{
			// Create the HTML for the lyrics
			QStringList lines = passage.split("\n");
			QStringList html;
			html << slideHeader;
			foreach(QString line, lines)
			{
				if(filter == AllowAll || !line.contains(excludeLineRegExp))
				{
					html << linePrefix;
					html << line;
					html << lineSuffix;
				}
			}
			html << slideFooter;
			
		 	htmlStr = html.join("");
		}
// 
// 		int ptSize = 32;	// starting pt size (must match pt size in static html above)
// 		int sizeInc = 4;	// how big of a jump to add to the ptSize each iteration
// 		int maxPtSize = 72;
// 
// 		int count = 0;		// current loop iteration
// 		int maxCount = 50; 	// max iterations of the search loop (allows font to get up to 232pt)
// 		bool done = false;
// 
// 		int lastGoodSize = ptSize;
// 		QString lastGoodHtml = htmlStr;
// 
// 		// for centering
// 		qreal boxHeight = -1;
// 
// 		while(!done && count++ < maxCount)
// 		{
// 			QString htmlCopy = htmlStr;
// 			htmlCopy = htmlCopy.replace("size:32pt",QString("size:%1pt").arg(ptSize));
// 			doc.setHtml(htmlCopy);
// 			doc.setTextWidth(textRect.width());
// 
// 			QSizeF sz = doc.size();
// 
// 			if(sz.height() < textRect.height() &&
// 			   ptSize < maxPtSize)
// 			{
// 				lastGoodSize = ptSize;
// 				lastGoodHtml = htmlCopy;
// 				boxHeight = sz.height();
// 
// 				//qDebug()<<"size search: "<<ptSize<<"pt was good, trying higher";
// 				ptSize += sizeInc;
// 
// 			}
// 			else
// 			{
// 				if(DEBUG_TEXTOSLIDES)
// 					qDebug()<<"SongSlideGroup::textToSlides(): size search: last good ptsize:"<<lastGoodSize<<", stopping search";
// 				done = true;
// 			}
// 		}
// 
// 		htmlStr = lastGoodHtml;
		text->setText(htmlStr);
		qreal boxHeight = text->fitToSize(textRect.size().toSize(), currentMinTextSize, 72);
		//qDebug() << "SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": firtToSize boxHeight:"<<boxHeight<<", given size:"<<textRect.size().toSize();
		
		// these two dynamic properties are used in the SongFoldbackTextFilter to 
		// reference back to this slide group, extract original text, and mutate
		// it for the foldback display
		QVariant slideGroupVar;
		slideGroupVar.setValue(this);
		text->setProperty("SongSlideGroup",  slideGroupVar);
		text->setProperty("SongSlideNumber", slideNbr);
			
		// Finalize setup
		// magic number 
		int heightDifference = abs(screenRect.height() - textRect.height());
		if(DEBUG_TEXTOSLIDES)
			qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": textRect.height():"<<textRect.height()<<"screenRect.height():"<<screenRect.height()<<", heightDifference:"<<heightDifference;
		
		// Arbitrary magic number to force centering for small amounts of differences.
		// We test the difference here because don't want to force-center the textbox if it came from the template
		// and was intentionally located off-center. But if the user tried to get it to fill the screen and missed
		// by a few pixels (<20), then go ahead and center it for the user.
		if(heightDifference < 20)
		{
			
			// Center text on screen
			if(boxHeight > -1)
			{
				qreal y = textRect.height()/2 - boxHeight/2;
				//qDebug() << "SongSlideGroup::textToSlides(): centering: boxHeight:"<<boxHeight<<", textRect height:"<<textRect.height()<<", centered Y:"<<y;
				textRect = QRectF(screenRect.x(),y + screenRect.y(),textRect.width(),boxHeight);
			}

			text->setPos(QPointF(0,0));
			text->setContentsRect(textRect);
			
			if(!textboxFromTemplate)
			{
				text->setOutlinePen(pen);
				text->setOutlineEnabled(true);
				text->setShadowEnabled(true);
				text->setShadowBlurRadius(11);
				slide->addItem(text);
			}

			if(DEBUG_TEXTOSLIDES)
				qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": Textbox was not in template (or almost centered), finalized setup at 0x0, rect:"<<textRect;
		}
		
		//qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": textboxFromTemplate:"<<textboxFromTemplate<<", boxHeight:"<<boxHeight;

		slide->setSlideNumber(slideNbr++);
		
		addSlide(slide);
		
		// Delay warming the visual cache to increase UI responsiveness when quickly adding songs
		int rv = (rand() % 500) - (500/2); // get a random number +/- 250ms
		int delay = slideNbr * 5000 + rv; // add a random +/- 250ms to stagger hits to the warming method
		if(DEBUG_TEXTOSLIDES)
			qDebug()<<"SongSlideGroup::textToSlides(): "<<(song() ? song()->title() : " (no song) ")<<": slideNbr:"<<slideNbr<<": delaying "<<delay<<"ms before warmVisualCache()";
		QTimer::singleShot(delay, text, SLOT(warmVisualCache()));
		
		if(DEBUG_TEXTOSLIDES)
			qDebug()<<"SongSlideGroup::textToSlides(): Added passage:"<<passage;
		//qDebug()<<"SongSlideGroup::textToSlides(): Added slide#:"<<slide->slideNumber()<<", numSlides():"<<numSlides();
	}
	
	if(DEBUG_TEXTOSLIDES)
		qDebug()<<"SongSlideGroup::textToSlides():"<<(song() ? song()->title() : " (no song) ")<<": End of text to slides, numSlides():"<<numSlides();
}

/* public */
void SongSlideGroup::setText(QString newText)
{
	//emit slideChanged(slide, "change", 0, "", "", QVariant());
	//Pseudo code:
	// 	if newText != m_text
	//		remove all slides
	//		emite slidechanged -remove
	//		convert text to slides
	//		add slides
	//		emit slide changed - add
	//	if newText != song text
	//		set text diff from db flag

	QString oldText = m_text;

	m_text = newText;

	if(newText != oldText)
	{
		//qDebug() << "SongSlideGroup::setText: Text is different, regenerating.";
		removeAllSlides();
		//textToSlides();
		hitTextToSlides();
	}

	if(song() && newText != song()->text())
		m_isTextDiffFromDb = true;
}

void SongSlideGroup::setSlideTemplates(SlideGroup *templates)
{
	m_slideTemplates = templates;
	removeAllSlides();
	//textToSlides();
	hitTextToSlides();
}

bool SongSlideGroup::fromXml(QDomElement & pe)
{

	//qDebug() << "SongSlideGroup::fromXml(): Start";
	QDomElement slideTemplateElement = pe.firstChildElement("templates");
	if(!slideTemplateElement.isNull())
	{
		SlideGroup *templates = new SlideGroup();

		// restore the item, and delete it if something goes wrong
		if (!templates->fromXml(slideTemplateElement))
		{
			// was qWarning
			qDebug("SongSlideGroup::fromXml(): Couldn't load templates from XML for some reason. Need to debug.");
			delete templates;
		}
		else
		{
			setSlideTemplates(templates);
		}
	}

	int songid = pe.attribute("songid").toInt();
	//qDebug() << "SongSlideGroup::fromXml(): songid from xml:"<<songid;

	SongRecord *song = SongRecord::retrieve(songid);
	if(song && song->songId())
	{
		//qDebug() << "SongSlideGroup::fromXml(): Validated song, title: "<<song->title();
		setSong(song);
		//qDebug() << "SongSlideGroup::fromXml(): Set song done,  title: "<<song->title()<<", num slides:"<<numSlides();
		//qDebug() << "SongSlideGroup::fromXml: VALIDATED SongID "<<songid<<", songTitle:"<<song->title();
	}
	else
	{
		// was qWarning
		qDebug("SongSlideGroup::fromXml: Invalid songid %d in XML!",songid);
	}

	//qDebug() << "SongSlideGroup::fromXml(): Loading text, num slides:"<<numSlides();
	QString text = pe.firstChildElement("text").text();
	setText(text);
	//qDebug() << "SongSlideGroup::fromXml(): Text loaded,  num slides:"<<numSlides();

	// load group atrributes after calling setSong() above so it can override the group title 
	loadGroupAttributes(pe);

	//qDebug() << "SongSlideGroup::fromXml(): Done loading song group, num slides:"<<numSlides();
	return true;
}

void SongSlideGroup::fromVariantMap(QVariantMap &map)
{
	int songid = map["songid"].toInt();
	//qDebug() << "SongSlideGroup::fromXml(): songid from xml:"<<songid;

	SongRecord *song = SongRecord::retrieve(songid);
	if(song && song->songId())
	{
		//qDebug() << "SongSlideGroup::fromXml(): Validated song, title: "<<song->title();
		setSong(song);
		//qDebug() << "SongSlideGroup::fromXml(): Set song done,  title: "<<song->title()<<", num slides:"<<numSlides();
		//qDebug() << "SongSlideGroup::fromXml: VALIDATED SongID "<<songid<<", songTitle:"<<song->title();
	}
	else
	{
		// was qWarning
		qDebug("SongSlideGroup::fromXml: Invalid songid %d in XML!",songid);
	}

	QVariant templates = map["templates"];
	if(templates.isValid())
	{
		QByteArray ba = templates.toByteArray();
		if(!ba.isEmpty())
		{
			//qDebug() << "SongSlideGroup::fromVariantMap: Loading templates from byte array";
			SlideGroup *templates = SlideGroup::fromByteArray(ba);
			setSlideTemplates(templates);
		}
		else
		{
			//qDebug() << "
		}
	}
	
	QVariantList arr = map["arr"].toList();
	m_arrangement.clear();
	foreach(QVariant var, arr)
		m_arrangement << var.toString();
		
	//m_arrangement = QStringList() << "Title" << "Verse 3" << "Verse 2" << "Chorus" << "Chorus";
	
	//qDebug() << "SongSlideGroup::fromXml(): Loading text, num slides:"<<numSlides();
	QString text = map["text"].toString();
	setText(text);
	//qDebug() << "SongSlideGroup::fromXml(): Text loaded,  num slides:"<<numSlides();

	// load group atrributes after calling setSong() above so it can override the group title 
	loadProperties(map);
	
	
	
}
	
void SongSlideGroup::toVariantMap(QVariantMap &map) const
{
	saveProperties(map);
	
	// song specific stuff
	map["text"] = m_text;
	
	QVariantList arr;
	foreach(QString name, m_arrangement)
		arr << name;
	map["arr"] = QVariant(arr);
	
	if(m_song)
		map["songid"] = m_song->songId();
	
	if(m_slideTemplates)
		map["templates"] = m_slideTemplates->toByteArray();
}

void SongSlideGroup::toXml(QDomElement & pe) const
{
	pe.setTagName("song");

	saveGroupAttributes(pe);
	
	// song specific stuff
	if(m_song)
	{
		pe.setAttribute("songid",m_song->songId());
		//qDebug() << "SongSlideGroup::toXml: songId:"<<m_song->songId()<<", title: "<<m_song->title();
	}

	QDomDocument doc = pe.ownerDocument();
	QDomElement domElement = doc.createElement("text");
	pe.appendChild(domElement);
	domElement.appendChild(doc.createTextNode(m_text));

	if(m_slideTemplates)
	{
		QDomElement element = doc.createElement("templates");
		pe.appendChild(element);
		m_slideTemplates->toXml(element);
	}

}


void SongSlideGroup::changeBackground(AbstractVisualItem::FillType fillType, QVariant fillValue, Slide *onlyThisSlide)
{
	SlideGroup::changeBackground(fillType,fillValue,onlyThisSlide);
	
	if(!slideTemplates() || !slideTemplates()->numSlides())
	{
		if(m_slideTemplates)
			delete m_slideTemplates;
		m_slideTemplates = createDefaultTemplates();
	}
	
	m_slideTemplates->changeBackground(fillType,fillValue,m_slideTemplates->at(0));
}

void SongSlideGroup::setArrangement(QStringList arr)
{
	m_arrangement = arr;
}


QString SongSlideGroup::rearrange(QString text, QStringList arragement)
{
	QString cleanedText = text.replace("\r\n","\n");
	if(arragement.isEmpty())
		return cleanedText;
	
	bool isEmpty = true;
	foreach(QString block, arragement)
	if(!block.isEmpty())
		isEmpty = false;
	
	if(isEmpty)
		return cleanedText;
	
	//qDebug() << "SongSlideGroup::rearrange: Original text: "<<text;
	QStringList defaultArr = findDefaultArragement(text);
	//qDebug() << "SongSlideGroup::rearrange: Original arrangement: "<<defaultArr;
	
	QRegExp blockTitleRegexp(tr("^\\s*((?:%1)(?:\\s+\\d+)?(?:\\s*\\(.*\\))?)\\s*.*$").arg(SongSlideGroup::songTagRegexpList()), Qt::CaseInsensitive);

	QStringList blockList = cleanedText.split("\n\n");
	
	QHash<QString,QString> blockHash;
	
	QString curBlockTitle;
	QStringList curBlockText;
	bool firstBlock = true;
	foreach(QString passage, blockList)
	{
		int pos = blockTitleRegexp.indexIn(passage);
		if(pos != -1)
		{
			// first, add block to hash if we have a block already
			if(!curBlockTitle.isEmpty())
			{
				blockHash[curBlockTitle] = curBlockText.join("\n\n");
			}
			else
			// or this is the first block and no title given - assume "Title" block
			if(firstBlock && curBlockTitle.isEmpty() && !curBlockText.isEmpty())
			{
				curBlockTitle = "Title";
				blockHash[curBlockTitle] = curBlockText.join("\n\n");
			}
			
			firstBlock = false;
			
			// start new block
			curBlockTitle = blockTitleRegexp.cap(1);
			curBlockText.clear();
		}
		
		QStringList filtered;
		QStringList lines = passage.split("\n");
		foreach(QString line, lines)
			if(!line.contains(blockTitleRegexp))
				filtered << line;
				
		curBlockText.append(filtered.join("\n"));
	}
	
	if(!curBlockTitle.isEmpty())
		blockHash[curBlockTitle] = curBlockText.join("\n\n");
		
	//qDebug() << "SongSlideGroup::rearrange: Original blocks: "<<blockHash;
	//qDebug() << "SongSlideGroup::rearrange: Processing arragnement: "<<arragement;
	
	QStringList output;
	foreach(QString blockTitle, arragement)
	{
		if(blockHash.contains(blockTitle))
		{
			//qDebug() << "SongSlideGroup::rearrange: [process] Block: "<<blockTitle;
			output << QString("%1\n%2")
				.arg(blockTitle)
				.arg(blockHash.value(blockTitle));
		}
		else
		{
			qDebug() << "SongSlideGroup::rearrange: [process] [Error] Arrangement block not found in original text: "<<blockTitle;
		}
	}
	
	QString outputText = output.join("\n\n");
	//qDebug() << "SongSlideGroup::rearrange: Output: "<<outputText;
	return outputText;
}


QStringList SongSlideGroup::findDefaultArragement(QString text)
{
	QRegExp blockTitleRegexp(tr("^\\s*((?:%1)(?:\\s+\\d+)?(?:\\s*\\(.*\\))?)\\s*.*$").arg(SongSlideGroup::songTagRegexpList()), Qt::CaseInsensitive);

	QString cleanedText = text.replace("\r\n","\n");
	QStringList blockList = cleanedText.split("\n\n");
	
	QStringList blockTitleList;
	
	QString curBlockTitle;
	bool firstBlock = true;
	int contentLines = 0;
	foreach(QString passage, blockList)
	{
		int pos = blockTitleRegexp.indexIn(passage);
		if(pos != -1)
		{
			// first, add block to hash if we have a block already
			if(!curBlockTitle.isEmpty())
				blockTitleList << curBlockTitle;
			else
			// or this is the first block and no title given - assume "Title" block
			if(firstBlock && curBlockTitle.isEmpty() && contentLines > 0)
			{
				blockTitleList << "Title";
			}
			
			
			// start new block
			curBlockTitle = blockTitleRegexp.cap(1);
			contentLines = 0;
			firstBlock = false;
		}
		
		contentLines ++;
	}
	
	if(!curBlockTitle.isEmpty())
		blockTitleList << curBlockTitle;
	
	return blockTitleList;
}

