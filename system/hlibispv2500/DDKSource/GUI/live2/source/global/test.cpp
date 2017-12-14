#include "test.hpp"
#include "css.hpp"
#include "objectregistry.hpp"
#include "testrunner.hpp"
#include "proxyhandler.hpp"

#include <qtreeview.h>
#include <qstandarditemmodel.h>
#include <qtablewidget.h>
#include "tablogview.hpp"
#include <qfiledialog.h>
#include <qxmlstream.h>

//
// Public Functions
//

VisionLive::Test::Test(ObjectRegistry *objReg, const QString &testFile, IMG_BOOL8 testStart, QMainWindow *parent): QMainWindow(parent)
{
	Ui::Test::setupUi(this);

	retranslate();

	_pObjReg = objReg;
	_pProxyHandler = NULL;
	_pTestRunner = NULL;
	_pObjectView = NULL;
	_pObjectViewModel = NULL;
	_pRootObject = NULL;
	_pCommandView = NULL;
	_pLogView = NULL;

	_fileName = testFile;

	initTestRunner();

	initMenuBar();

	initObjectView();

	initCommandView();

	initLogView();

	if(!_fileName.isEmpty())
	{
		loadTest(_fileName);
		if(testStart)
		{
			startTest();
		}
	}
}

VisionLive::Test::~Test()
{
	if(_pTestRunner) delete _pTestRunner;
	if(_pLogView) delete _pLogView;
}

void VisionLive::Test::setProxyHandler(ProxyHandler *proxyHandler)
{
	_pProxyHandler = proxyHandler;
	QObject::connect(_pTestRunner, SIGNAL(FB_get()), _pProxyHandler, SLOT(FB_get()));
	QObject::connect(_pProxyHandler, SIGNAL(FB_received(QMap<QString, QString>)), this, SLOT(FB_received(QMap<QString, QString>)));
}

//
// Private Functions
//

void VisionLive::Test::initTestRunner()
{
	_pTestRunner = new TestRunner(_pObjReg);

	QObject::connect(_pTestRunner, SIGNAL(testFinished()), this, SLOT(testFinished()));
	QObject::connect(_pTestRunner, SIGNAL(logError(const QString &, const QString &)), this, SIGNAL(logError(const QString &, const QString &)));
	QObject::connect(_pTestRunner, SIGNAL(logWarning(const QString &, const QString &)), this, SIGNAL(logWarning(const QString &, const QString &)));
	QObject::connect(_pTestRunner, SIGNAL(logMessage(const QString &, const QString &)), this, SIGNAL(logMessage(const QString &, const QString &)));
	QObject::connect(_pTestRunner, SIGNAL(logAction(const QString &)), this, SIGNAL(logAction(const QString &)));
	QObject::connect(_pTestRunner, SIGNAL(commandRun(const QString &, int)), this, SLOT(commandRun(const QString &, int)));
	QObject::connect(_pTestRunner, SIGNAL(commandPass(const QString &, int)), this, SLOT(commandPass(const QString &, int)));
	QObject::connect(_pTestRunner, SIGNAL(commandFail(const QString &, int)), this, SLOT(commandFail(const QString &, int)));
	QObject::connect(_pTestRunner, SIGNAL(commandBroken(const QString &, int)), this, SLOT(commandBroken(const QString &, int)));
	QObject::connect(_pTestRunner, SIGNAL(triggerAction(QAction *)), this, SIGNAL(triggerAction(QAction *)));
	QObject::connect(_pTestRunner, SIGNAL(applyConfiguration()), this, SIGNAL(applyConfiguration()));
	QObject::connect(_pTestRunner, SIGNAL(loadConfiguration(const QString &)), this, SIGNAL(loadConfiguration(const QString &)));
	QObject::connect(_pTestRunner, SIGNAL(saveConfiguration(const QString &)), this, SIGNAL(saveConfiguration(const QString &)));
	QObject::connect(_pTestRunner, SIGNAL(record(const QString &, unsigned int)), this, SIGNAL(record(const QString &, unsigned int)));
	QObject::connect(_pTestRunner, SIGNAL(connect(unsigned int)), this, SIGNAL(connect(unsigned int)));
	QObject::connect(_pTestRunner, SIGNAL(disconnect()), this, SIGNAL(disconnect()));
	QObject::connect(_pTestRunner, SIGNAL(exportObjectList(const QString &)), this, SLOT(exportObjectList(const QString &)));
	QObject::connect(_pTestRunner, SIGNAL(saveLog(const QString &)), this, SLOT(saveLog(const QString &)));
}

