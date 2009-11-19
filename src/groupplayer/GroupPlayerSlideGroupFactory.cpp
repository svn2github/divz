#include "GroupPlayerSlideGroupFactory.h"

#include "GroupPlayerSlideGroup.h"
#include "GroupPlayerEditorWindow.h"

GroupPlayerSlideGroupFactory::GroupPlayerSlideGroupFactory() {}
	
SlideGroup * GroupPlayerSlideGroupFactory::newSlideGroup() 
{
	return new GroupPlayerSlideGroup(); 
}

SlideGroupViewControl * GroupPlayerSlideGroupFactory::newViewControl()
{
	return new GroupPlayerSlideGroupViewControl();
}

AbstractSlideGroupEditor * GroupPlayerSlideGroupFactory::newEditor()
{
	return new GroupPlayerEditorWindow(0);
}


#define ITEM_GROUP_MEMBER(a) a ? a->data(Qt::UserRole).value<GroupPlayerSlideGroup::GroupMember>() : GroupPlayerSlideGroup::GroupMember()
#include <QVBoxLayout>
#include "OutputInstance.h"
#include "AppSettings.h"
#include "model/Output.h"

#include <QSettings>

#define DEBUG_GP_VIEWCONTROL 0 

GroupPlayerSlideGroupViewControl::GroupPlayerSlideGroupViewControl(OutputInstance *group, QWidget *w )
	: SlideGroupViewControl(group,w,false)
	, m_control(0)
	, m_group(0)
	, m_isPreviewControl(false)
{
	QVBoxLayout * layout = new QVBoxLayout();
	layout->setMargin(0);
	
	m_splitter = new QSplitter(this);
	m_splitter->setOrientation(Qt::Vertical);
	
	layout->addWidget(m_splitter);


	/** Setup the list view in icon mode */
	m_list = new QListWidget(this);
	//m_list = new GroupPlayerSlideGroupViewControlListView(this);
	m_list->setViewMode(QListView::IconMode);
	m_list->setMovement(QListView::Static);
	m_list->setWrapping(true);
	m_list->setWordWrap(true);
	m_list->setLayoutMode(QListView::Batched);
	m_list->setFlow(QListView::LeftToRight);
	m_list->setResizeMode(QListView::Adjust);
	m_list->setSelectionMode(QAbstractItemView::SingleSelection);
	m_list->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
	m_list->setMaximumSize(QWIDGETSIZE_MAX, 60);
// 	setFocusProxy(m_list);
// 	setFocusPolicy(Qt::StrongFocus);
	
	connect(m_list,SIGNAL(itemActivated(QListWidgetItem*)),this,SLOT(itemSelected(QListWidgetItem*)));
	connect(m_list,SIGNAL(itemClicked(QListWidgetItem*)),  this,SLOT(itemSelected(QListWidgetItem*)));
	//connect(m_list,SIGNAL(itemDoubleClicked(const QModelIndex &)),this,SLOT(slideDoubleClicked(const QModelIndex &)));
	//connect(m_list,SIGNAL(entered(const QModelIndex &)),  this,SLOT(slideSelected(const QModelIndex &)));
	
	m_splitter->addWidget(m_list);
	
	// the SlideGroupViewControl will be added when setGroup is called
	
	setLayout(layout);
	
	if(group)
		setOutputView(group);
	
	//QSettings s;
	//m_splitter->restoreState(s.value("GroupPlayerSlideGroupViewControl/splitter").toByteArray());
}

GroupPlayerSlideGroupViewControl::~GroupPlayerSlideGroupViewControl()
{
	//QSettings s;
	//s.setValue("GroupPlayerSlideGroupViewControl/splitter",m_splitter->saveState());
}

SlideGroupListModel * GroupPlayerSlideGroupViewControl::slideGroupListModel()
{
	if(!m_control)
		return 0;
		
	return m_control->slideGroupListModel();
}

void GroupPlayerSlideGroupViewControl::setQuickSlideEnabled(bool flag)
{
	if(m_control)
		m_control->setQuickSlideEnabled(flag);
}


