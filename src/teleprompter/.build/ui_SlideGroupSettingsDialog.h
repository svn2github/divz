/********************************************************************************
** Form generated from reading ui file 'SlideGroupSettingsDialog.ui'
**
** Created: Sat Dec 19 21:06:51 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SLIDEGROUPSETTINGSDIALOG_H
#define UI_SLIDEGROUPSETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QRadioButton>
#include <QtGui/QSlider>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SlideGroupSettingsDialog
{
public:
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QLineEdit *title;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QRadioButton *rNothing;
    QRadioButton *rChange;
    QRadioButton *rJump;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QComboBox *jumpToBox;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_5;
    QRadioButton *btnUseApp;
    QRadioButton *btnUseGroup;
    QWidget *fadeSettingsBase;
    QVBoxLayout *verticalLayout;
    QLabel *label_6;
    QGridLayout *gridLayout;
    QLabel *label_4;
    QLabel *label_5;
    QSlider *speedSlider;
    QSlider *qualitySlider;
    QSpinBox *speedBox;
    QSpinBox *qualityBox;
    QLabel *label_7;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SlideGroupSettingsDialog)
    {
        if (SlideGroupSettingsDialog->objectName().isEmpty())
            SlideGroupSettingsDialog->setObjectName(QString::fromUtf8("SlideGroupSettingsDialog"));
        SlideGroupSettingsDialog->resize(390, 573);
        verticalLayout_3 = new QVBoxLayout(SlideGroupSettingsDialog);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        groupBox = new QGroupBox(SlideGroupSettingsDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout->addWidget(label_2);

        title = new QLineEdit(groupBox);
        title->setObjectName(QString::fromUtf8("title"));

        horizontalLayout->addWidget(title);


        verticalLayout_3->addWidget(groupBox);

        groupBox_2 = new QGroupBox(SlideGroupSettingsDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout_2 = new QVBoxLayout(groupBox_2);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_2->addWidget(label);

        rNothing = new QRadioButton(groupBox_2);
        rNothing->setObjectName(QString::fromUtf8("rNothing"));

        verticalLayout_2->addWidget(rNothing);

        rChange = new QRadioButton(groupBox_2);
        rChange->setObjectName(QString::fromUtf8("rChange"));
        rChange->setChecked(true);

        verticalLayout_2->addWidget(rChange);

        rJump = new QRadioButton(groupBox_2);
        rJump->setObjectName(QString::fromUtf8("rJump"));

        verticalLayout_2->addWidget(rJump);

        widget = new QWidget(groupBox_2);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setEnabled(false);
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setMargin(0);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_3 = new QLabel(widget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(label_3);

        jumpToBox = new QComboBox(widget);
        jumpToBox->setObjectName(QString::fromUtf8("jumpToBox"));

        horizontalLayout_2->addWidget(jumpToBox);


        verticalLayout_2->addWidget(widget);


        verticalLayout_3->addWidget(groupBox_2);

        groupBox_4 = new QGroupBox(SlideGroupSettingsDialog);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        verticalLayout_5 = new QVBoxLayout(groupBox_4);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        btnUseApp = new QRadioButton(groupBox_4);
        btnUseApp->setObjectName(QString::fromUtf8("btnUseApp"));
        btnUseApp->setChecked(true);

        verticalLayout_5->addWidget(btnUseApp);

        btnUseGroup = new QRadioButton(groupBox_4);
        btnUseGroup->setObjectName(QString::fromUtf8("btnUseGroup"));

        verticalLayout_5->addWidget(btnUseGroup);

        fadeSettingsBase = new QWidget(groupBox_4);
        fadeSettingsBase->setObjectName(QString::fromUtf8("fadeSettingsBase"));
        fadeSettingsBase->setEnabled(false);
        verticalLayout = new QVBoxLayout(fadeSettingsBase);
        verticalLayout->setMargin(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_6 = new QLabel(fadeSettingsBase);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        verticalLayout->addWidget(label_6);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_4 = new QLabel(fadeSettingsBase);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 0, 0, 1, 1);

        label_5 = new QLabel(fadeSettingsBase);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 1, 0, 1, 1);

        speedSlider = new QSlider(fadeSettingsBase);
        speedSlider->setObjectName(QString::fromUtf8("speedSlider"));
        speedSlider->setMinimum(50);
        speedSlider->setMaximum(5000);
        speedSlider->setSingleStep(100);
        speedSlider->setPageStep(500);
        speedSlider->setValue(250);
        speedSlider->setOrientation(Qt::Horizontal);
        speedSlider->setInvertedAppearance(false);
        speedSlider->setInvertedControls(false);
        speedSlider->setTickPosition(QSlider::TicksBelow);
        speedSlider->setTickInterval(500);

        gridLayout->addWidget(speedSlider, 0, 1, 1, 1);

        qualitySlider = new QSlider(fadeSettingsBase);
        qualitySlider->setObjectName(QString::fromUtf8("qualitySlider"));
        qualitySlider->setMinimum(1);
        qualitySlider->setMaximum(60);
        qualitySlider->setSingleStep(5);
        qualitySlider->setValue(15);
        qualitySlider->setOrientation(Qt::Horizontal);
        qualitySlider->setTickPosition(QSlider::TicksBelow);
        qualitySlider->setTickInterval(5);

        gridLayout->addWidget(qualitySlider, 1, 1, 1, 1);

        speedBox = new QSpinBox(fadeSettingsBase);
        speedBox->setObjectName(QString::fromUtf8("speedBox"));
        speedBox->setMinimum(50);
        speedBox->setMaximum(5000);
        speedBox->setSingleStep(10);

        gridLayout->addWidget(speedBox, 0, 2, 1, 1);

        qualityBox = new QSpinBox(fadeSettingsBase);
        qualityBox->setObjectName(QString::fromUtf8("qualityBox"));
        qualityBox->setMinimum(1);
        qualityBox->setMaximum(60);

        gridLayout->addWidget(qualityBox, 1, 2, 1, 1);


        verticalLayout->addLayout(gridLayout);

        label_7 = new QLabel(fadeSettingsBase);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        verticalLayout->addWidget(label_7);


        verticalLayout_5->addWidget(fadeSettingsBase);


        verticalLayout_3->addWidget(groupBox_4);

        buttonBox = new QDialogButtonBox(SlideGroupSettingsDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_3->addWidget(buttonBox);

#ifndef QT_NO_SHORTCUT
        label_2->setBuddy(title);
#endif // QT_NO_SHORTCUT

        retranslateUi(SlideGroupSettingsDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SlideGroupSettingsDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SlideGroupSettingsDialog, SLOT(reject()));
        QObject::connect(btnUseGroup, SIGNAL(toggled(bool)), fadeSettingsBase, SLOT(setEnabled(bool)));
        QObject::connect(speedBox, SIGNAL(valueChanged(int)), speedSlider, SLOT(setValue(int)));
        QObject::connect(speedSlider, SIGNAL(valueChanged(int)), speedBox, SLOT(setValue(int)));
        QObject::connect(qualitySlider, SIGNAL(valueChanged(int)), qualityBox, SLOT(setValue(int)));
        QObject::connect(qualityBox, SIGNAL(valueChanged(int)), qualitySlider, SLOT(setValue(int)));
        QObject::connect(rJump, SIGNAL(toggled(bool)), widget, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(SlideGroupSettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SlideGroupSettingsDialog)
    {
        SlideGroupSettingsDialog->setWindowTitle(QApplication::translate("SlideGroupSettingsDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("SlideGroupSettingsDialog", "Slide Group Title", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("SlideGroupSettingsDialog", "Group &Title:", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("SlideGroupSettingsDialog", "End-of-Slides Action", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SlideGroupSettingsDialog", "If the last slide of this group had an automatic change\n"
"time specified, what do you want this group do do\n"
"when that slide is finished displaying?", 0, QApplication::UnicodeUTF8));
        rNothing->setText(QApplication::translate("SlideGroupSettingsDialog", "&Loop to the start of this slide group", 0, QApplication::UnicodeUTF8));
        rChange->setText(QApplication::translate("SlideGroupSettingsDialog", "&Automatically show first slide of next slide group", 0, QApplication::UnicodeUTF8));
        rJump->setText(QApplication::translate("SlideGroupSettingsDialog", "Jump to the first slide of another slide group", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("SlideGroupSettingsDialog", "Jump to Slide Group: ", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("SlideGroupSettingsDialog", "Cross Fade Speed && Quality", 0, QApplication::UnicodeUTF8));
        btnUseApp->setText(QApplication::translate("SlideGroupSettingsDialog", "Use &Program Settings for Cross Fade Speed && Quality", 0, QApplication::UnicodeUTF8));
        btnUseGroup->setText(QApplication::translate("SlideGroupSettingsDialog", "&Use the settings below instead:", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("SlideGroupSettingsDialog", "Speed is how long the total fade should take, expressed\n"
"in milliseconds. There are 1000ms in 1 second.", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("SlideGroupSettingsDialog", "Speed", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("SlideGroupSettingsDialog", "Quality", 0, QApplication::UnicodeUTF8));
        speedBox->setSuffix(QApplication::translate("SlideGroupSettingsDialog", " ms", 0, QApplication::UnicodeUTF8));
        qualityBox->setSuffix(QApplication::translate("SlideGroupSettingsDialog", " frames", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("SlideGroupSettingsDialog", "Quality is the number of intermediate (partially faded)\n"
"frames to show from start to end of the cross fade. \n"
"The more frames, the slower the fade may run and the\n"
"slower videos may display.", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(SlideGroupSettingsDialog);
    } // retranslateUi

};

namespace Ui {
    class SlideGroupSettingsDialog: public Ui_SlideGroupSettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SLIDEGROUPSETTINGSDIALOG_H
