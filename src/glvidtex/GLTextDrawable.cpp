#include "GLTextDrawable.h"

#include "RichTextRenderer.h"

#define DEFAULT_CLOCK_FORMAT "yyyy-MM-dd h:mm:ss ap"

GLTextDrawable::GLTextDrawable(QString text, QObject *parent)
	: GLImageDrawable("",parent)
	, m_isClock(false)
	, m_clockFormat(DEFAULT_CLOCK_FORMAT)
	, m_lockSetPlainText(false)
{
	QDateTime now = QDateTime::currentDateTime();
	m_targetTime = QDateTime(QDate(now.date().year()+1, 12, 25), QTime(0, 0));
	
	m_isCountdown = false;
	

	m_renderer = new RichTextRenderer();
	connect(m_renderer, SIGNAL(textRendered(QImage)), this, SLOT(setImage(const QImage&)));
	
	m_renderer->setTextWidth(1000); // just a guess
	setImage(QImage("dot.gif"));
	
	if(!text.isEmpty())
		setText(text);
	
	//QTimer::singleShot(1500, this, SLOT(testXfade()));
	
	foreach(CornerItem *corner, m_cornerItems)
		corner->setDefaultLeftOp(CornerItem::Scale);
		
	//setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
		
	connect(&m_countdownTimer, SIGNAL(timeout()), this, SLOT(countdownTick()));
	m_countdownTimer.setInterval(250);

	connect(&m_clockTimer, SIGNAL(timeout()), this, SLOT(clockTick()));
	m_clockTimer.setInterval(250);

}
GLTextDrawable::~GLTextDrawable()
{
	if(m_renderer)
	{
		delete m_renderer;
		m_renderer = 0;
	}
}
	
void GLTextDrawable::testXfade()
{
	qDebug() << "GLTextDrawable::testXfade(): loading text #2";
	setText("Friday 2010-11-26");
}

void GLTextDrawable::setIsCountdown(bool flag)
{
	m_isCountdown = flag;
	if(m_countdownTimer.isActive() && !flag)
		m_countdownTimer.stop();
	
	if(!m_countdownTimer.isActive() && flag)
		m_countdownTimer.start();
	
	if(flag)
	{
		setXFadeEnabled(false);
		countdownTick();
	}
	else
		setXFadeEnabled(true);
}

void GLTextDrawable::setTargetDateTime(const QDateTime& date)
{
	m_targetTime = date;
}


QString GLTextDrawable::formatTime(double time)
{
	double hour = time/60/60;
	double min = (hour - (int)(hour)) * 60;
	double sec = (min  - (int)(min)) * 60;
	//double ms  = (sec  - (int)(sec)) * 60;
	return  QString::number((int)hour) + ":" +
		(min<10? "0":"") + QString::number((int)min) + ":" +
		(sec<10? "0":"") + QString::number((int)(sec+.5));// + "." +
		//(ms <10? "0":"") + QString::number((int)ms );

}

void GLTextDrawable::countdownTick()
{
	QDateTime now = QDateTime::currentDateTime();
	//QDateTime xmas(QDate(now.date().year(), 12, 25), QTime(0, 0));
	//qDebug("There are %d seconds to Christmas", now.secsTo(xmas));
	int secsTo = now.secsTo(m_targetTime);

	//double time = ((double)m_currentTimeLength) - ((double)m_elapsedTime.elapsed())/1000;
	//m_timeLabel->setText(QString("<font color='%1'>%2</font>").arg(time <= 3 ? "red" : "black").arg(formatTime(secsTo)));
	
	
	//setText(formatTime(secsTo));
	QString newText = formatTime(secsTo);
	
	setPlainText(newText);
	
	// Force renderer to render text so it doesn't delay past another timer tick, thereby creating a gap in the countdown onscreen
	setXFadeEnabled(false);
	//qDebug() << "GLTextDrawable::countdownTick: "<<now<<secsTo<<newText;
	//m_renderer->renderText();
}

void GLTextDrawable::setIsClock(bool flag)
{
	m_isClock = flag;
	if(m_clockTimer.isActive() && !flag)
		m_clockTimer.stop();
	
	if(!m_clockTimer.isActive() && flag)
		m_clockTimer.start();
	
	if(flag)
	{
		setXFadeEnabled(false);
		clockTick();
	}
	else
		setXFadeEnabled(true);
}

void GLTextDrawable::setClockFormat(const QString& format)
{
	m_clockFormat = format;
	if(m_clockFormat.isEmpty())
		m_clockFormat = DEFAULT_CLOCK_FORMAT;
}

void GLTextDrawable::clockTick()
{
	QDateTime now = QDateTime::currentDateTime();
	QString newText = now.toString(clockFormat());
	//qDebug() << "GLTextDrawable::clockTick: now:"<<now<<", format:"<<clockFormat()<<", newText:"<<newText;
	
	setPlainText(newText);
	
	setXFadeEnabled(false);
	//m_renderer->renderText();
}

