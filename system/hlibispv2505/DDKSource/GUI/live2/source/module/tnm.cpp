#include "tnm.hpp"
#include "proxyhandler.hpp"

#include "ispc/ModuleTNM.h"
#include "ispc/ControlTNM.h"

#define TNM_ENABLE "TNM_ENABLE"

//
// Public Functions
//

VisionLive::ModuleTNM::ModuleTNM(QWidget *parent): ModuleBase(parent)
{
	Ui::TNM::setupUi(this);

	retranslate();

	_pCurve = NULL;

	initModule();

	initObjectsConnections();
}

VisionLive::ModuleTNM::~ModuleTNM()
{
	if(_pCurve)
	{
		_pCurve->deleteLater();
	}
}

void VisionLive::ModuleTNM::removeObjectValueChangedMark()
{
	switch(_css)
	{
	case CUSTOM_CSS:
		updateSpeed_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		minHist_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		maxHist_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		tempering_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		smoothing_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		localStrength_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		adaptive_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
		local_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);

		inMinY_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		inMaxY_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
        outMinY_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
        outMaxY_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		colourConfidence_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		colourSaturation_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		lineWeight_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		localWeight_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		flattening_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		minFlattening_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		bypass_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
		break;

	case DEFAULT_CSS:
		updateSpeed_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		minHist_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		maxHist_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		tempering_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		smoothing_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		localStrength_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		adaptive_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
		local_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);

		inMinY_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		inMaxY_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
        outMinY_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
        outMaxY_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		colourConfidence_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		colourSaturation_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		lineWeight_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		localWeight_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		flattening_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		minFlattening_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		bypass_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
		break;
	}
}

void VisionLive::ModuleTNM::retranslate()
{
	Ui::TNM::retranslateUi(this);
}

