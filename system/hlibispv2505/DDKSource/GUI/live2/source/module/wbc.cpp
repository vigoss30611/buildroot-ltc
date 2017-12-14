#include "wbc.hpp"
#include "proxyhandler.hpp"
#include "objectregistry.hpp"
#include "doubleslider.hpp"

#include "ispc/ModuleWBC.h"
#include "ispc/ModuleCCM.h"
#include "ispc/ModuleAWS.h"
#include "ispc/TemperatureCorrection.h"
#include "ispc/ControlAWB_Planckian.h"

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
    _computeWarning = NULL;

	initModule();

    initObjectsConnections(AWB_aws_gb);
    initObjectsConnections(AWB_ts_gb);
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
	switch(_css)
	{
	case CUSTOM_CSS:
		for(unsigned int i = 0; i < _CCMPaches.size(); i++)
		{
			_CCMPaches[i]->CCM_use_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
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

        AWB_AWS_enable_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
        AWB_AWS_debug_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
        AWB_AWS_BBDist_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
        AWB_AWS_RDarkThresh_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
        AWB_AWS_GDarkThresh_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
        AWB_AWS_BDarkThresh_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
        AWB_AWS_RClipThresh_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
        AWB_AWS_GClipThresh_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
        AWB_AWS_BClipThresh_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
        AWB_AWS_TileSizeWidth_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
        AWB_AWS_TileSizeStartCoordCol_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
        AWB_AWS_TileSizeHeight_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
        AWB_AWS_TileSizeStartCoordRow_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);

        AWB_TS_useSmoothing_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
        AWB_TS_useFlashFiltering_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
        AWB_TS_temporalStretch_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
        AWB_TS_weightBase_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		break;

	case DEFAULT_CSS:
		for(unsigned int i = 0; i < _CCMPaches.size(); i++)
		{
			_CCMPaches[i]->CCM_use_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
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

        AWB_AWS_enable_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
        AWB_AWS_debug_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
        AWB_AWS_BBDist_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
        AWB_AWS_RDarkThresh_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
        AWB_AWS_GDarkThresh_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
        AWB_AWS_BDarkThresh_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
        AWB_AWS_RClipThresh_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
        AWB_AWS_GClipThresh_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
        AWB_AWS_BClipThresh_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
        AWB_AWS_TileSizeWidth_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
        AWB_AWS_TileSizeStartCoordCol_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
        AWB_AWS_TileSizeHeight_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
        AWB_AWS_TileSizeStartCoordRow_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);

        AWB_TS_useSmoothing_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
        AWB_TS_useFlashFiltering_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
        AWB_TS_temporalStretch_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
        AWB_TS_weightBase_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
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
	ISPC::ModuleAWS aws;
	ISPC::TemperatureCorrection tc;
	ISPC::ControlAWB_Planckian awb;

	wbc.load(config);
	ccm.load(config);
	aws.load(config);
	tc.loadParameters(config);
	awb.load(config);

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

	// AWS
	AWB_AWS_BBDist_sl->setMinimumD(ISPC::ModuleAWS::AWS_BB_DIST.min);
	AWB_AWS_RDarkThresh_sl->setMinimumD(ISPC::ModuleAWS::AWS_R_DARK_THRESH.min);
	AWB_AWS_GDarkThresh_sl->setMinimumD(ISPC::ModuleAWS::AWS_R_DARK_THRESH.min);
	AWB_AWS_BDarkThresh_sl->setMinimumD(ISPC::ModuleAWS::AWS_R_DARK_THRESH.min);
	AWB_AWS_RClipThresh_sl->setMinimumD(ISPC::ModuleAWS::AWS_R_CLIP_THRESH.min);
	AWB_AWS_GClipThresh_sl->setMinimumD(ISPC::ModuleAWS::AWS_R_CLIP_THRESH.min);
	AWB_AWS_BClipThresh_sl->setMinimumD(ISPC::ModuleAWS::AWS_R_CLIP_THRESH.min);

	AWB_AWS_BBDist_sb->setMinimum(ISPC::ModuleAWS::AWS_BB_DIST.min);
	AWB_AWS_RDarkThresh_sb->setMinimum(ISPC::ModuleAWS::AWS_R_DARK_THRESH.min);
	AWB_AWS_GDarkThresh_sb->setMinimum(ISPC::ModuleAWS::AWS_G_DARK_THRESH.min);
	AWB_AWS_BDarkThresh_sb->setMinimum(ISPC::ModuleAWS::AWS_B_DARK_THRESH.min);
	AWB_AWS_RClipThresh_sb->setMinimum(ISPC::ModuleAWS::AWS_R_CLIP_THRESH.min);
	AWB_AWS_GClipThresh_sb->setMinimum(ISPC::ModuleAWS::AWS_G_CLIP_THRESH.min);
	AWB_AWS_BClipThresh_sb->setMinimum(ISPC::ModuleAWS::AWS_B_CLIP_THRESH.min);

    AWB_AWS_TileSizeWidth_sb->setMinimum(ISPC::ModuleAWS::AWS_TILE_SIZE.indexed(0).min);
    AWB_AWS_TileSizeHeight_sb->setMinimum(ISPC::ModuleAWS::AWS_TILE_SIZE.indexed(1).min);
    AWB_AWS_TileSizeStartCoordCol_sb->setMinimum(ISPC::ModuleAWS::AWS_TILE_START_COORDS.min);
    AWB_AWS_TileSizeStartCoordRow_sb->setMinimum(ISPC::ModuleAWS::AWS_TILE_START_COORDS.min);

	// AWB
	AWB_TS_temporalStretch_sb->setMinimum(ISPC::ControlAWB_Planckian::WBTS_TEMPORAL_STRETCH.min);
	AWB_TS_weightBase_sb->setMinimum(ISPC::ControlAWB_Planckian::WBTS_WEIGHT_BASE.min);
		
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

	// AWS
	AWB_AWS_BBDist_sl->setMaximumD(ISPC::ModuleAWS::AWS_BB_DIST.max);
	AWB_AWS_RDarkThresh_sl->setMaximumD(ISPC::ModuleAWS::AWS_R_DARK_THRESH.max);
	AWB_AWS_GDarkThresh_sl->setMaximumD(ISPC::ModuleAWS::AWS_R_DARK_THRESH.max);
	AWB_AWS_BDarkThresh_sl->setMaximumD(ISPC::ModuleAWS::AWS_R_DARK_THRESH.max);
	AWB_AWS_RClipThresh_sl->setMaximumD(ISPC::ModuleAWS::AWS_R_CLIP_THRESH.max);
	AWB_AWS_GClipThresh_sl->setMaximumD(ISPC::ModuleAWS::AWS_R_CLIP_THRESH.max);
	AWB_AWS_BClipThresh_sl->setMaximumD(ISPC::ModuleAWS::AWS_R_CLIP_THRESH.max);

	AWB_AWS_BBDist_sb->setMaximum(ISPC::ModuleAWS::AWS_BB_DIST.max);
	AWB_AWS_RDarkThresh_sb->setMaximum(ISPC::ModuleAWS::AWS_R_DARK_THRESH.max);
	AWB_AWS_GDarkThresh_sb->setMaximum(ISPC::ModuleAWS::AWS_G_DARK_THRESH.max);
	AWB_AWS_BDarkThresh_sb->setMaximum(ISPC::ModuleAWS::AWS_B_DARK_THRESH.max);
	AWB_AWS_RClipThresh_sb->setMaximum(ISPC::ModuleAWS::AWS_R_CLIP_THRESH.max);
	AWB_AWS_GClipThresh_sb->setMaximum(ISPC::ModuleAWS::AWS_G_CLIP_THRESH.max);
	AWB_AWS_BClipThresh_sb->setMaximum(ISPC::ModuleAWS::AWS_B_CLIP_THRESH.max);

    AWB_AWS_TileSizeWidth_sb->setMaximum(ISPC::ModuleAWS::AWS_TILE_SIZE.max);
    AWB_AWS_TileSizeHeight_sb->setMaximum(ISPC::ModuleAWS::AWS_TILE_SIZE.max);
    AWB_AWS_TileSizeStartCoordCol_sb->setMaximum(ISPC::ModuleAWS::AWS_TILE_START_COORDS.max);
    AWB_AWS_TileSizeStartCoordRow_sb->setMaximum(ISPC::ModuleAWS::AWS_TILE_START_COORDS.max);

	// AWB
	AWB_TS_temporalStretch_sb->setMaximum(ISPC::ControlAWB_Planckian::WBTS_TEMPORAL_STRETCH.max);
	AWB_TS_weightBase_sb->setMaximum(ISPC::ControlAWB_Planckian::WBTS_WEIGHT_BASE.max);
		
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

	// AWS
	DEFINE_PREC(AWB_AWS_BBDist_sb, 1024.0000f);
	DEFINE_PREC(AWB_AWS_RDarkThresh_sb, 1024.0000f);
	DEFINE_PREC(AWB_AWS_GDarkThresh_sb, 1024.0000f);
	DEFINE_PREC(AWB_AWS_BDarkThresh_sb, 1024.0000f);
	DEFINE_PREC(AWB_AWS_RClipThresh_sb, 1024.0000f);
	DEFINE_PREC(AWB_AWS_GClipThresh_sb, 1024.0000f);
	DEFINE_PREC(AWB_AWS_BClipThresh_sb, 1024.0000f);

	// AWB
	DEFINE_PREC(AWB_TS_weightBase_sb, 1024.0000f);

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

	// AWS
	AWB_AWS_enable_cb->setChecked(config.getParameter<bool>(aws.AWS_ENABLED));
	AWB_AWS_debug_cb->setChecked(config.getParameter<bool>(aws.AWS_DEBUG_MODE));
	AWB_AWS_BBDist_sb->setValue(aws.fBbDist);
	AWB_AWS_RDarkThresh_sb->setValue(aws.fRedDarkThresh);
	AWB_AWS_GDarkThresh_sb->setValue(aws.fGreenDarkThresh);
	AWB_AWS_BDarkThresh_sb->setValue(aws.fBlueDarkThresh);
	AWB_AWS_RClipThresh_sb->setValue(aws.fRedClipThresh);
	AWB_AWS_GClipThresh_sb->setValue(aws.fGreenClipThresh);
	AWB_AWS_BClipThresh_sb->setValue(aws.fBlueClipThresh);
    AWB_AWS_TileSizeWidth_sb->setValue(aws.ui16TileWidth);
    AWB_AWS_TileSizeHeight_sb->setValue(aws.ui16TileHeight);
    AWB_AWS_TileSizeStartCoordCol_sb->setValue(aws.ui16GridStartColumn);
    AWB_AWS_TileSizeStartCoordRow_sb->setValue(aws.ui16GridStartRow);

	// AWB
	AWB_TS_temporalStretch_sb->setValue(awb.ts_getTemporalStretch());
	AWB_TS_weightBase_sb->setValue(awb.ts_getWeightBase());
	AWB_TS_useSmoothing_cb->setChecked(awb.ts_getSmoothing());
	AWB_TS_useFlashFiltering_cb->setChecked(awb.ts_getFlashFiltering());
}

