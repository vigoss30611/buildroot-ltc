#include "out.hpp"
#include "proxyhandler.hpp"

#include "ispc/ModuleOUT.h"
#include "ispc/ModuleESC.h"
#include "ispc/ModuleDSC.h"
#include "ispc/ISPCDefs.h"  // ScalerRectType

#define QCIF_WIDTH 144
#define QCIF_HEIGHT 176

#define NUM_ENC_FORMATS 8
#define NUM_DISP_FORMATS 7

//
// Public Functions
//

VisionLive::ModuleOUT::ModuleOUT(QWidget *parent): ModuleBase(parent)
{
	Ui::OUT::setupUi(this);

	retranslate();

	initModule();

	initObjectsConnections();
}

VisionLive::ModuleOUT::~ModuleOUT()
{
}

void VisionLive::ModuleOUT::removeObjectValueChangedMark()
{
	switch(css)
	{
	case CUSTOM_CSS:
		encoderFormat_lb->setStyleSheet(CSS_CUSTOM_QCOMBOBOX);
		encoderWidth_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		encoderHeight_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);

		displayFormat_lb->setStyleSheet(CSS_CUSTOM_QCOMBOBOX);
		displayWidth_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);
		displayHeight_sb->setStyleSheet(CSS_CUSTOM_QSPINBOX);

		dataExtractionFormat_lb->setStyleSheet(CSS_CUSTOM_QCOMBOBOX);
		dataExtractionPoint_lb->setStyleSheet(CSS_CUSTOM_QCOMBOBOX);
		break;

	case DEFAULT_CSS:
		encoderFormat_lb->setStyleSheet(CSS_DEFAULT_QCOMBOBOX);
		encoderWidth_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		encoderHeight_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);

		displayFormat_lb->setStyleSheet(CSS_DEFAULT_QCOMBOBOX);
		displayWidth_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);
		displayHeight_sb->setStyleSheet(CSS_DEFAULT_QSPINBOX);

		dataExtractionFormat_lb->setStyleSheet(CSS_DEFAULT_QCOMBOBOX);
		dataExtractionPoint_lb->setStyleSheet(CSS_DEFAULT_QCOMBOBOX);
		break;
	}
}

void VisionLive::ModuleOUT::retranslate()
{
	Ui::OUT::retranslateUi(this);
}

void VisionLive::ModuleOUT::load(const ISPC::ParameterList &config)
{
	ISPC::ModuleOUT out;
	ISPC::ModuleESC esc;
	ISPC::ModuleDSC dsc;

	out.load(config);
	esc.load(config);
	dsc.load(config);

	if(!_firstSensorRead)
	{

		//
		// Set minimum
		//

		encoderWidth_sb->setMinimum(esc.ESC_RECT.min);
		encoderHeight_sb->setMinimum(esc.ESC_RECT.min);

		displayWidth_sb->setMinimum(dsc.DSC_RECT.min);
		displayHeight_sb->setMinimum(dsc.DSC_RECT.min);
	
		//
		// Set maximum
		//

		encoderWidth_sb->setMaximum(esc.ESC_RECT.max);
		encoderHeight_sb->setMaximum(esc.ESC_RECT.max);

		displayWidth_sb->setMaximum(dsc.DSC_RECT.max);
		displayHeight_sb->setMaximum(dsc.DSC_RECT.max);
	
		//
		// Set precision
		//

	}

	//
	// Set value
	//

	QString format = QString();
	int index = -1;

	encoderWidth_sb->setValue((esc.aRect[2] == 0)? encoderWidth_sb->maximum() : esc.aRect[2]);
	encoderHeight_sb->setValue((esc.aRect[3] == 0)? encoderWidth_sb->maximum() : esc.aRect[3]);

	format = FormatString((ePxlFormat)out.encoderType);
	index = encoderFormat_lb->findText(format);
	if(index > -1)
	{
		encoderFormat_lb->setCurrentIndex(index);
	}
	else
	{
		emit logError(QString(format) + " is not one of supported formats!", "ModuleOUT::load()");
	}

	// Display Pipeline
	displayWidth_sb->setValue((dsc.aRect[2] == 0)? encoderWidth_sb->maximum() : dsc.aRect[2]);
	displayHeight_sb->setValue((dsc.aRect[3] == 0)? encoderWidth_sb->maximum() : dsc.aRect[3]);

	format = FormatString(out.displayType);
	index = displayFormat_lb->findText(format);
	if(index > -1)
	{
		displayFormat_lb->setCurrentIndex(index);
	}
	else
	{
		emit logError(QString(format) + " is not one of supported formats!", "ModuleOUT::load()");
	}

	// Data Extraction
	format = FormatString(out.dataExtractionType);
	index = dataExtractionFormat_lb->findText(format);
	if(index > -1)
	{
		dataExtractionFormat_lb->setCurrentIndex(index);
	}
	else
	{
		emit logError(QString(format) + " is not one of supported formats!", "ModuleOUT::load()");
	}

	dataExtractionPoint_lb->setCurrentIndex(out.dataExtractionPoint);
}

