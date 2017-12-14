/**
*******************************************************************************
@file lshconfig.cpp

@brief LSHConfig implementation

@copyright Imagination Technologies Ltd. All Rights Reserved.

@license Strictly Confidential.
No part of this software, either material or conceptual may be copied or
distributed, transmitted, transcribed, stored in a retrieval system or
translated into any human or computer language in any form by any means,
electronic, mechanical, manual or other-wise, or disclosed to third
parties without the express written permission of
Imagination Technologies Limited,
Unit 8, HomePark Industrial Estate,
King's Langley, Hertfordshire,
WD4 8LZ, U.K.

******************************************************************************/
#include "lsh/config.hpp"

#include <QWidget>
#include <QGraphicsRectItem>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <string>
#include "lsh/compute.hpp"

#include "buffer.hpp"
#include "imageview.hpp"
#include "image.h"

// #include "felix_hw_info.h"
// may need to change according to HW version in the future
#define LSH_TILE_MIN (1 << 3)
#define LSH_TILE_MAX (1 << 7)


/*
 * Private Functions
 */

void LSHConfig::init()
{
    Ui::LSHConfig::setupUi(this);

    outputMSE[0] = outputMSE_0;
    outputMSE[1] = outputMSE_1;
    outputMSE[2] = outputMSE_2;
    outputMSE[3] = outputMSE_3;

    outputMIN[0] = outputMin_0;
    outputMIN[1] = outputMin_1;
    outputMIN[2] = outputMin_2;
    outputMIN[3] = outputMin_3;

    outputMAX[0] = outputMax_0;
    outputMAX[1] = outputMax_1;
    outputMAX[2] = outputMax_2;
    outputMAX[3] = outputMax_3;

    start_btn = buttonBox->addButton(QDialogButtonBox::Ok);
    start_btn->setEnabled(false);
    default_btn = buttonBox->button(QDialogButtonBox::RestoreDefaults);

    initInputFilesTable();
    initOutputFilesTable();
    outputFiles_tw->hide();

    lshConfigAdv_btn->setEnabled(false);

    warning_icon->setPixmap(this->style()->standardIcon(
        QStyle::SP_MessageBoxWarning).pixmap(QSize(40, 40)));
    warning_lbl->setVisible(false);
    warning_icon->setVisible(false);

    connect(start_btn, SIGNAL(clicked()), this, SLOT(finalizeSetup()));
    connect(default_btn, SIGNAL(clicked()), this, SLOT(restoreDefaults()));
    connect(lshConfigAdv_btn, SIGNAL(clicked()), this, SLOT(openAdvSett()));
    connect(addFiles_btn, SIGNAL(clicked()), this, SLOT(addInputFiles()));
    connect(addTemps_btn, SIGNAL(clicked()), this, SLOT(addInputTemps()));
    connect(clear_btn, SIGNAL(clicked()), this, SLOT(clearInputFiles()));

    retranslate();
    setReadOnly(false);
}

/*
 * Public Functions
 */

LSHConfig::LSHConfig(enum HW_VERSION hwv, QWidget *parent) : QDialog(parent), hw_version(hwv), config(hwv)
{
    start_btn = NULL;
    default_btn = NULL;
	advSett = NULL;

    init();
}

LSHConfig::~LSHConfig()
{
    if (start_btn) delete start_btn;
    if (default_btn) delete default_btn;
    if (advSett) delete advSett;
}

bool LSHConfig::checkMargins(const CVBuffer &image, int *marginsH, int *marginsV)
{
    // Margin can't be bigger then half size of shorter side of the picture
    const int step = 1;
    bool changedMargins = false;

    for (int c = 0; c < 2; c++)
    {
        if (marginsH[c] >= image.data.cols / 2)
        {
            marginsH[c] = image.data.cols / 2 - step;
            changedMargins = true;
        }
        if (marginsV[c] >= image.data.rows / 2)
        {
            marginsH[c] = image.data.rows / 2 - step;
            changedMargins = true;
        }
    }

    return changedMargins;
}

