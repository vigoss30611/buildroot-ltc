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
	switch(_css)
	{
	case CUSTOM_CSS:
		auto_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
		updateSpeed_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		targetBrightness_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		measuredBrightness_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		bracketSize_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		flicker_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
		flickerHz_lb->setStyleSheet(CSS_CUSTOM_QCOMBOBOX);

		setExposure_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		sensorExposure_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		setGain_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		sensorGain_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);

		flickerAutoDetect_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
		targetGain_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		maxGain_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		maxExposure_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		break;

	case DEFAULT_CSS:
		auto_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
		updateSpeed_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		targetBrightness_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		bracketSize_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		flicker_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
		flickerHz_lb->setStyleSheet(CSS_DEFAULT_QCOMBOBOX);

		setExposure_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		sensorExposure_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		setGain_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		sensorGain_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);

		flickerAutoDetect_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
		targetGain_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		maxGain_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		maxExposure_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
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

	targetBrightness_sb->setMinimum(ae.AE_TARGET_BRIGHTNESS.min);
	measuredBrightness_sb->setMinimum(ae.AE_TARGET_BRIGHTNESS.min);
	updateSpeed_sb->setMinimum(ae.AE_UPDATE_SPEED.min);
	bracketSize_sb->setMinimum(ae.AE_BRACKET_SIZE.min);

	targetGain_sb->setMinimum(ae.AE_TARGET_GAIN.min);
	maxGain_sb->setMinimum(ae.AE_MAX_GAIN.min);
	maxExposure_sb->setMinimum(ae.AE_MAX_EXPOSURE.min);
	detectedFlickerHZ_sb->setMinimum(ae.AE_FLICKER_FREQ.min);
	
	//
	// Set maximum
	//

	targetBrightness_sb->setMaximum(ae.AE_TARGET_BRIGHTNESS.max);
	measuredBrightness_sb->setMaximum(ae.AE_TARGET_BRIGHTNESS.max);
	updateSpeed_sb->setMaximum(ae.AE_UPDATE_SPEED.max);
	bracketSize_sb->setMaximum(ae.AE_BRACKET_SIZE.max);

	targetGain_sb->setMaximum(ae.AE_TARGET_GAIN.max);
	maxGain_sb->setMaximum(ae.AE_MAX_GAIN.max);
	maxExposure_sb->setMaximum(ae.AE_MAX_EXPOSURE.max);
	detectedFlickerHZ_sb->setMaximum(ae.AE_FLICKER_FREQ.max);
	
	//
	// Set precision
	//

	//
	// Set value
	//

	if(config.exists(AE_ENABLE))
	{
		auto_cb->setChecked(config.getParameter(AE_ENABLE)->get<bool>());
	}
	flicker_cb->setChecked(ae.getFlickerRejection());
    flickerHz_lb->setCurrentIndex((ae.getFlickerFreqConfig() == 50.0f) ? 0 : 1);
	targetBrightness_sb->setValue(ae.getTargetBrightness());
	updateSpeed_sb->setValue(ae.getUpdateSpeed());
	bracketSize_sb->setValue(ae.getTargetBracket());

	flickerAutoDetect_cb->setChecked(ae.getAutoFlickerRejection());
	targetGain_sb->setValue(ae.AE_TARGET_GAIN.def);
	maxGain_sb->setValue(ae.getMaxAeGain());
	maxExposure_sb->setValue(ae.getMaxAeExposure());

	if(config.exists(SENSOR_SET_EXPOSURE))
	{
		setExposure_sb->setValue(((double)config.getParameter(SENSOR_SET_EXPOSURE)->get<int>()));
	}
	if(config.exists(SENSOR_SET_GAIN))
	{
		setGain_sb->setValue(config.getParameter(SENSOR_SET_GAIN)->get<double>());
	}
}

