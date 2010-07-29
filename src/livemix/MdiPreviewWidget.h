#ifndef MdiPreviewWidget_H
#define MdiPreviewWidget_H

#include "MdiVideoChild.h"
#include "VideoSource.h"

#include <QComboBox>
#include <QList>
#include <QRect>

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
	
protected slots:
	void screenBoxChanged(int);	
	void textReturnPressed();
	
protected:
	QComboBox *m_screenBox;
	QList<QRect> m_screenList;
	
	QLineEdit *m_textInput;
	
	VideoWidget *m_outputWidget;
	//VideoOutputWidget * m_outputWidget;
	//GLWidget *m_outputWidget;
};

#endif
