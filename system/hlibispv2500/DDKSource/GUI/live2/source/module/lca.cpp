#include "lca.hpp"

#include "ispc/ModuleLCA.h"

#include "lca/tuning.hpp"

//
// Public Functions
//

VisionLive::ModuleLCA::ModuleLCA(QWidget *parent): ModuleBase(parent)
{
	Ui::LCA::setupUi(this);

	retranslate();

	_pGenConfig = NULL;
	_pLCATuning = NULL;

	initModule();

	initObjectsConnections();
}

VisionLive::ModuleLCA::~ModuleLCA()
{
}

void VisionLive::ModuleLCA::removeObjectValueChangedMark()
{
	switch(css)
	{
	case CUSTOM_CSS:
		LCA_bluePolyX1_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		LCA_bluePolyX2_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		LCA_bluePolyX3_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		LCA_bluePolyY1_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		LCA_bluePolyY2_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		LCA_bluePolyY3_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		LCA_blueCenterX_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		LCA_blueCenterY_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);

		LCA_redPolyX1_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		LCA_redPolyX2_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		LCA_redPolyX3_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		LCA_redPolyY1_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		LCA_redPolyY2_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		LCA_redPolyY3_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		LCA_redCenterX_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		LCA_redCenterY_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);

		LCA_shiftX_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		LCA_shiftY_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);

		LCA_decimationX_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		LCA_decimationY_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		break;

	case DEFAULT_CSS:
		LCA_bluePolyX1_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		LCA_bluePolyX2_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		LCA_bluePolyX3_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		LCA_bluePolyY1_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		LCA_bluePolyY2_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		LCA_bluePolyY3_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		LCA_blueCenterX_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		LCA_blueCenterY_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);

		LCA_redPolyX1_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		LCA_redPolyX2_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		LCA_redPolyX3_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		LCA_redPolyY1_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		LCA_redPolyY2_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		LCA_redPolyY3_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		LCA_redCenterX_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		LCA_redCenterY_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);

		LCA_shiftX_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		LCA_shiftY_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);

		LCA_decimationX_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		LCA_decimationY_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		break;
	}
}

void VisionLive::ModuleLCA::retranslate()
{
	Ui::LCA::retranslateUi(this);
}

