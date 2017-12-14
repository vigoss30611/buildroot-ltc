#ifndef TESTRUNNER_H
#define TESTRUNNER_H

#include <qthread.h>
#include "algorithms.hpp"

#include <qmutex.h>

namespace VisionLive
{

#define TEST_PASS 0
#define TEST_FAIL 1

class ObjectRegistry;

class TestRunner: public QThread
{
	Q_OBJECT

public:
	TestRunner(ObjectRegistry *objReg, QThread *parent = 0); // Class constructor
	~TestRunner(); // Class destructor

	void clearCommands(); // Clears _commands
	void addCommand(const QString &command); // Adds new command to _commands

	void startTest(); // Starts running the test
	void stopTest(); // Stops the test

	void setFeedback(QMap<QString, QString> feedback); // Sets feedback params
	QMap<QString, QString> getFeedback(); // Returns copy of _feedback

protected:
	void run(); // Thread loop

private:
	ObjectRegistry *_pObjReg; // ObjectRegistry object
	
	Type whatsThis(const QString &param); // Returns type of object of name name
	template<typename T> T getValue(Type paramType, QString param); // Returns value of object of name name

	struct TestCommandResult 
	{
		bool executedProperly;
		int result;
		bool stopTest;
		TestCommandResult() { executedProperly = true; result = TEST_PASS; stopTest = false; }
	};

	std::vector<QString> _commands; // Holds all commands loded from test file in Test
	TestCommandResult processCommand(QStringList command); // Proccesses command and calls specyfic function to generate result
	bool evaluateResult(const QString &command, int index, TestCommandResult &result); // Evaluates result (PASS, FAIL or BROKEN)
	TestCommandResult executeLog(const QString &text); // Logs text
	TestCommandResult executeStop(); // Stops test
	TestCommandResult executeClick(const QString &obj); // Emits click() signal of obj object
	TestCommandResult executeExist(const QString &obj); // Checks if obj object exists
	TestCommandResult executeWait(const QString &val); // Pauses thread for val msec
	TestCommandResult executeSet(const QString &obj, const QString &val); // Sets obj object value to val
	TestCommandResult executeCall(const QString &fun, const QString &param = QString(), const QString &param2 = QString()); // Executes fun function
	TestCommandResult executeCompare(const QString &op, const QString &param1, const QString &param2); // Compares param1 and param2

	QMutex _feedbackLock; // Synchronizez usage of _feedback
	QMap<QString, QString> _feedback; // Holds all feedback values

	bool _continueRun; // Indicates if test should continue to run or not

signals:
	void testFinished(); // Indicates that test has hinished

	void logError(const QString &error, const QString &src); // Requests Log to log error
	void logWarning(const QString &warning, const QString &src); // Requests Log to log warning
	void logMessage(const QString &message, const QString &src); // Requests Log to log message
	void logAction(const QString &action); // Requests Log to log action

	void commandRun(const QString &command, int index); // Indicates that command is running
	void commandPass(const QString &command, int index); // Indicates that command has passed
	void commandFail(const QString &command, int index); // Indicates that command has failed
	void commandBroken(const QString &command, int index); // Indicates that command is broken

	void triggerAction(QAction *action); // Requests MainWindow through Test to trigger action action
	void connect(unsigned int port); // Requests MainWindow through Test to call connect()
	void disconnect(); // Requests MainWindow through Test to call disconnect()
	void applyConfiguration(); // Requests MainWindow through Test to call applyConfiguration()
	void loadConfiguration(const QString &filename); // Requests MainWindow through Test to call loadConfiguration()
	void saveConfiguration(const QString &filename); // Requests MainWindow through Test to call saveConfiguration()
	void record(const QString &name = QString(), unsigned int nFrames = 0); // Requests MainWindow through Test to call record()

	void exportObjectList(const QString &filename); // Requests Test to export object list
	void saveLog(const QString &filename); // Requests Test to save log

	void FB_get(); // Requests Test to get feedback
};

template<> QString TestRunner::getValue(Type paramType, QString param); // Specielized template function for QStringa. Returns value of object of name name

} // namespace VisionLive

#endif // TESTRUNNER_H
