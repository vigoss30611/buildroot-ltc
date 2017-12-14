#include "dgm.hpp"

#include "ispc/ModuleDGM.h"

//
// Public Functions
//

VisionLive::ModuleDGM::ModuleDGM(QWidget *parent): ModuleBase(parent)
{
	Ui::DGM::setupUi(this);

	retranslate();

	initModule();

	initObjectsConnections();
}

VisionLive::ModuleDGM::~ModuleDGM()
{
}

void VisionLive::ModuleDGM::removeObjectValueChangedMark()
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

void VisionLive::ModuleDGM::retranslate()
{
	Ui::DGM::retranslateUi(this);
}

void VisionLive::ModuleDGM::load(const ISPC::ParameterList &config)
{
	ISPC::ModuleDGM dgm;
	dgm.load(config);

	//
	// Set minimum
	//

	coeff0_sb->setMinimum(dgm.DGM_COEFF.min);
	coeff1_sb->setMinimum(dgm.DGM_COEFF.min);
	coeff2_sb->setMinimum(dgm.DGM_COEFF.min);
	coeff3_sb->setMinimum(dgm.DGM_COEFF.min);
	coeff4_sb->setMinimum(dgm.DGM_COEFF.min);
	coeff5_sb->setMinimum(dgm.DGM_COEFF.min);

	slope0_sb->setMinimum(dgm.DGM_SLOPE.min);
	slope1_sb->setMinimum(dgm.DGM_SLOPE.min);
	slope2_sb->setMinimum(dgm.DGM_SLOPE.min);

	clipMin_sb->setMinimum(dgm.DGM_CLIP_MIN.min);
	srcNorm_sb->setMinimum(dgm.DGM_CLIP_MAX.min);
	clipMax_sb->setMinimum(dgm.DGM_SRC_NORM.min);

	//
	// Set maximum
	//

	coeff0_sb->setMaximum(dgm.DGM_COEFF.max);
	coeff1_sb->setMaximum(dgm.DGM_COEFF.max);
	coeff2_sb->setMaximum(dgm.DGM_COEFF.max);
	coeff3_sb->setMaximum(dgm.DGM_COEFF.max);
	coeff4_sb->setMaximum(dgm.DGM_COEFF.max);
	coeff5_sb->setMaximum(dgm.DGM_COEFF.max);

	slope0_sb->setMaximum(dgm.DGM_SLOPE.max);
	slope1_sb->setMaximum(dgm.DGM_SLOPE.max);
	slope2_sb->setMaximum(dgm.DGM_SLOPE.max);

	clipMin_sb->setMaximum(dgm.DGM_CLIP_MIN.max);
	srcNorm_sb->setMaximum(dgm.DGM_CLIP_MAX.max);
	clipMax_sb->setMaximum(dgm.DGM_SRC_NORM.max);
	
	//
	// Set precision
	//

	//
	// Set value
	//

	coeff0_sb->setValue(dgm.aCoeff[0]);
	coeff1_sb->setValue(dgm.aCoeff[1]);
	coeff2_sb->setValue(dgm.aCoeff[2]);
	coeff3_sb->setValue(dgm.aCoeff[3]);
	coeff4_sb->setValue(dgm.aCoeff[4]);
	coeff5_sb->setValue(dgm.aCoeff[5]);

	slope0_sb->setValue(dgm.aSlope[0]);
	slope1_sb->setValue(dgm.aSlope[1]);
	slope2_sb->setValue(dgm.aSlope[2]);

	clipMin_sb->setValue(dgm.fClipMin);
	srcNorm_sb->setValue(dgm.fSrcNorm);
	clipMax_sb->setValue(dgm.fClipMax);
}

void VisionLive::ModuleDGM::save(ISPC::ParameterList &config)
{
	ISPC::ModuleDGM dgm;
	dgm.load(config);

	dgm.aCoeff[0] = coeff0_sb->value();
	dgm.aCoeff[1] = coeff1_sb->value();
	dgm.aCoeff[2] = coeff2_sb->value();
	dgm.aCoeff[3] = coeff3_sb->value();
	dgm.aCoeff[4] = coeff4_sb->value();
	dgm.aCoeff[5] = coeff5_sb->value();

	dgm.aSlope[0] = slope0_sb->value();
	dgm.aSlope[1] = slope1_sb->value();
	dgm.aSlope[2] = slope2_sb->value();

	dgm.fClipMin = clipMin_sb->value();
	dgm.fSrcNorm = srcNorm_sb->value();
	dgm.fClipMax = clipMax_sb->value();
	
	dgm.save(config, ISPC::ModuleBase::SAVE_VAL);
}

//
// Protected Functions
//

void VisionLive::ModuleDGM::initModule()
{
	_name = windowTitle();
}
