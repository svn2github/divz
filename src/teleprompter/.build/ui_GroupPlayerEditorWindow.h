/********************************************************************************
** Form generated from reading UI file 'GroupPlayerEditorWindow.ui'
**
** Created: Sat Dec 19 18:09:11 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GROUPPLAYEREDITORWINDOW_H
#define UI_GROUPPLAYEREDITORWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GroupPlayerEditorWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *title;
    QHBoxLayout *horizontalLayout_2;
    QListWidget *groupList;
    QVBoxLayout *verticalLayout;
    QPushButton *moveUpBtn;
    QPushButton *moveDownBtn;
    QSpacerItem *verticalSpacer;
    QPushButton *addBtn;
    QSpacerItem *verticalSpacer_2;
    QPushButton *delBtn;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout_3;
    QRadioButton *showThis;
    QHBoxLayout *horizontalLayout_3;
    QRadioButton *showOther;
    QLineEdit *otherDoc;
    QPushButton *browseBtn;
    QListView *docList;
    QDialogButtonBox *buttonBox;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *GroupPlayerEditorWindow)
    {
        if (GroupPlayerEditorWindow->objectName().isEmpty())
            GroupPlayerEditorWindow->setObjectName(QString::fromUtf8("GroupPlayerEditorWindow"));
        GroupPlayerEditorWindow->resize(800, 600);
        centralwidget = new QWidget(GroupPlayerEditorWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout_4 = new QVBoxLayout(centralwidget);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        title = new QLineEdit(centralwidget);
        title->setObjectName(QString::fromUtf8("title"));

        horizontalLayout->addWidget(title);


        verticalLayout_4->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        groupList = new QListWidget(centralwidget);
        groupList->setObjectName(QString::fromUtf8("groupList"));

        horizontalLayout_2->addWidget(groupList);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        moveUpBtn = new QPushButton(centralwidget);
        moveUpBtn->setObjectName(QString::fromUtf8("moveUpBtn"));

        verticalLayout->addWidget(moveUpBtn);

        moveDownBtn = new QPushButton(centralwidget);
        moveDownBtn->setObjectName(QString::fromUtf8("moveDownBtn"));

        verticalLayout->addWidget(moveDownBtn);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        addBtn = new QPushButton(centralwidget);
        addBtn->setObjectName(QString::fromUtf8("addBtn"));

        verticalLayout->addWidget(addBtn);

        verticalSpacer_2 = new QSpacerItem(20, 90, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        delBtn = new QPushButton(centralwidget);
        delBtn->setObjectName(QString::fromUtf8("delBtn"));

        verticalLayout->addWidget(delBtn);


        horizontalLayout_2->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        showThis = new QRadioButton(centralwidget);
        showThis->setObjectName(QString::fromUtf8("showThis"));
        showThis->setChecked(true);

        verticalLayout_3->addWidget(showThis);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        showOther = new QRadioButton(centralwidget);
        showOther->setObjectName(QString::fromUtf8("showOther"));

        horizontalLayout_3->addWidget(showOther);

        otherDoc = new QLineEdit(centralwidget);
        otherDoc->setObjectName(QString::fromUtf8("otherDoc"));
        otherDoc->setEnabled(false);

        horizontalLayout_3->addWidget(otherDoc);

        browseBtn = new QPushButton(centralwidget);
        browseBtn->setObjectName(QString::fromUtf8("browseBtn"));
        browseBtn->setEnabled(false);

        horizontalLayout_3->addWidget(browseBtn);


        verticalLayout_3->addLayout(horizontalLayout_3);


        verticalLayout_2->addLayout(verticalLayout_3);

        docList = new QListView(centralwidget);
        docList->setObjectName(QString::fromUtf8("docList"));

        verticalLayout_2->addWidget(docList);


        horizontalLayout_2->addLayout(verticalLayout_2);


        verticalLayout_4->addLayout(horizontalLayout_2);

        buttonBox = new QDialogButtonBox(centralwidget);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_4->addWidget(buttonBox);

        GroupPlayerEditorWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(GroupPlayerEditorWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 19));
        GroupPlayerEditorWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(GroupPlayerEditorWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        GroupPlayerEditorWindow->setStatusBar(statusbar);
#ifndef QT_NO_SHORTCUT
        label->setBuddy(title);
#endif // QT_NO_SHORTCUT

        retranslateUi(GroupPlayerEditorWindow);
        QObject::connect(showOther, SIGNAL(toggled(bool)), otherDoc, SLOT(setEnabled(bool)));
        QObject::connect(showOther, SIGNAL(toggled(bool)), browseBtn, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(GroupPlayerEditorWindow);
    } // setupUi

    void retranslateUi(QMainWindow *GroupPlayerEditorWindow)
    {
        GroupPlayerEditorWindow->setWindowTitle(QApplication::translate("GroupPlayerEditorWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("GroupPlayerEditorWindow", "&Title:", 0, QApplication::UnicodeUTF8));
        moveUpBtn->setText(QApplication::translate("GroupPlayerEditorWindow", "Move &Up", 0, QApplication::UnicodeUTF8));
        moveUpBtn->setShortcut(QApplication::translate("GroupPlayerEditorWindow", "Shift+S", 0, QApplication::UnicodeUTF8));
        moveDownBtn->setText(QApplication::translate("GroupPlayerEditorWindow", "Move &Down", 0, QApplication::UnicodeUTF8));
        moveDownBtn->setShortcut(QApplication::translate("GroupPlayerEditorWindow", "Shift+A", 0, QApplication::UnicodeUTF8));
        addBtn->setText(QApplication::translate("GroupPlayerEditorWindow", "&Add", 0, QApplication::UnicodeUTF8));
        delBtn->setText(QApplication::translate("GroupPlayerEditorWindow", "&Remove", 0, QApplication::UnicodeUTF8));
        delBtn->setShortcut(QApplication::translate("GroupPlayerEditorWindow", "Del", 0, QApplication::UnicodeUTF8));
        showThis->setText(QApplication::translate("GroupPlayerEditorWindow", "Show &Groups from Current Document", 0, QApplication::UnicodeUTF8));
        showOther->setText(QApplication::translate("GroupPlayerEditorWindow", "Load &document:", 0, QApplication::UnicodeUTF8));
        browseBtn->setText(QApplication::translate("GroupPlayerEditorWindow", "Browse", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class GroupPlayerEditorWindow: public Ui_GroupPlayerEditorWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GROUPPLAYEREDITORWINDOW_H
