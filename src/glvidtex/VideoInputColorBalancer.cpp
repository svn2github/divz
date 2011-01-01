#include "VideoInputColorBalancer.h"


#include <QtGui>

#include "../livemix/VideoWidget.h"
#include "HistogramFilter.h"
#include "VideoReceiver.h"

VideoInputColorBalancer::VideoInputColorBalancer(QWidget *parent)
	: QWidget(parent)
	, m_histoMaster(new HistogramFilter())
	, m_histoSlave (new HistogramFilter())
{
	connect(m_histoMaster, SIGNAL(hsvStatsUpdated(int , int , int , 
						      int , int , int ,
						      int , int , int )),

		         this,   SLOT(hsvStatsUpdated(int , int , int , 
						      int , int , int ,
						      int , int , int )));

	connect(m_histoSlave,  SIGNAL(hsvStatsUpdated(int , int , int , 
						      int , int , int ,
						      int , int , int )),

		         this,   SLOT(hsvStatsUpdated(int , int , int , 
						      int , int , int ,
						      int , int , int )));
						      
	
	for(int i=0;i<9;i++)
		m_slaveStats[i]=0;
	
	for(int i=0;i<9;i++)
		m_masterStats[i]=0;
	
	for(int i=0;i<9;i++)
		m_deltas[i] = 0;
		
	m_vals[0]=50;
	m_vals[1]=50;
	m_vals[2]=50;
		
	QVBoxLayout *vbox = new QVBoxLayout(this);
	
	VideoWidget *vidMaster = new VideoWidget();
	VideoReceiver *sourceMaster = VideoReceiver::getReceiver("localhost",7755);
	
	VideoWidget *vidSlave = new VideoWidget();
	VideoReceiver *sourceSlave = VideoReceiver::getReceiver("localhost",7756);
	
	m_histoMaster->setVideoSource(sourceMaster);
	vidMaster->setVideoSource(m_histoMaster);
	
	m_histoSlave->setVideoSource(sourceSlave);
	vidSlave->setVideoSource(m_histoSlave);
	
	vbox->addWidget(vidMaster);
	vbox->addWidget(vidSlave);
}

VideoInputColorBalancer::~VideoInputColorBalancer()
{}
	
void VideoInputColorBalancer::hsvStatsUpdated(int hMin, int hMax, int hAvg, 
					      int sMin, int sMax, int sAvg,
					      int vMin, int vMax, int vAvg)
{
	HistogramFilter *filter = dynamic_cast<HistogramFilter*>(sender());
	
	if(filter == m_histoMaster)
	{
		m_masterStats[0] = hAvg;
		m_masterStats[1] = sAvg;
		m_masterStats[2] = vAvg;
	}
	else
	{
		m_slaveStats[0] = hAvg;
		m_slaveStats[1] = sAvg;
		m_slaveStats[2] = vAvg;
	}
	
	// convert deltas to percents
	m_deltas[0] = (m_masterStats[0] - m_slaveStats[0]) ;/// 255 * 100;
	m_deltas[1] = (m_masterStats[1] - m_slaveStats[1]) ;/// 255 * 100;
	m_deltas[2] = (m_masterStats[2] - m_slaveStats[2]) ;/// 255 * 100;
	
	int threshold = 5;
	 
	bool ht = abs(m_deltas[0]) > threshold;
	bool st = abs(m_deltas[1]) > threshold;
	bool vt = abs(m_deltas[2]) > threshold; 
	
	qDebug() << "HSV Deltas: H:"<<m_deltas[0]<<"% - threshold?"<<ht<<", S:"<<m_deltas[1]<<"% - threshold?"<<st<<", V:"<<m_deltas[2]<<"% - threshold?"<<vt;
	 
	if(ht)
	{
		if(m_deltas[0]>0)
		{
			m_vals[0]+=5;
			if(m_vals[0]>100)
				m_vals[0] = 100;
			QString cmd = QString("v4lctl -c /dev/video0 hue %1%").arg(m_vals[0]);
			qDebug() << "Hue hit top threshold, cmd:" << cmd;
			 
			system(qPrintable(cmd));
		}
		else
		{
			m_vals[0]-=5;
			if(m_vals[0]<0)
				m_vals[0] = 0;
			QString cmd = QString("v4lctl -c /dev/video0 hue %1%").arg(m_vals[0]);
			qDebug() << "Hue hit bottom threshold, cmd:" << cmd;
			 
			system(qPrintable(cmd));
		}
	}
	else
	{
		// TODO
	}
	
}
