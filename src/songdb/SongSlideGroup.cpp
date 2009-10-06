#include "SongSlideGroup.h"

#include "model/TextBoxItem.h"
#include "model/BackgroundItem.h"
#include "MainWindow.h"

SongSlideGroup::SongSlideGroup() : SlideGroup(),
	m_song(0),
	m_text(""),
	m_isTextDiffFromDb(false)
{}

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
	
	if(m_song)
		removeAllSlides();
	
	m_song = s;
	
	m_text = s->text();
	
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

void SongSlideGroup::textToSlides()
{
	QString text = m_text.replace("\r\n","\n");
	QStringList list = text.split("\n\n");
	
	foreach(QString passage, list)
	{
		Slide *slide = new Slide();
		
		BackgroundItem * bg = dynamic_cast<BackgroundItem*>(slide->background());
		if(bg)
		{
			bg->setFillType(AbstractVisualItem::Solid);
			bg->setFillBrush(Qt::blue);
		}
		
		TextBoxItem *text = new TextBoxItem();
		
		text->setText(
			"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">p, li { white-space: pre-wrap; }</style></head><body style=\"font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\"><p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:48pt; font-weight:600;\">" 
				+ passage +
			"</span></p></body></html>"
		);
		
		text->setPos(QPointF(0,0));
		text->setContentsRect(MainWindow::mw()->standardSceneRect()); //QRectF(0,0,1024,768));
		slide->addItem(text);
		
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
		setSong(song);
	
	QString text = pe.firstChildElement("text").text();
	setText(text);
}

void SongSlideGroup::toXml(QDomElement & pe) 
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
	if(song())
		pe.setAttribute("songid",song()->songId());
	
	QDomDocument doc = pe.ownerDocument();
	QDomElement domElement = doc.createElement("text");
	pe.appendChild(domElement);
	domElement.appendChild(doc.createTextNode(text()));
}
