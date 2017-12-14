#include "wbc.hpp"
#include "proxyhandler.hpp"
#include "objectregistry.hpp"

#include "ispc/ModuleWBC.h"
#include "ispc/ModuleCCM.h"
#include "ispc/TemperatureCorrection.h"

#include "ccm/tuning.hpp"

#include <qinputdialog.h>

#define AWB_MODE "AWB_MODE"

//
// Public Functions
//

VisionLive::ModuleWBC::ModuleWBC(QWidget *parent): ModuleBase(parent)
{
	Ui::WBC::setupUi(this);

	retranslate();

	_pGenConfig = NULL;
	_pCCMTuning = NULL;

	initModule();

	initObjectsConnections();
}

VisionLive::ModuleWBC::~ModuleWBC()
{
	if(_pGenConfig)
	{
		_pGenConfig->deleteLater();
	}

	if(_pCCMTuning)
	{
		_pCCMTuning->deleteLater();
	}
}

void VisionLive::ModuleWBC::removeObjectValueChangedMark()
{
	switch(css)
	{
	case CUSTOM_CSS:
		for(unsigned int i = 0; i < _CCMPaches.size(); i++)
		{
			_CCMPaches[i]->CCM_temp_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
			for(int j = 0; j < 4; j++)
			{
				_CCMPaches[i]->CCM_Gains[j]->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			}
			for(int j = 0; j < 4; j++)
			{
				_CCMPaches[i]->CCM_Clips[j]->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			}
			for(int j = 0; j < 9; j++)
			{
				_CCMPaches[i]->CCM_Matrix[j]->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			}
			for(int j = 0; j < 3; j++)
			{
				_CCMPaches[i]->CCM_Offsets[j]->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			}
		}
		break;

	case DEFAULT_CSS:
		for(unsigned int i = 0; i < _CCMPaches.size(); i++)
		{
			_CCMPaches[i]->CCM_temp_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
			for(int j = 0; j < 4; j++)
			{
				_CCMPaches[i]->CCM_Gains[j]->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			}
			for(int j = 0; j < 4; j++)
			{
				_CCMPaches[i]->CCM_Clips[j]->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			}
			for(int j = 0; j < 9; j++)
			{
				_CCMPaches[i]->CCM_Matrix[j]->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			}
			for(int j = 0; j < 3; j++)
			{
				_CCMPaches[i]->CCM_Offsets[j]->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			}
		}
		break;
	}
}

void VisionLive::ModuleWBC::retranslate()
{
	Ui::WBC::retranslateUi(this);
}