lsh_input LSHConfig::getConfig() const
{
    lsh_input retConfig(hw_version);

    retConfig = config;

    // These values we get from widgets
    retConfig.tileSize = tilesize->itemData(tilesize->currentIndex()).toInt();
    retConfig.fitting = algorithm->currentIndex() == 1;

    retConfig.output_prefix = std::string(outputPrefix->text().toLatin1());

    return retConfig;
}

void LSHConfig::setConfig(const lsh_input &newConfig, bool reconfig)
{
    config = newConfig;

    for (int i = 0; i < tilesize->count(); i++)
    {
        if (tilesize->itemData(i).toInt() == newConfig.tileSize)
        {
            tilesize->setCurrentIndex(i);
            break;
        }
    }

    if (newConfig.fitting)
        algorithm->setCurrentIndex(1);
    else
        algorithm->setCurrentIndex(0);

    outputPrefix->setText(QString::fromStdString(newConfig.output_prefix));

    if (!reconfig)
    {
        for (std::list<LSH_TEMP>::const_iterator it = config.outputTemperatures.begin(); it != config.outputTemperatures.end(); it++)
        {
            if (config.inputs.size() && config.inputs.find(*it) != config.inputs.end())
            {
                addInputEntry(*it, QString::fromLatin1((config.inputs.find(*it)->second.imageFilename.c_str())));

                proxyInfo newInfo;
                newInfo.imageFilename = config.inputs.find(*it)->second.imageFilename;
                newInfo.temp = *it;
                newInfo.margins[0] = config.inputs.find(*it)->second.margins[0];
                newInfo.margins[1] = config.inputs.find(*it)->second.margins[1];
                newInfo.margins[2] = config.inputs.find(*it)->second.margins[2];
                newInfo.margins[3] = config.inputs.find(*it)->second.margins[3];
                newInfo.smoothing = config.inputs.find(*it)->second.smoothing;
                newInfo.granularity = config.inputs.find(*it)->second.granularity;
                _proxyInput.push_back(newInfo);
            }
            else
            {
                addInputEntry(*it);

                proxyInfo newInfo;
                newInfo.temp = *it;
                _proxyInput.push_back(newInfo);
            }
        }
        inputFiles_tw->resizeColumnsToContents();

        config.inputs.clear();
        config.outputTemperatures.clear();

        validateInputs();
    }
}

void LSHConfig::setConfig(const lsh_output *output)
{
    warning_icon->setVisible(false);
    warning_lbl->setVisible(false);
    if (output)
    {
        nBits->setValue(output->bitsPerDiff);
        // to ensure it is always displayed set the maximums
        lineSize->setMaximum(output->lineSize);
        lineSize->setValue(output->lineSize);
        matrixSize->setMaximum(output->lineSize*output->channels[0].rows);
        matrixSize->setValue(output->lineSize*output->channels[0].rows);

        rescaled->setValue(output->rescaled);
        wbscale->setValue(1.0 / output->rescaled);

        bool ro = false;

        if (output->outputMSE[0].rows > 0 && output->outputMSE[0].cols > 0)
        {
            cv::Scalar mean, stddev;
            double acc = 0.0;
            double min_v, max_v;

            for (int c = 0; c < 4; c++)
            {
                cv::minMaxLoc(output->outputMSE[c], &(min_v), &(max_v));
                mean = cv::mean(output->outputMSE[c]);

                if (mean[0] > outputMSE[c]->maximum())
                {
                    outputMSE[c]->setPrefix(">");
                }
                else
                {
                    outputMSE[c]->setPrefix("");
                }
                outputMSE[c]->setValue(mean[0]);

                if (min_v > outputMIN[c]->maximum())
                {
                    outputMIN[c]->setPrefix(">");
                }
                else
                {
                    outputMIN[c]->setPrefix("");
                }
                outputMIN[c]->setValue(min_v);

                if (max_v > outputMAX[c]->maximum())
                {
                    outputMAX[c]->setPrefix(">");
                }
                else
                {
                    outputMAX[c]->setPrefix("");
                }
                outputMAX[c]->setValue(max_v);
            }

            ro = true;
        }
        outputMSE_lbl->setVisible(ro);
        outputMin_lbl->setVisible(ro);
        outputMax_lbl->setVisible(ro);
        for (int c = 0; c < 4; c++)
        {
            outputMSE[c]->setVisible(ro);
            outputMIN[c]->setVisible(ro);
            outputMAX[c]->setVisible(ro);
        }
    }
    else
    {
        nBits->setValue(0);
        lineSize->setValue(0);
        matrixSize->setValue(0);
    }
}