void VisionLive::Test::initMenuBar()
{
	actionStart->setEnabled(true);
	actionStop->setEnabled(false);

	QObject::connect(actionLoad_Test, SIGNAL(triggered()), this, SLOT(loadTest()));
	QObject::connect(actionSave_Log, SIGNAL(triggered()), this, SLOT(saveLog()));
	QObject::connect(actionExport_Object_List, SIGNAL(triggered()), this, SLOT(exportObjectList()));
	QObject::connect(actionExport_Feedback, SIGNAL(triggered()), this, SLOT(exportFeedback()));
	QObject::connect(actionStart, SIGNAL(triggered()), this, SLOT(startTest()));
	QObject::connect(actionStop, SIGNAL(triggered()), this, SLOT(stopTest()));
}

void VisionLive::Test::initObjectView()
{
	_pObjectView = new QTreeView();
	_pObjectViewModel = new QStandardItemModel(0, 1);
	_pObjectViewModel->setHorizontalHeaderLabels(QStringList("Registered Objects"));
	_pRootObject = new QStandardItem("ROOT");
	_pObjectViewModel->appendRow(_pRootObject);
	_pObjectView->setModel(_pObjectViewModel);
	_pObjectView->setAnimated(true);
	_pObjectView->setEditTriggers(QAbstractItemView::NoEditTriggers);

	objectViewLayout->addWidget(_pObjectView);

	for(unsigned int i = 0; i < _pObjReg->size(); i++)
	{
		addObject(_pObjReg->getObjectName(i), _pObjReg->getObjectPath(i));
	}
}

QStandardItem *VisionLive::Test::getItem(const QString &name, QStandardItem *startItem)
{
	for(int i = 0; i < startItem->rowCount(); i++)
	{
		if(startItem->child(i)->text() == name)
		{
			return startItem->child(i);
		}
	}

	return NULL;
}

QStandardItem *VisionLive::Test::getItemGlobal(const QString &name, QStandardItem *startItem)
{
	for(int i = 0; i < startItem->rowCount(); i++)
	{
		if(startItem->child(i)->text() == name)
		{
			return startItem->child(i);
		}
		else
		{ 
			if(startItem->hasChildren())
			{
				QStandardItem *ret = getItemGlobal(name, startItem->child(i));
				if(ret)
				{
					return ret;
				}
			}
		}
	}

	return NULL;
}

void VisionLive::Test::initCommandView()
{
	_pCommandView = new QTableWidget(0, 3);
	QTableWidgetItem *commandColumn = new QTableWidgetItem(tr("Command"));
	QTableWidgetItem *statusColumn = new QTableWidgetItem(tr("State"));
	QTableWidgetItem *resultColumn = new QTableWidgetItem(tr("Result"));
	_pCommandView->setHorizontalHeaderItem(0, commandColumn);
	_pCommandView->setHorizontalHeaderItem(1, statusColumn);
	_pCommandView->setHorizontalHeaderItem(2, resultColumn);
	_pCommandView->setEditTriggers(false);

	commandViewLayout->addWidget(_pCommandView);
}

void VisionLive::Test::initLogView()
{
	_pLogView = new TabLogView();
	_pLogView->messages_cb->setChecked(true);
	//_pLogView->removeTab(_pLogView->indexOf(_pLogView->log_t));
	_pLogView->removeTab(_pLogView->indexOf(_pLogView->action_t));
	logViewLayout->addWidget(_pLogView);

	QObject::connect(this, SIGNAL(logError(const QString &, const QString &)), _pLogView, SLOT(logError(const QString &, const QString &)));
	QObject::connect(this, SIGNAL(logWarning(const QString &, const QString &)), _pLogView, SLOT(logWarning(const QString &, const QString &)));
	QObject::connect(this, SIGNAL(logMessage(const QString &, const QString &)), _pLogView, SLOT(logMessage(const QString &, const QString &)));
	QObject::connect(this, SIGNAL(logAction(const QString &)), _pLogView, SLOT(logAction(const QString &)));
}

void VisionLive::Test::retranslate()
{
	Ui::Test::retranslateUi(this);
}

//
// Public Slots
//

void VisionLive::Test::refreshObjectView()
{
	_pObjectViewModel->setRowCount(0);
	_pRootObject = new QStandardItem("ROOT");
	_pObjectViewModel->appendRow(_pRootObject);

	for(unsigned int i = 0; i < _pObjReg->size(); i++)
	{
		addObject(_pObjReg->getObjectName(i), _pObjReg->getObjectPath(i));
	}
}

void VisionLive::Test::addObject(const QString &name, const QString &path)
{
	QStringList objectPath = path.split("/", QString::SkipEmptyParts);
	QStandardItem *parent = _pRootObject;
	for(int i = 0; i < objectPath.size(); i++)
	{
		QStandardItem *item = getItem(objectPath[i], parent);
		if(item != NULL)
		{
			parent = item;
		}
		else
		{
			item = new QStandardItem(objectPath[i]);
			parent->appendRow(item);
			parent = item;
		}
	}
	if(!getItemGlobal(name, _pRootObject))
	{
		parent->appendRow(new QStandardItem(name));
	}
}