void GroupPlayerSlideGroupViewControl::addQuickSlide()
{
	if(m_control)
		m_control->addQuickSlide();
}


Slide * GroupPlayerSlideGroupViewControl::selectedSlide() 
{
	if(m_control)
		return m_control->selectedSlide();
	return 0;
}

void GroupPlayerSlideGroupViewControl::showQuickSlide(bool flag)
{
	if(m_control)
		m_control->showQuickSlide(flag);
}

void GroupPlayerSlideGroupViewControl::setQuickSlideText(const QString& tmp)
{
	if(m_control)
		m_control->setQuickSlideText(tmp);
}
	
void GroupPlayerSlideGroupViewControl::setIsPreviewControl(bool flag)
{
	m_isPreviewControl = flag;
	if(m_control)
		m_control->setIsPreviewControl(flag);
}

void GroupPlayerSlideGroupViewControl::setEnabled(bool flag)
{
	if(m_control)
		m_control->setEnabled(flag);
	QWidget::setEnabled(flag);
}

void GroupPlayerSlideGroupViewControl::toggleTimerState(TimerState state, bool resetTimer)
{
	if(m_control)
		m_control->toggleTimerState(state,resetTimer);
		
}


// void GroupPlayerSlideGroupViewControl::slideDoubleClicked(const QModelIndex &idx)
// {
// 	// consider re-emitting this
// 	//emit slideDoubleClicked(slide);
// }

void GroupPlayerSlideGroupViewControl::setOutputView(OutputInstance *inst) 
{ 
	if(DEBUG_GP_VIEWCONTROL)
		qDebug() << "GroupPlayerSlideGroupViewControl::setOutputView: Setting output instance: "<<inst->output()->name();
	if(m_control)
		m_control->setOutputView(inst);
	
	if(m_slideViewer)
		disconnect(m_slideViewer, 0, this, 0);
	
	m_slideViewer = inst;
	m_slideViewer->setEndActionOverrideEnabled(true);
	m_slideViewer->setEndGroupAction(SlideGroup::Stop);
	connect(m_slideViewer, SIGNAL(endOfGroup()), this, SLOT(endOfGroup()));
	
	if(DEBUG_GP_VIEWCONTROL)
		qDebug() << "GroupPlayerSlideGroupViewControl::setOutputView: Instance "<<inst->output()->name()<<" set.";
}

void GroupPlayerSlideGroupViewControl::endOfGroup()
{
	// go to next group
	int row = m_list->currentRow(); //(item);
	if(row +1 >= m_list->count())
	{
		m_list->clearSelection();
		m_list->setCurrentRow(0, QItemSelectionModel::Select);
		
		itemSelected(m_list->item(0));
		return;
	}
	
	m_list->clearSelection();
	m_list->setCurrentRow(row+1, QItemSelectionModel::Select);
	
	itemSelected(m_list->item(row+1));
}

void GroupPlayerSlideGroupViewControl::itemSelected(QListWidgetItem *item)
{
	setCurrentMember(ITEM_GROUP_MEMBER(item));
}

void GroupPlayerSlideGroupViewControl::setCurrentMember(GroupPlayerSlideGroup::GroupMember mem)
{
	if(DEBUG_GP_VIEWCONTROL)
		qDebug()<<"GroupPlayerSlideGroupViewControl::setCurrentMember: Member#"<<mem.sequenceNumber<<": mark1";
	SlideGroup * group = mem.group;
	
	SlideGroupFactory *factory = SlideGroupFactory::factoryForType(group->groupType());
	if(!factory)
		factory = SlideGroupFactory::factoryForType(SlideGroup::GroupType);
	
// 	QSettings settings;
// 	settings.setValue("GroupPlayerSlideGroupViewControl/splitter",m_splitter->saveState());
	
	if(m_control)
	{
		m_control->deleteLater();
		m_control = 0;
	}
	
	if(DEBUG_GP_VIEWCONTROL)
		qDebug()<<"GroupPlayerSlideGroupViewControl::setCurrentMember: Member#"<<mem.sequenceNumber<<": removed control, making new control";
	
	m_control = factory->newViewControl();
	m_splitter->addWidget(m_control);
	
	//m_splitter->restoreState(settings.value("GroupPlayerSlideGroupViewControl/splitter").toByteArray());
	
	setOutputView(m_slideViewer);
	
	if(DEBUG_GP_VIEWCONTROL)
		qDebug()<<"GroupPlayerSlideGroupViewControl::setCurrentMember: Member#"<<mem.sequenceNumber<<": output view set, sending group to control";
	
	m_control->setIsPreviewControl(m_isPreviewControl);
	m_control->setSlideGroup(group);
	
	if(DEBUG_GP_VIEWCONTROL)
		qDebug()<<"GroupPlayerSlideGroupViewControl::setCurrentMember: Member#"<<mem.sequenceNumber<<": done setting current member";
	
}


