#ifndef MODULELBC_H
#define MODULELBC_H

#include "modulebase.hpp"
#include "ui_lbc.h"

namespace VisionLive
{

class LBCConfig;
class ProxyHandler;

class ModuleLBC : public ModuleBase, public Ui::LBC
{
	Q_OBJECT

public:
    ModuleLBC(QWidget *parent = 0); // Class constructor
    ~ModuleLBC(); // Class destructor
	void removeObjectValueChangedMark();  // Removes object marking indicating value change
	void retranslate(); // Retranslates GUI
	void load(const ISPC::ParameterList &config); // Loads module configuration from config
	void save(ISPC::ParameterList &config) const; // Saves module configuration to config
	void setProxyHandler(ProxyHandler *proxy); // Sets ProxyHandler object
	void resetConnectionDependantControls(); // Redoes all connection dependent actions

protected:
	void initModule(); // Initializes module

private:
	QSpacerItem *_pSpacer; // Holds QSpacerItem object
	std::vector<LBCConfig*> _LBCConfigs; // Holds all LBC configurations

private slots:
	void addEntry(); // Adds new LBC configuration
	void removeEntry(LBCConfig *config); // Removes LBC configuration
	void LBC_received(QMap<QString, QString> params); // Called from CommandHandler via ProxyHandler when LBC params are received
	void LBC_set(int state); // Requests CommandHandler via ProxyHandler to enable/disable LBC control module

};

} // namespace VisionLive

#endif // MODULELBC_H
