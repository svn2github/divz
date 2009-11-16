#include "GroupPlayerEditorWindow.h"
#include "ui_GroupPlayerEditorWindow.h"

#include "DocumentListModel.h"
#include "model/Document.h"
#include "AppSettings.h"
#include "MainWindow.h"

#include <QDirModel>
#include <QCompleter>
#include <QFileDialog>
#include <QMessageBox>

#define ITEM_GROUP_MEMBER(a) a ? a->data(Qt::UserRole).value<GroupPlayerSlideGroup::GroupMember>() : GroupPlayerSlideGroup::GroupMember()

static void GroupPlayerEditorWindow_setupGenericDirectoryCompleter(QLineEdit *lineEdit)
{
	QCompleter *completer = new QCompleter(lineEdit);
	QDirModel *dirModel = new QDirModel(completer);
	completer->setModel(dirModel);
	//completer->setMaxVisibleItems(10);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setWrapAround(true);
	lineEdit->setCompleter(completer);
}

GroupPlayerEditorWindow::GroupPlayerEditorWindow(SlideGroup *group, QWidget *parent) 
	: AbstractSlideGroupEditor(group,parent)
	, m_ui(new Ui::GroupPlayerEditorWindow)
	, m_docModel(new DocumentListModel())
	, m_doc(0)
{
	m_ui->setupUi(this);
	
	setWindowIcon(QIcon(":/data/icon-d.png"));
	setWindowTitle("Group Player Editor");
	
	m_ui->docList->setModel(m_docModel);
	
	m_docModel->setDocument(MainWindow::mw()->currentDocument());
	
	GroupPlayerEditorWindow_setupGenericDirectoryCompleter(m_ui->otherDoc);
	
	connect(m_ui->showThis,  SIGNAL(clicked()),  this, SLOT(showThisDoc()));
	connect(m_ui->showOther, SIGNAL(clicked()),  this, SLOT(showOtherDoc()));
	connect(m_ui->browseBtn, SIGNAL(clicked()),  this, SLOT(browseDoc()));
	connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(accepted()));
	connect(m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(close()));
	
	connect(m_ui->otherDoc,  SIGNAL(returnPressed()), this, SLOT(docReturnPressed()));
	
	connect(m_ui->moveUpBtn,   SIGNAL(clicked()), this, SLOT(btnMoveUp()));
	connect(m_ui->moveDownBtn, SIGNAL(clicked()), this, SLOT(btnMoveDown()));
	connect(m_ui->addBtn,      SIGNAL(clicked()), this, SLOT(btnAdd()));
	connect(m_ui->delBtn,      SIGNAL(clicked()), this, SLOT(btnRemove()));
	
	if(group)
		setSlideGroup(group);
	
}

GroupPlayerEditorWindow::~GroupPlayerEditorWindow()
{
	if(m_doc)
		delete m_doc;
		
	delete m_ui;
}

void GroupPlayerEditorWindow::btnMoveUp()
{
// 	GroupPlayerSlideGroup::GroupMember mem = m_groupModel->memberForIndex(m_ui->groupList->currentIndex());
// 	m_group->resequence(mem,3);

	//QListWidgetItem * item = m_ui->groupList->currentItem();
	int row = m_ui->groupList->currentRow();
	if(row == 0)
		return;
		
	m_ui->groupList->insertItem(row-1,  m_ui->groupList->takeItem(row));
	m_ui->groupList->clearSelection();
	m_ui->groupList->setCurrentRow(row-1, QItemSelectionModel::Select);
	
	resequenceItems();
	
}

void GroupPlayerEditorWindow::btnMoveDown()
{
	//QListWidgetItem * item = m_ui->groupList->currentItem();
	int row = m_ui->groupList->currentRow(); //(item);
	if(row +1 >= m_ui->groupList->count())
		return;
		
	m_ui->groupList->insertItem(row+1,  m_ui->groupList->takeItem(row));
	m_ui->groupList->clearSelection();
	m_ui->groupList->setCurrentRow(row+1, QItemSelectionModel::Select);
	
	resequenceItems();
		
// 	GroupPlayerSlideGroup::GroupMember mem = m_groupModel->memberForIndex(m_ui->groupList->currentIndex());
// 	m_group->resequence(mem,2);
}