void LSHConfig::changeCSS(const QString &css)
{
    _css = css;
    setStyleSheet(css);
}

/*
 * Public Slots
 */

void LSHConfig::retranslate()
{
    Ui::LSHConfig::retranslateUi(this);

    int idx = algorithm->currentIndex();

    algorithm->clear();  // equivalent to lsh_input::fitting
    algorithm->insertItem(0, tr("Direct curve"));
    algorithm->insertItem(1, tr("Fitting curve"));

    if ( idx < 0 )
    {
        idx = 0;
    }
    algorithm->setCurrentIndex(idx);

    idx = tilesize->currentIndex();

    tilesize->clear();
    int i = 0;
    for (int n = LSH_TILE_MIN; n <= LSH_TILE_MAX; n *= 2)
    {
        tilesize->insertItem(i, tr("%n CFA", "", n), QVariant(n));
        i++;
    }

    if ( idx < 0 )
    {
        idx = 0;
    }
    tilesize->setCurrentIndex(idx);

    start_btn->setText(tr("Start"));
    /* need to be done every retranslate or UI message is there
     * instead of the icon */
    warning_icon->setPixmap(this->style()->standardIcon(WARN_ICON_TI)\
        .pixmap(QSize(WARN_ICON_W, WARN_ICON_H)));
}

void LSHConfig::setReadOnly(bool ro)
{
    QAbstractSpinBox::ButtonSymbols btn = QAbstractSpinBox::UpDownArrows;
    if ( ro )
    {
        btn = QAbstractSpinBox::NoButtons;
    }

    outputPrefix_lbl->setVisible(!ro);
    outputPrefix->setVisible(!ro);

    // visible when ro but cannot be changed
    rescaled_lbl->setVisible(ro);
    rescaled->setVisible(ro);
    wbscale_lbl->setVisible(ro);
    wbscale->setVisible(ro);

    nBits_lbl->setVisible(ro);
    nBits->setVisible(ro);
    lineSize_lbl->setVisible(ro);
    lineSize->setVisible(ro);
    matrixSize_lbl->setVisible(ro);
    matrixSize->setVisible(ro);

    outputMSE_lbl->setVisible(ro);
    outputMin_lbl->setVisible(ro);
    outputMax_lbl->setVisible(ro);
    for (int c = 0; c < 4; c++)
    {
        outputMSE[c]->setVisible(ro);
        outputMIN[c]->setVisible(ro);
        outputMAX[c]->setVisible(ro);
    }

    // only way to make it read only for the moment...
    algorithm->setDisabled(ro);
    tilesize->setDisabled(ro);
    buttonBox->setVisible(!ro);
}

void LSHConfig::restoreDefaults()
{
    lsh_input defaults(hw_version);
    setConfig(defaults);
}

void LSHConfig::initInputFilesTable()
{
    QStringList labels;
    labels << "File" << "Temperature" << "Configuration" << "Preview" << "Remove";
    inputFiles_tw->setColumnCount(labels.size());
    inputFiles_tw->setRowCount(0);
    inputFiles_tw->setHorizontalHeaderLabels(labels);
    inputFiles_tw->setEditTriggers(false);
}

