#ifndef MODULEBLC_H
#define MODULEBLC_H

#include "modulebase.hpp"
#include "ui_blc.h"

namespace VisionLive
{

class ModuleBLC : public ModuleBase, public Ui::BLC
{
	Q_OBJECT

public:
    ModuleBLC(QWidget *parent = 0); // Class constructor
    ~ModuleBLC(); // Class destructor
	void removeObjectValueChangedMark();  // Removes object marking indicating value change
	void retranslate(); // Retranslates GUI
	void load(const ISPC::ParameterList &config); // Loads module configuration from config
	void save(ISPC::ParameterList &config) const; // Saves module configuration to config

protected:
	void initModule();

signals:
	void moduleBLCChanged(int sensorBlack0, int sensorBlack1, int sensorBlack2, int sensorBlack3, int systemBlack); // Indicates new state of BLC module to CCM module

protected slots:
	void moduleBLCChanged(); // Emits moduleBLCChanged() signal when any of BLC values change

};

} // namespace VisionLive

#endif // MODULEBLC_H
