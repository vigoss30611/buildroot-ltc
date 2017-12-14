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

	C0_sb->setMinimum(dgm.DGM_COEFF.min);
	C1_sb->setMinimum(dgm.DGM_COEFF.min);
	C2_sb->setMinimum(dgm.DGM_COEFF.min);
	C3_sb->setMinimum(dgm.DGM_COEFF.min);
	C4_sb->setMinimum(dgm.DGM_COEFF.min);
	C5_sb->setMinimum(dgm.DGM_COEFF.min);

	S0_sb->setMinimum(dgm.DGM_SLOPE.min);
	S1_sb->setMinimum(dgm.DGM_SLOPE.min);
	S2_sb->setMinimum(dgm.DGM_SLOPE.min);

	P0_sb->setMinimum(dgm.DGM_CLIP_MIN.min);
	P1_sb->setMinimum(dgm.DGM_CLIP_MAX.min);
	P2_sb->setMinimum(dgm.DGM_SRC_NORM.min);

	//
	// Set maximum
	//

	C0_sb->setMaximum(dgm.DGM_COEFF.max);
	C1_sb->setMaximum(dgm.DGM_COEFF.max);
	C2_sb->setMaximum(dgm.DGM_COEFF.max);
	C3_sb->setMaximum(dgm.DGM_COEFF.max);
	C4_sb->setMaximum(dgm.DGM_COEFF.max);
	C5_sb->setMaximum(dgm.DGM_COEFF.max);

	S0_sb->setMaximum(dgm.DGM_SLOPE.max);
	S1_sb->setMaximum(dgm.DGM_SLOPE.max);
	S2_sb->setMaximum(dgm.DGM_SLOPE.max);

	P0_sb->setMaximum(dgm.DGM_CLIP_MIN.max);
	P1_sb->setMaximum(dgm.DGM_CLIP_MAX.max);
	P2_sb->setMaximum(dgm.DGM_SRC_NORM.max);
	
	//
	// Set precision
	//

	//
	// Set value
	//

	C0_sb->setValue(dgm.aCoeff[0]);
	C1_sb->setValue(dgm.aCoeff[1]);
	C2_sb->setValue(dgm.aCoeff[2]);
	C3_sb->setValue(dgm.aCoeff[3]);
	C4_sb->setValue(dgm.aCoeff[4]);
	C5_sb->setValue(dgm.aCoeff[5]);

	S0_sb->setValue(dgm.aSlope[0]);
	S1_sb->setValue(dgm.aSlope[1]);
	S2_sb->setValue(dgm.aSlope[2]);

	P0_sb->setValue(dgm.fClipMin);
	P1_sb->setValue(dgm.fSrcNorm);
	P2_sb->setValue(dgm.fClipMax);
}

void VisionLive::ModuleDGM::save(ISPC::ParameterList &config) const
{
	ISPC::ModuleDGM dgm;
	dgm.load(config);

	dgm.aCoeff[0] = C0_sb->value();
	dgm.aCoeff[1] = C1_sb->value();
	dgm.aCoeff[2] = C2_sb->value();
	dgm.aCoeff[3] = C3_sb->value();
	dgm.aCoeff[4] = C4_sb->value();
	dgm.aCoeff[5] = C5_sb->value();

	dgm.aSlope[0] = S0_sb->value();
	dgm.aSlope[1] = S1_sb->value();
	dgm.aSlope[2] = S2_sb->value();

	dgm.fClipMin = P0_sb->value();
	dgm.fSrcNorm = P1_sb->value();
	dgm.fClipMax = P2_sb->value();
	
	dgm.save(config, ISPC::ModuleBase::SAVE_VAL);
}

//
// Protected Functions
//

void VisionLive::ModuleDGM::initModule()
{
	_name = windowTitle();
}
