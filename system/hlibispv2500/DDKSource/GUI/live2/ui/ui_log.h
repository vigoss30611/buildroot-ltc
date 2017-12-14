/********************************************************************************
** Form generated from reading UI file 'log.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOG_H
#define UI_LOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Log
{
public:
    QWidget *error_t;
    QGridLayout *gridLayout;
    QTextEdit *errorLog_te;
    QWidget *warning_t;
    QGridLayout *gridLayout_2;
    QTextEdit *warningLog_te;
    QWidget *info_t;
    QGridLayout *gridLayout_3;
    QTextEdit *infoLog_te;
    QWidget *action_t;
    QGridLayout *gridLayout_4;
    QTextEdit *actionLog_te;
    QWidget *log_t;
    QGridLayout *gridLayout_5;
    QCheckBox *errors_cb;
    QCheckBox *warnings_cb;
    QCheckBox *messages_cb;
    QTableWidget *log_tw;

    void setupUi(QTabWidget *Log)
    {
        if (Log->objectName().isEmpty())
            Log->setObjectName(QStringLiteral("Log"));
        Log->resize(400, 300);
        error_t = new QWidget();
        error_t->setObjectName(QStringLiteral("error_t"));
        gridLayout = new QGridLayout(error_t);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        errorLog_te = new QTextEdit(error_t);
        errorLog_te->setObjectName(QStringLiteral("errorLog_te"));

        gridLayout->addWidget(errorLog_te, 0, 0, 1, 1);

        Log->addTab(error_t, QString());
        warning_t = new QWidget();
        warning_t->setObjectName(QStringLiteral("warning_t"));
        gridLayout_2 = new QGridLayout(warning_t);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        warningLog_te = new QTextEdit(warning_t);
        warningLog_te->setObjectName(QStringLiteral("warningLog_te"));

        gridLayout_2->addWidget(warningLog_te, 0, 0, 1, 1);

        Log->addTab(warning_t, QString());
        info_t = new QWidget();
        info_t->setObjectName(QStringLiteral("info_t"));
        gridLayout_3 = new QGridLayout(info_t);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        infoLog_te = new QTextEdit(info_t);
        infoLog_te->setObjectName(QStringLiteral("infoLog_te"));

        gridLayout_3->addWidget(infoLog_te, 0, 0, 1, 1);

        Log->addTab(info_t, QString());
        action_t = new QWidget();
        action_t->setObjectName(QStringLiteral("action_t"));
        gridLayout_4 = new QGridLayout(action_t);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        actionLog_te = new QTextEdit(action_t);
        actionLog_te->setObjectName(QStringLiteral("actionLog_te"));

        gridLayout_4->addWidget(actionLog_te, 0, 0, 1, 1);

        Log->addTab(action_t, QString());
        log_t = new QWidget();
        log_t->setObjectName(QStringLiteral("log_t"));
        gridLayout_5 = new QGridLayout(log_t);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        errors_cb = new QCheckBox(log_t);
        errors_cb->setObjectName(QStringLiteral("errors_cb"));

        gridLayout_5->addWidget(errors_cb, 0, 0, 1, 1);

        warnings_cb = new QCheckBox(log_t);
        warnings_cb->setObjectName(QStringLiteral("warnings_cb"));

        gridLayout_5->addWidget(warnings_cb, 0, 1, 1, 1);

        messages_cb = new QCheckBox(log_t);
        messages_cb->setObjectName(QStringLiteral("messages_cb"));

        gridLayout_5->addWidget(messages_cb, 0, 2, 1, 1);

        log_tw = new QTableWidget(log_t);
        if (log_tw->columnCount() < 3)
            log_tw->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        log_tw->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        log_tw->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        log_tw->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        log_tw->setObjectName(QStringLiteral("log_tw"));

        gridLayout_5->addWidget(log_tw, 1, 0, 1, 3);

        Log->addTab(log_t, QString());

        retranslateUi(Log);

        Log->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(Log);
    } // setupUi

    void retranslateUi(QTabWidget *Log)
    {
        Log->setWindowTitle(QApplication::translate("Log", "TabWidget", 0));
        Log->setTabText(Log->indexOf(error_t), QApplication::translate("Log", "Error", 0));
        Log->setTabText(Log->indexOf(warning_t), QApplication::translate("Log", "Warning", 0));
        Log->setTabText(Log->indexOf(info_t), QApplication::translate("Log", "Info", 0));
        Log->setTabText(Log->indexOf(action_t), QApplication::translate("Log", "Action", 0));
        errors_cb->setText(QApplication::translate("Log", "Errors", 0));
        warnings_cb->setText(QApplication::translate("Log", "Warnings", 0));
        messages_cb->setText(QApplication::translate("Log", "Messages", 0));
        QTableWidgetItem *___qtablewidgetitem = log_tw->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("Log", "Type", 0));
        QTableWidgetItem *___qtablewidgetitem1 = log_tw->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("Log", "Description", 0));
        QTableWidgetItem *___qtablewidgetitem2 = log_tw->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("Log", "Source", 0));
        Log->setTabText(Log->indexOf(log_t), QApplication::translate("Log", "Log", 0));
    } // retranslateUi

};

namespace Ui {
    class Log: public Ui_Log {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOG_H
