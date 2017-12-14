#include "vib.hpp"

#include "ispc/ModuleVIB.h"

//
// Public Functions
//

VisionLive::ModuleVIB::ModuleVIB(QWidget *parent): ModuleBase(parent)
{
	Ui::VIB::setupUi(this);

	retranslate();

	_pCurve = NULL;

	initModule();

	initObjectsConnections();
}

VisionLive::ModuleVIB::~ModuleVIB()
{
	if(_pCurve)
	{
		_pCurve->deleteLater();
	}
}

void VisionLive::ModuleVIB::removeObjectValueChangedMark()
{
	switch(css)
	{
	case CUSTOM_CSS:
		VIB_enable_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
		VIB_point1X_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		VIB_point1Y_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		VIB_point2X_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		VIB_point2Y_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		break;

	case DEFAULT_CSS:
		VIB_enable_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
		VIB_point1X_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		VIB_point1Y_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		VIB_point2X_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		VIB_point2Y_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		break;
	}
}

void VisionLive::ModuleVIB::retranslate()
{
	Ui::VIB::retranslateUi(this);
}

void VisionLive::ModuleVIB::load(const ISPC::ParameterList &config)
{
	//
	// Set minimum
	//

	VIB_point1X_sb->setMinimum(0.000f);
	VIB_point1Y_sb->setMinimum(0.000f);
	VIB_point2X_sb->setMinimum(0.000f);
	VIB_point2Y_sb->setMinimum(0.000f);

	//
	// Set maximum
	//
	
	VIB_point1X_sb->setMaximum(1.000f);
	VIB_point1Y_sb->setMaximum(1.000f);
	VIB_point2X_sb->setMaximum(1.000f);
	VIB_point2Y_sb->setMaximum(1.000f);

	//
	// Set precision
	//

	DEFINE_PREC(VIB_point1X_sb, 255.000f);
	DEFINE_PREC(VIB_point1Y_sb, 255.000f);
	DEFINE_PREC(VIB_point2X_sb, 255.000f);
	DEFINE_PREC(VIB_point2Y_sb, 255.000f);

	//
	// Set value
	//

	const ISPC::Parameter *tmp;

	if((tmp = config.getParameter(ISPC::ModuleVIB::VIB_ON.name)) != NULL)
		{
			VIB_enable_cb->setChecked((tmp->get<bool>())? true : false);
		}

		if((tmp = config.getParameter("MIE_VIB_SCPOINT_IN")) != NULL)
		{
			VIB_point1X_sb->setValue(tmp->get<double>());
		}
		else
		{
			VIB_point1X_sb->setValue(0.01f);
		}

		if((tmp = config.getParameter("MIE_VIB_SCPOINT_OUT")) != NULL)
		{
			VIB_point1Y_sb->setValue(tmp->get<double>());
		}
		else
		{
			VIB_point1Y_sb->setValue(0.01f);
		}

		if((tmp = config.getParameter("MIE_VIB_SCPOINT_IN2")) != NULL)
		{
			VIB_point2X_sb->setValue(tmp->get<double>());
		}
		else
		{
			VIB_point2X_sb->setValue(0.99f);
		}

		if((tmp = config.getParameter("MIE_VIB_SCPOINT_OUT2")) != NULL)
		{
			VIB_point2Y_sb->setValue(tmp->get<double>());
		}
		else
		{
			VIB_point2Y_sb->setValue(0.99f);
		}

		emit sendPoints(VIB_point1X_sb->value(), VIB_point1Y_sb->value(), VIB_point2X_sb->value(), VIB_point2Y_sb->value());

}

void VisionLive::ModuleVIB::save(ISPC::ParameterList &config) const
{
	QVector<QPointF> satCurve = _pCurve->getGains();
	std::vector<std::string> params;

	for(int i = 0; i < satCurve.size(); i++)
	{
		params.push_back(std::string(QString::number(satCurve[i].y()).toLatin1()));
	}
	ISPC::Parameter saturationCurve(ISPC::ModuleVIB::VIB_SATURATION_CURVE.name, params);
	config.addParameter(saturationCurve, true); // Override existing one
	ISPC::Parameter vibOn(ISPC::ModuleVIB::VIB_ON.name, (VIB_enable_cb->isChecked())? "1" : "0");
	config.addParameter(vibOn, true); // Override existing one

	ISPC::Parameter scPointIn("MIE_VIB_SCPOINT_IN", std::string(QString::number(VIB_point1X_sb->value()).toLatin1()));
	config.addParameter(scPointIn, true); // Override existing one
	ISPC::Parameter scPointOut("MIE_VIB_SCPOINT_OUT", std::string(QString::number(VIB_point1Y_sb->value()).toLatin1()));
	config.addParameter(scPointOut, true); // Override existing one

	ISPC::Parameter scPointIn2("MIE_VIB_SCPOINT_IN2", std::string(QString::number(VIB_point2X_sb->value()).toLatin1()));
	config.addParameter(scPointIn2, true); // Override existing one
	ISPC::Parameter scPointOut2("MIE_VIB_SCPOINT_OUT2", std::string(QString::number(VIB_point2Y_sb->value()).toLatin1()));
	config.addParameter(scPointOut2, true); // Override existing one
}

//
// Protected Functions
//

void VisionLive::ModuleVIB::initModule()
{
	_name = windowTitle();

	_pCurve = new VIBCurve();
    curveLayout->addWidget(_pCurve);

	connect(VIB_point1X_sb, SIGNAL(valueChanged(double)), this, SLOT(pointChanged()));
	connect(VIB_point1Y_sb, SIGNAL(valueChanged(double)), this, SLOT(pointChanged()));
	connect(VIB_point2X_sb, SIGNAL(valueChanged(double)), this, SLOT(pointChanged()));
	connect(VIB_point2Y_sb, SIGNAL(valueChanged(double)), this, SLOT(pointChanged()));
	connect(this, SIGNAL(sendPoints(double, double, double, double)), _pCurve, SLOT(receivePoints(double, double, double, double))); 
	connect(_pCurve, SIGNAL(updateSpins(double, double, double, double)), this, SLOT(updateSpins(double, double, double, double)));
}

//
// Public Slots
//

void VisionLive::ModuleVIB::pointChanged()
{
	emit sendPoints(VIB_point1X_sb->value(), VIB_point1Y_sb->value(), VIB_point2X_sb->value(), VIB_point2Y_sb->value());
}

void VisionLive::ModuleVIB::updateSpins(double p1x, double p1y, double p2x, double p2y)
{
	VIB_point1X_sb->setValue(p1x);
	VIB_point1Y_sb->setValue(p1y);
	VIB_point2X_sb->setValue(p2x);
	VIB_point2Y_sb->setValue(p2y);
}
 