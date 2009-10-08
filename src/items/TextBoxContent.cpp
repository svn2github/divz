#include "TextBoxContent.h"
#include "frames/Frame.h"
#include "items/BezierCubicItem.h"
#include "items/TextProperties.h"
#include "model/TextItem.h"
// #include "CPixmap.h"
#include "RenderOpts.h"
#include <QDebug>
#include <QFileInfo>
#include <QGraphicsScene>
#include <QMimeData>
#include <QPainter>
#include <QTextDocument>
#include <QTextFrame>
#include <QUrl>
#include <QAbstractTextDocumentLayout>
#include <QDebug>
#include <QTextOption>
#include <QTimer>
#include <QPainterPathStroker>
#include "items/CornerItem.h"

#define DEBUG_LAYOUT 0

static QString trimLeft(QString str)
{
	static QRegExp white("\\s");
	while(white.indexIn(str) == 0)
	{
		str = str.right(str.length()-1);
	}
	return str;
}


TextBoxContent::TextBoxContent(QGraphicsScene * scene, QGraphicsItem * parent)
    : AbstractContent(scene, parent, false)
    , m_text(0)
    , m_textRect(0, 0, 0, 0)
    , m_textMargin(4)
    , m_shapeEditor(0)
    , m_xTextAlign(Qt::AlignLeft)
    , m_yTextAlign(Qt::AlignTop)
{
	m_dontSyncToModel = true;
	
	setFrame(0);
	setFrameTextEnabled(false);
	setToolTip(tr("Right click to Edit the text"));
	
	// create a text document
	m_text = new QTextDocument(this);
	
	#if QT_VERSION >= 0x040500
		m_textMargin = (int)m_text->documentMargin();
	#endif
	
	// template text
	QFont font;
	#ifdef Q_OS_WIN
		font.setFamily("Arial");
	#endif
	font.setPointSize(16);
	m_text->setDefaultFont(font);
	m_text->setPlainText(tr("right click to edit..."));
	setHtml(m_text->toHtml());
	
	// shape editor
	m_shapeEditor = new BezierCubicItem(this);
	m_shapeEditor->setVisible(false);
	m_shapeEditor->setControlPoints(QList<QPointF>() << QPointF(-100, -50) << QPointF(-10, 40) << QPointF(100, -50) << QPointF(100, 50));
	connect(m_shapeEditor, SIGNAL(shapeChanged(const QPainterPath &)), this, SLOT(setShapePath(const QPainterPath &)));
	
	
	//connect(this, SIGNAL(resized()), this, SLOT(delayContentsResized()));
	
	for(int i=0;i<m_cornerItems.size();i++)
		m_cornerItems.at(i)->setDefaultLeftOp(CornerItem::Scale);
		
	m_dontSyncToModel = false;
}

TextBoxContent::~TextBoxContent()
{
	delete m_shapeEditor;
	delete m_text;
}

QString TextBoxContent::toHtml() const
{
	return m_text->toHtml();
}

void TextBoxContent::setHtml(const QString & htmlCode)
{
        //qDebug("Setting HTML... [%s]",htmlCode.toAscii().constData());
	m_text->setHtml(htmlCode);
	//qDebug()<<"TextBoxContent::setHtml: (not shown)";
	updateTextConstraints();
        //qDebug("Calling syncToModelItem");
	syncToModelItem(0);
        //qDebug("Done with syncToModelItem()");
}

void TextBoxContent::setXTextAlign(Qt::Alignment x)
{
	m_xTextAlign = x;
        //qDebug()<<"TextBoxContent::setXTextAlign: "<<x;
        updateTextConstraints();
        syncToModelItem(0);
}

void TextBoxContent::setYTextAlign(Qt::Alignment y)
{
	m_yTextAlign = y;
        //qDebug()<<"TextBoxContent::setYTextAlign: "<<y;
        updateTextConstraints();
        syncToModelItem(0);
}
	
bool TextBoxContent::hasShape() const
{
	return !m_shapePath.isEmpty();
}

void TextBoxContent::clearShape()
{
	setShapePath(QPainterPath());
	setShapeEditing(false);
	emit notifyHasShape(false);
}

