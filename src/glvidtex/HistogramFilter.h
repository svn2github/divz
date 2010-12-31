#ifndef HistogramFilter_H
#define HistogramFilter_H

#include <QtGui>
#include "VideoFilter.h"

class HistogramFilter : public VideoFilter
{
	Q_OBJECT
public:
	HistogramFilter(QObject *parent=0);
	~HistogramFilter();
	
	enum HistoType { Gray, RGB, Red, Green, Blue, HSV, Everything };
	
	HistoType histoType() { return m_histoType; }
	bool includeOriginalImage() { return m_includeOriginalImage; }
	
	bool chartHsv() { return m_chartHsv; }
	bool calcHsvStats() { return m_calcHsvStats; }

public slots:
	void setHistoType(HistoType type);
	void setIncludeOriginalImage(bool flag);
	
	void setChartHsv(bool flag);
	void setCalcHsvStats(bool flag);

signals:
	void hsvStatsUpdated(int hMin, int hMax, int hAvg, 
	                     int sMin, int sMax, int sAvg,
	                     int vMin, int vMax, int vAvg);
		
protected:
	virtual void processFrame();
	
	QImage makeHistogram(const QImage& image);
	
	void drawBarRect(QPainter *p, int min, int max, int avg, int startX, int startY, int w, int h);
	
	HistoType m_histoType;
	bool m_includeOriginalImage;

	bool m_chartHsv;
	bool m_calcHsvStats;
};

#endif
