
#ifndef MdiVideoChild_H
#define MdiVideoChild_H

#include <QWidget>
#include "MdiChild.h"
#include "VideoSource.h"
#include "VideoWidget.h"
#include <QLineEdit>
#include <QVBoxLayout>
#include <QMouseEvent>

class MdiVideoChild : public MdiChild
{
	Q_OBJECT

public:
	MdiVideoChild(QWidget *parent=0);
	
	VideoSource *videoSource() { return m_videoSource; }
	VideoWidget *videoWidget() { return m_videoWidget; }
	
	virtual void setVideoSource(VideoSource*);

signals:
	void clicked();
	
protected slots:
	void textReturnPressed();
	
protected:
	void setupDefaultGui();
	
	QVBoxLayout *m_layout;
	QLineEdit *m_textInput;
	VideoSource *m_videoSource;
	VideoWidget *m_videoWidget;
	
	
	
};

#endif