void VisionLive::ModuleTNM::load(const ISPC::ParameterList &config)
{
	ISPC::ModuleTNM tnm;
	tnm.load(config);
	ISPC::ControlTNM ctnm;
	ctnm.load(config);

	//
	// Set minimum
	//

	// Manual TNM
	inMinY_sb->setMinimum(ISPC::ModuleTNM::TNM_IN_Y.min);
	inMaxY_sb->setMinimum(ISPC::ModuleTNM::TNM_IN_Y.min);
    outMinY_sb->setMinimum(ISPC::ModuleTNM::TNM_OUT_Y.min);
    outMaxY_sb->setMinimum(ISPC::ModuleTNM::TNM_OUT_Y.min);
	colourConfidence_sb->setMinimum(ISPC::ModuleTNM::TNM_COLOUR_CONFIDENCE.min);
	colourSaturation_sb->setMinimum(ISPC::ModuleTNM::TNM_COLOUR_SATURATION.min);
	lineWeight_sb->setMinimum(ISPC::ModuleTNM::TNM_WEIGHT_LINE.min);
	localWeight_sb->setMinimum(ISPC::ModuleTNM::TNM_WEIGHT_LOCAL.min);
	flattening_sb->setMinimum(ISPC::ModuleTNM::TNM_FLAT_FACTOR.min);
	minFlattening_sb->setMinimum(ISPC::ModuleTNM::TNM_FLAT_MIN.min);

	current_inMinY_sb->setMinimum(ISPC::ModuleTNM::TNM_IN_Y.min);
	current_inMaxY_sb->setMinimum(ISPC::ModuleTNM::TNM_IN_Y.min);
    current_outMinY_sb->setMinimum(ISPC::ModuleTNM::TNM_OUT_Y.min);
    current_outMaxY_sb->setMinimum(ISPC::ModuleTNM::TNM_OUT_Y.min);
	current_colourConfidence_sb->setMinimum(ISPC::ModuleTNM::TNM_COLOUR_CONFIDENCE.min);
	current_colourSaturation_sb->setMinimum(ISPC::ModuleTNM::TNM_COLOUR_SATURATION.min);
	current_lineWeight_sb->setMinimum(ISPC::ModuleTNM::TNM_WEIGHT_LINE.min);
	current_localWeight_sb->setMinimum(ISPC::ModuleTNM::TNM_WEIGHT_LOCAL.min);
	current_flattening_sb->setMinimum(ISPC::ModuleTNM::TNM_FLAT_FACTOR.min);
	current_minFlattening_sb->setMinimum(ISPC::ModuleTNM::TNM_FLAT_MIN.min);

	// Auto TNM
	updateSpeed_sb->setMinimum(ISPC::ControlTNM::TNMC_UPDATESPEED.min);
	minHist_sb->setMinimum(ISPC::ControlTNM::TNMC_HISTMIN.min);
	maxHist_sb->setMinimum(ISPC::ControlTNM::TNMC_HISTMAX.min);
	tempering_sb->setMinimum(ISPC::ControlTNM::TNMC_TEMPERING.min);
	smoothing_sb->setMinimum(ISPC::ControlTNM::TNMC_SMOOTHING.min);
	localStrength_sb->setMinimum(ISPC::ControlTNM::TNMC_LOCAL_STRENGTH.min);

	//
	// Set maximum
	//

	// Manual TNM
	inMinY_sb->setMaximum(ISPC::ModuleTNM::TNM_IN_Y.max);
	inMaxY_sb->setMaximum(ISPC::ModuleTNM::TNM_IN_Y.max);
    outMinY_sb->setMaximum(ISPC::ModuleTNM::TNM_OUT_Y.max);
    outMaxY_sb->setMaximum(ISPC::ModuleTNM::TNM_OUT_Y.max);
	colourConfidence_sb->setMaximum(ISPC::ModuleTNM::TNM_COLOUR_CONFIDENCE.max);
	colourSaturation_sb->setMaximum(ISPC::ModuleTNM::TNM_COLOUR_SATURATION.max);
	lineWeight_sb->setMaximum(ISPC::ModuleTNM::TNM_WEIGHT_LINE.max);
	localWeight_sb->setMaximum(ISPC::ModuleTNM::TNM_WEIGHT_LOCAL.max);
	flattening_sb->setMaximum(ISPC::ModuleTNM::TNM_FLAT_FACTOR.max);
	minFlattening_sb->setMaximum(ISPC::ModuleTNM::TNM_FLAT_MIN.max);

	current_inMinY_sb->setMaximum(ISPC::ModuleTNM::TNM_IN_Y.max);
	current_inMaxY_sb->setMaximum(ISPC::ModuleTNM::TNM_IN_Y.max);
    current_outMinY_sb->setMaximum(ISPC::ModuleTNM::TNM_OUT_Y.max);
    current_outMaxY_sb->setMaximum(ISPC::ModuleTNM::TNM_OUT_Y.max);
	current_colourConfidence_sb->setMaximum(ISPC::ModuleTNM::TNM_COLOUR_CONFIDENCE.max);
	current_colourSaturation_sb->setMaximum(ISPC::ModuleTNM::TNM_COLOUR_SATURATION.max);
	current_lineWeight_sb->setMaximum(ISPC::ModuleTNM::TNM_WEIGHT_LINE.max);
	current_localWeight_sb->setMaximum(ISPC::ModuleTNM::TNM_WEIGHT_LOCAL.max);
	current_flattening_sb->setMaximum(ISPC::ModuleTNM::TNM_FLAT_FACTOR.max);
	current_minFlattening_sb->setMaximum(ISPC::ModuleTNM::TNM_FLAT_MIN.max);

	// Auto TNM
	updateSpeed_sb->setMaximum(ISPC::ControlTNM::TNMC_UPDATESPEED.max);
	minHist_sb->setMaximum(ISPC::ControlTNM::TNMC_HISTMIN.max);
	maxHist_sb->setMaximum(ISPC::ControlTNM::TNMC_HISTMAX.max);
	tempering_sb->setMaximum(ISPC::ControlTNM::TNMC_TEMPERING.max);
	smoothing_sb->setMaximum(ISPC::ControlTNM::TNMC_SMOOTHING.max);
	localStrength_sb->setMaximum(ISPC::ControlTNM::TNMC_LOCAL_STRENGTH.max);
	
	//
	// Set precision
	//

	DEFINE_PREC(inMinY_sb, 1.0f);
	DEFINE_PREC(inMaxY_sb, 1.0f);
    DEFINE_PREC(outMinY_sb, 1.0f);
    DEFINE_PREC(outMaxY_sb, 1.0f);
	DEFINE_PREC(colourConfidence_sb, 1024.0000f);
	DEFINE_PREC(colourSaturation_sb, 1024.0000f);
	DEFINE_PREC(lineWeight_sb, 1024.0000f);
	DEFINE_PREC(localWeight_sb, 1024.0000f);
	DEFINE_PREC(flattening_sb, 1024.0000f);
	DEFINE_PREC(minFlattening_sb, 1024.0000f);

	DEFINE_PREC(current_inMinY_sb, 1.0f);
	DEFINE_PREC(current_inMaxY_sb, 1.0f);
    DEFINE_PREC(current_outMinY_sb, 1.0f);
    DEFINE_PREC(current_outMaxY_sb, 1.0f);
	DEFINE_PREC(current_colourConfidence_sb, 1024.0000f);
	DEFINE_PREC(current_colourSaturation_sb, 1024.0000f);
	DEFINE_PREC(current_lineWeight_sb, 1024.0000f);
	DEFINE_PREC(current_localWeight_sb, 1024.0000f);
	DEFINE_PREC(current_flattening_sb, 1024.0000f);
	DEFINE_PREC(current_minFlattening_sb, 1024.0000f);

	//
	// Set value
	//

	// Manual TNM
	inMinY_sb->setValue(tnm.aInY[0]);
	inMaxY_sb->setValue(tnm.aInY[1]);
    outMinY_sb->setValue(tnm.aOutY[0]);
    outMaxY_sb->setValue(tnm.aOutY[1]);
	bypass_cb->setChecked(tnm.bBypass);
	colourConfidence_sb->setValue(tnm.fColourConfidence);
	colourSaturation_sb->setValue(tnm.fColourSaturation);
	lineWeight_sb->setValue(tnm.fWeightLine);
	localWeight_sb->setValue(tnm.fWeightLocal);
	flattening_sb->setValue(tnm.fFlatFactor);
	minFlattening_sb->setValue(tnm.fFlatMin);

	// Curve
	QVector<QPointF> data;
	data.push_back(QPointF(0.0f, 0.0f));
	for (int i = 0 ; i < TNM_CURVE_NPOINTS; i++)
	{
		data.push_back(QPointF(i, tnm.aCurve[i]));
	}
	data.push_back(QPointF(TNM_CURVE_NPOINTS, 1.0f));
	_pCurve->setData(data);
	//_pCurve->replotCurve();

	// Auto TNM
	if(config.exists(TNM_ENABLE))
	{
		auto_cb->setChecked(config.getParameter(TNM_ENABLE)->get<bool>());
	}
	adaptive_cb->setChecked(ctnm.getAdaptiveTNM());
	local_cb->setChecked(ctnm.getLocalTNM());
	updateSpeed_sb->setValue(ctnm.getUpdateSpeed());
	minHist_sb->setValue(ctnm.getHistMin());
	maxHist_sb->setValue(ctnm.getHistMax());
	tempering_sb->setValue(ctnm.getTempering());
	smoothing_sb->setValue(ctnm.getSmoothing());
	localStrength_sb->setValue(ctnm.getLocalStrength());
}

