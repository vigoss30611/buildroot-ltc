#ifndef MODULEY2R_H
#define MODULEY2R_H

#include "modulebase.hpp"
#include "ui_y2r.h"

namespace VisionLive
{

class ModuleY2R : public ModuleBase, public Ui::Y2R
{
	Q_OBJECT

public:
    ModuleY2R(QWidget *parent = 0); // Class constructor
    ~ModuleY2R(); // Class destructor
	void removeObjectValueChangedMark();  // Removes object marking indicating value change
	void retranslate(); // Retranslates GUI
	void load(const ISPC::ParameterList &config); // Loads module configuration from config
	void save(ISPC::ParameterList &config); // Saves module configuration to config

protected:
	void initModule();
};

} // namespace VisionLive

#endif // MODULEY2R_H
