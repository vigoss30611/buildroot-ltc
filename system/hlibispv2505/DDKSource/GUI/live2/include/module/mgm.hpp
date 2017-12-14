#ifndef MODULEMGM_H
#define MODULEMGM_H

#include "modulebase.hpp"
#include "ui_mgm.h"

namespace VisionLive
{

class ModuleMGM : public ModuleBase, public Ui::MGM
{
	Q_OBJECT

public:
    ModuleMGM(QWidget *parent = 0); // Class constructor
    ~ModuleMGM(); // Class destructor
	void removeObjectValueChangedMark();  // Removes object marking indicating value change
	void retranslate(); // Retranslates GUI
	void load(const ISPC::ParameterList &config); // Loads module configuration from config
	void save(ISPC::ParameterList &config); // Saves module configuration to config

protected:
	void initModule();
};

} // namespace VisionLive

#endif // MODULEMGM_H