void VisionLive::ModuleTNM::save(ISPC::ParameterList &config)
{
	// Manual TNM
	ISPC::ModuleTNM tnm;
	tnm.load(config);

	if(!bypass_cb->isChecked())
	{
		tnm.aInY[0] = inMinY_sb->value();
		tnm.aInY[1] = inMaxY_sb->value();
        tnm.aOutY[0] = outMinY_sb->value();
        tnm.aOutY[1] = outMaxY_sb->value();
		tnm.bBypass = ISPC::ModuleTNM::TNM_BYPASS.def;
		tnm.fColourConfidence = colourConfidence_sb->value();
		tnm.fColourSaturation = colourSaturation_sb->value();
		tnm.fWeightLine = lineWeight_sb->value();
		tnm.fWeightLocal = localWeight_sb->value();
		tnm.fFlatFactor = flattening_sb->value();
		tnm.fFlatMin = minFlattening_sb->value();
	}
	else
	{
		tnm.aInY[0] = ISPC::ModuleTNM::TNM_IN_Y.def[0];
		tnm.aInY[1] = ISPC::ModuleTNM::TNM_IN_Y.def[1];
        tnm.aOutY[0] = ISPC::ModuleTNM::TNM_OUT_Y.def[0];
        tnm.aOutY[1] = ISPC::ModuleTNM::TNM_OUT_Y.def[1];
		tnm.bBypass = bypass_cb->isChecked();
		tnm.fColourConfidence = ISPC::ModuleTNM::TNM_COLOUR_CONFIDENCE.def;
		tnm.fColourSaturation = ISPC::ModuleTNM::TNM_COLOUR_SATURATION.def;
		tnm.fWeightLine = ISPC::ModuleTNM::TNM_WEIGHT_LINE.def;
		tnm.fWeightLocal = ISPC::ModuleTNM::TNM_WEIGHT_LOCAL.def;
		tnm.fFlatFactor = ISPC::ModuleTNM::TNM_FLAT_FACTOR.def;
		tnm.fFlatMin = ISPC::ModuleTNM::TNM_FLAT_MIN.def;
	}

	// Curve
	const QVector<QPointF> data = _pCurve->data();
    if(data.size() == TNM_IN_MAX_C + 1)
    {
		for(int i = 1; i < TNM_IN_MAX_C; i++)
        {
			tnm.aCurve[i-1] = data.at(i).y();
        }
    }
	else
	{
		emit logError("TNM curve has wrong size - " + QString::number(data.size()) + " should be " + QString::number(TNM_IN_MAX_C + 1), "ModuleTNM::save()");
	}

	tnm.save(config, ISPC::ModuleTNM::SAVE_VAL);

	// Auto TNM
	ISPC::ControlTNM ctnm;
	ctnm.load(config);

	ISPC::Parameter tnm_enable(TNM_ENABLE, auto_cb->isChecked()? "1" : "0");
	config.addParameter(tnm_enable, true);
	ctnm.enableAdaptiveTNM(adaptive_cb->isChecked());
	ctnm.enableLocalTNM(local_cb->isChecked());
	ctnm.setHistMin(minHist_sb->value());
	ctnm.setHistMax(maxHist_sb->value());
	ctnm.setTempering(tempering_sb->value());
	ctnm.setSmoothing(smoothing_sb->value());
	ctnm.setLocalStrength(localStrength_sb->value());
	ctnm.setUpdateSpeed(updateSpeed_sb->value());

	ctnm.save(config, ISPC::ControlModule::SAVE_VAL);
}

