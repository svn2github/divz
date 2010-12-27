#include "GLSvgDrawable.h"

#include <QSvgRenderer>

GLSvgDrawable::GLSvgDrawable(QString file, QObject *parent)
	: GLImageDrawable("",parent)
{
	m_renderer = new QSvgRenderer(this);
	connect(m_renderer, SIGNAL(repaintNeeded()), this, SLOT(renderSvg()));
	
	setImage(QImage("dot.gif"));
	
	connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(renderSvg()));
	m_updateTimer.setInterval(250);
	m_updateTimer.setSingleShot(true);
	
	// Dont pass file to the QIMageDrawable construct, even tho it would
	// call setImageFile, because at that time we didnt have m_svgRenderer created OR initalized yet!
	if(!file.isEmpty())
		setImageFile(file);
}

GLSvgDrawable::~GLSvgDrawable()
{
	if(m_renderer)
	{
		delete m_renderer;
		m_renderer = 0;
	}
}

bool GLSvgDrawable::setImageFile(const QString& file)
{
	if(file.isEmpty())
		return false;
	
	QFileInfo fileInfo(file);
	if(!fileInfo.exists())
	{
		qDebug() << "GLSvgDrawable::setImageFile: "<<file<<" does not exist!";
		return false;
	}


	m_renderer->load(file);
	qDebug() << "GLSvgDrawable::setImageFile: Loaded SVG file:"<<file<<", is valid?"<<m_renderer->isValid()<<", is animated?"<<m_renderer->animated();
	
	internalSetFilename(file);
	renderSvg();
}

void GLSvgDrawable::renderSvg()
{
	if(!m_renderer->isValid())
	{
		qDebug() << "GLSvgDrawable::renderSvg: No valid SVG file loaded, unable to render.";
		return;
	}
	
	QSize imageSize = m_renderer->viewBox().size();
	imageSize.scale(rect().size().toSize(), Qt::KeepAspectRatio);
	
	QImage image(imageSize, QImage::Format_ARGB32);
	QPainter painter(&image);
	m_renderer->render(&painter);//, m_renderer->viewBoxF());
	qDebug() << "GLSvgDrawable::renderSvg: Rendered image, size:"<<image.size();
	setImage(image); 
}

void GLSvgDrawable::drawableResized(const QSizeF& newSize)
{
	if(m_updateTimer.isActive())
		m_updateTimer.stop();
	m_updateTimer.start();	
	GLVideoDrawable::drawableResized(newSize);
}



/*
void GLSvgDrawable::loadPropsFromMap(const QVariantMap& map, bool onlyApplyIfChanged)
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

QVariantMap GLSvgDrawable::propsToMap()
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
}*/