void VisionLive::ModuleWBC::load(const ISPC::ParameterList &config)
{
	ISPC::ModuleWBC wbc;
	ISPC::ModuleCCM ccm;
	ISPC::TemperatureCorrection tc;

	wbc.load(config);
	ccm.load(config);
	tc.loadParameters(config);

	// Remove all CCM Patches
	clearAll();	

	// Create ne CCM Patches
	addCCM("CCM", true);
	for(int c = 0; c < tc.size(); c++)
	{
		addCCM("CCM " + QString::number(c));
	}

	//
	// Set minimum
	//

	// CCM TAB
	for(int i = 0; i < 4; i++)
    {
		_CCMPaches[0]->CCM_Gains[i]->setMinimum(ISPC::ModuleWBC::WBC_GAIN.min);
		_CCMPaches[0]->CCM_Clips[i]->setMinimum(ISPC::ModuleWBC::WBC_CLIP.min);
    }
    for(int i = 0; i < 9; i++)
	{
		_CCMPaches[0]->CCM_Matrix[i]->setMinimum(ISPC::ModuleCCM::CCM_MATRIX.min);
	}
    for(int i = 0; i < 3; i++)
	{
        _CCMPaches[0]->CCM_Offsets[i]->setMinimum(ISPC::ModuleCCM::CCM_OFFSETS.min);
	}

	// MULTI CCM TABS
	for(int c = 0; c < tc.size(); c++)
	{
		for(int i = 0; i < 4; i++)
		{
			_CCMPaches[c+1]->CCM_Gains[i]->setMinimum(ISPC::ModuleWBC::WBC_GAIN.min);
			_CCMPaches[c+1]->CCM_Clips[i]->setMinimum(ISPC::ModuleWBC::WBC_CLIP.min);
		}
		for(int i = 0; i < 9; i++)
		{
			_CCMPaches[c+1]->CCM_Matrix[i]->setMinimum(ISPC::ModuleCCM::CCM_MATRIX.min);
		}
		for(int i = 0; i < 3; i++)
		{
			_CCMPaches[c+1]->CCM_Offsets[i]->setMinimum(ISPC::ModuleCCM::CCM_OFFSETS.min);
		}
	}

	// AUTO CCM
    for(int i = 0; i < 4; i++)
    {
		AWB_Gains[i]->setMinimum(ISPC::ModuleWBC::WBC_GAIN.min);
		AWB_Clips[i]->setMinimum(ISPC::ModuleWBC::WBC_CLIP.min);
    }
    for(int i = 0; i < 9; i++)
	{
		AWB_Matrix[i]->setMinimum(ISPC::ModuleCCM::CCM_MATRIX.min);
	}
    for(int i = 0; i < 3; i++)
	{
		AWB_Offsets[i]->setMinimum(ISPC::ModuleCCM::CCM_OFFSETS.min);
	}
		
	//
	// Set maximum
	//

	// CCM TAB
	for(int i = 0; i < 4; i++)
    {
		_CCMPaches[0]->CCM_Gains[i]->setMaximum(ISPC::ModuleWBC::WBC_GAIN.max);
		_CCMPaches[0]->CCM_Clips[i]->setMaximum(ISPC::ModuleWBC::WBC_CLIP.max);
    }
    for(int i = 0; i < 9; i++)
	{
		_CCMPaches[0]->CCM_Matrix[i]->setMaximum(ISPC::ModuleCCM::CCM_MATRIX.max);
	}
    for(int i = 0; i < 3; i++)
	{
        _CCMPaches[0]->CCM_Offsets[i]->setMaximum(ISPC::ModuleCCM::CCM_OFFSETS.max);
	}

	// MULTI CCM TABS
	for(int c = 0; c < tc.size(); c++)
	{
		for(int i = 0; i < 4; i++)
		{
			_CCMPaches[c+1]->CCM_Gains[i]->setMaximum(ISPC::ModuleWBC::WBC_GAIN.max);
			_CCMPaches[c+1]->CCM_Clips[i]->setMaximum(ISPC::ModuleWBC::WBC_CLIP.max);
		}
		for(int i = 0; i < 9; i++)
		{
			_CCMPaches[c+1]->CCM_Matrix[i]->setMaximum(ISPC::ModuleCCM::CCM_MATRIX.max);
		}
		for(int i = 0; i < 3; i++)
		{
			_CCMPaches[c+1]->CCM_Offsets[i]->setMaximum(ISPC::ModuleCCM::CCM_OFFSETS.max);
		}
	}

	// AUTO CCM
    for(int i = 0; i < 4; i++)
    {
		AWB_Gains[i]->setMaximum(ISPC::ModuleWBC::WBC_GAIN.max);
		AWB_Clips[i]->setMaximum(ISPC::ModuleWBC::WBC_CLIP.max);
    }
    for(int i = 0; i < 9; i++)
	{
		AWB_Matrix[i]->setMaximum(ISPC::ModuleCCM::CCM_MATRIX.max);
	}
    for(int i = 0; i < 3; i++)
	{
		AWB_Offsets[i]->setMaximum(ISPC::ModuleCCM::CCM_OFFSETS.max);
	}
		
	//
	// Set precision
	//

	// CCM TAB
	for(int i = 0; i < 4; i++)
    {
		DEFINE_PREC(_CCMPaches[0]->CCM_Gains[i], 1024.0000f);
		DEFINE_PREC(_CCMPaches[0]->CCM_Clips[i], 4096.0000f);
    }
    for(int i = 0; i < 9; i++)
	{
		DEFINE_PREC(_CCMPaches[0]->CCM_Matrix[i], 512.000f);
	}
    for(int i = 0; i < 3; i++)
	{
        DEFINE_PREC(_CCMPaches[0]->CCM_Offsets[i], 1.0f);
	}

	// MULTI CCM TABS
	for(int c = 0; c < tc.size(); c++)
	{
		for(int i = 0; i < 4; i++)
		{
			DEFINE_PREC(_CCMPaches[c+1]->CCM_Gains[i], 1024.0000f);
			DEFINE_PREC(_CCMPaches[c+1]->CCM_Clips[i], 4096.0000f);
		}
		for(int i = 0; i < 9; i++)
		{
			DEFINE_PREC(_CCMPaches[c+1]->CCM_Matrix[i], 512.000f);
		}
		for(int i = 0; i < 3; i++)
		{
			DEFINE_PREC(_CCMPaches[c+1]->CCM_Offsets[i], 1.0f);
		}
	}

	// AUTO CCM
    for(int i = 0; i < 4; i++)
    {
		DEFINE_PREC(AWB_Gains[i], 1024.0000f);
        DEFINE_PREC(AWB_Clips[i], 4096.0000f);
    }
    for(int i = 0; i < 9; i++)
	{
		DEFINE_PREC(AWB_Matrix[i], 512.000f);
	}
    for(int i = 0; i < 3; i++)
	{
		DEFINE_PREC(AWB_Offsets[i], 1.0f);
	}

	//
	// Set value
	//

	for(unsigned int i = 0; i < ISPC::ModuleWBC::WBC_GAIN.n; i++)
	{
		_CCMPaches[0]->CCM_Gains[i]->setValue(wbc.aWBGain[i]);
		_CCMPaches[0]->CCM_Clips[i]->setValue(wbc.aWBClip[i]);
	}
	for(unsigned int i = 0; i < ISPC::ModuleCCM::CCM_MATRIX.n; i++)
	{
		_CCMPaches[0]->CCM_Matrix[i]->setValue(ccm.aMatrix[i]);
	}
	for(unsigned int i = 0; i < ISPC::ModuleCCM::CCM_OFFSETS.n; i++)
	{
		_CCMPaches[0]->CCM_Offsets[i]->setValue(ccm.aOffset[i]);
	}

	// MULTI CCM TABS
	for(int c = 0; c < tc.size(); c++)
	{
		ISPC::ColorCorrection cc = tc.getCorrection(c);

		_CCMPaches[c+1]->CCM_temp_sb->setValue(cc.temperature);

		for(int i = 0; i < 4; i++)
		{
			_CCMPaches[c+1]->CCM_Gains[i]->setValue(cc.gains[0][i]);
		}
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				_CCMPaches[c+1]->CCM_Matrix[i*3+j]->setValue(cc.coefficients[i][j]);
			}
		}
		for(int i = 0; i < 3; i++)
		{
			_CCMPaches[c+1]->CCM_Offsets[i]->setValue(cc.offsets[0][i]);
		}
	}

	if(config.exists(AWB_MODE))
	{
		AWB_mode_lb->setCurrentIndex(config.getParameter(AWB_MODE)->get<int>());
	}
}

