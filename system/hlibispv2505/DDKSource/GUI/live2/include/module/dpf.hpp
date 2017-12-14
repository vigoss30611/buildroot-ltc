#ifndef MODULEDPF_H
#define MODULEDPF_H

#include "modulebase.hpp"
#include "ui_dpf.h"

namespace VisionLive
{

class ProxyHandler;

class ModuleDPF : public ModuleBase, public Ui::DPF
{
	Q_OBJECT

public:
    ModuleDPF(QWidget *parent = 0); // Class constructor
    ~ModuleDPF(); // Class destructor
	void removeObjectValueChangedMark();  // Removes object marking indicating value change
	void retranslate(); // Retranslates GUI
	void load(const ISPC::ParameterList &config); // Loads module configuration from config
	void save(ISPC::ParameterList &config); // Saves module configuration to config
	void setProxyHandler(ProxyHandler *proxy); // Sets ProxyHandler object

protected:
	void initModule(); // Initializes module

private:
	QByteArray _inputMapData; // Loaded DPF input map

	// DPF related sensor info
	IMG_UINT32 _paralelism;
	IMG_UINT32 _DPFInternalSize;
	IMG_UINT32 _sensorHeight;
	IMG_UINT32 _readMapSize;

	// _inputMapData statistics
	int _minConf;
	int _maxConf;
	double _avgConf;

	bool _inputMapApplyed;

	void showInputMap(bool show); // Shows and hides input map related controls
	void showProgress(int percent); // Shows and hides prograss bar indicating input map related operations progress
	void inputMapInfo(const IMG_UINT16* pDPFOutput, int nDefects, IMG_UINT height, IMG_UINT paralelism); // Calculates and displays input map info

public slots:
	void DPF_received(QMap<QString, QString> params); // Called when DPF parameters are received by CommanHandler
	void SENSOR_received(QMap<QString, QString> params); // Called when Sensor parameters are received by CommandHandler

protected slots:
	void detectChanged(); // Called when DPF_HWDetect_cb state has changed
	void DPF_loadMap(const QString &filename = QString()); // Loads DPF input map
	void filterMap(); // Filters loaded input map
	void DPF_applyMap(); // Applys DPF input map
	
};

} // namespace VisionLive

#endif // MODULEDPF_H
