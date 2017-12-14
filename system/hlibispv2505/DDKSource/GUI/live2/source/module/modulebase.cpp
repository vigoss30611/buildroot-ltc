#include "modulebase.hpp"
#include "css.hpp"
#include "objectregistry.hpp"
#include "proxyhandler.hpp"

#include <qspinbox.h>
#include <qcombobox.h>

//
// Public Functions
//

VisionLive::ModuleBase::ModuleBase(QWidget *parent): QWidget(parent)
{
	_pProxyHandler = NULL;
	_pObjReg = NULL;

	_name = QString();

	_css = DEFAULT_CSS;

	_hwVersion = HW_UNKNOWN;
}

VisionLive::ModuleBase::~ModuleBase()
{
}

void VisionLive::ModuleBase::setCSS(CSS newCSS) 
{ 
	_css = newCSS;
}

void VisionLive::ModuleBase::setHWVersion(HW_VERSION hwVersion)
{
	_hwVersion = hwVersion;

	HWVersionChanged();
}

const QString& VisionLive::ModuleBase::getName() const 
{ 
	return _name; 
}

void VisionLive::ModuleBase::setName(const QString &name) 
{ 
	_name = name; 
}

//
// Protected Functions
//

void VisionLive::ModuleBase::initObjectsConnections(QObject *rootObject)
{
	if(!rootObject)
	{
		rootObject = this;
	}

	QMap<QString, std::pair<QObject *, QString> > childrenObjects = Algorithms::getChildrenObjects(rootObject, QString(), QString());

	for(QMap<QString, std::pair<QObject *, QString> >::iterator it = childrenObjects.begin(); it != childrenObjects.end(); it++)
	{
		Type objType = Algorithms::getObjectType((*it).first);

		if(objType == QSPINBOX)
		{
			QObject::connect((*it).first, SIGNAL(valueChanged(int)), this, SLOT(spinBoxValueChanged()));
		}
		else if(objType == QDOUBLESPINBOX)
		{
			QObject::connect((*it).first, SIGNAL(valueChanged(double)), this, SLOT(doubleSpinBoxValueChanged()));
		}
		else if(objType == QCOMBOBOX)
		{
			QObject::connect((*it).first, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxValueChanged()));
		}
		else if(objType == QCHECKBOX)
		{
			QObject::connect((*it).first, SIGNAL(stateChanged(int)), this, SLOT(checkBoxValueChanged()));
		}
	}
}

//
// Private Functions
//

void VisionLive::ModuleBase::markObjectValueChanged(QObject *object)
{
	((QWidget *)object)->setStyleSheet(CSS_QWIDGET_CHANGED);
}

//
// Private Slots
//

void VisionLive::ModuleBase::spinBoxValueChanged()
{
	// Don't mark and don't log readOnly spinboxes
	if(!((QAbstractSpinBox *)sender())->isReadOnly())
	{
		if(sender()->objectName() != "minConfidence_sb" &&  // DPF
			sender()->objectName() != "maxConfidence_sb") // DPF
		{
			markObjectValueChanged(sender());
		}

		QString action = "SET " + sender()->objectName() + " " + QString::number(((QSpinBox *)sender())->value());
		emit logAction(action);
	}
}

void VisionLive::ModuleBase::doubleSpinBoxValueChanged()
{
	// Don't mark and don't log readOnly spinboxes
	if(!((QAbstractSpinBox *)sender())->isReadOnly())
	{
		markObjectValueChanged(sender());

		QString action = "SET " + sender()->objectName() + " " + QString::number(((QDoubleSpinBox *)sender())->value());
		emit logAction(action);
	}
}

void VisionLive::ModuleBase::comboBoxValueChanged()
{
	if(sender()->objectName() != "AWB_mode_lb" && // WBC
		sender()->objectName() != "s_lb" && // DPF
		sender()->objectName() != "GMA_changeGLUT_lb" && // GMA
		sender()->objectName() != "range_lb") // GMAWidget?
	{
		markObjectValueChanged(sender());
	}

	QString action = "SET " + sender()->objectName() + " " + QString::number(((QComboBox *)sender())->currentIndex());
	emit logAction(action);
}

void VisionLive::ModuleBase::checkBoxValueChanged()
{
	// Don't mark "instant" checkboxes
	if(!((QCheckBox *)sender())->text().endsWith("(instant)") &&
		sender()->objectName() != "interpolateFwd" &&
        sender()->objectName() != "interpolateBwd" &&
        sender()->objectName() != "selectAll_cb") // LSH
	{
		markObjectValueChanged(sender());
	}

	QString action = "SET " + sender()->objectName() + " " + QString::number(((QCheckBox *)sender())->isChecked());
	emit logAction(action);
}
