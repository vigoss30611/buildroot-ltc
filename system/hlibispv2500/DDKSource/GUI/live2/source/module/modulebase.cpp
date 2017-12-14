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

	css = DEFAULT_CSS;
}

VisionLive::ModuleBase::~ModuleBase()
{
}

void VisionLive::ModuleBase::setCSS(CSS newCSS)
{
	css = newCSS;
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
		if(sender()->objectName() != "DPF_minConfidence_sb" &&
			sender()->objectName() != "DPF_maxConfidence_sb")
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
	if(sender()->objectName() != "AWB_mode_lb" &&
		sender()->objectName() != "DPF_s_lb" &&
		sender()->objectName() != "GMA_changeGLUT_lb" &&
		sender()->objectName() != "range_lb")
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
		sender()->objectName() != "interpolateBwd")
	{
		markObjectValueChanged(sender());
	}

	QString action = "SET " + sender()->objectName() + " " + QString::number(((QCheckBox *)sender())->isChecked());
	emit logAction(action);
}
