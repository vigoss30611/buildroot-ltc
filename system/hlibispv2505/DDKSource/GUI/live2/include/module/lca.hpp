#ifndef MODULELCA_H
#define MODULELCA_H

#include "modulebase.hpp"
#include "ui_lca.h"

#include "genconfig.hpp"

class LSHTuning;

namespace VisionLive
{

class ModuleLCA : public ModuleBase, public Ui::LCA
{
	Q_OBJECT

public:
    ModuleLCA(QWidget *parent = 0); // Class constructor
    ~ModuleLCA(); // Class destructor
	void removeObjectValueChangedMark();  // Removes object marking indicating value change
	void retranslate(); // Retranslates GUI
	void load(const ISPC::ParameterList &config); // Loads module configuration from config
	void save(ISPC::ParameterList &config); // Saves module configuration to config
	void setCSS(CSS newCSS); // Sets css 

protected:
	void initModule();

private:
	blc_config _blcConfig; // Holds current ModuleBLC configuration
	GenConfig *_pGenConfig; // GenConfig object - used by _pLSHTuning
	LCATuning *_pLCATuning; // LCATuning object - used for tuning LCA grid

protected slots:
	void tune(); // Opens lca tunning window
	void integrate(); // Called from LCATuning after tuning finished to integrate results
	
};

} // namespace VisionLive

#endif // MODULELCA_H
