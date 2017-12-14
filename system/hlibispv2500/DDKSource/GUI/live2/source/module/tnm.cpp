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
	switch(css)
	{
	case CUSTOM_CSS:
		ATNM_updateSpeed_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		ATNM_minHist_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		ATNM_maxHist_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		ATNM_tempering_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		ATNM_smoothing_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		ATNM_localStrength_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		ATNM_adaptive_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
		ATNM_local_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);

		TNM_minY_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		TNM_maxY_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		TNM_colourConfidence_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		TNM_colourSaturation_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		TNM_lineWeight_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		TNM_localWeight_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		TNM_flattening_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		TNM_minFlattening_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		TNM_bypass_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
		break;

	case DEFAULT_CSS:
		ATNM_updateSpeed_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		ATNM_minHist_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		ATNM_maxHist_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		ATNM_tempering_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		ATNM_smoothing_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		ATNM_localStrength_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		ATNM_adaptive_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
		ATNM_local_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);

		TNM_minY_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		TNM_maxY_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		TNM_colourConfidence_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		TNM_colourSaturation_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		TNM_lineWeight_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		TNM_localWeight_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		TNM_flattening_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		TNM_minFlattening_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		TNM_bypass_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
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
	TNM_minY_sb->setMinimum(ISPC::ModuleTNM::TNM_IN_Y.min);
	TNM_maxY_sb->setMinimum(ISPC::ModuleTNM::TNM_IN_Y.min);
	TNM_colourConfidence_sb->setMinimum(ISPC::ModuleTNM::TNM_COLOUR_CONFIDENCE.min);
	TNM_colourSaturation_sb->setMinimum(ISPC::ModuleTNM::TNM_COLOUR_SATURATION.min);
	TNM_lineWeight_sb->setMinimum(ISPC::ModuleTNM::TNM_WEIGHT_LINE.min);
	TNM_localWeight_sb->setMinimum(ISPC::ModuleTNM::TNM_WEIGHT_LOCAL.min);
	TNM_flattening_sb->setMinimum(ISPC::ModuleTNM::TNM_FLAT_FACTOR.min);
	TNM_minFlattening_sb->setMinimum(ISPC::ModuleTNM::TNM_FLAT_MIN.min);

	TNM_current_minY_sb->setMinimum(ISPC::ModuleTNM::TNM_IN_Y.min);
	TNM_current_maxY_sb->setMinimum(ISPC::ModuleTNM::TNM_IN_Y.min);
	TNM_current_colourConfidence_sb->setMinimum(ISPC::ModuleTNM::TNM_COLOUR_CONFIDENCE.min);
	TNM_current_colourSaturation_sb->setMinimum(ISPC::ModuleTNM::TNM_COLOUR_SATURATION.min);
	TNM_current_lineWeight_sb->setMinimum(ISPC::ModuleTNM::TNM_WEIGHT_LINE.min);
	TNM_current_localWeight_sb->setMinimum(ISPC::ModuleTNM::TNM_WEIGHT_LOCAL.min);
	TNM_current_flattening_sb->setMinimum(ISPC::ModuleTNM::TNM_FLAT_FACTOR.min);
	TNM_current_minFlattening_sb->setMinimum(ISPC::ModuleTNM::TNM_FLAT_MIN.min);

	// Auto TNM
	ATNM_updateSpeed_sb->setMinimum(ISPC::ControlTNM::TNMC_UPDATESPEED.min);
	ATNM_minHist_sb->setMinimum(ISPC::ControlTNM::TNMC_HISTMIN.min);
	ATNM_maxHist_sb->setMinimum(ISPC::ControlTNM::TNMC_HISTMAX.min);
	ATNM_tempering_sb->setMinimum(ISPC::ControlTNM::TNMC_TEMPERING.min);
	ATNM_smoothing_sb->setMinimum(ISPC::ControlTNM::TNMC_SMOOTHING.min);
	ATNM_localStrength_sb->setMinimum(ISPC::ControlTNM::TNMC_LOCAL_STRENGTH.min);

	//
	// Set maximum
	//

	// Manual TNM
	TNM_minY_sb->setMaximum(ISPC::ModuleTNM::TNM_IN_Y.max);
	TNM_maxY_sb->setMaximum(ISPC::ModuleTNM::TNM_IN_Y.max);
	TNM_colourConfidence_sb->setMaximum(ISPC::ModuleTNM::TNM_COLOUR_CONFIDENCE.max);
	TNM_colourSaturation_sb->setMaximum(ISPC::ModuleTNM::TNM_COLOUR_SATURATION.max);
	TNM_lineWeight_sb->setMaximum(ISPC::ModuleTNM::TNM_WEIGHT_LINE.max);
	TNM_localWeight_sb->setMaximum(ISPC::ModuleTNM::TNM_WEIGHT_LOCAL.max);
	TNM_flattening_sb->setMaximum(ISPC::ModuleTNM::TNM_FLAT_FACTOR.max);
	TNM_minFlattening_sb->setMaximum(ISPC::ModuleTNM::TNM_FLAT_MIN.max);

	TNM_current_minY_sb->setMaximum(ISPC::ModuleTNM::TNM_IN_Y.max);
	TNM_current_maxY_sb->setMaximum(ISPC::ModuleTNM::TNM_IN_Y.max);
	TNM_current_colourConfidence_sb->setMaximum(ISPC::ModuleTNM::TNM_COLOUR_CONFIDENCE.max);
	TNM_current_colourSaturation_sb->setMaximum(ISPC::ModuleTNM::TNM_COLOUR_SATURATION.max);
	TNM_current_lineWeight_sb->setMaximum(ISPC::ModuleTNM::TNM_WEIGHT_LINE.max);
	TNM_current_localWeight_sb->setMaximum(ISPC::ModuleTNM::TNM_WEIGHT_LOCAL.max);
	TNM_current_flattening_sb->setMaximum(ISPC::ModuleTNM::TNM_FLAT_FACTOR.max);
	TNM_current_minFlattening_sb->setMaximum(ISPC::ModuleTNM::TNM_FLAT_MIN.max);

	// Auto TNM
	ATNM_updateSpeed_sb->setMaximum(ISPC::ControlTNM::TNMC_UPDATESPEED.max);
	ATNM_minHist_sb->setMaximum(ISPC::ControlTNM::TNMC_HISTMIN.max);
	ATNM_maxHist_sb->setMaximum(ISPC::ControlTNM::TNMC_HISTMAX.max);
	ATNM_tempering_sb->setMaximum(ISPC::ControlTNM::TNMC_TEMPERING.max);
	ATNM_smoothing_sb->setMaximum(ISPC::ControlTNM::TNMC_SMOOTHING.max);
	ATNM_localStrength_sb->setMaximum(ISPC::ControlTNM::TNMC_LOCAL_STRENGTH.max);
	
	//
	// Set precision
	//

	DEFINE_PREC(TNM_minY_sb, 1.0f);
	DEFINE_PREC(TNM_maxY_sb, 1.0f);
	DEFINE_PREC(TNM_colourConfidence_sb, 1024.0000f);
	DEFINE_PREC(TNM_colourSaturation_sb, 1024.0000f);
	DEFINE_PREC(TNM_lineWeight_sb, 1024.0000f);
	DEFINE_PREC(TNM_localWeight_sb, 1024.0000f);
	DEFINE_PREC(TNM_flattening_sb, 1024.0000f);
	DEFINE_PREC(TNM_minFlattening_sb, 1024.0000f);

	DEFINE_PREC(TNM_current_minY_sb, 1.0f);
	DEFINE_PREC(TNM_current_maxY_sb, 1.0f);
	DEFINE_PREC(TNM_current_colourConfidence_sb, 1024.0000f);
	DEFINE_PREC(TNM_current_colourSaturation_sb, 1024.0000f);
	DEFINE_PREC(TNM_current_lineWeight_sb, 1024.0000f);
	DEFINE_PREC(TNM_current_localWeight_sb, 1024.0000f);
	DEFINE_PREC(TNM_current_flattening_sb, 1024.0000f);
	DEFINE_PREC(TNM_current_minFlattening_sb, 1024.0000f);

	//
	// Set value
	//

	// Manual TNM
	TNM_minY_sb->setValue(tnm.aInY[0]);
	TNM_maxY_sb->setValue(tnm.aInY[1]);
	TNM_bypass_cb->setChecked(tnm.bBypass);
	TNM_colourConfidence_sb->setValue(tnm.fColourConfidence);
	TNM_colourSaturation_sb->setValue(tnm.fColourSaturation);
	TNM_lineWeight_sb->setValue(tnm.fWeightLine);
	TNM_localWeight_sb->setValue(tnm.fWeightLocal);
	TNM_flattening_sb->setValue(tnm.fFlatFactor);
	TNM_minFlattening_sb->setValue(tnm.fFlatMin);

	// Curve
	QVector<QPointF> data;
	data.push_back(QPointF(0.0f, 0.0f));
	for (int i = 0 ; i < TNM_CURVE_NPOINTS; i++)
	{
		data.push_back(QPointF(i, tnm.aCurve[i]));
	}
	data.push_back(QPointF(TNM_CURVE_NPOINTS, 1.0f));
	_pCurve->setData(data);
	_pCurve->replotCurve();

	// Auto TNM
	if(config.exists(TNM_ENABLE))
	{
		ATNM_enable_cb->setChecked(config.getParameter(TNM_ENABLE)->get<bool>());
	}
	ATNM_adaptive_cb->setChecked(ctnm.getAdaptiveTNM());
	ATNM_local_cb->setChecked(ctnm.getLocalTNM());
	ATNM_updateSpeed_sb->setValue(ctnm.getUpdateSpeed());
	ATNM_minHist_sb->setValue(ctnm.getHistMin());
	ATNM_maxHist_sb->setValue(ctnm.getHistMax());
	ATNM_tempering_sb->setValue(ctnm.getTempering());
	ATNM_smoothing_sb->setValue(ctnm.getSmoothing());
	ATNM_localStrength_sb->setValue(ctnm.getLocalStrength());

}

