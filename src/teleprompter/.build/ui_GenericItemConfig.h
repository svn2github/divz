/********************************************************************************
** Form generated from reading ui file 'GenericItemConfig.ui'
**
** Created: Sat Dec 19 21:06:51 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_GENERICITEMCONFIG_H
#define UI_GENERICITEMCONFIG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDial>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFormLayout>
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
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GenericItemConfig
{
public:
    QVBoxLayout *verticalLayout_2;
    QTabWidget *tabWidget;
    QWidget *generalTab;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_8;
    QGridLayout *gridLayout;
    QToolButton *front;
    QToolButton *raise;
    QToolButton *lower;
    QToolButton *back;
    QSpacerItem *horizontalSpacer_7;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_7;
    QGridLayout *gridLayout_3;
    QLabel *label_5;
    QDoubleSpinBox *locationY;
    QDoubleSpinBox *locationX;
    QSpacerItem *horizontalSpacer_8;
    QToolButton *locationReset;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_6;
    QGridLayout *sizeGridLayout;
    QLabel *label_9;
    QDoubleSpinBox *contentHeight;
    QDoubleSpinBox *contentWidth;
    QSpacerItem *horizontalSpacer_12;
    QToolButton *sizeReset;
    QGroupBox *groupBox_5;
    QVBoxLayout *verticalLayout_5;
    QGridLayout *gridLayout_7;
    QToolButton *opacityReset;
    QSpacerItem *horizontalSpacer_14;
    QSpinBox *opacityBox;
    QSlider *opacitySlider;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_4;
    QGridLayout *gridLayout_2;
    QToolButton *save;
    QToolButton *del;
    QSpacerItem *horizontalSpacer_6;
    QSpacerItem *verticalSpacer_2;
    QWidget *outlineTab;
    QVBoxLayout *verticalLayout_9;
    QRadioButton *outlineNone;
    QRadioButton *outlineSolid;
    QFormLayout *formLayout;
    QLabel *label;
    QWidget *outlineColorPlaceholder;
    QLabel *label_2;
    QDoubleSpinBox *outlineBox;
    QWidget *backgroundTab;
    QVBoxLayout *verticalLayout_10;
    QWidget *bgOptNoBg;
    QVBoxLayout *verticalLayout_13;
    QRadioButton *bgNone;
    QRadioButton *bgColor;
    QWidget *bgOptColor;
    QVBoxLayout *verticalLayout_14;
    QHBoxLayout *bgColorPickerLayout;
    QRadioButton *bgGradient;
    QWidget *bgOptGradient;
    QVBoxLayout *verticalLayout_15;
    QComboBox *gradientSelector;
    QRadioButton *bgImage;
    QWidget *bgOptImage;
    QVBoxLayout *verticalLayout_16;
    QGridLayout *gridLayout_8;
    QLineEdit *imageFilenameBox;
    QWidget *imagePreviewWidget;
    QPushButton *imageBrowseButton;
    QRadioButton *bgVideo;
    QWidget *bgOptVideo;
    QVBoxLayout *verticalLayout_17;
    QGridLayout *gridLayout_9;
    QPushButton *videoBrowseButton;
    QWidget *videoPreviewWidget;
    QVBoxLayout *verticalLayout;
    QPushButton *videoPlayButton;
    QSpacerItem *verticalSpacer;
    QLineEdit *videoFilenameBox;
    QLabel *label_12;
    QComboBox *endActionCombo;
    QSpacerItem *verticalSpacer_3;
    QWidget *shadowTab;
    QVBoxLayout *verticalLayout_11;
    QRadioButton *shadowNone;
    QRadioButton *shadowSolid;
    QGroupBox *shadowColorGb;
    QGroupBox *groupBox_7;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_6;
    QSpinBox *alphaBox;
    QSlider *transparencySlider;
    QGroupBox *blurGroup;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QSpinBox *blurBox;
    QSlider *blurSlider;
    QGroupBox *shadowDirectionBox;
    QHBoxLayout *horizontalLayout_5;
    QVBoxLayout *verticalLayout_23;
    QDial *directionDial;
    QSpinBox *directionBox;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_7;
    QSlider *distanceSlider;
    QSpinBox *distanceBox;
    QGroupBox *groupBox_6;
    QVBoxLayout *verticalLayout_18;
    QGridLayout *gridLayout_4;
    QPushButton *shadowOffsetPresetTL;
    QPushButton *shadowOffsetPresetT;
    QPushButton *shadowOffsetPresetTR;
    QPushButton *shadowOffsetPresetL;
    QPushButton *shadowOffsetPresetR;
    QPushButton *shadowOffsetPresetBL;
    QPushButton *shadowOffsetPresetB;
    QPushButton *shadowOffsetPresetBR;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QDoubleSpinBox *shadowXOffsetBox;
    QDoubleSpinBox *shadowYOffsetBox;
    QPushButton *shadowOffsetPreset0;
    QLabel *label_4;
    QSpacerItem *verticalSpacer_4;
    QWidget *reflectionTab;
    QVBoxLayout *verticalLayout_20;
    QGroupBox *gbMirror;
    QVBoxLayout *verticalLayout_12;
    QRadioButton *reflectionNone;
    QRadioButton *reflectionEnabled;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_11;
    QDoubleSpinBox *mirrorOffset;
    QSpacerItem *horizontalSpacer;
    QGroupBox *gbZoomEffect;
    QVBoxLayout *verticalLayout_19;
    QHBoxLayout *horizontalLayout_7;
    QCheckBox *zoomEnabled;
    QLabel *label_10;
    QDoubleSpinBox *zoomFactor;
    QGroupBox *groupBox_9;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_8;
    QSlider *zoomSpeedSlider;
    QSpinBox *zoomSpeedBox;
    QGroupBox *groupBox_10;
    QHBoxLayout *horizontalLayout_8;
    QRadioButton *zoomDirRandom;
    QRadioButton *zoomDirOut;
    QRadioButton *zoomDirIn;
    QCheckBox *zoomLoop;
    QGroupBox *groupBox_11;
    QVBoxLayout *verticalLayout_21;
    QRadioButton *anchorCenter;
    QRadioButton *anchorRandom;
    QRadioButton *anchorOther;
    QHBoxLayout *horizontalLayout_10;
    QComboBox *zoomAnchorCombo;
    QDial *zoomAnchorDial;
    QSpacerItem *verticalSpacer_6;
    QSpacerItem *verticalSpacer_5;
    QHBoxLayout *btnBarLayout;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *btnSave;

    void setupUi(QWidget *GenericItemConfig)
    {
        if (GenericItemConfig->objectName().isEmpty())
            GenericItemConfig->setObjectName(QString::fromUtf8("GenericItemConfig"));
        GenericItemConfig->resize(439, 632);
        verticalLayout_2 = new QVBoxLayout(GenericItemConfig);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        tabWidget = new QTabWidget(GenericItemConfig);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setTabPosition(QTabWidget::North);
        tabWidget->setTabShape(QTabWidget::Rounded);
        tabWidget->setUsesScrollButtons(false);
        generalTab = new QWidget();
        generalTab->setObjectName(QString::fromUtf8("generalTab"));
        verticalLayout_3 = new QVBoxLayout(generalTab);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        groupBox_2 = new QGroupBox(generalTab);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout_8 = new QVBoxLayout(groupBox_2);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        front = new QToolButton(groupBox_2);
        front->setObjectName(QString::fromUtf8("front"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/data/action-order-front.png"), QSize(), QIcon::Normal, QIcon::Off);
        front->setIcon(icon);
        front->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        gridLayout->addWidget(front, 0, 0, 1, 1);

        raise = new QToolButton(groupBox_2);
        raise->setObjectName(QString::fromUtf8("raise"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/data/action-order-raise.png"), QSize(), QIcon::Normal, QIcon::Off);
        raise->setIcon(icon1);
        raise->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        gridLayout->addWidget(raise, 0, 1, 1, 1);

        lower = new QToolButton(groupBox_2);
        lower->setObjectName(QString::fromUtf8("lower"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/data/action-order-lower.png"), QSize(), QIcon::Normal, QIcon::Off);
        lower->setIcon(icon2);
        lower->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        gridLayout->addWidget(lower, 0, 2, 1, 1);

        back = new QToolButton(groupBox_2);
        back->setObjectName(QString::fromUtf8("back"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/data/action-order-back.png"), QSize(), QIcon::Normal, QIcon::Off);
        back->setIcon(icon3);
        back->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        gridLayout->addWidget(back, 0, 3, 1, 1);

        horizontalSpacer_7 = new QSpacerItem(37, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_7, 0, 4, 1, 1);


        verticalLayout_8->addLayout(gridLayout);


        verticalLayout_3->addWidget(groupBox_2);

        groupBox = new QGroupBox(generalTab);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_7 = new QVBoxLayout(groupBox);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_5, 0, 1, 1, 1);

        locationY = new QDoubleSpinBox(groupBox);
        locationY->setObjectName(QString::fromUtf8("locationY"));
        locationY->setMaximum(10000);

        gridLayout_3->addWidget(locationY, 0, 2, 1, 1);

        locationX = new QDoubleSpinBox(groupBox);
        locationX->setObjectName(QString::fromUtf8("locationX"));
        locationX->setMaximum(10000);

        gridLayout_3->addWidget(locationX, 0, 0, 1, 1);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_8, 0, 4, 1, 1);

        locationReset = new QToolButton(groupBox);
        locationReset->setObjectName(QString::fromUtf8("locationReset"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/data/action-delete.png"), QSize(), QIcon::Normal, QIcon::Off);
        locationReset->setIcon(icon4);
        locationReset->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        gridLayout_3->addWidget(locationReset, 0, 3, 1, 1);


        verticalLayout_7->addLayout(gridLayout_3);


        verticalLayout_3->addWidget(groupBox);

        groupBox_4 = new QGroupBox(generalTab);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        verticalLayout_6 = new QVBoxLayout(groupBox_4);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        sizeGridLayout = new QGridLayout();
        sizeGridLayout->setObjectName(QString::fromUtf8("sizeGridLayout"));
        label_9 = new QLabel(groupBox_4);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setAlignment(Qt::AlignCenter);

        sizeGridLayout->addWidget(label_9, 0, 1, 1, 1);

        contentHeight = new QDoubleSpinBox(groupBox_4);
        contentHeight->setObjectName(QString::fromUtf8("contentHeight"));
        contentHeight->setMaximum(10000);

        sizeGridLayout->addWidget(contentHeight, 0, 2, 1, 1);

        contentWidth = new QDoubleSpinBox(groupBox_4);
        contentWidth->setObjectName(QString::fromUtf8("contentWidth"));
        contentWidth->setMaximum(10000);

        sizeGridLayout->addWidget(contentWidth, 0, 0, 1, 1);

        horizontalSpacer_12 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        sizeGridLayout->addItem(horizontalSpacer_12, 0, 4, 1, 1);

        sizeReset = new QToolButton(groupBox_4);
        sizeReset->setObjectName(QString::fromUtf8("sizeReset"));
        sizeReset->setIcon(icon4);
        sizeReset->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        sizeGridLayout->addWidget(sizeReset, 0, 3, 1, 1);


        verticalLayout_6->addLayout(sizeGridLayout);


        verticalLayout_3->addWidget(groupBox_4);

        groupBox_5 = new QGroupBox(generalTab);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        verticalLayout_5 = new QVBoxLayout(groupBox_5);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        gridLayout_7 = new QGridLayout();
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        opacityReset = new QToolButton(groupBox_5);
        opacityReset->setObjectName(QString::fromUtf8("opacityReset"));
        opacityReset->setIcon(icon4);
        opacityReset->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        gridLayout_7->addWidget(opacityReset, 0, 2, 1, 1);

        horizontalSpacer_14 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_7->addItem(horizontalSpacer_14, 0, 3, 1, 1);

        opacityBox = new QSpinBox(groupBox_5);
        opacityBox->setObjectName(QString::fromUtf8("opacityBox"));
        opacityBox->setMaximum(100);

        gridLayout_7->addWidget(opacityBox, 0, 1, 1, 1);

        opacitySlider = new QSlider(groupBox_5);
        opacitySlider->setObjectName(QString::fromUtf8("opacitySlider"));
        opacitySlider->setMaximum(100);
        opacitySlider->setOrientation(Qt::Horizontal);

        gridLayout_7->addWidget(opacitySlider, 0, 0, 1, 1);


        verticalLayout_5->addLayout(gridLayout_7);


        verticalLayout_3->addWidget(groupBox_5);

        groupBox_3 = new QGroupBox(generalTab);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        verticalLayout_4 = new QVBoxLayout(groupBox_3);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        save = new QToolButton(groupBox_3);
        save->setObjectName(QString::fromUtf8("save"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/data/action-save.png"), QSize(), QIcon::Normal, QIcon::Off);
        save->setIcon(icon5);
        save->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        gridLayout_2->addWidget(save, 0, 0, 1, 1);

        del = new QToolButton(groupBox_3);
        del->setObjectName(QString::fromUtf8("del"));
        del->setIcon(icon4);
        del->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        gridLayout_2->addWidget(del, 0, 1, 1, 1);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_6, 0, 2, 1, 1);


        verticalLayout_4->addLayout(gridLayout_2);


        verticalLayout_3->addWidget(groupBox_3);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_2);

        tabWidget->addTab(generalTab, QString());
        outlineTab = new QWidget();
        outlineTab->setObjectName(QString::fromUtf8("outlineTab"));
        verticalLayout_9 = new QVBoxLayout(outlineTab);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        outlineNone = new QRadioButton(outlineTab);
        outlineNone->setObjectName(QString::fromUtf8("outlineNone"));

        verticalLayout_9->addWidget(outlineNone);

        outlineSolid = new QRadioButton(outlineTab);
        outlineSolid->setObjectName(QString::fromUtf8("outlineSolid"));
        outlineSolid->setChecked(true);

        verticalLayout_9->addWidget(outlineSolid);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        formLayout->setLabelAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        label = new QLabel(outlineTab);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        outlineColorPlaceholder = new QWidget(outlineTab);
        outlineColorPlaceholder->setObjectName(QString::fromUtf8("outlineColorPlaceholder"));

        formLayout->setWidget(0, QFormLayout::FieldRole, outlineColorPlaceholder);

        label_2 = new QLabel(outlineTab);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        outlineBox = new QDoubleSpinBox(outlineTab);
        outlineBox->setObjectName(QString::fromUtf8("outlineBox"));

        formLayout->setWidget(1, QFormLayout::FieldRole, outlineBox);


        verticalLayout_9->addLayout(formLayout);

        tabWidget->addTab(outlineTab, QString());
        backgroundTab = new QWidget();
        backgroundTab->setObjectName(QString::fromUtf8("backgroundTab"));
        verticalLayout_10 = new QVBoxLayout(backgroundTab);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        bgOptNoBg = new QWidget(backgroundTab);
        bgOptNoBg->setObjectName(QString::fromUtf8("bgOptNoBg"));
        verticalLayout_13 = new QVBoxLayout(bgOptNoBg);
        verticalLayout_13->setMargin(0);
        verticalLayout_13->setObjectName(QString::fromUtf8("verticalLayout_13"));

        verticalLayout_10->addWidget(bgOptNoBg);

        bgNone = new QRadioButton(backgroundTab);
        bgNone->setObjectName(QString::fromUtf8("bgNone"));

        verticalLayout_10->addWidget(bgNone);

        bgColor = new QRadioButton(backgroundTab);
        bgColor->setObjectName(QString::fromUtf8("bgColor"));
        bgColor->setChecked(true);

        verticalLayout_10->addWidget(bgColor);

        bgOptColor = new QWidget(backgroundTab);
        bgOptColor->setObjectName(QString::fromUtf8("bgOptColor"));
        verticalLayout_14 = new QVBoxLayout(bgOptColor);
        verticalLayout_14->setMargin(0);
        verticalLayout_14->setObjectName(QString::fromUtf8("verticalLayout_14"));
        bgColorPickerLayout = new QHBoxLayout();
        bgColorPickerLayout->setObjectName(QString::fromUtf8("bgColorPickerLayout"));

        verticalLayout_14->addLayout(bgColorPickerLayout);


        verticalLayout_10->addWidget(bgOptColor);

        bgGradient = new QRadioButton(backgroundTab);
        bgGradient->setObjectName(QString::fromUtf8("bgGradient"));

        verticalLayout_10->addWidget(bgGradient);

        bgOptGradient = new QWidget(backgroundTab);
        bgOptGradient->setObjectName(QString::fromUtf8("bgOptGradient"));
        verticalLayout_15 = new QVBoxLayout(bgOptGradient);
        verticalLayout_15->setMargin(0);
        verticalLayout_15->setObjectName(QString::fromUtf8("verticalLayout_15"));
        gradientSelector = new QComboBox(bgOptGradient);
        gradientSelector->setObjectName(QString::fromUtf8("gradientSelector"));

        verticalLayout_15->addWidget(gradientSelector);


        verticalLayout_10->addWidget(bgOptGradient);

        bgImage = new QRadioButton(backgroundTab);
        bgImage->setObjectName(QString::fromUtf8("bgImage"));

        verticalLayout_10->addWidget(bgImage);

        bgOptImage = new QWidget(backgroundTab);
        bgOptImage->setObjectName(QString::fromUtf8("bgOptImage"));
        verticalLayout_16 = new QVBoxLayout(bgOptImage);
        verticalLayout_16->setMargin(0);
        verticalLayout_16->setObjectName(QString::fromUtf8("verticalLayout_16"));
        gridLayout_8 = new QGridLayout();
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        imageFilenameBox = new QLineEdit(bgOptImage);
        imageFilenameBox->setObjectName(QString::fromUtf8("imageFilenameBox"));

        gridLayout_8->addWidget(imageFilenameBox, 1, 0, 1, 1);

        imagePreviewWidget = new QWidget(bgOptImage);
        imagePreviewWidget->setObjectName(QString::fromUtf8("imagePreviewWidget"));

        gridLayout_8->addWidget(imagePreviewWidget, 2, 0, 2, 2);

        imageBrowseButton = new QPushButton(bgOptImage);
        imageBrowseButton->setObjectName(QString::fromUtf8("imageBrowseButton"));

        gridLayout_8->addWidget(imageBrowseButton, 1, 1, 1, 1);


        verticalLayout_16->addLayout(gridLayout_8);


        verticalLayout_10->addWidget(bgOptImage);

        bgVideo = new QRadioButton(backgroundTab);
        bgVideo->setObjectName(QString::fromUtf8("bgVideo"));

        verticalLayout_10->addWidget(bgVideo);

        bgOptVideo = new QWidget(backgroundTab);
        bgOptVideo->setObjectName(QString::fromUtf8("bgOptVideo"));
        verticalLayout_17 = new QVBoxLayout(bgOptVideo);
        verticalLayout_17->setMargin(0);
        verticalLayout_17->setObjectName(QString::fromUtf8("verticalLayout_17"));
        gridLayout_9 = new QGridLayout();
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        videoBrowseButton = new QPushButton(bgOptVideo);
        videoBrowseButton->setObjectName(QString::fromUtf8("videoBrowseButton"));

        gridLayout_9->addWidget(videoBrowseButton, 1, 1, 1, 1);

        videoPreviewWidget = new QWidget(bgOptVideo);
        videoPreviewWidget->setObjectName(QString::fromUtf8("videoPreviewWidget"));

        gridLayout_9->addWidget(videoPreviewWidget, 3, 0, 2, 1);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        videoPlayButton = new QPushButton(bgOptVideo);
        videoPlayButton->setObjectName(QString::fromUtf8("videoPlayButton"));

        verticalLayout->addWidget(videoPlayButton);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        gridLayout_9->addLayout(verticalLayout, 3, 1, 2, 1);

        videoFilenameBox = new QLineEdit(bgOptVideo);
        videoFilenameBox->setObjectName(QString::fromUtf8("videoFilenameBox"));

        gridLayout_9->addWidget(videoFilenameBox, 1, 0, 1, 1);

        label_12 = new QLabel(bgOptVideo);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_9->addWidget(label_12, 2, 0, 1, 1);

        endActionCombo = new QComboBox(bgOptVideo);
        endActionCombo->setObjectName(QString::fromUtf8("endActionCombo"));

        gridLayout_9->addWidget(endActionCombo, 2, 1, 1, 1);


        verticalLayout_17->addLayout(gridLayout_9);


        verticalLayout_10->addWidget(bgOptVideo);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_10->addItem(verticalSpacer_3);

        tabWidget->addTab(backgroundTab, QString());
        shadowTab = new QWidget();
        shadowTab->setObjectName(QString::fromUtf8("shadowTab"));
        verticalLayout_11 = new QVBoxLayout(shadowTab);
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        shadowNone = new QRadioButton(shadowTab);
        shadowNone->setObjectName(QString::fromUtf8("shadowNone"));
        shadowNone->setChecked(true);

        verticalLayout_11->addWidget(shadowNone);

        shadowSolid = new QRadioButton(shadowTab);
        shadowSolid->setObjectName(QString::fromUtf8("shadowSolid"));

        verticalLayout_11->addWidget(shadowSolid);

        shadowColorGb = new QGroupBox(shadowTab);
        shadowColorGb->setObjectName(QString::fromUtf8("shadowColorGb"));

        verticalLayout_11->addWidget(shadowColorGb);

        groupBox_7 = new QGroupBox(shadowTab);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        horizontalLayout_3 = new QHBoxLayout(groupBox_7);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_6 = new QLabel(groupBox_7);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        horizontalLayout_3->addWidget(label_6);

        alphaBox = new QSpinBox(groupBox_7);
        alphaBox->setObjectName(QString::fromUtf8("alphaBox"));
        alphaBox->setMaximum(100);

        horizontalLayout_3->addWidget(alphaBox);

        transparencySlider = new QSlider(groupBox_7);
        transparencySlider->setObjectName(QString::fromUtf8("transparencySlider"));
        transparencySlider->setMaximum(100);
        transparencySlider->setOrientation(Qt::Horizontal);

        horizontalLayout_3->addWidget(transparencySlider);


        verticalLayout_11->addWidget(groupBox_7);

        blurGroup = new QGroupBox(shadowTab);
        blurGroup->setObjectName(QString::fromUtf8("blurGroup"));
        horizontalLayout_2 = new QHBoxLayout(blurGroup);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_3 = new QLabel(blurGroup);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_2->addWidget(label_3);

        blurBox = new QSpinBox(blurGroup);
        blurBox->setObjectName(QString::fromUtf8("blurBox"));
        blurBox->setMaximum(16);

        horizontalLayout_2->addWidget(blurBox);

        blurSlider = new QSlider(blurGroup);
        blurSlider->setObjectName(QString::fromUtf8("blurSlider"));
        blurSlider->setMaximum(16);
        blurSlider->setPageStep(4);
        blurSlider->setOrientation(Qt::Horizontal);

        horizontalLayout_2->addWidget(blurSlider);


        verticalLayout_11->addWidget(blurGroup);

        shadowDirectionBox = new QGroupBox(shadowTab);
        shadowDirectionBox->setObjectName(QString::fromUtf8("shadowDirectionBox"));
        horizontalLayout_5 = new QHBoxLayout(shadowDirectionBox);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        verticalLayout_23 = new QVBoxLayout();
        verticalLayout_23->setObjectName(QString::fromUtf8("verticalLayout_23"));
        directionDial = new QDial(shadowDirectionBox);
        directionDial->setObjectName(QString::fromUtf8("directionDial"));
        directionDial->setMaximum(360);
        directionDial->setPageStep(22);
        directionDial->setInvertedAppearance(false);
        directionDial->setInvertedControls(false);
        directionDial->setWrapping(true);
        directionDial->setNotchTarget(22.5);
        directionDial->setNotchesVisible(true);

        verticalLayout_23->addWidget(directionDial);

        directionBox = new QSpinBox(shadowDirectionBox);
        directionBox->setObjectName(QString::fromUtf8("directionBox"));
        directionBox->setAlignment(Qt::AlignCenter);

        verticalLayout_23->addWidget(directionBox);


        horizontalLayout_5->addLayout(verticalLayout_23);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(0, -1, -1, -1);
        label_7 = new QLabel(shadowDirectionBox);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout_6->addWidget(label_7);

        distanceSlider = new QSlider(shadowDirectionBox);
        distanceSlider->setObjectName(QString::fromUtf8("distanceSlider"));
        distanceSlider->setOrientation(Qt::Horizontal);

        horizontalLayout_6->addWidget(distanceSlider);

        distanceBox = new QSpinBox(shadowDirectionBox);
        distanceBox->setObjectName(QString::fromUtf8("distanceBox"));

        horizontalLayout_6->addWidget(distanceBox);


        horizontalLayout_5->addLayout(horizontalLayout_6);


        verticalLayout_11->addWidget(shadowDirectionBox);

        groupBox_6 = new QGroupBox(shadowTab);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        verticalLayout_18 = new QVBoxLayout(groupBox_6);
        verticalLayout_18->setObjectName(QString::fromUtf8("verticalLayout_18"));
        gridLayout_4 = new QGridLayout();
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        shadowOffsetPresetTL = new QPushButton(groupBox_6);
        shadowOffsetPresetTL->setObjectName(QString::fromUtf8("shadowOffsetPresetTL"));

        gridLayout_4->addWidget(shadowOffsetPresetTL, 1, 0, 1, 1);

        shadowOffsetPresetT = new QPushButton(groupBox_6);
        shadowOffsetPresetT->setObjectName(QString::fromUtf8("shadowOffsetPresetT"));

        gridLayout_4->addWidget(shadowOffsetPresetT, 1, 1, 1, 1);

        shadowOffsetPresetTR = new QPushButton(groupBox_6);
        shadowOffsetPresetTR->setObjectName(QString::fromUtf8("shadowOffsetPresetTR"));

        gridLayout_4->addWidget(shadowOffsetPresetTR, 1, 2, 1, 1);

        shadowOffsetPresetL = new QPushButton(groupBox_6);
        shadowOffsetPresetL->setObjectName(QString::fromUtf8("shadowOffsetPresetL"));

        gridLayout_4->addWidget(shadowOffsetPresetL, 2, 0, 1, 1);

        shadowOffsetPresetR = new QPushButton(groupBox_6);
        shadowOffsetPresetR->setObjectName(QString::fromUtf8("shadowOffsetPresetR"));

        gridLayout_4->addWidget(shadowOffsetPresetR, 2, 2, 1, 1);

        shadowOffsetPresetBL = new QPushButton(groupBox_6);
        shadowOffsetPresetBL->setObjectName(QString::fromUtf8("shadowOffsetPresetBL"));

        gridLayout_4->addWidget(shadowOffsetPresetBL, 3, 0, 1, 1);

        shadowOffsetPresetB = new QPushButton(groupBox_6);
        shadowOffsetPresetB->setObjectName(QString::fromUtf8("shadowOffsetPresetB"));

        gridLayout_4->addWidget(shadowOffsetPresetB, 3, 1, 1, 1);

        shadowOffsetPresetBR = new QPushButton(groupBox_6);
        shadowOffsetPresetBR->setObjectName(QString::fromUtf8("shadowOffsetPresetBR"));

        gridLayout_4->addWidget(shadowOffsetPresetBR, 3, 2, 1, 1);

        widget = new QWidget(groupBox_6);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setMargin(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        shadowXOffsetBox = new QDoubleSpinBox(widget);
        shadowXOffsetBox->setObjectName(QString::fromUtf8("shadowXOffsetBox"));
        shadowXOffsetBox->setAlignment(Qt::AlignCenter);
        shadowXOffsetBox->setMinimum(-99.99);
        shadowXOffsetBox->setSingleStep(0.5);

        horizontalLayout->addWidget(shadowXOffsetBox);

        shadowYOffsetBox = new QDoubleSpinBox(widget);
        shadowYOffsetBox->setObjectName(QString::fromUtf8("shadowYOffsetBox"));
        shadowYOffsetBox->setFrame(true);
        shadowYOffsetBox->setAlignment(Qt::AlignCenter);
        shadowYOffsetBox->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
        shadowYOffsetBox->setMinimum(-100);
        shadowYOffsetBox->setMaximum(100);
        shadowYOffsetBox->setSingleStep(0.5);

        horizontalLayout->addWidget(shadowYOffsetBox);


        gridLayout_4->addWidget(widget, 0, 1, 1, 1);

        shadowOffsetPreset0 = new QPushButton(groupBox_6);
        shadowOffsetPreset0->setObjectName(QString::fromUtf8("shadowOffsetPreset0"));

        gridLayout_4->addWidget(shadowOffsetPreset0, 2, 1, 1, 1);

        label_4 = new QLabel(groupBox_6);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_4->addWidget(label_4, 0, 0, 1, 1);


        verticalLayout_18->addLayout(gridLayout_4);


        verticalLayout_11->addWidget(groupBox_6);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_11->addItem(verticalSpacer_4);

        tabWidget->addTab(shadowTab, QString());
        reflectionTab = new QWidget();
        reflectionTab->setObjectName(QString::fromUtf8("reflectionTab"));
        verticalLayout_20 = new QVBoxLayout(reflectionTab);
        verticalLayout_20->setObjectName(QString::fromUtf8("verticalLayout_20"));
        gbMirror = new QGroupBox(reflectionTab);
        gbMirror->setObjectName(QString::fromUtf8("gbMirror"));
        verticalLayout_12 = new QVBoxLayout(gbMirror);
        verticalLayout_12->setObjectName(QString::fromUtf8("verticalLayout_12"));
        reflectionNone = new QRadioButton(gbMirror);
        reflectionNone->setObjectName(QString::fromUtf8("reflectionNone"));
        reflectionNone->setChecked(true);

        verticalLayout_12->addWidget(reflectionNone);

        reflectionEnabled = new QRadioButton(gbMirror);
        reflectionEnabled->setObjectName(QString::fromUtf8("reflectionEnabled"));

        verticalLayout_12->addWidget(reflectionEnabled);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_11 = new QLabel(gbMirror);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        horizontalLayout_4->addWidget(label_11);

        mirrorOffset = new QDoubleSpinBox(gbMirror);
        mirrorOffset->setObjectName(QString::fromUtf8("mirrorOffset"));
        mirrorOffset->setMinimum(-99.99);
        mirrorOffset->setSingleStep(0.5);

        horizontalLayout_4->addWidget(mirrorOffset);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);


        verticalLayout_12->addLayout(horizontalLayout_4);


        verticalLayout_20->addWidget(gbMirror);

        gbZoomEffect = new QGroupBox(reflectionTab);
        gbZoomEffect->setObjectName(QString::fromUtf8("gbZoomEffect"));
        verticalLayout_19 = new QVBoxLayout(gbZoomEffect);
        verticalLayout_19->setObjectName(QString::fromUtf8("verticalLayout_19"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        zoomEnabled = new QCheckBox(gbZoomEffect);
        zoomEnabled->setObjectName(QString::fromUtf8("zoomEnabled"));

        horizontalLayout_7->addWidget(zoomEnabled);

        label_10 = new QLabel(gbZoomEffect);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_7->addWidget(label_10);

        zoomFactor = new QDoubleSpinBox(gbZoomEffect);
        zoomFactor->setObjectName(QString::fromUtf8("zoomFactor"));
        zoomFactor->setAlignment(Qt::AlignCenter);
        zoomFactor->setDecimals(2);
        zoomFactor->setMinimum(1.01);
        zoomFactor->setMaximum(10);
        zoomFactor->setValue(2);

        horizontalLayout_7->addWidget(zoomFactor);


        verticalLayout_19->addLayout(horizontalLayout_7);

        groupBox_9 = new QGroupBox(gbZoomEffect);
        groupBox_9->setObjectName(QString::fromUtf8("groupBox_9"));
        horizontalLayout_9 = new QHBoxLayout(groupBox_9);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        label_8 = new QLabel(groupBox_9);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_9->addWidget(label_8);

        zoomSpeedSlider = new QSlider(groupBox_9);
        zoomSpeedSlider->setObjectName(QString::fromUtf8("zoomSpeedSlider"));
        zoomSpeedSlider->setMinimum(1);
        zoomSpeedSlider->setMaximum(100);
        zoomSpeedSlider->setOrientation(Qt::Horizontal);

        horizontalLayout_9->addWidget(zoomSpeedSlider);

        zoomSpeedBox = new QSpinBox(groupBox_9);
        zoomSpeedBox->setObjectName(QString::fromUtf8("zoomSpeedBox"));
        zoomSpeedBox->setMinimum(1);
        zoomSpeedBox->setMaximum(100);

        horizontalLayout_9->addWidget(zoomSpeedBox);


        verticalLayout_19->addWidget(groupBox_9);

        groupBox_10 = new QGroupBox(gbZoomEffect);
        groupBox_10->setObjectName(QString::fromUtf8("groupBox_10"));
        horizontalLayout_8 = new QHBoxLayout(groupBox_10);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        zoomDirRandom = new QRadioButton(groupBox_10);
        zoomDirRandom->setObjectName(QString::fromUtf8("zoomDirRandom"));

        horizontalLayout_8->addWidget(zoomDirRandom);

        zoomDirOut = new QRadioButton(groupBox_10);
        zoomDirOut->setObjectName(QString::fromUtf8("zoomDirOut"));

        horizontalLayout_8->addWidget(zoomDirOut);

        zoomDirIn = new QRadioButton(groupBox_10);
        zoomDirIn->setObjectName(QString::fromUtf8("zoomDirIn"));

        horizontalLayout_8->addWidget(zoomDirIn);

        zoomLoop = new QCheckBox(groupBox_10);
        zoomLoop->setObjectName(QString::fromUtf8("zoomLoop"));

        horizontalLayout_8->addWidget(zoomLoop);


        verticalLayout_19->addWidget(groupBox_10);

        groupBox_11 = new QGroupBox(gbZoomEffect);
        groupBox_11->setObjectName(QString::fromUtf8("groupBox_11"));
        verticalLayout_21 = new QVBoxLayout(groupBox_11);
        verticalLayout_21->setObjectName(QString::fromUtf8("verticalLayout_21"));
        anchorCenter = new QRadioButton(groupBox_11);
        anchorCenter->setObjectName(QString::fromUtf8("anchorCenter"));

        verticalLayout_21->addWidget(anchorCenter);

        anchorRandom = new QRadioButton(groupBox_11);
        anchorRandom->setObjectName(QString::fromUtf8("anchorRandom"));

        verticalLayout_21->addWidget(anchorRandom);

        anchorOther = new QRadioButton(groupBox_11);
        anchorOther->setObjectName(QString::fromUtf8("anchorOther"));

        verticalLayout_21->addWidget(anchorOther);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        zoomAnchorCombo = new QComboBox(groupBox_11);
        zoomAnchorCombo->setObjectName(QString::fromUtf8("zoomAnchorCombo"));
        zoomAnchorCombo->setEnabled(false);

        horizontalLayout_10->addWidget(zoomAnchorCombo);

        zoomAnchorDial = new QDial(groupBox_11);
        zoomAnchorDial->setObjectName(QString::fromUtf8("zoomAnchorDial"));
        zoomAnchorDial->setEnabled(false);
        zoomAnchorDial->setMaximum(8);
        zoomAnchorDial->setSingleStep(1);
        zoomAnchorDial->setPageStep(1);
        zoomAnchorDial->setWrapping(true);
        zoomAnchorDial->setNotchTarget(1);
        zoomAnchorDial->setNotchesVisible(true);

        horizontalLayout_10->addWidget(zoomAnchorDial);


        verticalLayout_21->addLayout(horizontalLayout_10);

        verticalSpacer_6 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_21->addItem(verticalSpacer_6);


        verticalLayout_19->addWidget(groupBox_11);


        verticalLayout_20->addWidget(gbZoomEffect);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_20->addItem(verticalSpacer_5);

        tabWidget->addTab(reflectionTab, QString());

        verticalLayout_2->addWidget(tabWidget);

        btnBarLayout = new QHBoxLayout();
        btnBarLayout->setObjectName(QString::fromUtf8("btnBarLayout"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        btnBarLayout->addItem(horizontalSpacer_2);

        btnSave = new QPushButton(GenericItemConfig);
        btnSave->setObjectName(QString::fromUtf8("btnSave"));

        btnBarLayout->addWidget(btnSave);


        verticalLayout_2->addLayout(btnBarLayout);


        retranslateUi(GenericItemConfig);
        QObject::connect(opacitySlider, SIGNAL(valueChanged(int)), opacityBox, SLOT(setValue(int)));
        QObject::connect(opacityBox, SIGNAL(valueChanged(int)), opacitySlider, SLOT(setValue(int)));
        QObject::connect(blurSlider, SIGNAL(valueChanged(int)), blurBox, SLOT(setValue(int)));
        QObject::connect(blurBox, SIGNAL(valueChanged(int)), blurSlider, SLOT(setValue(int)));
        QObject::connect(alphaBox, SIGNAL(valueChanged(int)), transparencySlider, SLOT(setValue(int)));
        QObject::connect(transparencySlider, SIGNAL(valueChanged(int)), alphaBox, SLOT(setValue(int)));
        QObject::connect(zoomEnabled, SIGNAL(toggled(bool)), groupBox_10, SLOT(setEnabled(bool)));
        QObject::connect(zoomEnabled, SIGNAL(toggled(bool)), groupBox_11, SLOT(setEnabled(bool)));
        QObject::connect(directionDial, SIGNAL(valueChanged(int)), directionBox, SLOT(setValue(int)));
        QObject::connect(directionBox, SIGNAL(valueChanged(int)), directionDial, SLOT(setValue(int)));
        QObject::connect(distanceSlider, SIGNAL(valueChanged(int)), distanceBox, SLOT(setValue(int)));
        QObject::connect(distanceBox, SIGNAL(valueChanged(int)), distanceSlider, SLOT(setValue(int)));
        QObject::connect(zoomSpeedSlider, SIGNAL(valueChanged(int)), zoomSpeedBox, SLOT(setValue(int)));
        QObject::connect(zoomSpeedBox, SIGNAL(valueChanged(int)), zoomSpeedSlider, SLOT(setValue(int)));
        QObject::connect(zoomEnabled, SIGNAL(toggled(bool)), groupBox_9, SLOT(setEnabled(bool)));
        QObject::connect(anchorOther, SIGNAL(toggled(bool)), zoomAnchorCombo, SLOT(setEnabled(bool)));
        QObject::connect(anchorOther, SIGNAL(toggled(bool)), zoomAnchorDial, SLOT(setEnabled(bool)));
        QObject::connect(zoomAnchorCombo, SIGNAL(currentIndexChanged(int)), zoomAnchorDial, SLOT(setValue(int)));
        QObject::connect(zoomAnchorDial, SIGNAL(valueChanged(int)), zoomAnchorCombo, SLOT(setCurrentIndex(int)));

        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(GenericItemConfig);
    } // setupUi

    void retranslateUi(QWidget *GenericItemConfig)
    {
        GenericItemConfig->setWindowTitle(QApplication::translate("GenericItemConfig", "Item Properties", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("GenericItemConfig", "Stacking", 0, QApplication::UnicodeUTF8));
        front->setText(QApplication::translate("GenericItemConfig", "To Front", 0, QApplication::UnicodeUTF8));
        raise->setText(QApplication::translate("GenericItemConfig", "Raise", 0, QApplication::UnicodeUTF8));
        lower->setText(QApplication::translate("GenericItemConfig", "Lower", 0, QApplication::UnicodeUTF8));
        back->setText(QApplication::translate("GenericItemConfig", "To Back", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("GenericItemConfig", "Location", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("GenericItemConfig", "x", 0, QApplication::UnicodeUTF8));
        locationReset->setText(QApplication::translate("GenericItemConfig", "Reset", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("GenericItemConfig", "Size", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("GenericItemConfig", "x", 0, QApplication::UnicodeUTF8));
        sizeReset->setText(QApplication::translate("GenericItemConfig", "Reset", 0, QApplication::UnicodeUTF8));
        groupBox_5->setTitle(QApplication::translate("GenericItemConfig", "Transparency", 0, QApplication::UnicodeUTF8));
        opacityReset->setText(QApplication::translate("GenericItemConfig", "Reset", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("GenericItemConfig", "Operations", 0, QApplication::UnicodeUTF8));
        save->setText(QApplication::translate("GenericItemConfig", "Save As Image ...", 0, QApplication::UnicodeUTF8));
        del->setText(QApplication::translate("GenericItemConfig", "Remove", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(generalTab), QApplication::translate("GenericItemConfig", "Basic", 0, QApplication::UnicodeUTF8));
        outlineNone->setText(QApplication::translate("GenericItemConfig", "No Outline", 0, QApplication::UnicodeUTF8));
        outlineSolid->setText(QApplication::translate("GenericItemConfig", "Solid Color", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("GenericItemConfig", "Line Color", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("GenericItemConfig", "Line Width", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(outlineTab), QApplication::translate("GenericItemConfig", "Line", 0, QApplication::UnicodeUTF8));
        bgNone->setText(QApplication::translate("GenericItemConfig", "No Background", 0, QApplication::UnicodeUTF8));
        bgColor->setText(QApplication::translate("GenericItemConfig", "Solid Color", 0, QApplication::UnicodeUTF8));
        bgGradient->setText(QApplication::translate("GenericItemConfig", "Gradient Fill", 0, QApplication::UnicodeUTF8));
        bgImage->setText(QApplication::translate("GenericItemConfig", "Picture from File", 0, QApplication::UnicodeUTF8));
        imageBrowseButton->setText(QApplication::translate("GenericItemConfig", "Browse...", 0, QApplication::UnicodeUTF8));
        bgVideo->setText(QApplication::translate("GenericItemConfig", "Video from File", 0, QApplication::UnicodeUTF8));
        videoBrowseButton->setText(QApplication::translate("GenericItemConfig", "Browse...", 0, QApplication::UnicodeUTF8));
        videoPlayButton->setText(QApplication::translate("GenericItemConfig", "Play", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("GenericItemConfig", "Video End Action:", 0, QApplication::UnicodeUTF8));
        endActionCombo->clear();
        endActionCombo->insertItems(0, QStringList()
         << QApplication::translate("GenericItemConfig", "Loop Video", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("GenericItemConfig", "Stop Video", 0, QApplication::UnicodeUTF8)
        );
        tabWidget->setTabText(tabWidget->indexOf(backgroundTab), QApplication::translate("GenericItemConfig", "Background", 0, QApplication::UnicodeUTF8));
        shadowNone->setText(QApplication::translate("GenericItemConfig", "No Shadow", 0, QApplication::UnicodeUTF8));
        shadowSolid->setText(QApplication::translate("GenericItemConfig", "Solid Shadow", 0, QApplication::UnicodeUTF8));
        shadowColorGb->setTitle(QApplication::translate("GenericItemConfig", "Color", 0, QApplication::UnicodeUTF8));
        groupBox_7->setTitle(QApplication::translate("GenericItemConfig", "Transparency", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("GenericItemConfig", "Transparency:", 0, QApplication::UnicodeUTF8));
        alphaBox->setSuffix(QApplication::translate("GenericItemConfig", "%", 0, QApplication::UnicodeUTF8));
        blurGroup->setTitle(QApplication::translate("GenericItemConfig", "Blur Radius", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("GenericItemConfig", "Blur Radius: ", 0, QApplication::UnicodeUTF8));
        blurBox->setSpecialValueText(QApplication::translate("GenericItemConfig", "(No Blur)", 0, QApplication::UnicodeUTF8));
        shadowDirectionBox->setTitle(QApplication::translate("GenericItemConfig", "Shadow Distance and Direction", 0, QApplication::UnicodeUTF8));
        directionBox->setSuffix(QApplication::translate("GenericItemConfig", " deg", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("GenericItemConfig", "Distance:", 0, QApplication::UnicodeUTF8));
        distanceBox->setSuffix(QApplication::translate("GenericItemConfig", " px", 0, QApplication::UnicodeUTF8));
        groupBox_6->setTitle(QApplication::translate("GenericItemConfig", "Shadow Position Presets", 0, QApplication::UnicodeUTF8));
        shadowOffsetPresetTL->setText(QApplication::translate("GenericItemConfig", "Top Left", 0, QApplication::UnicodeUTF8));
        shadowOffsetPresetT->setText(QApplication::translate("GenericItemConfig", "Top", 0, QApplication::UnicodeUTF8));
        shadowOffsetPresetTR->setText(QApplication::translate("GenericItemConfig", "Top Right", 0, QApplication::UnicodeUTF8));
        shadowOffsetPresetL->setText(QApplication::translate("GenericItemConfig", "Left", 0, QApplication::UnicodeUTF8));
        shadowOffsetPresetR->setText(QApplication::translate("GenericItemConfig", "Right", 0, QApplication::UnicodeUTF8));
        shadowOffsetPresetBL->setText(QApplication::translate("GenericItemConfig", "Bottom Left", 0, QApplication::UnicodeUTF8));
        shadowOffsetPresetB->setText(QApplication::translate("GenericItemConfig", "Bottom", 0, QApplication::UnicodeUTF8));
        shadowOffsetPresetBR->setText(QApplication::translate("GenericItemConfig", "Bottom Right", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        shadowXOffsetBox->setToolTip(QApplication::translate("GenericItemConfig", "Horizontal position of the shadow, relative to the item", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        shadowYOffsetBox->setToolTip(QApplication::translate("GenericItemConfig", "Vertical position of the shadow, relative to the item", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        shadowOffsetPreset0->setText(QApplication::translate("GenericItemConfig", "Center", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("GenericItemConfig", "Shadow Distance:", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(shadowTab), QApplication::translate("GenericItemConfig", "Shadow", 0, QApplication::UnicodeUTF8));
        gbMirror->setTitle(QApplication::translate("GenericItemConfig", "Mirror", 0, QApplication::UnicodeUTF8));
        reflectionNone->setText(QApplication::translate("GenericItemConfig", "No Reflection", 0, QApplication::UnicodeUTF8));
        reflectionEnabled->setText(QApplication::translate("GenericItemConfig", "Fancy Reflection", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("GenericItemConfig", "Mirror Offset", 0, QApplication::UnicodeUTF8));
        gbZoomEffect->setTitle(QApplication::translate("GenericItemConfig", "Zoom Effect", 0, QApplication::UnicodeUTF8));
        zoomEnabled->setText(QApplication::translate("GenericItemConfig", "Enable Smooth Zoom Effect", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("GenericItemConfig", "Max Size:", 0, QApplication::UnicodeUTF8));
        zoomFactor->setPrefix(QString());
        zoomFactor->setSuffix(QApplication::translate("GenericItemConfig", "x", 0, QApplication::UnicodeUTF8));
        groupBox_9->setTitle(QApplication::translate("GenericItemConfig", "Speed", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("GenericItemConfig", "Speed Factor:", 0, QApplication::UnicodeUTF8));
        zoomSpeedBox->setSuffix(QApplication::translate("GenericItemConfig", "%", 0, QApplication::UnicodeUTF8));
        groupBox_10->setTitle(QApplication::translate("GenericItemConfig", "Direction", 0, QApplication::UnicodeUTF8));
        zoomDirRandom->setText(QApplication::translate("GenericItemConfig", "Random", 0, QApplication::UnicodeUTF8));
        zoomDirOut->setText(QApplication::translate("GenericItemConfig", "Zoom Out", 0, QApplication::UnicodeUTF8));
        zoomDirIn->setText(QApplication::translate("GenericItemConfig", "Zoom In", 0, QApplication::UnicodeUTF8));
        zoomLoop->setText(QApplication::translate("GenericItemConfig", "Auto-Switch", 0, QApplication::UnicodeUTF8));
        groupBox_11->setTitle(QApplication::translate("GenericItemConfig", "Anchor Point", 0, QApplication::UnicodeUTF8));
        anchorCenter->setText(QApplication::translate("GenericItemConfig", "Anchor Center", 0, QApplication::UnicodeUTF8));
        anchorRandom->setText(QApplication::translate("GenericItemConfig", "Random Third-Point Anchor", 0, QApplication::UnicodeUTF8));
        anchorOther->setText(QApplication::translate("GenericItemConfig", "Other Point:", 0, QApplication::UnicodeUTF8));
        zoomAnchorCombo->clear();
        zoomAnchorCombo->insertItems(0, QStringList()
         << QApplication::translate("GenericItemConfig", "Bottom Mid", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("GenericItemConfig", "Bottom Left", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("GenericItemConfig", "Left Mid", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("GenericItemConfig", "Top Left", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("GenericItemConfig", "Top Mid", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("GenericItemConfig", "Top Right", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("GenericItemConfig", "Right Mid", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("GenericItemConfig", "Bottom Right", 0, QApplication::UnicodeUTF8)
        );
        tabWidget->setTabText(tabWidget->indexOf(reflectionTab), QApplication::translate("GenericItemConfig", "Effects", 0, QApplication::UnicodeUTF8));
        btnSave->setText(QApplication::translate("GenericItemConfig", "&Save", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(GenericItemConfig);
    } // retranslateUi

};

namespace Ui {
    class GenericItemConfig: public Ui_GenericItemConfig {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GENERICITEMCONFIG_H
