#ifndef MODULER2Y_H
#define MODULER2Y_H

#include "modulebase.hpp"
#include "ui_r2y.h"

namespace VisionLive
{

class ModuleR2Y : public ModuleBase, public Ui::R2Y
{
	Q_OBJECT

public:
    ModuleR2Y(QWidget *parent = 0); // Class constructor
    ~ModuleR2Y(); // Class destructor
	void removeObjectValueChangedMark();  // Removes object marking indicating value change
	void retranslate(); // Retranslates GUI
	void load(const ISPC::ParameterList &config); // Loads module configuration from config
	void save(ISPC::ParameterList &config) const; // Saves module configuration to config

protected:
	void initModule();
};

} // namespace VisionLive

#endif // MODULER2Y_H
