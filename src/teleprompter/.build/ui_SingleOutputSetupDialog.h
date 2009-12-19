/********************************************************************************
** Form generated from reading UI file 'SingleOutputSetupDialog.ui'
**
** Created: Sat Dec 19 18:09:11 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SINGLEOUTPUTSETUPDIALOG_H
#define UI_SINGLEOUTPUTSETUPDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SingleOutputSetupDialog
{
public:
    QVBoxLayout *verticalLayout_8;
    QTabWidget *tabWidget_2;
    QWidget *tab;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *gbSettings;
    QVBoxLayout *verticalLayout_3;
    QFormLayout *formLayout;
    QLineEdit *outputName;
    QLabel *label;
    QLabel *label_2;
    QCheckBox *cbOutputEnabled;
    QTabWidget *tabWidget;
    QWidget *tabScreen;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_3;
    QTableWidget *screenListView;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QCheckBox *mjpegEnabled;
    QLabel *label_16;
    QSpinBox *mjpegPort;
    QLabel *label_17;
    QSpinBox *mjpegFps;
    QSpacerItem *horizontalSpacer;
    QWidget *tabCustom;
    QVBoxLayout *verticalLayout;
    QLabel *label_4;
    QLabel *label_5;
    QFrame *line_2;
    QCheckBox *stayOnTop;
    QFormLayout *formLayout_2;
    QLabel *label_6;
    QSpinBox *customLeft;
    QLabel *label_7;
    QSpinBox *customTop;
    QLabel *label_8;
    QSpinBox *customWidth;
    QLabel *label_9;
    QSpinBox *customHeight;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_3;
    QCheckBox *mjpegEnabled2;
    QLabel *label_18;
    QSpinBox *mjpegPort2;
    QLabel *label_19;
    QSpinBox *mjpegFps2;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer;
    QWidget *tabNetwork;
    QVBoxLayout *verticalLayout_6;
    QLabel *label_10;
    QFrame *line;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_11;
    QSpinBox *listenPort;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *verticalSpacer_2;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_7;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_5;
    QCheckBox *cbUseOpenGL;
    QLabel *label_12;
    QSpacerItem *verticalSpacer_3;
    QWidget *tab_3;
    QVBoxLayout *verticalLayout_10;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_9;
    QLabel *label_13;
    QHBoxLayout *horizontalLayout;
    QLabel *label_14;
    QSlider *cacheSlider;
    QSpinBox *cacheBox;
    QGroupBox *groupBox_7;
    QVBoxLayout *verticalLayout_13;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_15;
    QLineEdit *diskCacheBox;
    QPushButton *diskCacheBrowseBtn;
    QWidget *diskCacheSizeBase;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_20;
    QSlider *diskCacheSlider;
    QSpinBox *diskCacheSizeBox;
    QSpacerItem *verticalSpacer_4;
    QWidget *tab_4;
    QVBoxLayout *verticalLayout_12;
    QGroupBox *groupBox_8;
    QVBoxLayout *verticalLayout_11;
    QTableWidget *resourceTranslations;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *resourceAddBtn;
    QPushButton *resourceDelBtn;
    QSpacerItem *horizontalSpacer_4;
    QLabel *label_21;
    QSpacerItem *verticalSpacer_5;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SingleOutputSetupDialog)
    {
        if (SingleOutputSetupDialog->objectName().isEmpty())
            SingleOutputSetupDialog->setObjectName(QString::fromUtf8("SingleOutputSetupDialog"));
        SingleOutputSetupDialog->resize(509, 565);
        verticalLayout_8 = new QVBoxLayout(SingleOutputSetupDialog);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        tabWidget_2 = new QTabWidget(SingleOutputSetupDialog);
        tabWidget_2->setObjectName(QString::fromUtf8("tabWidget_2"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayout_2 = new QVBoxLayout(tab);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        gbSettings = new QGroupBox(tab);
        gbSettings->setObjectName(QString::fromUtf8("gbSettings"));
        verticalLayout_3 = new QVBoxLayout(gbSettings);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        outputName = new QLineEdit(gbSettings);
        outputName->setObjectName(QString::fromUtf8("outputName"));

        formLayout->setWidget(0, QFormLayout::FieldRole, outputName);

        label = new QLabel(gbSettings);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);


        verticalLayout_3->addLayout(formLayout);

        label_2 = new QLabel(gbSettings);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_3->addWidget(label_2);

        cbOutputEnabled = new QCheckBox(gbSettings);
        cbOutputEnabled->setObjectName(QString::fromUtf8("cbOutputEnabled"));

        verticalLayout_3->addWidget(cbOutputEnabled);

        tabWidget = new QTabWidget(gbSettings);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabScreen = new QWidget();
        tabScreen->setObjectName(QString::fromUtf8("tabScreen"));
        verticalLayout_4 = new QVBoxLayout(tabScreen);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        label_3 = new QLabel(tabScreen);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout_4->addWidget(label_3);

        screenListView = new QTableWidget(tabScreen);
        if (screenListView->columnCount() < 2)
            screenListView->setColumnCount(2);
        screenListView->setObjectName(QString::fromUtf8("screenListView"));
        screenListView->setColumnCount(2);
        screenListView->verticalHeader()->setVisible(false);

        verticalLayout_4->addWidget(screenListView);

        groupBox = new QGroupBox(tabScreen);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        mjpegEnabled = new QCheckBox(groupBox);
        mjpegEnabled->setObjectName(QString::fromUtf8("mjpegEnabled"));

        gridLayout_2->addWidget(mjpegEnabled, 0, 0, 1, 4);

        label_16 = new QLabel(groupBox);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        gridLayout_2->addWidget(label_16, 1, 0, 1, 1);

        mjpegPort = new QSpinBox(groupBox);
        mjpegPort->setObjectName(QString::fromUtf8("mjpegPort"));
        mjpegPort->setMinimum(1025);
        mjpegPort->setMaximum(65536);
        mjpegPort->setValue(8080);

        gridLayout_2->addWidget(mjpegPort, 1, 1, 1, 1);

        label_17 = new QLabel(groupBox);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        gridLayout_2->addWidget(label_17, 1, 2, 1, 1);

        mjpegFps = new QSpinBox(groupBox);
        mjpegFps->setObjectName(QString::fromUtf8("mjpegFps"));
        mjpegFps->setMinimum(1);
        mjpegFps->setMaximum(30);

        gridLayout_2->addWidget(mjpegFps, 1, 3, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 1, 4, 1, 1);


        verticalLayout_4->addWidget(groupBox);

        tabWidget->addTab(tabScreen, QString());
        tabCustom = new QWidget();
        tabCustom->setObjectName(QString::fromUtf8("tabCustom"));
        verticalLayout = new QVBoxLayout(tabCustom);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_4 = new QLabel(tabCustom);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        verticalLayout->addWidget(label_4);

        label_5 = new QLabel(tabCustom);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        verticalLayout->addWidget(label_5);

        line_2 = new QFrame(tabCustom);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_2);

        stayOnTop = new QCheckBox(tabCustom);
        stayOnTop->setObjectName(QString::fromUtf8("stayOnTop"));

        verticalLayout->addWidget(stayOnTop);

        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        label_6 = new QLabel(tabCustom);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_6);

        customLeft = new QSpinBox(tabCustom);
        customLeft->setObjectName(QString::fromUtf8("customLeft"));
        customLeft->setMinimum(-99999);
        customLeft->setMaximum(99999);

        formLayout_2->setWidget(0, QFormLayout::FieldRole, customLeft);

        label_7 = new QLabel(tabCustom);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_7);

        customTop = new QSpinBox(tabCustom);
        customTop->setObjectName(QString::fromUtf8("customTop"));
        customTop->setMinimum(-99999);
        customTop->setMaximum(99999);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, customTop);

        label_8 = new QLabel(tabCustom);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_8);

        customWidth = new QSpinBox(tabCustom);
        customWidth->setObjectName(QString::fromUtf8("customWidth"));
        customWidth->setMinimum(-99999);
        customWidth->setMaximum(99999);
        customWidth->setValue(1024);

        formLayout_2->setWidget(2, QFormLayout::FieldRole, customWidth);

        label_9 = new QLabel(tabCustom);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, label_9);

        customHeight = new QSpinBox(tabCustom);
        customHeight->setObjectName(QString::fromUtf8("customHeight"));
        customHeight->setMinimum(-99999);
        customHeight->setMaximum(99999);
        customHeight->setValue(768);

        formLayout_2->setWidget(3, QFormLayout::FieldRole, customHeight);


        verticalLayout->addLayout(formLayout_2);

        groupBox_2 = new QGroupBox(tabCustom);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_3 = new QGridLayout(groupBox_2);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        mjpegEnabled2 = new QCheckBox(groupBox_2);
        mjpegEnabled2->setObjectName(QString::fromUtf8("mjpegEnabled2"));

        gridLayout_3->addWidget(mjpegEnabled2, 0, 0, 1, 4);

        label_18 = new QLabel(groupBox_2);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        gridLayout_3->addWidget(label_18, 1, 0, 1, 1);

        mjpegPort2 = new QSpinBox(groupBox_2);
        mjpegPort2->setObjectName(QString::fromUtf8("mjpegPort2"));
        mjpegPort2->setMinimum(1025);
        mjpegPort2->setMaximum(65536);
        mjpegPort2->setValue(8080);

        gridLayout_3->addWidget(mjpegPort2, 1, 1, 1, 1);

        label_19 = new QLabel(groupBox_2);
        label_19->setObjectName(QString::fromUtf8("label_19"));

        gridLayout_3->addWidget(label_19, 1, 2, 1, 1);

        mjpegFps2 = new QSpinBox(groupBox_2);
        mjpegFps2->setObjectName(QString::fromUtf8("mjpegFps2"));
        mjpegFps2->setMinimum(1);
        mjpegFps2->setMaximum(30);

        gridLayout_3->addWidget(mjpegFps2, 1, 3, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_2, 1, 4, 1, 1);


        verticalLayout->addWidget(groupBox_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        tabWidget->addTab(tabCustom, QString());
        tabNetwork = new QWidget();
        tabNetwork->setObjectName(QString::fromUtf8("tabNetwork"));
        verticalLayout_6 = new QVBoxLayout(tabNetwork);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        label_10 = new QLabel(tabNetwork);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        verticalLayout_6->addWidget(label_10);

        line = new QFrame(tabNetwork);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_6->addWidget(line);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_11 = new QLabel(tabNetwork);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        horizontalLayout_2->addWidget(label_11);

        listenPort = new QSpinBox(tabNetwork);
        listenPort->setObjectName(QString::fromUtf8("listenPort"));
        listenPort->setMinimum(1025);
        listenPort->setMaximum(65536);
        listenPort->setValue(7777);

        horizontalLayout_2->addWidget(listenPort);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);


        verticalLayout_6->addLayout(horizontalLayout_2);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer_2);

        tabWidget->addTab(tabNetwork, QString());

        verticalLayout_3->addWidget(tabWidget);


        verticalLayout_2->addWidget(gbSettings);

        tabWidget_2->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        verticalLayout_7 = new QVBoxLayout(tab_2);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        groupBox_3 = new QGroupBox(tab_2);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        verticalLayout_5 = new QVBoxLayout(groupBox_3);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        cbUseOpenGL = new QCheckBox(groupBox_3);
        cbUseOpenGL->setObjectName(QString::fromUtf8("cbUseOpenGL"));

        verticalLayout_5->addWidget(cbUseOpenGL);

        label_12 = new QLabel(groupBox_3);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        verticalLayout_5->addWidget(label_12);


        verticalLayout_7->addWidget(groupBox_3);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_7->addItem(verticalSpacer_3);

        tabWidget_2->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        verticalLayout_10 = new QVBoxLayout(tab_3);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        groupBox_4 = new QGroupBox(tab_3);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        verticalLayout_9 = new QVBoxLayout(groupBox_4);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        label_13 = new QLabel(groupBox_4);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        verticalLayout_9->addWidget(label_13);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_14 = new QLabel(groupBox_4);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        horizontalLayout->addWidget(label_14);

        cacheSlider = new QSlider(groupBox_4);
        cacheSlider->setObjectName(QString::fromUtf8("cacheSlider"));
        cacheSlider->setMinimum(10);
        cacheSlider->setMaximum(2048);
        cacheSlider->setSingleStep(16);
        cacheSlider->setPageStep(64);
        cacheSlider->setOrientation(Qt::Horizontal);
        cacheSlider->setTickPosition(QSlider::TicksBelow);
        cacheSlider->setTickInterval(128);

        horizontalLayout->addWidget(cacheSlider);

        cacheBox = new QSpinBox(groupBox_4);
        cacheBox->setObjectName(QString::fromUtf8("cacheBox"));
        cacheBox->setMinimum(10);
        cacheBox->setMaximum(2048);
        cacheBox->setSingleStep(10);

        horizontalLayout->addWidget(cacheBox);


        verticalLayout_9->addLayout(horizontalLayout);


        verticalLayout_10->addWidget(groupBox_4);

        groupBox_7 = new QGroupBox(tab_3);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        verticalLayout_13 = new QVBoxLayout(groupBox_7);
        verticalLayout_13->setObjectName(QString::fromUtf8("verticalLayout_13"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_15 = new QLabel(groupBox_7);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        horizontalLayout_4->addWidget(label_15);

        diskCacheBox = new QLineEdit(groupBox_7);
        diskCacheBox->setObjectName(QString::fromUtf8("diskCacheBox"));

        horizontalLayout_4->addWidget(diskCacheBox);

        diskCacheBrowseBtn = new QPushButton(groupBox_7);
        diskCacheBrowseBtn->setObjectName(QString::fromUtf8("diskCacheBrowseBtn"));

        horizontalLayout_4->addWidget(diskCacheBrowseBtn);


        verticalLayout_13->addLayout(horizontalLayout_4);

        diskCacheSizeBase = new QWidget(groupBox_7);
        diskCacheSizeBase->setObjectName(QString::fromUtf8("diskCacheSizeBase"));
        horizontalLayout_3 = new QHBoxLayout(diskCacheSizeBase);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_20 = new QLabel(diskCacheSizeBase);
        label_20->setObjectName(QString::fromUtf8("label_20"));

        horizontalLayout_3->addWidget(label_20);

        diskCacheSlider = new QSlider(diskCacheSizeBase);
        diskCacheSlider->setObjectName(QString::fromUtf8("diskCacheSlider"));
        diskCacheSlider->setMinimum(10);
        diskCacheSlider->setMaximum(1048576);
        diskCacheSlider->setSingleStep(256);
        diskCacheSlider->setPageStep(1024);
        diskCacheSlider->setOrientation(Qt::Horizontal);
        diskCacheSlider->setTickPosition(QSlider::TicksBelow);
        diskCacheSlider->setTickInterval(65536);

        horizontalLayout_3->addWidget(diskCacheSlider);

        diskCacheSizeBox = new QSpinBox(diskCacheSizeBase);
        diskCacheSizeBox->setObjectName(QString::fromUtf8("diskCacheSizeBox"));
        diskCacheSizeBox->setMinimum(10);
        diskCacheSizeBox->setMaximum(1048576);
        diskCacheSizeBox->setSingleStep(10);

        horizontalLayout_3->addWidget(diskCacheSizeBox);


        verticalLayout_13->addWidget(diskCacheSizeBase);


        verticalLayout_10->addWidget(groupBox_7);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_10->addItem(verticalSpacer_4);

        tabWidget_2->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        verticalLayout_12 = new QVBoxLayout(tab_4);
        verticalLayout_12->setObjectName(QString::fromUtf8("verticalLayout_12"));
        groupBox_8 = new QGroupBox(tab_4);
        groupBox_8->setObjectName(QString::fromUtf8("groupBox_8"));
        verticalLayout_11 = new QVBoxLayout(groupBox_8);
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        resourceTranslations = new QTableWidget(groupBox_8);
        if (resourceTranslations->columnCount() < 2)
            resourceTranslations->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        resourceTranslations->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        resourceTranslations->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        resourceTranslations->setObjectName(QString::fromUtf8("resourceTranslations"));

        verticalLayout_11->addWidget(resourceTranslations);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        resourceAddBtn = new QPushButton(groupBox_8);
        resourceAddBtn->setObjectName(QString::fromUtf8("resourceAddBtn"));

        horizontalLayout_5->addWidget(resourceAddBtn);

        resourceDelBtn = new QPushButton(groupBox_8);
        resourceDelBtn->setObjectName(QString::fromUtf8("resourceDelBtn"));

        horizontalLayout_5->addWidget(resourceDelBtn);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_4);


        verticalLayout_11->addLayout(horizontalLayout_5);

        label_21 = new QLabel(groupBox_8);
        label_21->setObjectName(QString::fromUtf8("label_21"));

        verticalLayout_11->addWidget(label_21);


        verticalLayout_12->addWidget(groupBox_8);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_12->addItem(verticalSpacer_5);

        tabWidget_2->addTab(tab_4, QString());

        verticalLayout_8->addWidget(tabWidget_2);

        buttonBox = new QDialogButtonBox(SingleOutputSetupDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_8->addWidget(buttonBox);


        retranslateUi(SingleOutputSetupDialog);
        QObject::connect(cacheSlider, SIGNAL(valueChanged(int)), cacheBox, SLOT(setValue(int)));
        QObject::connect(cacheBox, SIGNAL(valueChanged(int)), cacheSlider, SLOT(setValue(int)));

        tabWidget_2->setCurrentIndex(0);
        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SingleOutputSetupDialog);
    } // setupUi

    void retranslateUi(QDialog *SingleOutputSetupDialog)
    {
        SingleOutputSetupDialog->setWindowTitle(QApplication::translate("SingleOutputSetupDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        gbSettings->setTitle(QApplication::translate("SingleOutputSetupDialog", "Output Settings", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SingleOutputSetupDialog", "Name", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("SingleOutputSetupDialog", "(Description Goes Here)", 0, QApplication::UnicodeUTF8));
        cbOutputEnabled->setText(QApplication::translate("SingleOutputSetupDialog", "Output Enabled", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("SingleOutputSetupDialog", "Select a screen from the list below for this output.", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        screenListView->setToolTip(QApplication::translate("SingleOutputSetupDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Tahoma'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">List of screens connected to your computer</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        groupBox->setTitle(QApplication::translate("SingleOutputSetupDialog", "MJPEG Server", 0, QApplication::UnicodeUTF8));
        mjpegEnabled->setText(QApplication::translate("SingleOutputSetupDialog", "MJPEG Server Enabled", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("SingleOutputSetupDialog", "Port: ", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("SingleOutputSetupDialog", "FPS: ", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabScreen), QApplication::translate("SingleOutputSetupDialog", "External Screen", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("SingleOutputSetupDialog", "You may position this output by entering a custom", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("SingleOutputSetupDialog", "location, width and height in pixels, below.", 0, QApplication::UnicodeUTF8));
        stayOnTop->setText(QApplication::translate("SingleOutputSetupDialog", "Keep output on top of all other windows", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("SingleOutputSetupDialog", "Left (X)", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("SingleOutputSetupDialog", "Top (Y)", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("SingleOutputSetupDialog", "Width", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("SingleOutputSetupDialog", "Height", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("SingleOutputSetupDialog", "MJPEG Server", 0, QApplication::UnicodeUTF8));
        mjpegEnabled2->setText(QApplication::translate("SingleOutputSetupDialog", "MJPEG Server Enabled", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("SingleOutputSetupDialog", "Port: ", 0, QApplication::UnicodeUTF8));
        label_19->setText(QApplication::translate("SingleOutputSetupDialog", "FPS: ", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabCustom), QApplication::translate("SingleOutputSetupDialog", "Custom", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("SingleOutputSetupDialog", "Network Outputs allow you to send data over the wire\n"
"to be displayed by another computer using the\n"
"DViz Network Viewer application.", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("SingleOutputSetupDialog", "Accept Incomming Connections on Port:", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabNetwork), QApplication::translate("SingleOutputSetupDialog", "Network", 0, QApplication::UnicodeUTF8));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab), QApplication::translate("SingleOutputSetupDialog", "Output", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("SingleOutputSetupDialog", "Graphics Acceleration", 0, QApplication::UnicodeUTF8));
        cbUseOpenGL->setText(QApplication::translate("SingleOutputSetupDialog", "Use OpenGL Graphics Acceleration", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("SingleOutputSetupDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<img src=\":/data/opengl-logo.png\" align=\"right\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dlg 2';\">The only reason to turn off OpenGL acceleration is if </span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dlg 2';\">the program doesn't run smoothly or freezes with </span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dl"
                        "g 2';\">OpenGL turned on.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_2), QApplication::translate("SingleOutputSetupDialog", "Open GL", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("SingleOutputSetupDialog", "Cache Size", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("SingleOutputSetupDialog", "DViz stores the results of certain graphical operations \n"
"in memory inorder to speed up the display of slides. \n"
"Less memory allocated to that cache means that DViz\n"
"potentially has to redraw items more frequently, \n"
"resulting in slower performance.", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("SingleOutputSetupDialog", "Cache Size:", 0, QApplication::UnicodeUTF8));
        cacheBox->setSuffix(QApplication::translate("SingleOutputSetupDialog", " MB", 0, QApplication::UnicodeUTF8));
        groupBox_7->setTitle(QApplication::translate("SingleOutputSetupDialog", "Disk Cache", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("SingleOutputSetupDialog", "Disk Cache Location:", 0, QApplication::UnicodeUTF8));
        diskCacheBrowseBtn->setText(QApplication::translate("SingleOutputSetupDialog", "Browse", 0, QApplication::UnicodeUTF8));
        label_20->setText(QApplication::translate("SingleOutputSetupDialog", "Disk Cache Size:", 0, QApplication::UnicodeUTF8));
        diskCacheSizeBox->setSuffix(QApplication::translate("SingleOutputSetupDialog", " MB", 0, QApplication::UnicodeUTF8));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_3), QApplication::translate("SingleOutputSetupDialog", "Cache Size", 0, QApplication::UnicodeUTF8));
        groupBox_8->setTitle(QApplication::translate("SingleOutputSetupDialog", "Resource Path Translations", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = resourceTranslations->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("SingleOutputSetupDialog", "From", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = resourceTranslations->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("SingleOutputSetupDialog", "To", 0, QApplication::UnicodeUTF8));
        resourceAddBtn->setText(QApplication::translate("SingleOutputSetupDialog", "Add", 0, QApplication::UnicodeUTF8));
        resourceDelBtn->setText(QApplication::translate("SingleOutputSetupDialog", "Remove", 0, QApplication::UnicodeUTF8));
        label_21->setText(QApplication::translate("SingleOutputSetupDialog", "Resource Path Translations are designed to be used with Network Viewers\n"
"to translate the folders used on the controlling computer to paths \n"
"accessible on the viewer.", 0, QApplication::UnicodeUTF8));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_4), QApplication::translate("SingleOutputSetupDialog", "Path Translations", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SingleOutputSetupDialog: public Ui_SingleOutputSetupDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SINGLEOUTPUTSETUPDIALOG_H
