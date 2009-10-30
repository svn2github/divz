#include "SongSlideGroup.h"

#include "model/TextBoxItem.h"
#include "model/BackgroundItem.h"
#include "model/ItemFactory.h"
#include "model/Slide.h"
#include "model/SlideGroup.h"
#include "MainWindow.h"

#include <QTextDocument>
#include <QTextBlock>
#include <QTextOption>

#define DEBUG_TEXTOSLIDES 0

SongSlideGroup::SongSlideGroup() : SlideGroup(),
	m_song(0),
	m_text(""),
	m_isTextDiffFromDb(false),
	m_slideTemplates(0)
{
	if(MainWindow::mw())
		connect(MainWindow::mw(), SIGNAL(aspectRatioChanged(double)), this, SLOT(aspectRatioChanged(double)));
}

void SongSlideGroup::setSong(SongRecord *s)
{
	// Pseudo code:
	// If has previous song
	//	go thru all existing slides
	//	emit slideChanged - "remove"
	//	remove slides
	// After set song:
	//	convert song to slides
	//	emit slide changed "add"
	//emit slideChanged(slide, "add", 0, "", "", QVariant());

	//if(m_song)
	removeAllSlides();

	m_song = s;
	//qDebug() << "SongSlideGroup::setSong: songId:"<<m_song->songId()<<", title: "<<s->title();

	m_text = s->text();

	setGroupTitle(s->title());

	// convert the text to slides
	textToSlides();
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

	m_slides.clear();
	qDeleteAll(m_slides);
}

void SongSlideGroup::aspectRatioChanged(double x)
{
	removeAllSlides();
	textToSlides();
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

	QRectF textRect = MainWindow::mw() ? MainWindow::mw()->standardSceneRect() : FALLBACK_SCREEN_RECT;

	text->setPos(QPointF(0,0));
	text->setContentsRect(textRect);

	QPen pen = QPen(Qt::black,1.5);
	pen.setJoinStyle(Qt::MiterJoin);
	text->setOutlinePen(pen);

	text->setOutlineEnabled(true);
	text->setShadowEnabled(true);
	text->setShadowBlurRadius(6);

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
				     "<body style=\"font-family:'Tahoma, Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">";
	static QString linePrefix  = "<p align=\"center\" style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
				     "<span style=\" font-family:'Tahoma, Sans-Serif'; font-size:32pt; font-weight:800;\">";
	static QString lineSuffix =  "</span>"
				     "</p>";
	static QString slideFooter = "</body></html>";
	
	QString text = m_text.replace("\r\n","\n");
	QStringList list = text.split("\n\n");

	// Outline pen for the text
	QPen pen = QPen(Qt::black,1.5);
	pen.setJoinStyle(Qt::MiterJoin);

	// Used for centering and font size search
	QTextDocument doc;

	QRegExp excludeLineRegExp(
        		//filter == Standard ? "^\\s*(Verse|Chorus|Tag|Bridge|End(ing)?|Intro(duction)|B:|R:|C:|T:|G:)?)(\\s*\\(.*\\))?\\s*$" :
        		filter == Standard  ? "^\\s*(Verse|Chorus|Tag|Bridge|End(ing)?|Intro(duction)?|B:|R:|C:|T:|G:|\\[|\\|)(\\s+\\d+)?(\\s*\\(.*\\))?\\s*.*$" :
        		filter == AllowRear ? "^\\s*(Verse|Chorus|Tag|Bridge|End(ing)?|Intro(duction)?)(\\s+\\d+)?(\\s*\\(.*\\))?\\s*.*$" :
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
			qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": Cloned slide 0 (master)";
			AbstractItemList list = slide->itemList();
			foreach(AbstractItem *item, list)
			{
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

		// Run a basic algorithim to find the max font size to fit inside this text rectangle (textRect)
		QString htmlStr = html.join("");
		QRectF screenRect = MainWindow::mw() ? MainWindow::mw()->standardSceneRect() : FALLBACK_SCREEN_RECT;
		QRectF textRect = screenRect;
		if(textboxFromTemplate)
			textRect = text->contentsRect();

		int ptSize = 32;	// starting pt size (must match pt size in static html above)
		int sizeInc = 4;	// how big of a jump to add to the ptSize each iteration

		int count = 0;		// current loop iteration
		int maxCount = 50; 	// max iterations of the search loop (allows font to get up to 232pt)
		bool done = false;

		int lastGoodSize = ptSize;
		QString lastGoodHtml = htmlStr;

		// for centering
		qreal boxHeight = -1;

		while(!done && count++ < maxCount)
		{
			QString htmlCopy = htmlStr;
			htmlCopy = htmlCopy.replace("size:32pt",QString("size:%1pt").arg(ptSize));
			doc.setHtml(htmlCopy);
			doc.setTextWidth(textRect.width());

			QSizeF sz = doc.size();

			if(sz.height() < textRect.height())
			{
				lastGoodSize = ptSize;
				lastGoodHtml = htmlCopy;
				boxHeight = sz.height();

				//qDebug()<<"size search: "<<ptSize<<"pt was good, trying higher";
				ptSize += sizeInc;

			}
			else
			{
				if(DEBUG_TEXTOSLIDES)
					qDebug()<<"SongSlideGroup::textToSlides(): size search: last good ptsize:"<<lastGoodSize<<", stopping search";
				done = true;
			}
		}

		htmlStr = lastGoodHtml;
		text->setText(htmlStr);
		
		// these two dynamic properties are used in the SongFoldbackTextFilter to 
		// reference back to this slide group, extract original text, and mutate
		// it for the foldback display
		QVariant slideGroupVar;
		slideGroupVar.setValue(this);
		text->setProperty("SongSlideGroup",  slideGroupVar);
		text->setProperty("SongSlideNumber", slideNbr);
			
		// Finalize setup
		if(textRect == screenRect)
		{
			
			// Center text on screen
			if(boxHeight > -1)
			{
				qreal y = textRect.height()/2 - boxHeight/2;
				//qDebug() << "SongSlideGroup::textToSlides(): centering: boxHeight:"<<boxHeight<<", textRect height:"<<textRect.height()<<", centered Y:"<<y;
				textRect = QRectF(0,y,textRect.width(),boxHeight);
			}

			text->setPos(QPointF(0,0));
			text->setContentsRect(textRect);
			
			if(!textboxFromTemplate)
			{
				text->setOutlinePen(pen);
				text->setOutlineEnabled(true);
				text->setShadowEnabled(true);
				text->setShadowBlurRadius(7);
				slide->addItem(text);
			}

			if(DEBUG_TEXTOSLIDES)
				qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": Textbox was not in template, finalized setup at 0x0, rect:"<<textRect;
		}
		
		//qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": textboxFromTemplate:"<<textboxFromTemplate<<", boxHeight:"<<boxHeight;

		slide->setSlideNumber(slideNbr++);
		
		addSlide(slide);
		
		text->warmVisualCache();
		
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
		textToSlides();
	}

	if(song() && newText != song()->text())
		m_isTextDiffFromDb = true;
}

void SongSlideGroup::setSlideTemplates(SlideGroup *templates)
{
	m_slideTemplates = templates;
	removeAllSlides();
	textToSlides();
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
			qWarning("SongSlideGroup::fromXml(): Couldn't load templates from XML for some reason. Need to debug.");
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
		qWarning("SongSlideGroup::fromXml: Invalid songid %d in XML!",songid);
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