void GroupPlayerSlideGroupViewControl::addMemberToList(GroupPlayerSlideGroup::GroupMember mem)
{
	QVariant var;
	var.setValue(mem);
	
	SlideGroup * group = mem.group;
	
	QListWidgetItem *item = new QListWidgetItem();
	item->setText(group->assumedName());
	item->setData(Qt::UserRole, var);
	
	QPixmap icon;
	
	SlideGroupFactory *factory = SlideGroupFactory::factoryForType(group->groupType());
	if(!factory)
		factory = SlideGroupFactory::factoryForType(SlideGroup::GroupType);
	
	QSize iconSize = QSize(48,(int)(48 * (1/AppSettings::liveAspectRatio())));
	QSize screenSize = QSize((int)(768 * AppSettings::liveAspectRatio()), 768);
	
	if(factory)
		icon = factory->generatePreviewPixmap(group,iconSize,QRect(QPoint(0,0),screenSize));
	
		
	item->setIcon(icon);
	
	m_list->addItem(item);

}

	
void GroupPlayerSlideGroupViewControl::setSlideGroup(SlideGroup *group, Slide *curSlide, bool allowProgressDialog)
{
	if(DEBUG_GP_VIEWCONTROL)
		qDebug()<<"GroupPlayerSlideGroupViewControl::setSlideGroup: Loading "<<group->assumedName();
	m_group = dynamic_cast<GroupPlayerSlideGroup *>(group);
	
	m_list->clear();
	
	if(DEBUG_GP_VIEWCONTROL)
		qDebug()<<"GroupPlayerSlideGroupViewControl::setSlideGroup: Setting up list";
	
	QList<GroupPlayerSlideGroup::GroupMember> members = m_group->members();
	
	foreach(GroupPlayerSlideGroup::GroupMember mem, members)
		addMemberToList(mem);
		
	if(DEBUG_GP_VIEWCONTROL)
		qDebug()<<"GroupPlayerSlideGroupViewControl::setSlideGroup: Setting first group";
	if(!members.isEmpty())
	{
		m_list->clearSelection();
		m_list->setCurrentRow(0, QItemSelectionModel::Select);
	
		setCurrentMember(members.first());
	}
}

void GroupPlayerSlideGroupViewControl::releaseSlideGroup()
{
	m_releasingSlideGroup = true;
	m_group = 0;
	m_subGroup = 0;
	if(m_control)
		m_control->releaseSlideGroup();
	m_releasingSlideGroup = false;
}

void GroupPlayerSlideGroupViewControl::nextSlide()
{
	if(m_control)
		m_control->nextSlide();
}

void GroupPlayerSlideGroupViewControl::prevSlide()
{
	if(m_control)
		m_control->prevSlide();
}

void GroupPlayerSlideGroupViewControl::setCurrentSlide(int x)
{
	if(m_control)
		m_control->setCurrentSlide(x);
}

void GroupPlayerSlideGroupViewControl::setCurrentSlide(Slide *s)
{
	if(m_control)
		m_control->setCurrentSlide(s);
}

void GroupPlayerSlideGroupViewControl::fadeBlackFrame(bool toggled)
{
// 	if(m_control)
// 		m_control->fadeBlackFrame(toggled);
}
	
void GroupPlayerSlideGroupViewControl::fadeClearFrame(bool toggled)
{
// 	if(m_control)
// 		m_control->fadeClearFrame(toggled);
}