void VisionLive::ModuleWBC::save(ISPC::ParameterList &config) const
{
	ISPC::ModuleWBC wbc;
	ISPC::ModuleCCM ccm;
	ISPC::TemperatureCorrection tc;

	wbc.load(config);
	ccm.load(config);
	tc.loadParameters(config);
	tc.clearCorrections();

	for(unsigned int i = 1; i < _CCMPaches.size(); i++)
	{
		config.removeParameter(tc.WB_CCM_S.name + "_" + std::string(QString::number(i).toLatin1()));
		config.removeParameter(tc.WB_GAINS_S.name + "_" + std::string(QString::number(i).toLatin1()));
		config.removeParameter(tc.WB_OFFSETS_S.name + "_" + std::string(QString::number(i).toLatin1()));
		config.removeParameter(tc.WB_TEMPERATURE_S.name + "_" + std::string(QString::number(i).toLatin1()));
	}

	// CCM
	for(unsigned int i = 0; i < ISPC::ModuleWBC::WBC_GAIN.n; i++)
		wbc.aWBGain[i] = _CCMPaches[0]->CCM_Gains[i]->value();
	for(unsigned int i = 0; i < ISPC::ModuleWBC::WBC_CLIP.n; i++)
		wbc.aWBClip[i] = _CCMPaches[0]->CCM_Clips[i]->value();
	for(unsigned int i = 0; i < ISPC::ModuleCCM::CCM_MATRIX.n; i++)
		ccm.aMatrix[i] = _CCMPaches[0]->CCM_Matrix[i]->value();
	for(unsigned int i = 0; i < ISPC::ModuleCCM::CCM_OFFSETS.n; i++)
		ccm.aOffset[i] = _CCMPaches[0]->CCM_Offsets[i]->value();

	// Multi CCM
	for(unsigned int c = 1; c < _CCMPaches.size(); c++)
	{
		if(_CCMPaches[c]->CCM_use_cb->isChecked())
		{
			ISPC::ColorCorrection cc;

			cc.temperature = _CCMPaches[c]->CCM_temp_sb->value();
			for(unsigned int i = 0; i < ISPC::ModuleWBC::WBC_GAIN.n; i++)
				cc.gains[0][i] = _CCMPaches[c]->CCM_Gains[i]->value();
			for(unsigned int i = 0; i < 3; i++)
			{
				for(unsigned int j = 0; j < 3; j++)
					cc.coefficients[i][j] = _CCMPaches[c]->CCM_Matrix[i*3+j]->value();
			}
			for(unsigned int i = 0; i < ISPC::ModuleCCM::CCM_OFFSETS.n; i++)
				cc.offsets[0][i] = _CCMPaches[c]->CCM_Offsets[i]->value();

			tc.addCorrection(cc);
		}
	}

	wbc.save(config, ISPC::ModuleWBC::SAVE_VAL);
	ccm.save(config, ISPC::ModuleCCM::SAVE_VAL);
	tc.saveParameters(config, ISPC::ModuleBase::SAVE_VAL);

	ISPC::Parameter awb_mode(AWB_MODE, std::string(QString::number(AWB_mode_lb->currentIndex()).toLatin1()));
	config.addParameter(awb_mode, true);
}

