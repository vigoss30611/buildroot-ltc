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
	switch(_css)
	{
	case CUSTOM_CSS:
		bluePolyX1_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		bluePolyX2_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		bluePolyX3_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		bluePolyY1_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		bluePolyY2_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		bluePolyY3_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		blueCenterX_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		blueCenterY_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);

		redPolyX1_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		redPolyX2_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		redPolyX3_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		redPolyY1_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		redPolyY2_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		redPolyY3_sb->setStyleSheet(CSS_CUSTOM_QDOUBLESPINBOX);
		redCenterX_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		redCenterY_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);

		shiftX_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		shiftY_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);

		decimationX_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		decimationY_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		break;

	case DEFAULT_CSS:
		bluePolyX1_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		bluePolyX2_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		bluePolyX3_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		bluePolyY1_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		bluePolyY2_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		bluePolyY3_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		blueCenterX_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		blueCenterY_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);

		redPolyX1_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		redPolyX2_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		redPolyX3_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		redPolyY1_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		redPolyY2_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		redPolyY3_sb->setStyleSheet(CSS_DEFAULT_QDOUBLESPINBOX);
		redCenterX_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		redCenterY_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);

		shiftX_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		shiftY_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);

		decimationX_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		decimationY_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
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

	bluePolyX1_sb->setMinimum(lca.LCA_BLUEPOLY_X.min);
	bluePolyX2_sb->setMinimum(lca.LCA_BLUEPOLY_X.min);
	bluePolyX3_sb->setMinimum(lca.LCA_BLUEPOLY_X.min);
	bluePolyY1_sb->setMinimum(lca.LCA_BLUEPOLY_Y.min);
	bluePolyY2_sb->setMinimum(lca.LCA_BLUEPOLY_Y.min);
	bluePolyY3_sb->setMinimum(lca.LCA_BLUEPOLY_Y.min);
	blueCenterX_sb->setMinimum(lca.LCA_BLUECENTER.min);
	blueCenterY_sb->setMinimum(lca.LCA_BLUECENTER.min);

	redPolyX1_sb->setMinimum(lca.LCA_REDPOLY_X.min);
	redPolyX2_sb->setMinimum(lca.LCA_REDPOLY_X.min);
	redPolyX3_sb->setMinimum(lca.LCA_REDPOLY_X.min);
	redPolyY1_sb->setMinimum(lca.LCA_REDPOLY_Y.min);
	redPolyY2_sb->setMinimum(lca.LCA_REDPOLY_Y.min);
	redPolyY3_sb->setMinimum(lca.LCA_REDPOLY_Y.min);
	redCenterX_sb->setMinimum(lca.LCA_REDCENTER.min);
	redCenterY_sb->setMinimum(lca.LCA_REDCENTER.min);

	shiftX_sb->setMinimum(lca.LCA_SHIFT.min);
	shiftY_sb->setMinimum(lca.LCA_SHIFT.min);

	decimationX_sb->setMinimum(lca.LCA_DEC.min);
	decimationY_sb->setMinimum(lca.LCA_DEC.min);

	//
	// Set maximum
	//
	
	bluePolyX1_sb->setMaximum(lca.LCA_BLUEPOLY_X.max);
	bluePolyX2_sb->setMaximum(lca.LCA_BLUEPOLY_X.max);
	bluePolyX3_sb->setMaximum(lca.LCA_BLUEPOLY_X.max);
	bluePolyY1_sb->setMaximum(lca.LCA_BLUEPOLY_Y.max);
	bluePolyY2_sb->setMaximum(lca.LCA_BLUEPOLY_Y.max);
	bluePolyY3_sb->setMaximum(lca.LCA_BLUEPOLY_Y.max);
	blueCenterX_sb->setMaximum(lca.LCA_BLUECENTER.max);
	blueCenterY_sb->setMaximum(lca.LCA_BLUECENTER.max);

	redPolyX1_sb->setMaximum(lca.LCA_REDPOLY_X.max);
	redPolyX2_sb->setMaximum(lca.LCA_REDPOLY_X.max);
	redPolyX3_sb->setMaximum(lca.LCA_REDPOLY_X.max);
	redPolyY1_sb->setMaximum(lca.LCA_REDPOLY_Y.max);
	redPolyY2_sb->setMaximum(lca.LCA_REDPOLY_Y.max);
	redPolyY3_sb->setMaximum(lca.LCA_REDPOLY_Y.max);
	redCenterX_sb->setMaximum(lca.LCA_REDCENTER.max);
	redCenterY_sb->setMaximum(lca.LCA_REDCENTER.max);

	shiftX_sb->setMaximum(lca.LCA_SHIFT.max);
	shiftY_sb->setMaximum(lca.LCA_SHIFT.max);

	decimationX_sb->setMaximum(lca.LCA_DEC.max);
	decimationY_sb->setMaximum(lca.LCA_DEC.max);

	//
	// Set precision
	//

	//
	// Set value
	//
	
	bluePolyX1_sb->setValue(lca.aBluePoly_X[0]);
	bluePolyX2_sb->setValue(lca.aBluePoly_X[1]);
	bluePolyX3_sb->setValue(lca.aBluePoly_X[2]);
	bluePolyY1_sb->setValue(lca.aBluePoly_Y[0]);
	bluePolyY2_sb->setValue(lca.aBluePoly_Y[1]);
	bluePolyY3_sb->setValue(lca.aBluePoly_Y[2]);
	blueCenterX_sb->setValue(lca.aBlueCenter[0]);
	blueCenterY_sb->setValue(lca.aBlueCenter[1]);

	redPolyX1_sb->setValue(lca.aRedPoly_X[0]);
	redPolyX2_sb->setValue(lca.aRedPoly_X[1]);
	redPolyX3_sb->setValue(lca.aRedPoly_X[2]);
	redPolyY1_sb->setValue(lca.aRedPoly_Y[0]);
	redPolyY2_sb->setValue(lca.aRedPoly_Y[1]);
	redPolyY3_sb->setValue(lca.aRedPoly_Y[2]);
	redCenterX_sb->setValue(lca.aRedCenter[0]);
	redCenterY_sb->setValue(lca.aRedCenter[1]);

	shiftX_sb->setValue(lca.aShift[0]);
	shiftY_sb->setValue(lca.aShift[1]);

	decimationX_sb->setValue(lca.aDecimation[0]);
	decimationY_sb->setValue(lca.aDecimation[1]);
}

