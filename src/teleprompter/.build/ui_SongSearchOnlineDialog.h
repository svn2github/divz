/********************************************************************************
** Form generated from reading UI file 'SongSearchOnlineDialog.ui'
**
** Created: Sat Dec 19 18:09:11 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SONGSEARCHONLINEDIALOG_H
#define UI_SONGSEARCHONLINEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTableWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SongSearchOnlineDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *searchBox;
    QPushButton *searchButton;
    QFrame *line_2;
    QTableWidget *sourceList;
    QFrame *line;
    QTextEdit *preview;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SongSearchOnlineDialog)
    {
        if (SongSearchOnlineDialog->objectName().isEmpty())
            SongSearchOnlineDialog->setObjectName(QString::fromUtf8("SongSearchOnlineDialog"));
        SongSearchOnlineDialog->resize(506, 404);
        verticalLayout = new QVBoxLayout(SongSearchOnlineDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(SongSearchOnlineDialog);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        searchBox = new QLineEdit(SongSearchOnlineDialog);
        searchBox->setObjectName(QString::fromUtf8("searchBox"));

        horizontalLayout->addWidget(searchBox);

        searchButton = new QPushButton(SongSearchOnlineDialog);
        searchButton->setObjectName(QString::fromUtf8("searchButton"));

        horizontalLayout->addWidget(searchButton);


        verticalLayout->addLayout(horizontalLayout);

        line_2 = new QFrame(SongSearchOnlineDialog);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_2);

        sourceList = new QTableWidget(SongSearchOnlineDialog);
        sourceList->setObjectName(QString::fromUtf8("sourceList"));

        verticalLayout->addWidget(sourceList);

        line = new QFrame(SongSearchOnlineDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        preview = new QTextEdit(SongSearchOnlineDialog);
        preview->setObjectName(QString::fromUtf8("preview"));

        verticalLayout->addWidget(preview);

        buttonBox = new QDialogButtonBox(SongSearchOnlineDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(SongSearchOnlineDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SongSearchOnlineDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SongSearchOnlineDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(SongSearchOnlineDialog);
    } // setupUi

    void retranslateUi(QDialog *SongSearchOnlineDialog)
    {
        SongSearchOnlineDialog->setWindowTitle(QApplication::translate("SongSearchOnlineDialog", "Song Search Online", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SongSearchOnlineDialog", "Song Title:", 0, QApplication::UnicodeUTF8));
        searchButton->setText(QApplication::translate("SongSearchOnlineDialog", "Search", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SongSearchOnlineDialog: public Ui_SongSearchOnlineDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SONGSEARCHONLINEDIALOG_H
