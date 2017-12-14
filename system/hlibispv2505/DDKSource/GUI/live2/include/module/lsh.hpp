#ifndef MODULELSH_H
#define MODULELSH_H

#include "modulebase.hpp"
#include "ui_lsh.h"

#include "genconfig.hpp"

class LSHTuning;

namespace VisionLive
{

class ProxyHandler;

class ModuleLSH : public ModuleBase, public Ui::LSH
{
	Q_OBJECT

public:
    ModuleLSH(QWidget *parent = 0); // Class constructor
    ~ModuleLSH(); // Class destructor
	void removeObjectValueChangedMark();  // Removes object marking indicating value change
	void retranslate(); // Retranslates GUI
	void load(const ISPC::ParameterList &config); // Loads module configuration from config
	void save(ISPC::ParameterList &config); // Saves module configuration to config
    void setProxyHandler(ProxyHandler *proxy); // Sets ProxyHandler object
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
    void moduleAWBModeChanged(int mode); // Disables Apply_btn if AWB is on

protected slots:
	void addLSHGrid(); // Requests Commandhandler via ProxyHandler to add LSH grids
    void LSH_added(QString filename); // Recall from Commandhandler via ProxyHandler after succesfull adding
    void removeLSHGrid(); // Requests Commandhandler via ProxyHandler to remove LSH grids
    void LSH_removed(QString filename); // Recall from Commandhandler via ProxyHandler after succesfull removing
    void applyLSHGrid(); // Requests Commandhandler via ProxyHandler to apply LSH grid
    void LSH_applyed(QString filename); // Recall from Commandhandler via ProxyHandler after succesfull applying
    void LSH_received(QString filename); // Received info of current LSH matrix from ISP
    void selectAll(int state); // Selects or unselects all LSH grids
	void fileChanged(); // Called when LSH_file_le text has changed
	void loadFile(); // Called to load LSH grid file
	void tune(); // Opens lsh tunning window
    void initOutputFilesTable(); // Initializes table of generated grids
	void integrate(); // Called from LSHTuning after tuning finished to integrate results
    void deleteInput(); // Removes row from input table
    bool inputOk(QString filename, int temp); // Checkes if no namefile or temerature was used twice

};

} // namespace VisionLive

#endif // MODULELSH_H
