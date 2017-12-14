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
	switch(_css)
	{
	case CUSTOM_CSS:
		coeff0_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		coeff1_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		coeff2_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		coeff3_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		coeff4_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		coeff5_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);

		slope0_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		slope1_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		slope2_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);

		clipMin_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		srcNorm_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		clipMax_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		break;

	case DEFAULT_CSS:
		coeff0_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		coeff1_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		coeff2_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		coeff3_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		coeff4_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		coeff5_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);

		slope0_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		slope1_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		slope2_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);

		clipMin_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		srcNorm_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		clipMax_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
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

	coeff0_sb->setMinimum(mgm.MGM_COEFF.min);
	coeff1_sb->setMinimum(mgm.MGM_COEFF.min);
	coeff2_sb->setMinimum(mgm.MGM_COEFF.min);
	coeff3_sb->setMinimum(mgm.MGM_COEFF.min);
	coeff4_sb->setMinimum(mgm.MGM_COEFF.min);
	coeff5_sb->setMinimum(mgm.MGM_COEFF.min);

	slope0_sb->setMinimum(mgm.MGM_SLOPE.min);
	slope1_sb->setMinimum(mgm.MGM_SLOPE.min);
	slope2_sb->setMinimum(mgm.MGM_SLOPE.min);

	clipMin_sb->setMinimum(mgm.MGM_CLIP_MIN.min);
	srcNorm_sb->setMinimum(mgm.MGM_CLIP_MAX.min);
	clipMax_sb->setMinimum(mgm.MGM_SRC_NORM.min);

	//
	// Set maximum
	//

	coeff0_sb->setMaximum(mgm.MGM_COEFF.max);
	coeff1_sb->setMaximum(mgm.MGM_COEFF.max);
	coeff2_sb->setMaximum(mgm.MGM_COEFF.max);
	coeff3_sb->setMaximum(mgm.MGM_COEFF.max);
	coeff4_sb->setMaximum(mgm.MGM_COEFF.max);
	coeff5_sb->setMaximum(mgm.MGM_COEFF.max);

	slope0_sb->setMaximum(mgm.MGM_SLOPE.max);
	slope1_sb->setMaximum(mgm.MGM_SLOPE.max);
	slope2_sb->setMaximum(mgm.MGM_SLOPE.max);

	clipMin_sb->setMaximum(mgm.MGM_CLIP_MIN.max);
	srcNorm_sb->setMaximum(mgm.MGM_CLIP_MAX.max);
	clipMax_sb->setMaximum(mgm.MGM_SRC_NORM.max);

	//
	// Set precision
	//

	//
	// Set value
	//

	coeff0_sb->setValue(mgm.aCoeff[0]);
	coeff1_sb->setValue(mgm.aCoeff[1]);
	coeff2_sb->setValue(mgm.aCoeff[2]);
	coeff3_sb->setValue(mgm.aCoeff[3]);
	coeff4_sb->setValue(mgm.aCoeff[4]);
	coeff5_sb->setValue(mgm.aCoeff[5]);

	slope0_sb->setValue(mgm.aSlope[0]);
	slope1_sb->setValue(mgm.aSlope[1]);
	slope2_sb->setValue(mgm.aSlope[2]);

	clipMin_sb->setValue(mgm.fClipMin);
	srcNorm_sb->setValue(mgm.fSrcNorm);
	clipMax_sb->setValue(mgm.fClipMax);
}

void VisionLive::ModuleMGM::save(ISPC::ParameterList &config)
{
	ISPC::ModuleMGM mgm;
	mgm.load(config);

	mgm.aCoeff[0] = coeff0_sb->value();
	mgm.aCoeff[1] = coeff1_sb->value();
	mgm.aCoeff[2] = coeff2_sb->value();
	mgm.aCoeff[3] = coeff3_sb->value();
	mgm.aCoeff[4] = coeff4_sb->value();
	mgm.aCoeff[5] = coeff5_sb->value();

	mgm.aSlope[0] = slope0_sb->value();
	mgm.aSlope[1] = slope1_sb->value();
	mgm.aSlope[2] = slope2_sb->value();

	mgm.fClipMin = clipMin_sb->value();
	mgm.fSrcNorm = srcNorm_sb->value();
	mgm.fClipMax = clipMax_sb->value();
	
	mgm.save(config, ISPC::ModuleBase::SAVE_VAL);
}

//
// Protected Functions
//

void VisionLive::ModuleMGM::initModule()
{
	_name = windowTitle();
}
