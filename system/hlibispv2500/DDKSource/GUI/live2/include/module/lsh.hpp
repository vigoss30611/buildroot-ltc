#ifndef MODULELSH_H
#define MODULELSH_H

#include "modulebase.hpp"
#include "ui_lsh.h"

#include "genconfig.hpp"

class LSHTuning;

namespace VisionLive
{

class ModuleLSH : public ModuleBase, public Ui::LSH
{
	Q_OBJECT

public:
    ModuleLSH(QWidget *parent = 0); // Class constructor
    ~ModuleLSH(); // Class destructor
	void removeObjectValueChangedMark();  // Removes object marking indicating value change
	void retranslate(); // Retranslates GUI
	void load(const ISPC::ParameterList &config); // Loads module configuration from config
	void save(ISPC::ParameterList &config) const; // Saves module configuration to config
	void setCSS(CSS newCSS); // Sets css 

protected:
	void initModule(); // Initializes module

private:
	std::string _lastLSHFile; // Holds lastly loaded LSH grid filename
	bool _lshApplyed; // Indicates that LSH grid has been applyed

	blc_config _blcConfig; // Holds current ModuleBLC configuration
	GenConfig *_pGenConfig; // GenConfig object - used by _pLSHTuning
	LSHTuning *_pLSHTuning; // LSHTuning object - used for tuning LSH grid

public slots:
	void moduleBLCChanged(int sensorBlack0, int sensorBlack1, int sensorBlack2, int sensorBlack3, int systemBlack); // Updates BLC params for tuning

protected slots:
	void enableChanged(); // Called to change LSH_gb backgroud color indicating LHS_enable_cb is checked
	void applyLSHGrid(); // Requests Commandhandler via ProxyHandler to apply LSH grid
	void fileChanged(); // Called when LSH_file_le text has changed
	void loadFile(); // Called to load LSH grid file
	void tune(); // Opens lsh tunning window
	void integrate(); // Called from LSHTuning after tuning finished to integrate results

};

} // namespace VisionLive

#endif // MODULELSH_H
