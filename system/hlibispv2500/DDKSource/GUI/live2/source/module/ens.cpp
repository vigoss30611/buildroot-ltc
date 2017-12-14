#include "ens.hpp"
#include "proxyhandler.hpp"

#include "ispc/ModuleENS.h"

//
// Public Functions
//

VisionLive::ModuleENS::ModuleENS(QWidget *parent): ModuleBase(parent)
{
	Ui::ENS::setupUi(this);

	retranslate();

	initModule();

	initObjectsConnections();
}

VisionLive::ModuleENS::~ModuleENS()
{
}

void VisionLive::ModuleENS::removeObjectValueChangedMark()
{
	switch(css)
	{
	case CUSTOM_CSS:
		ENSenable_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
		linesPerRegion_lb->setStyleSheet(CSS_CUSTOM_QCOMBOBOX);
		horizontalSubSampling_lb->setStyleSheet(CSS_CUSTOM_QCOMBOBOX);
		break;

	case DEFAULT_CSS:
		ENSenable_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
		linesPerRegion_lb->setStyleSheet(CSS_DEFAULT_QCOMBOBOX);
		horizontalSubSampling_lb->setStyleSheet(CSS_DEFAULT_QCOMBOBOX);
		break;
	}
}

void VisionLive::ModuleENS::retranslate()
{
	Ui::ENS::retranslateUi(this);
}

void VisionLive::ModuleENS::load(const ISPC::ParameterList &config)
{
	ISPC::ModuleENS ens;
	ens.load(config);

	//
	// Set minimum
	//

	//
	// Set maximum
	//

	//
	// Set precision
	//

	//
	// Set value
	//

	ENSenable_cb->setChecked(ens.bEnable);
	linesPerRegion_lb->setCurrentIndex(linesPerRegion_lb->findText(QString::number(ens.ui32NLines)));
	horizontalSubSampling_lb->setCurrentIndex(horizontalSubSampling_lb->findText(QString::number(ens.ui32SubsamplingFactor)));
}

void VisionLive::ModuleENS::save(ISPC::ParameterList &config) const
{
	ISPC::ModuleENS ens;
	ens.load(config);

	ens.bEnable = ENSenable_cb->isChecked();
	ens.ui32NLines = linesPerRegion_lb->currentText().toInt();
	ens.ui32SubsamplingFactor = horizontalSubSampling_lb->currentText().toInt();

	ens.save(config, ISPC::ModuleBase::SAVE_VAL);
}

void VisionLive::ModuleENS::setProxyHandler(ProxyHandler *proxy)
{
    _pProxyHandler = proxy;

	QObject::connect(_pProxyHandler, SIGNAL(ENS_received(IMG_UINT32)), this, SLOT(ENS_received(IMG_UINT32)));
}

//
// Protected Functions
//

void VisionLive::ModuleENS::initModule()
{
	_name = windowTitle();

	// Set Defaults
	linesPerRegion_lb->setCurrentIndex(1);
	horizontalSubSampling_lb->setCurrentIndex(0);
}

//
// Protected Slots
//

void VisionLive::ModuleENS::ENS_received(IMG_UINT32 size)
{
	statisticsSize_sb->setValue(size);
}
