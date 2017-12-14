#ifndef MODULEVIB_H
#define MODULEVIB_H

#include "modulebase.hpp"
#include "ui_vib.h"
#include "plot.h"

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
	void save(ISPC::ParameterList &config); // Saves module configuration to config

protected:
	void initModule(); // Initializes module

private:
	EasyPlot::Plot *_pPlot; // Holds Vibrancy Plot widget
	QVector<QPointF> getGains(); // Returns 32 point curve generated based on 2 VIB points
   
public slots:
	void pointChanged(); // Called on spinBoxes change to update plot
	void updateSpins(QString name, unsigned index, QPointF prevPos, QPointF currPos); // Called on plot change to update spinBoxes
	void curveToIdentity(); // Called to turn curve to identity

};

} // namespace VisionLive

#endif // MODULEVIB_H