void VisionLive::ModuleLCA::save(ISPC::ParameterList &config)
{
	ISPC::ModuleLCA lca;
	lca.load(config);

	lca.aBluePoly_X[0] = bluePolyX1_sb->value();
	lca.aBluePoly_X[1] = bluePolyX2_sb->value();
	lca.aBluePoly_X[2] = bluePolyX3_sb->value();
	lca.aBluePoly_Y[0] = bluePolyY1_sb->value();
	lca.aBluePoly_Y[1] = bluePolyY2_sb->value();
	lca.aBluePoly_Y[2] = bluePolyY3_sb->value();
	lca.aBlueCenter[0] = blueCenterX_sb->value();
	lca.aBlueCenter[1] = blueCenterY_sb->value();

	lca.aRedPoly_X[0] = redPolyX1_sb->value();
	lca.aRedPoly_X[1] = redPolyX2_sb->value();
	lca.aRedPoly_X[2] = redPolyX3_sb->value();
	lca.aRedPoly_Y[0] = redPolyY1_sb->value();
	lca.aRedPoly_Y[1] = redPolyY2_sb->value();
	lca.aRedPoly_Y[2] = redPolyY3_sb->value();
	lca.aRedCenter[0] = redCenterX_sb->value();
	lca.aRedCenter[1] = redCenterY_sb->value();

	lca.aShift[0] = shiftX_sb->value();
	lca.aShift[1] = shiftY_sb->value();

	lca.aDecimation[0] = decimationX_sb->value();
	lca.aDecimation[1] = decimationY_sb->value();

	lca.save(config, ISPC::ModuleBase::SAVE_VAL);
}

void VisionLive::ModuleLCA::setCSS(CSS newCSS) 
{ 
	_css = newCSS; 

	switch(_css)
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

	QObject::connect(tune_btn, SIGNAL(clicked()), this, SLOT(tune()));
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
	setCSS(_css);
	QObject::connect(_pLCATuning->integrate_btn, SIGNAL(clicked()), this, SLOT(integrate()));
	_pLCATuning->show();
}

void VisionLive::ModuleLCA::integrate()
{
	bluePolyX1_sb->setValue(_pLCATuning->bluePolyX0->value());
	bluePolyX2_sb->setValue(_pLCATuning->bluePolyX1->value());
	bluePolyX3_sb->setValue(_pLCATuning->bluePolyX2->value());
	bluePolyY1_sb->setValue(_pLCATuning->bluePolyY0->value());
	bluePolyY2_sb->setValue(_pLCATuning->bluePolyY1->value());
	bluePolyY3_sb->setValue(_pLCATuning->bluePolyY2->value());
	blueCenterX_sb->setValue(_pLCATuning->centerX->value());
	blueCenterY_sb->setValue(_pLCATuning->centerY->value());

	redPolyX1_sb->setValue(_pLCATuning->redPolyX0->value());
	redPolyX2_sb->setValue(_pLCATuning->redPolyX1->value());
	redPolyX3_sb->setValue(_pLCATuning->redPolyX2->value());
	redPolyY1_sb->setValue(_pLCATuning->redPolyY0->value());
	redPolyY2_sb->setValue(_pLCATuning->redPolyY1->value());
	redPolyY3_sb->setValue(_pLCATuning->redPolyY2->value());
	redCenterX_sb->setValue(_pLCATuning->centerX->value());
	redCenterY_sb->setValue(_pLCATuning->centerY->value());

	shiftX_sb->setValue(_pLCATuning->shiftX->value());
	shiftY_sb->setValue(_pLCATuning->shiftY->value());

	/*decimationX_sb->setValue(lca.aDecimation[0]);
	decimationY_sb->setValue(lca.aDecimation[1]);*/
}
