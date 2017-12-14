#ifndef MODULEVIB_H
#define MODULEVIB_H

#include "modulebase.hpp"
#include "ui_vib.h"
#include "vibcurve.hpp"

#define VIB_SAT_POINT_MIN 0.0f
#define VIB_SAT_POINT_MAX 1.0f

namespace VisionLive
{

class ModuleVIB : public ModuleBase, public Ui::VIB
{
	Q_OBJECT

public:
    ModuleVIB(QWidget *parent = 0); // Class constructor
    ~ModuleVIB(); // Class destructor
	void removeObjectValueChangedMark();  // Removes object marking indicating value change
	void retranslate(); // Retranslates GUI
	void load(const ISPC::ParameterList &config); // Loads module configuration from config
	void save(ISPC::ParameterList &config) const; // Saves module configuration to config

protected:
	void initModule(); // Initializes module

private:
	VIBCurve *_pCurve; // Holds VIBCurve widget

signals:
	void sendPoints(double p1x, double p1y, double p2x, double p2y);
    
public slots:
	void pointChanged();
	void updateSpins(double, double, double, double);

};

} // namespace VisionLive

#endif // MODULEVIB_H
