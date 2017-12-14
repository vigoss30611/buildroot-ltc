#include "test.hpp"
#include "css.hpp"
#include "objectregistry.hpp"
#include "testrunner.hpp"
#include "proxyhandler.hpp"
#include "htmlviewer.hpp"

#include <qtreeview.h>
#include <qstandarditemmodel.h>
#include <qtablewidget.h>
#include "tablogview.hpp"
#include <qfiledialog.h>
#include <qxmlstream.h>
#include <QCloseEvent>

//
// Public Functions
//

VisionLive::Test::Test(ObjectRegistry *objReg, const QString &testFile, IMG_BOOL8 testStart, QMainWindow *parent): QMainWindow(parent)
{
	Ui::Test::setupUi(this);

	retranslate();

	_pObjReg = objReg;
	_pProxyHandler = nullptr;
	_pTestRunner = nullptr;
	_pObjectView = nullptr;
	_pObjectViewModel = nullptr;
	_pRootObject = nullptr;
	_pCommandView = nullptr;
	_pLogView = nullptr;
	_pHelp = nullptr;

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
	if(_pHelp) delete _pHelp;
}

void VisionLive::Test::setProxyHandler(ProxyHandler *proxyHandler)
{
	_pProxyHandler = proxyHandler;

	if(_pProxyHandler)
	{
		QObject::connect(_pTestRunner, SIGNAL(FB_get()), _pProxyHandler, SLOT(FB_get()));
		QObject::connect(_pTestRunner, SIGNAL(saveRec_TF(QString,QString,bool)), _pProxyHandler, SLOT(saveRec_TF(QString,QString,bool)));
	}
}

//
// Protected Functions
//

void VisionLive::Test::closeEvent(QCloseEvent *event)
{
    emit testClosed();
    event->accept();
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
	//QObject::connect(_pTestRunner, SIGNAL(exportFeedback(const QString &)), this, SLOT(exportFeedback(const QString &)));
	QObject::connect(_pTestRunner, SIGNAL(saveLog(const QString &)), this, SLOT(saveLog(const QString &)));
}

void VisionLive::Test::initMenuBar()
{
	actionStart->setEnabled(false);
	actionStop->setEnabled(false);

	QObject::connect(actionLoad_Test, SIGNAL(triggered()), this, SLOT(loadTest()));
    QObject::connect(actionReload_Test, SIGNAL(triggered()), this, SLOT(reloadTest()));
	QObject::connect(actionSave_Log, SIGNAL(triggered()), this, SLOT(saveLog()));
	QObject::connect(actionExport_Object_List, SIGNAL(triggered()), this, SLOT(exportObjectList()));
	//QObject::connect(actionExport_Feedback, SIGNAL(triggered()), this, SLOT(exportFeedback()));
	QObject::connect(actionStart, SIGNAL(triggered()), this, SLOT(startTest()));
	QObject::connect(actionStop, SIGNAL(triggered()), this, SLOT(stopTest()));
	QObject::connect(actionHelp, SIGNAL(triggered()), this, SLOT(help()));
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

	return nullptr;
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

	return nullptr;
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

void  VisionLive::Test::connected()
{
	if(_pTestRunner)
	{
		_pTestRunner->setConnected(true);
	}
}

void  VisionLive::Test::disconnected()
{
	if(_pTestRunner)
	{
		_pTestRunner->setConnected(false);
	}
}

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
		if(item != nullptr)
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
	if(item != nullptr)
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
    QString tmp;

	if(fileName.isEmpty())
	{
        tmp = QFileDialog::getOpenFileName(this, tr("Load test"), ".", tr("Test file (*.txt)"));
        if (tmp.isEmpty()) return;
        else _fileName = tmp;
	}
	else _fileName = fileName;

	//if(_fileName.isEmpty()) return;

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
	//_pCommandView->resizeColumnsToContents();

	actionStart->setEnabled(true);
}

void VisionLive::Test::reloadTest()
{
    emit logMessage(QStringLiteral("Reloading test..."), QStringLiteral("Test::reloadTest()"));
    loadTest(_fileName);
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

	logFile.write(_pLogView->getLogs().toLatin1());

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
		_pCommandView->item(index, i)->setTextColor(Qt::white);
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
		_pCommandView->item(index, i)->setTextColor(Qt::white);
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
		_pCommandView->item(index, i)->setTextColor(Qt::white);
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
		_pCommandView->item(index, i)->setTextColor(Qt::white);
	}
}

void VisionLive::Test::FB_received(const ISPC::ParameterList &intList, const ISPC::ParameterList extList)
{
    if (_pTestRunner)
    {
        _pTestRunner->FB_received(intList, extList);
    }
}

void VisionLive::Test::help()
{
	if(_pHelp)
	{
		delete _pHelp;
	}

	_pHelp = new HTMLViewer();
	QObject::connect(_pHelp, SIGNAL(loadingFailed()), this, SLOT(helpLoadingFailed()));
	_pHelp->show();

	QDir dir;
	dir.setPath(dir.absolutePath());
	if(dir.exists())
	{
		_pHelp->loadFile(dir.path() + QStringLiteral("/resources/tfHelp.html"));
	}
	else
	{
		delete _pHelp;
		_pHelp = nullptr;
		emit logError(QStringLiteral("Documentation path ") + dir.path() + QStringLiteral(" not found!"),  __FUNCTION__ + QStringLiteral(" ") + QString::number(__LINE__)); 
	}
}

void VisionLive::Test::helpLoadingFailed()
{
	emit logError(tr("Failed to load help!"), __FUNCTION__ + QStringLiteral(" ") + QString::number(__LINE__));
}