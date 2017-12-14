#ifndef MODULEWBC_H
#define MODULEWBC_H

#include "modulebase.hpp"
#include "ui_wbc.h"
#include "ccmpatch.hpp"
#include "genconfig.hpp"

namespace VisionLive
{

class ProxyHandler;

class ModuleWBC : public ModuleBase, public Ui::WBC
{
	Q_OBJECT

public:
    ModuleWBC(QWidget *parent = 0); // Class constructor
    ~ModuleWBC(); // Class destructor
	void removeObjectValueChangedMark();  // Removes object marking indicating value change
	void retranslate(); // Retranslates GUI
	void load(const ISPC::ParameterList &config); // Loads module configuration from config
	void save(ISPC::ParameterList &config) const; // Saves module configuration to config
	void setProxyHandler(ProxyHandler *proxy); // Sets ProxyHandler object
	void resetConnectionDependantControls(); // Redoes all connection dependent actions
	void setCSS(CSS newCSS); // Sets css 

protected:
	void initModule();

private:
	// More convinient storage for actual widgets
	QDoubleSpinBox *AWB_Gains[4];
	QDoubleSpinBox *AWB_Clips[4];
	QDoubleSpinBox *AWB_Matrix[9];
	QDoubleSpinBox *AWB_Offsets[3];

	std::vector<CCMPatch *> _CCMPaches; // Holds all CCM patches

	blc_config _blcConfig; // Holds current ModuleBLC configuration
	GenConfig *_pGenConfig; // GenConfig object - used by _pCCMTunning
	CCMTuning *_pCCMTuning; // CCMTuning object - used for tuning CCM patch

public slots:
	void moduleBLCChanged(int sensorBlack0, int sensorBlack1, int sensorBlack2, int sensorBlack3, int systemBlack); // Updates BLC params in default CCM patches

protected slots:
	void addCCM(const QString &name, bool isDefault = false); // Adds new CCMPatch tab
	void remove(CCMPatch *ob); // Removes CCMPatch tab
	void clearAll(); // Removes all CCMPatch tabs
	void promote(); // Copies default CCMPatch tab to multi CCMPatch tabs
	void setDefault(CCMPatch *ob); // Copies values from CCMPatch tab to default CCMPatch tab
	void setDefault(); // Copies values from AWB tab to default CCMPatch tab

	void AWB_changeMode(int mode); // Requests CommandHandler via ProxyHandler to change AWB control module mode
	void AWB_received(QMap<QString, QString> params); // Called from CommandHandler via ProxyHandler when AWB params received

	void tune(); // Opens patch tunning window
	void integrate(); // Called from CCMTunning after tuning finished to integrate results into CCMPatch

};

} // namespace VisionLive

#endif // MODULEWBC_H
