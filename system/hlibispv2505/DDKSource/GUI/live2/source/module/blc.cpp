#include "blc.hpp"

#include "ispc/ModuleBLC.h"

//
// Public Functions
//

VisionLive::ModuleBLC::ModuleBLC(QWidget *parent): ModuleBase(parent)
{
	Ui::BLC::setupUi(this);

	retranslate();

	initModule();

	initObjectsConnections();
}

VisionLive::ModuleBLC::~ModuleBLC()
{
}

void VisionLive::ModuleBLC::removeObjectValueChangedMark()
{
	switch(_css)
	{
	case CUSTOM_CSS:
		sensorBlackLevel0_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		sensorBlackLevel1_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		sensorBlackLevel2_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		sensorBlackLevel3_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);

		systemBlackLevel_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		break;

	case DEFAULT_CSS:
		sensorBlackLevel0_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		sensorBlackLevel1_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		sensorBlackLevel2_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		sensorBlackLevel3_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);

		systemBlackLevel_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		break;
	}
}

void VisionLive::ModuleBLC::retranslate()
{
	Ui::BLC::retranslateUi(this);
}

void VisionLive::ModuleBLC::load(const ISPC::ParameterList &config)
{
	ISPC::ModuleBLC blc;
	blc.load(config);

	//
	// Set minimum
	//

	sensorBlackLevel0_sb->setMinimum(blc.BLC_SENSOR_BLACK.min);
	sensorBlackLevel1_sb->setMinimum(blc.BLC_SENSOR_BLACK.min);
	sensorBlackLevel2_sb->setMinimum(blc.BLC_SENSOR_BLACK.min);
	sensorBlackLevel3_sb->setMinimum(blc.BLC_SENSOR_BLACK.min);

	systemBlackLevel_sb->setMinimum(blc.BLC_SYS_BLACK.min);

	//
	// Set maximum
	//

	sensorBlackLevel0_sb->setMaximum(blc.BLC_SENSOR_BLACK.max);
	sensorBlackLevel1_sb->setMaximum(blc.BLC_SENSOR_BLACK.max);
	sensorBlackLevel2_sb->setMaximum(blc.BLC_SENSOR_BLACK.max);
	sensorBlackLevel3_sb->setMaximum(blc.BLC_SENSOR_BLACK.max);

	systemBlackLevel_sb->setMaximum(blc.BLC_SYS_BLACK.max);
	
	//
	// Set precision
	//

	//
	// Set value
	//

	sensorBlackLevel0_sb->setValue(blc.aSensorBlack[0]);
	sensorBlackLevel1_sb->setValue(blc.aSensorBlack[1]);
	sensorBlackLevel2_sb->setValue(blc.aSensorBlack[2]);
	sensorBlackLevel3_sb->setValue(blc.aSensorBlack[3]);

	systemBlackLevel_sb->setValue(blc.ui32SystemBlack);
}

void VisionLive::ModuleBLC::save(ISPC::ParameterList &config)
{
	ISPC::ModuleBLC blc;
	blc.load(config);

	blc.aSensorBlack[0] = sensorBlackLevel0_sb->value();
	blc.aSensorBlack[1] = sensorBlackLevel1_sb->value();
	blc.aSensorBlack[2] = sensorBlackLevel2_sb->value();
	blc.aSensorBlack[3] = sensorBlackLevel3_sb->value();

    blc.ui32SystemBlack = systemBlackLevel_sb->value();

	blc.save(config, ISPC::ModuleBLC::SAVE_VAL);
}

//
// Protected Functions
//

void VisionLive::ModuleBLC::initModule()
{
	_name = windowTitle();

	QObject::connect(sensorBlackLevel0_sb, SIGNAL(valueChanged(int)), this, SLOT(moduleBLCChanged()));
	QObject::connect(sensorBlackLevel1_sb, SIGNAL(valueChanged(int)), this, SLOT(moduleBLCChanged()));
	QObject::connect(sensorBlackLevel2_sb, SIGNAL(valueChanged(int)), this, SLOT(moduleBLCChanged()));
	QObject::connect(sensorBlackLevel3_sb, SIGNAL(valueChanged(int)), this, SLOT(moduleBLCChanged()));
	QObject::connect(systemBlackLevel_sb, SIGNAL(valueChanged(int)), this, SLOT(moduleBLCChanged()));
}

//
// Protected Slots
//

void VisionLive::ModuleBLC::moduleBLCChanged()
{
	emit moduleBLCChanged(
		sensorBlackLevel0_sb->value(), 
		sensorBlackLevel1_sb->value(), 
		sensorBlackLevel2_sb->value(), 
		sensorBlackLevel3_sb->value(), 
		systemBlackLevel_sb->value());
}