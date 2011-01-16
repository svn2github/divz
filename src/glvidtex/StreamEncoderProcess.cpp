

#include "StreamEncoderProcess.h"
#include "DVizSharedMemoryThread.h"
#include "VideoEncoder.h"

StreamEncoderProcess::StreamEncoderProcess(QObject *parent)
	: QObject(parent)
{
	m_shMemRec = DVizSharedMemoryThread::threadForKey("PlayerWindow");
	
	QString outputFile = "streamencoder.avi";
	
	QFileInfo info(outputFile);
	if(info.exists())
	{
		int counter = 1;
		QString newFile;
		while(QFileInfo(newFile = QString("%1-%2.%3")
			.arg(info.baseName())
			.arg(counter)
			.arg(info.completeSuffix()))
			.exists())
			
			counter++;
		
		qDebug() << "StreamEncoderProcess: Video output file"<<outputFile<<"exists, writing to"<<newFile<<"instead.";
		outputFile = newFile;
	}

	
	m_encoder = new VideoEncoder(outputFile, this);
	m_outputEncoder->setVideoSource(m_shMemRec);
	m_outputEncoder->startEncoder(); 
};
