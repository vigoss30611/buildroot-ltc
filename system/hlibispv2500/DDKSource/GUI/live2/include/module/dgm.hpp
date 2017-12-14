#ifndef MODULEDGM_H
#define MODULEDGM_H

#include "modulebase.hpp"
#include "ui_dgm.h"

namespace VisionLive
{

class ModuleDGM : public ModuleBase, public Ui::DGM
{
	Q_OBJECT

public:
    ModuleDGM(QWidget *parent = 0); // Class constructor
    ~ModuleDGM(); // Class destructor
	void removeObjectValueChangedMark();  // Removes object marking indicating value change
	void retranslate(); // Retranslates GUI
	void load(const ISPC::ParameterList &config); // Loads module configuration from config
	void save(ISPC::ParameterList &config) const; // Saves module configuration to config

protected:
	void initModule();
};

} // namespace VisionLive

#endif // MODULEDGM_H