void VisionLive::ModuleTNM::setProxyHandler(ProxyHandler *proxy) 
{ 
	_pProxyHandler = proxy; 

	QObject::connect(_pProxyHandler, SIGNAL(TNM_received(QMap<QString, QString>, QVector<QPointF>)), this, SLOT(TNM_received(QMap<QString, QString>, QVector<QPointF>)));
} 

void VisionLive::ModuleTNM::resetConnectionDependantControls()
{
	if(_pProxyHandler)
	{
		_pProxyHandler->TNM_set(auto_cb->isChecked());
	}
}

//
// Protected Functions
//

void VisionLive::ModuleTNM::initModule()
{
	_name = windowTitle();

	_pCurve = new TNMCurve();
    _pCurve->curveControl_grp->setChecked(false);
    _pCurve->curveControl_grp->setVisible(true);
    curveLayout->addWidget(_pCurve);

	setEnables(auto_cb->isChecked());

	QObject::connect(auto_cb, SIGNAL(stateChanged(int)), this, SLOT(TNM_enable(int)));
}

//
// Private Functions
//

void VisionLive::ModuleTNM::setEnables(bool autoEnabled)
{
	if(auto_cb->isChecked())
	{
		// Manual TNM
		inMinY_sb->setEnabled(false);
		inMaxY_sb->setEnabled(false);
        outMinY_sb->setEnabled(false);
        outMaxY_sb->setEnabled(false);
		bypass_cb->setEnabled(false);
		colourConfidence_sb->setEnabled(false);
		lineWeight_sb->setEnabled(false);
		flattening_sb->setEnabled(false);
		colourSaturation_sb->setEnabled(false);
		localWeight_sb->setEnabled(false);
		minFlattening_sb->setEnabled(false);

		// Current TNM
		current_inMinY_sb->setEnabled(true);
		current_inMaxY_sb->setEnabled(true);
        current_outMinY_sb->setEnabled(true);
        current_outMaxY_sb->setEnabled(true);
		current_colourConfidence_sb->setEnabled(true);
		current_colourSaturation_sb->setEnabled(true);
		current_lineWeight_sb->setEnabled(true);
		current_localWeight_sb->setEnabled(true);
		current_flattening_sb->setEnabled(true);
		current_minFlattening_sb->setEnabled(true);

		// Auto TNM
		adaptive_cb->setEnabled(true);
		local_cb->setEnabled(true);
		updateSpeed_sb->setEnabled(true);
		minHist_sb->setEnabled(true);
		maxHist_sb->setEnabled(true);
		tempering_sb->setEnabled(true);
		smoothing_sb->setEnabled(true);
		localStrength_sb->setEnabled(true);
	}
	else
	{
		// Manual TNM
		inMinY_sb->setEnabled(true);
		inMaxY_sb->setEnabled(true);
        outMinY_sb->setEnabled(true);
        outMaxY_sb->setEnabled(true);
		bypass_cb->setEnabled(true);
		colourConfidence_sb->setEnabled(true);
		lineWeight_sb->setEnabled(true);
		flattening_sb->setEnabled(true);
		colourSaturation_sb->setEnabled(true);
		localWeight_sb->setEnabled(true);
		minFlattening_sb->setEnabled(true);

		// Current TNM
		current_inMinY_sb->setEnabled(false);
		current_inMaxY_sb->setEnabled(false);
        current_outMinY_sb->setEnabled(false);
        current_outMaxY_sb->setEnabled(false);
		current_colourConfidence_sb->setEnabled(false);
		current_colourSaturation_sb->setEnabled(false);
		current_lineWeight_sb->setEnabled(false);
		current_localWeight_sb->setEnabled(false);
		current_flattening_sb->setEnabled(false);
		current_minFlattening_sb->setEnabled(false);

		// Auto TNM
		adaptive_cb->setEnabled(false);
		local_cb->setEnabled(false);
		updateSpeed_sb->setEnabled(false);
		minHist_sb->setEnabled(false);
		maxHist_sb->setEnabled(false);
		tempering_sb->setEnabled(false);
		smoothing_sb->setEnabled(false);
		localStrength_sb->setEnabled(false);
	}
}