void VisionLive::ModuleWBC::setProxyHandler(ProxyHandler *proxy) 
{ 
	_pProxyHandler = proxy; 

	QObject::connect(_pProxyHandler, SIGNAL(AWB_received(QMap<QString, QString>)), this, SLOT(AWB_received(QMap<QString, QString>)));
} 

void VisionLive::ModuleWBC::resetConnectionDependantControls()
{
	if(_pProxyHandler)
	{
		_pProxyHandler->AWB_set(AWB_mode_lb->currentIndex());
	}
}

void VisionLive::ModuleWBC::setCSS(CSS newCSS) 
{ 
	css = newCSS; 

	switch(css)
	{
	case DEFAULT_CSS:
		if(_pCCMTuning)
		{
			_pCCMTuning->changeCSS(CSS_DEFAULT_STYLESHEET);
		}
		break;
	case CUSTOM_CSS:
		if(_pCCMTuning)
		{
			_pCCMTuning->changeCSS(CSS_CUSTOM_VT_STYLESHEET);
		}
		break;
	}
}

//
// Protected Functions
//

void VisionLive::ModuleWBC::initModule()
{
	_name = windowTitle();

	AWB_Gains[0] = AWB_gain0_sb;
	AWB_Gains[1] = AWB_gain1_sb;
	AWB_Gains[2] = AWB_gain2_sb;
	AWB_Gains[3] = AWB_gain3_sb;

	AWB_Clips[0] = AWB_clip0_sb;
	AWB_Clips[1] = AWB_clip1_sb;
	AWB_Clips[2] = AWB_clip2_sb;
	AWB_Clips[3] = AWB_clip3_sb;

	AWB_Matrix[0] = AWB_matrix0_sb;
	AWB_Matrix[1] = AWB_matrix1_sb;
	AWB_Matrix[2] = AWB_matrix2_sb;
	AWB_Matrix[3] = AWB_matrix3_sb;
	AWB_Matrix[4] = AWB_matrix4_sb;
	AWB_Matrix[5] = AWB_matrix5_sb;
	AWB_Matrix[6] = AWB_matrix6_sb;
	AWB_Matrix[7] = AWB_matrix7_sb;
	AWB_Matrix[8] = AWB_matrix8_sb;

	AWB_Offsets[0] = AWB_offsets0_sb;
	AWB_Offsets[1] = AWB_offsets1_sb;
	AWB_Offsets[2] = AWB_offsets2_sb;

	for(int i = 0; i < 4; i++)
	{
		AWB_Gains[i]->setReadOnly(true);
		AWB_Clips[i]->setReadOnly(true);
	}
	for(int i = 0; i < 9; i++)
	{
		AWB_Matrix[i]->setReadOnly(true);
	}
	for(int i = 0; i < 3; i++)
	{
		AWB_Offsets[i]->setReadOnly(true);
	}

	addCCM("CCM", true);

	QObject::connect(AWB_default_btn, SIGNAL(clicked()), this, SLOT(setDefault()));

	QObject::connect(AWB_mode_lb, SIGNAL(currentIndexChanged(int)), this, SLOT(AWB_changeMode(int)));

	QObject::connect(tune_btn, SIGNAL(clicked()), this, SLOT(tune()));
}

