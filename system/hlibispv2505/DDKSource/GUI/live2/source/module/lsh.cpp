#include "lsh.hpp"
#include "proxyhandler.hpp"

#include "ispc/ModuleLSH.h"
#include "ispc/ControlLSH.h"

#include "lsh/tuning.hpp"

#include <QFileDialog>
#include <QTableWidgetItem>

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
	switch(_css)
	{
	case CUSTOM_CSS:
		break;

	case DEFAULT_CSS:
		break;
	}
}

void VisionLive::ModuleLSH::retranslate()
{
	Ui::LSH::retranslateUi(this);
}

void VisionLive::ModuleLSH::load(const ISPC::ParameterList &config)
{
	ISPC::ControlLSH clsh;

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

    if (!config.exists(clsh.LSH_CORRECTIONS.name) || config.getParameter(clsh.LSH_CORRECTIONS) == 0)
    {
        // Remove all unused matrices
        for (int i = lshGrids_tw->rowCount() - 1; i >= 0; i--)
        {
            if (lshGrids_tw->item(i, 0)->text().isEmpty())
                lshGrids_tw->removeRow(i);
        }

    }

    bool allFilesExists = true;
    bool duplicateDetected = false;
    for (int i = 0; i < config.getParameter(clsh.LSH_CORRECTIONS); i++)
    {
        QString filename = QString::fromLatin1(config.getParameter(clsh.LSH_FILE_S.indexed(i)).c_str());
        QFile f(filename);
        if (!f.exists())
        {
            emit logError(tr("Error adding LSH grid. File ") + filename + tr(" doesn't exists!"), "ModuleLSH::load()");
            allFilesExists = false;
            continue;
        }

        for (int j = 0; j < lshGrids_tw->rowCount(); j++)
        {
            if (lshGrids_tw->item(j, 1)->text() == filename)
            {
                duplicateDetected = true;
                break;
            }
        }
        if (duplicateDetected)
        {
            duplicateDetected = false;
            continue;
        }

        lshGrids_tw->insertRow(lshGrids_tw->rowCount());
        lshGrids_tw->setItem(lshGrids_tw->rowCount() - 1, 0, new QTableWidgetItem(QString()));
        lshGrids_tw->setItem(lshGrids_tw->rowCount() - 1, 1, new QTableWidgetItem(filename));
        QSpinBox *temp = new QSpinBox();
        temp->setRange(0, 100000);
        temp->setValue(config.getParameter(clsh.LSH_CTRL_TEMPERATURE_S.indexed(i)));
        temp->setReadOnly(true);
        temp->setButtonSymbols(QAbstractSpinBox::NoButtons);
        temp->setAlignment(Qt::AlignHCenter);
        lshGrids_tw->setCellWidget(lshGrids_tw->rowCount() - 1, 2, temp);
        QSpinBox *bitsPerDiff = new QSpinBox();
        bitsPerDiff->setRange(0, config.getParameter(clsh.LSH_CTRL_BITS_DIFF.indexed(i)));
        bitsPerDiff->setValue(config.getParameter(clsh.LSH_CTRL_BITS_DIFF.indexed(i)));
        bitsPerDiff->setReadOnly(true);
        bitsPerDiff->setButtonSymbols(QAbstractSpinBox::NoButtons);
        bitsPerDiff->setAlignment(Qt::AlignHCenter);
        lshGrids_tw->setCellWidget(lshGrids_tw->rowCount() - 1, 3, bitsPerDiff);
        QDoubleSpinBox *wbScale = new QDoubleSpinBox();
        wbScale->setRange(0, config.getParameter(clsh.LSH_CTRL_SCALE_WB_S.indexed(i)));
        wbScale->setValue(config.getParameter(clsh.LSH_CTRL_SCALE_WB_S.indexed(i)));
        wbScale->setReadOnly(true);
        wbScale->setButtonSymbols(QAbstractSpinBox::NoButtons);
        wbScale->setAlignment(Qt::AlignHCenter);
        lshGrids_tw->setCellWidget(lshGrids_tw->rowCount() - 1, 4, wbScale);
        QCheckBox *apply = new QCheckBox();
        lshGrids_tw->setCellWidget(lshGrids_tw->rowCount() - 1, 5, apply);
        QPushButton *deleteBtn = new QPushButton("Delete");
        QObject::connect(deleteBtn, SIGNAL(clicked()), this, SLOT(deleteInput()));
        lshGrids_tw->setCellWidget(lshGrids_tw->rowCount() - 1, 6, deleteBtn);
    }
    lshGrids_tw->resizeColumnsToContents();

    if (lshGrids_tw->rowCount())
    {
        add_btn->setEnabled(true);
        remove_btn->setEnabled(true);
        apply_btn->setEnabled(true);
    }

    if (!allFilesExists)
        emit riseError("Error occured while loading LSH.\nCheck log for details.");
}

