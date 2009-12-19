/********************************************************************************
** Form generated from reading UI file 'outputsetupdialog.ui'
**
** Created: Sat Dec 19 18:09:11 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OUTPUTSETUPDIALOG_H
#define UI_OUTPUTSETUPDIALOG_H

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
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OutputSetupDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QGroupBox *gbOutputs;
    QVBoxLayout *verticalLayout_7;
    QTableWidget *outputListView;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *btnNewOutput;
    QPushButton *btnDelOutput;
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
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_11;
    QSpinBox *listenPort;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *verticalSpacer_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *OutputSetupDialog)
    {
        if (OutputSetupDialog->objectName().isEmpty())
            OutputSetupDialog->setObjectName(QString::fromUtf8("OutputSetupDialog"));
        OutputSetupDialog->resize(785, 535);
        verticalLayout_2 = new QVBoxLayout(OutputSetupDialog);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        gbOutputs = new QGroupBox(OutputSetupDialog);
        gbOutputs->setObjectName(QString::fromUtf8("gbOutputs"));
        verticalLayout_7 = new QVBoxLayout(gbOutputs);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        outputListView = new QTableWidget(gbOutputs);
        if (outputListView->columnCount() < 3)
            outputListView->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        outputListView->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        outputListView->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        outputListView->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        outputListView->setObjectName(QString::fromUtf8("outputListView"));
        outputListView->setColumnCount(3);
        outputListView->horizontalHeader()->setProperty("showSortIndicator", QVariant(false));
        outputListView->verticalHeader()->setVisible(false);

        verticalLayout_7->addWidget(outputListView);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        btnNewOutput = new QPushButton(gbOutputs);
        btnNewOutput->setObjectName(QString::fromUtf8("btnNewOutput"));

        horizontalLayout_2->addWidget(btnNewOutput);

        btnDelOutput = new QPushButton(gbOutputs);
        btnDelOutput->setObjectName(QString::fromUtf8("btnDelOutput"));

        horizontalLayout_2->addWidget(btnDelOutput);


        verticalLayout_7->addLayout(horizontalLayout_2);


        horizontalLayout->addWidget(gbOutputs);

        gbSettings = new QGroupBox(OutputSetupDialog);
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

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_11 = new QLabel(tabNetwork);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        horizontalLayout_3->addWidget(label_11);

        listenPort = new QSpinBox(tabNetwork);
        listenPort->setObjectName(QString::fromUtf8("listenPort"));
        listenPort->setMinimum(1025);
        listenPort->setMaximum(65536);
        listenPort->setValue(7777);

        horizontalLayout_3->addWidget(listenPort);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);


        verticalLayout_6->addLayout(horizontalLayout_3);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer_2);

        tabWidget->addTab(tabNetwork, QString());

        verticalLayout_3->addWidget(tabWidget);


        horizontalLayout->addWidget(gbSettings);


        verticalLayout_2->addLayout(horizontalLayout);

        buttonBox = new QDialogButtonBox(OutputSetupDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_2->addWidget(buttonBox);


        retranslateUi(OutputSetupDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), OutputSetupDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), OutputSetupDialog, SLOT(reject()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(OutputSetupDialog);
    } // setupUi

    void retranslateUi(QDialog *OutputSetupDialog)
    {
        OutputSetupDialog->setWindowTitle(QApplication::translate("OutputSetupDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        gbOutputs->setTitle(QApplication::translate("OutputSetupDialog", "Available Outputs", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = outputListView->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("OutputSetupDialog", "Name", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = outputListView->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("OutputSetupDialog", "System?", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = outputListView->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("OutputSetupDialog", "Enabled?", 0, QApplication::UnicodeUTF8));
        btnNewOutput->setText(QApplication::translate("OutputSetupDialog", "New Output", 0, QApplication::UnicodeUTF8));
        btnDelOutput->setText(QApplication::translate("OutputSetupDialog", "Delete output", 0, QApplication::UnicodeUTF8));
        gbSettings->setTitle(QApplication::translate("OutputSetupDialog", "Output Settings", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("OutputSetupDialog", "Name", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("OutputSetupDialog", "(Description Goes Here)", 0, QApplication::UnicodeUTF8));
        cbOutputEnabled->setText(QApplication::translate("OutputSetupDialog", "Output Enabled", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("OutputSetupDialog", "Select a screen from the list below for this output.", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        screenListView->setToolTip(QApplication::translate("OutputSetupDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Tahoma'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">List of screens connected to your computer</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        groupBox->setTitle(QApplication::translate("OutputSetupDialog", "MJPEG Server", 0, QApplication::UnicodeUTF8));
        mjpegEnabled->setText(QApplication::translate("OutputSetupDialog", "MJPEG Server Enabled", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("OutputSetupDialog", "Port: ", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("OutputSetupDialog", "FPS: ", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabScreen), QApplication::translate("OutputSetupDialog", "External Screen", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("OutputSetupDialog", "You may position this output by entering a custom", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("OutputSetupDialog", "location, width and height in pixels, below.", 0, QApplication::UnicodeUTF8));
        stayOnTop->setText(QApplication::translate("OutputSetupDialog", "Keep output on top of all other windows", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("OutputSetupDialog", "Left (X)", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("OutputSetupDialog", "Top (Y)", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("OutputSetupDialog", "Width", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("OutputSetupDialog", "Height", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("OutputSetupDialog", "MJPEG Server", 0, QApplication::UnicodeUTF8));
        mjpegEnabled2->setText(QApplication::translate("OutputSetupDialog", "MJPEG Server Enabled", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("OutputSetupDialog", "Port: ", 0, QApplication::UnicodeUTF8));
        label_19->setText(QApplication::translate("OutputSetupDialog", "FPS: ", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabCustom), QApplication::translate("OutputSetupDialog", "Custom", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("OutputSetupDialog", "Network Outputs allow you to send data over the wire\n"
"to be displayed by another computer using the\n"
"DViz Network Viewer application.", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("OutputSetupDialog", "Accept Incomming Connections on Port:", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabNetwork), QApplication::translate("OutputSetupDialog", "Network", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class OutputSetupDialog: public Ui_OutputSetupDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OUTPUTSETUPDIALOG_H
