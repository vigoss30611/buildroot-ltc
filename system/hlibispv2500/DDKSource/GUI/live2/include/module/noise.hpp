#ifndef MODULENOISE_H
#define MODULENOISE_H

#include "modulebase.hpp"
#include "ui_noise.h"

namespace VisionLive
{

class ModuleNoise : public ModuleBase, public Ui::Noise
{
	Q_OBJECT

public:
    ModuleNoise(QWidget *parent = 0); // Class constructor
    ~ModuleNoise(); // Class destructor
	void removeObjectValueChangedMark();  // Removes object marking indicating value change
	void retranslate(); // Retranslates GUI
	void load(const ISPC::ParameterList &config); // Loads module configuration from config
	void save(ISPC::ParameterList &config) const; // Saves module configuration to config
	void resetConnectionDependantControls(); // Redoes all connection dependent actions

protected:
	void initModule(); // Initializes module

protected slots:
	void DNS_set(int enable); // Request DNS control module enable/disable

};

} // namespace VisionLive

#endif // MODULENOISE_H
