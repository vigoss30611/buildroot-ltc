#ifndef MODULETNM_H
#define MODULETNM_H

#include "modulebase.hpp"
#include "ui_tnm.h"
#include "tnmcurve.hpp"

namespace VisionLive
{

class ProxyHandler;

class ModuleTNM : public ModuleBase, public Ui::TNM
{
	Q_OBJECT

public:
    ModuleTNM(QWidget *parent = 0); // Class constructor
    ~ModuleTNM(); // Class destructor
	void removeObjectValueChangedMark();  // Removes object marking indicating value change
	void retranslate(); // Retranslates GUI
	void load(const ISPC::ParameterList &config); // Loads module configuration from config
	void save(ISPC::ParameterList &config); // Saves module configuration to config
	void setProxyHandler(ProxyHandler *proxy); // Sets ProxyHandler object
	void resetConnectionDependantControls(); // Redoes all connection dependent actions

protected:
	void initModule(); // Initializes module

private:
	TNMCurve *_pCurve; // Holds TNMCurve widget

	void setEnables(bool autoEnabled); // Called to enable/disable parts of module

protected slots:
	void TNM_enable(int state); // Called to set Auto TNM enable
	void TNM_received(QMap<QString, QString> params, QVector<QPointF> curve); // Called when TNM data received from ISP
	
};

} // namespace VisionLive

#endif // MODULETNM_H
