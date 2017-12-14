#ifndef MODULEEXPOSURE_H
#define MODULEEXPOSURE_H

#include "modulebase.hpp"
#include "ui_exposure.h"

namespace VisionLive
{

class ProxyHandler;

class ModuleExposure : public ModuleBase, public Ui::Exposure
{
	Q_OBJECT

public:
    ModuleExposure(QWidget *parent = 0); // Class constructor
    ~ModuleExposure(); // Class destructor
	void removeObjectValueChangedMark();  // Removes object marking indicating value change
	void retranslate(); // Retranslates GUI
	void load(const ISPC::ParameterList &config); // Loads module configuration from config
	void save(ISPC::ParameterList &config); // Saves module configuration to config
	void setProxyHandler(ProxyHandler *proxy); // Sets ProxyHandler object
	void resetConnectionDependantControls(); // Redoes all connection dependent actions

protected:
	void initModule();

private:
	bool _firstSensorRead;

protected slots:
	void exposureModeChanged(int state); // Called when exposure mode (Auto/Manual) has changed
	void autoFlickerModeChanged(int state); // Called when auto flicker detect mode (ON/OFF) has changed
	void SENSOR_received(QMap<QString, QString> params); // Called when sensor informations has been received
	void AE_received(QMap<QString, QString> params); // Called when AE informations has been received

};

} // namespace VisionLive

#endif // MODULEEXPOSURE_H
