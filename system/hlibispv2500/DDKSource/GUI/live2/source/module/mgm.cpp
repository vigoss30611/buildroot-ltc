#include "mgm.hpp"

#include "ispc/ModuleMGM.h"

//
// Public Functions
//

VisionLive::ModuleMGM::ModuleMGM(QWidget *parent): ModuleBase(parent)
{
	Ui::MGM::setupUi(this);

	retranslate();

	initModule();

	initObjectsConnections();
}

VisionLive::ModuleMGM::~ModuleMGM()
{
}

void VisionLive::ModuleMGM::removeObjectValueChangedMark()
{
	switch(css)
	{
	case CUSTOM_CSS:
		C0_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		C1_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		C2_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		C3_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		C4_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		C5_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);

		S0_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		S1_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		S2_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);

		P0_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		P1_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		P2_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		break;

	case DEFAULT_CSS:
		C0_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		C1_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		C2_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		C3_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		C4_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		C5_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);

		S0_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		S1_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		S2_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);

		P0_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		P1_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		P2_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		break;
	}
}

void VisionLive::ModuleMGM::retranslate()
{
	Ui::MGM::retranslateUi(this);
}

void VisionLive::ModuleMGM::load(const ISPC::ParameterList &config)
{
	ISPC::ModuleMGM mgm;
	mgm.load(config);
	
	//
	// Set minimum
	//

	C0_sb->setMinimum(mgm.MGM_COEFF.min);
	C1_sb->setMinimum(mgm.MGM_COEFF.min);
	C2_sb->setMinimum(mgm.MGM_COEFF.min);
	C3_sb->setMinimum(mgm.MGM_COEFF.min);
	C4_sb->setMinimum(mgm.MGM_COEFF.min);
	C5_sb->setMinimum(mgm.MGM_COEFF.min);

	S0_sb->setMinimum(mgm.MGM_SLOPE.min);
	S1_sb->setMinimum(mgm.MGM_SLOPE.min);
	S2_sb->setMinimum(mgm.MGM_SLOPE.min);

	P0_sb->setMinimum(mgm.MGM_CLIP_MIN.min);
	P1_sb->setMinimum(mgm.MGM_CLIP_MAX.min);
	P2_sb->setMinimum(mgm.MGM_SRC_NORM.min);

	//
	// Set maximum
	//

	C0_sb->setMaximum(mgm.MGM_COEFF.max);
	C1_sb->setMaximum(mgm.MGM_COEFF.max);
	C2_sb->setMaximum(mgm.MGM_COEFF.max);
	C3_sb->setMaximum(mgm.MGM_COEFF.max);
	C4_sb->setMaximum(mgm.MGM_COEFF.max);
	C5_sb->setMaximum(mgm.MGM_COEFF.max);

	S0_sb->setMaximum(mgm.MGM_SLOPE.max);
	S1_sb->setMaximum(mgm.MGM_SLOPE.max);
	S2_sb->setMaximum(mgm.MGM_SLOPE.max);

	P0_sb->setMaximum(mgm.MGM_CLIP_MIN.max);
	P1_sb->setMaximum(mgm.MGM_CLIP_MAX.max);
	P2_sb->setMaximum(mgm.MGM_SRC_NORM.max);

	//
	// Set precision
	//

	//
	// Set value
	//

	C0_sb->setValue(mgm.aCoeff[0]);
	C1_sb->setValue(mgm.aCoeff[1]);
	C2_sb->setValue(mgm.aCoeff[2]);
	C3_sb->setValue(mgm.aCoeff[3]);
	C4_sb->setValue(mgm.aCoeff[4]);
	C5_sb->setValue(mgm.aCoeff[5]);

	S0_sb->setValue(mgm.aSlope[0]);
	S1_sb->setValue(mgm.aSlope[1]);
	S2_sb->setValue(mgm.aSlope[2]);

	P0_sb->setValue(mgm.fClipMin);
	P1_sb->setValue(mgm.fSrcNorm);
	P2_sb->setValue(mgm.fClipMax);
}

void VisionLive::ModuleMGM::save(ISPC::ParameterList &config) const
{
	ISPC::ModuleMGM mgm;
	mgm.load(config);

	mgm.aCoeff[0] = C0_sb->value();
	mgm.aCoeff[1] = C1_sb->value();
	mgm.aCoeff[2] = C2_sb->value();
	mgm.aCoeff[3] = C3_sb->value();
	mgm.aCoeff[4] = C4_sb->value();
	mgm.aCoeff[5] = C5_sb->value();

	mgm.aSlope[0] = S0_sb->value();
	mgm.aSlope[1] = S1_sb->value();
	mgm.aSlope[2] = S2_sb->value();

	mgm.fClipMin = P0_sb->value();
	mgm.fSrcNorm = P1_sb->value();
	mgm.fClipMax = P2_sb->value();
	
	mgm.save(config, ISPC::ModuleBase::SAVE_VAL);
}

//
// Protected Functions
//

void VisionLive::ModuleMGM::initModule()
{
	_name = windowTitle();
}
