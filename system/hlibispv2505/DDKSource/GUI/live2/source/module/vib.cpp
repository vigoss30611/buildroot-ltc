#include "vib.hpp"

#include "ispc/ModuleVIB.h"

#define VIB_SAT_POINT_MIN 0.0f
#define VIB_SAT_POINT_MAX 1.0f
#define VIB_CURVE_NAME "Vibrancy"
#define IDENTITY_CURVE_NAME "Identity"

//
// Public Functions
//

VisionLive::ModuleVIB::ModuleVIB(QWidget *parent): ModuleBase(parent)
{
	Ui::VIB::setupUi(this);

	retranslate();

	_pPlot = NULL;

	initModule();

	initObjectsConnections();
}

VisionLive::ModuleVIB::~ModuleVIB()
{
	if(_pPlot)
	{
		_pPlot->deleteLater();
	}
}

void VisionLive::ModuleVIB::removeObjectValueChangedMark()
{
	switch(_css)
	{
	case CUSTOM_CSS:
		enable_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
		point1X_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		point1Y_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		point2X_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		point2Y_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		break;

	case DEFAULT_CSS:
		enable_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
		point1X_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		point1Y_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		point2X_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		point2Y_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
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

	point1X_sb->setMinimum(0.000f);
	point1Y_sb->setMinimum(0.000f);
	point2X_sb->setMinimum(0.000f);
	point2Y_sb->setMinimum(0.000f);

	//
	// Set maximum
	//
	
	point1X_sb->setMaximum(1.000f);
	point1Y_sb->setMaximum(1.000f);
	point2X_sb->setMaximum(1.000f);
	point2Y_sb->setMaximum(1.000f);

	//
	// Set precision
	//

	DEFINE_PREC(point1X_sb, 255.000f);
	DEFINE_PREC(point1Y_sb, 255.000f);
	DEFINE_PREC(point2X_sb, 255.000f);
	DEFINE_PREC(point2Y_sb, 255.000f);

	//
	// Set value
	//

	const ISPC::Parameter *tmp;

	if((tmp = config.getParameter(ISPC::ModuleVIB::VIB_ON.name)) != NULL)
	{
		enable_cb->setChecked((tmp->get<bool>())? true : false);
	}

	if((tmp = config.getParameter("MIE_VIB_SCPOINT_IN")) != NULL)
	{
		point1X_sb->setValue(tmp->get<double>());
	}
	else
	{
		point1X_sb->setValue(0.1f);
	}

	if((tmp = config.getParameter("MIE_VIB_SCPOINT_OUT")) != NULL)
	{
		point1Y_sb->setValue(tmp->get<double>());
	}
	else
	{
		point1Y_sb->setValue(0.1f);
	}

	if((tmp = config.getParameter("MIE_VIB_SCPOINT_IN2")) != NULL)
	{
		point2X_sb->setValue(tmp->get<double>());
	}
	else
	{
		point2X_sb->setValue(0.9f);
	}

	if((tmp = config.getParameter("MIE_VIB_SCPOINT_OUT2")) != NULL)
	{
		point2Y_sb->setValue(tmp->get<double>());
	}
	else
	{
		point2Y_sb->setValue(0.9f);
	}
}

void VisionLive::ModuleVIB::save(ISPC::ParameterList &config)
{
	QVector<QPointF> satCurve = getGains();
	std::vector<std::string> params;
	for(int i = 0; i < satCurve.size(); i++)
	{
		params.push_back(std::string(QString::number(satCurve[i].y()).toLatin1()));
	}
	ISPC::Parameter saturationCurve(ISPC::ModuleVIB::VIB_SATURATION_CURVE.name, params);
	config.addParameter(saturationCurve, true); // Override existing one
	ISPC::Parameter vibOn(ISPC::ModuleVIB::VIB_ON.name, (enable_cb->isChecked())? "1" : "0");
	config.addParameter(vibOn, true); // Override existing one

	ISPC::Parameter scPointIn("MIE_VIB_SCPOINT_IN", std::string(QString::number(point1X_sb->value()).toLatin1()));
	config.addParameter(scPointIn, true); // Override existing one
	ISPC::Parameter scPointOut("MIE_VIB_SCPOINT_OUT", std::string(QString::number(point1Y_sb->value()).toLatin1()));
	config.addParameter(scPointOut, true); // Override existing one

	ISPC::Parameter scPointIn2("MIE_VIB_SCPOINT_IN2", std::string(QString::number(point2X_sb->value()).toLatin1()));
	config.addParameter(scPointIn2, true); // Override existing one
	ISPC::Parameter scPointOut2("MIE_VIB_SCPOINT_OUT2", std::string(QString::number(point2Y_sb->value()).toLatin1()));
	config.addParameter(scPointOut2, true); // Override existing one
}

//
// Protected Functions
//

void VisionLive::ModuleVIB::initModule()
{
	_name = windowTitle();

	if(_pPlot)
	{
		return;
	}

	_pPlot = new EasyPlot::Plot();
	curveLayout->addWidget(_pPlot);

	QVector<QPointF> identitybData;
	identitybData.push_back(QPointF(VIB_SAT_POINT_MIN, VIB_SAT_POINT_MIN));
	identitybData.push_back(QPointF(VIB_SAT_POINT_MAX, VIB_SAT_POINT_MAX));
	_pPlot->addCurve(QStringLiteral(IDENTITY_CURVE_NAME), identitybData);
	_pPlot->setCurveZValue(QStringLiteral(IDENTITY_CURVE_NAME), 1);

	_pPlot->setCurvePen(QStringLiteral(IDENTITY_CURVE_NAME), QPen(QBrush(Qt::black), 1, Qt::DashLine));

	QVector<QPointF> vibData;
	vibData.push_back(QPointF(VIB_SAT_POINT_MIN, VIB_SAT_POINT_MIN));
	vibData.push_back(QPointF(0.1, 0.1));
	vibData.push_back(QPointF(0.9, 0.9));
	vibData.push_back(QPointF(VIB_SAT_POINT_MAX, VIB_SAT_POINT_MAX));
	_pPlot->addCurve(QStringLiteral(VIB_CURVE_NAME), vibData);
	_pPlot->setCurveEditable(QStringLiteral(VIB_CURVE_NAME), true);
	_pPlot->setCurveZValue(QStringLiteral(VIB_CURVE_NAME), 2);

	_pPlot->setCurvePen(QStringLiteral(VIB_CURVE_NAME), QPen(QBrush(Qt::green), 3, Qt::SolidLine));
    _pPlot->setCurveType(QStringLiteral(VIB_CURVE_NAME), EasyPlot::Line);
    _pPlot->setMarkerPen(QStringLiteral(VIB_CURVE_NAME), QPen(QBrush(Qt::green), 5));
    _pPlot->setMarkerEditPen(QStringLiteral(VIB_CURVE_NAME), QPen(QBrush(qRgb(255, 0, 0)), 7));
    _pPlot->setMarkerType(QStringLiteral(VIB_CURVE_NAME), EasyPlot::Ellipse);

	_pPlot->setTitleVisible(false);

	_pPlot->setLegendVisible(false);

	_pPlot->setAxisXTitleText(QStringLiteral("Saturation IN"));
    _pPlot->setAxisXTitleFont(QFont("Times New Roman", 20));
    _pPlot->setAxisXTitleColor(Qt::white);

    _pPlot->setAxisXPen(QPen(QBrush(Qt::white), 1));

    _pPlot->setAxisYTitleText(QStringLiteral("Saturation OUT"));
    _pPlot->setAxisYTitleFont(QFont("Times New Roman", 20));
    _pPlot->setAxisYTitleColor(Qt::white);

    _pPlot->setAxisYPen(QPen(QBrush(Qt::white), 1));

    _pPlot->setLegendPen(QPen(QBrush(Qt::white), 1));

	_pPlot->setPositionInfoColor(Qt::white);

	connect(point1X_sb, SIGNAL(valueChanged(double)), this, SLOT(pointChanged()));
	connect(point1Y_sb, SIGNAL(valueChanged(double)), this, SLOT(pointChanged()));
	connect(point2X_sb, SIGNAL(valueChanged(double)), this, SLOT(pointChanged()));
	connect(point2Y_sb, SIGNAL(valueChanged(double)), this, SLOT(pointChanged()));
	connect(_pPlot, SIGNAL(pointChanged(QString, unsigned, QPointF, QPointF)), this, SLOT(updateSpins(QString, unsigned, QPointF, QPointF)));
	connect(toIdentity_btn, SIGNAL(clicked()), this, SLOT(curveToIdentity()));
}

//
// Private Functions
//

QVector<QPointF> VisionLive::ModuleVIB::getGains()
{
	if(!_pPlot)
	{
		return QVector<QPointF>();
	}

	// Remove all below when vibC will be brought back
	QVector<QPointF> data = _pPlot->data(QStringLiteral(VIB_CURVE_NAME));

	// Calculate vibrancy curve points
	double points[32];
	ISPC::ModuleVIB::saturationCurve(data.at(1).x(), data.at(1).y(), data.at(2).x(), data.at(2).y(), points);

	QVector<QPointF> ret;
	for(int i = 0; i < MIE_SATMULT_NUMPOINTS; i++)
	{
		ret.push_back(QPointF(1/double(MIE_SATMULT_NUMPOINTS - 1), points[i]));
	}

	return ret;
}

//
// Public Slots
//

void VisionLive::ModuleVIB::pointChanged()
{
	if(!_pPlot || point1X_sb->value() > 1.0 || point1X_sb->value() < 0.0 || point1Y_sb->value() > 1.0 || point1Y_sb->value() < 0.0 || 
		point2X_sb->value() > 1.0 || point2X_sb->value() < 0.0 || point2Y_sb->value() > 1.0 || point2Y_sb->value() < 0.0)
	{
		return;
	}

	QVector<QPointF> data = _pPlot->data(QStringLiteral(VIB_CURVE_NAME));
	if(point2X_sb->value() > point1X_sb->value())
	{
		data[1] = QPointF(point1X_sb->value(), point1Y_sb->value());
		data[2] = QPointF(point2X_sb->value(), point2Y_sb->value());
		_pPlot->updateCurve(QStringLiteral(VIB_CURVE_NAME), data);
	}
	else
	{
		point1X_sb->setValue(data[1].x());
		point1Y_sb->setValue(data[1].y());
		point2X_sb->setValue(data[2].x());
		point2Y_sb->setValue(data[2].y());
	}
}

void VisionLive::ModuleVIB::updateSpins(QString name, unsigned index, QPointF prevPos, QPointF currPos)
{
	if(!_pPlot || index == 0 || index == 3 || currPos.x() > 1.0 || currPos.x() < 0.0 || currPos.y() > 1.0 || currPos.y() < 0.0)
	{
		return;
	}

	QVector<QPointF> data = _pPlot->data(name);

	if(data.size() != 4)
	{
		emit logError(QStringLiteral("Vibrancy data size wrong"), QStringLiteral("ModuleVIB::updateSpins()"));
		return;
	}

	data[index] = currPos;

	if(data[2].x() > data[1].x())
	{
		_pPlot->updateCurve(name, data);
	}
	else
	{
		data[index] = prevPos;
		_pPlot->updateCurve(name, data);
	}

	point1X_sb->setValue(data[1].x());
	point1Y_sb->setValue(data[1].y());
	point2X_sb->setValue(data[2].x());
	point2Y_sb->setValue(data[2].y());
}

void VisionLive::ModuleVIB::curveToIdentity()
{
	if(!_pPlot)
	{
		return;
	}

	QVector<QPointF> data;
	data.push_back(QPointF(VIB_SAT_POINT_MIN, VIB_SAT_POINT_MIN));
	data.push_back(QPointF(0.1f, 0.1f));
	data.push_back(QPointF(0.9f, 0.9f));
	data.push_back(QPointF(VIB_SAT_POINT_MAX, VIB_SAT_POINT_MAX));
	_pPlot->updateCurve(QStringLiteral(VIB_CURVE_NAME), data);

	point1X_sb->setValue(data[1].x());
	point1Y_sb->setValue(data[1].y());
	point2X_sb->setValue(data[2].x());
	point2Y_sb->setValue(data[2].y());
}
