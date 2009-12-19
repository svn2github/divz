/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created: Sat Dec 19 18:09:11 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionSetup_Outputs;
    QAction *actionExit;
    QAction *actionOpen_Text_File;
    QAction *actionSave;
    QAction *actionSave_As;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QTextEdit *textEdit;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QSpinBox *posBox;
    QSlider *posSlider;
    QPushButton *playBtn;
    QLabel *label_2;
    QSpinBox *accelBox;
    QSlider *accelSlider;
    QMenuBar *menubar;
    QMenu *menuFile;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(547, 449);
        actionSetup_Outputs = new QAction(MainWindow);
        actionSetup_Outputs->setObjectName(QString::fromUtf8("actionSetup_Outputs"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionOpen_Text_File = new QAction(MainWindow);
        actionOpen_Text_File->setObjectName(QString::fromUtf8("actionOpen_Text_File"));
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        actionSave_As = new QAction(MainWindow);
        actionSave_As->setObjectName(QString::fromUtf8("actionSave_As"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        textEdit = new QTextEdit(centralwidget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));

        verticalLayout_2->addWidget(textEdit);

        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        posBox = new QSpinBox(groupBox);
        posBox->setObjectName(QString::fromUtf8("posBox"));
        posBox->setMinimum(0);
        posBox->setMaximum(100);

        horizontalLayout->addWidget(posBox);

        posSlider = new QSlider(groupBox);
        posSlider->setObjectName(QString::fromUtf8("posSlider"));
        posSlider->setMinimum(0);
        posSlider->setMaximum(100);
        posSlider->setOrientation(Qt::Horizontal);
        posSlider->setTickPosition(QSlider::TicksBelow);
        posSlider->setTickInterval(10);

        horizontalLayout->addWidget(posSlider);

        playBtn = new QPushButton(groupBox);
        playBtn->setObjectName(QString::fromUtf8("playBtn"));

        horizontalLayout->addWidget(playBtn);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout->addWidget(label_2);

        accelBox = new QSpinBox(groupBox);
        accelBox->setObjectName(QString::fromUtf8("accelBox"));
        accelBox->setMinimum(-100);
        accelBox->setMaximum(100);

        horizontalLayout->addWidget(accelBox);

        accelSlider = new QSlider(groupBox);
        accelSlider->setObjectName(QString::fromUtf8("accelSlider"));
        accelSlider->setMinimum(-100);
        accelSlider->setMaximum(100);
        accelSlider->setSingleStep(10);
        accelSlider->setPageStep(30);
        accelSlider->setOrientation(Qt::Horizontal);
        accelSlider->setTickPosition(QSlider::TicksBelow);
        accelSlider->setTickInterval(50);

        horizontalLayout->addWidget(accelSlider);


        verticalLayout->addLayout(horizontalLayout);


        verticalLayout_2->addWidget(groupBox);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 547, 25));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpen_Text_File);
        menuFile->addAction(actionSave);
        menuFile->addAction(actionSave_As);
        menuFile->addSeparator();
        menuFile->addAction(actionSetup_Outputs);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);

        retranslateUi(MainWindow);
        QObject::connect(posBox, SIGNAL(valueChanged(int)), posSlider, SLOT(setValue(int)));
        QObject::connect(posSlider, SIGNAL(valueChanged(int)), posBox, SLOT(setValue(int)));
        QObject::connect(accelBox, SIGNAL(valueChanged(int)), accelSlider, SLOT(setValue(int)));
        QObject::connect(accelSlider, SIGNAL(valueChanged(int)), accelBox, SLOT(setValue(int)));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionSetup_Outputs->setText(QApplication::translate("MainWindow", "Setup Output", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("MainWindow", "Exit", 0, QApplication::UnicodeUTF8));
        actionOpen_Text_File->setText(QApplication::translate("MainWindow", "Open Text File ...", 0, QApplication::UnicodeUTF8));
        actionSave->setText(QApplication::translate("MainWindow", "Save", 0, QApplication::UnicodeUTF8));
        actionSave_As->setText(QApplication::translate("MainWindow", "Save As ...", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "Live Control", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Position:", 0, QApplication::UnicodeUTF8));
        posBox->setSuffix(QApplication::translate("MainWindow", "%", 0, QApplication::UnicodeUTF8));
        posBox->setPrefix(QString());
        playBtn->setText(QApplication::translate("MainWindow", "Play", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Accel:", 0, QApplication::UnicodeUTF8));
        accelBox->setSpecialValueText(QString());
        accelBox->setSuffix(QApplication::translate("MainWindow", "%", 0, QApplication::UnicodeUTF8));
        accelBox->setPrefix(QString());
        menuFile->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
