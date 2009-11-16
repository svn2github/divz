#ifndef GROUPPLAYEREDITORWINDOW_H
#define GROUPPLAYEREDITORWINDOW_H

#include <QtGui/QMainWindow>

namespace Ui {
	class GroupPlayerEditorWindow;
}

#include "model/SlideGroupFactory.h"
#include "GroupPlayerSlideGroup.h"
class DocumentListModel;
class Document;

class GroupPlayerEditorWindow : public AbstractSlideGroupEditor 
{
	Q_OBJECT
public:
	GroupPlayerEditorWindow(SlideGroup *, QWidget *parent = 0);
	~GroupPlayerEditorWindow();

public slots:
	void setSlideGroup(SlideGroup *, Slide *curSlide=0);
	
	void accepted();
	
	void browseDoc();
	
	void loadOtherDoc(const QString&);
	void showOtherDoc();
	void showThisDoc();
	
	void docReturnPressed();
	
	void btnMoveUp();
	void btnMoveDown();
	void btnRemove();
	void btnAdd();

protected:
	void changeEvent(QEvent *e);

private:
	void addMemberToList(GroupPlayerSlideGroup::GroupMember);
	void resequenceItems();
	
	Ui::GroupPlayerEditorWindow *m_ui;
	DocumentListModel * m_docModel;
	Document *m_doc;
	GroupPlayerSlideGroup * m_group;
	
};

#endif // GROUPPLAYEREDITORWINDOW_H
