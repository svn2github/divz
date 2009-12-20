/********************************************************************************
** Form generated from reading ui file 'DocumentSettingsDialog.ui'
**
** Created: Sat Dec 19 21:06:51 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DOCUMENTSETTINGSDIALOG_H
#define UI_DOCUMENTSETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DocumentSettingsDialog
{
public:
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label_3;
    QDoubleSpinBox *aspectRatio;
    QPushButton *btn4x3;
    QPushButton *btn16x9;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DocumentSettingsDialog)
    {
        if (DocumentSettingsDialog->objectName().isEmpty())
            DocumentSettingsDialog->setObjectName(QString::fromUtf8("DocumentSettingsDialog"));
        DocumentSettingsDialog->resize(534, 219);
        verticalLayout_3 = new QVBoxLayout(DocumentSettingsDialog);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        groupBox = new QGroupBox(DocumentSettingsDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_2->addWidget(label_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout->addWidget(label_3);

        aspectRatio = new QDoubleSpinBox(groupBox);
        aspectRatio->setObjectName(QString::fromUtf8("aspectRatio"));

        horizontalLayout->addWidget(aspectRatio);

        btn4x3 = new QPushButton(groupBox);
        btn4x3->setObjectName(QString::fromUtf8("btn4x3"));

        horizontalLayout->addWidget(btn4x3);

        btn16x9 = new QPushButton(groupBox);
        btn16x9->setObjectName(QString::fromUtf8("btn16x9"));

        horizontalLayout->addWidget(btn16x9);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout_2->addLayout(horizontalLayout);

        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_2->addWidget(label);


        verticalLayout_3->addWidget(groupBox);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(DocumentSettingsDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_3->addWidget(buttonBox);


        retranslateUi(DocumentSettingsDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), DocumentSettingsDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DocumentSettingsDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(DocumentSettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *DocumentSettingsDialog)
    {
        DocumentSettingsDialog->setWindowTitle(QApplication::translate("DocumentSettingsDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("DocumentSettingsDialog", "Intended Aspect Ratio", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("DocumentSettingsDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Aspect Ratio tells DViz what ratio of width to height you want to use for the slides and<br />other items in your document. </span><span style=\" font-size:8pt; font-weight:600;\">It is important</span><span style=\" font-size:8pt;\"> to design slides at the same aspect <br />ratio that you plan to show them at. If the ratio you design at does not match the<br />screen they are shown on, </span><span style=\" font-size:8pt; font-style:italic;\">some content may get cut off</span><span style=\" font-size:8pt;\"> on the top/"
                        "bottom or sides.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("DocumentSettingsDialog", "Aspect Ratio:", 0, QApplication::UnicodeUTF8));
        btn4x3->setText(QApplication::translate("DocumentSettingsDialog", "Use 4 x 3 (Normal)", 0, QApplication::UnicodeUTF8));
        btn16x9->setText(QApplication::translate("DocumentSettingsDialog", "Use 16 x 9 (Wide Screen)", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DocumentSettingsDialog", "Aspect Ratio is expressed as width divided by height.", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(DocumentSettingsDialog);
    } // retranslateUi

};

namespace Ui {
    class DocumentSettingsDialog: public Ui_DocumentSettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DOCUMENTSETTINGSDIALOG_H
