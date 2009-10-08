#include "SongSlideGroup.h"

#include "model/TextBoxItem.h"
#include "model/BackgroundItem.h"
#include "model/ItemFactory.h"
#include "MainWindow.h"

#include <QTextDocument>
#include <QTextBlock>
#include <QTextOption>


SongSlideGroup::SongSlideGroup() : SlideGroup(),
	m_song(0),
	m_text(""),
	m_isTextDiffFromDb(false)
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
		m_slides.removeAll(slide);
		emit slideChanged(slide, "remove", 0, "", "", QVariant());
	}
	
	qDeleteAll(m_slides);
}

void SongSlideGroup::aspectRatioChanged(double x)
{
	removeAllSlides();
	textToSlides();
}

void SongSlideGroup::textToSlides(SongTextFilter filter)
{
	static QString slideHeader = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">p, li { white-space: pre-wrap; }</style></head><body style=\"font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">";
	static QString slideFooter = "</body></html>";
	static QString linePrefix = "<p align=\"center\" style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Sans Serif'; font-size:32pt; font-weight:600;\">";
	static QString lineSuffix = "</span></p>";
	
	QString text = m_text.replace("\r\n","\n");
	QStringList list = text.split("\n\n");
	
	// Outline pen for the text
	QPen pen = QPen(Qt::black,1.5);
	pen.setJoinStyle(Qt::MiterJoin);
	
	// Used for centering and font size search
	QTextDocument doc;
	
	QRegExp excludeLineRegExp(
        		//filter == Standard ? "^\\s*(Verse|Chorus|Tag|Bridge|End(ing)?|Intro(duction)|B:|R:|C:|T:|G:)?)(\\s*\\(.*\\))?\\s*$" :
        		filter == Standard  ? "^\\s*(Verse|Chorus|Tag|Bridge|End(ing)?|Intro(duction)?|B:|R:|C:|T:|G:)(\\s+\\d+)?(\\s*\\(.*\\))?\\s*$" :
        		filter == AllowRear ? "^\\s*(Verse|Chorus|Tag|Bridge|End(ing)?|Intro(duction)?)(\\s+\\d+)?(\\s*\\(.*\\))?\\s*$" :
        		"",
        	Qt::CaseInsensitive);
        
        //qDebug() << "SongSlideGroup::textToSlides: filter int:"<<filter<<", using exclusion pattern:"<<excludeLineRegExp.pattern();
        
	//qDebug() << "SongSlideGroup::textToSlides: slides:"<<list;
	int slideNbr = 0;
	foreach(QString passage, list)
	{
		Slide *slide = new Slide();
		
		// Setup the slide background (TODO this needs to be a changeable in the UI once in the list
		BackgroundItem * bg = dynamic_cast<BackgroundItem*>(slide->background());
		if(bg)
		{
			bg->setFillType(AbstractVisualItem::Solid);
			bg->setFillBrush(Qt::blue);
		}
		
		// Create the textbox to hold the slide lyrics
		TextBoxItem *text = new TextBoxItem();
		text->setItemId(ItemFactory::nextId());
		text->setItemName(QString("TextBox%1").arg(text->itemId()));
		
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
		
		// Run a basic algorithim to find the max font size to fit inside this screen
		QString htmlStr = html.join("");
		QRectF textRect = MainWindow::mw()->standardSceneRect();
		
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
				//qDebug()<<"size search: last good ptsize:"<<lastGoodSize<<", stopping search";
				done = true;
			}
		}
		
		htmlStr = lastGoodHtml;
		text->setText(htmlStr);
		
		// Center text on screen
		if(boxHeight > -1)
		{
			qreal y = textRect.height()/2 - boxHeight/2;
			//qDebug() << "centering: boxHeight:"<<boxHeight<<", textRect height:"<<textRect.height()<<", centered Y:"<<y;
			textRect = QRectF(0,y,textRect.width(),boxHeight);
		}
		
		// Finalize setup
		text->setPos(QPointF(0,0));
		text->setContentsRect(textRect);
		text->setOutlineEnabled(true);
		text->setOutlinePen(pen);
		text->setShadowEnabled(true);
		slide->addItem(text);
		
		slide->setSlideNumber(slideNbr++);
		
		addSlide(slide);
	}
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
		qDebug() << "SongSlideGroup::setText: Text is different, regenerating.";
		removeAllSlides();
		textToSlides();
	}
		
	if(newText != song()->text())
		m_isTextDiffFromDb = true;
}
	

bool SongSlideGroup::fromXml(QDomElement & pe)
{
	SlideGroup::fromXml(pe);
	
	int songid = pe.attribute("songid").toInt();
	
	SongRecord *song = SongRecord::retrieve(songid);
	if(song)
	{
		setSong(song);
		//qDebug() << "SongSlideGroup::fromXml: VALIDATED SongID "<<songid;
	}
	else
	{
		qWarning("SongSlideGroup::fromXml: Invalid songid %d in XML!",songid);
	}
	
	QString text = pe.firstChildElement("text").text();
	setText(text);
	
	return true;
}

void SongSlideGroup::toXml(QDomElement & pe) const
{
	pe.setTagName("song");
	
	// duplicating SlideGroup code here because I dont want to call the super function and have it
	// save all the generated slides
	pe.setAttribute("number",groupNumber());
	pe.setAttribute("id",groupId());
	pe.setAttribute("type",(int)groupType());
	pe.setAttribute("title",groupTitle());
	pe.setAttribute("icon",iconFile());
	
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
}