void VisionLive::ModuleWBC::save(ISPC::ParameterList &config)
{
	ISPC::ModuleWBC wbc;
	ISPC::ModuleCCM ccm;
	ISPC::ModuleAWS aws;
	ISPC::ControlAWB_Planckian awb;
    ISPC::TemperatureCorrection& tc = awb.getTemperatureCorrectionsMutable();
	wbc.load(config);
	ccm.load(config);
	aws.load(config);
	awb.load(config);

	tc.clearCorrections();

    int corrections =
        config.getParameter(ISPC::TemperatureCorrection::WB_CORRECTIONS);

    for(int i = 1; i < corrections; i++)
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

	// AWS
	aws.enabled = AWB_AWS_enable_cb->isChecked();
	aws.debugMode = AWB_AWS_debug_cb->isChecked();
	aws.fBbDist = AWB_AWS_BBDist_sb->value();
	aws.fRedDarkThresh = AWB_AWS_RDarkThresh_sb->value();
	aws.fGreenDarkThresh = AWB_AWS_GDarkThresh_sb->value();
	aws.fBlueDarkThresh = AWB_AWS_BDarkThresh_sb->value();
	aws.fRedClipThresh = AWB_AWS_RClipThresh_sb->value();
	aws.fGreenClipThresh = AWB_AWS_GClipThresh_sb->value();
	aws.fBlueClipThresh = AWB_AWS_BClipThresh_sb->value();
    aws.ui16TileWidth = AWB_AWS_TileSizeWidth_sb->value();
    aws.ui16TileHeight = AWB_AWS_TileSizeHeight_sb->value();
    aws.ui16GridStartColumn = AWB_AWS_TileSizeStartCoordCol_sb->value();
    aws.ui16GridStartRow = AWB_AWS_TileSizeStartCoordRow_sb->value();

	// AWB
	awb.setFlashFiltering(AWB_TS_useFlashFiltering_cb->isChecked());
	awb.ts_setSmoothing(AWB_TS_useSmoothing_cb->isChecked());
	awb.ts_setWeightBase(AWB_TS_weightBase_sb->value());
	awb.ts_setTemporalStretch(AWB_TS_temporalStretch_sb->value());


	wbc.save(config, ISPC::ModuleWBC::SAVE_VAL);
	ccm.save(config, ISPC::ModuleCCM::SAVE_VAL);
	aws.save(config, ISPC::ModuleCCM::SAVE_VAL);
	awb.save(config, ISPC::ModuleCCM::SAVE_VAL);

	ISPC::Parameter awb_mode(AWB_MODE, std::string(QString::number(AWB_mode_lb->currentIndex()).toLatin1()));
	config.addParameter(awb_mode, true);

    // generate AWB planckian approximation here
    locusCompute.setParams(config);
    locusCompute.compute(); // blocking call
    // Commenting out cause it distubs tests
    /*if(locusCompute.hasFailed())
    {
        _computeWarning = new QMessageBox(QMessageBox::Information, tr("Warning"),
                                          QString::fromStdString(locusCompute.status()), QMessageBox::Ok, this);
        QObject::connect(_computeWarning, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(buttonClicked(QAbstractButton *)));
        _computeWarning->show();
    }*/
    config = locusCompute.getParams();
}

