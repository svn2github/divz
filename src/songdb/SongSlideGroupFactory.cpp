#include "SongSlideGroupFactory.h"

#include "SongEditorWindow.h"
#include "SongSlideGroup.h"
#include "SongSlideGroupListModel.h"

#include "OutputInstance.h"
#include "model/Output.h"
#include "model/TextBoxItem.h"

#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include <assert.h>

/** SongFoldbackTextFilter **/

SongFoldbackTextFilter * SongFoldbackTextFilter::m_staticInstance = new SongFoldbackTextFilter();

SongFoldbackTextFilter::SongFoldbackTextFilter()
	: SlideTextOnlyFilter()
	{}

bool SongFoldbackTextFilter::isMandatoryFor(OutputInstance * inst)
{
	return inst && inst->output() && (inst->output()->name().toLower().indexOf("foldback") > -1 ||
					  inst->output()->tags().toLower().indexOf("foldback") > -1);
}

bool SongFoldbackTextFilter::approve(AbstractItem *) { return true; }
AbstractItem * SongFoldbackTextFilter::mutate(const AbstractItem *sourceItem)
{
		
	static QString slideHeader = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">p, li { white-space: pre-wrap; }</style></head><body style=\"font-family:'Monospace'; font-size:38pt; font-weight:800; font-style:normal;\">";
	static QString slideFooter = "</body></html>";
	static QString linePrefix = "<p align=\"left\" style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Monospace'; font-size:38pt; font-weight:800;\">";
	static QString lineSuffix = "</span></p>";

	static QString highlightBlockPrefix = "<p align=\"left\" style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; background:red; color:white\"><span style=\" font-family:'Monospace'; color:white;background:red;font-size:38pt; font-weight:400;\">";
	
	static QRegExp excludeLineRegExp("^\\s*(Verse|Chorus|Tag|Bridge|End(ing)?|Intro(duction)?|B:|R:|C:|T:|G:|\\|)(.*)?\\s*$",Qt::CaseInsensitive);

	//Adds in guitar chords and other rear-screen text, as well as the next passage preview on the last line
	if(sourceItem)
	{
		if(sourceItem->inherits("TextBoxItem"))
		{
			QVariant slideNumber = sourceItem->property("SongSlideNumber");
			if(slideNumber.isValid())
			{
				QVariant slideGroup = sourceItem->property("SongSlideGroup");
				SongSlideGroup * songGroup = slideGroup.value<SongSlideGroup*>();
					
				QString text = songGroup->text().replace("\r\n","\n");
				QStringList list = text.split("\n\n");
				
				int row = slideNumber.toInt();
				QString passage = row >= list.size() ? "" : list[row];
				
				
				// Create the HTML for the lyrics
				QStringList lines = passage.split("\n");
				QStringList html;
				html << slideHeader;
				foreach(QString line, lines)
				{
					if(line.contains(excludeLineRegExp))
						html << highlightBlockPrefix;
					else
						html << linePrefix;
						
					html << line;
					html << lineSuffix;
				}
				
				// add in last line
				if(row+1 < list.size())
				{
					QString next = list[row+1];
					next = next.replace(QRegExp("(Verse|Chorus|Tag|Bridge|End(ing)?|Intro(duction)?|B:|R:|C:|T:|G:|\\|)(\\s+\\d+)?(\\s*\\(.*\\).*)?",Qt::CaseInsensitive),""); // dont show rear text in first line of next slide
					QString textBlob = next.replace("\n","/");
					QString substring = textBlob.left(28); // TODO MAGIC NUMBER ...is 30 a good width?
					html << linePrefix;
					html << "..."; // <br> is intentional to give it some whitespace before this line
					html << substring;
					html << lineSuffix;
				}
				
				
				html << slideFooter;
				
				AbstractItem * clone = sourceItem->clone();
				TextBoxItem *textbox = dynamic_cast<TextBoxItem*>(clone); 
				textbox->setText(html.join(""));
				textbox->setShadowEnabled(false);
				textbox->setOutlineEnabled(false);
				//qDebug() << "Song slide filter html: "<<html.join("");
				
				return clone;
				
			}
			else
			{
				// no way to ID which passage of the song this textbox is for,
				// therefore no way to find the original text
			}
		
		}
		else
		{
			return SlideTextOnlyFilter::mutate(sourceItem);
		}
	}
	
	return 0;
}



/** SlideGroupViewControl:: **/
SongSlideGroupViewControl::SongSlideGroupViewControl(OutputInstance *g, QWidget *w )
	: SlideGroupViewControl(g,w)
{
	if(m_slideModel)
		delete m_slideModel;
		
	//qDebug()<<"SongSlideGroupViewControl(): creating new SongSlideGroupListModel()";
	m_slideModel = new SongSlideGroupListModel();
	m_listView->setModel(m_slideModel);
	m_listView->setViewMode(QListView::ListMode);
	m_listView->reset();	
	
}



/** SongSlideGroupFactory:: **/

SongSlideGroupFactory::SongSlideGroupFactory() : SlideGroupFactory() {}
/*SongSlideGroupFactory::~SongSlideGroupFactory()
{
}*/
	
SlideGroup * SongSlideGroupFactory::newSlideGroup()
{
	return dynamic_cast<SlideGroup*>(new SongSlideGroup());
}
	
AbstractItemFilterList SongSlideGroupFactory::customFiltersFor(OutputInstance */*instace*/)
{
	AbstractItemFilterList list;
	list << SongFoldbackTextFilter::instance();
	return list;
}

SlideGroupViewControl * SongSlideGroupFactory::newViewControl()
{
	return new SongSlideGroupViewControl();
}

AbstractSlideGroupEditor * SongSlideGroupFactory::newEditor()
{
	return new SongEditorWindow();
}
