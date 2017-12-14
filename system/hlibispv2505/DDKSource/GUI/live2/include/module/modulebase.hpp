#ifndef MODULEBASE_H
#define MODULEBASE_H

#include <qwidget.h>

#include "css.hpp"
#include "ispc/ParameterList.h"
#include "visiontuning.hpp"
#ifdef WIN32
#define NOMINMAX
#endif
#include <math.h>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

#ifdef USE_MATH_NEON
#define DEFINE_PREC(gui, param) if ( param > 0 ) {\
	int prec = (int)floor(log10f_neon((float)param)) +1; \
	gui->setDecimals(prec); \
	gui->setSingleStep((double)powf_neon(10.0f, -1.0f*(float)prec) ); }
#else
#define DEFINE_PREC(gui, param) if ( param > 0 ) {\
	int prec = (int)floor(log10(param)) +1; \
	gui->setDecimals(prec); \
	gui->setSingleStep( pow(10.0f, -1*prec) ); }
#endif

namespace VisionLive
{

class ObjectRegistry;
class ProxyHandler;

class ModuleBase : public QWidget
{
	Q_OBJECT

public:
    ModuleBase(QWidget *parent = 0); // Class constructor
    virtual ~ModuleBase(); // Class destructor

	virtual void removeObjectValueChangedMark() = 0; // Removes object marking indicating value change
	virtual void retranslate() = 0; // Retranslates GUI
	virtual void load(const ISPC::ParameterList &config) = 0; // Loads module configuration from config
	virtual void save(ISPC::ParameterList &config) = 0; // Saves module configuration to config
	virtual void setCSS(CSS newCSS); // Sets css 
	virtual void setHWVersion(HW_VERSION hwVersion); // Sets css 
	const QString& getName() const; // Returns module name
	void setName(const QString &name); // Sets module name
	void setObjectRegistry(ObjectRegistry *objReg) { _pObjReg = objReg; } // Sets ObjectRegistry object
	virtual void setProxyHandler(ProxyHandler *proxy) { _pProxyHandler = proxy; } // Sets ProxyHandler object
	virtual void resetConnectionDependantControls() {} // Redoes all connection dependent actions

protected:
	ProxyHandler *_pProxyHandler; // Holds ProxyHandler object

	QString _name; // Holds object name
	
	CSS _css; // Holds css type

	HW_VERSION _hwVersion; // Holds HW version

	ObjectRegistry *_pObjReg; // Holds ObjectRegistry object

	virtual void initModule() = 0; // Initializes module

	void initObjectsConnections(QObject *rootObject = NULL); // Sets up connections of all objects in module

private:
	void markObjectValueChanged(QObject *object); // Marks object object indicating value changed
	virtual void HWVersionChanged() {} // Changes hw version dependant stuff of module that overwrite this function

signals:
	void logError(const QString &error, const QString &src); // Rquests Log to log error
	void logWarning(const QString &warning, const QString &src); // Rquests Log to log warning
	void logMessage(const QString &message, const QString &src); // Rquests Log to log message
	void logAction(const QString &action); // Rquests Log to log action
    void riseError(const QString &error); // Popups error MessageBox

private slots:
	void spinBoxValueChanged(); // Called when module SpinBox object vale has been changed
	void doubleSpinBoxValueChanged(); // Called when module DoubleSpinBox object vale has been changed
	void comboBoxValueChanged(); // Called when module ComboBox object vale has been changed
	void checkBoxValueChanged(); // Called when module CheckBox object vale has been changed

};

} // namespace VisionLive

#endif // MODULEBASE_H
