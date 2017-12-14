/********************************************************************************
** Form generated from reading UI file 'test.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEST_H
#define UI_TEST_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Test
{
public:
    QAction *actionLoad_Test;
    QAction *actionSave_Log;
    QAction *actionExport_Object_List;
    QAction *actionStart;
    QAction *actionStop;
    QWidget *centralwidget;
    QGridLayout *gridLayout_4;
    QSplitter *splitter_2;
    QWidget *gridLayoutWidget_3;
    QGridLayout *objectViewLayout;
    QSplitter *splitter;
    QWidget *gridLayoutWidget;
    QGridLayout *commandViewLayout;
    QWidget *gridLayoutWidget_2;
    QGridLayout *logViewLayout;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuTest;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *Test)
    {
        if (Test->objectName().isEmpty())
            Test->setObjectName(QStringLiteral("Test"));
        Test->resize(800, 600);
        actionLoad_Test = new QAction(Test);
        actionLoad_Test->setObjectName(QStringLiteral("actionLoad_Test"));
        actionSave_Log = new QAction(Test);
        actionSave_Log->setObjectName(QStringLiteral("actionSave_Log"));
        actionExport_Object_List = new QAction(Test);
        actionExport_Object_List->setObjectName(QStringLiteral("actionExport_Object_List"));
        actionStart = new QAction(Test);
        actionStart->setObjectName(QStringLiteral("actionStart"));
        actionStop = new QAction(Test);
        actionStop->setObjectName(QStringLiteral("actionStop"));
        centralwidget = new QWidget(Test);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayout_4 = new QGridLayout(centralwidget);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        splitter_2 = new QSplitter(centralwidget);
        splitter_2->setObjectName(QStringLiteral("splitter_2"));
        splitter_2->setOrientation(Qt::Horizontal);
        gridLayoutWidget_3 = new QWidget(splitter_2);
        gridLayoutWidget_3->setObjectName(QStringLiteral("gridLayoutWidget_3"));
        objectViewLayout = new QGridLayout(gridLayoutWidget_3);
        objectViewLayout->setObjectName(QStringLiteral("objectViewLayout"));
        objectViewLayout->setContentsMargins(0, 0, 0, 0);
        splitter_2->addWidget(gridLayoutWidget_3);
        splitter = new QSplitter(splitter_2);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Vertical);
        gridLayoutWidget = new QWidget(splitter);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        commandViewLayout = new QGridLayout(gridLayoutWidget);
        commandViewLayout->setObjectName(QStringLiteral("commandViewLayout"));
        commandViewLayout->setContentsMargins(0, 0, 0, 0);
        splitter->addWidget(gridLayoutWidget);
        gridLayoutWidget_2 = new QWidget(splitter);
        gridLayoutWidget_2->setObjectName(QStringLiteral("gridLayoutWidget_2"));
        logViewLayout = new QGridLayout(gridLayoutWidget_2);
        logViewLayout->setObjectName(QStringLiteral("logViewLayout"));
        logViewLayout->setContentsMargins(0, 0, 0, 0);
        splitter->addWidget(gridLayoutWidget_2);
        splitter_2->addWidget(splitter);

        gridLayout_4->addWidget(splitter_2, 0, 0, 1, 1);

        Test->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Test);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuTest = new QMenu(menubar);
        menuTest->setObjectName(QStringLiteral("menuTest"));
        Test->setMenuBar(menubar);
        statusbar = new QStatusBar(Test);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        Test->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuTest->menuAction());
        menuFile->addAction(actionLoad_Test);
        menuFile->addSeparator();
        menuFile->addAction(actionSave_Log);
        menuFile->addSeparator();
        menuFile->addAction(actionExport_Object_List);
        menuTest->addAction(actionStart);
        menuTest->addAction(actionStop);

        retranslateUi(Test);

        QMetaObject::connectSlotsByName(Test);
    } // setupUi

    void retranslateUi(QMainWindow *Test)
    {
        Test->setWindowTitle(QApplication::translate("Test", "Test", 0));
        actionLoad_Test->setText(QApplication::translate("Test", "Load Test", 0));
        actionSave_Log->setText(QApplication::translate("Test", "Save Log", 0));
        actionExport_Object_List->setText(QApplication::translate("Test", "Export Object List", 0));
        actionStart->setText(QApplication::translate("Test", "Start", 0));
        actionStop->setText(QApplication::translate("Test", "Stop", 0));
        menuFile->setTitle(QApplication::translate("Test", "File", 0));
        menuTest->setTitle(QApplication::translate("Test", "Test", 0));
    } // retranslateUi

};

namespace Ui {
    class Test: public Ui_Test {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEST_H