bool TextBoxContent::isShapeEditing() const
{
	return m_shapeEditor->isVisible();
}

void TextBoxContent::setShapeEditing(bool enabled)
{
	if (enabled) 
	{
		// shape editor on
		if (!m_shapeEditor->isVisible()) 
		{
			m_shapeEditor->show();
			emit notifyShapeEditing(true);
		}
	
		// begin new shape
		if (!hasShape()) 
		{
			// use caching only when drawing shaped [disabled because updates are wrong when cached!]
			//setCacheMode(enabled ? QGraphicsItem::DeviceCoordinateCache : QGraphicsItem::NoCache);
		
			// use new shape
			setShapePath(m_shapeEditor->shape());
			emit notifyHasShape(true);
		}
	} 
	else 
	{
		// shape editor off
		if (m_shapeEditor->isVisible()) 
		{
			m_shapeEditor->hide();
			emit notifyShapeEditing(false);
		}
	}
}

QWidget * TextBoxContent::createPropertyWidget()
{
	TextProperties * p = new TextProperties();
	
	// common properties
	connect(p->bFront, SIGNAL(clicked()), this, SLOT(slotStackFront()));
	connect(p->bRaise, SIGNAL(clicked()), this, SLOT(slotStackRaise()));
	connect(p->bLower, SIGNAL(clicked()), this, SLOT(slotStackLower()));
	connect(p->bBack, SIGNAL(clicked()), this, SLOT(slotStackBack()));
	connect(p->bDel, SIGNAL(clicked()), this, SIGNAL(deleteItem()), Qt::QueuedConnection);
	
	// shape properties
	p->bEditShape->setChecked(isShapeEditing());
	connect(this, SIGNAL(notifyShapeEditing(bool)), p->bEditShape, SLOT(setChecked(bool)));
	connect(p->bEditShape, SIGNAL(toggled(bool)), this, SLOT(setShapeEditing(bool)));
	p->bClearShape->setVisible(hasShape());
	connect(this, SIGNAL(notifyHasShape(bool)), p->bClearShape, SLOT(setVisible(bool)));
	connect(p->bClearShape, SIGNAL(clicked()), this, SLOT(clearShape()));
	
	return p;
}

void TextBoxContent::syncFromModelItem(AbstractVisualItem *model)
{
        m_dontSyncToModel = true;
	if(!modelItem())
		setModelItem(model);
	
	static int x = 0;
	x++;
	//qDebug() << x<<": TextBoxContent::syncFromModelItem() mark";
	QFont font;
	TextItem * textModel = dynamic_cast<TextItem*>(model);
	
	if(textModel->text().indexOf('<') < 0)
	{
		m_text->setPlainText(textModel->text());
		textModel->setText(m_text->toHtml());
	}
	setHtml(textModel->text());
	
	font.setFamily(textModel->fontFamily());
	font.setPointSize((int)textModel->fontSize());
	m_text->setDefaultFont(font);
	
	
	bool shapeEnabled = textModel->shapeEnabled();
	if (shapeEnabled)
	{
		QList<QPointF> points;
		QStringList strPoint;
		points << textModel->shapePoint1();
		points << textModel->shapePoint2();
		points << textModel->shapePoint3();
		points << textModel->shapePoint4();
		m_shapeEditor->setControlPoints(points);
	}
	
	setXTextAlign(textModel->xTextAlign());
	setYTextAlign(textModel->yTextAlign());
	
	AbstractContent::syncFromModelItem(model);
	
        m_dontSyncToModel = false;
}

