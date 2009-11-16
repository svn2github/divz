#ifndef GroupPlayerEditor_H
#define GroupPlayerEditor_H

#include "GroupPlayerSlideGroupFactory.h"

#include <QHash>
#include <QLineEdit>


class SlideEditorWindow;
class QCloseEvent;
class QPushButton;
#include <QCheckBox>

class GroupPlayerEditor : public AbstractSlideGroupEditor 
{
	Q_OBJECT
public:
	GroupPlayerEditor(SlideGroup *g=0, QWidget * parent = 0);
	~GroupPlayerEditor();
	
public slots:
	void setSlideGroup(SlideGroup*g,Slide *curSlide=0);
	
	void accepted();
	//void rejected();

protected:
	void closeEvent(QCloseEvent*);

private slots:
	void editorWindowClosed();

private:
	QLineEdit *m_title;
	SlideGroup *m_slideGroup;
	
};


#endif // GroupPlayerEditor_H
