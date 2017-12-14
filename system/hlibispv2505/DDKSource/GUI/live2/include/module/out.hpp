#ifndef MODULEOUT_H
#define MODULEOUT_H

#include "modulebase.hpp"
#include "ui_out.h"

namespace VisionLive
{

class ProxyHandler;

class ModuleOUT : public ModuleBase, public Ui::OUT
{
	Q_OBJECT

public:
    ModuleOUT(QWidget *parent = 0); // Class constructor
    ~ModuleOUT(); // Class destructor
	void removeObjectValueChangedMark();  // Removes object marking indicating value change
	void retranslate(); // Retranslates GUI
	void load(const ISPC::ParameterList &config); // Loads module configuration from config
	void save(ISPC::ParameterList &config); // Saves module configuration to config
	void setProxyHandler(ProxyHandler *proxy); // Sets ProxyHandler object

protected:
	void initModule();

private:
	bool _firstSensorRead;
    bool _firstLoad;

protected slots:
	void encoderFormatChanged(int index); // Called when encoder format changed 
	void displayFormatChanged(int index); // Called when display format changed 
	void dataExtractionFormatChanged(int index); // Called when data extraction format changed 
	void HDRFormatChanged(int index); // Called when HDR format changed 
	void RAW2DFormatChanged(int index); // Called when RAW2D format changed 
	void SENSOR_received(QMap<QString, QString> params); // Called when sensor informations has been received
};

} // namespace VisionLive

#endif // MODULEOUT_H
