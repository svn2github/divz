#include "SongSlideGroupListModel.h"
#include "SongSlideGroup.h"
#include <QAbstractTextDocumentLayout>
#include "MainWindow.h"
#include "model/Slide.h"
//#include "3rdparty/md5/md5.h"
#include <QPixmapCache>

SongSlideGroupListModel::SongSlideGroupListModel(SlideGroup *g, QObject *parent) 
	: SlideGroupListModel(g,parent)
{
	m_text = new QTextDocument();	
}

SongSlideGroupListModel::~SongSlideGroupListModel()
{
	delete m_text;
}
	
QVariant SongSlideGroupListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
	
	if (index.row() >= m_sortedSlides.size())
		return QVariant();
	
	if (role == Qt::DisplayRole)
	{
		//return QString("Slide %1").arg(index.row()); 
		return QVariant(); 
	}
	else
	{
		return SlideGroupListModel::data(index,role);
	}
}
	

QPixmap SongSlideGroupListModel::generatePixmap(Slide *slide)
{
	//return;
	
	// Ask the parent to generate a pixmap *just* so that 
	// the TextBoxContent in the slide at "row" can cache
	// rendered pixmaps before going live.
	// Disabled for now due to the performance hit by this function for long slide groups such as long songs
	//SlideGroupListModel::generatePixmap(row);
	
	static QString slideHeader = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">p, li { white-space: pre-wrap; }</style></head><body style=\"font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">";
	static QString slideFooter = "</body></html>";
	static QString linePrefix = "<p align=\"left\" style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400;\">";
	static QString lineSuffix = "</span></p>";

	static QString highlightBlockPrefix = "<p align=\"left\" style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; background:red; color:white\"><span style=\" font-family:'Sans Serif'; color:white;background:red;font-size:9pt; font-weight:400;\">";
	
	static QRegExp rxTag("\\s*(Verse|Chorus|Tag|Bridge|End(ing)?|Intro(duction)?)(\\s+\\d+)?(\\s*\\(.*\\))?\\s*");
	static QRegExp rxRear("\\s*((?:B:|R:|C:|T:|G:|\\[|\\|).*)\\s*");
	
	SongSlideGroup * songGroup = dynamic_cast<SongSlideGroup*>(m_slideGroup);
	if(!songGroup)
	{
		return QPixmap();
	}
	
	QString text = songGroup->text().replace("\r\n","\n");
	QStringList list = text.split("\n\n");
	
	int row = slide->slideNumber();
	QString passage = row >= list.size() ? "" : list[row];
	
	QPixmap icon;
	if(!QPixmapCache::find(passage,icon))
	{
		//qDebug() << "SongSlideGroup: Regen row "<<row;
		
		// Outline pen for the text
		QPen pen = QPen(Qt::black,1.5);
		pen.setJoinStyle(Qt::MiterJoin);
		
		// Used for centering and font size search
		QTextDocument doc;
	/*	
		static QRegExp excludeLineRegExp(
				//filter == Standard ? "^\\s*(Verse|Chorus|Tag|Bridge|End(ing)?|Intro(duction)|B:|R:|C:|T:|G:)?)(\\s*\\(.*\\))?\\s*$" :
				//filter == Standard  ? 
				"^\\s*(Verse|Chorus|Tag|Bridge|End(ing)?|Intro(duction)?|B:|R:|C:|T:|G:|\\|)(\\s+\\d+)?(\\s*\\(.*\\).*)?\\s*$"
				//filter == AllowRear ? "^\\s*(Verse|Chorus|Tag|Bridge|End(ing)?|Intro(duction)?)(\\s+\\d+)?(\\s*\\(.*\\))?\\s*$" :
				//"",
				,
			Qt::CaseInsensitive);*/
		
		// Create the HTML for the lyrics
		QStringList lines = passage.split("\n");
		QStringList html;
		html << slideHeader;
		foreach(QString line, lines)
		{
			if(line.contains(rxTag))
				html << highlightBlockPrefix;
			else
			if(line.contains(rxRear))
				continue;
			else
				html << linePrefix;
				
			html << line;
			html << lineSuffix;
		}
		html << slideFooter;
		
		m_text->setHtml(html.join(""));
		
		// Run a basic algorithim to find the max font size to fit inside this screen
		QString htmlStr = html.join("");
		
		QRectF textRect = MainWindow::mw() ? MainWindow::mw()->standardSceneRect() : QRectF(0,0,1024,768);
		
		int iconWidth = textRect.width() / 48 * 9;
		m_text->setTextWidth(iconWidth);
		
		int iconHeight = m_text->size().height();
		
		//qDebug()<<"SongSlideGroupListModel::generatePixmap: passage#:"<<row<<"<<, iconWidth:"<<iconWidth<<", iconHeight:"<<iconHeight;
		
		//QPixmap icon(iconWidth,iconHeight);
		icon = QPixmap(iconWidth,iconHeight);
		icon.fill(Qt::transparent);
		
		QPainter textPainter(&icon);
		QAbstractTextDocumentLayout::PaintContext pCtx;
		m_text->documentLayout()->draw(&textPainter, pCtx);
		
		textPainter.setPen(QPen(Qt::black,1,Qt::DotLine));
		textPainter.setBrush(Qt::NoBrush);
		textPainter.drawLine(0,iconHeight-1,iconWidth-1,iconHeight-1);
		
		textPainter.end();
		
		QPixmapCache::insert(passage,icon);
	}
		
 	return icon;
}