void LSHConfig::initOutputFilesTable()
{
    QStringList labels;
    labels << "File" << "Temperature";
    outputFiles_tw->setColumnCount(labels.size());
    outputFiles_tw->setRowCount(0);
    outputFiles_tw->setHorizontalHeaderLabels(labels);
    outputFiles_tw->setEditTriggers(false);
}

void LSHConfig::openAdvSett(bool adv)
{
    if (advSett)
    {
        disconnect(advSett);
        advSett->deleteLater();
    }
    advSett = new LSHConfigAdv(hw_version);
    advSett->setStyleSheet(_css);

	if (adv)
	{
		advSett->fileConfig_gb->hide();

		advSett->maxMatrixValue_sb->setValue(config.maxMatrixValue);
		advSett->maxMatrixValueUsed_cb->setChecked(config.bUseMaxValue);
		advSett->minMatrixValue_sb->setValue(config.minMatrixValue);
		advSett->minMatrixValueUsed_cb->setChecked(config.bUseMinValue);
		advSett->maxLineSize_sb->setValue(config.maxLineSize);
		advSett->maxLineSize_cb->setChecked(config.bUseMaxLineSize);
		advSett->maxBitDiff_sb->setValue(config.maxBitDiff);
		advSett->maxBitDiff_cb->setChecked(config.bUseMaxBitsPerDiff);

		if (advSett->exec() == QDialog::Accepted)
		{
			config.maxMatrixValue = advSett->maxMatrixValue_sb->value();
			config.bUseMaxValue = advSett->maxMatrixValueUsed_cb->isChecked();
			config.minMatrixValue = advSett->minMatrixValue_sb->value();
			config.bUseMinValue = advSett->minMatrixValueUsed_cb->isChecked();
			config.maxLineSize = advSett->maxLineSize_sb->value();
			config.bUseMaxLineSize = advSett->maxLineSize_cb->isChecked();
			config.maxBitDiff = advSett->maxBitDiff_sb->value();
			config.bUseMaxBitsPerDiff = advSett->maxBitDiff_cb->isChecked();
		}
	}
	else
	{
		advSett->advParams_gb->hide();

		connect(advSett->marginLeft_sb, SIGNAL(valueChanged(int)), this,
			SLOT(advSettPreview()));
		connect(advSett->marginRight_sb, SIGNAL(valueChanged(int)), this,
			SLOT(advSettPreview()));
		connect(advSett->marginTop_sb, SIGNAL(valueChanged(int)), this,
			SLOT(advSettPreview()));
		connect(advSett->marginBottom_sb, SIGNAL(valueChanged(int)), this,
			SLOT(advSettPreview()));
		connect(advSett->color_cb, SIGNAL(currentIndexChanged(int)), this,
			SLOT(advSettPreview()));

        unsigned int i = 0;
        for (i = 0; i < _proxyInput.size(); i++)
        {
            if (_proxyInput[i].imageFilename == std::string(currentFileName.toLatin1()).c_str())
            {
                advSett->marginLeft_sb->setValue(_proxyInput[i].margins[0]);
                advSett->marginTop_sb->setValue(_proxyInput[i].margins[1]);
                advSett->marginRight_sb->setValue(_proxyInput[i].margins[2]);
                advSett->marginBottom_sb->setValue(_proxyInput[i].margins[3]);
                advSett->smoothing_sb->setValue(_proxyInput[i].smoothing);
                break;
            }
        }

		advSettPreview();

		if (advSett->exec() == QDialog::Accepted)
		{
            _proxyInput[i].margins[0] = advSett->marginLeft_sb->value();
            _proxyInput[i].margins[1] = advSett->marginTop_sb->value();
            _proxyInput[i].margins[2] = advSett->marginRight_sb->value();
            _proxyInput[i].margins[3] = advSett->marginBottom_sb->value();
            _proxyInput[i].smoothing = advSett->smoothing_sb->value();
		}
	}
}