void VisionLive::Test::removeObject(const QString &name)
{
	QStandardItem *item = getItemGlobal(name, _pRootObject);
	if(item != NULL)
	{
		if(item->hasChildren())
		{
			for(int i = item->rowCount()-1; i >= 0; i--)
			{
				removeObject(item->child(i)->text());
			}
			removeObject(item->text());
		}
		else
		{
			item->parent()->removeRow(item->row());
		}
	}
}

void VisionLive::Test::removeObjects(QStandardItem *startObject)
{
	if(startObject->hasChildren())
	{
		for(int i = startObject->rowCount()-1; i >= 0; i--)
		{
			removeObjects(startObject->child(i));
		}
		removeObjects(startObject);
	}
	else
	{
		startObject->parent()->removeRow(startObject->row());
	}
}

void VisionLive::Test::loadTest(const QString &fileName)
{
	if(fileName.isEmpty())
	{
		_fileName = QFileDialog::getOpenFileName(this, tr("Load test"), ".", tr("Test file (*.txt)"));
	}
	else
	{
		_fileName = fileName;
	}

	QFile testFile(_fileName);
	if(!testFile.open(QIODevice::ReadOnly))
	{
		emit logError(tr("Failed to open test file ") + _fileName + tr(" !"), tr("Test::loadTest()"));
		return;
	}

	int row = 0;

	_pCommandView->setRowCount(0);

	_pTestRunner->clearCommands();

	while(!testFile.atEnd())
	{
		QString line = testFile.readLine();
		line.remove(QRegExp("[\n\t\r]"));

		if(line.isEmpty() || line.startsWith("//")) continue;

		_pCommandView->insertRow(_pCommandView->rowCount());
		_pCommandView->setItem(row, Test::CMD, new QTableWidgetItem(line));
		_pCommandView->setItem(row, Test::STATE, new QTableWidgetItem(tr("Not executed")));
		_pCommandView->setItem(row, Test::RES, new QTableWidgetItem(tr("")));

		_pTestRunner->addCommand(line);

		row++;
	}
	_pCommandView->resizeColumnsToContents();

	//_pStartAct->setEnabled(true);

	//actionStart->setEnabled(true);
	//actionStartStepping->setEnabled(true);
}

void VisionLive::Test::saveLog(const QString &fileName)
{
	QString path = fileName;
	
	if(fileName.isEmpty())
	{
		path = QFileDialog::getSaveFileName(this, tr("Save log"), ".", tr("File (*.txt)"));
	}

	QFile logFile(path);
	if(!logFile.open(QIODevice::WriteOnly))
	{
		emit logError(tr("Failed to open txt file!"), tr("Test::saveLog()"));
		return;
	}

	logFile.write(_pLogView->getActionLogs().toLatin1());

	logFile.close();
}

void VisionLive::Test::exportObjectList(const QString &fileName)
{
	QString path = fileName;

	if(fileName.isEmpty())
	{
		path = QFileDialog::getSaveFileName(this, tr("Export object list"), ".", tr("File (*.xml)"));
	}

	QFile xmlFile(path);
	if(!xmlFile.open(QIODevice::WriteOnly))
	{
		emit logError(tr("Failed to open xml file!"), tr("Test::exportObjectList()"));
		return;
	}

	QXmlStreamWriter xmlWriter(&xmlFile);
	xmlWriter.setAutoFormatting(true);
	xmlWriter.writeStartDocument();
	writeTreeView(&xmlWriter, _pRootObject);
	xmlWriter.writeEndDocument();

	xmlFile.close();
}

void VisionLive::Test::writeTreeView(QXmlStreamWriter *writer, QStandardItem *startItem)
{
	if(startItem->hasChildren())
	{
		writer->writeStartElement(startItem->text());
		for(int i = 0; i < startItem->rowCount(); i++)
		{
			writeTreeView(writer, startItem->child(i));
		}
		writer->writeEndElement();
	}
	else
	{
		writer->writeTextElement("Object", startItem->text());
	}
}

