#include "lbc.hpp"
#include "lbcconfig.hpp"
#include "proxyhandler.hpp"
#include "objectregistry.hpp"

#include "ispc/ControlLBC.h"

//
// Public Functions
//

VisionLive::ModuleLBC::ModuleLBC(QWidget *parent): ModuleBase(parent)
{
	Ui::LBC::setupUi(this);

	retranslate();

	initModule();

	initObjectsConnections();
}

VisionLive::ModuleLBC::~ModuleLBC()
{
	for(unsigned int i = 0; i < _LBCConfigs.size(); i++)
	{
		if(_LBCConfigs[i])
		{
			_LBCConfigs[i]->deleteLater();
		}
	}
}

void VisionLive::ModuleLBC::removeObjectValueChangedMark()
{
	switch(css)
	{
	case CUSTOM_CSS:
		LBC_updateSpeed_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);

		for(unsigned int i = 0; i < _LBCConfigs.size(); i++)
		{
			_LBCConfigs[i]->LBCConfig_lightLevel_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_LBCConfigs[i]->LBCConfig_brightness_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_LBCConfigs[i]->LBCConfig_contrast_cb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_LBCConfigs[i]->LBCConfig_saturation_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_LBCConfigs[i]->LBCConfig_sharpness_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		}
		break;

	case DEFAULT_CSS:
		LBC_updateSpeed_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);

		for(unsigned int i = 0; i < _LBCConfigs.size(); i++)
		{
			_LBCConfigs[i]->LBCConfig_lightLevel_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_LBCConfigs[i]->LBCConfig_brightness_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_LBCConfigs[i]->LBCConfig_contrast_cb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_LBCConfigs[i]->LBCConfig_saturation_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_LBCConfigs[i]->LBCConfig_sharpness_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		}
		break;
	}
}

void VisionLive::ModuleLBC::retranslate()
{
	Ui::LBC::retranslateUi(this);
}

void VisionLive::ModuleLBC::load(const ISPC::ParameterList &config)
{
	ISPC::ControlLBC lbc;
	lbc.load(config);

	for(int i = _LBCConfigs.size() - 1; i > -1; i--)
	{
		removeEntry(_LBCConfigs[i]);
	}

	//
	// Set minimum
	//

	LBC_updateSpeed_sb->setMinimum(lbc.LBC_UPDATE_SPEED.min);
	LBC_measuredLightLevel_sb->setMinimum(lbc.LBC_LIGHT_LEVEL_S.min);
	LBC_lightLevel_sb->setMinimum(lbc.LBC_LIGHT_LEVEL_S.min);
	LBC_brightness_sb->setMinimum(lbc.LBC_BRIGHTNESS_S.min);
	LBC_contrast_cb->setMinimum(lbc.LBC_CONTRAST_S.min);
	LBC_saturation_sb->setMinimum(lbc.LBC_SATURATION_S.min);
	LBC_sharpness_sb->setMinimum(lbc.LBC_SHARPNESS_S.min);

	//
	// Set maximum
	//

	LBC_updateSpeed_sb->setMaximum(lbc.LBC_UPDATE_SPEED.max);
	LBC_measuredLightLevel_sb->setMaximum(lbc.LBC_LIGHT_LEVEL_S.max);
	LBC_lightLevel_sb->setMaximum(lbc.LBC_LIGHT_LEVEL_S.max);
	LBC_brightness_sb->setMaximum(lbc.LBC_BRIGHTNESS_S.max);
	LBC_contrast_cb->setMaximum(lbc.LBC_CONTRAST_S.max);
	LBC_saturation_sb->setMaximum(lbc.LBC_SATURATION_S.max);
	LBC_sharpness_sb->setMaximum(lbc.LBC_SHARPNESS_S.max);
	
	//
	// Set precision
	//

	//
	// Set value
	//

	for(int i = 0; i < lbc.getConfigurationsNumber(); i++)
	{
		addEntry();
        ISPC::LightCorrection corr = lbc.getCorrection(i);
		_LBCConfigs[i]->LBCConfig_lightLevel_sb->setValue(corr.lightLevel);
		_LBCConfigs[i]->LBCConfig_brightness_sb->setValue(corr.brightness);
		_LBCConfigs[i]->LBCConfig_contrast_cb->setValue(corr.contrast);
		_LBCConfigs[i]->LBCConfig_saturation_sb->setValue(corr.saturation);
		_LBCConfigs[i]->LBCConfig_sharpness_sb->setValue(corr.sharpness);
	}
	LBC_updateSpeed_sb->setValue(lbc.getUpdateSpeed());
}

