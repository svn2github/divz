/********************************************************************************
** Form generated from reading UI file 'TextProperties.ui'
**
** Created: Sat Dec 19 18:09:11 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEXTPROPERTIES_H
#define UI_TEXTPROPERTIES_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TextProperties
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QFrame *stackFrame;
    QHBoxLayout *horizontalLayout_3;
    QToolButton *bFront;
    QToolButton *bRaise;
    QToolButton *bLower;
    QToolButton *bBack;
    QSpacerItem *horizontalSpacer;
    QToolButton *bDel;
    QHBoxLayout *horizontalLayout_2;
    QToolButton *bEditShape;
    QToolButton *bClearShape;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *TextProperties)
    {
        if (TextProperties->objectName().isEmpty())
            TextProperties->setObjectName(QString::fromUtf8("TextProperties"));
        TextProperties->resize(191, 56);
        TextProperties->setStyleSheet(QString::fromUtf8(""));
        verticalLayout = new QVBoxLayout(TextProperties);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        stackFrame = new QFrame(TextProperties);
        stackFrame->setObjectName(QString::fromUtf8("stackFrame"));
        stackFrame->setStyleSheet(QString::fromUtf8("QFrame {\n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(250, 250, 250, 255), stop:0.500 rgba(200, 200, 200, 255), stop:0.501 rgba(202, 202, 202, 255), stop:1 rgba(230, 230, 230, 255));\n"
"    border: 1px solid rgb(190, 190, 190);\n"
"    border-radius: 4px;\n"
"}\n"
""));
        horizontalLayout_3 = new QHBoxLayout(stackFrame);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        bFront = new QToolButton(stackFrame);
        bFront->setObjectName(QString::fromUtf8("bFront"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/data/action-order-front.png"), QSize(), QIcon::Normal, QIcon::Off);
        bFront->setIcon(icon);
        bFront->setAutoRaise(true);

        horizontalLayout_3->addWidget(bFront);

        bRaise = new QToolButton(stackFrame);
        bRaise->setObjectName(QString::fromUtf8("bRaise"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/data/action-order-raise.png"), QSize(), QIcon::Normal, QIcon::Off);
        bRaise->setIcon(icon1);
        bRaise->setAutoRaise(true);

        horizontalLayout_3->addWidget(bRaise);

        bLower = new QToolButton(stackFrame);
        bLower->setObjectName(QString::fromUtf8("bLower"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/data/action-order-lower.png"), QSize(), QIcon::Normal, QIcon::Off);
        bLower->setIcon(icon2);
        bLower->setAutoRaise(true);

        horizontalLayout_3->addWidget(bLower);

        bBack = new QToolButton(stackFrame);
        bBack->setObjectName(QString::fromUtf8("bBack"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/data/action-order-back.png"), QSize(), QIcon::Normal, QIcon::Off);
        bBack->setIcon(icon3);
        bBack->setAutoRaise(true);

        horizontalLayout_3->addWidget(bBack);


        horizontalLayout->addWidget(stackFrame);

        horizontalSpacer = new QSpacerItem(0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        bDel = new QToolButton(TextProperties);
        bDel->setObjectName(QString::fromUtf8("bDel"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/data/action-delete.png"), QSize(), QIcon::Normal, QIcon::Off);
        bDel->setIcon(icon4);
        bDel->setAutoRaise(true);

        horizontalLayout->addWidget(bDel);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        bEditShape = new QToolButton(TextProperties);
        bEditShape->setObjectName(QString::fromUtf8("bEditShape"));
        bEditShape->setCheckable(true);

        horizontalLayout_2->addWidget(bEditShape);

        bClearShape = new QToolButton(TextProperties);
        bClearShape->setObjectName(QString::fromUtf8("bClearShape"));

        horizontalLayout_2->addWidget(bClearShape);

        horizontalSpacer_2 = new QSpacerItem(0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(TextProperties);

        QMetaObject::connectSlotsByName(TextProperties);
    } // setupUi

    void retranslateUi(QWidget *TextProperties)
    {
        bFront->setText(QString());
        bRaise->setText(QString());
        bLower->setText(QString());
        bBack->setText(QString());
        bEditShape->setText(QApplication::translate("TextProperties", "edit shape", 0, QApplication::UnicodeUTF8));
        bClearShape->setText(QApplication::translate("TextProperties", "X", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(TextProperties);
    } // retranslateUi

};

namespace Ui {
    class TextProperties: public Ui_TextProperties {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEXTPROPERTIES_H