void VisionLive::ModuleOUT::save(ISPC::ParameterList &config) const
{
	ISPC::ModuleOUT out;
	ISPC::ModuleESC esc;
	ISPC::ModuleDSC dsc;

	out.load(config);
	esc.load(config);
	dsc.load(config);

	esc.aPitch[0] = 0.0f;
    esc.aPitch[1] = 0.0f;
	esc.eRectType = ISPC::SCALER_RECT_SIZE;
	esc.aRect[2] = encoderWidth_sb->value();
	esc.aRect[3] = encoderHeight_sb->value();
	out.encoderType = (ePxlFormat)encoderFormat_lb->currentIndex();

	dsc.aPitch[0] = 0.0f;
    dsc.aPitch[1] = 0.0f;
	dsc.eRectType = ISPC::SCALER_RECT_SIZE;
	dsc.aRect[2] = displayWidth_sb->value();
	dsc.aRect[3] = displayHeight_sb->value();
	out.displayType = (ePxlFormat)((displayFormat_lb->currentIndex() == 0)? 0 : displayFormat_lb->currentIndex() + NUM_ENC_FORMATS);

	out.dataExtractionType = (ePxlFormat)((dataExtractionFormat_lb->currentIndex() == 0)? 0 : dataExtractionFormat_lb->currentIndex() + NUM_ENC_FORMATS + NUM_DISP_FORMATS);
	out.dataExtractionPoint = (CI_INOUT_POINTS)dataExtractionPoint_lb->currentIndex();

	out.save(config, ISPC::ModuleOUT::SAVE_VAL);
	esc.save(config, ISPC::ModuleESC::SAVE_VAL);
	dsc.save(config, ISPC::ModuleDSC::SAVE_VAL);
}

void VisionLive::ModuleOUT::setProxyHandler(ProxyHandler *proxy) 
{ 
	_pProxyHandler = proxy; 

	QObject::connect(_pProxyHandler, SIGNAL(SENSOR_received(QMap<QString, QString>)), this, SLOT(SENSOR_received(QMap<QString, QString>)));
} 

//
// Protected Functions
//

void VisionLive::ModuleOUT::initModule()
{
	_name = windowTitle();

	_firstSensorRead = false;

	connect(encoderFormat_lb, SIGNAL(currentIndexChanged(int)), this, SLOT(encoderFormatChanged(int)));
	connect(displayFormat_lb, SIGNAL(currentIndexChanged(int)), this, SLOT(displayFormatChanged(int)));
	connect(dataExtractionFormat_lb, SIGNAL(currentIndexChanged(int)), this, SLOT(dataExtractionFormatChanged(int)));
}

//
// Protected Slots
//

void VisionLive::ModuleOUT::encoderFormatChanged(int index)
{
	if(index)
	{
		encoderGroup_gb->setStyleSheet(CSS_QGROUPBOX_SELECTED);
	}
	else
	{
		encoderGroup_gb->setStyleSheet((css == DEFAULT_CSS)? CSS_DEFAULT_QGROUPBOX : CSS_CUSTOM_QGROUPBOX);
	}
}

void VisionLive::ModuleOUT::displayFormatChanged(int index)
{
	if(index)
	{
		dataExtractionFormat_lb->setCurrentIndex(0);

		displayGroup_gb->setStyleSheet(CSS_QGROUPBOX_SELECTED);
	}
	else
	{
		displayGroup_gb->setStyleSheet((css == DEFAULT_CSS)? CSS_DEFAULT_QGROUPBOX : CSS_CUSTOM_QGROUPBOX);
	}
}

void VisionLive::ModuleOUT::dataExtractionFormatChanged(int index)
{
	if(index)
	{
		displayFormat_lb->setCurrentIndex(0);

		dataExtractionGroup_gb->setStyleSheet(CSS_QGROUPBOX_SELECTED);
	}
	else
	{
		dataExtractionGroup_gb->setStyleSheet((css == DEFAULT_CSS)? CSS_DEFAULT_QGROUPBOX : CSS_CUSTOM_QGROUPBOX);
	}
}

void VisionLive::ModuleOUT::SENSOR_received(QMap<QString, QString> params)
{
	if(!_firstSensorRead)
	{
		encoderWidth_sb->setRange(QCIF_WIDTH, params.find("Width").value().toInt());
		encoderHeight_sb->setRange(QCIF_HEIGHT, params.find("Height").value().toInt());
		displayWidth_sb->setRange(QCIF_WIDTH, params.find("Width").value().toInt());
		displayHeight_sb->setRange(QCIF_HEIGHT, params.find("Height").value().toInt());
	}

	_firstSensorRead = true;
}