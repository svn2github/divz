/********************************************************************************
** Form generated from reading ui file 'GridDialog.ui'
**
** Created: Sat Dec 19 21:06:51 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_GRIDDIALOG_H
#define UI_GRIDDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_GridDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QCheckBox *cbSnapToGrid;
    QLabel *label;
    QDoubleSpinBox *gridSize;
    QSpacerItem *horizontalSpacer;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout;
    QCheckBox *cbThirds;
    QLabel *linkLabel;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *GridDialog)
    {
        if (GridDialog->objectName().isEmpty())
            GridDialog->setObjectName(QString::fromUtf8("GridDialog"));
        GridDialog->resize(318, 261);
        verticalLayout_2 = new QVBoxLayout(GridDialog);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        groupBox = new QGroupBox(GridDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        cbSnapToGrid = new QCheckBox(groupBox);
        cbSnapToGrid->setObjectName(QString::fromUtf8("cbSnapToGrid"));

        gridLayout->addWidget(cbSnapToGrid, 0, 0, 1, 2);

        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 1, 0, 1, 1);

        gridSize = new QDoubleSpinBox(groupBox);
        gridSize->setObjectName(QString::fromUtf8("gridSize"));

        gridLayout->addWidget(gridSize, 1, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 1, 2, 1, 1);


        verticalLayout_2->addWidget(groupBox);

        groupBox_2 = new QGroupBox(GridDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout = new QVBoxLayout(groupBox_2);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        cbThirds = new QCheckBox(groupBox_2);
        cbThirds->setObjectName(QString::fromUtf8("cbThirds"));

        verticalLayout->addWidget(cbThirds);

        linkLabel = new QLabel(groupBox_2);
        linkLabel->setObjectName(QString::fromUtf8("linkLabel"));

        verticalLayout->addWidget(linkLabel);


        verticalLayout_2->addWidget(groupBox_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(GridDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_2->addWidget(buttonBox);


        retranslateUi(GridDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), GridDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), GridDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(GridDialog);
    } // setupUi

    void retranslateUi(QDialog *GridDialog)
    {
        GridDialog->setWindowTitle(QApplication::translate("GridDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("GridDialog", "Snap to Grid", 0, QApplication::UnicodeUTF8));
        cbSnapToGrid->setText(QApplication::translate("GridDialog", "Snap Items to Grid", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("GridDialog", "Grid Size:", 0, QApplication::UnicodeUTF8));
        gridSize->setSuffix(QApplication::translate("GridDialog", " px ", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("GridDialog", "\"Rule of Thirds\" Guides", 0, QApplication::UnicodeUTF8));
        cbThirds->setText(QApplication::translate("GridDialog", "Draw Rule of Thirds Guides", 0, QApplication::UnicodeUTF8));
        linkLabel->setText(QApplication::translate("GridDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dlg 2';\">For more information on the \"Rule of Thirds\", <br />see: </span><a href=\"http://en.wikipedia.org/wiki/Rule_of_thirds\"><span style=\" font-family:'MS Shell Dlg 2'; text-decoration: underline; color:#0000ff;\">http://en.wikipedia.org/wiki/Rule_of_thirds</span></a></p></body></html>", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(GridDialog);
    } // retranslateUi

};

namespace Ui {
    class GridDialog: public Ui_GridDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRIDDIALOG_H
