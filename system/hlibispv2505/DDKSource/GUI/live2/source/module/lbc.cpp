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
	switch(_css)
	{
	case CUSTOM_CSS:
		updateSpeed_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);

		for(unsigned int i = 0; i < _LBCConfigs.size(); i++)
		{
			_LBCConfigs[i]->lightLevel_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_LBCConfigs[i]->brightness_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_LBCConfigs[i]->contrast_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_LBCConfigs[i]->saturation_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_LBCConfigs[i]->sharpness_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		}
		break;

	case DEFAULT_CSS:
		updateSpeed_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);

		for(unsigned int i = 0; i < _LBCConfigs.size(); i++)
		{
			_LBCConfigs[i]->lightLevel_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_LBCConfigs[i]->brightness_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_LBCConfigs[i]->contrast_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_LBCConfigs[i]->saturation_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_LBCConfigs[i]->sharpness_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
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

	updateSpeed_sb->setMinimum(lbc.LBC_UPDATE_SPEED.min);
	measuredLightLevel_sb->setMinimum(lbc.LBC_LIGHT_LEVEL_S.min);
	lightLevel_sb->setMinimum(lbc.LBC_LIGHT_LEVEL_S.min);
	brightness_sb->setMinimum(lbc.LBC_BRIGHTNESS_S.min);
	contrast_sb->setMinimum(lbc.LBC_CONTRAST_S.min);
	saturation_sb->setMinimum(lbc.LBC_SATURATION_S.min);
	sharpness_sb->setMinimum(lbc.LBC_SHARPNESS_S.min);

	//
	// Set maximum
	//

	updateSpeed_sb->setMaximum(lbc.LBC_UPDATE_SPEED.max);
	measuredLightLevel_sb->setMaximum(lbc.LBC_LIGHT_LEVEL_S.max);
	lightLevel_sb->setMaximum(lbc.LBC_LIGHT_LEVEL_S.max);
	brightness_sb->setMaximum(lbc.LBC_BRIGHTNESS_S.max);
	contrast_sb->setMaximum(lbc.LBC_CONTRAST_S.max);
	saturation_sb->setMaximum(lbc.LBC_SATURATION_S.max);
	sharpness_sb->setMaximum(lbc.LBC_SHARPNESS_S.max);
	
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
		_LBCConfigs[i]->lightLevel_sb->setValue(corr.lightLevel);
		_LBCConfigs[i]->brightness_sb->setValue(corr.brightness);
		_LBCConfigs[i]->contrast_sb->setValue(corr.contrast);
		_LBCConfigs[i]->saturation_sb->setValue(corr.saturation);
		_LBCConfigs[i]->sharpness_sb->setValue(corr.sharpness);
	}
	updateSpeed_sb->setValue(lbc.getUpdateSpeed());
}

void VisionLive::ModuleLBC::save(ISPC::ParameterList &config)
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

		lbcConfig.lightLevel = _LBCConfigs[lbcN]->lightLevel_sb->value();
		lbcConfig.brightness = _LBCConfigs[lbcN]->brightness_sb->value();
		lbcConfig.contrast = _LBCConfigs[lbcN]->contrast_sb->value();
		lbcConfig.saturation = _LBCConfigs[lbcN]->saturation_sb->value();
		lbcConfig.sharpness = _LBCConfigs[lbcN]->sharpness_sb->value();

		lbc.addConfiguration(lbcConfig);
	}
	lbc.setUpdateSpeed(updateSpeed_sb->value());

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
		_pProxyHandler->LBC_set(auto_cb->isChecked());
	}
}

//
// Protected Functions
//

void VisionLive::ModuleLBC::initModule()
{
	_name = windowTitle();

	configurations_sa->setAlignment(Qt::AlignTop);

	_pSpacer = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
	configurationsLayout->addItem(_pSpacer, 0, 0);

	QObject::connect(auto_cb, SIGNAL(stateChanged(int)), this, SLOT(LBC_set(int)));
	QObject::connect(addConfiguration_btn, SIGNAL(clicked()), this, SLOT(addEntry()));
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

	int r = configurationsLayout->rowCount() - 1;
	configurationsLayout->removeItem(_pSpacer);
	configurationsLayout->addWidget(_LBCConfigs[_LBCConfigs.size() - 1], r, 0);
	configurationsLayout->addItem(_pSpacer, r + 1, 0);

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

			configurationsLayout->removeWidget(config);

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
	lightLevel_sb->setValue(params.find("LightLevel").value().toDouble());
	brightness_sb->setValue(params.find("Brightness").value().toDouble());
	contrast_sb->setValue(params.find("Contrast").value().toDouble());
	saturation_sb->setValue(params.find("Saturation").value().toDouble());
	sharpness_sb->setValue(params.find("Sharpness").value().toDouble());
	measuredLightLevel_sb->setValue(params.find("MeasuredLightLevel").value().toDouble());
}

void VisionLive::ModuleLBC::LBC_set(int state)
{
	if(_pProxyHandler)
	{
		_pProxyHandler->LBC_set(state);
	}
}
