/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

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

class Ui_MainWindow
{
public:
    QAction *actionDefault;
    QAction *actionCustom;
    QAction *actionConnect;
    QAction *actionDisconnect;
    QAction *actionShow_HW_Info;
    QAction *actionShow_Histogram;
    QWidget *centralwidget;
    QGridLayout *gridLayout_5;
    QSplitter *splitter;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QSplitter *splitter_2;
    QWidget *gridLayoutWidget_3;
    QGridLayout *moduleViewLayout;
    QWidget *gridLayoutWidget_4;
    QGridLayout *showViewLayout;
    QWidget *gridLayoutWidget_2;
    QGridLayout *logViewLayout;
    QMenuBar *Menu;
    QMenu *menuFile;
    QMenu *menuConnection;
    QMenu *menuView;
    QMenu *menuSet_Style_Sheet;
    QMenu *menuModule_Access;
    QMenu *menuHelp;
    QStatusBar *Status;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(800, 599);
        actionDefault = new QAction(MainWindow);
        actionDefault->setObjectName(QStringLiteral("actionDefault"));
        actionCustom = new QAction(MainWindow);
        actionCustom->setObjectName(QStringLiteral("actionCustom"));
        actionConnect = new QAction(MainWindow);
        actionConnect->setObjectName(QStringLiteral("actionConnect"));
        actionDisconnect = new QAction(MainWindow);
        actionDisconnect->setObjectName(QStringLiteral("actionDisconnect"));
        actionShow_HW_Info = new QAction(MainWindow);
        actionShow_HW_Info->setObjectName(QStringLiteral("actionShow_HW_Info"));
        actionShow_HW_Info->setCheckable(true);
        actionShow_Histogram = new QAction(MainWindow);
        actionShow_Histogram->setObjectName(QStringLiteral("actionShow_Histogram"));
        actionShow_Histogram->setCheckable(true);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayout_5 = new QGridLayout(centralwidget);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        splitter = new QSplitter(centralwidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Vertical);
        gridLayoutWidget = new QWidget(splitter);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        splitter_2 = new QSplitter(gridLayoutWidget);
        splitter_2->setObjectName(QStringLiteral("splitter_2"));
        splitter_2->setOrientation(Qt::Horizontal);
        gridLayoutWidget_3 = new QWidget(splitter_2);
        gridLayoutWidget_3->setObjectName(QStringLiteral("gridLayoutWidget_3"));
        moduleViewLayout = new QGridLayout(gridLayoutWidget_3);
        moduleViewLayout->setObjectName(QStringLiteral("moduleViewLayout"));
        moduleViewLayout->setContentsMargins(0, 0, 0, 0);
        splitter_2->addWidget(gridLayoutWidget_3);
        gridLayoutWidget_4 = new QWidget(splitter_2);
        gridLayoutWidget_4->setObjectName(QStringLiteral("gridLayoutWidget_4"));
        showViewLayout = new QGridLayout(gridLayoutWidget_4);
        showViewLayout->setObjectName(QStringLiteral("showViewLayout"));
        showViewLayout->setContentsMargins(0, 0, 0, 0);
        splitter_2->addWidget(gridLayoutWidget_4);

        gridLayout->addWidget(splitter_2, 0, 0, 1, 1);

        splitter->addWidget(gridLayoutWidget);
        gridLayoutWidget_2 = new QWidget(splitter);
        gridLayoutWidget_2->setObjectName(QStringLiteral("gridLayoutWidget_2"));
        logViewLayout = new QGridLayout(gridLayoutWidget_2);
        logViewLayout->setObjectName(QStringLiteral("logViewLayout"));
        logViewLayout->setContentsMargins(0, 0, 0, 0);
        splitter->addWidget(gridLayoutWidget_2);

        gridLayout_5->addWidget(splitter, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        Menu = new QMenuBar(MainWindow);
        Menu->setObjectName(QStringLiteral("Menu"));
        Menu->setGeometry(QRect(0, 0, 800, 21));
        menuFile = new QMenu(Menu);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuConnection = new QMenu(Menu);
        menuConnection->setObjectName(QStringLiteral("menuConnection"));
        menuView = new QMenu(Menu);
        menuView->setObjectName(QStringLiteral("menuView"));
        menuSet_Style_Sheet = new QMenu(menuView);
        menuSet_Style_Sheet->setObjectName(QStringLiteral("menuSet_Style_Sheet"));
        menuModule_Access = new QMenu(menuView);
        menuModule_Access->setObjectName(QStringLiteral("menuModule_Access"));
        menuHelp = new QMenu(Menu);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        MainWindow->setMenuBar(Menu);
        Status = new QStatusBar(MainWindow);
        Status->setObjectName(QStringLiteral("Status"));
        MainWindow->setStatusBar(Status);

        Menu->addAction(menuFile->menuAction());
        Menu->addAction(menuConnection->menuAction());
        Menu->addAction(menuView->menuAction());
        Menu->addAction(menuHelp->menuAction());
        menuConnection->addAction(actionConnect);
        menuConnection->addAction(actionDisconnect);
        menuView->addAction(menuSet_Style_Sheet->menuAction());
        menuView->addAction(menuModule_Access->menuAction());
        menuView->addAction(actionShow_HW_Info);
        menuView->addAction(actionShow_Histogram);
        menuSet_Style_Sheet->addAction(actionDefault);
        menuSet_Style_Sheet->addAction(actionCustom);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        actionDefault->setText(QApplication::translate("MainWindow", "Default", 0));
        actionCustom->setText(QApplication::translate("MainWindow", "Custom", 0));
        actionConnect->setText(QApplication::translate("MainWindow", "Connect", 0));
        actionDisconnect->setText(QApplication::translate("MainWindow", "Disconnect", 0));
        actionShow_HW_Info->setText(QApplication::translate("MainWindow", "Show HW Info", 0));
        actionShow_Histogram->setText(QApplication::translate("MainWindow", "Show Histogram", 0));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0));
        menuConnection->setTitle(QApplication::translate("MainWindow", "Connection", 0));
        menuView->setTitle(QApplication::translate("MainWindow", "View", 0));
        menuSet_Style_Sheet->setTitle(QApplication::translate("MainWindow", "Set Style Sheet", 0));
        menuModule_Access->setTitle(QApplication::translate("MainWindow", "Module Access", 0));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
