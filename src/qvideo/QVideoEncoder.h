#ifndef QVIDEOENCODER_H
#define QVIDEOENCODER_H

#include <QtCore>

#include "QVideo.h"
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}

class QVideoEncoder : public QThread
{
	Q_OBJECT

public:
	QVideoEncoder(QVideo * video, QObject * parent = 0);
	~QVideoEncoder();

	void run();

	void setOutputFile(const QString & output_file)	 { m_output_filename = output_file; }
	void setSourceBytes(uchar * source_bits)		 { m_source_bits = source_bits; }
	void setSourceNumBytes(quint64 source_num_bytes) { m_source_num_bytes = source_num_bytes; }
	void setFrameRate(int frame_rate)				 { m_frame_rate = frame_rate; }
	void setDuration(int duration)					 { m_duration = duration; }
	void setOutputSize(QSize size);

signals:
	void errorStatus(const QString & status, bool fatal);
	void encodeFinished();
	void advanceSource(int ms);

private:
	QString m_output_filename;

	int m_frame_rate; //in fps
	int m_duration; //in seconds

	//width & height both must be multiple of two
	int m_output_width;
	int m_output_height;

	uchar * m_source_bits;
	quint64 m_source_num_bytes;

	QVideo * m_video;
};

#endif // QVIDEOENCODER_H
