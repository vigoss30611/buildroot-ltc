#include "lbcconfig.hpp"

#include <ispc/ControlLBC.h>

VisionLive::LBCConfig::LBCConfig(QWidget *parent): QWidget(parent)
{
	Ui::LBCConfig::setupUi(this);

	lightLevel_sb->setMinimum(ISPC::ControlLBC::LBC_LIGHT_LEVEL_S.min);
	lightLevel_sb->setMaximum(ISPC::ControlLBC::LBC_LIGHT_LEVEL_S.max);
	lightLevel_sb->setValue(ISPC::ControlLBC::LBC_LIGHT_LEVEL_S.def);
	
	brightness_sb->setMinimum(ISPC::ControlLBC::LBC_BRIGHTNESS_S.min);
	brightness_sb->setMaximum(ISPC::ControlLBC::LBC_BRIGHTNESS_S.max);
	brightness_sb->setValue(ISPC::ControlLBC::LBC_BRIGHTNESS_S.def);

	contrast_sb->setMinimum(ISPC::ControlLBC::LBC_CONTRAST_S.min);
	contrast_sb->setMaximum(ISPC::ControlLBC::LBC_CONTRAST_S.max);
	contrast_sb->setValue(ISPC::ControlLBC::LBC_CONTRAST_S.def);

	saturation_sb->setMinimum(ISPC::ControlLBC::LBC_SATURATION_S.min);
	saturation_sb->setMaximum(ISPC::ControlLBC::LBC_SATURATION_S.max);
	saturation_sb->setValue(ISPC::ControlLBC::LBC_SATURATION_S.def);
	
	sharpness_sb->setMinimum(ISPC::ControlLBC::LBC_SHARPNESS_S.min);
	sharpness_sb->setMaximum(ISPC::ControlLBC::LBC_SHARPNESS_S.max);
	sharpness_sb->setValue(ISPC::ControlLBC::LBC_SHARPNESS_S.def);

	QObject::connect(remove_btn, SIGNAL(clicked()), this, SLOT(remove()));
}

VisionLive::LBCConfig::~LBCConfig()
{
}

void VisionLive::LBCConfig::remove()
{
	emit remove(this);
}
