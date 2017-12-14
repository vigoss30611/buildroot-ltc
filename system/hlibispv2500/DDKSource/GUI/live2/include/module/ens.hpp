#ifndef MODULEENS_H
#define MODULEENS_H

#include "modulebase.hpp"
#include "ui_ens.h"

namespace VisionLive
{

	class ProxyHandler;

class ModuleENS : public ModuleBase, public Ui::ENS
{
	Q_OBJECT

public:
    ModuleENS(QWidget *parent = 0); // Class constructor
    ~ModuleENS(); // Class destructor
	void removeObjectValueChangedMark();  // Removes object marking indicating value change
	void retranslate(); // Retranslates GUI
	void load(const ISPC::ParameterList &config); // Loads module configuration from config
	void save(ISPC::ParameterList &config) const; // Saves module configuration to config
	void setProxyHandler(ProxyHandler *proxy); // Sets ProxyHandler object

protected:
	void initModule();

protected slots:
	void ENS_received(IMG_UINT32 size); // Called when ENS statistics size has been received
};

} // namespace VisionLive

#endif // MODULEENS_H
