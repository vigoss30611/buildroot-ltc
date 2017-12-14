#include "mie.hpp"
#include "objectregistry.hpp"

#include "ispc/ModuleMIE.h"

#define MIE_ENABLE "MIE_ENABLE"

//
// Public Functions
//

VisionLive::ModuleMIE::ModuleMIE(QWidget *parent): ModuleBase(parent)
{
	Ui::MIE::setupUi(this);

	retranslate();

	_firstLoad = true;

	initModule();

	initObjectsConnections();
}

VisionLive::ModuleMIE::~ModuleMIE()
{
	for(unsigned int i = 0; i < _MIEWidgets.size(); i++)
	{
		if(_MIEWidgets[i])
		{
			//_MIEWidgets[i]->deleteLater();
            delete _MIEWidgets[i];
		}
        _MIEWidgets.clear();
	}
}

void VisionLive::ModuleMIE::removeObjectValueChangedMark()
{
	switch(_css)
	{
	case CUSTOM_CSS:
		for(int i = 0; i < MIE_NUM_MEMCOLOURS; i++)
		{
			_MIEWidgets[i]->lumaMin_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_MIEWidgets[i]->lumaMax_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);

			_MIEWidgets[i]->gain0_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_MIEWidgets[i]->gain1_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_MIEWidgets[i]->gain2_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_MIEWidgets[i]->gain3_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);

			_MIEWidgets[i]->chromaExtent0_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_MIEWidgets[i]->chromaExtent1_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_MIEWidgets[i]->chromaExtent2_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_MIEWidgets[i]->chromaExtent3_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);

			//_MIEWidgets[i]->cbCenter_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			//_MIEWidgets[i]->crCenter_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_MIEWidgets[i]->cbCenter_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_MIEWidgets[i]->crCenter_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_MIEWidgets[i]->aspect_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_MIEWidgets[i]->rotation_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);

			_MIEWidgets[i]->brightness_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_MIEWidgets[i]->contrast_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_MIEWidgets[i]->saturation_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
			_MIEWidgets[i]->hue_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);

			_MIEWidgets[i]->enable_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);

			_MIEWidgets[i]->showSelection_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
		}
		break;

	case DEFAULT_CSS:
		for(int i = 0; i < MIE_NUM_MEMCOLOURS; i++)
		{
			_MIEWidgets[i]->lumaMin_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_MIEWidgets[i]->lumaMax_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);

			_MIEWidgets[i]->gain0_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_MIEWidgets[i]->gain1_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_MIEWidgets[i]->gain2_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_MIEWidgets[i]->gain3_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);

			_MIEWidgets[i]->chromaExtent0_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_MIEWidgets[i]->chromaExtent1_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_MIEWidgets[i]->chromaExtent2_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_MIEWidgets[i]->chromaExtent3_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);

			//_MIEWidgets[i]->cbCenter_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			//_MIEWidgets[i]->crCenter_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_MIEWidgets[i]->cbCenter_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_MIEWidgets[i]->crCenter_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_MIEWidgets[i]->aspect_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_MIEWidgets[i]->rotation_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);

			_MIEWidgets[i]->brightness_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_MIEWidgets[i]->contrast_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_MIEWidgets[i]->saturation_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
			_MIEWidgets[i]->hue_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);

			_MIEWidgets[i]->enable_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);

			_MIEWidgets[i]->showSelection_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
		}
		break;
	}
}

void VisionLive::ModuleMIE::retranslate()
{
	Ui::MIE::retranslateUi(this);
}

