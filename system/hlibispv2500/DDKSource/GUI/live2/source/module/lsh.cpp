#include "lsh.hpp"
#include "proxyhandler.hpp"

#include "ispc/ModuleLSH.h"

#include "lsh/tuning.hpp"

#include <QFileDialog>

//
// Public Functions
//

VisionLive::ModuleLSH::ModuleLSH(QWidget *parent): ModuleBase(parent)
{
	Ui::LSH::setupUi(this);

	retranslate();

	_lshApplyed = false;

	_pGenConfig = NULL;
	_pLSHTuning = NULL;

	initModule();

	initObjectsConnections();
}

VisionLive::ModuleLSH::~ModuleLSH()
{
}

void VisionLive::ModuleLSH::removeObjectValueChangedMark()
{
	switch(css)
	{
	case CUSTOM_CSS:
		LSH_enable_cb->setStyleSheet(CSS_CUSTOM_QCHECKBOX);
		break;

	case DEFAULT_CSS:
		LSH_enable_cb->setStyleSheet(CSS_DEFAULT_QCHECKBOX);
		break;
	}
}

void VisionLive::ModuleLSH::retranslate()
{
	Ui::LSH::retranslateUi(this);
}

void VisionLive::ModuleLSH::load(const ISPC::ParameterList &config)
{
	ISPC::ModuleLSH lsh;
	lsh.load(config);

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

	// We don't save nor load LSH_FILE and LSH_enable_cb cann't be checked if LSH grid has not been applyed
	//LSH_enable_cb->setChecked(lsh.bEnableMatrix);
	//LSH_file_le->setText(QString(lsh.lshFilename.c_str()));
}

void VisionLive::ModuleLSH::save(ISPC::ParameterList &config) const
{
	ISPC::ModuleLSH lsh;
	lsh.load(config);

	lsh.bEnableMatrix = LSH_enable_cb->isChecked();
	//lsh.lshFilename = std::string(LSH_file_le->text().toLatin1()).c_str(); // ISPC shouldn't read grid from file

	lsh.save(config, ISPC::ModuleLSH::SAVE_VAL);
}

void VisionLive::ModuleLSH::setCSS(CSS newCSS) 
{ 
	css = newCSS; 

	switch(css)
	{
	case DEFAULT_CSS:
		if(_pLSHTuning)
		{
			_pLSHTuning->changeCSS(CSS_DEFAULT_STYLESHEET);
		}
		break;
	case CUSTOM_CSS:
		if(_pLSHTuning)
		{
			_pLSHTuning->changeCSS(CSS_CUSTOM_VT_STYLESHEET);
		}
		break;
	}
}

//
// Protected Functions
//

void VisionLive::ModuleLSH::initModule()
{
	_name = windowTitle();

	LSH_enable_cb->setEnabled(false);
	LSH_apply_btn->setEnabled(false);

	QObject::connect(LSH_enable_cb, SIGNAL(clicked()), this, SLOT(enableChanged()));
	QObject::connect(LSH_apply_btn, SIGNAL(clicked()), this, SLOT(applyLSHGrid()));
	QObject::connect(LSH_file_le, SIGNAL(textChanged(QString)), this, SLOT(fileChanged()));
	QObject::connect(LSH_load_btn, SIGNAL(clicked()), this, SLOT(loadFile()));
	QObject::connect(LSH_tune_btn, SIGNAL(clicked()), this, SLOT(tune()));
}

//
// Public Slots
//

void VisionLive::ModuleLSH::moduleBLCChanged(int sensorBlack0, int sensorBlack1, int sensorBlack2, int sensorBlack3, int systemBlack)
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

void VisionLive::ModuleLSH::enableChanged()
{
	switch(css)
	{
	case CUSTOM_CSS:
		if(LSH_enable_cb->isChecked())
		{
			LSH_gb->setStyleSheet(CSS_QGROUPBOX_SELECTED);
		}
		else
		{
			LSH_gb->setStyleSheet(CSS_CUSTOM_QGROUPBOX);
		}
		break;

	case DEFAULT_CSS:
		if(LSH_enable_cb->isChecked())
		{
			LSH_gb->setStyleSheet(CSS_QGROUPBOX_SELECTED);
		}
		else
		{
			LSH_gb->setStyleSheet(CSS_DEFAULT_QGROUPBOX);
		}
		break;
	}
}

void VisionLive::ModuleLSH::applyLSHGrid()
{
	if(_pProxyHandler)
	{
		_pProxyHandler->LSH_set(LSH_file_le->text().toLocal8Bit().constData());

		_lastLSHFile = LSH_file_le->text().toLocal8Bit().constData();
		_lshApplyed = true;
		LSH_info_l->setText("");
		LSH_enable_cb->setEnabled(true);
	}
}

void VisionLive::ModuleLSH::fileChanged()
{
	QFile lshFile;
	lshFile.setFileName(LSH_file_le->text());
	if(!lshFile.exists() && !LSH_file_le->text().isEmpty()) 
	{
		LSH_info_l->setText("Can't find " + lshFile.fileName() + " file!");
	}
	else 
	{
		LSH_info_l->clear();
	}

	if(!LSH_file_le->text().isEmpty() && lshFile.exists()) 
	{
		LSH_apply_btn->setEnabled(true);
	}
	else 
	{
		LSH_apply_btn->setEnabled(false);
	}

	if(!LSH_file_le->text().isEmpty() && !_lastLSHFile.empty() && lshFile.exists())
	{
		if(strcmp(std::string(LSH_file_le->text().toLatin1()).c_str(), _lastLSHFile.c_str()) != 0)
		{
			LSH_info_l->setText(tr("Loaded LSH grid is not up to date!"));
		}
	}
}

void VisionLive::ModuleLSH::loadFile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Load parameters"), ".", tr("Parameter file (*.lsh)"));

    if(!filename.isEmpty())
	{
		LSH_file_le->setText(filename);
	}
}

void VisionLive::ModuleLSH::tune()
{
	if(!_pGenConfig)
	{
		_pGenConfig = new GenConfig();
	}
	_pGenConfig->setBLC(_blcConfig);

	if(_pLSHTuning)
	{
		_pLSHTuning->deleteLater();
	}
	_pLSHTuning = new LSHTuning(_pGenConfig, true);
	setCSS(css);
	QObject::connect(_pLSHTuning->integrate_btn, SIGNAL(clicked()), this, SLOT(integrate()));
	_pLSHTuning->show();
}

void VisionLive::ModuleLSH::integrate()
{
	QString filename = QFileDialog::getSaveFileName(this,
            tr("Save LSH grid"), QString(), tr("*.lsh"));

	if(_pLSHTuning->saveMatrix(QFileInfo(filename).dir().absolutePath()) != EXIT_SUCCESS)
    {
		emit logError(tr("Error saving LSG grid!"), "ModuleLSH::integrate()");
    }

	LSH_file_le->setText(filename);
}
