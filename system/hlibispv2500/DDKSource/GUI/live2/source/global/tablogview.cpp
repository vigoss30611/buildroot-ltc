#include "tablogview.hpp"

#include "css.hpp"
#include "names.hpp"

#include <qtabwidget.h>
#include <qtextedit.h>
#include <QFileDialog>

//
// Public Functions
//

VisionLive::TabLogView::TabLogView(QTabWidget *parent): QTabWidget(parent)
{
	Ui::Log::setupUi(this);

	retranslate();

	initLog();

	removeTab(indexOf(error_t));
	removeTab(indexOf(warning_t));
	removeTab(indexOf(info_t));
}

VisionLive::TabLogView::~TabLogView()
{
}

//
// Private Functions
//

void VisionLive::TabLogView::initLog()
{
	Log_progress_pb->hide();

	errorLog_te->setReadOnly(true);
	warningLog_te->setReadOnly(true);
	infoLog_te->setReadOnly(true);
	actionLog_te->setReadOnly(true);
	
	setTabText(0, tr(NAME_MAINWINDOW_LOGVIEW_ERRORLOG));
	setTabText(1, tr(NAME_MAINWINDOW_LOGVIEW_WARNINGLOG));
	setTabText(2, tr(NAME_MAINWINDOW_LOGVIEW_INFOLOG));
	setTabText(3, tr(NAME_MAINWINDOW_LOGVIEW_ACTIONLOG));

	errors_cb->setChecked(true);

	QObject::connect(errors_cb, SIGNAL(clicked()), this, SLOT(filterLogs()));
	QObject::connect(warnings_cb, SIGNAL(clicked()), this, SLOT(filterLogs()));
	QObject::connect(messages_cb, SIGNAL(clicked()), this, SLOT(filterLogs()));
}

void VisionLive::TabLogView::addLog(const QString &type, const QString &desc, const QString &src)
{
	LogData newLog;
	newLog.type = type;
	newLog.desc = desc;
	newLog.src = src;
	_logs.push_back(newLog);

	if(newLog.type == NAME_TABLOGVIEW_LOG_TYPE_ERROR && errors_cb->isChecked())
	{
		showLog(newLog);
	}
	else if(newLog.type == NAME_TABLOGVIEW_LOG_TYPE_WARNING && warnings_cb->isChecked())
	{
		showLog(newLog);
	}
	else if(newLog.type == NAME_TABLOGVIEW_LOG_TYPE_MESSAGE && messages_cb->isChecked())
	{
		showLog(newLog);
	}
}

void VisionLive::TabLogView::showLog(LogData log, bool scroll)
{
	int row = log_tw->rowCount();
	log_tw->insertRow(log_tw->rowCount());
	log_tw->setItem(row, TabLogView::TYPE, new QTableWidgetItem(log.type));
	log_tw->setItem(row, TabLogView::DESC, new QTableWidgetItem(log.desc));
	log_tw->setItem(row, TabLogView::SRC, new QTableWidgetItem(log.src));
	//log_tw->resizeColumnsToContents();
	if(scroll)
	{
		log_tw->scrollToItem(log_tw->item(row, 0), QAbstractItemView::PositionAtCenter);
	}
}

void VisionLive::TabLogView::showProgress(int percent)
{
	if(percent < 100)
	{
		Log_progress_pb->show();
		Log_progress_pb->setValue(percent);
	}
	else
	{
		Log_progress_pb->hide();
	}
}

void VisionLive::TabLogView::retranslate()
{
	Ui::Log::retranslateUi(this);
}

//
// Public Slots
//

void VisionLive::TabLogView::logError(const QString &error, const QString &src)
{
	errorLog_te->append(error + "    " + src);

	addLog(NAME_TABLOGVIEW_LOG_TYPE_ERROR, error, src);
}

void VisionLive::TabLogView::logWarning(const QString &warning, const QString &src)
{
	warningLog_te->append(warning + "    " + src);

	addLog(NAME_TABLOGVIEW_LOG_TYPE_WARNING, warning, src);
}

void VisionLive::TabLogView::logMessage(const QString &message, const QString &src)
{
	infoLog_te->append(message + "    " + src);

	addLog(NAME_TABLOGVIEW_LOG_TYPE_MESSAGE, message, src);
}

void VisionLive::TabLogView::logAction(const QString &action)
{
	actionLog_te->append(action);
}

void VisionLive::TabLogView::filterLogs()
{
	log_tw->setRowCount(0);

	for(int i = 0; i < _logs.size(); i++)
	{
		if(_logs.size() > 1)
		{
			showProgress(i*100/(_logs.size()-1));
		}

		if(_logs[i].type == NAME_TABLOGVIEW_LOG_TYPE_ERROR && errors_cb->isChecked())
		{
			showLog(_logs[i], false);
		}
		else if(_logs[i].type == NAME_TABLOGVIEW_LOG_TYPE_WARNING && warnings_cb->isChecked())
		{
			showLog(_logs[i], false);
		}
		else if(_logs[i].type == NAME_TABLOGVIEW_LOG_TYPE_MESSAGE && messages_cb->isChecked())
		{
			showLog(_logs[i], false);
		}
	}
}

void VisionLive::TabLogView::exportLog(const QString &filename)
{
	QString fileToUse = filename;
	if(fileToUse.isEmpty())
    {
		fileToUse = QFileDialog::getOpenFileName(this, tr("Export Log"), ".", tr("Filename (*.txt)"));
    }

	emit logAction("CALL exportLog() " + fileToUse);

	if(fileToUse.isEmpty()) 
	{
		return;
	}

	QFile logFile(fileToUse);
	if(!logFile.open(QIODevice::WriteOnly))
	{
		emit logError(tr("Failed to open txt file!"), tr("TabLogView::exportLog()"));
		return;
	}

	std::string log;
	for(QList<LogData>::iterator it = _logs.begin(); it != _logs.end(); it++)
    {
		log = std::string(QString(it->type + " " + it->desc + " " + it->src + "\n").toLatin1());
		logFile.write(log.c_str(), log.size());
    }

	logFile.close();
}

void VisionLive::TabLogView::exportActionLog(const QString &filename)
{
	QString fileToUse = filename;
	if(fileToUse.isEmpty())
    {
		fileToUse = QFileDialog::getOpenFileName(this, tr("Export Action Log"), ".", tr("Filename (*.txt)"));
    }

	emit logAction("CALL exportActionLog() " + fileToUse);

	if(fileToUse.isEmpty()) 
	{
		return;
	}

	QFile logFile(fileToUse);
	if(!logFile.open(QIODevice::WriteOnly))
	{
		emit logError(tr("Failed to open txt file!"), tr("TabLogView::exportActionLog()"));
		return;
	}

	QString actionLog = actionLog_te->toPlainText();
	logFile.write(actionLog.toLatin1(), actionLog.size());

	logFile.close();
}