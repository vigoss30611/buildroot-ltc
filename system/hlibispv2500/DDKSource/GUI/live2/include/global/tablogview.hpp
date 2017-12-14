#ifndef TABLOGVIEW_H
#define TABLOGVIEW_H

#include <qtabwidget.h>
#include "ui_log.h"

class QTextEdit;

namespace VisionLive
{

struct LogData
{
	QString type;
	QString desc;
	QString src;

	LogData()
	{
		type = QString();
		desc = QString();
		src = QString();
	}
};

class TabLogView: public QTabWidget, public Ui::Log
{
	Q_OBJECT

public:
	TabLogView(QTabWidget *parent = 0); // Class constructor
	~TabLogView(); // Class destructor

	QString getErrorLogs() const { return errorLog_te->toPlainText(); } // Returns error logs
	QString getWarningLogs() const { return warningLog_te->toPlainText(); } // Returns warning logs
	QString getInfoLogs() const { return infoLog_te->toPlainText(); } // Returns message logs
	QString getActionLogs() const { return actionLog_te->toPlainText(); } // Returns action logs

private:
	enum LogSections
	{
		TYPE,
		DESC,
		SRC
	};

	QList<LogData> _logs; // Contains all logs
	void initLog(); // Initiates TabLogView
	void addLog(const QString &type, const QString &desc, const QString &src); // Adds new log to _logs
	void showLog(LogData log, bool scroll = true); // Displays log log in TabLogView widget
	void showProgress(int percent); // Displays filtering progress

	void retranslate(); // Retranslates GUI

public slots:
	void logError(const QString &error, const QString &src = QString()); // Called to log error
	void logWarning(const QString &warning, const QString &src = QString()); // Called to log warning
	void logMessage(const QString &message, const QString &src = QString()); // Called to log message
	void logAction(const QString &action); // Called to log action

	void filterLogs(); // Filters logs displayed in TabLogView widget

	void exportLog(const QString &filename = QString()); // Called from MainWindow to save log to file
	void exportActionLog(const QString &filename = QString()); // Called from MainWindow to save action log to file
};

} // namespace VisionLive

#endif // TABLOGVIEW_H
