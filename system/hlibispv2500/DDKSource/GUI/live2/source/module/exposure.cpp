#include "exposure.hpp"
#include "proxyhandler.hpp"

#include "ispc/ControlAE.h"

#define AE_ENABLE "AE_ENABLE"
#define SENSOR_SET_EXPOSURE "SENSOR_SET_EXPOSURE"
#define SENSOR_SET_GAIN "SENSOR_SET_GAIN"

//
// Public Functions
//

VisionLive::ModuleExposure::ModuleExposure(QWidget *parent): ModuleBase(parent)
{
	Ui::Exposure::setupUi(this);

	retranslate();

	initModule();

	initObjectsConnections();
}

VisionLive::ModuleExposure::~ModuleExposure()
{
}

void VisionLive::ModuleExposure::removeObjectValueChangedMark()
{
	switch(css)
	{
	case CUSTOM_CSS:
		AE_enable_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
		AE_updateSpeed_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		AE_targetBrightness_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		AE_measuredBrightness_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		AE_bracketSize_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		AE_flickerRejectionEnable_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
		AE_flickerRejectionHz_lb->setStyleSheet(CSS_CUSTOM_QCOMBOBOX);

		setExposure_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		sensorExposure_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		setGain_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		sensorGain_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		break;

	case DEFAULT_CSS:
		AE_enable_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
		AE_updateSpeed_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		AE_targetBrightness_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		AE_bracketSize_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		AE_flickerRejectionEnable_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
		AE_flickerRejectionHz_lb->setStyleSheet(CSS_DEFAULT_QCOMBOBOX);

		setExposure_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		sensorExposure_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		setGain_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		sensorGain_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		break;
	}
}

void VisionLive::ModuleExposure::retranslate()
{
	Ui::Exposure::retranslateUi(this);
}

void VisionLive::ModuleExposure::load(const ISPC::ParameterList &config)
{
	ISPC::ControlAE ae;
	ae.load(config);

	//
	// Set minimum
	//

	AE_targetBrightness_sb->setMinimum(ae.AE_TARGET_BRIGHTNESS.min);
	AE_measuredBrightness_sb->setMinimum(ae.AE_TARGET_BRIGHTNESS.min);
	AE_updateSpeed_sb->setMinimum(ae.AE_UPDATE_SPEED.min);
	AE_bracketSize_sb->setMinimum(ae.AE_BRACKET_SIZE.min);
	
	//
	// Set maximum
	//

	AE_targetBrightness_sb->setMaximum(ae.AE_TARGET_BRIGHTNESS.max);
	AE_measuredBrightness_sb->setMaximum(ae.AE_TARGET_BRIGHTNESS.max);
	AE_updateSpeed_sb->setMaximum(ae.AE_UPDATE_SPEED.max);
	AE_bracketSize_sb->setMaximum(ae.AE_BRACKET_SIZE.max);
	
	//
	// Set precision
	//

	//
	// Set value
	//

	if(config.exists(AE_ENABLE))
	{
		AE_enable_cb->setChecked(config.getParameter(AE_ENABLE)->get<bool>());
	}
	AE_flickerRejectionEnable_cb->setChecked(ae.getFlickerRejection());
	AE_flickerRejectionHz_lb->setCurrentIndex((ae.getFlickerRejectionFrequency() == 50.0f)? 0 : 1);
	AE_targetBrightness_sb->setValue(ae.getTargetBrightness());
	AE_updateSpeed_sb->setValue(ae.getUpdateSpeed());
	AE_bracketSize_sb->setValue(ae.getTargetBracket());

	if(config.exists(SENSOR_SET_EXPOSURE))
	{
		setExposure_sb->setValue(((double)config.getParameter(SENSOR_SET_EXPOSURE)->get<int>()));
	}
	if(config.exists(SENSOR_SET_GAIN))
	{
		setGain_sb->setValue(config.getParameter(SENSOR_SET_GAIN)->get<double>());
	}
}