void VisionLive::ModuleMIE::load(const ISPC::ParameterList &config)
{
	if(_firstLoad)
	{
		for(int i = 0; i < MIE_NUM_MEMCOLOURS; i++)
		{
			addMC();
		}

		_firstLoad = false;
	}

	ISPC::ModuleMIE mie;
	mie.load(config);

	//
	// Set minimum
	//

	for(int i = 0; i < MIE_NUM_MEMCOLOURS; i++)
    {
		_MIEWidgets[i]->lumaMin_sb->setMinimum(ISPC::ModuleMIE::MIE_YMIN_S.min);
		_MIEWidgets[i]->lumaMax_sb->setMinimum(ISPC::ModuleMIE::MIE_YMAX_S.min);

		_MIEWidgets[i]->gain0_sl->setMinimumD(ISPC::ModuleMIE::MIE_YGAINS_S.min);
		_MIEWidgets[i]->gain1_sl->setMinimumD(ISPC::ModuleMIE::MIE_YGAINS_S.min);
		_MIEWidgets[i]->gain2_sl->setMinimumD(ISPC::ModuleMIE::MIE_YGAINS_S.min);
		_MIEWidgets[i]->gain3_sl->setMinimumD(ISPC::ModuleMIE::MIE_YGAINS_S.min);

		_MIEWidgets[i]->gain0_sb->setMinimum(ISPC::ModuleMIE::MIE_YGAINS_S.min);
		_MIEWidgets[i]->gain1_sb->setMinimum(ISPC::ModuleMIE::MIE_YGAINS_S.min);
		_MIEWidgets[i]->gain2_sb->setMinimum(ISPC::ModuleMIE::MIE_YGAINS_S.min);
		_MIEWidgets[i]->gain3_sb->setMinimum(ISPC::ModuleMIE::MIE_YGAINS_S.min);

		_MIEWidgets[i]->chromaExtent0_sl->setMinimumD(ISPC::ModuleMIE::MIE_CEXTENT_S.min);
		_MIEWidgets[i]->chromaExtent1_sl->setMinimumD(ISPC::ModuleMIE::MIE_CEXTENT_S.min);
		_MIEWidgets[i]->chromaExtent2_sl->setMinimumD(ISPC::ModuleMIE::MIE_CEXTENT_S.min);
		_MIEWidgets[i]->chromaExtent3_sl->setMinimumD(ISPC::ModuleMIE::MIE_CEXTENT_S.min);

		_MIEWidgets[i]->chromaExtent0_sb->setMinimum(ISPC::ModuleMIE::MIE_CEXTENT_S.min);
		_MIEWidgets[i]->chromaExtent1_sb->setMinimum(ISPC::ModuleMIE::MIE_CEXTENT_S.min);
		_MIEWidgets[i]->chromaExtent2_sb->setMinimum(ISPC::ModuleMIE::MIE_CEXTENT_S.min);
		_MIEWidgets[i]->chromaExtent3_sb->setMinimum(ISPC::ModuleMIE::MIE_CEXTENT_S.min);

		_MIEWidgets[i]->cbCenter_sl->setMinimumD(-0.25f);
        _MIEWidgets[i]->crCenter_sl->setMinimumD(-0.25f);
		_MIEWidgets[i]->aspect_sl->setMinimumD(ISPC::ModuleMIE::MIE_CASPECT_S.min);
		_MIEWidgets[i]->rotation_sl->setMinimumD(ISPC::ModuleMIE::MIE_CROTATION_S.min);

		//_MIEWidgets[i]->cbCenter_sb->setMinimum(ISPC::ModuleMIE::MIE_CB0.min);
		//_MIEWidgets[i]->crCenter_sb->setMinimum(ISPC::ModuleMIE::MIE_CR0.min);
		_MIEWidgets[i]->cbCenter_sb->setMinimum(-0.25f);
        _MIEWidgets[i]->crCenter_sb->setMinimum(-0.25f);
		_MIEWidgets[i]->aspect_sb->setMinimum(ISPC::ModuleMIE::MIE_CASPECT_S.min);
		_MIEWidgets[i]->rotation_sb->setMinimum(ISPC::ModuleMIE::MIE_CROTATION_S.min);

		_MIEWidgets[i]->brightness_sl->setMinimumD(ISPC::ModuleMIE::MIE_OUT_BRIGHTNESS_S.min);
		_MIEWidgets[i]->contrast_sl->setMinimumD(ISPC::ModuleMIE::MIE_OUT_CONTRAST_S.min);
		_MIEWidgets[i]->saturation_sl->setMinimumD(ISPC::ModuleMIE::MIE_OUT_SATURATION_S.min);
		_MIEWidgets[i]->hue_sl->setMinimumD(ISPC::ModuleMIE::MIE_OUT_HUE_S.min);

		_MIEWidgets[i]->brightness_sb->setMinimum(ISPC::ModuleMIE::MIE_OUT_BRIGHTNESS_S.min);
		_MIEWidgets[i]->contrast_sb->setMinimum(ISPC::ModuleMIE::MIE_OUT_CONTRAST_S.min);
		_MIEWidgets[i]->saturation_sb->setMinimum(ISPC::ModuleMIE::MIE_OUT_SATURATION_S.min);
		_MIEWidgets[i]->hue_sb->setMinimum(ISPC::ModuleMIE::MIE_OUT_HUE_S.min);
	}

	//
	// Set maximum
	//

	for(int i = 0; i < MIE_NUM_MEMCOLOURS; i++)
    {
		_MIEWidgets[i]->lumaMin_sb->setMaximum(ISPC::ModuleMIE::MIE_YMIN_S.max);
		_MIEWidgets[i]->lumaMax_sb->setMaximum(ISPC::ModuleMIE::MIE_YMAX_S.max);

		_MIEWidgets[i]->gain0_sl->setMaximumD(ISPC::ModuleMIE::MIE_YGAINS_S.max);
		_MIEWidgets[i]->gain1_sl->setMaximumD(ISPC::ModuleMIE::MIE_YGAINS_S.max);
		_MIEWidgets[i]->gain2_sl->setMaximumD(ISPC::ModuleMIE::MIE_YGAINS_S.max);
		_MIEWidgets[i]->gain3_sl->setMaximumD(ISPC::ModuleMIE::MIE_YGAINS_S.max);

		_MIEWidgets[i]->gain0_sb->setMaximum(ISPC::ModuleMIE::MIE_YGAINS_S.max);
		_MIEWidgets[i]->gain1_sb->setMaximum(ISPC::ModuleMIE::MIE_YGAINS_S.max);
		_MIEWidgets[i]->gain2_sb->setMaximum(ISPC::ModuleMIE::MIE_YGAINS_S.max);
		_MIEWidgets[i]->gain3_sb->setMaximum(ISPC::ModuleMIE::MIE_YGAINS_S.max);

		_MIEWidgets[i]->chromaExtent0_sl->setMaximumD(ISPC::ModuleMIE::MIE_CEXTENT_S.max);
		_MIEWidgets[i]->chromaExtent1_sl->setMaximumD(ISPC::ModuleMIE::MIE_CEXTENT_S.max);
		_MIEWidgets[i]->chromaExtent2_sl->setMaximumD(ISPC::ModuleMIE::MIE_CEXTENT_S.max);
		_MIEWidgets[i]->chromaExtent3_sl->setMaximumD(ISPC::ModuleMIE::MIE_CEXTENT_S.max);

		_MIEWidgets[i]->chromaExtent0_sb->setMaximum(ISPC::ModuleMIE::MIE_CEXTENT_S.max);
		_MIEWidgets[i]->chromaExtent1_sb->setMaximum(ISPC::ModuleMIE::MIE_CEXTENT_S.max);
		_MIEWidgets[i]->chromaExtent2_sb->setMaximum(ISPC::ModuleMIE::MIE_CEXTENT_S.max);
		_MIEWidgets[i]->chromaExtent3_sb->setMaximum(ISPC::ModuleMIE::MIE_CEXTENT_S.max);

		_MIEWidgets[i]->cbCenter_sl->setMaximumD(0.25f);
        _MIEWidgets[i]->crCenter_sl->setMaximumD(0.25f);
		_MIEWidgets[i]->aspect_sl->setMaximumD(ISPC::ModuleMIE::MIE_CASPECT_S.max);
		_MIEWidgets[i]->rotation_sl->setMaximumD(ISPC::ModuleMIE::MIE_CROTATION_S.max);

		//_MIEWidgets[i]->cbCenter_sb->setMaximum(ISPC::ModuleMIE::MIE_CB0.max);
		//_MIEWidgets[i]->crCenter_sb->setMaximum(ISPC::ModuleMIE::MIE_CR0.max);
		_MIEWidgets[i]->cbCenter_sb->setMaximum(0.25f);
        _MIEWidgets[i]->crCenter_sb->setMaximum(0.25f);
		_MIEWidgets[i]->aspect_sb->setMaximum(ISPC::ModuleMIE::MIE_CASPECT_S.max);
		_MIEWidgets[i]->rotation_sb->setMaximum(ISPC::ModuleMIE::MIE_CROTATION_S.max);

		_MIEWidgets[i]->brightness_sl->setMaximumD(ISPC::ModuleMIE::MIE_OUT_BRIGHTNESS_S.max);
		_MIEWidgets[i]->contrast_sl->setMaximumD(ISPC::ModuleMIE::MIE_OUT_CONTRAST_S.max);
		_MIEWidgets[i]->saturation_sl->setMaximumD(ISPC::ModuleMIE::MIE_OUT_SATURATION_S.max);
		_MIEWidgets[i]->hue_sl->setMaximumD(ISPC::ModuleMIE::MIE_OUT_HUE_S.max);

		_MIEWidgets[i]->brightness_sb->setMaximum(ISPC::ModuleMIE::MIE_OUT_BRIGHTNESS_S.max);
		_MIEWidgets[i]->contrast_sb->setMaximum(ISPC::ModuleMIE::MIE_OUT_CONTRAST_S.max);
		_MIEWidgets[i]->saturation_sb->setMaximum(ISPC::ModuleMIE::MIE_OUT_SATURATION_S.max);
		_MIEWidgets[i]->hue_sb->setMaximum(ISPC::ModuleMIE::MIE_OUT_HUE_S.max);
	}
	
	//
	// Set precision
	//

	for(int i = 0; i < MIE_NUM_MEMCOLOURS; i++)
    {
		DEFINE_PREC(_MIEWidgets[i]->lumaMin_sb, 1023.0000f);
		DEFINE_PREC(_MIEWidgets[i]->lumaMax_sb, 1023.0000f);

		DEFINE_PREC(_MIEWidgets[i]->gain0_sb, 255.000f);
		DEFINE_PREC(_MIEWidgets[i]->gain1_sb, 255.000f);
		DEFINE_PREC(_MIEWidgets[i]->gain2_sb, 255.000f);
		DEFINE_PREC(_MIEWidgets[i]->gain3_sb, 255.000f);

		//DEFINE_PREC(_MIEWidgets[i]->chromaExtent0_sb, 255.000f);
		//DEFINE_PREC(_MIEWidgets[i]->chromaExtent1_sb, 255.000f);
		//DEFINE_PREC(_MIEWidgets[i]->chromaExtent2_sb, 255.000f);
		//DEFINE_PREC(_MIEWidgets[i]->chromaExtent3_sb, 255.000f);
		DEFINE_PREC(_MIEWidgets[i]->chromaExtent0_sb, 1023.0000f);
		DEFINE_PREC(_MIEWidgets[i]->chromaExtent1_sb, 1023.0000f);
		DEFINE_PREC(_MIEWidgets[i]->chromaExtent2_sb, 1023.0000f);
		DEFINE_PREC(_MIEWidgets[i]->chromaExtent3_sb, 1023.0000f);

		DEFINE_PREC(_MIEWidgets[i]->cbCenter_sb, 254.000f);
		DEFINE_PREC(_MIEWidgets[i]->crCenter_sb, 254.000f);
		DEFINE_PREC(_MIEWidgets[i]->aspect_sb, 256.000f);
		DEFINE_PREC(_MIEWidgets[i]->rotation_sb, 256.000f);

		DEFINE_PREC(_MIEWidgets[i]->brightness_sb, 511.000f);
		DEFINE_PREC(_MIEWidgets[i]->contrast_sb, 16.00f);
		DEFINE_PREC(_MIEWidgets[i]->saturation_sb, 16.00f);
		DEFINE_PREC(_MIEWidgets[i]->hue_sb, 127.000f);
	}

	//
	// Set value
	//

	for(int i = 0; i < MIE_NUM_MEMCOLOURS; i++)
    {
		_MIEWidgets[i]->lumaMin_sb->setValue(mie.aYMin[i]);
		_MIEWidgets[i]->lumaMax_sb->setValue(mie.aYMax[i]);

		_MIEWidgets[i]->gain0_sl->setValueD(mie.aYGain[i][0]);
		_MIEWidgets[i]->gain1_sl->setValueD(mie.aYGain[i][1]);
		_MIEWidgets[i]->gain2_sl->setValueD(mie.aYGain[i][2]);
		_MIEWidgets[i]->gain3_sl->setValueD(mie.aYGain[i][3]);

		_MIEWidgets[i]->gain0_sb->setValue(mie.aYGain[i][0]);
		_MIEWidgets[i]->gain1_sb->setValue(mie.aYGain[i][1]);
		_MIEWidgets[i]->gain2_sb->setValue(mie.aYGain[i][2]);
		_MIEWidgets[i]->gain3_sb->setValue(mie.aYGain[i][3]);

		_MIEWidgets[i]->chromaExtent0_sl->setValueD(mie.aCExtent[i][0]);
		_MIEWidgets[i]->chromaExtent1_sl->setValueD(mie.aCExtent[i][1]);
		_MIEWidgets[i]->chromaExtent2_sl->setValueD(mie.aCExtent[i][2]);
		_MIEWidgets[i]->chromaExtent3_sl->setValueD(mie.aCExtent[i][3]);

		_MIEWidgets[i]->chromaExtent0_sb->setValue(mie.aCExtent[i][0]);
		_MIEWidgets[i]->chromaExtent1_sb->setValue(mie.aCExtent[i][1]);
		_MIEWidgets[i]->chromaExtent2_sb->setValue(mie.aCExtent[i][2]);
		_MIEWidgets[i]->chromaExtent3_sb->setValue(mie.aCExtent[i][3]);

		/*_MIEWidgets[i]->enable_cb->setChecked((mie.bEnable) & 
			((mie.aYGain[i][0] > 0.0f) | (mie.aYGain[i][1] > 0.0f) | 
			(mie.aYGain[i][2] > 0.0f) | (mie.aYGain[i][3] > 0.0f)));*/
		/*if(config.exists(MIE_ENABLE))
		{
			_MIEWidgets[i]->enable_cb->setChecked(config.getParameter(MIE_ENABLE)->get<bool>(i));
		}*/
		_MIEWidgets[i]->enable_cb->setChecked(mie.bEnabled[i]);

		_MIEWidgets[i]->cbCenter_sl->setValueD(mie.aCbCenter[i]);
		_MIEWidgets[i]->crCenter_sl->setValueD(mie.aCrCenter[i]);
		_MIEWidgets[i]->aspect_sl->setValueD(mie.aCAspect[i]);
		_MIEWidgets[i]->rotation_sl->setValueD(mie.aCRotation[i]);

		_MIEWidgets[i]->cbCenter_sb->setValue(mie.aCbCenter[i]);
		_MIEWidgets[i]->crCenter_sb->setValue(mie.aCrCenter[i]);
		_MIEWidgets[i]->aspect_sb->setValue(mie.aCAspect[i]);
		_MIEWidgets[i]->rotation_sb->setValue(mie.aCRotation[i]);

		_MIEWidgets[i]->brightness_sl->setValueD(mie.aOutBrightness[i]);
		_MIEWidgets[i]->contrast_sl->setValueD(mie.aOutContrast[i]);
		_MIEWidgets[i]->saturation_sl->setValueD(mie.aOutStaturation[i]);
		_MIEWidgets[i]->hue_sl->setValueD(mie.aOutHue[i]);

		_MIEWidgets[i]->brightness_sb->setValue(mie.aOutBrightness[i]);
		_MIEWidgets[i]->contrast_sb->setValue(mie.aOutContrast[i]);
		_MIEWidgets[i]->saturation_sb->setValue(mie.aOutStaturation[i]);
		_MIEWidgets[i]->hue_sb->setValue(mie.aOutHue[i]);
	}
}

