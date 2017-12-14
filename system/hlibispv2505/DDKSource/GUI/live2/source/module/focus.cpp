#include "focus.hpp"
#include "proxyhandler.hpp"

#include "ispc/ControlAF.h"

#define AF_ENABLE "AF_ENABLE"
#define AF_DISTANCE "AF_DISTANCE"

//
// Public Functions
//

VisionLive::ModuleFocus::ModuleFocus(QWidget *parent): ModuleBase(parent)
{
	Ui::Focus::setupUi(this);

	retranslate();

	initModule();

	initObjectsConnections();
}

VisionLive::ModuleFocus::~ModuleFocus()
{
}

void VisionLive::ModuleFocus::removeObjectValueChangedMark()
{
	switch(_css)
	{
	case CUSTOM_CSS:
		auto_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
		setDistance_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		break;

	case DEFAULT_CSS:
		auto_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
		setDistance_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		break;
	}
}

void VisionLive::ModuleFocus::retranslate()
{
	Ui::Focus::retranslateUi(this);
}

void VisionLive::ModuleFocus::load(const ISPC::ParameterList &config)
{
	ISPC::ControlAF af;
	af.load(config);

	//
	// Set minimum
	//

	//
	// Set maximum
	//

	//
	// Set precision
	//

	//
	// Set value
	//

	if(config.exists(AF_ENABLE))
	{
		auto_cb->setChecked(config.getParameter(AF_ENABLE)->get<bool>());
	}
	if(config.exists(AF_DISTANCE))
	{
		setDistance_sb->setValue(config.getParameter(AF_DISTANCE)->get<int>());
	}
}

void VisionLive::ModuleFocus::save(ISPC::ParameterList &config)
{
	ISPC::Parameter af_enable(AF_ENABLE, auto_cb->isChecked()? "1" : "0");
	config.addParameter(af_enable, true);
	ISPC::Parameter af_distance(AF_DISTANCE, std::string(QString::number(setDistance_sb->value()).toLatin1()));
	config.addParameter(af_distance, true);
}

void VisionLive::ModuleFocus::setProxyHandler(ProxyHandler *proxy) 
{ 
	_pProxyHandler = proxy; 

	QObject::connect(_pProxyHandler, SIGNAL(SENSOR_received(QMap<QString, QString>)), this, SLOT(SENSOR_received(QMap<QString, QString>)));
	QObject::connect(_pProxyHandler, SIGNAL(AF_received(QMap<QString, QString>)), this, SLOT(AF_received(QMap<QString, QString>)));
}

void VisionLive::ModuleFocus::resetConnectionDependantControls()
{
	if(_pProxyHandler)
	{
		_pProxyHandler->AF_set(auto_cb->isChecked());
	}
}

//
// Protected Functions
//

void VisionLive::ModuleFocus::initModule()
{
	_name = windowTitle();

	_firstSensorRead = false;
	_focusing = false;

	QObject::connect(auto_cb, SIGNAL(stateChanged(int)), this, SLOT(focusModeChanged(int)));
	QObject::connect(search_btn, SIGNAL(clicked()), this, SLOT(focusSearchTriggered()));

	// To set initial state of groupboxes
	focusModeChanged(auto_cb->isChecked());
}

//
// Protected Slots
//

void VisionLive::ModuleFocus::focusSearchTriggered()
{
	emit logAction("CLICK " + sender()->objectName());

	if(_pProxyHandler)
	{
		_pProxyHandler->AF_search();
	}
}

void VisionLive::ModuleFocus::focusModeChanged(int state)
{
	if(state)
	{
		auto_gb->setStyleSheet(CSS_QGROUPBOX_SELECTED);
		manual_gb->setStyleSheet((_css == DEFAULT_CSS)? CSS_DEFAULT_QGROUPBOX : CSS_CUSTOM_QGROUPBOX);
	}
	else
	{
		auto_gb->setStyleSheet((_css == DEFAULT_CSS)? CSS_DEFAULT_QGROUPBOX : CSS_CUSTOM_QGROUPBOX);
		manual_gb->setStyleSheet(CSS_QGROUPBOX_SELECTED);
	}

	search_btn->setEnabled(state);
	setDistance_sb->setEnabled(!state);

	if(_pProxyHandler)
	{
		_pProxyHandler->AF_set(state);
	}
}

void VisionLive::ModuleFocus::SENSOR_received(QMap<QString, QString> params)
{
	if(!_firstSensorRead)
	{
		// Disable Focus tab if sensor does not support focus
		QMap<QString, QString>::const_iterator it = params.find("FocusSupported");
		if(it != params.end() && !params.find("FocusSupported").value().toInt())
		{
			this->setEnabled(false);
		}

		setDistance_sb->setRange(params.find("MinFocus").value().toInt(), params.find("MaxFocus").value().toInt());
		sensorDistance_sb->setRange(params.find("MinFocus").value().toInt(), params.find("MaxFocus").value().toInt());
	}

	sensorDistance_sb->setValue(params.find("Focus").value().toInt());

	_firstSensorRead = true;
}

void VisionLive::ModuleFocus::AF_received(QMap<QString, QString> params)
{
	QString state = QString(ISPC::ControlAF::StateName((ISPC::ControlAF::State)params.find("FocusState").value().toInt()));
	QString scanState = QString(ISPC::ControlAF::ScanStateName((ISPC::ControlAF::ScanState)params.find("FocusScanState").value().toInt()));

	if(auto_cb->isChecked())
	{
		if(state == "AF_IDLE")
		{
			// Enable change focus mode or trigger search again
			auto_cb->setEnabled(true);
			search_btn->setEnabled(true);
			_focusing = false;
		}
		else
		{
			// To change focus mode or trigger search again have to wait to finish focusing
			auto_cb->setEnabled(false);
			search_btn->setEnabled(false);
		}
	}

	state_lbl->setText(state);
	scanState_lbl->setText(scanState);
}