void VisionLive::ModuleLSH::save(ISPC::ParameterList &config)
{
    ISPC::ControlLSH clsh;

    // Remove params from config
    if (config.exists(clsh.LSH_CORRECTIONS.name))
    {
        for (int i = 0; i < config.getParameter(clsh.LSH_CORRECTIONS); i++)
        {
            config.removeParameter(clsh.LSH_FILE_S.indexed(i).name);
            config.removeParameter(clsh.LSH_CTRL_TEMPERATURE_S.indexed(i).name);
            config.removeParameter(clsh.LSH_CTRL_BITS_DIFF.indexed(i).name);
            config.removeParameter(clsh.LSH_CTRL_SCALE_WB_S.indexed(i).name);
        }
    }

    ISPC::Parameter configurations(clsh.LSH_CORRECTIONS.name,
        std::string(QString::number(lshGrids_tw->rowCount()).toLatin1()).c_str());
    config.addParameter(configurations);
    for (int i = 0; i < lshGrids_tw->rowCount(); i++)
    {
        ISPC::Parameter filename(clsh.LSH_FILE_S.indexed(i).name,
            std::string(lshGrids_tw->item(i, 1)->text().toLatin1()).c_str());
        ISPC::Parameter temp(clsh.LSH_CTRL_TEMPERATURE_S.indexed(i).name,
            std::string(QString::number(((QSpinBox *)lshGrids_tw->cellWidget(i, 2))->value()).toLatin1()).c_str());
        ISPC::Parameter bitPerDiff(clsh.LSH_CTRL_BITS_DIFF.indexed(i).name,
            std::string(QString::number(((QSpinBox *)lshGrids_tw->cellWidget(i, 3))->value()).toLatin1()).c_str());
        ISPC::Parameter wbScale(clsh.LSH_CTRL_SCALE_WB_S.indexed(i).name,
            std::string(QString::number(((QDoubleSpinBox *)lshGrids_tw->cellWidget(i, 4))->value()).toLatin1()).c_str());

        config.addParameter(filename);
        config.addParameter(temp);
        config.addParameter(bitPerDiff);
        config.addParameter(wbScale);
    }
}

void VisionLive::ModuleLSH::setProxyHandler(ProxyHandler *proxy)
{
    _pProxyHandler = proxy;

    QObject::connect(_pProxyHandler, SIGNAL(LSH_added(QString)), this, SLOT(LSH_added(QString)));
    QObject::connect(_pProxyHandler, SIGNAL(LSH_removed(QString)), this, SLOT(LSH_removed(QString)));
    QObject::connect(_pProxyHandler, SIGNAL(LSH_applyed(QString)), this, SLOT(LSH_applyed(QString)));
    QObject::connect(_pProxyHandler, SIGNAL(LSH_received(QString)), this, SLOT(LSH_received(QString)));
}

