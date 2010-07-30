#ifndef MdiPreviewWidget_H
#define MdiPreviewWidget_H

#include "MdiVideoChild.h"
#include "VideoSource.h"

#include <QComboBox>
#include <QList>
#include <QRect>
#include <QSlider>
#include <QDoubleSpinBox>

class VideoOutputWidget;
class GLWidget;
class MdiPreviewWidget : public MdiVideoChild
{
	Q_OBJECT
public:
	MdiPreviewWidget(QWidget*parent=0);
	
public slots:
	void takeSource(MdiVideoChild *);
	void takeSource(VideoSource *);
	void setRenderFps(bool flag);
	
protected slots:
	void outputActionTriggered(QAction*);
	void textReturnPressed();
	void setFadeTimef(double);
	void setFadeTime(int);
	
protected:
	QComboBox *m_screenBox;
	QList<QRect> m_screenList;
	
	QLineEdit *m_textInput;
	
	QSlider *m_fadeSlider;
	QDoubleSpinBox *m_timeBox;
	
	VideoWidget *m_outputWidget;
	//VideoOutputWidget * m_outputWidget;
	//GLWidget *m_outputWidget;
};

#endif