void VisionLive::ModuleMIE::save(ISPC::ParameterList &config)
{
	ISPC::ModuleMIE mie;
	mie.load(config);

	std::string mieEnableParam;

    for(int i = 0; i < MIE_NUM_MEMCOLOURS; i++)
    {
		mieEnableParam.append(_MIEWidgets[i]->enable_cb->isChecked()? "1 " : "0 ");

        mie.bEnabled[i] = _MIEWidgets[i]->enable_cb->isChecked();
		mie.aYMin[i] = _MIEWidgets[i]->lumaMin_sb->value();
		mie.aYMax[i] = _MIEWidgets[i]->lumaMax_sb->value();

		if(_MIEWidgets[i]->enable_cb->isChecked())
		{
			mie.aYGain[i][0] = _MIEWidgets[i]->gain0_sb->value();
			mie.aYGain[i][1] = _MIEWidgets[i]->gain1_sb->value();
			mie.aYGain[i][2] = _MIEWidgets[i]->gain2_sb->value();
			mie.aYGain[i][3] = _MIEWidgets[i]->gain3_sb->value();
		}
		else
		{
			mie.aYGain[i][0] = 0.0f;
			mie.aYGain[i][1] = 0.0f;
			mie.aYGain[i][2] = 0.0f;
			mie.aYGain[i][3] = 0.0f;
		}

		mie.aCExtent[i][0] = _MIEWidgets[i]->chromaExtent0_sb->value();
		mie.aCExtent[i][1] = _MIEWidgets[i]->chromaExtent1_sb->value();
		mie.aCExtent[i][2] = _MIEWidgets[i]->chromaExtent2_sb->value();
		mie.aCExtent[i][3] = _MIEWidgets[i]->chromaExtent3_sb->value();

		mie.aCbCenter[i] = _MIEWidgets[i]->cbCenter_sb->value();
		mie.aCrCenter[i] = _MIEWidgets[i]->crCenter_sb->value();
		mie.aCAspect[i] = _MIEWidgets[i]->aspect_sb->value();
		mie.aCRotation[i] = _MIEWidgets[i]->rotation_sb->value();

		if(_MIEWidgets[i]->showSelection_cb->isChecked())
        {
			mie.aOutBrightness[i] = -1.0f;
        }
        else
        {
			mie.aOutBrightness[i] = _MIEWidgets[i]->brightness_sb->value();
        }

		mie.aOutContrast[i] = _MIEWidgets[i]->contrast_sb->value();
		mie.aOutStaturation[i] = _MIEWidgets[i]->saturation_sb->value();
		mie.aOutHue[i] = _MIEWidgets[i]->hue_sb->value();
        emit logError(QString::number(_MIEWidgets[i]->hue_sb->value()), "");
	}

	ISPC::Parameter mie_enable(MIE_ENABLE, mieEnableParam);
	config.addParameter(mie_enable, true);

	mie.save(config, ISPC::ModuleMIE::SAVE_VAL);
}

void VisionLive::ModuleMIE::updatePixelInfo(QPoint pixel, std::vector<int> pixelInfo)
{
	for(unsigned int i = 0; i < _MIEWidgets.size(); i++)
	{
        if (_MIEWidgets[i])
        {
            _MIEWidgets[i]->updatePixelInfo(pixel, pixelInfo);
        }
	}
}

//
// Protected Functions
//

void VisionLive::ModuleMIE::initModule()
{
	_name = windowTitle();
}

//
// Private Functions
//

void VisionLive::ModuleMIE::addMC()
{
	MIEWidget *widget = new MIEWidget();
	widget->setObjectName("MC_" + QString::number(_MIEWidgets.size()));

	if(_pObjReg)
	{
		_pObjReg->registerChildrenObjects(widget, "Modules/MIE/", widget->objectName());
	}
	else
	{
		emit logError("Couldn't register CCM patch!", "ModuleWBC::addCCM()");
	}

	MIE_memoryColours_tw->addTab(widget, "Memory colour " + QString::number(_MIEWidgets.size()));
	initObjectsConnections(widget);

	_MIEWidgets.push_back(widget);
}
