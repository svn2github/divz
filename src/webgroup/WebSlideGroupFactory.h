#ifndef WebSlideGroupFactory_H
#define WebSlideGroupFactory_H

#include "model/SlideGroupFactory.h"

#include <QAction>
#include <QLineEdit>
#include <QWebView>
#include <QProgressBar>
#include <QSplitter>
#include <QToolBar>

class WebSlideGroup;
class WebSlideGroupViewControl : public SlideGroupViewControl
{
	Q_OBJECT
public:
	WebSlideGroupViewControl(OutputInstance * view=0, QWidget * parent = 0);
	~WebSlideGroupViewControl();

	bool canSetIconSize() { return false; }
	
protected:
	void resizeEvent(QResizeEvent*);

public slots:
	virtual void setSlideGroup(SlideGroup *g, Slide *curSlide=0, bool allowProgressDialog=true);
	virtual void setIsPreviewControl(bool);

protected slots:
	void adjustLocation();
	void changeLocation();
	void setProgress(int);
	void finishLoading(bool);
	void popWidget();
	void adjustTitle();

private:
	WebSlideGroup * m_webGroup;
	QWebView * m_view;
	QLineEdit * m_locationEdit;
	QProgressBar * m_progress;
	QWidget * m_controlBase;
	QSplitter * m_splitter;
	QToolBar * m_toolBar;
	QAction * m_popAction;

};


class WebSlideGroupFactory : public SlideGroupFactory
{
public:
	WebSlideGroupFactory();
	
	SlideGroup * newSlideGroup();
	
	//AbstractItemFilterList customFiltersFor(OutputInstance *instace = 0);
	SlideGroupViewControl * newViewControl();
	AbstractSlideGroupEditor   * newEditor();
	NativeViewer * newNativeViewer(OutputInstance *instance = 0);
	
// 	QPixmap generatePreviewPixmap(SlideGroup*, QSize iconSize, QRect sceneRect);
};


#endif