//
// Public Slots
//

void VisionLive::ModuleWBC::moduleBLCChanged(int sensorBlack0, int sensorBlack1, int sensorBlack2, int sensorBlack3, int systemBlack)
{
	_blcConfig.sensorBlack[0] = sensorBlack0;
	_blcConfig.sensorBlack[1] = sensorBlack1;
	_blcConfig.sensorBlack[2] = sensorBlack2;
	_blcConfig.sensorBlack[3] = sensorBlack3;

	_blcConfig.systemBlack = systemBlack;
}

//
// Protected Slots
//

void VisionLive::ModuleWBC::addCCM(const QString &name, bool isDefault)
{
	CCMPatch *newPatch = new CCMPatch(_CCMPaches.size()-1);
	initObjectsConnections(newPatch);

	if(isDefault)
	{
		newPatch->setObjectName("CCM");
		newPatch->label->hide();
		newPatch->CCM_temp_sb->hide();
		newPatch->CCM_default_btn->hide();
		newPatch->CCM_remove_btn->hide();
		newPatch->CCM_use_cb->hide();
	}
	else
	{
		newPatch->CCM_promote_btn->hide();
		newPatch->label_4->hide();
		for(int i = 0; i < 4; i++)
		{
			newPatch->CCM_Clips[i]->hide();
		}

		if(_pObjReg)
		{
			_pObjReg->registerChildrenObjects(newPatch, "Modules/WBC/", newPatch->objectName());
		}
		else
		{
			emit logError("Couldn't register CCM patch!", "ModuleWBC::addCCM()");
		}
	}

	_CCMPaches.push_back(newPatch);
	patchTabs_tw->addTab(newPatch, name);

	QObject::connect(newPatch, SIGNAL(requestPromote()), this, SLOT(promote()));
	QObject::connect(newPatch, SIGNAL(requestRemove(CCMPatch *)), this, SLOT(remove(CCMPatch *)));
	QObject::connect(newPatch, SIGNAL(requestDefault(CCMPatch *)), this, SLOT(setDefault(CCMPatch *)));
}