void VisionLive::ModuleExposure::save(ISPC::ParameterList &config) const
{
	ISPC::Parameter ae_enable(AE_ENABLE, AE_enable_cb->isChecked()? "1" : "0");
	config.addParameter(ae_enable, true);
	ISPC::Parameter ae_flicker_enable(ISPC::ControlAE::AE_FLICKER.name, AE_flickerRejectionEnable_cb->isChecked()? "1" : "0");
	config.addParameter(ae_flicker_enable, true);
	ISPC::Parameter ae_flicker_freq(ISPC::ControlAE::AE_FLICKER_FREQ.name, (AE_flickerRejectionHz_lb->currentIndex() == 0)? "50" : "60");
	config.addParameter(ae_flicker_freq, true);
	ISPC::Parameter ae_brightness(ISPC::ControlAE::AE_TARGET_BRIGHTNESS.name, std::string(QString::number(AE_targetBrightness_sb->value()).toLatin1()));
	config.addParameter(ae_brightness, true);
	ISPC::Parameter ae_updade_speed(ISPC::ControlAE::AE_UPDATE_SPEED.name, std::string(QString::number(AE_updateSpeed_sb->value()).toLatin1()));
	config.addParameter(ae_updade_speed, true);
	ISPC::Parameter ae_bracket_size(ISPC::ControlAE::AE_BRACKET_SIZE.name, std::string(QString::number(AE_bracketSize_sb->value()).toLatin1()));
	config.addParameter(ae_bracket_size, true);

	ISPC::Parameter sensor_exposure(SENSOR_SET_EXPOSURE, std::string(QString::number((int)(setExposure_sb->value())).toLatin1()));
	config.addParameter(sensor_exposure, true);
	ISPC::Parameter sensor_gain(SENSOR_SET_GAIN, std::string(QString::number(setGain_sb->value()).toLatin1()));
	config.addParameter(sensor_gain, true);
}

void VisionLive::ModuleExposure::setProxyHandler(ProxyHandler *proxy) 
{ 
	_pProxyHandler = proxy; 

	QObject::connect(_pProxyHandler, SIGNAL(SENSOR_received(QMap<QString, QString>)), this, SLOT(SENSOR_received(QMap<QString, QString>)));
	QObject::connect(_pProxyHandler, SIGNAL(AE_received(QMap<QString, QString>)), this, SLOT(AE_received(QMap<QString, QString>)));
} 

void VisionLive::ModuleExposure::resetConnectionDependantControls()
{
	if(_pProxyHandler)
	{
		_pProxyHandler->AE_set(AE_enable_cb->isChecked());
	}
}

//
// Protected Functions
//

void VisionLive::ModuleExposure::initModule()
{
	_name = windowTitle();

	_firstSensorRead = false;

	QObject::connect(AE_enable_cb, SIGNAL(stateChanged(int)), this, SLOT(exposureModeChanged(int)));

	// To set initial state of groupboxes
	exposureModeChanged(AE_enable_cb->isChecked());
}

//
// Protected Slots
//

void VisionLive::ModuleExposure::exposureModeChanged(int state)
{
	if(state)
	{
		auto_gb->setStyleSheet(CSS_QGROUPBOX_SELECTED);
		manual_gb->setStyleSheet((css == DEFAULT_CSS)? CSS_DEFAULT_QGROUPBOX : CSS_CUSTOM_QGROUPBOX);
	}
	else
	{
		auto_gb->setStyleSheet((css == DEFAULT_CSS)? CSS_DEFAULT_QGROUPBOX : CSS_CUSTOM_QGROUPBOX);
		manual_gb->setStyleSheet(CSS_QGROUPBOX_SELECTED);
	}

	AE_updateSpeed_sb->setEnabled(state);
	AE_targetBrightness_sb->setEnabled(state);
	AE_bracketSize_sb->setEnabled(state);
	AE_flickerRejectionEnable_cb->setEnabled(state);
	AE_flickerRejectionHz_lb->setEnabled(state);

	setExposure_sb->setEnabled(!state);
	setGain_sb->setEnabled(!state);

	if(_pProxyHandler)
	{
		_pProxyHandler->AE_set(state);
	}
}

void VisionLive::ModuleExposure::SENSOR_received(QMap<QString, QString> params)
{
	if(!_firstSensorRead)
	{
		setExposure_sb->setRange(params.find("MinExposure").value().toInt(), params.find("MaxExposure").value().toInt());
		setExposure_sb->setValue(params.find("Exposure").value().toInt());
		setGain_sb->setRange(params.find("MinGain").value().toDouble(), params.find("MaxGain").value().toDouble());
		setGain_sb->setValue(params.find("Gain").value().toDouble());

		sensorExposure_sb->setRange(params.find("MinExposure").value().toInt(), params.find("MaxExposure").value().toInt());
		sensorGain_sb->setRange(params.find("MinGain").value().toDouble(), params.find("MaxGain").value().toDouble());
	}

	sensorExposure_sb->setValue(params.find("Exposure").value().toInt());
	sensorGain_sb->setValue(params.find("Gain").value().toDouble());

	_firstSensorRead = true;
}

void VisionLive::ModuleExposure::AE_received(QMap<QString, QString> params)
{
	AE_measuredBrightness_sb->setValue(params.find("MeasuredBrightness").value().toDouble());
}