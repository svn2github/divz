/********************************************************************************
** Form generated from reading ui file 'SlideSettingsDialog.ui'
**
** Created: Sat Dec 19 21:06:51 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SLIDESETTINGSDIALOG_H
#define UI_SLIDESETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SlideSettingsDialog
{
public:
    QVBoxLayout *verticalLayout_4;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QDoubleSpinBox *slideChangeTime;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnNever;
    QPushButton *btnGuess;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_5;
    QRadioButton *btnUseGroup;
    QRadioButton *btnUseSlide;
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
    QSpacerItem *verticalSpacer;
    QSpacerItem *verticalSpacer_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SlideSettingsDialog)
    {
        if (SlideSettingsDialog->objectName().isEmpty())
            SlideSettingsDialog->setObjectName(QString::fromUtf8("SlideSettingsDialog"));
        SlideSettingsDialog->resize(407, 480);
        verticalLayout_4 = new QVBoxLayout(SlideSettingsDialog);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        tabWidget = new QTabWidget(SlideSettingsDialog);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayout_3 = new QVBoxLayout(tab);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        groupBox = new QGroupBox(tab);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        slideChangeTime = new QDoubleSpinBox(groupBox);
        slideChangeTime->setObjectName(QString::fromUtf8("slideChangeTime"));

        horizontalLayout->addWidget(slideChangeTime);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        btnNever = new QPushButton(groupBox);
        btnNever->setObjectName(QString::fromUtf8("btnNever"));

        horizontalLayout_2->addWidget(btnNever);

        btnGuess = new QPushButton(groupBox);
        btnGuess->setObjectName(QString::fromUtf8("btnGuess"));

        horizontalLayout_2->addWidget(btnGuess);


        verticalLayout_2->addLayout(horizontalLayout_2);


        verticalLayout_3->addWidget(groupBox);

        groupBox_4 = new QGroupBox(tab);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        verticalLayout_5 = new QVBoxLayout(groupBox_4);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        btnUseGroup = new QRadioButton(groupBox_4);
        btnUseGroup->setObjectName(QString::fromUtf8("btnUseGroup"));
        btnUseGroup->setChecked(true);

        verticalLayout_5->addWidget(btnUseGroup);

        btnUseSlide = new QRadioButton(groupBox_4);
        btnUseSlide->setObjectName(QString::fromUtf8("btnUseSlide"));

        verticalLayout_5->addWidget(btnUseSlide);

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

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

        tabWidget->addTab(tab, QString());

        verticalLayout_4->addWidget(tabWidget);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_2);

        buttonBox = new QDialogButtonBox(SlideSettingsDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_4->addWidget(buttonBox);

#ifndef QT_NO_SHORTCUT
        label->setBuddy(slideChangeTime);
#endif // QT_NO_SHORTCUT

        retranslateUi(SlideSettingsDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SlideSettingsDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SlideSettingsDialog, SLOT(reject()));
        QObject::connect(btnUseSlide, SIGNAL(toggled(bool)), fadeSettingsBase, SLOT(setEnabled(bool)));
        QObject::connect(speedSlider, SIGNAL(valueChanged(int)), speedBox, SLOT(setValue(int)));
        QObject::connect(qualitySlider, SIGNAL(valueChanged(int)), qualityBox, SLOT(setValue(int)));
        QObject::connect(qualityBox, SIGNAL(valueChanged(int)), qualitySlider, SLOT(setValue(int)));
        QObject::connect(speedBox, SIGNAL(valueChanged(int)), speedSlider, SLOT(setValue(int)));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SlideSettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SlideSettingsDialog)
    {
        SlideSettingsDialog->setWindowTitle(QApplication::translate("SlideSettingsDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("SlideSettingsDialog", "Auto Change Time Settings", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SlideSettingsDialog", "&Change slide automatically after:", 0, QApplication::UnicodeUTF8));
        slideChangeTime->setSpecialValueText(QApplication::translate("SlideSettingsDialog", "(Never)", 0, QApplication::UnicodeUTF8));
        slideChangeTime->setSuffix(QApplication::translate("SlideSettingsDialog", " seconds", 0, QApplication::UnicodeUTF8));
        btnNever->setText(QApplication::translate("SlideSettingsDialog", "&Never Change", 0, QApplication::UnicodeUTF8));
        btnGuess->setText(QApplication::translate("SlideSettingsDialog", "&Guess Time", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("SlideSettingsDialog", "Cross Fade Speed && Quality", 0, QApplication::UnicodeUTF8));
        btnUseGroup->setText(QApplication::translate("SlideSettingsDialog", "Use the Slide &Group cross fade && speed settings", 0, QApplication::UnicodeUTF8));
        btnUseSlide->setText(QApplication::translate("SlideSettingsDialog", "&Use the settings below instead:", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("SlideSettingsDialog", "Speed is how long the total fade should take, expressed\n"
"in milliseconds. There are 1000ms in 1 second.", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("SlideSettingsDialog", "Speed", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("SlideSettingsDialog", "Quality", 0, QApplication::UnicodeUTF8));
        speedBox->setSuffix(QApplication::translate("SlideSettingsDialog", " ms", 0, QApplication::UnicodeUTF8));
        qualityBox->setSuffix(QApplication::translate("SlideSettingsDialog", " frames", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("SlideSettingsDialog", "Quality is the number of intermediate (partially faded)\n"
"frames to show from start to end of the cross fade. \n"
"The more frames, the slower the fade may run and the\n"
"slower videos may display.", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("SlideSettingsDialog", "General Settings", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(SlideSettingsDialog);
    } // retranslateUi

};

namespace Ui {
    class SlideSettingsDialog: public Ui_SlideSettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SLIDESETTINGSDIALOG_H