void VisionLive::ModuleWBC::remove(CCMPatch *ob)
{
	patchTabs_tw->removeTab(patchTabs_tw->indexOf(ob));
	
	for(unsigned int i = 0; i < _CCMPaches.size(); i++)
	{
		if(_CCMPaches[i] == ob)
		{
			// Deregister all multi CCM tabs
			if(_pObjReg)
			{
				for(unsigned int p = 1; p < _CCMPaches.size(); p++)
				{
					_pObjReg->deregisterObject(_CCMPaches[p]->objectName());
				}
			}

			_CCMPaches.erase(_CCMPaches.begin() + i);

			delete ob;

			for(unsigned int j = i; j < _CCMPaches.size(); j++)
			{
				patchTabs_tw->setTabText(j+1, "CCM " + QString::number(j-1));
			}

			// Reregister all multi CCM tabs
			if(_pObjReg)
			{
				for(unsigned int p = 1; p < _CCMPaches.size(); p++)
				{
					_CCMPaches[p]->setObjectName("CCM_" + QString::number(p-1));
					_pObjReg->registerChildrenObjects(_CCMPaches[p], "Modules/WBC/", _CCMPaches[p]->objectName());
				}
			}
			else
			{
				emit logError("Couldn't register CCM patch!", "ModuleWBC::remove()");
			}

			break;
		}
	}
}

void VisionLive::ModuleWBC::clearAll()
{
	for(int i = _CCMPaches.size() - 1; i > -1; i--)
	{
		remove(_CCMPaches[i]);
	}
}

void VisionLive::ModuleWBC::promote()
{
	int temp = 0;

	bool ok;
	temp = QInputDialog::getInt(this, tr("Set Temperature"), tr("Temperature:"), 0, 0, 100000, 1, &ok);
	if (ok)
	{
		addCCM("CCM " + QString::number(_CCMPaches.size() - 1));

		_CCMPaches[_CCMPaches.size() - 1]->CCM_temp_sb->setValue(temp);
		for(int i = 0; i < 4; i++)
		{
			_CCMPaches[_CCMPaches.size() - 1]->CCM_Gains[i]->setValue(_CCMPaches[0]->CCM_Gains[i]->value());
			_CCMPaches[_CCMPaches.size() - 1]->CCM_Clips[i]->setValue(_CCMPaches[0]->CCM_Clips[i]->value());
		}
		for(int i = 0; i < 9; i++)
		{
			_CCMPaches[_CCMPaches.size() - 1]->CCM_Matrix[i]->setValue(_CCMPaches[0]->CCM_Matrix[i]->value());
		}
		for(int i = 0; i < 3; i++)
		{
			_CCMPaches[_CCMPaches.size() - 1]->CCM_Offsets[i]->setValue(_CCMPaches[0]->CCM_Offsets[i]->value());
		}
	}
}

void VisionLive::ModuleWBC::setDefault(CCMPatch *ob)
{
	for(unsigned int index = 0; index < _CCMPaches.size(); index++)
	{
		if(_CCMPaches[index] == ob)
		{
			_CCMPaches[0]->CCM_temp_sb->setValue(_CCMPaches[index]->CCM_temp_sb->value());
			for(int i = 0; i < 4; i++)
			{
				_CCMPaches[0]->CCM_Gains[i]->setValue(_CCMPaches[index]->CCM_Gains[i]->value());
			}
			for(int i = 0; i < 9; i++)
			{
				_CCMPaches[0]->CCM_Matrix[i]->setValue(_CCMPaches[index]->CCM_Matrix[i]->value());
			}
			for(int i = 0; i < 3; i++)
			{
				_CCMPaches[0]->CCM_Offsets[i]->setValue(_CCMPaches[index]->CCM_Offsets[i]->value());
			}

			break;
		}
	}
}