AbstractVisualItem * TextBoxContent::syncToModelItem(AbstractVisualItem *model)
{
	TextItem * textModel = dynamic_cast<TextItem*>(AbstractContent::syncToModelItem(model));
	
	setModelItemIsChanging(true);
	
	if(!textModel)
	{
		setModelItemIsChanging(false);
                //qDebug("TextBoxContent::syncToModelItem: textModel is null, cannot sync\n");
		return 0;
	}
        //qDebug("TextBoxContent:syncToModelItem: Syncing to model! Yay!");
	textModel->setText(m_text->toHtml());
	textModel->setFontFamily(m_text->defaultFont().family());
	textModel->setFontSize(m_text->defaultFont().pointSize());
	textModel->setShapeEnabled(hasShape());
	if (hasShape()) 
	{
		QList<QPointF> cp = m_shapeEditor->controlPoints();
		textModel->setShapePoint1(cp[0]);
		textModel->setShapePoint2(cp[2]);
		textModel->setShapePoint3(cp[3]);
		textModel->setShapePoint4(cp[4]);
	}
	
	textModel->setXTextAlign(xTextAlign());
	textModel->setYTextAlign(yTextAlign());
	
	setModelItemIsChanging(false);
	
	return model;
}

QPixmap TextBoxContent::renderContent(const QSize & size, Qt::AspectRatioMode /*ratio*/) const
{
	// get the base empty pixmap
	QSize textSize = boundingRect().size().toSize();
	const float w = size.width(),
			h = size.height(),
			tw = textSize.width(),
			th = textSize.height();
	if (w < 2 || h < 2 || tw < 2 || th < 2)
		return QPixmap();
	
	// draw text (centered, maximized keeping aspect ratio)
	float scale = qMin(w / (tw + 16), h / (th + 16));
	QPixmap pix(size);
	pix.fill(Qt::transparent);
	QPainter pixPainter(&pix);
	pixPainter.translate((w - (int)((float)tw * scale)) / 2, (h - (int)((float)th * scale)) / 2);
	pixPainter.scale(scale, scale);
	m_text->drawContents(&pixPainter);
	pixPainter.end();
	return pix;
}

int TextBoxContent::contentHeightForWidth(int width) const
{
	// if no text size is available, use default
	if (m_textRect.width() < 1 || m_textRect.height() < 1)
		return AbstractContent::contentHeightForWidth(width);
		
	
	int textWidth = width;

	//QRegExp rxWhite("\\s+");
	QRegExp rxWhite("\\b");
	QRegExp rxNonAZ("[A-Za-z0-9\\s]");
	
	// 2. LAYOUT TEXT. find out Block rects and Document rect
	
	QPoint cursor(0,0);
	
	QRect cursorRect(cursor.x(),cursor.y(),0,0);
	
// 	int blockCount = 0;
	for (QTextBlock tb = m_text->begin(); tb.isValid(); tb = tb.next()) 
	{
		if (!tb.isVisible())
			continue;
		
		QFontMetrics metrics(tb.charFormat().font());
		int textHeight = 1; // wil be set later
					
		QTextBlockFormat tbFormat = tb.blockFormat();
		//int xZero = -(int)tbFormat.leftMargin();
		int xZero = 0;
		cursor.setX(xZero);
		
// 		int blockTop = cursor.y();
		
		for (QTextBlock::iterator tbIt = tb.begin(); !(tbIt.atEnd()); ++tbIt) 
		{
			QTextFragment frag = tbIt.fragment();
			if (!frag.isValid())
				continue;
		
			QString text = frag.text();
// 			if (text.trimmed().isEmpty())
// 				continue;
			QFontMetrics metrics(frag.charFormat().font());
		
			QRect textRect = metrics.boundingRect(text+" ");
			textHeight = textRect.height();
			
			if(cursor.x() + textRect.width() > textWidth)
			{
				QStringList words = text.split(rxWhite);
				// start from end of frag and move backwards
				
				QString tmp;
				QString next;
				for(int i=0;i<words.size();i++)
				{
					tmp += words.at(i);
					
					next = "";
					if(i+1 < words.size())
						next = words.at(i+1);
							
					QRect tmpRect = metrics.boundingRect(tmp+" ");
					if(cursor.x() + tmpRect.width() > textWidth)
					{
						cursor.setX(xZero);
						cursor.setY(cursor.y() + textHeight);
					}
					
					QRect tmpRect2 = metrics.boundingRect(tmp+next+" ");
					if(cursor.x() + tmpRect2.width() >= textWidth)
					{
						// if the next "word" contains a non-A-Z (or 0-9) character (like a period), dont leave it dangling
						if(next != "")
						{
							int pos = rxNonAZ.indexIn(next);
							//qDebug() << "updateTextConstraints(): WrapAlpha: 1: Dangle Check: next:"<<next<<", pos:"<<pos;
							if(pos < 0)
							{
								tmp += next;
								i++;
							}
						}
						
						tmp =  cursor.x() == 0 ? trimLeft(tmp) : tmp;
						if(tmp.trimmed().isEmpty())
							continue;
							
						
						TextLineSpec ts(frag,QRect(cursor - tmpRect.topLeft(), tmpRect.size()), tmp);
						//m_lineSpecs.append(ts);
						
						cursorRect |= ts.rect;
						cursor.setX(xZero);
						cursor.setY(cursor.y() + ts.rect.height());
						
						tmp = "";
					}
				}
				
				if(tmp != "")
				{
					QRect tmpRect = metrics.boundingRect(tmp+" ");
					
					TextLineSpec ts(frag,QRect(cursor - tmpRect.topLeft(), tmpRect.size()),cursor.x() == 0 ? trimLeft(tmp) : tmp);
					//m_lineSpecs.append(ts);
					
					cursorRect |= ts.rect;
					cursor.setX(cursor.x() + ts.rect.width()); //ts.rect.right());
				}
			}
			else
			{
				TextLineSpec ts(frag,QRect(cursor - textRect.topLeft(), textRect.size()),cursor.x() == 0 ? trimLeft(text) : text);
				//m_lineSpecs.append(ts);
				
				cursorRect |= ts.rect;
				cursor.setX(cursor.x() + ts.rect.width());
			}
		}
		
		
		if(cursor.x() > cursorRect.right())
			cursorRect.setRight(cursor.x());
		if(cursor.y() > cursorRect.bottom())
			cursorRect.setBottom(cursor.y());
			
		cursor.setY(cursor.y() + textHeight);

	}
	
	return cursorRect.height();
}

