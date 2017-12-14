/********************************************************************************
** Form generated from reading UI file 'out.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OUT_H
#define UI_OUT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OUT
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *encoderGroup_gb;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label;
    QComboBox *encoderFormat_lb;
    QLabel *label_2;
    QSpinBox *encoderWidth_sb;
    QLabel *label_3;
    QSpinBox *encoderHeight_sb;
    QGroupBox *displayGroup_gb;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QComboBox *displayFormat_lb;
    QLabel *label_5;
    QSpinBox *displayWidth_sb;
    QLabel *label_6;
    QSpinBox *displayHeight_sb;
    QGroupBox *dataExtractionGroup_gb;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_8;
    QComboBox *dataExtractionFormat_lb;
    QLabel *label_7;
    QComboBox *dataExtractionPoint_lb;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *OUT)
    {
        if (OUT->objectName().isEmpty())
            OUT->setObjectName(QStringLiteral("OUT"));
        OUT->resize(400, 300);
        verticalLayout = new QVBoxLayout(OUT);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        encoderGroup_gb = new QGroupBox(OUT);
        encoderGroup_gb->setObjectName(QStringLiteral("encoderGroup_gb"));
        encoderGroup_gb->setCheckable(false);
        horizontalLayout_3 = new QHBoxLayout(encoderGroup_gb);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label = new QLabel(encoderGroup_gb);
        label->setObjectName(QStringLiteral("label"));
        label->setMaximumSize(QSize(60, 16777215));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(label);

        encoderFormat_lb = new QComboBox(encoderGroup_gb);
        encoderFormat_lb->setObjectName(QStringLiteral("encoderFormat_lb"));

        horizontalLayout_3->addWidget(encoderFormat_lb);

        label_2 = new QLabel(encoderGroup_gb);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setMaximumSize(QSize(60, 16777215));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(label_2);

        encoderWidth_sb = new QSpinBox(encoderGroup_gb);
        encoderWidth_sb->setObjectName(QStringLiteral("encoderWidth_sb"));
        encoderWidth_sb->setFrame(true);
        encoderWidth_sb->setAlignment(Qt::AlignCenter);
        encoderWidth_sb->setButtonSymbols(QAbstractSpinBox::NoButtons);
        encoderWidth_sb->setMaximum(4000);

        horizontalLayout_3->addWidget(encoderWidth_sb);

        label_3 = new QLabel(encoderGroup_gb);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setMaximumSize(QSize(20, 16777215));
        label_3->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(label_3);

        encoderHeight_sb = new QSpinBox(encoderGroup_gb);
        encoderHeight_sb->setObjectName(QStringLiteral("encoderHeight_sb"));
        encoderHeight_sb->setFrame(true);
        encoderHeight_sb->setAlignment(Qt::AlignCenter);
        encoderHeight_sb->setButtonSymbols(QAbstractSpinBox::NoButtons);
        encoderHeight_sb->setMaximum(4000);

        horizontalLayout_3->addWidget(encoderHeight_sb);


        verticalLayout->addWidget(encoderGroup_gb);

        displayGroup_gb = new QGroupBox(OUT);
        displayGroup_gb->setObjectName(QStringLiteral("displayGroup_gb"));
        displayGroup_gb->setCheckable(false);
        displayGroup_gb->setChecked(false);
        horizontalLayout_4 = new QHBoxLayout(displayGroup_gb);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_4 = new QLabel(displayGroup_gb);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setEnabled(true);
        label_4->setMaximumSize(QSize(60, 16777215));
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_4->addWidget(label_4);

        displayFormat_lb = new QComboBox(displayGroup_gb);
        displayFormat_lb->setObjectName(QStringLiteral("displayFormat_lb"));
        displayFormat_lb->setEnabled(true);

        horizontalLayout_4->addWidget(displayFormat_lb);

        label_5 = new QLabel(displayGroup_gb);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setEnabled(true);
        label_5->setMaximumSize(QSize(60, 16777215));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_4->addWidget(label_5);

        displayWidth_sb = new QSpinBox(displayGroup_gb);
        displayWidth_sb->setObjectName(QStringLiteral("displayWidth_sb"));
        displayWidth_sb->setFrame(true);
        displayWidth_sb->setAlignment(Qt::AlignCenter);
        displayWidth_sb->setButtonSymbols(QAbstractSpinBox::NoButtons);
        displayWidth_sb->setMaximum(4000);

        horizontalLayout_4->addWidget(displayWidth_sb);

        label_6 = new QLabel(displayGroup_gb);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setEnabled(true);
        label_6->setMaximumSize(QSize(20, 16777215));
        label_6->setAlignment(Qt::AlignCenter);

        horizontalLayout_4->addWidget(label_6);

        displayHeight_sb = new QSpinBox(displayGroup_gb);
        displayHeight_sb->setObjectName(QStringLiteral("displayHeight_sb"));
        displayHeight_sb->setFrame(true);
        displayHeight_sb->setAlignment(Qt::AlignCenter);
        displayHeight_sb->setButtonSymbols(QAbstractSpinBox::NoButtons);
        displayHeight_sb->setMaximum(4000);

        horizontalLayout_4->addWidget(displayHeight_sb);


        verticalLayout->addWidget(displayGroup_gb);

        dataExtractionGroup_gb = new QGroupBox(OUT);
        dataExtractionGroup_gb->setObjectName(QStringLiteral("dataExtractionGroup_gb"));
        dataExtractionGroup_gb->setMaximumSize(QSize(16777215, 16777215));
        dataExtractionGroup_gb->setCheckable(false);
        dataExtractionGroup_gb->setChecked(false);
        horizontalLayout_5 = new QHBoxLayout(dataExtractionGroup_gb);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_8 = new QLabel(dataExtractionGroup_gb);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setEnabled(true);
        label_8->setMaximumSize(QSize(60, 16777215));
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_5->addWidget(label_8);

        dataExtractionFormat_lb = new QComboBox(dataExtractionGroup_gb);
        dataExtractionFormat_lb->setObjectName(QStringLiteral("dataExtractionFormat_lb"));
        dataExtractionFormat_lb->setEnabled(true);

        horizontalLayout_5->addWidget(dataExtractionFormat_lb);

        label_7 = new QLabel(dataExtractionGroup_gb);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setEnabled(true);
        label_7->setMaximumSize(QSize(100, 16777215));
        label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_5->addWidget(label_7);

        dataExtractionPoint_lb = new QComboBox(dataExtractionGroup_gb);
        dataExtractionPoint_lb->setObjectName(QStringLiteral("dataExtractionPoint_lb"));
        dataExtractionPoint_lb->setEnabled(true);

        horizontalLayout_5->addWidget(dataExtractionPoint_lb);


        verticalLayout->addWidget(dataExtractionGroup_gb);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(OUT);

        QMetaObject::connectSlotsByName(OUT);
    } // setupUi

    void retranslateUi(QWidget *OUT)
    {
        OUT->setWindowTitle(QApplication::translate("OUT", "OUT", 0));
        encoderGroup_gb->setTitle(QApplication::translate("OUT", "Encoder Pipeline (YUV)", 0));
        label->setText(QApplication::translate("OUT", "Format", 0));
        encoderFormat_lb->clear();
        encoderFormat_lb->insertItems(0, QStringList()
         << QApplication::translate("OUT", "None", 0)
         << QApplication::translate("OUT", "NV12", 0)
         << QApplication::translate("OUT", "NV21", 0)
         << QApplication::translate("OUT", "NV16", 0)
         << QApplication::translate("OUT", "NV61", 0)
        );
        label_2->setText(QApplication::translate("OUT", "Size", 0));
        label_3->setText(QApplication::translate("OUT", "x", 0));
        displayGroup_gb->setTitle(QApplication::translate("OUT", "Display Pipeline (RGB)", 0));
        label_4->setText(QApplication::translate("OUT", "Format", 0));
        displayFormat_lb->clear();
        displayFormat_lb->insertItems(0, QStringList()
         << QApplication::translate("OUT", "None", 0)
         << QApplication::translate("OUT", "RGB 8", 0)
         << QApplication::translate("OUT", "BGR 8", 0)
         << QApplication::translate("OUT", "RGB 10", 0)
         << QApplication::translate("OUT", "BGR 10", 0)
        );
        label_5->setText(QApplication::translate("OUT", "Size", 0));
        label_6->setText(QApplication::translate("OUT", "x", 0));
        dataExtractionGroup_gb->setTitle(QApplication::translate("OUT", "Data Extraction (Bayer)", 0));
        label_8->setText(QApplication::translate("OUT", "Format", 0));
        dataExtractionFormat_lb->clear();
        dataExtractionFormat_lb->insertItems(0, QStringList()
         << QApplication::translate("OUT", "None", 0)
         << QApplication::translate("OUT", "RGB 8", 0)
         << QApplication::translate("OUT", "BGR 8", 0)
         << QApplication::translate("OUT", "RGB 10", 0)
         << QApplication::translate("OUT", "BGR 10", 0)
        );
        label_7->setText(QApplication::translate("OUT", "Extraction Point", 0));
        dataExtractionPoint_lb->clear();
        dataExtractionPoint_lb->insertItems(0, QStringList()
         << QApplication::translate("OUT", "EXT 1", 0)
         << QApplication::translate("OUT", "EXT 2", 0)
        );
    } // retranslateUi

};

namespace Ui {
    class OUT: public Ui_OUT {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OUT_H
