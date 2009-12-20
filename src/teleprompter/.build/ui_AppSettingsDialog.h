/********************************************************************************
** Form generated from reading ui file 'AppSettingsDialog.ui'
**
** Created: Sat Dec 19 21:06:51 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_APPSETTINGSDIALOG_H
#define UI_APPSETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AppSettingsDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_14;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_3;
    QLabel *label;
    QPushButton *btnConfigOutputs;
    QGroupBox *groupBox_8;
    QVBoxLayout *verticalLayout_9;
    QCheckBox *httpEnabled;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_13;
    QSpinBox *httpPort;
    QSpacerItem *horizontalSpacer;
    QLabel *httpUrlLabel;
    QSpacerItem *verticalSpacer_2;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_8;
    QGroupBox *groupBox_6;
    QVBoxLayout *verticalLayout_11;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_9;
    QSpinBox *autosaveBox;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_14;
    QSpinBox *maxBackups;
    QSpacerItem *horizontalSpacer_3;
    QGroupBox *groupBox_5;
    QVBoxLayout *verticalLayout_12;
    QRadioButton *editModeSmooth;
    QRadioButton *editModeLive;
    QRadioButton *editModePublished;
    QLabel *label_10;
    QSpacerItem *verticalSpacer_3;
    QWidget *tab_3;
    QVBoxLayout *verticalLayout_7;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QCheckBox *cbUseOpenGL;
    QLabel *label_2;
    QSpacerItem *verticalSpacer_4;
    QWidget *tab_4;
    QVBoxLayout *verticalLayout_6;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_3;
    QHBoxLayout *horizontalLayout;
    QLabel *label_8;
    QSlider *cacheSlider;
    QSpinBox *cacheBox;
    QGroupBox *groupBox_7;
    QVBoxLayout *verticalLayout_13;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_11;
    QLineEdit *diskCacheBox;
    QPushButton *diskCacheBrowseBtn;
    QWidget *diskCacheSizeBase;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_12;
    QSlider *diskCacheSlider;
    QSpinBox *diskCacheSizeBox;
    QSpacerItem *verticalSpacer_5;
    QWidget *tab_5;
    QVBoxLayout *verticalLayout_10;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_6;
    QGridLayout *gridLayout;
    QLabel *label_4;
    QLabel *label_5;
    QSlider *speedSlider;
    QSlider *qualitySlider;
    QSpinBox *speedBox;
    QSpinBox *qualityBox;
    QLabel *label_7;
    QSpacerItem *verticalSpacer_6;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *AppSettingsDialog)
    {
        if (AppSettingsDialog->objectName().isEmpty())
            AppSettingsDialog->setObjectName(QString::fromUtf8("AppSettingsDialog"));
        AppSettingsDialog->resize(519, 395);
        verticalLayout_2 = new QVBoxLayout(AppSettingsDialog);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        tabWidget = new QTabWidget(AppSettingsDialog);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayout_14 = new QVBoxLayout(tab);
        verticalLayout_14->setObjectName(QString::fromUtf8("verticalLayout_14"));
        groupBox_2 = new QGroupBox(tab);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout_3 = new QVBoxLayout(groupBox_2);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_3->addWidget(label);

        btnConfigOutputs = new QPushButton(groupBox_2);
        btnConfigOutputs->setObjectName(QString::fromUtf8("btnConfigOutputs"));

        verticalLayout_3->addWidget(btnConfigOutputs);


        verticalLayout_14->addWidget(groupBox_2);

        groupBox_8 = new QGroupBox(tab);
        groupBox_8->setObjectName(QString::fromUtf8("groupBox_8"));
        verticalLayout_9 = new QVBoxLayout(groupBox_8);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        httpEnabled = new QCheckBox(groupBox_8);
        httpEnabled->setObjectName(QString::fromUtf8("httpEnabled"));

        verticalLayout_9->addWidget(httpEnabled);

        widget = new QWidget(groupBox_8);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout_5 = new QHBoxLayout(widget);
        horizontalLayout_5->setMargin(0);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_13 = new QLabel(widget);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        horizontalLayout_5->addWidget(label_13);

        httpPort = new QSpinBox(widget);
        httpPort->setObjectName(QString::fromUtf8("httpPort"));
        httpPort->setMinimum(1025);
        httpPort->setMaximum(65536);
        httpPort->setValue(8080);

        horizontalLayout_5->addWidget(httpPort);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);


        verticalLayout_9->addWidget(widget);

        httpUrlLabel = new QLabel(groupBox_8);
        httpUrlLabel->setObjectName(QString::fromUtf8("httpUrlLabel"));

        verticalLayout_9->addWidget(httpUrlLabel);


        verticalLayout_14->addWidget(groupBox_8);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_14->addItem(verticalSpacer_2);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        verticalLayout_8 = new QVBoxLayout(tab_2);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        groupBox_6 = new QGroupBox(tab_2);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        verticalLayout_11 = new QVBoxLayout(groupBox_6);
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_9 = new QLabel(groupBox_6);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        horizontalLayout_3->addWidget(label_9);

        autosaveBox = new QSpinBox(groupBox_6);
        autosaveBox->setObjectName(QString::fromUtf8("autosaveBox"));
        autosaveBox->setMinimum(0);
        autosaveBox->setMaximum(3600);
        autosaveBox->setSingleStep(30);
        autosaveBox->setValue(60);

        horizontalLayout_3->addWidget(autosaveBox);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout_11->addLayout(horizontalLayout_3);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_14 = new QLabel(groupBox_6);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        horizontalLayout_6->addWidget(label_14);

        maxBackups = new QSpinBox(groupBox_6);
        maxBackups->setObjectName(QString::fromUtf8("maxBackups"));
        maxBackups->setMinimum(1);
        maxBackups->setMaximum(9999);
        maxBackups->setValue(10);

        horizontalLayout_6->addWidget(maxBackups);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_3);


        verticalLayout_11->addLayout(horizontalLayout_6);


        verticalLayout_8->addWidget(groupBox_6);

        groupBox_5 = new QGroupBox(tab_2);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        verticalLayout_12 = new QVBoxLayout(groupBox_5);
        verticalLayout_12->setObjectName(QString::fromUtf8("verticalLayout_12"));
        editModeSmooth = new QRadioButton(groupBox_5);
        editModeSmooth->setObjectName(QString::fromUtf8("editModeSmooth"));

        verticalLayout_12->addWidget(editModeSmooth);

        editModeLive = new QRadioButton(groupBox_5);
        editModeLive->setObjectName(QString::fromUtf8("editModeLive"));

        verticalLayout_12->addWidget(editModeLive);

        editModePublished = new QRadioButton(groupBox_5);
        editModePublished->setObjectName(QString::fromUtf8("editModePublished"));

        verticalLayout_12->addWidget(editModePublished);

        label_10 = new QLabel(groupBox_5);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        verticalLayout_12->addWidget(label_10);


        verticalLayout_8->addWidget(groupBox_5);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_8->addItem(verticalSpacer_3);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        verticalLayout_7 = new QVBoxLayout(tab_3);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        groupBox = new QGroupBox(tab_3);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        cbUseOpenGL = new QCheckBox(groupBox);
        cbUseOpenGL->setObjectName(QString::fromUtf8("cbUseOpenGL"));

        verticalLayout->addWidget(cbUseOpenGL);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);


        verticalLayout_7->addWidget(groupBox);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_7->addItem(verticalSpacer_4);

        tabWidget->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        verticalLayout_6 = new QVBoxLayout(tab_4);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        groupBox_3 = new QGroupBox(tab_4);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        verticalLayout_4 = new QVBoxLayout(groupBox_3);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        label_3 = new QLabel(groupBox_3);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout_4->addWidget(label_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout->addWidget(label_8);

        cacheSlider = new QSlider(groupBox_3);
        cacheSlider->setObjectName(QString::fromUtf8("cacheSlider"));
        cacheSlider->setMinimum(10);
        cacheSlider->setMaximum(2048);
        cacheSlider->setSingleStep(16);
        cacheSlider->setPageStep(64);
        cacheSlider->setOrientation(Qt::Horizontal);
        cacheSlider->setTickPosition(QSlider::TicksBelow);
        cacheSlider->setTickInterval(128);

        horizontalLayout->addWidget(cacheSlider);

        cacheBox = new QSpinBox(groupBox_3);
        cacheBox->setObjectName(QString::fromUtf8("cacheBox"));
        cacheBox->setMinimum(10);
        cacheBox->setMaximum(2048);
        cacheBox->setSingleStep(10);

        horizontalLayout->addWidget(cacheBox);


        verticalLayout_4->addLayout(horizontalLayout);


        verticalLayout_6->addWidget(groupBox_3);

        groupBox_7 = new QGroupBox(tab_4);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        verticalLayout_13 = new QVBoxLayout(groupBox_7);
        verticalLayout_13->setObjectName(QString::fromUtf8("verticalLayout_13"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_11 = new QLabel(groupBox_7);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        horizontalLayout_4->addWidget(label_11);

        diskCacheBox = new QLineEdit(groupBox_7);
        diskCacheBox->setObjectName(QString::fromUtf8("diskCacheBox"));

        horizontalLayout_4->addWidget(diskCacheBox);

        diskCacheBrowseBtn = new QPushButton(groupBox_7);
        diskCacheBrowseBtn->setObjectName(QString::fromUtf8("diskCacheBrowseBtn"));

        horizontalLayout_4->addWidget(diskCacheBrowseBtn);


        verticalLayout_13->addLayout(horizontalLayout_4);

        diskCacheSizeBase = new QWidget(groupBox_7);
        diskCacheSizeBase->setObjectName(QString::fromUtf8("diskCacheSizeBase"));
        horizontalLayout_2 = new QHBoxLayout(diskCacheSizeBase);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_12 = new QLabel(diskCacheSizeBase);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        horizontalLayout_2->addWidget(label_12);

        diskCacheSlider = new QSlider(diskCacheSizeBase);
        diskCacheSlider->setObjectName(QString::fromUtf8("diskCacheSlider"));
        diskCacheSlider->setMinimum(10);
        diskCacheSlider->setMaximum(1048576);
        diskCacheSlider->setSingleStep(256);
        diskCacheSlider->setPageStep(1024);
        diskCacheSlider->setOrientation(Qt::Horizontal);
        diskCacheSlider->setTickPosition(QSlider::TicksBelow);
        diskCacheSlider->setTickInterval(65536);

        horizontalLayout_2->addWidget(diskCacheSlider);

        diskCacheSizeBox = new QSpinBox(diskCacheSizeBase);
        diskCacheSizeBox->setObjectName(QString::fromUtf8("diskCacheSizeBox"));
        diskCacheSizeBox->setMinimum(10);
        diskCacheSizeBox->setMaximum(1048576);
        diskCacheSizeBox->setSingleStep(10);

        horizontalLayout_2->addWidget(diskCacheSizeBox);


        verticalLayout_13->addWidget(diskCacheSizeBase);


        verticalLayout_6->addWidget(groupBox_7);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer_5);

        tabWidget->addTab(tab_4, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QString::fromUtf8("tab_5"));
        verticalLayout_10 = new QVBoxLayout(tab_5);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        groupBox_4 = new QGroupBox(tab_5);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        verticalLayout_5 = new QVBoxLayout(groupBox_4);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        label_6 = new QLabel(groupBox_4);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        verticalLayout_5->addWidget(label_6);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_4 = new QLabel(groupBox_4);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 0, 0, 1, 1);

        label_5 = new QLabel(groupBox_4);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 1, 0, 1, 1);

        speedSlider = new QSlider(groupBox_4);
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

        qualitySlider = new QSlider(groupBox_4);
        qualitySlider->setObjectName(QString::fromUtf8("qualitySlider"));
        qualitySlider->setMinimum(1);
        qualitySlider->setMaximum(60);
        qualitySlider->setSingleStep(5);
        qualitySlider->setValue(15);
        qualitySlider->setOrientation(Qt::Horizontal);
        qualitySlider->setTickPosition(QSlider::TicksBelow);
        qualitySlider->setTickInterval(5);

        gridLayout->addWidget(qualitySlider, 1, 1, 1, 1);

        speedBox = new QSpinBox(groupBox_4);
        speedBox->setObjectName(QString::fromUtf8("speedBox"));
        speedBox->setMinimum(50);
        speedBox->setMaximum(5000);
        speedBox->setSingleStep(10);

        gridLayout->addWidget(speedBox, 0, 2, 1, 1);

        qualityBox = new QSpinBox(groupBox_4);
        qualityBox->setObjectName(QString::fromUtf8("qualityBox"));
        qualityBox->setMinimum(1);
        qualityBox->setMaximum(60);

        gridLayout->addWidget(qualityBox, 1, 2, 1, 1);


        verticalLayout_5->addLayout(gridLayout);

        label_7 = new QLabel(groupBox_4);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        verticalLayout_5->addWidget(label_7);


        verticalLayout_10->addWidget(groupBox_4);

        verticalSpacer_6 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_10->addItem(verticalSpacer_6);

        tabWidget->addTab(tab_5, QString());

        verticalLayout_2->addWidget(tabWidget);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(AppSettingsDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_2->addWidget(buttonBox);


        retranslateUi(AppSettingsDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), AppSettingsDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), AppSettingsDialog, SLOT(reject()));
        QObject::connect(speedSlider, SIGNAL(valueChanged(int)), speedBox, SLOT(setValue(int)));
        QObject::connect(speedBox, SIGNAL(valueChanged(int)), speedSlider, SLOT(setValue(int)));
        QObject::connect(qualitySlider, SIGNAL(valueChanged(int)), qualityBox, SLOT(setValue(int)));
        QObject::connect(qualityBox, SIGNAL(valueChanged(int)), qualitySlider, SLOT(setValue(int)));
        QObject::connect(cacheBox, SIGNAL(valueChanged(int)), cacheSlider, SLOT(setValue(int)));
        QObject::connect(cacheSlider, SIGNAL(valueChanged(int)), cacheBox, SLOT(setValue(int)));
        QObject::connect(httpEnabled, SIGNAL(toggled(bool)), widget, SLOT(setEnabled(bool)));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(AppSettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *AppSettingsDialog)
    {
        AppSettingsDialog->setWindowTitle(QApplication::translate("AppSettingsDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("AppSettingsDialog", "Outputs", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("AppSettingsDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dlg 2';\">DViz supports multiple outputs for various uses.<br />Use this button to setup options for outputs.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        btnConfigOutputs->setText(QApplication::translate("AppSettingsDialog", "Configure Live Outputs", 0, QApplication::UnicodeUTF8));
        groupBox_8->setTitle(QApplication::translate("AppSettingsDialog", "HTTP Remote Control", 0, QApplication::UnicodeUTF8));
        httpEnabled->setText(QApplication::translate("AppSettingsDialog", "Enable HTTP Remote Control Server", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("AppSettingsDialog", "Remote Control Port:", 0, QApplication::UnicodeUTF8));
        httpUrlLabel->setText(QApplication::translate("AppSettingsDialog", "Control URL: http://localhost:8080/", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("AppSettingsDialog", "Outputs", 0, QApplication::UnicodeUTF8));
        groupBox_6->setTitle(QApplication::translate("AppSettingsDialog", "Auto Save", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("AppSettingsDialog", "Auto-Save Every: ", 0, QApplication::UnicodeUTF8));
        autosaveBox->setSpecialValueText(QApplication::translate("AppSettingsDialog", "( No Autosave )", 0, QApplication::UnicodeUTF8));
        autosaveBox->setSuffix(QApplication::translate("AppSettingsDialog", " seconds", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("AppSettingsDialog", "Max Backups to Keep:", 0, QApplication::UnicodeUTF8));
        groupBox_5->setTitle(QApplication::translate("AppSettingsDialog", "Editing Mode", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        editModeSmooth->setToolTip(QApplication::translate("AppSettingsDialog", "Smooth Edit means that if the slide is live and you edit it in the editor,  the live view will cross fade to the new changes any time a change is made in the editor.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        editModeSmooth->setText(QApplication::translate("AppSettingsDialog", "Smooth Edit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        editModeLive->setToolTip(QApplication::translate("AppSettingsDialog", "Live Edit Mode means that the instant a change is made in the editor, the live slide will cut right to that change, no cross fade.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        editModeLive->setText(QApplication::translate("AppSettingsDialog", "Live Edit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        editModePublished->setToolTip(QApplication::translate("AppSettingsDialog", "No changes are made visible unless the slide is re-sent to the output.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        editModePublished->setText(QApplication::translate("AppSettingsDialog", "Published Edit", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("AppSettingsDialog", "The \"Edit Mode\" controls what happens when a slide is \"live\"\n"
"on an output and it is changed in the editor.", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("AppSettingsDialog", "Editing", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("AppSettingsDialog", "Graphics Acceleration", 0, QApplication::UnicodeUTF8));
        cbUseOpenGL->setText(QApplication::translate("AppSettingsDialog", "Use OpenGL Graphics Acceleration", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("AppSettingsDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<img src=\":/data/opengl-logo.png\" align=\"right\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dlg 2';\">The only reason to turn off OpenGL acceleration is if </span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dlg 2';\">the program doesn't run smoothly or freezes with </span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dl"
                        "g 2';\">OpenGL turned on.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("AppSettingsDialog", "OpenGL", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("AppSettingsDialog", "Live Cache Size", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("AppSettingsDialog", "DViz stores the results of certain graphical operations \n"
"in memory inorder to speed up the display of slides. \n"
"Less memory allocated to that cache means that DViz\n"
"potentially has to redraw items more frequently, \n"
"resulting in slower performance.", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("AppSettingsDialog", "Live Cache Size:", 0, QApplication::UnicodeUTF8));
        cacheBox->setSuffix(QApplication::translate("AppSettingsDialog", " MB", 0, QApplication::UnicodeUTF8));
        groupBox_7->setTitle(QApplication::translate("AppSettingsDialog", "Disk Cache", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("AppSettingsDialog", "Disk Cache Location:", 0, QApplication::UnicodeUTF8));
        diskCacheBrowseBtn->setText(QApplication::translate("AppSettingsDialog", "Browse", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("AppSettingsDialog", "Disk Cache Size:", 0, QApplication::UnicodeUTF8));
        diskCacheSizeBox->setSuffix(QApplication::translate("AppSettingsDialog", " MB", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("AppSettingsDialog", "Cache", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("AppSettingsDialog", "Cross Fade Speed && Quality", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("AppSettingsDialog", "Speed is how long the total fade should take, expressed\n"
"in milliseconds. There are 1000ms in 1 second.", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("AppSettingsDialog", "Speed", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("AppSettingsDialog", "Quality", 0, QApplication::UnicodeUTF8));
        speedBox->setSuffix(QApplication::translate("AppSettingsDialog", " ms", 0, QApplication::UnicodeUTF8));
        qualityBox->setSuffix(QApplication::translate("AppSettingsDialog", " frames", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("AppSettingsDialog", "Quality is the number of intermediate (partially faded)\n"
"frames to show from start to end of the cross fade. \n"
"The more frames, the slower the fade may run and the\n"
"slower videos may display.", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_5), QApplication::translate("AppSettingsDialog", "Cross Fading", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(AppSettingsDialog);
    } // retranslateUi

};

namespace Ui {
    class AppSettingsDialog: public Ui_AppSettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_APPSETTINGSDIALOG_H
