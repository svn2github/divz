#ifndef VideoInputColorBalancer_H
#define VideoInputColorBalancer_H

#include <QtGui>

class HistogramFilter;
class VideoInputColorBalancer : public QWidget
{
	Q_OBJECT
public:
	VideoInputColorBalancer(QWidget *parent=0);
	~VideoInputColorBalancer();
	
protected slots:
	void hsvStatsUpdated(int hMin, int hMax, int hAvg, 
	                     int sMin, int sMax, int sAvg,
	                     int vMin, int vMax, int vAvg);

private:
	HistogramFilter *m_histoMaster;
	HistogramFilter *m_histoSlave;

	int m_masterStats[9];
	int m_slaveStats[9];
	
	int m_deltas[9];
	
	int m_vals[3];
};

#endif