void TextBoxContent::selectionChanged(bool selected)
{
	// hide shape editing controls
	if (!selected && isShapeEditing())
		setShapeEditing(false);
}

void TextBoxContent::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
	emit backgroundMe();
	QGraphicsItem::mouseDoubleClickEvent(event);
}

void TextBoxContent::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	// paint parent
	AbstractContent::paint(painter, option, widget);
	
	painter->save();
	QPen p = modelItem()->outlinePen();
	p.setJoinStyle(Qt::MiterJoin);
	if(sceneContextHint() == MyGraphicsScene::Preview)
	{
		QTransform tx = painter->transform();
		qreal scale = qMax(tx.m11(),tx.m22());
		if(scale>0)
			p.setWidthF(1/scale * p.widthF() * .5);
	}
		
	//TODO should we clip to the rect or FORCE resize the rect? probably clip...
	painter->setClipRect(contentsRect());
	painter->translate(contentsRect().topLeft() + QPoint(p.width(),p.width()));
	
	// stroke the path (create fillable path from text points)
// 	QPainterPathStroker s;
// 	QPainterPath path = s.createStroke(m_textPath);
	// Doesn't look right, so I disabled it
	
	// draw shadow
	if(modelItem()->shadowEnabled())
	{
		painter->save();
		
		// apply the "mask" of the text to be painted on top
		painter->setClipPath(m_shadowClipPath);
		
		// draw the text - but only the parts not under the "top" text should be painted
		double x = modelItem()->shadowOffsetX();
		double y = modelItem()->shadowOffsetY();
		painter->translate(x,y);
		painter->setPen(Qt::NoPen);	
		painter->setBrush(modelItem()->shadowBrush());
		painter->drawPath(m_textPath);
		painter->translate(-x,-y);
		
		// reset clipping rect
		painter->restore();
	}
	
	// draw text
	painter->setPen(p);
	painter->setBrush(modelItem()->fillBrush());
	painter->drawPath(m_textPath);
	painter->restore();
}

void TextBoxContent::updateShadowClipPath()
{
	QPainterPath clipPath;
	clipPath.addRect(QRectF(0,0,contentsRect().width(),contentsRect().height()));
	
	m_shadowClipPath = clipPath.subtracted(m_textPath);
}

