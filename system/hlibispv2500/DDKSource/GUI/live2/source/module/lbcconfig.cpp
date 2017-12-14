#include "lbcconfig.hpp"

#include <ispc/ControlLBC.h>

VisionLive::LBCConfig::LBCConfig(QWidget *parent): QWidget(parent)
{
	Ui::LBCConfig::setupUi(this);

	LBCConfig_lightLevel_sb->setMinimum(ISPC::ControlLBC::LBC_LIGHT_LEVEL_S.min);
	LBCConfig_lightLevel_sb->setMaximum(ISPC::ControlLBC::LBC_LIGHT_LEVEL_S.max);
	LBCConfig_lightLevel_sb->setValue(ISPC::ControlLBC::LBC_LIGHT_LEVEL_S.def);
	
	LBCConfig_brightness_sb->setMinimum(ISPC::ControlLBC::LBC_BRIGHTNESS_S.min);
	LBCConfig_brightness_sb->setMaximum(ISPC::ControlLBC::LBC_BRIGHTNESS_S.max);
	LBCConfig_brightness_sb->setValue(ISPC::ControlLBC::LBC_BRIGHTNESS_S.def);

	LBCConfig_contrast_cb->setMinimum(ISPC::ControlLBC::LBC_CONTRAST_S.min);
	LBCConfig_contrast_cb->setMaximum(ISPC::ControlLBC::LBC_CONTRAST_S.max);
	LBCConfig_contrast_cb->setValue(ISPC::ControlLBC::LBC_CONTRAST_S.def);

	LBCConfig_saturation_sb->setMinimum(ISPC::ControlLBC::LBC_SATURATION_S.min);
	LBCConfig_saturation_sb->setMaximum(ISPC::ControlLBC::LBC_SATURATION_S.max);
	LBCConfig_saturation_sb->setValue(ISPC::ControlLBC::LBC_SATURATION_S.def);
	
	LBCConfig_sharpness_sb->setMinimum(ISPC::ControlLBC::LBC_SHARPNESS_S.min);
	LBCConfig_sharpness_sb->setMaximum(ISPC::ControlLBC::LBC_SHARPNESS_S.max);
	LBCConfig_sharpness_sb->setValue(ISPC::ControlLBC::LBC_SHARPNESS_S.def);

	QObject::connect(LBCConfig_remove_btn, SIGNAL(clicked()), this, SLOT(remove()));
}

VisionLive::LBCConfig::~LBCConfig()
{
}

void VisionLive::LBCConfig::remove()
{
	emit remove(this);
}
