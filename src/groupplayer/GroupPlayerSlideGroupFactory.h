#ifndef GroupPlayerSlideGroupFactory_H
#define GroupPlayerSlideGroupFactory_H

#include "model/SlideGroupFactory.h"
#include "GroupPlayerSlideGroup.h"

#include <QListWidget>
#include <QSplitter>

/// @brief GroupPlayerSlideGroupViewControl is a meta-control - it contains a list of groups in this group player, and displays an appropriate SlideGroupViewControl based on the current group.

class GroupPlayerSlideGroupViewControl : public SlideGroupViewControl
{
	Q_OBJECT
public:
	GroupPlayerSlideGroupViewControl(OutputInstance * view=0, QWidget * parent = 0);
	
	SlideGroup * slideGroup() { return m_group; }
	SlideGroupListModel * slideGroupListModel();
	
	OutputInstance * view() { return m_slideViewer; }
	virtual void setOutputView(OutputInstance *);
	
	Slide * selectedSlide();
	
	bool isQuickSlideToggled() { return m_showQuickSlideBtn->isChecked(); }
	
public slots:
	virtual void setSlideGroup(SlideGroup *g, Slide *curSlide=0, bool allowProgressDialog=true); //handle, switch groups
	virtual void releaseSlideGroup(); // handle 
	
	virtual void nextSlide(); // proxy, till end, then switch
	virtual void prevSlide(); // proxy, till end, then switch
	virtual void setCurrentSlide(int); // proxy
	virtual void setCurrentSlide(Slide*); // proxy
	virtual void toggleTimerState(TimerState state  = Undefined, bool resetTimer = false);// proxy
	
	virtual void fadeBlackFrame(bool); //proxy
	virtual void fadeClearFrame(bool); //proxy
	
	virtual void setIsPreviewControl(bool);//handle
	virtual void setEnabled(bool);//proxy
	
	virtual void setQuickSlideEnabled(bool); //proxy
	virtual void addQuickSlide();//proxy
	virtual void showQuickSlide(bool flag=true);//proxy
	virtual void setQuickSlideText(const QString& text = "");//proxy
	
	void setCurrentMember(GroupPlayerSlideGroup::GroupMember);
	
protected slots:
	void endOfGroup();
	void itemSelected(QListWidgetItem *);
	
	void addMemberToList(GroupPlayerSlideGroup::GroupMember mem);
	
protected:
	GroupPlayerSlideGroup * m_group;
	SlideGroup * m_subGroup;
	
	SlideGroupViewControl * m_control;
	
	QListWidget * m_list;
	QSplitter * m_splitter;
	
	bool m_isPreviewControl;
	
};


class GroupPlayerSlideGroupFactory : public SlideGroupFactory 
{
public:
	GroupPlayerSlideGroupFactory();
	
	SlideGroup * newSlideGroup();
	SlideGroupViewControl * newViewControl();
	AbstractSlideGroupEditor   * newEditor();
	
};


#endif