void VisionLive::ModuleLCA::load(const ISPC::ParameterList &config)
{
	ISPC::ModuleLCA lca;
	lca.load(config);

	//
	// Set minimum
	//

	LCA_bluePolyX1_sb->setMinimum(lca.LCA_BLUEPOLY_X.min);
	LCA_bluePolyX2_sb->setMinimum(lca.LCA_BLUEPOLY_X.min);
	LCA_bluePolyX3_sb->setMinimum(lca.LCA_BLUEPOLY_X.min);
	LCA_bluePolyY1_sb->setMinimum(lca.LCA_BLUEPOLY_Y.min);
	LCA_bluePolyY2_sb->setMinimum(lca.LCA_BLUEPOLY_Y.min);
	LCA_bluePolyY3_sb->setMinimum(lca.LCA_BLUEPOLY_Y.min);
	LCA_blueCenterX_sb->setMinimum(lca.LCA_BLUECENTER.min);
	LCA_blueCenterY_sb->setMinimum(lca.LCA_BLUECENTER.min);

	LCA_redPolyX1_sb->setMinimum(lca.LCA_REDPOLY_X.min);
	LCA_redPolyX2_sb->setMinimum(lca.LCA_REDPOLY_X.min);
	LCA_redPolyX3_sb->setMinimum(lca.LCA_REDPOLY_X.min);
	LCA_redPolyY1_sb->setMinimum(lca.LCA_REDPOLY_Y.min);
	LCA_redPolyY2_sb->setMinimum(lca.LCA_REDPOLY_Y.min);
	LCA_redPolyY3_sb->setMinimum(lca.LCA_REDPOLY_Y.min);
	LCA_redCenterX_sb->setMinimum(lca.LCA_REDCENTER.min);
	LCA_redCenterY_sb->setMinimum(lca.LCA_REDCENTER.min);

	LCA_shiftX_sb->setMinimum(lca.LCA_SHIFT.min);
	LCA_shiftY_sb->setMinimum(lca.LCA_SHIFT.min);

	LCA_decimationX_sb->setMinimum(lca.LCA_DEC.min);
	LCA_decimationY_sb->setMinimum(lca.LCA_DEC.min);

	//
	// Set maximum
	//
	
	LCA_bluePolyX1_sb->setMaximum(lca.LCA_BLUEPOLY_X.max);
	LCA_bluePolyX2_sb->setMaximum(lca.LCA_BLUEPOLY_X.max);
	LCA_bluePolyX3_sb->setMaximum(lca.LCA_BLUEPOLY_X.max);
	LCA_bluePolyY1_sb->setMaximum(lca.LCA_BLUEPOLY_Y.max);
	LCA_bluePolyY2_sb->setMaximum(lca.LCA_BLUEPOLY_Y.max);
	LCA_bluePolyY3_sb->setMaximum(lca.LCA_BLUEPOLY_Y.max);
	LCA_blueCenterX_sb->setMaximum(lca.LCA_BLUECENTER.max);
	LCA_blueCenterY_sb->setMaximum(lca.LCA_BLUECENTER.max);

	LCA_redPolyX1_sb->setMaximum(lca.LCA_REDPOLY_X.max);
	LCA_redPolyX2_sb->setMaximum(lca.LCA_REDPOLY_X.max);
	LCA_redPolyX3_sb->setMaximum(lca.LCA_REDPOLY_X.max);
	LCA_redPolyY1_sb->setMaximum(lca.LCA_REDPOLY_Y.max);
	LCA_redPolyY2_sb->setMaximum(lca.LCA_REDPOLY_Y.max);
	LCA_redPolyY3_sb->setMaximum(lca.LCA_REDPOLY_Y.max);
	LCA_redCenterX_sb->setMaximum(lca.LCA_REDCENTER.max);
	LCA_redCenterY_sb->setMaximum(lca.LCA_REDCENTER.max);

	LCA_shiftX_sb->setMaximum(lca.LCA_SHIFT.max);
	LCA_shiftY_sb->setMaximum(lca.LCA_SHIFT.max);

	LCA_decimationX_sb->setMaximum(lca.LCA_DEC.max);
	LCA_decimationY_sb->setMaximum(lca.LCA_DEC.max);

	//
	// Set precision
	//

	//
	// Set value
	//
	
	LCA_bluePolyX1_sb->setValue(lca.aBluePoly_X[0]);
	LCA_bluePolyX2_sb->setValue(lca.aBluePoly_X[1]);
	LCA_bluePolyX3_sb->setValue(lca.aBluePoly_X[2]);
	LCA_bluePolyY1_sb->setValue(lca.aBluePoly_Y[0]);
	LCA_bluePolyY2_sb->setValue(lca.aBluePoly_Y[1]);
	LCA_bluePolyY3_sb->setValue(lca.aBluePoly_Y[2]);
	LCA_blueCenterX_sb->setValue(lca.aBlueCenter[0]);
	LCA_blueCenterY_sb->setValue(lca.aBlueCenter[1]);

	LCA_redPolyX1_sb->setValue(lca.aRedPoly_X[0]);
	LCA_redPolyX2_sb->setValue(lca.aRedPoly_X[1]);
	LCA_redPolyX3_sb->setValue(lca.aRedPoly_X[2]);
	LCA_redPolyY1_sb->setValue(lca.aRedPoly_Y[0]);
	LCA_redPolyY2_sb->setValue(lca.aRedPoly_Y[1]);
	LCA_redPolyY3_sb->setValue(lca.aRedPoly_Y[2]);
	LCA_redCenterX_sb->setValue(lca.aRedCenter[0]);
	LCA_redCenterY_sb->setValue(lca.aRedCenter[1]);

	LCA_shiftX_sb->setValue(lca.aShift[0]);
	LCA_shiftY_sb->setValue(lca.aShift[1]);

	LCA_decimationX_sb->setValue(lca.aDecimation[0]);
	LCA_decimationY_sb->setValue(lca.aDecimation[1]);
}