QPainterPath TextBoxContent::shapePath() const
{
	return m_shapePath;
}

void TextBoxContent::setShapePath(const QPainterPath & path)
{
// 	if (path == m_shapePath)
// 		return;
// 	
// 	// invalidate rectangles
// 	m_textRect = QRect();
// 	m_shapeRect = QRect();
// 	
// 	// set new path
// 	m_shapePath = path;
// 	
// 	// regenerate text layouting
// 	updateTextConstraints();
}


void TextBoxContent::updateTextConstraints(int w)
{
	if(!m_text)
		return;
		
	int textWidth = w;
	if(w < 0)
		textWidth = contentsRect().width();
	
	if(DEBUG_LAYOUT)
		qDebug("updateTextConstraints() BEGIN (width: %d)",textWidth);
		
		
		
	/*	
		
		
	QTextOption t;
        t.setWrapMode(QTextOption::WordWrap);
        //m_text->documentLayout()->setTextOption(t);
        
        QPen p = modelItem()->outlinePen();
	p.setJoinStyle(Qt::MiterJoin);
        
        for (QTextBlock tb = m_text->begin(); tb.isValid(); tb = tb.next())
        {
                tb.layout()->setTextOption(t);
                
                for (QTextBlock::iterator tbIt = tb.begin(); !(tbIt.atEnd()); ++tbIt) 
		{
			QTextFragment frag = tbIt.fragment();
			if (!frag.isValid())
				continue;
			
        }
        
        m_text->setTextWidth(contentsRect().width());*/
        
//      QSizeF sz = m_text->documentLayout()->documentSize();
//      m_textRect = QRect(QPoint(0,0),QSize((int)sz.width(),(int)sz.height()));


		
	
	QRegExp rxWhite("\\b");
	QRegExp rxNonAZ("[A-Za-z0-9\\s]");
	
	QPoint cursor(0,0);
	//m_lineSpecs.clear();
	
	QRect cursorRect(cursor.x(),cursor.y(),0,0);

#ifdef Q_OS_UNIX
        bool add_space = true;
#else
        bool add_space = false;
#endif
	
	QPainterPath textPath;
	
	for (QTextBlock tb = m_text->begin(); tb.isValid(); tb = tb.next()) 
	{
		if (!tb.isVisible())
			continue;
		
		int textHeight = 1;
					
		QTextBlockFormat tbFormat = tb.blockFormat();
		//int xZero = -(int)tbFormat.leftMargin();
		int xZero = 0;
		cursor.setX(xZero);
		
		Qt::Alignment blockAlign = (Qt::Alignment)tbFormat.intProperty(QTextFormat::BlockAlignment);
		
		
// 		if(m_currentLine.size()>0) 
// 			m_currentLine.clear();
	
		if(DEBUG_LAYOUT)
			qDebug("updateTextConstraints(): align debug: START of block, blockAlign: %d",(int)blockAlign);
		//blockAlign = Qt::AlignHCenter;
		
		// 2.1.A. calc the Block size uniting Fragments bounding rects
		QRect blockRect(0, 0, 0, 0);
		for (QTextBlock::iterator tbIt = tb.begin(); !(tbIt.atEnd()); ++tbIt) 
		{
			QTextFragment frag = tbIt.fragment();
			if (!frag.isValid())
				continue;
		
			QString text = frag.text();
			
			if(DEBUG_LAYOUT)
				qDebug() << "updateTextConstraints(): 2.1.A.1: frag.text():"<<frag.text();
		
			QFontMetrics metrics(frag.charFormat().font());
				
			// TODO: implement superscript / subscript (it's in charFormat's alignment)
			// it must be implemented in paint too
		
                        QRect textRect = metrics.boundingRect(text+(add_space ? " " : ""));
			if(DEBUG_LAYOUT)
				qDebug() << "updateTextConstraints(): 2.1.A.2: metrics.boundingRect(...):"<<textRect;
			
			// for use in generating the end-of-block break
			textHeight = textRect.height();
			
			if(cursor.x() + textRect.width() > textWidth)
			{
				QStringList words = text.split(rxWhite);
				
				QString tmp;
				QString next;
				for(int i=0;i<words.size();i++)
				{
					tmp += words.at(i);
					
					// for non-letter (or #) dangle detection (e.g. dont dangle a period, etc) 
					next = "";
					if(i+1 < words.size())
						next = words.at(i+1);
							
					// WHY does Qt metrics.boundingRect() ignore spaces at the end of the line? This is probably my fault - but its confusing.
					// You see, if tmp ends in a space, ex. "test ", Qt *seems* to only give me the rect for "test", not "text ". However,
					// if we add a *second* space, Qt drops the 'fake' space instaed, but includes anys spaces in the 'real' text. Wierd? Or am I
					// screwing up somewhere? Anyone? - JB 20090915
					
					// UPDATE: It appears that this spacing 'bug' is only on linux - I've only tested on
					// FC8 so far and windows XP, and XP works fine, but linux needs the extra space. - JB 20090916
					
					// UPDATE: I've reported the "bug" on the qt-interest mailing list - we'll see what turns up from that - if anything. - JB 20090916
					
					QRect tmpRect = metrics.boundingRect(tmp+(add_space ? " " : ""));
					
					// wrap cursor if the current word in (tmp) wont fit - e.g. the start of the frag is about the only time this would hit
					if(!tmp.isEmpty() && cursorRect.width() > 0 && cursorRect.height() > 0 && cursor.x() > 0 && cursor.x() + tmpRect.width() > textWidth)
					{
						applyTextXAlign(blockAlign);
						addLineToPath(&textPath);
						cursor.setX(xZero);
						cursor.setY(cursor.y() + textHeight);
						if(DEBUG_LAYOUT)
							qDebug() << "updateTextConstraints(): WrapAlpha: 1.2: Text wouldn't fit in current line, wrapping, cursor now at:"<<cursor<<", cursorRect:"<<cursorRect;
					}
					
					// visualTmp is the string to render. Only difference is if this string is at the start of the line,
					// we trip any spaces at the start of the string. However, if this word is NOT at the start of the line -
					// we dont trim - e.x. a frag that starts it the middle of the line.
					QString visualTmp = cursor.x() == 0 ? trimLeft(tmp) : tmp;
					
                                        QRect tmpRect2 = metrics.boundingRect(tmp+next+(add_space ? " " : ""));
					if(visualTmp != "" && cursor.x() + tmpRect2.width() >= textWidth)
					{
						// if the next "word" contains a non-A-Z (or 0-9) character (like a period), dont leave it dangling
						if(next != "")
						{
							int pos = rxNonAZ.indexIn(next);
							if(DEBUG_LAYOUT)
								qDebug() << "updateTextConstraints(): WrapAlpha: 1: Dangle Check: next:"<<next<<", pos:"<<pos;
							if(pos < 0)
							{
								visualTmp += next;
								i++;
							}
						}
						
                                                // TextLineSpec holds the position of this subset of the frag on screen.
                                                // Internally, it also converts the text (visualTmp) to a QPainterPath
                                                // for quicker rendering.
                                                TextLineSpec *ts = new TextLineSpec(frag,QRect(cursor - tmpRect.topLeft(), tmpRect.size()), visualTmp);
                                                //m_lineSpecs.append(ts);
						m_currentLine << ts;
						
                                                // Unify the linespec rect with the cursor to create the bounding rect
                                                // for this layout
						cursorRect |= ts->rect;
						
                                                // wrap cursor to next line
						applyTextXAlign(blockAlign);
						addLineToPath(&textPath);
						cursor.setX(xZero);
						cursor.setY(cursor.y() + ts->rect.height());
						
						if(DEBUG_LAYOUT)
							qDebug() << "updateTextConstraints(): WrapAlpha: 1: Partial frag used, text:"<<visualTmp<<", ts->rect:"<<ts->rect<<", cursor now at:"<<cursor<<", cursorRect:"<<cursorRect;
						
						tmp = "";
					}
				}
				
                                // Catch anything leftover from the block above - e.g. the remainder of the frag
                                // that didnt hit the end of the line - add it to the list of linespecs, but
                                // dont wrap the cursor.
				if(tmp != "")
				{
					QString visualTmp = cursor.x() == 0 ? trimLeft(tmp) : tmp;
					if(visualTmp != "")
					{
						QRect tmpRect = metrics.boundingRect(tmp+(add_space ? " " : ""));
						
						TextLineSpec *ts = new TextLineSpec(frag,QRect(cursor - tmpRect.topLeft(), tmpRect.size()),visualTmp);
						//m_lineSpecs.append(ts);
						m_currentLine << ts;
						
						cursorRect |= ts->rect;
	
						// You'll notice here, among other places,
						// we set the cursor X to cursor.x+ts->rect.width,
						// instead of using ts->rect.right - this is because
						// ts->rect.right = ts->rect.x + ts->rect.width, which
						// is not always the same as cursor.x + ts->rect.width,
						// since the text can overlap previous characters.
						cursor.setX(cursor.x() + ts->rect.width());
						
						if(DEBUG_LAYOUT)
							qDebug() << "updateTextConstraints(): WrapAlpha: 1.1: Leftover partial frag, text:"<<visualTmp<<", ts->rect:"<<ts->rect<<", cursor now at:"<<cursor<<", cursorRect:"<<cursorRect;
					}
						
				}
			}
			else
			{
                                // This block handles non-broken frags - just add the entire fragment to the line
                                // and continue on (not wrapping cursor)
                                TextLineSpec *ts = new TextLineSpec(frag,QRect(cursor - textRect.topLeft(), textRect.size()),cursor.x() == 0 ? trimLeft(text) : text);
                                //m_lineSpecs.append(ts);
                                m_currentLine << ts;
				
				cursorRect |= ts->rect;
				cursor.setX(cursor.x() + ts->rect.width());
				if(DEBUG_LAYOUT)
					qDebug() << "updateTextConstraints(): WrapAlpha: 2: No break needed, full frag fits, text:"<<text<<", ts->rect:"<<ts->rect<<", cursor now at:"<<cursor<<", cursorRect:"<<cursorRect;
			}
		}
		
		
		// 2.1.B. calc the Block size of blank lines
		if (tb.begin() == tb.end()) 
		{
			QFontMetrics metrics(tb.charFormat().font());
			int textHeight = metrics.height();
			
			QRect rect(0,0,1,textHeight);
			
			cursorRect |= rect;
						
			// wrap cursor to next line
			//applyTextXAlign(blockAlign);
			//addLineToPath(&textPath);
			cursor.setX(xZero);
			cursor.setY(cursor.y() + textHeight);
			
			//cursor.setX(1);
			//cursor.setY(cursor.y() + textHeight); // + (cursorRect.bottom() - blockTop) + 1);
		
			if(DEBUG_LAYOUT)
				qDebug() << "updateTextConstraints(): 2.1.B: empty line, rect="<<rect;
		}
		
		
                // unify the rect again
		if(cursor.x() > cursorRect.right())
			cursorRect.setRight(cursor.x());
		if(cursor.y() > cursorRect.bottom())
			cursorRect.setBottom(cursor.y());
			
		if(DEBUG_LAYOUT)
			qDebug("updateTextConstraints(): align debug: End of block, mark1"); 
                // We wrap the cursor to the next line AFTER
                // unifying the rect again because if this is the last block in the document,
                // we dont want and empty line at the end when the bounding rect is drawn.
                applyTextXAlign(blockAlign);
                addLineToPath(&textPath);
                cursor.setX(0);
		cursor.setY(cursor.y() + textHeight);
		
		if(DEBUG_LAYOUT)
			qDebug("updateTextConstraints(): align debug: End of block, mark2");
	}
	
// 	m_textRect.adjust(-m_textMargin, -m_textMargin, m_textMargin, m_textMargin);
	
	//qDebug() << "updateTextConstraints(): m_textMargin="<<m_textMargin<<" m_textRect="<<m_textRect;
	if(DEBUG_LAYOUT)
		qDebug() << "updateTextConstraints(): WrapAlpha: cursorRect:"<<cursorRect<<", contentsRect:"<<contentsRect();
	
	m_textRect = cursorRect;//.adjusted(-1,-1,+1,+1);
	m_textPath = textPath;
	
        // Adjust the bounding rect *height* to our document wrapped height, but leave
        // the width alone.
	bool changed = false;
	QRect newRect = contentsRect();
	if(m_textRect.height() != newRect.height())
	{
		newRect.setHeight(m_textRect.height());
		changed = true;
	}
// 	if(m_textRect.width() > newRect.width())
// 	{
// 		newRect.setWidth(m_textRect.width());
// 		changed = true;
// 	}
	if(changed)
	{
		AbstractContent::resizeContents(newRect);
	}
	
	updateShadowClipPath();
}