void LSHConfig::advSettPreview()
{
    QString path = currentFileName;
    if (path.isEmpty())
    {
        QtExtra::Exception ex(tr("Empty filename"));
        ex.displayQMessageBox(this);
    }
    CVBuffer sBuffer;

    try
    {
        sBuffer = CVBuffer::loadFile(path.toLatin1());
    }
    catch(QtExtra::Exception ex)
    {
        ex.displayQMessageBox(this);
    }

    if (!advSett)
    {
        return;
    }

    // Display image
    advSett->pImageView->clearButImage();
    advSett->pImageView->loadBuffer(sBuffer);

    int marginsH[2], marginsV[2];
    marginsH[0] = advSett->marginLeft_sb->value();
    marginsH[1] = advSett->marginRight_sb->value();
    marginsV[0] = advSett->marginTop_sb->value();
    marginsV[1] = advSett->marginBottom_sb->value();

    if (checkMargins(sBuffer, marginsH, marginsV))
    {
        advSett->marginLeft_sb->setValue(marginsH[0]);
        advSett->marginRight_sb->setValue(marginsH[1]);
        advSett->marginTop_sb->setValue(marginsV[0]);
        advSett->marginBottom_sb->setValue(marginsV[1]);
        return;
    }

    // Display margin rect
    QPen pen((Qt::GlobalColor)advSett->color_cb->itemData(
        advSett->color_cb->currentIndex()).toInt());
    QGraphicsRectItem *imgMargin = new QGraphicsRectItem(
        marginsH[0],
        marginsV[0],
        sBuffer.data.cols - (marginsH[0] + marginsH[1]),
        sBuffer.data.rows - (marginsV[0] + marginsV[1]));
    imgMargin->setPen(pen);
    advSett->pImageView->getScene()->addItem(imgMargin);
}

void LSHConfig::addInputFiles()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setNameFilter(tr("Files (*.flx)"));

	QStringList fileNames;
	if (dialog.exec())
	{
		fileNames = dialog.selectedFiles();
	}

	if (fileNames.empty())
	{
		return;
	}

    for (int i = 0; i < fileNames.size(); i++)
    {
        proxyInfo newInfo;
        newInfo.imageFilename = std::string(fileNames[i].toLatin1()).c_str();
        newInfo.temp = 0;
        _proxyInput.push_back(newInfo);

        addInputEntry(0, fileNames[i]);
    }
    inputFiles_tw->resizeColumnsToContents();

    sortIputs();
}

void LSHConfig::addInputTemps()
{
	unsigned int temperature;

	bool ok;
	temperature = QInputDialog::getInt(this, tr("Temperature"), tr("Temperature:"), 0, 0, 100000, 1, &ok);
	if (!ok)
	{
		return;
	}

    addInputEntry(temperature);

    inputFiles_tw->resizeColumnsToContents();

    proxyInfo newInfo;
    newInfo.temp = temperature;
    _proxyInput.push_back(newInfo);

	sortIputs();
}

void LSHConfig::addInputEntry(int temperature, const QString &filename)
{
    inputFiles_tw->insertRow(inputFiles_tw->rowCount());
    inputFiles_tw->setItem(inputFiles_tw->rowCount() - 1, INPUT_ENRTY::FILE, new QTableWidgetItem(filename));
    QSpinBox *temp = new QSpinBox();
    temp->setRange(0, 100000);
    temp->setButtonSymbols(QAbstractSpinBox::NoButtons);
    temp->setAlignment(Qt::AlignHCenter);
    temp->setValue(temperature);
    connect(temp, SIGNAL(editingFinished()), this, SLOT(sortIputs()));
    inputFiles_tw->setCellWidget(inputFiles_tw->rowCount() - 1, INPUT_ENRTY::TEMP, temp);
    if (!filename.isEmpty())
    {
        QPushButton *config = new QPushButton("Configure");
        config->setEnabled(false);
        connect(config, SIGNAL(clicked()), this, SLOT(configureInputFile()));
        inputFiles_tw->setCellWidget(inputFiles_tw->rowCount() - 1, INPUT_ENRTY::CONFIG, config);
        QPushButton *preview = new QPushButton("Preview");
        connect(preview, SIGNAL(clicked()), this, SLOT(previewInputFile()));
        inputFiles_tw->setCellWidget(inputFiles_tw->rowCount() - 1, INPUT_ENRTY::PREVIEW, preview);
    }
    QPushButton *remove = new QPushButton("Remove");
    connect(remove, SIGNAL(clicked()), this, SLOT(removeInputFile()));
    inputFiles_tw->setCellWidget(inputFiles_tw->rowCount() - 1, INPUT_ENRTY::REMOVE, remove);
}