void VisionLive::ModuleLCA::save(ISPC::ParameterList &config) const
{
	ISPC::ModuleLCA lca;
	lca.load(config);

	lca.aBluePoly_X[0] = LCA_bluePolyX1_sb->value();
	lca.aBluePoly_X[1] = LCA_bluePolyX2_sb->value();
	lca.aBluePoly_X[2] = LCA_bluePolyX3_sb->value();
	lca.aBluePoly_Y[0] = LCA_bluePolyY1_sb->value();
	lca.aBluePoly_Y[1] = LCA_bluePolyY2_sb->value();
	lca.aBluePoly_Y[2] = LCA_bluePolyY3_sb->value();
	lca.aBlueCenter[0] = LCA_blueCenterX_sb->value();
	lca.aBlueCenter[1] = LCA_blueCenterY_sb->value();

	lca.aRedPoly_X[0] = LCA_redPolyX1_sb->value();
	lca.aRedPoly_X[1] = LCA_redPolyX2_sb->value();
	lca.aRedPoly_X[2] = LCA_redPolyX3_sb->value();
	lca.aRedPoly_Y[0] = LCA_redPolyY1_sb->value();
	lca.aRedPoly_Y[1] = LCA_redPolyY2_sb->value();
	lca.aRedPoly_Y[2] = LCA_redPolyY3_sb->value();
	lca.aRedCenter[0] = LCA_redCenterX_sb->value();
	lca.aRedCenter[1] = LCA_redCenterY_sb->value();

	lca.aShift[0] = LCA_shiftX_sb->value();
	lca.aShift[1] = LCA_shiftY_sb->value();

	lca.aDecimation[0] = LCA_decimationX_sb->value();
	lca.aDecimation[1] = LCA_decimationY_sb->value();

	lca.save(config, ISPC::ModuleBase::SAVE_VAL);
}

void VisionLive::ModuleLCA::setCSS(CSS newCSS) 
{ 
	css = newCSS; 

	switch(css)
	{
	case DEFAULT_CSS:
		if(_pLCATuning)
		{
			_pLCATuning->changeCSS(CSS_DEFAULT_STYLESHEET);
		}
		break;
	case CUSTOM_CSS:
		if(_pLCATuning)
		{
			_pLCATuning->changeCSS(CSS_CUSTOM_VT_STYLESHEET);
		}
		break;
	}
}

//
// Protected Functions
//

void VisionLive::ModuleLCA::initModule()
{
	_name = windowTitle();

	QObject::connect(LCA_tune_btn, SIGNAL(clicked()), this, SLOT(tune()));
}

//
// Protected Slots
//

void VisionLive::ModuleLCA::tune()
{
	if(!_pGenConfig)
	{
		_pGenConfig = new GenConfig();
	}
	_pGenConfig->setBLC(_blcConfig);

	if(_pLCATuning)
	{
		_pLCATuning->deleteLater();
	}
	_pLCATuning = new LCATuning(_pGenConfig, true);
	setCSS(css);
	QObject::connect(_pLCATuning->integrate_btn, SIGNAL(clicked()), this, SLOT(integrate()));
	_pLCATuning->show();
}

void VisionLive::ModuleLCA::integrate()
{
	LCA_bluePolyX1_sb->setValue(_pLCATuning->bluePolyX0->value());
	LCA_bluePolyX2_sb->setValue(_pLCATuning->bluePolyX1->value());
	LCA_bluePolyX3_sb->setValue(_pLCATuning->bluePolyX2->value());
	LCA_bluePolyY1_sb->setValue(_pLCATuning->bluePolyY0->value());
	LCA_bluePolyY2_sb->setValue(_pLCATuning->bluePolyY1->value());
	LCA_bluePolyY3_sb->setValue(_pLCATuning->bluePolyY2->value());
	LCA_blueCenterX_sb->setValue(_pLCATuning->centerX->value());
	LCA_blueCenterY_sb->setValue(_pLCATuning->centerY->value());

	LCA_redPolyX1_sb->setValue(_pLCATuning->redPolyX0->value());
	LCA_redPolyX2_sb->setValue(_pLCATuning->redPolyX1->value());
	LCA_redPolyX3_sb->setValue(_pLCATuning->redPolyX2->value());
	LCA_redPolyY1_sb->setValue(_pLCATuning->redPolyY0->value());
	LCA_redPolyY2_sb->setValue(_pLCATuning->redPolyY1->value());
	LCA_redPolyY3_sb->setValue(_pLCATuning->redPolyY2->value());
	LCA_redCenterX_sb->setValue(_pLCATuning->centerX->value());
	LCA_redCenterY_sb->setValue(_pLCATuning->centerY->value());

	LCA_shiftX_sb->setValue(_pLCATuning->shiftX->value());
	LCA_shiftY_sb->setValue(_pLCATuning->shiftY->value());

	/*LCA_decimationX_sb->setValue(lca.aDecimation[0]);
	LCA_decimationY_sb->setValue(lca.aDecimation[1]);*/
}
