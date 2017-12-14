#ifndef MODULEGMA_H
#define MODULEGMA_H

#include "modulebase.hpp"
#include "ui_gma.h"

#include "ci/ci_api_structs.h"

namespace VisionLive
{

class GMAWidget;
class ProxyHandler;

class ModuleGMA : public ModuleBase, public Ui::GMA
{
	Q_OBJECT

public:
    ModuleGMA(QWidget *parent = 0); // Class constructor
    ~ModuleGMA(); // Class destructor
	void removeObjectValueChangedMark();  // Removes object marking indicating value change
	void retranslate(); // Retranslates GUI
	void load(const ISPC::ParameterList &config); // Loads module configuration from config
	void save(ISPC::ParameterList &config); // Saves module configuration to config
	void setProxyHandler(ProxyHandler *proxy); // Sets ProxyHandler object
	void resetConnectionDependantControls(); // Redoes all connection dependent actions

protected:
	void initModule(); // Initializes module

private:
	bool _firstLoad; // Indicates if its the first time loading params
	std::vector<GMAWidget *> _gammaCurves; // Holds all GLUTs

private slots:
	void addGammaCurve(QVector<QPointF> dataRed, QVector<QPointF> dataGreen, QVector<QPointF> dataBlue, bool editable, QString name); // Adds new new GLUT
	void removeGammaCurve(GMAWidget *gma); // Removes specyfic GLUT
	void GMA_get(); // Requests CommandHandler via ProxyHandler to get current GLUT
	void GMA_set(int index); // Requests CommandHandler via ProxyHandler to set GLUT
	void GMA_received(CI_MODULE_GMA_LUT data); // Called from Commandhandler via ProxyHandler when GLUT has been received
	void generatePseudoCode(const QString &filename = QString()); // Exports txt file containig pseudo code of all GLUTs

};

} // namespace VisionLive

#endif // MODULEGMA_H