void VisionLive::ModuleWBC::setDefault()
{
	for(int i = 0; i < 4; i++)
	{
		_CCMPaches[0]->CCM_Gains[i]->setValue(AWB_Gains[i]->value());
	}
	for(int i = 0; i < 9; i++)
	{
		_CCMPaches[0]->CCM_Matrix[i]->setValue(AWB_Matrix[i]->value());
	}
	for(int i = 0; i < 3; i++)
	{
		_CCMPaches[0]->CCM_Offsets[i]->setValue(AWB_Offsets[i]->value());
	}
}

void VisionLive::ModuleWBC::AWB_changeMode(int mode)
{
	if(_pProxyHandler)
	{
		_pProxyHandler->AWB_set(mode);
	}
}

void VisionLive::ModuleWBC::AWB_received(QMap<QString, QString> params)
{
	AWB_measuredTemp_sb->setValue(params.find("MeasuredTemperature").value().toDouble());

	QStringList gains = params.find("Gains").value().split(" ");
	QStringList clips = params.find("Clips").value().split(" ");
	QStringList matrix = params.find("Matrix").value().split(" ");
	QStringList offset = params.find("Offsets").value().split(" ");

	for(int i = 0; i < 4; i++)
	{
		AWB_Gains[i]->setValue(gains[i].toDouble());
		AWB_Clips[i]->setValue(clips[i].toDouble());
	}

	for(int i = 0; i < 9; i++)
	{
		AWB_Matrix[i]->setValue(matrix[i].toDouble());
	}

	for(int i = 0; i < 3; i++)
	{
		AWB_Offsets[i]->setValue(offset[i].toDouble());
	}
}

void VisionLive::ModuleWBC::tune()
{
	if(!_pGenConfig)
	{
		_pGenConfig = new GenConfig();
	}
	_pGenConfig->setBLC(_blcConfig);

	if(_pCCMTuning)
	{
		_pCCMTuning->deleteLater();
	}
	_pCCMTuning = new CCMTuning(_pGenConfig, true);
	setCSS(css);
	QObject::connect(_pCCMTuning->integrate_btn, SIGNAL(clicked()), this, SLOT(integrate()));
	_pCCMTuning->show();
}

void VisionLive::ModuleWBC::integrate()
{
	ISPC::ParameterList result = *_pCCMTuning->pResult;

	ISPC::ModuleWBC wbc;
	wbc.load(result);
	ISPC::ModuleCCM ccm;
	ccm.load(result);

	addCCM("CCM " + QString::number(_CCMPaches.size() - 1));

	_CCMPaches[_CCMPaches.size() - 1]->CCM_temp_sb->setValue(_pCCMTuning->temperature->value());
	for(unsigned int i = 0; i < ISPC::ModuleWBC::WBC_GAIN.n; i++)
	{
		_CCMPaches[_CCMPaches.size() - 1]->CCM_Gains[i]->setValue(wbc.aWBGain[i]);
		_CCMPaches[_CCMPaches.size() - 1]->CCM_Clips[i]->setValue(wbc.aWBClip[i]);
	}
	for(unsigned int i = 0; i < ISPC::ModuleCCM::CCM_MATRIX.n; i++)
	{
		_CCMPaches[_CCMPaches.size() - 1]->CCM_Matrix[i]->setValue(ccm.aMatrix[i]);
	}
	for(unsigned int i = 0; i < ISPC::ModuleCCM::CCM_OFFSETS.n; i++)
	{
		_CCMPaches[_CCMPaches.size() - 1]->CCM_Offsets[i]->setValue(ccm.aOffset[i]);
	}
}