void VisionLive::ModuleExposure::save(ISPC::ParameterList &config)
{
	ISPC::Parameter ae_enable(AE_ENABLE, auto_cb->isChecked()? "1" : "0");
	config.addParameter(ae_enable, true);
	ISPC::Parameter ae_flicker_enable(ISPC::ControlAE::AE_FLICKER.name, flicker_cb->isChecked()? "1" : "0");
	config.addParameter(ae_flicker_enable, true);
	ISPC::Parameter ae_flicker_freq(ISPC::ControlAE::AE_FLICKER_FREQ.name, (flickerHz_lb->currentIndex() == 0)? "50" : "60");
	config.addParameter(ae_flicker_freq, true);
	ISPC::Parameter ae_brightness(ISPC::ControlAE::AE_TARGET_BRIGHTNESS.name, std::string(QString::number(targetBrightness_sb->value()).toLatin1()));
	config.addParameter(ae_brightness, true);
	ISPC::Parameter ae_updade_speed(ISPC::ControlAE::AE_UPDATE_SPEED.name, std::string(QString::number(updateSpeed_sb->value()).toLatin1()));
	config.addParameter(ae_updade_speed, true);
	ISPC::Parameter ae_bracket_size(ISPC::ControlAE::AE_BRACKET_SIZE.name, std::string(QString::number(bracketSize_sb->value()).toLatin1()));
	config.addParameter(ae_bracket_size, true);

	ISPC::Parameter ae_autoFlicker(ISPC::ControlAE::AE_FLICKER_AUTODETECT.name, flickerAutoDetect_cb->isChecked()? "1" : "0");
	config.addParameter(ae_autoFlicker, true);
	ISPC::Parameter ae_targetGain(ISPC::ControlAE::AE_TARGET_GAIN.name, std::string(QString::number(targetGain_sb->value()).toLatin1()));
	config.addParameter(ae_targetGain, true);
	ISPC::Parameter ae_maxGain(ISPC::ControlAE::AE_MAX_GAIN.name, std::string(QString::number(maxGain_sb->value()).toLatin1()));
	config.addParameter(ae_maxGain, true);
	ISPC::Parameter ae_maxExposure(ISPC::ControlAE::AE_MAX_EXPOSURE.name, std::string(QString::number((IMG_UINT32)maxExposure_sb->value()).toLatin1()));
	config.addParameter(ae_maxExposure, true);

    ISPC::Parameter sensor_exposure(SENSOR_SET_EXPOSURE, std::string(QString::number((IMG_UINT32)(setExposure_sb->value())).toLatin1()));
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
		_pProxyHandler->AE_set(auto_cb->isChecked());
	}
}

//
// Protected Functions
//

void VisionLive::ModuleExposure::initModule()
{
	_name = windowTitle();

	_firstSensorRead = false;

	QObject::connect(auto_cb, SIGNAL(stateChanged(int)), this, SLOT(exposureModeChanged(int)));
	QObject::connect(flickerAutoDetect_cb, SIGNAL(stateChanged(int)), this, SLOT(autoFlickerModeChanged(int)));

	// To set initial state of groupboxes
	exposureModeChanged(auto_cb->isChecked());
}

//
// Protected Slots
//

void VisionLive::ModuleExposure::exposureModeChanged(int state)
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

	updateSpeed_sb->setEnabled(state);
	targetBrightness_sb->setEnabled(state);
	bracketSize_sb->setEnabled(state);

	flickerAutoDetect_cb->setEnabled(state);
	targetGain_sb->setEnabled(state && flickerAutoDetect_cb->isChecked());
	maxGain_sb->setEnabled(state && flickerAutoDetect_cb->isChecked());
	maxExposure_sb->setEnabled(state && flickerAutoDetect_cb->isChecked());

	flicker_cb->setEnabled(state && !flickerAutoDetect_cb->isChecked());
	flickerHz_lb->setEnabled(state && !flickerAutoDetect_cb->isChecked());

	setExposure_sb->setEnabled(!state);
	setGain_sb->setEnabled(!state);

	if(_pProxyHandler)
	{
		_pProxyHandler->AE_set(state);
	}
}

void VisionLive::ModuleExposure::autoFlickerModeChanged(int state)
{
	targetGain_sb->setEnabled(state && auto_cb->isChecked());
	maxGain_sb->setEnabled(state && auto_cb->isChecked());
	maxExposure_sb->setEnabled(state && auto_cb->isChecked());

	flicker_cb->setEnabled(!state && auto_cb->isChecked());
	flickerHz_lb->setEnabled(!state && auto_cb->isChecked());
}

void VisionLive::ModuleExposure::SENSOR_received(QMap<QString, QString> params)
{
	if(!_firstSensorRead)
	{
		setExposure_sb->setRange(params.find("MinExposure").value().toInt(), params.find("MaxExposure").value().toInt());
		setExposure_sb->setValue(params.find("Exposure").value().toInt());
		setGain_sb->setRange(params.find("MinGain").value().toDouble(), params.find("MaxGain").value().toDouble());
		setGain_sb->setValue(params.find("Gain").value().toDouble());

		targetGain_sb->setRange(params.find("MinGain").value().toDouble(), params.find("MaxGain").value().toDouble());
		maxGain_sb->setRange(params.find("MinGain").value().toDouble(), params.find("MaxGain").value().toDouble());
		maxExposure_sb->setRange(params.find("MinExposure").value().toInt(), params.find("MaxExposure").value().toInt());

		sensorExposure_sb->setRange(params.find("MinExposure").value().toInt(), params.find("MaxExposure").value().toInt());
		sensorGain_sb->setRange(params.find("MinGain").value().toDouble(), params.find("MaxGain").value().toDouble());
	}

	sensorExposure_sb->setValue(params.find("Exposure").value().toInt());
	sensorGain_sb->setValue(params.find("Gain").value().toDouble());

	_firstSensorRead = true;
}

void VisionLive::ModuleExposure::AE_received(QMap<QString, QString> params)
{
	measuredBrightness_sb->setValue(params.find("MeasuredBrightness").value().toDouble());
	detectedFlickerHZ_sb->setValue(params.find("DetectedFlickerFrequency").value().toDouble());
}