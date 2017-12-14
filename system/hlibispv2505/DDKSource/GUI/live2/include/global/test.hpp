#ifndef TEST_H
#define TEST_H

#include <qmainwindow.h>
#include "ui_test.h"
#include "img_types.h"
#include "ParameterList.h"

#include <qmutex.h>

class QTreeView;
class QStandardItemModel;
class QStandardItem;
class QTableWidget;
class QXmlStreamWriter;

namespace VisionLive
{

class ObjectRegistry;
class TestRunner;
class TabLogView;
class ProxyHandler;
class HTMLViewer;

class Test: public QMainWindow, public Ui::Test
{
	Q_OBJECT

public:
	enum CommandViewSections
	{
		CMD,
		STATE,
		RES
	};

	Test(ObjectRegistry *objReg, const QString &testFile = QString(), IMG_BOOL8 testStart = false, QMainWindow *parent = 0); // Class constructor
	~Test(); // Class destructor

	void setProxyHandler(ProxyHandler *proxyHandler); // Sets ProxyHandler object

protected:
    void closeEvent(QCloseEvent *event); // Called on closing

private:
	ObjectRegistry *_pObjReg; // Holds ObjectRegistry object

	ProxyHandler *_pProxyHandler; // ProxyHandler object

	TestRunner *_pTestRunner; // Holds TestRunner object used to run test in another thread
	void initTestRunner(); // Initializes TestRunner

	void initMenuBar(); // Initializes MenuBar

	QTreeView *_pObjectView; // ObjectView object used to display objects registered in ObjectRegistry
	QStandardItemModel *_pObjectViewModel; // Model for ObjectView
	QStandardItem *_pRootObject; // Root Item of ObjectView
	void initObjectView(); // Initializes ObjectView
	QStandardItem *getItem(const QString &name, QStandardItem *startItem); // Looks (not recursively) for an item specyfied by name name staring from item startItem and return pointer to it or NULL
	QStandardItem *getItemGlobal(const QString &name, QStandardItem *startItem); // Same as getItem() but recursively

	QTableWidget *_pCommandView; // QTableWidget object used to display test commands
	void initCommandView(); // Initializes CommandView

	TabLogView *_pLogView; // LogView object used to display test logs
	void initLogView(); // Initializes LogView

	QString _fileName; // Holds name of the file containing test

	HTMLViewer *_pHelp; // Help widget

	void retranslate(); // Retranslates GUI

signals:
	void logError(const QString &error, const QString &src); // Requests LogView to log error
	void logWarning(const QString &warning, const QString &src); // Requests LogView to log warning
	void logMessage(const QString &message, const QString &src); // Requests LogView to log message
	void logAction(const QString &action); // Requests LogView to log action

    void testClosed(); // Inform MainWindow that Test is closed

	void triggerAction(QAction *action); // Requests MainWindow to trigger action action
	void connect(unsigned int port); // Requests MainWindow to call connect()
	void disconnect(); // Requests MainWindow to call disconnect()
	void applyConfiguration(); // Requests MainWindow to call applyConfiguration()
	void loadConfiguration(const QString &filename); // Requests MainWindow to call loadConfiguration()
	void saveConfiguration(const QString &filename); // Requests MainWindow to call saveConfiguration()
	void record(const QString &name = QString(), unsigned int nFrames = 0); // Requests MainWindow to call record()

public slots:
	void connected(); // Called by emiting ISPC_connected() from MainWindow
	void disconnected(); // Called by emiting ISPC_disconnected() from MainWindow
	void refreshObjectView(); // Refreshes ObjectView object
	void addObject(const QString &name, const QString &path); // Adds object to ObjectView
	void removeObject(const QString &name); // Removes object from ObjectView
	void removeObjects(QStandardItem *startObject); // Removes objects recursively starting from startObject

	void loadTest(const QString &fileName = QString()); // Loads test from file
    void reloadTest(); // Reloads current test from file
	void saveLog(const QString &fileName = QString()); // Saves test logs to file
	void exportObjectList(const QString &fileName = QString()); // Exports ObjectView entries to xml file
	void writeTreeView(QXmlStreamWriter *writer, QStandardItem *startItem); // Helper function for exportObjectList(). Stores ObjectView into xml struct
	
	void startTest(); // Starts TestRunner thread and test
	void stopTest(); // Stops TestRunner thread and test
	void testFinished(); // Called from TestRunner to indicate test has finished

	void commandRun(const QString &command, int index); // Marks entry of index index in CommandView to indicate command is running
	void commandPass(const QString &command, int index); // Marks entry of index index in CommandView to indicate command has passed
	void commandFail(const QString &command, int index); // Marks entry of index index in CommandView to indicate command has failed
	void commandBroken(const QString &command, int index); // Marks entry of index index in CommandView to indicate command is broken

    void FB_received(const ISPC::ParameterList &intList, const ISPC::ParameterList extList); // Called when feedback is received

	void help(); // Displays help page
	void helpLoadingFailed(); // Called when loading help fails to emit logError()

};

} // namespace VisionLive

#endif // TEST_H
