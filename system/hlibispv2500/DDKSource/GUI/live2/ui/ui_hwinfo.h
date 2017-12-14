/********************************************************************************
** Form generated from reading UI file 'hwinfo.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HWINFO_H
#define UI_HWINFO_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_HWINFO
{
public:
    QGridLayout *gridLayout;
    QScrollArea *hwinfoList;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *hwinfoListLayout;
    QLabel *label;
    QFrame *line;

    void setupUi(QWidget *HWINFO)
    {
        if (HWINFO->objectName().isEmpty())
            HWINFO->setObjectName(QStringLiteral("HWINFO"));
        HWINFO->resize(400, 300);
        gridLayout = new QGridLayout(HWINFO);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        hwinfoList = new QScrollArea(HWINFO);
        hwinfoList->setObjectName(QStringLiteral("hwinfoList"));
        hwinfoList->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 380, 252));
        hwinfoListLayout = new QGridLayout(scrollAreaWidgetContents);
        hwinfoListLayout->setObjectName(QStringLiteral("hwinfoListLayout"));
        hwinfoList->setWidget(scrollAreaWidgetContents);

        gridLayout->addWidget(hwinfoList, 2, 0, 1, 1);

        label = new QLabel(HWINFO);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        line = new QFrame(HWINFO);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 1, 0, 1, 1);


        retranslateUi(HWINFO);

        QMetaObject::connectSlotsByName(HWINFO);
    } // setupUi

    void retranslateUi(QWidget *HWINFO)
    {
        HWINFO->setWindowTitle(QApplication::translate("HWINFO", "HWINFO", 0));
        label->setText(QApplication::translate("HWINFO", "HW INFO", 0));
    } // retranslateUi

};

namespace Ui {
    class HWINFO: public Ui_HWINFO {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HWINFO_H