void VisionLive::ModuleWBC::buttonClicked(QAbstractButton *)
{
    if(_computeWarning)
    {
        _computeWarning->close();
        _computeWarning = NULL;
    }
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
	_css = newCSS; 

	switch(_css)
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

	QObject::connect(AWB_AWS_BBDist_sl, SIGNAL(valueChanged(double)), AWB_AWS_BBDist_sb, SLOT(setValue(double)));
	QObject::connect(AWB_AWS_RDarkThresh_sl, SIGNAL(valueChanged(double)), AWB_AWS_RDarkThresh_sb, SLOT(setValue(double)));
	QObject::connect(AWB_AWS_GDarkThresh_sl, SIGNAL(valueChanged(double)), AWB_AWS_GDarkThresh_sb, SLOT(setValue(double)));
	QObject::connect(AWB_AWS_BDarkThresh_sl, SIGNAL(valueChanged(double)), AWB_AWS_BDarkThresh_sb, SLOT(setValue(double)));
	QObject::connect(AWB_AWS_RClipThresh_sl, SIGNAL(valueChanged(double)), AWB_AWS_RClipThresh_sb, SLOT(setValue(double)));
	QObject::connect(AWB_AWS_GClipThresh_sl, SIGNAL(valueChanged(double)), AWB_AWS_GClipThresh_sb, SLOT(setValue(double)));
	QObject::connect(AWB_AWS_BClipThresh_sl, SIGNAL(valueChanged(double)), AWB_AWS_BClipThresh_sb, SLOT(setValue(double)));

	QObject::connect(AWB_AWS_BBDist_sb, SIGNAL(valueChanged(double)), AWB_AWS_BBDist_sl, SLOT(setValueD(double)));
	QObject::connect(AWB_AWS_RDarkThresh_sb, SIGNAL(valueChanged(double)), AWB_AWS_RDarkThresh_sl, SLOT(setValueD(double)));
	QObject::connect(AWB_AWS_GDarkThresh_sb, SIGNAL(valueChanged(double)), AWB_AWS_GDarkThresh_sl, SLOT(setValueD(double)));
	QObject::connect(AWB_AWS_BDarkThresh_sb, SIGNAL(valueChanged(double)), AWB_AWS_BDarkThresh_sl, SLOT(setValueD(double)));
	QObject::connect(AWB_AWS_RClipThresh_sb, SIGNAL(valueChanged(double)), AWB_AWS_RClipThresh_sl, SLOT(setValueD(double)));
	QObject::connect(AWB_AWS_GClipThresh_sb, SIGNAL(valueChanged(double)), AWB_AWS_GClipThresh_sl, SLOT(setValueD(double)));
	QObject::connect(AWB_AWS_BClipThresh_sb, SIGNAL(valueChanged(double)), AWB_AWS_BClipThresh_sl, SLOT(setValueD(double)));
}

//
// Private Functions
//

void VisionLive::ModuleWBC::HWVersionChanged()
{
	AWB_aws_gb->setVisible(_hwVersion >= HW_2_6);
	AWB_ts_gb->setVisible(_hwVersion >= HW_2_6);

	int previousState = AWB_mode_lb->currentIndex();
	AWB_mode_lb->clear();
	if (_hwVersion >= HW_2_6)
	{
		AWB_mode_lb->addItems(QStringList({ "Disabled", "Enabled" }));
		AWB_mode_lb->setCurrentIndex((previousState > 0) ? 1 : 0);
	}
	else
	{
		AWB_mode_lb->addItems(QStringList({ "Disabled", "Average Colour", "White Patch", "High Luminance White", "Combined" }));
		AWB_mode_lb->setCurrentIndex((previousState > 0) ? 4 : 0);
	}
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
        if (mode >= 0)
        {
            _pProxyHandler->LSH_set((bool)mode);
            emit moduleAWBModeChanged(mode);

            _pProxyHandler->AWB_set(mode);
        }
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
	setCSS(_css);
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

