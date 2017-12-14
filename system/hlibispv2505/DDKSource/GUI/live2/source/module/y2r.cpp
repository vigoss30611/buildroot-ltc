#include "y2r.hpp"

#include "ispc/ModuleY2R.h"
#include "ispc/ModuleR2Y.h"

//
// Public Functions
//

VisionLive::ModuleY2R::ModuleY2R(QWidget *parent): ModuleBase(parent)
{
	Ui::Y2R::setupUi(this);

	retranslate();

	initModule();

	initObjectsConnections();
}

VisionLive::ModuleY2R::~ModuleY2R()
{
}

void VisionLive::ModuleY2R::removeObjectValueChangedMark()
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

void VisionLive::ModuleY2R::retranslate()
{
	Ui::Y2R::retranslateUi(this);

    int currMatrix = conversionMatrix_lb->currentIndex();
    conversionMatrix_lb->clear();
    for (int i = ISPC::ModuleR2YBase::STD_MIN; i <= ISPC::ModuleR2YBase::STD_MAX; i++)
    {
        if (i != ISPC::ModuleR2YBase::JFIF) // Currently unsupported
            conversionMatrix_lb->addItem(ISPC::ModuleR2YBase::stdMatrixName((ISPC::ModuleR2YBase::StdMatrix)i));
    }
    conversionMatrix_lb->setCurrentIndex(currMatrix);
}

void VisionLive::ModuleY2R::load(const ISPC::ParameterList &config)
{
	ISPC::ModuleY2R y2r;
	y2r.load(config);

	//
	// Set minimum
	//

	brightness_sb->setMinimum(ISPC::ModuleY2R::Y2R_BRIGHTNESS.min);
	contrast_sb->setMinimum(ISPC::ModuleY2R::Y2R_CONTRAST.min);
	saturation_sb->setMinimum(ISPC::ModuleY2R::Y2R_SATURATION.min);
	hue_sb->setMinimum(ISPC::ModuleY2R::Y2R_HUE.min);
	multY_sb->setMinimum(ISPC::ModuleY2R::Y2R_RANGEMULT.min);
	multCb_sb->setMinimum(ISPC::ModuleY2R::Y2R_RANGEMULT.min);
	multCr_sb->setMinimum(ISPC::ModuleY2R::Y2R_RANGEMULT.min);
	offsetU_sb->setMinimum(ISPC::ModuleY2R::Y2R_OFFSETU.min);
	offsetV_sb->setMinimum(ISPC::ModuleY2R::Y2R_OFFSETV.min);

	//
	// Set maximum
	//

	brightness_sb->setMaximum(ISPC::ModuleY2R::Y2R_BRIGHTNESS.max);
	contrast_sb->setMaximum(ISPC::ModuleY2R::Y2R_CONTRAST.max);
	saturation_sb->setMaximum(ISPC::ModuleY2R::Y2R_SATURATION.max);
	hue_sb->setMaximum(ISPC::ModuleY2R::Y2R_HUE.max);
	multY_sb->setMaximum(ISPC::ModuleY2R::Y2R_RANGEMULT.max);
	multCb_sb->setMaximum(ISPC::ModuleY2R::Y2R_RANGEMULT.max);
	multCr_sb->setMaximum(ISPC::ModuleY2R::Y2R_RANGEMULT.max);
	offsetU_sb->setMaximum(ISPC::ModuleY2R::Y2R_OFFSETU.max);
	offsetV_sb->setMaximum(ISPC::ModuleY2R::Y2R_OFFSETV.max);

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

	brightness_sb->setValue(y2r.fBrightness);
	contrast_sb->setValue(y2r.fContrast);
	saturation_sb->setValue(y2r.fSaturation);
	hue_sb->setValue(y2r.fHue);
	conversionMatrix_lb->setCurrentIndex(y2r.eMatrix);
	multY_sb->setValue(y2r.aRangeMult[0]);
	multCb_sb->setValue(y2r.aRangeMult[1]);
	multCr_sb->setValue(y2r.aRangeMult[2]);
	offsetU_sb->setValue(y2r.fOffsetU);
	offsetV_sb->setValue(y2r.fOffsetV);
}

void VisionLive::ModuleY2R::save(ISPC::ParameterList &config)
{
	ISPC::ModuleY2R y2r;
	y2r.load(config);

	y2r.fBrightness = brightness_sb->value();
	y2r.fContrast = contrast_sb->value();
	y2r.fSaturation = saturation_sb->value();
	y2r.fHue = hue_sb->value();
	y2r.eMatrix = (ISPC::ModuleR2YBase::StdMatrix)conversionMatrix_lb->currentIndex();
	y2r.aRangeMult[0] = multY_sb->value();
	y2r.aRangeMult[1] = multCb_sb->value();
	y2r.aRangeMult[2] = multCr_sb->value();
	y2r.fOffsetU = offsetU_sb->value();
	y2r.fOffsetV = offsetV_sb->value();

	y2r.save(config, ISPC::ModuleY2R::SAVE_VAL);
}

//
// Protected Functions
//

void VisionLive::ModuleY2R::initModule()
{
	_name = windowTitle();
}
