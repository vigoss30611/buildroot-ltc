/********************************************************************************
@file lbcconfig.cpp

@brief LBCConfig class implementation

@copyright Imagination Technologies Ltd. All Rights Reserved.

@license Strictly Confidential.
No part of this software, either material or conceptual may be copied or
distributed, transmitted, transcribed, stored in a retrieval system or
translated into any human or computer language in any form by any means,
electronic, mechanical, manual or other-wise, or disclosed to third
parties without the express written permission of
Imagination Technologies Limited,
Unit 8, HomePark Industrial Estate,
King's Langley, Hertfordshire,
WD4 8LZ, U.K.

******************************************************************************/
#include "lbc/config.hpp"
#include "lbc/tuning.hpp"
#include "ui/ui_lbcwidget.h"
#include <ispc/ControlLBC.h>


void LBCConfig::init()
{
	Ui::LBCWidget::setupUi(this);

	sharpnessDoubleBoxSlider = new QtExtra::DoubleBoxSlider(sharpness_spin, sharpness_slide);
	brightnessDoubleBoxSlider = new QtExtra::DoubleBoxSlider(brightness_spin, brightness_slide);
	contrastDoubleBoxSlider = new QtExtra::DoubleBoxSlider(contrast_spin, contrast_slide);
	saturationDoubleBoxSlider = new QtExtra::DoubleBoxSlider(saturation_spin, saturation_slide);

	lightLevel_spin->setMinimum(ISPC::ControlLBC::LBC_LIGHT_LEVEL_S.min);
	lightLevel_spin->setMaximum(ISPC::ControlLBC::LBC_LIGHT_LEVEL_S.max);
	lightLevel_spin->setValue(ISPC::ControlLBC::LBC_LIGHT_LEVEL_S.def);

	sharpnessDoubleBoxSlider->setMinimum(ISPC::ControlLBC::LBC_SHARPNESS_S.min);
	sharpnessDoubleBoxSlider->setMaximum(ISPC::ControlLBC::LBC_SHARPNESS_S.max);
	sharpnessDoubleBoxSlider->setValue(ISPC::ControlLBC::LBC_SHARPNESS_S.def);

	brightnessDoubleBoxSlider->setMinimum(ISPC::ControlLBC::LBC_BRIGHTNESS_S.min);
	brightnessDoubleBoxSlider->setMaximum(ISPC::ControlLBC::LBC_BRIGHTNESS_S.max);
	brightnessDoubleBoxSlider->setValue(ISPC::ControlLBC::LBC_BRIGHTNESS_S.def);

	contrastDoubleBoxSlider->setMinimum(ISPC::ControlLBC::LBC_CONTRAST_S.min);
	contrastDoubleBoxSlider->setMaximum(ISPC::ControlLBC::LBC_CONTRAST_S.max);
	contrastDoubleBoxSlider->setValue(ISPC::ControlLBC::LBC_CONTRAST_S.def);

	saturationDoubleBoxSlider->setMinimum(ISPC::ControlLBC::LBC_SATURATION_S.min);
	saturationDoubleBoxSlider->setMaximum(ISPC::ControlLBC::LBC_SATURATION_S.max);
	saturationDoubleBoxSlider->setValue(ISPC::ControlLBC::LBC_SATURATION_S.def);

	connect(remove_btn, SIGNAL(clicked()), this, SLOT(reqRemoveSlot()));
}

LBCConfig::LBCConfig(QWidget *parent): QDialog(parent)
{
	init();
}

void LBCConfig::reqRemoveSlot()
{
	emit removeSig(this);
}