void TextBoxContent::applyTextXAlign(Qt::Alignment x)
{
	//Qt::Alignment x = m_xTextAlign;
	//Qt::AlignRight; //
	
	// already aligned left
	if(x == Qt::AlignLeft || (int)x == 0)
		return;
	
	if(DEBUG_LAYOUT)
		qDebug() << "applyTextAlign(): alignment:"<<x;
	// either center or right align needs total width
	int totalLineWidth = 0;
	foreach(TextLineSpec * ts, m_currentLine)
	{
		totalLineWidth += ts->rect.width();
	}
	
	if(DEBUG_LAYOUT)
		qDebug() << "applyTextAlign(): totalLineWidth:"<<totalLineWidth;
	
	// Aligning center or right is just a matter of where we start the line
	int availWidth = contentsRect().width();
	int startingPoint = 
		x == Qt::AlignRight || (int)x == 18 ? 
		                      availWidth - totalLineWidth :
				      availWidth / 2 - totalLineWidth / 2;
	
	if(DEBUG_LAYOUT)
		qDebug() << "applyTextAlign(): startingPoint:"<<startingPoint<<" (availWidth:"<<availWidth<<")";
	// Go thru the frags on the line and put them in their new spot
	int cursor = startingPoint;
	int w;
	foreach(TextLineSpec * ts, m_currentLine)
	{
		
		w = ts->rect.width();
		ts->rect.setLeft(cursor);
		
		if(DEBUG_LAYOUT)
			qDebug() << "applyTextAlign(): moved"<<ts->text<<" to X"<<cursor<<", ts->rect:"<<ts->rect;
		
		//ts->rect.setX(cursor);
//		qDebug() << "applyTextAlign(): DEBUG MOVE: ts->rect:"<<ts->rect;
		cursor += w; //ts->rect.width();
	}
	
}

