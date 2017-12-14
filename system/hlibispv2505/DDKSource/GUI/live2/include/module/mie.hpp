#ifndef MODULEMIE_H
#define MODULEMIE_H

#include "modulebase.hpp"
#include "ui_mie.h"

#include "miewidget.hpp"

namespace VisionLive
{

class ModuleMIE : public ModuleBase, public Ui::MIE
{
	Q_OBJECT

public:
    ModuleMIE(QWidget *parent = 0); // Class constructor
    ~ModuleMIE(); // Class destructor
	void removeObjectValueChangedMark();  // Removes object marking indicating value change
	void retranslate(); // Retranslates GUI
	void load(const ISPC::ParameterList &config); // Loads module configuration from config
	void save(ISPC::ParameterList &config); // Saves module configuration to config
	void updatePixelInfo(QPoint pixel, std::vector<int> pixelInfo); // Called from MainWindow when pixel info received

protected:
	void initModule(); // Initializes module

private:
	bool _firstLoad; // Indicates if its the first time loading params
	std::vector<MIEWidget *> _MIEWidgets; // Holds all MIEWidgets
	void addMC(); // Adds memory colour tab

};

} // namespace VisionLive

#endif // MODULEMIE_H
