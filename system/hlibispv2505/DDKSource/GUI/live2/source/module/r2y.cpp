#include "r2y.hpp"

#include "ispc/ModuleR2Y.h"

//
// Public Functions
//

VisionLive::ModuleR2Y::ModuleR2Y(QWidget *parent): ModuleBase(parent)
{
	Ui::R2Y::setupUi(this);

	retranslate();

	initModule();

	initObjectsConnections();
}

VisionLive::ModuleR2Y::~ModuleR2Y()
{
}

void VisionLive::ModuleR2Y::removeObjectValueChangedMark()
{
	switch(_css)
	{
	case CUSTOM_CSS:
		brightness_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		contrast_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		saturation_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		hue_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		conversionMatrix_lb->setStyleSheet(CSS_CUSTOM_QCOMBOBOX);
		multY_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		multCb_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		multCr_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		offsetU_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		offsetV_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		break;

	case DEFAULT_CSS:
		brightness_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		contrast_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		saturation_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		hue_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		conversionMatrix_lb->setStyleSheet(CSS_DEFAULT_QCOMBOBOX);
		multY_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		multCb_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		multCr_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		offsetU_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		offsetV_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		break;
	}
}

void VisionLive::ModuleR2Y::retranslate()
{
	Ui::R2Y::retranslateUi(this);

    int currMatrix = conversionMatrix_lb->currentIndex();
    conversionMatrix_lb->clear();
    for (int i = ISPC::ModuleR2YBase::STD_MIN; i <= ISPC::ModuleR2YBase::STD_MAX; i++)
    {
        if (i != ISPC::ModuleR2YBase::JFIF) // Currently unsupported
            conversionMatrix_lb->addItem(ISPC::ModuleR2YBase::stdMatrixName((ISPC::ModuleR2YBase::StdMatrix)i));
    }
    conversionMatrix_lb->setCurrentIndex(currMatrix);
}

void VisionLive::ModuleR2Y::load(const ISPC::ParameterList &config)
{
	ISPC::ModuleR2Y r2y;
	r2y.load(config);

	//
	// Set minimum
	//

	brightness_sb->setMinimum(ISPC::ModuleR2Y::R2Y_BRIGHTNESS.min);
	contrast_sb->setMinimum(ISPC::ModuleR2Y::R2Y_CONTRAST.min);
	saturation_sb->setMinimum(ISPC::ModuleR2Y::R2Y_SATURATION.min);
	hue_sb->setMinimum(ISPC::ModuleR2Y::R2Y_HUE.min);
	multY_sb->setMinimum(ISPC::ModuleR2Y::R2Y_RANGEMULT.min);
	multCb_sb->setMinimum(ISPC::ModuleR2Y::R2Y_RANGEMULT.min);
	multCr_sb->setMinimum(ISPC::ModuleR2Y::R2Y_RANGEMULT.min);
	offsetU_sb->setMinimum(ISPC::ModuleR2Y::R2Y_OFFSETU.min);
	offsetV_sb->setMinimum(ISPC::ModuleR2Y::R2Y_OFFSETV.min);

	//
	// Set maximum
	//

	brightness_sb->setMaximum(ISPC::ModuleR2Y::R2Y_BRIGHTNESS.max);
	contrast_sb->setMaximum(ISPC::ModuleR2Y::R2Y_CONTRAST.max);
	saturation_sb->setMaximum(ISPC::ModuleR2Y::R2Y_SATURATION.max);
	hue_sb->setMaximum(ISPC::ModuleR2Y::R2Y_HUE.max);
	multY_sb->setMaximum(ISPC::ModuleR2Y::R2Y_RANGEMULT.max);
	multCb_sb->setMaximum(ISPC::ModuleR2Y::R2Y_RANGEMULT.max);
	multCr_sb->setMaximum(ISPC::ModuleR2Y::R2Y_RANGEMULT.max);
	offsetU_sb->setMaximum(ISPC::ModuleR2Y::R2Y_OFFSETU.max);
	offsetV_sb->setMaximum(ISPC::ModuleR2Y::R2Y_OFFSETV.max);

	//
	// Set precision
	//

	DEFINE_PREC(brightness_sb, 4095.0000f);
	DEFINE_PREC(contrast_sb, 1024.0000f);
	DEFINE_PREC(saturation_sb, 1024.0000f);
	DEFINE_PREC(hue_sb, 256.000f);
	DEFINE_PREC(multY_sb, 1024.0000f);
	DEFINE_PREC(multCb_sb, 1024.0000f);
	DEFINE_PREC(multCr_sb, 1024.0000f);
	DEFINE_PREC(offsetU_sb, 1024.0000f);
	DEFINE_PREC(offsetV_sb, 1024.0000f);

	//
	// Set value
	//

	brightness_sb->setValue(r2y.fBrightness);
	contrast_sb->setValue(r2y.fContrast);
	saturation_sb->setValue(r2y.fSaturation);
	hue_sb->setValue(r2y.fHue);
	conversionMatrix_lb->setCurrentIndex(r2y.eMatrix);
	multY_sb->setValue(r2y.aRangeMult[0]);
	multCb_sb->setValue(r2y.aRangeMult[1]);
	multCr_sb->setValue(r2y.aRangeMult[2]);
	offsetU_sb->setValue(r2y.fOffsetU);
	offsetV_sb->setValue(r2y.fOffsetV);
}

void VisionLive::ModuleR2Y::save(ISPC::ParameterList &config)
{
	ISPC::ModuleR2Y r2y;
	r2y.load(config);

	r2y.fBrightness = brightness_sb->value();
	r2y.fContrast = contrast_sb->value();
	r2y.fSaturation = saturation_sb->value();
	r2y.fHue = hue_sb->value();
	r2y.eMatrix = (ISPC::ModuleR2YBase::StdMatrix)conversionMatrix_lb->currentIndex();
	r2y.aRangeMult[0] = multY_sb->value();
	r2y.aRangeMult[1] = multCb_sb->value();
	r2y.aRangeMult[2] = multCr_sb->value();
	r2y.fOffsetU = offsetU_sb->value();
	r2y.fOffsetV = offsetV_sb->value();

	r2y.save(config, ISPC::ModuleR2Y::SAVE_VAL);
}

//
// Protected Functions
//

void VisionLive::ModuleR2Y::initModule()
{
	_name = windowTitle();
}