void VisionLive::ModuleTNM::save(ISPC::ParameterList &config) const
{
	// Manual TNM
	ISPC::ModuleTNM tnm;
	tnm.load(config);

	if(!TNM_bypass_cb->isChecked())
	{
		tnm.aInY[0] = TNM_minY_sb->value();
		tnm.aInY[1] = TNM_maxY_sb->value();
		tnm.bBypass = ISPC::ModuleTNM::TNM_BYPASS.def;
		tnm.fColourConfidence = TNM_colourConfidence_sb->value();
		tnm.fColourSaturation = TNM_colourSaturation_sb->value();
		tnm.fWeightLine = TNM_lineWeight_sb->value();
		tnm.fWeightLocal = TNM_localWeight_sb->value();
		tnm.fFlatFactor = TNM_flattening_sb->value();
		tnm.fFlatMin = TNM_minFlattening_sb->value();
	}
	else
	{
		tnm.aInY[0] = ISPC::ModuleTNM::TNM_IN_Y.def[0];
		tnm.aInY[1] = ISPC::ModuleTNM::TNM_IN_Y.def[1];
		tnm.bBypass = TNM_bypass_cb->isChecked();
		tnm.fColourConfidence = ISPC::ModuleTNM::TNM_COLOUR_CONFIDENCE.def;
		tnm.fColourSaturation = ISPC::ModuleTNM::TNM_COLOUR_SATURATION.def;
		tnm.fWeightLine = ISPC::ModuleTNM::TNM_WEIGHT_LINE.def;
		tnm.fWeightLocal = ISPC::ModuleTNM::TNM_WEIGHT_LOCAL.def;
		tnm.fFlatFactor = ISPC::ModuleTNM::TNM_FLAT_FACTOR.def;
		tnm.fFlatMin = ISPC::ModuleTNM::TNM_FLAT_MIN.def;
	}

	// Curve
	const QVector<QPointF> &data = _pCurve->curveData();
    if (data.size() == TNM_IN_MAX_C + 1)
    {
		for (int i = 1; i < TNM_IN_MAX_C; i++)
        {
			tnm.aCurve[i-1] = data.at(i).y();
        }
    }

	tnm.save(config, ISPC::ModuleTNM::SAVE_VAL);

	// Auto TNM
	ISPC::ControlTNM ctnm;
	ctnm.load(config);

	ISPC::Parameter tnm_enable(TNM_ENABLE, ATNM_enable_cb->isChecked()? "1" : "0");
	config.addParameter(tnm_enable, true);
	ctnm.enableAdaptiveTNM(ATNM_adaptive_cb->isChecked());
	ctnm.enableLocalTNM(ATNM_local_cb->isChecked());
	ctnm.setHistMin(ATNM_minHist_sb->value());
	ctnm.setHistMax(ATNM_maxHist_sb->value());
	ctnm.setTempering(ATNM_tempering_sb->value());
	ctnm.setSmoothing(ATNM_smoothing_sb->value());
	ctnm.setLocalStrength(ATNM_localStrength_sb->value());
	ctnm.setUpdateSpeed(ATNM_updateSpeed_sb->value());

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
		_pProxyHandler->AWB_set(ATNM_enable_cb->isChecked());
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
    TNM_curveLayout->addWidget(_pCurve);

	setEnables(ATNM_enable_cb->isChecked());

	QObject::connect(ATNM_enable_cb, SIGNAL(stateChanged(int)), this, SLOT(TNM_enable(int)));
}

