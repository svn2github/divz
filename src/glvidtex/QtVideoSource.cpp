#include "QtVideoSource.h"

#include <QDebug>
#include <QtGui>
#include <qabstractvideosurface.h>
#include <qvideosurfaceformat.h>
#include <qmediaservice.h>
#include <qmediaplaylist.h>
#include <qmediaservice.h>
#include <qvideorenderercontrol.h>

VideoSurfaceAdapter::VideoSurfaceAdapter(QtVideoSource *e, QObject *parent)
	: QAbstractVideoSurface(parent)
	, emitter(e)
	, m_pixelFormat(QVideoFrame::Format_Invalid)
	, imageFormat(QImage::Format_Invalid)
{
}

QList<QVideoFrame::PixelFormat> VideoSurfaceAdapter::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
	if (handleType == QAbstractVideoBuffer::NoHandle) 
	{
		return QList<QVideoFrame::PixelFormat>()
			<< QVideoFrame::Format_RGB32
			<< QVideoFrame::Format_ARGB32
			<< QVideoFrame::Format_ARGB32_Premultiplied
			<< QVideoFrame::Format_RGB565
			<< QVideoFrame::Format_RGB555;
	} 
	else 
	{
		return QList<QVideoFrame::PixelFormat>();
	}
}

bool VideoSurfaceAdapter::isFormatSupported(
        const QVideoSurfaceFormat &format, QVideoSurfaceFormat *similar) const
{
	Q_UNUSED(similar);
	
	const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
	const QSize size = format.frameSize();
	
	qDebug()<< "VideoSurfaceAdapter::isFormatSupported: got imageFormat:"<<imageFormat<<" for pixelFormat:"<<format.pixelFormat();
	
	return imageFormat != QImage::Format_Invalid
		&& !size.isEmpty()
		&& format.handleType() == QAbstractVideoBuffer::NoHandle;
}

bool VideoSurfaceAdapter::start(const QVideoSurfaceFormat &format)
{
	const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
	const QSize size = format.frameSize();
	
	if (imageFormat != QImage::Format_Invalid && !size.isEmpty()) 
	{
		this->imageFormat = imageFormat;
		this->m_pixelFormat = format.pixelFormat();
		imageSize = size;
		sourceRect = format.viewport();
	
		QAbstractVideoSurface::start(format);
	
		//widget->updateGeometry();
		updateVideoRect();
		
		qDebug()<< "VideoSurfaceAdapter::start: Started with imageFormat:"<<imageFormat<<", pixelFormat:"<<format.pixelFormat();
	
		return true;
	} 
	else 
	{
		qDebug()<< "VideoSurfaceAdapter::start: NOT starting, either format invalid or size empty.";
		return false;
	}
}

void VideoSurfaceAdapter::stop()
{
	currentFrame = QVideoFrame();
	targetRect = QRect();
	
	QAbstractVideoSurface::stop();
	
	//widget->update();
	emitter->present(QImage());
}

bool VideoSurfaceAdapter::present(const QVideoFrame &frame)
{
	if (surfaceFormat().pixelFormat() != frame.pixelFormat()
	 || surfaceFormat().frameSize() != frame.size()) 
	{
		setError(IncorrectFormatError);
		stop();
		qDebug()<< "VideoSurfaceAdapter::present: Format/size mismatch, stopping.";
	
		return false;
	} 
	else 
	{
		currentFrame = frame;
	
		//widget->repaint(targetRect);
		
		if (currentFrame.map(QAbstractVideoBuffer::ReadOnly)) 
		{
// 			const QTransform oldTransform = painter->transform();
// 		
// 			if (surfaceFormat().scanLineDirection() == QVideoSurfaceFormat::BottomToTop) {
// 			painter->scale(1, -1);
// 			painter->translate(0, -widget->height());
// 			}
			
			QImage image(
				currentFrame.bits(),
				currentFrame.width(),
				currentFrame.height(),
				currentFrame.bytesPerLine(),
				imageFormat);
				
// 			qDebug()<< "VideoSurfaceAdapter::present: Presenting image with "<<image.byteCount()<<" bytes";
				
			emitter->present(image.copy());
		
			//qDebug() << "Painting image size: "<<image.size();
		
// 			painter->drawImage(targetRect, image, sourceRect);
// 		
// 			painter->setTransform(oldTransform);
		
			currentFrame.unmap();
		}
	
		return true;
	}
}

void VideoSurfaceAdapter::updateVideoRect()
{
	QSize size = surfaceFormat().sizeHint();
// 	size.scale(widget->size().boundedTo(size), Qt::KeepAspectRatio);
	
	targetRect = QRect(QPoint(0, 0), size);
// 	targetRect.moveCenter(widget->rect().center());
}

// void VideoSurfaceAdapter::paint(QPainter *painter)
// {
// 	if (currentFrame.map(QAbstractVideoBuffer::ReadOnly)) 
// 	{
// 		const QTransform oldTransform = painter->transform();
// 	
// 		if (surfaceFormat().scanLineDirection() == QVideoSurfaceFormat::BottomToTop) {
// 		painter->scale(1, -1);
// 		painter->translate(0, -widget->height());
// 		}
// 		
// 		QImage image(
// 			currentFrame.bits(),
// 			currentFrame.width(),
// 			currentFrame.height(),
// 			currentFrame.bytesPerLine(),
// 			imageFormat);
// 	
// 		//qDebug() << "Painting image size: "<<image.size();
// 	
// 		painter->drawImage(targetRect, image, sourceRect);
// 	
// 		painter->setTransform(oldTransform);
// 	
// 		currentFrame.unmap();
// 	}
// }

VideoFormat QtVideoSource::videoFormat()
{ 
	return VideoFormat(VideoFrame::BUFFER_IMAGE,m_surfaceAdapter ? m_surfaceAdapter->pixelFormat() : QVideoFrame::Format_RGB32); 
}

QtVideoSource::QtVideoSource(QObject *parent)
	: VideoSource(parent)
{
	m_player   = new QMediaPlayer(this);
	m_playlist = new QMediaPlaylist(this);
	m_player->setPlaylist(m_playlist);
	
	m_surfaceAdapter = new VideoSurfaceAdapter(this);
	
	QVideoRendererControl* rendererControl = m_player->service()->requestControl<QVideoRendererControl*>();
	if (rendererControl)
		rendererControl->setSurface(m_surfaceAdapter);
	else
		qDebug() << "QtVideoSource: Unable to get QVideoRenderControl for video integration. No video will be emitted from this video source.";

}

QtVideoSource::~QtVideoSource()
{

}

void QtVideoSource::start(QThread::Priority /*priority*/)
{
	m_player->play();
}
	
void QtVideoSource::setFile(const QString& file)
{
	m_file = file;

	QFileInfo fileInfo(file);
	if (fileInfo.exists()) 
	{
		QUrl url = QUrl::fromLocalFile(fileInfo.absoluteFilePath());
		if (fileInfo.suffix().toLower() == QLatin1String("m3u"))
			m_playlist->load(url);
		else
			m_playlist->addMedia(url);
	} 
	else 
	{
		QUrl url(file);
		if (url.isValid()) 
		{
			m_playlist->addMedia(url);
		}
	}
}

void QtVideoSource::run()
{

}

void QtVideoSource::present(QImage image)
{
	// TODO is there some way to get the FPS from the QMediaPlayer or friends?
	m_frame = VideoFrame(image,1000/60);
// 	qDebug()<< "QtVideoSource::present: Got image, size:"<<image.size();
	enqueue(m_frame);
	emit frameReady();
}