void LSHConfig::removeInputFile()
{
    for (int i = 0; i < inputFiles_tw->rowCount(); i++)
    {
        if ((QPushButton *)inputFiles_tw->cellWidget(i, INPUT_ENRTY::REMOVE) == (QPushButton *)sender())
        {
            _proxyInput.erase(_proxyInput.begin() + i);
            inputFiles_tw->removeRow(i);
            break;
        }
    }

    validateInputs();
}

void LSHConfig::clearInputFiles()
{
    for (int i = inputFiles_tw->rowCount() - 1; i >= 0; i--)
    {
        inputFiles_tw->removeRow(i);
    }
    _proxyInput.clear();

    validateInputs();
}

void LSHConfig::sortIputs()
{
    // Update temp in _proxyInput
    for (int i = 0; i < inputFiles_tw->rowCount(); i++)
    {
        if (((QSpinBox *)inputFiles_tw->cellWidget(i, 1)) == sender())
        {
            _proxyInput[i].temp = ((QSpinBox *)inputFiles_tw->cellWidget(i, 1))->value();
            break;
        }
    }

    // Disconnect signals
    for (int i = 0; i < inputFiles_tw->rowCount(); i++)
    {
        disconnect((QSpinBox *)inputFiles_tw->cellWidget(i, 1), SIGNAL(editingFinished()), this, SLOT(sortIputs()));
    }

    // Remove all entries
    initInputFilesTable();

    std::sort(_proxyInput.begin(), _proxyInput.end(), [](proxyInfo a, proxyInfo b){ return a.temp < b.temp; });

    // Readd all entries
    for (unsigned int i = 0; i < _proxyInput.size(); i++)
    {
        addInputEntry(_proxyInput[i].temp, QString::fromLatin1(_proxyInput[i].imageFilename.c_str()));
    }
    inputFiles_tw->resizeColumnsToContents();

    validateInputs();

    // Reconnect signals
    for (int i = 0; i < inputFiles_tw->rowCount(); i++)
    {
        connect((QSpinBox *)inputFiles_tw->cellWidget(i, 1), SIGNAL(editingFinished()), this, SLOT(sortIputs()));
    }
}

bool LSHConfig::validateInputs(bool showError)
{
    // If input table empty fail
    if (!inputFiles_tw->rowCount())
    {
        enableControls(false);
        return false;
    }

    // First item and last item must be file input
    if (inputFiles_tw->item(0, 0)->text().isEmpty() ||
        inputFiles_tw->item(inputFiles_tw->rowCount() - 1, 0)->text().isEmpty())
    {
        info_lbl->setText("First item and last item must be file input!");
        enableControls(false);
        if (showError)
        {
            QMessageBox::critical(this, tr("Error"), QStringLiteral("First item and last item must be file input!"));
        }
        return false;
    }
    else
    {
        info_lbl->setText("");
    }

	// Validate temeperatures
	std::map<int, int> tmp;
	for (int i = 0; i < inputFiles_tw->rowCount(); i++)
	{
		if (tmp.size() && tmp.find(((QSpinBox *)inputFiles_tw->cellWidget(i, 1))->value()) != tmp.end())
		{
            info_lbl->setText("Temperatures must not be duplicated!");
            enableControls(false);
            if (showError)
            {
                QMessageBox::critical(this, tr("Error"), QStringLiteral("Duplicated temperatures detected!"));
            }
			return false;
		}
		else
		{
			tmp.insert(std::pair<int, int>(((QSpinBox *)inputFiles_tw->cellWidget(i, 1))->value(), 0));
		}
	}

	// Validate files sizes
	/*const char *err;
	CImageFlx flx;
	int fileSize = -1;
	for (int i = 0; i < inputFiles_tw->rowCount(); i++)
	{
		if (inputFiles_tw->item(i, 0)->text().isEmpty())
		{
			continue;
		}

		if ((err = flx.LoadFileHeader(std::string(inputFiles_tw->item(i, 0)->text().toLatin1()).c_str(), NULL)) != NULL)
		{
			QMessageBox::critical(this, tr("Failed to load FLX header"), QString(err));
			return false;
		}

		if (fileSize < 0)
		{
			fileSize = flx.chnl[0].chnlHeight * flx.chnl[0].chnlWidth;
			continue;
		}
		else
		{
			if (fileSize != flx.chnl[0].chnlHeight * flx.chnl[0].chnlWidth)
			{
				QMessageBox::critical(this, tr("Error"), QStringLiteral("Loaded!"));
				return false;
			}
		}
	}*/

    // Validation OK
    info_lbl->setText("");
    enableControls(true);

	return true;
}