//
// Private Functions
//

void VisionLive::ModuleTNM::setEnables(bool autoEnabled)
{
	if(ATNM_enable_cb->isChecked())
	{
		// Manual TNM
		TNM_manual_gb->setEnabled(false);

		// Current TNM
		TNM_current_minY_sb->setEnabled(true);
		TNM_current_maxY_sb->setEnabled(true);
		TNM_current_colourConfidence_sb->setEnabled(true);
		TNM_current_colourSaturation_sb->setEnabled(true);
		TNM_current_lineWeight_sb->setEnabled(true);
		TNM_current_localWeight_sb->setEnabled(true);
		TNM_current_flattening_sb->setEnabled(true);
		TNM_current_minFlattening_sb->setEnabled(true);

		// Auto TNM
		ATNM_adaptive_cb->setEnabled(true);
		ATNM_local_cb->setEnabled(true);
		ATNM_updateSpeed_sb->setEnabled(true);
		ATNM_minHist_sb->setEnabled(true);
		ATNM_maxHist_sb->setEnabled(true);
		ATNM_tempering_sb->setEnabled(true);
		ATNM_smoothing_sb->setEnabled(true);
		ATNM_localStrength_sb->setEnabled(true);
	}
	else
	{
		TNM_manual_gb->setEnabled(true);

		// Current TNM
		TNM_current_minY_sb->setEnabled(false);
		TNM_current_maxY_sb->setEnabled(false);
		TNM_current_colourConfidence_sb->setEnabled(false);
		TNM_current_colourSaturation_sb->setEnabled(false);
		TNM_current_lineWeight_sb->setEnabled(false);
		TNM_current_localWeight_sb->setEnabled(false);
		TNM_current_flattening_sb->setEnabled(false);
		TNM_current_minFlattening_sb->setEnabled(false);

		// Auto TNM
		ATNM_adaptive_cb->setEnabled(false);
		ATNM_local_cb->setEnabled(false);
		ATNM_updateSpeed_sb->setEnabled(false);
		ATNM_minHist_sb->setEnabled(false);
		ATNM_maxHist_sb->setEnabled(false);
		ATNM_tempering_sb->setEnabled(false);
		ATNM_smoothing_sb->setEnabled(false);
		ATNM_localStrength_sb->setEnabled(false);
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

	setEnables(ATNM_enable_cb->isChecked());

	if(_pCurve)
	{
		_pCurve->curveControl_grp->setChecked(!ATNM_enable_cb->isChecked());
		_pCurve->curveControl_grp->setEnabled(!ATNM_enable_cb->isChecked());
	}
}

void VisionLive::ModuleTNM::TNM_received(QMap<QString, QString> params, QVector<QPointF> curve)
{
	if(ATNM_enable_cb->isChecked())
	{
		_pCurve->setData(curve);

		TNM_current_minY_sb->setValue(params.find("LumaMin").value().toDouble());
		TNM_current_maxY_sb->setValue(params.find("LumaMax").value().toDouble());
		//TNM_current_bypass_cb->setChecked(params.find("Bypass").value().toInt());
		TNM_current_colourConfidence_sb->setValue(params.find("ColourConfidance").value().toDouble());
		TNM_current_colourSaturation_sb->setValue(params.find("ColourSaturation").value().toDouble());
		TNM_current_lineWeight_sb->setValue(params.find("WeightLine").value().toDouble());
		TNM_current_localWeight_sb->setValue(params.find("WeightLocal").value().toDouble());
		TNM_current_flattening_sb->setValue(params.find("FlatFactor").value().toDouble());
		TNM_current_minFlattening_sb->setValue(params.find("FlatMin").value().toDouble());
	}
}
