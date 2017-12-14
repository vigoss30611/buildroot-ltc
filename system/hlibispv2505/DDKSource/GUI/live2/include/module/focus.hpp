#ifndef MODULEFOCUS_H
#define MODULEFOCUS_H

#include "modulebase.hpp"
#include "ui_focus.h"

namespace VisionLive
{

class ProxyHandler;

class ModuleFocus : public ModuleBase, public Ui::Focus
{
	Q_OBJECT

public:
    ModuleFocus(QWidget *parent = 0); // Class constructor
    ~ModuleFocus(); // Class destructor
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
	bool _focusing;

protected slots:
	void focusModeChanged(int state); // Called when focus mode (Auto/Manual) has changed
	void focusSearchTriggered(); // Called when focus search is clicked
	void SENSOR_received(QMap<QString, QString> params); // Called when sensor informations has been received
	void AF_received(QMap<QString, QString> params); // Called when AF informations has been received
};

} // namespace VisionLive

#endif // MODULEFOCUS_H