void GLTextDrawable::setText(const QString& text)
{
	m_text = text;
	//qDebug() << "GLTextDrawable::setText(): text:"<<text;
	bool lock = false;
	
	if(m_cachedImageText == text && 
	  !m_cachedImage.isNull())
	{
		qDebug() << "GLTextDrawable::setText: Cached image matches text, not re-rendering.";
		lock = m_renderer->lockUpdates(true);
		setImage(m_cachedImage);
	}
		
	m_renderer->setHtml(text);
	if(!Qt::mightBeRichText(text))
		changeFontSize(40);
		
	m_renderer->lockUpdates(lock);
	
	emit textChanged(text);
	
	emit plainTextChanged(plainText());
}

QString GLTextDrawable::plainText()
{
	return htmlToPlainText(m_text);
}

/* static */
QString GLTextDrawable::htmlToPlainText(const QString& text)
{
	QString plain = text;
	//plain = plain.replace( QRegExp("(<[bB][rR]>|\n)"), " / ");
	plain = plain.replace( QRegExp("<style[^>]*>.*</style>", Qt::CaseInsensitive), "" );
	plain = plain.replace( QRegExp("<[^>]*>"), "" );
	plain = plain.replace( QRegExp("(^\\s+)"), "" );
	return plain;
}

void GLTextDrawable::setPlainText(const QString& text, bool replaceNewlineSlash)
{
	if(m_lockSetPlainText)
		return;
	m_lockSetPlainText = true;
	
	QTextDocument doc;
	QString origText = m_text;
	if (Qt::mightBeRichText(origText))
		doc.setHtml(origText);
	else
		doc.setPlainText(origText);

	QTextCursor cursor(&doc);
	cursor.select(QTextCursor::Document);

	QTextCharFormat format = cursor.charFormat();
	if(!format.isValid())
		format = cursor.blockCharFormat();
	//qDebug() << "Format at cursor: "<<format.fontPointSize()<<", "<<format.font()<<", "<<format.fontItalic()<<", "<<format.font().rawName();
	QString newText = text;
	//if(replaceNewlineSlash)
	//	newText = newText.replace(QRegExp("\\s/\\s")," \n ");

	if(format.fontPointSize() > 0)
	{
		cursor.insertText(newText);

		// doesnt seem to be needed
		//cursor.mergeCharFormat(format);

		setText(doc.toHtml());
	}
	else
	{
		setText(newText);
	}	
	
	m_lockSetPlainText = false;
}

void GLTextDrawable::drawableResized(const QSizeF& newSize)
{
	if(m_renderer->textWidth() != (int)newSize.width())
		m_renderer->setTextWidth((int)newSize.width());
	GLVideoDrawable::drawableResized(newSize);
}

QSize GLTextDrawable::findNaturalSize(int atWidth)
{
	return m_renderer->findNaturalSize(atWidth);
}

double GLTextDrawable::findFontSize()
{
	return m_renderer->findFontSize();
}

void GLTextDrawable::changeFontSize(double size)
{
	m_renderer->changeFontSize(size);
	m_text = m_renderer->html();
	emit textChanged(m_text);
}


void GLTextDrawable::updateRects(bool secondSource)
{
	QRectF sourceRect = m_frame->rect();
	
	updateTextureOffsets();
	
	// Honor croping since the rendering will honor croping - but not really needed for text.
	QRectF adjustedSource = sourceRect.adjusted(
		m_displayOpts.cropTopLeft.x(),
		m_displayOpts.cropTopLeft.y(),
		m_displayOpts.cropBottomRight.x(),
		m_displayOpts.cropBottomRight.y());
		
	QRectF targetRect = QRectF(rect().topLeft(),adjustedSource.size()); 
	
	if(!secondSource)
	{
		//qDebug() << "GLTextDrawable::updateRects: source:"<<sourceRect<<", target:"<<targetRect;
		m_sourceRect = sourceRect;
		m_targetRect = targetRect;
	}
	else
	{
		m_sourceRect2 = sourceRect;
		m_targetRect2 = targetRect;
	}
}

void GLTextDrawable::loadPropsFromMap(const QVariantMap& map, bool onlyApplyIfChanged)
{
	
	QByteArray bytes = map["text_image"].toByteArray();
	QImage image;
	image.loadFromData(bytes);
	//qDebug() << "GLSceneLayout::fromByteArray(): image size:"<<image.size()<<", isnull:"<<image.isNull();
	
	if(!image.isNull())
		setImage(image);
	m_cachedImageText = map["text_image_alt"].toString();
	
	GLDrawable::loadPropsFromMap(map);
}

QVariantMap GLTextDrawable::propsToMap()
{
	QVariantMap map = GLDrawable::propsToMap();
	
	// Save the image to the map for sending a cached render over the network so the player
	// can cheat and use this image that was rendered by the director as opposed to re-rendering
	// the same text
	QByteArray bytes;
	QBuffer buffer(&bytes);
	buffer.open(QIODevice::WriteOnly);
	image().save(&buffer, "PNG"); // writes pixmap into bytes in PNG format
	buffer.close();
	map["text_image"] = bytes;
	map["text_image_alt"] = m_text;
	
	return map;
}