void VisionLive::ModuleLBC::save(ISPC::ParameterList &config) const
{
	ISPC::ControlLBC lbc;
	lbc.load(config);

	for(int i = 0; i < lbc.getConfigurationsNumber(); i++)
	{
		config.removeParameter(lbc.LBC_LIGHT_LEVEL_S.name + "_" + std::string(QString::number(i).toLatin1()));
		config.removeParameter(lbc.LBC_BRIGHTNESS_S.name + "_" + std::string(QString::number(i).toLatin1()));
		config.removeParameter(lbc.LBC_CONTRAST_S.name + "_" + std::string(QString::number(i).toLatin1()));
		config.removeParameter(lbc.LBC_SATURATION_S.name + "_" + std::string(QString::number(i).toLatin1()));
		config.removeParameter(lbc.LBC_SHARPNESS_S.name + "_" + std::string(QString::number(i).toLatin1()));
	}

	lbc.clearConfigurations();

	for(unsigned int lbcN = 0; lbcN < _LBCConfigs.size(); lbcN++)
	{
		ISPC::LightCorrection lbcConfig;

		lbcConfig.lightLevel = _LBCConfigs[lbcN]->LBCConfig_lightLevel_sb->value();
		lbcConfig.brightness = _LBCConfigs[lbcN]->LBCConfig_brightness_sb->value();
		lbcConfig.contrast = _LBCConfigs[lbcN]->LBCConfig_contrast_cb->value();
		lbcConfig.saturation = _LBCConfigs[lbcN]->LBCConfig_saturation_sb->value();
		lbcConfig.sharpness = _LBCConfigs[lbcN]->LBCConfig_sharpness_sb->value();

		lbc.addConfiguration(lbcConfig);
	}
	lbc.setUpdateSpeed(LBC_updateSpeed_sb->value());

	ISPC::Parameter nConfigurations(lbc.LBC_CONFIGURATIONS.name, std::string(QString::number(_LBCConfigs.size()).toLatin1()));
	config.addParameter(nConfigurations);

	lbc.save(config, ISPC::ModuleBase::SAVE_VAL);
}

void VisionLive::ModuleLBC::setProxyHandler(ProxyHandler *proxy) 
{ 
	_pProxyHandler = proxy; 

	QObject::connect(_pProxyHandler, SIGNAL(LBC_received(QMap<QString, QString>)), this, SLOT(LBC_received(QMap<QString, QString>)));
} 

void VisionLive::ModuleLBC::resetConnectionDependantControls()
{
	if(_pProxyHandler)
	{
		_pProxyHandler->LBC_set(LBC_enable_cb->isChecked());
	}
}

//
// Protected Functions
//

void VisionLive::ModuleLBC::initModule()
{
	_name = windowTitle();

	LBC_configurations_sa->setAlignment(Qt::AlignTop);

	_pSpacer = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
	LBCConfigurationsLayout->addItem(_pSpacer, 0, 0);

	QObject::connect(LBC_enable_cb, SIGNAL(stateChanged(int)), this, SLOT(LBC_set(int)));
	QObject::connect(LBC_addConfiguration_btn, SIGNAL(clicked()), this, SLOT(addEntry()));
}

//
// Private Slots
//

void VisionLive::ModuleLBC::addEntry()
{
	emit logAction(tr("CALL LBC_addConfiguration()"));

	LBCConfig *config = new LBCConfig();
	config->setObjectName("LBC_" + QString::number(_LBCConfigs.size()));
	initObjectsConnections(config);
	_LBCConfigs.push_back(config);

	if(_pObjReg)
	{
		_pObjReg->registerChildrenObjects(config, "Modules/LBC/", config->objectName());
	}
	else
	{
		emit logError("Couldn't register LBC configuration!", "ModuleLBC::addEntry()");
	}

	int r = LBCConfigurationsLayout->rowCount() - 1;
	LBCConfigurationsLayout->removeItem(_pSpacer);
	LBCConfigurationsLayout->addWidget(_LBCConfigs[_LBCConfigs.size() - 1], r, 0);
	LBCConfigurationsLayout->addItem(_pSpacer, r + 1, 0);

	QObject::connect(_LBCConfigs[_LBCConfigs.size() - 1], SIGNAL(remove(LBCConfig*)), this, SLOT(removeEntry(LBCConfig*)));
}

void VisionLive::ModuleLBC::removeEntry(LBCConfig *config)
{
	emit logAction(tr("CALL LBC_removeConfiguration() ") + config->objectName());

	for(unsigned int i = 0; i < _LBCConfigs.size(); i++)
	{
		if(_LBCConfigs[i] == config)
		{
			// Deregister all multi LBC configurations
			if(_pObjReg)
			{
				for(unsigned int p = 0; p < _LBCConfigs.size(); p++)
				{
					_pObjReg->deregisterObject(_LBCConfigs[p]->objectName());
				}
			}

			LBCConfigurationsLayout->removeWidget(config);

			delete _LBCConfigs[i];
			
			_LBCConfigs.erase(_LBCConfigs.begin() + i);

			// Reregister all multi LBC configurations
			if(_pObjReg)
			{
				for(unsigned int p = 0; p < _LBCConfigs.size(); p++)
				{
					_LBCConfigs[p]->setObjectName("LBC_" + QString::number(p));
					_pObjReg->registerChildrenObjects(_LBCConfigs[p], "Modules/LBC/", _LBCConfigs[p]->objectName());
				}
			}
			else
			{
				emit logError("Couldn't register LBC configuration!", "ModuleLBC::removeEntry()");
			}
		}
	}
}

void VisionLive::ModuleLBC::LBC_received(QMap<QString, QString> params)
{
	LBC_lightLevel_sb->setValue(params.find("LightLevel").value().toDouble());
	LBC_brightness_sb->setValue(params.find("Brightness").value().toDouble());
	LBC_contrast_cb->setValue(params.find("Contrast").value().toDouble());
	LBC_saturation_sb->setValue(params.find("Saturation").value().toDouble());
	LBC_sharpness_sb->setValue(params.find("Sharpness").value().toDouble());
	LBC_measuredLightLevel_sb->setValue(params.find("MeasuredLightLevel").value().toDouble());
}

void VisionLive::ModuleLBC::LBC_set(int state)
{
	if(_pProxyHandler)
	{
		_pProxyHandler->LBC_set(state);
	}
}