void LSHConfig::enableControls(bool opt)
{
    for (int i = 0; i < inputFiles_tw->rowCount(); i++)
    {
        QPushButton *btn = ((QPushButton *)inputFiles_tw->cellWidget(i, 2));
        if (btn)
        {
            btn->setEnabled(opt);
        }
    }
    start_btn->setEnabled(opt);
    lshConfigAdv_btn->setEnabled(start_btn->isEnabled());
}

void LSHConfig::configureInputFile()
{
	for (int i = 0; i < inputFiles_tw->rowCount(); i++)
	{
        if ((QPushButton *)inputFiles_tw->cellWidget(i, 2) == (QPushButton *)sender())
        {
            currentFileName = inputFiles_tw->item(i, 0)->text();
            openAdvSett(false);
            return;
        }
	}
}

void LSHConfig::previewInputFile()
{
	QString path;

	for (int i = 0; i < inputFiles_tw->rowCount(); i++)
	{
		if (inputFiles_tw->cellWidget(i, 3) == (QPushButton *)sender())
		{
			path = inputFiles_tw->item(i, 0)->text();
			break;
		}
	}

	if (path.isEmpty())
	{
		QtExtra::Exception ex(tr("Empty filename"));
		ex.displayQMessageBox(this);
		return;
	}

	CVBuffer sBuffer;

	try
	{
		sBuffer = CVBuffer::loadFile(path.toLatin1());
	}
	catch (QtExtra::Exception ex)
	{
		ex.displayQMessageBox(this);
		return;
	}

	QDialog *pDiag = new QDialog(this);
	pDiag->setWindowTitle(tr("LSH input preview"));

	QGridLayout *pLay = new QGridLayout();

	ImageView *pView = new ImageView();
	pView->loadBuffer(sBuffer);
	pView->menubar->hide();  // Don't need menu bar here
	pView->showDPF_cb->hide();  // No DPF viewed here
	pLay->addWidget(pView);

	pDiag->setLayout(pLay);
	pDiag->resize(this->size());
	pDiag->exec();
}

void LSHConfig::fill_lsh_input()
{
    for (unsigned int i = 0; i < _proxyInput.size(); i++)
    {
        if (!_proxyInput[i].imageFilename.empty())
        {
            config.addInput(
                _proxyInput[i].temp,
                _proxyInput[i].imageFilename,
                _proxyInput[i].margins[0],
                _proxyInput[i].margins[1],
                _proxyInput[i].margins[2],
                _proxyInput[i].margins[3],
                _proxyInput[i].smoothing,
                _proxyInput[i].granularity);
        }
        else
        {
            config.outputTemperatures.push_back(_proxyInput[i].temp);
        }
    }
}

void LSHConfig::finalizeSetup()
{
    sortIputs();
    fill_lsh_input();

    accept();
}