void VisionLive::ModuleLSH::setCSS(CSS newCSS) 
{ 
	_css = newCSS; 

	switch(_css)
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

    file_le->setReadOnly(true);
    add_btn->setEnabled(false);
    remove_btn->setEnabled(false);
    apply_btn->setEnabled(false);

    initOutputFilesTable();

    QObject::connect(add_btn, SIGNAL(clicked()), this, SLOT(addLSHGrid()));
    QObject::connect(remove_btn, SIGNAL(clicked()), this, SLOT(removeLSHGrid()));
    QObject::connect(apply_btn, SIGNAL(clicked()), this, SLOT(applyLSHGrid()));

    QObject::connect(selectAll_cb, SIGNAL(stateChanged(int)), this, SLOT(selectAll(int)));

	QObject::connect(file_le, SIGNAL(textChanged(QString)), this, SLOT(fileChanged()));
	QObject::connect(tune_btn, SIGNAL(clicked()), this, SLOT(tune()));
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

void VisionLive::ModuleLSH::moduleAWBModeChanged(int mode)
{
    apply_btn->setEnabled(!((bool)mode));

    for (int i = 0; i < lshGrids_tw->rowCount(); i++)
    {
        if (lshGrids_tw->item(i, 0)->text() == QStringLiteral("APPLYED"))
        {
            lshGrids_tw->item(i, 0)->setText(QStringLiteral("ADDED"));
            break;
        }
    }
    file_le->setText(QStringLiteral(""));
}

//
// Protected Slots
//

void VisionLive::ModuleLSH::addLSHGrid()
{
    if (_pProxyHandler)
    {
        for (int i = 0; i < lshGrids_tw->rowCount(); i++)
        {
            if (((QCheckBox *)lshGrids_tw->cellWidget(i, 5))->isChecked())
                _pProxyHandler->LSH_add(std::string(lshGrids_tw->item(i, 1)->text().toLatin1()).c_str(), 
                ((QSpinBox *)lshGrids_tw->cellWidget(i, 2))->value(), 
                ((QSpinBox *)lshGrids_tw->cellWidget(i, 3))->value(),
                ((QSpinBox *)lshGrids_tw->cellWidget(i, 4))->value());
        }
	}
}

void VisionLive::ModuleLSH::LSH_added(QString filename)
{
    for (int i = 0; i < lshGrids_tw->rowCount(); i++)
    {
        if (lshGrids_tw->item(i, 1)->text() == filename)
        {
            if (lshGrids_tw->item(i, 0)->text() == "" || lshGrids_tw->item(i, 0)->text() == "ADDED")
            {
                lshGrids_tw->item(i, 0)->setText(QStringLiteral("ADDED"));
            }
            else if (lshGrids_tw->item(i, 0)->text() == "APPLYED")
            {
                lshGrids_tw->item(i, 0)->setText(QStringLiteral("APPLYED"));
            }
            lshGrids_tw->resizeColumnsToContents();
            break;
        }
    }

    // Enable/Disable "Delete" buttons
    for (int i = 0; i < lshGrids_tw->rowCount(); i++)
        ((QPushButton *)lshGrids_tw->cellWidget(i, 6))->setEnabled(lshGrids_tw->item(i, 0)->text().isEmpty());
}

void VisionLive::ModuleLSH::removeLSHGrid()
{
    if (_pProxyHandler)
    {
        for (int i = 0; i < lshGrids_tw->rowCount(); i++)
        {
            if (((QCheckBox *)lshGrids_tw->cellWidget(i, 5))->isChecked())
                _pProxyHandler->LSH_remove(std::string(lshGrids_tw->item(i, 1)->text().toLatin1()).c_str());
        }
    }
}

void VisionLive::ModuleLSH::LSH_removed(QString filename)
{
    for (int i = 0; i < lshGrids_tw->rowCount(); i++)
    {
        if (lshGrids_tw->item(i, 1)->text() == filename)
        {
            lshGrids_tw->item(i, 0)->setText(QString());
            lshGrids_tw->resizeColumnsToContents();
            if (lshGrids_tw->item(i, 1)->text() == file_le->text())
                file_le->setText(QString());
            break;
        }
    }

    // Enable/Disable "Delete" buttons
    for (int i = 0; i < lshGrids_tw->rowCount(); i++)
        ((QPushButton *)lshGrids_tw->cellWidget(i, 6))->setEnabled(lshGrids_tw->item(i, 0)->text().isEmpty());
}

void VisionLive::ModuleLSH::applyLSHGrid()
{
    if (_pProxyHandler)
    {
        for (int i = 0; i < lshGrids_tw->rowCount(); i++)
        {
            if (((QCheckBox *)lshGrids_tw->cellWidget(i, 5))->isChecked())
            {
                _pProxyHandler->LSH_apply(std::string(lshGrids_tw->item(i, 1)->text().toLatin1()).c_str());
                return;
            }
        }
    }
}

void VisionLive::ModuleLSH::LSH_applyed(QString filename)
{
    for (int i = 0; i < lshGrids_tw->rowCount(); i++)
    {
        if (lshGrids_tw->item(i, 1)->text() == filename)
        {
            lshGrids_tw->item(i, 0)->setText(QStringLiteral("APPLYED"));
            lshGrids_tw->resizeColumnsToContents();
        }
        else if (lshGrids_tw->item(i, 0)->text() == QStringLiteral("APPLYED"))
        {
            lshGrids_tw->item(i, 0)->setText(QStringLiteral("ADDED"));
        }
    }
}

void VisionLive::ModuleLSH::LSH_received(QString filename)
{
    if (file_le)
        file_le->setText(filename);
}

void VisionLive::ModuleLSH::selectAll(int state)
{
    for (int i = 0; i < lshGrids_tw->rowCount(); i++)
        ((QCheckBox *)lshGrids_tw->cellWidget(i, 5))->setChecked(state);
}

void VisionLive::ModuleLSH::fileChanged()
{
	QFile lshFile;
	lshFile.setFileName(file_le->text());
	if(!lshFile.exists() && !file_le->text().isEmpty()) 
	{
		info_lbl->setText("Can't find " + lshFile.fileName() + " file!");
	}
	else 
	{
		info_lbl->clear();
	}

	if(!file_le->text().isEmpty() && !_lastLSHFile.empty() && lshFile.exists())
	{
		if(strcmp(std::string(file_le->text().toLatin1()).c_str(), _lastLSHFile.c_str()) != 0)
		{
			info_lbl->setText(tr("Loaded LSH grid is not up to date!"));
		}
	}
}

void VisionLive::ModuleLSH::loadFile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Load parameters"), ".", tr("Parameter file (*.lsh)"));

    if(!filename.isEmpty())
	{
		file_le->setText(filename);
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
	_pLSHTuning = new LSHTuning(_pGenConfig, _hwVersion, true);
	setCSS(_css);
	QObject::connect(_pLSHTuning->integrate_btn, SIGNAL(clicked()), this, SLOT(integrate()));
	_pLSHTuning->show();
}

void VisionLive::ModuleLSH::initOutputFilesTable()
{
    QStringList labels;
    labels << "Sate" << "File" << "Temperature" << "BitsPerDiff" << "WBScale" << "Select" << "Delete";
    lshGrids_tw->setColumnCount(labels.size());
    lshGrids_tw->setRowCount(0);
    lshGrids_tw->setHorizontalHeaderLabels(labels);

    lshGrids_tw->setEditTriggers(false);
}

void VisionLive::ModuleLSH::integrate()
{
    QString dir = QFileDialog::getExistingDirectory();
    if (dir.isEmpty()) return;

	if(_pLSHTuning->saveMatrix(dir) != EXIT_SUCCESS)
    {
		emit logError(tr("Error saving LSG grid!"), "ModuleLSH::integrate()");
    }

	// Add to table all the files from _pLSHTuning
    std::map<int, QString> output = _pLSHTuning->getOutputFiles();
    std::map<int, QString>::const_iterator it;
    bool allDataValid = true;
    for (it = output.begin(); it != output.end(); it++)
    {
        if (!inputOk(it->second, it->first))
        {
            emit logError(tr("Error adding LSH grid. Filename or temeperature duplication detected."), "ModuleLSH::integrate()");
            allDataValid = false;
            continue;
        }
        lshGrids_tw->insertRow(lshGrids_tw->rowCount());
        lshGrids_tw->setItem(lshGrids_tw->rowCount() - 1, 0, new QTableWidgetItem(QString()));
        lshGrids_tw->setItem(lshGrids_tw->rowCount() - 1, 1, new QTableWidgetItem(dir + "/" + it->second));
        QSpinBox *temp = new QSpinBox();
        temp->setRange(0, 100000);
        temp->setValue(it->first);
        temp->setReadOnly(true);
        temp->setButtonSymbols(QAbstractSpinBox::NoButtons);
        temp->setAlignment(Qt::AlignHCenter);
        lshGrids_tw->setCellWidget(lshGrids_tw->rowCount() - 1, 2, temp);
        QSpinBox *bitsPerDiff = new QSpinBox();
        //bitsPerDiff->setRange(0, LSH_DELTA_BITS_MAX);
        //bitsPerDiff->setValue(LSH_DELTA_BITS_MAX);
        bitsPerDiff->setRange(0, _pLSHTuning->getBitsPerDiff(temp->value()));
        bitsPerDiff->setValue(_pLSHTuning->getBitsPerDiff(temp->value()));
        bitsPerDiff->setReadOnly(true);
        bitsPerDiff->setButtonSymbols(QAbstractSpinBox::NoButtons);
        bitsPerDiff->setAlignment(Qt::AlignHCenter);
        lshGrids_tw->setCellWidget(lshGrids_tw->rowCount() - 1, 3, bitsPerDiff);
        QDoubleSpinBox *wbScale = new QDoubleSpinBox();
        wbScale->setRange(0, _pLSHTuning->getWBScale(temp->value()));
        wbScale->setValue(_pLSHTuning->getWBScale(temp->value()));
        wbScale->setReadOnly(true);
        wbScale->setButtonSymbols(QAbstractSpinBox::NoButtons);
        wbScale->setAlignment(Qt::AlignHCenter);
        lshGrids_tw->setCellWidget(lshGrids_tw->rowCount() - 1, 4, wbScale);
        QCheckBox *apply = new QCheckBox();
        lshGrids_tw->setCellWidget(lshGrids_tw->rowCount() - 1, 5, apply);
        QPushButton *deleteBtn = new QPushButton("Delete");
        QObject::connect(deleteBtn, SIGNAL(clicked()), this, SLOT(deleteInput()));
        lshGrids_tw->setCellWidget(lshGrids_tw->rowCount() - 1, 6, deleteBtn);
    }
    lshGrids_tw->resizeColumnsToContents();

    if (lshGrids_tw->rowCount())
    {
        add_btn->setEnabled(true);
        remove_btn->setEnabled(true);
        apply_btn->setEnabled(true);
    }

    if (!allDataValid)
        emit riseError("Error occured while integrating.\nCheck log for details.");
}

void VisionLive::ModuleLSH::deleteInput()
{
    for (int i = 0; i < lshGrids_tw->rowCount(); i++)
    {
        if (((QPushButton *)lshGrids_tw->cellWidget(i, 6)) == (QPushButton *)sender())
        {
            lshGrids_tw->removeRow(i);
            return;
        }
    }
}

bool VisionLive::ModuleLSH::inputOk(QString filename, int temp)
{
    for (int i = 0; i < lshGrids_tw->rowCount(); i++)
    {
        if (lshGrids_tw->item(i, 1)->text() == filename ||
            ((QSpinBox *)lshGrids_tw->cellWidget(i, 2))->value() == temp)
        {
            return false;
        }
    }

    return true;
}