void TextBoxContent::addLineToPath(QPainterPath *path)
{
	foreach(TextLineSpec * ts, m_currentLine)
	{
		if(DEBUG_LAYOUT)
			qDebug() << "addLineToPath() adding"<<ts->text<<" at "<<ts->rect.topLeft();
		path->addText(ts->rect.topLeft(),ts->frag.charFormat().font(),ts->text);
	}
	
	qDeleteAll(m_currentLine);
	m_currentLine.clear();
}
						

void TextBoxContent::delayContentsResized()
{
	QTimer::singleShot(0,this,SLOT(contentsResized()));
}

void TextBoxContent::contentsResized()
{
	//qDebug("contentsResized hit");
 	//qDebug()<<"TextBoxContent::contentsResized()";
 	updateTextConstraints();
 	update();
// 	QRect un = rect | m_textRect;
// 	AbstractContent::resizeContents(un,keepRatio);
//	AbstractContent::resizeContents(rect,keepRatio);
}

void TextBoxContent::resizeContents(const QRect & rect, bool keepRatio)
{
	updateTextConstraints(rect.width());
	AbstractContent::resizeContents(rect,keepRatio);
	update();
}

void TextBoxContent::updateCache()
{
    /*
    m_cachePixmap = QPixmap(contentsRect().size());
    m_cachePixmap.fill(QColor(0, 0, 0, 0));
    QPainter painter(&m_cachePixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    ...
    */
}