//
// Protected Slots
//

void VisionLive::ModuleTNM::TNM_enable(int state)
{
	if(_pProxyHandler)
	{
		_pProxyHandler->TNM_set(state);
	}

	setEnables(auto_cb->isChecked());

	if(_pCurve)
	{
		_pCurve->curveControl_grp->setChecked(!auto_cb->isChecked());
		_pCurve->curveControl_grp->setEnabled(!auto_cb->isChecked());
	}
}

void VisionLive::ModuleTNM::TNM_received(QMap<QString, QString> params, QVector<QPointF> curve)
{
	if(auto_cb->isChecked())
	{
		_pCurve->setData(curve);

		current_inMinY_sb->setValue(params.find("LumaInMin").value().toDouble());
		current_inMaxY_sb->setValue(params.find("LumaInMax").value().toDouble());
        current_outMinY_sb->setValue(params.find("LumaOutMin").value().toDouble());
        current_outMaxY_sb->setValue(params.find("LumaOutMax").value().toDouble());
		//current_bypass_cb->setChecked(params.find("Bypass").value().toInt());
		current_colourConfidence_sb->setValue(params.find("ColourConfidance").value().toDouble());
		current_colourSaturation_sb->setValue(params.find("ColourSaturation").value().toDouble());
		current_lineWeight_sb->setValue(params.find("WeightLine").value().toDouble());
		current_localWeight_sb->setValue(params.find("WeightLocal").value().toDouble());
		current_flattening_sb->setValue(params.find("FlatFactor").value().toDouble());
		current_minFlattening_sb->setValue(params.find("FlatMin").value().toDouble());
	}
}