void VisionLive::Test::exportFeedback(const QString &filename)
{
	QString path = filename;

	if(filename.isEmpty())
	{
		path = QFileDialog::getSaveFileName(this, tr("Export feedback"), ".", tr("File (*.xml)"));
	}

	QFile xmlFile(path);
	if(!xmlFile.open(QIODevice::WriteOnly))
	{
		emit logError(tr("Failed to open xml file!"), tr("Test::exportFeedback()"));
		return;
	}

	QXmlStreamWriter xmlWriter(&xmlFile);
	xmlWriter.setAutoFormatting(true);
	xmlWriter.writeStartDocument();
	QStandardItem *testItem = getItemGlobal("Feedback", _pRootObject);
	if(testItem)
	{
		xmlWriter.writeStartElement(testItem->text());
		if(testItem->hasChildren())
		{
			QMap<QString, QString> feedback = _pTestRunner->getFeedback();
			for(QMap<QString, QString>::iterator it = feedback.begin(); it != feedback.end(); it++)
			{
				xmlWriter.writeTextElement("Object", it.key());
				xmlWriter.writeTextElement("Value", it.value());
			}
		}
		xmlWriter.writeEndElement();
	}
	else
	{
		emit logError(tr("Feedback is not registered!"), tr("Test::exportFeedback()"));
	}
	xmlFile.close();
}

void VisionLive::Test::startTest()
{
	emit logMessage(tr("Starting test..."), tr("Test::startTest()"));

	actionStart->setEnabled(false);
	actionStop->setEnabled(true);

	for(int row = 0; row < _pCommandView->rowCount(); row++)
	{
		for(int col = 0; col < _pCommandView->columnCount(); col++)
		{
			_pCommandView->setItem(row, Test::STATE, new QTableWidgetItem("NOT EXECUTED"));
			_pCommandView->setItem(row, Test::RES, new QTableWidgetItem(""));
			_pCommandView->item(row, col)->setBackgroundColor(Qt::transparent);
		}
	}

	if(!_pTestRunner->isRunning())
	{
		_pTestRunner->startTest();
		while(!_pTestRunner->isRunning()){}

		emit logMessage(tr("Test started!"), tr("Test::startTest()"));
	}
}

void VisionLive::Test::stopTest()
{
	emit logMessage(tr("Stopping test..."), tr("Test::stopTest()"));

	_pTestRunner->stopTest();
}

void VisionLive::Test::testFinished()
{
	emit logMessage(tr("Test finished!"), tr("Test::testFinished()"));

	actionStart->setEnabled(true);
	actionStop->setEnabled(false);
}

void VisionLive::Test::commandRun(const QString &command, int index)
{
	_pCommandView->setItem(index, Test::STATE, new QTableWidgetItem(tr("RUNNING")));
	_pCommandView->resizeColumnsToContents();
	emit logMessage(tr("Executing \"") + command + "\"...", tr("Test::commandRun()"));
	for(int i = 0; i <  _pCommandView->columnCount(); i++)
	{
		_pCommandView->item(index, i)->setBackgroundColor(Qt::darkMagenta);
	}
	_pCommandView->scrollToItem(_pCommandView->item(index, 0), QAbstractItemView::PositionAtCenter);
}

void VisionLive::Test::commandPass(const QString &command, int index)
{
	_pCommandView->setItem(index, Test::STATE, new QTableWidgetItem(tr("EXECUTED")));
	_pCommandView->setItem(index, Test::RES, new QTableWidgetItem(tr("PASS")));
	_pCommandView->resizeColumnsToContents();
	emit logMessage(tr("PASS!"), tr("Test::commandPass()"));
	for(int i = 0; i <  _pCommandView->columnCount(); i++)
	{
		_pCommandView->item(index, i)->setBackgroundColor(Qt::green);
	}
}

void VisionLive::Test::commandFail(const QString &command, int index)
{
	_pCommandView->setItem(index, Test::STATE, new QTableWidgetItem(tr("EXECUTED")));
	_pCommandView->setItem(index, Test::RES, new QTableWidgetItem(tr("FAIL")));
	_pCommandView->resizeColumnsToContents();
	emit logMessage(tr("FAIL!"), tr("Test::commandFail()"));
	for(int i = 0; i < _pCommandView->columnCount(); i++)
	{
		_pCommandView->item(index, i)->setBackgroundColor(Qt::red);
	}
}

void VisionLive::Test::commandBroken(const QString &command, int index)
{
	_pCommandView->setItem(index, Test::STATE, new QTableWidgetItem(tr("EXECUTION FAIL")));
	_pCommandView->resizeColumnsToContents();
	emit logMessage(tr("BROKEN!"), tr("Test::commandBroken()"));
	for(int i = 0; i < _pCommandView->columnCount(); i++)
	{
		_pCommandView->item(index, i)->setBackgroundColor(Qt::red);
	}
}

void VisionLive::Test::FB_received(QMap<QString, QString> feedback)
{
	_pTestRunner->setFeedback(feedback);

	// Deregister all feedback objects
	QStandardItem *fbItem = getItemGlobal("Feedback", _pRootObject);
	if(fbItem)
	{
		removeObjects(fbItem);
	}

	// Register all feedback values 
	QMap<QString, QString>::const_iterator it;
	for(it = feedback.begin(); it != feedback.end(); it ++)
	{
		addObject(it.key(), "Feedback/" + it.key().split("_")[0]);
	}
}