void GroupPlayerEditorWindow::resequenceItems()
{
	for(int i=0; i<m_ui->groupList->count(); i++)
	{
		QListWidgetItem * item = m_ui->groupList->item(i);
		
		GroupPlayerSlideGroup::GroupMember mem = ITEM_GROUP_MEMBER(item);
		mem.sequenceNumber = i;
		
		QVariant var;
		var.setValue(mem);
		
		item->setData(Qt::UserRole, var);
	}

}

void GroupPlayerEditorWindow::addMemberToList(GroupPlayerSlideGroup::GroupMember mem)
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
	
	m_ui->groupList->addItem(item);

}

void GroupPlayerEditorWindow::btnRemove()
{
	GroupPlayerSlideGroup::GroupMember mem = ITEM_GROUP_MEMBER(m_ui->groupList->currentItem());
	if(mem.group)
	{
		m_group->removeGroup(mem);
		
		int row = m_ui->groupList->currentRow();
		delete m_ui->groupList->takeItem(row);
		
		m_ui->groupList->clearSelection();
		if(row > 0)
			m_ui->groupList->setCurrentRow(row-1, QItemSelectionModel::Select);
	}
}

void GroupPlayerEditorWindow::btnAdd()
{
	SlideGroup * group = m_docModel->groupFromIndex(m_ui->docList->currentIndex());
	if(group)
	{
		if(group == m_group)
		{
			QMessageBox::warning(this,"Cannot Add This Group","Sorry, you cannot add the same group that you are editing to this group!");
			return;
		}
		else
		{
			GroupPlayerSlideGroup::GroupMember mem = m_group->addGroup(group, m_ui->showOther->isChecked() ? m_doc : 0);
			addMemberToList(mem);
		}
	}
}

void GroupPlayerEditorWindow::setSlideGroup(SlideGroup *groupTmp, Slide *curSlide)
{
	GroupPlayerSlideGroup *group = dynamic_cast<GroupPlayerSlideGroup*>(groupTmp);
	if(!group)
	{
		// show some sort of error
		return;
	}
	
	m_group = group;
	
	m_ui->title->setText(group->groupTitle());
	
	setWindowTitle(QString(tr("%1 - Group Player Editor").arg(group->assumedName())));
	
	QList<GroupPlayerSlideGroup::GroupMember> members = m_group->members();
	
	foreach(GroupPlayerSlideGroup::GroupMember mem, members)
		addMemberToList(mem);
}

void GroupPlayerEditorWindow::accepted()
{
	m_group->setGroupTitle(m_ui->title->text());
	
	QList<GroupPlayerSlideGroup::GroupMember> list;
	
	for(int i=0; i<m_ui->groupList->count(); i++)
	{
		QListWidgetItem * item = m_ui->groupList->item(i);
		
		//GroupPlayerSlideGroup::GroupMember mem = ;
		list.append(ITEM_GROUP_MEMBER(item));
	}
	
	m_group->setMembers(list);
	
	close();
}


void GroupPlayerEditorWindow::browseDoc()
{
	QString text = m_ui->otherDoc->text();
	if(text.trimmed().isEmpty())
	{
		text = AppSettings::previousPath("last-groupplayer-doc");
	}
	
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select DViz Document"), text, tr("DViz Document (*.xml *.dviz *.dvz *.dv);;Any File (*.*)"));
	if(fileName != "")
	{
		loadOtherDoc(fileName);
		m_ui->otherDoc->setText(fileName);
		AppSettings::setPreviousPath("last-groupplayer-doc",QFileInfo(fileName).absolutePath());
	}
}

void GroupPlayerEditorWindow::loadOtherDoc(const QString& other)
{
	if(!m_doc)
		delete m_doc;
		
	m_doc = new Document(other);
	m_docModel->setDocument(m_doc);
	if(!m_ui->showOther->isChecked())
		m_ui->showOther->setChecked(true);
}

void GroupPlayerEditorWindow::docReturnPressed()
{
	loadOtherDoc(m_ui->otherDoc->text());
}

void GroupPlayerEditorWindow::showOtherDoc()
{
	m_docModel->setDocument(m_doc);
}

void GroupPlayerEditorWindow::showThisDoc()
{
	m_docModel->setDocument(MainWindow::mw()->currentDocument());
}


void GroupPlayerEditorWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) 
	{
		case QEvent::LanguageChange:
			m_ui->retranslateUi(this);
		break;
			default:
		break;
	}
}
