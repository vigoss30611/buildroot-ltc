/**
******************************************************************************
@file lshtuning.cpp

@brief LSHTuning implementation

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
#include "lsh/tuning.hpp"

#include <felix_hw_info.h>  // LSH_VERTEX size info
#include <QGraphicsRectItem>

#include <fstream>
#include <iostream>
#include <string>
#include <QtExtra/exception.hpp>

#include "genconfig.hpp"  // NO_LINT
#include "lsh/config.hpp"
#include "lsh/compute.hpp"
#include "lsh/configadv.hpp"
#include "ispc/ModuleLSH.h"
#include "ispc/ModuleBLC.h"
#include "ispc/ParameterFileParser.h"

std::ostream& operator<<(std::ostream &os, const lsh_input &in)
{
    os << "image: " << in.imageFilename << std::endl
        << "tile size: " << in.tileSize << std::endl
        << "algorithm: ";
    if (in.fitting)
    {
        os << "fitting";
    }
    else
    {
        os << "direct";
    }
    os << std::endl
        << "margins: "
        << in.margins[0] << ", "
        << in.margins[1] << ", "
        << in.margins[2] << ", "
        << in.margins[3]
        << std::endl
        << "black level: "
        << in.blackLevel[0] << ", "
        << in.blackLevel[1] << ", "
        << in.blackLevel[2] << ", "
        << in.blackLevel[3] << std::endl
        << "granularity: " << in.granularity << std::endl
        << "smoothing: " << in.smoothing << std::endl
        << "max gain: ";
    if (in.bUseMaxValue)
    {
        os << in.maxMatrixValue;
    }
    else
    {
        os << "none";
    }
    os << std::endl
        << "max line size: ";
    if (in.bUseMaxLineSize)
    {
        os << in.maxLineSize << "B";
    }
    else
    {
        os << "none";
    }
    os << std::endl
        << "max bit per diff: ";
    if (in.bUseMaxBitsPerDiff)
    {
        os << in.maxBitDiff << "b";
    }
    else
    {
        os << "none";
    }
    os << std::endl;

    return os;
}

/*
 * lsh_input
 */

lsh_input::lsh_input()
{
    int c;
    imageFilename = "";
    tileSize = LSH_TILE_MAX;
    granularity = 8;
    smoothing = 0.0001f;
    for (c = 0; c < 4; c++)
    {
        margins[c] = 0;
        blackLevel[c] = 0;
    }
    fitting = true;

    // the maximum value for the gain is the size of the accumulator -1
    unsigned int m = (1 <<
        (LSH_VERTEX_INT + LSH_VERTEX_FRAC - LSH_VERTEX_SIGNED)) - 1;
    // but the number is a fixed point
    maxMatrixValue = static_cast<double>(m) / (1 << LSH_VERTEX_FRAC);
    bUseMaxValue = true;
    maxLineSize = SYSMEM_ALIGNMENT;  // 512b
    bUseMaxLineSize = true;
    maxBitDiff = LSH_DELTA_BITS_MAX;
}

/*
 * LSHTuning
 */

/*
 * private
 */

void LSHTuning::init()
{
    Ui::LSHTuning::setupUi(this);

    gridView = new LSHGridView();

    connect(reset_btn, SIGNAL(clicked()), this, SLOT(reset()));
    connect(save_btn, SIGNAL(clicked()), this, SLOT(saveResult()));

    params = new LSHConfig();
    // don`t want "Advance Settings" button in LSH tab
    params->lshConfigAdv_btn->hide();
    param_grp_lay->addWidget(params);
    params->setReadOnly(true);

    blc_sensor0->setDisabled(true);
    blc_sensor1->setDisabled(true);
    blc_sensor2->setDisabled(true);
    blc_sensor3->setDisabled(true);
    blc_sensor0->setRange(ISPC::ModuleBLC::BLC_SENSOR_BLACK.min,
        ISPC::ModuleBLC::BLC_SENSOR_BLACK.max);
    blc_sensor1->setRange(ISPC::ModuleBLC::BLC_SENSOR_BLACK.min,
        ISPC::ModuleBLC::BLC_SENSOR_BLACK.max);
    blc_sensor2->setRange(ISPC::ModuleBLC::BLC_SENSOR_BLACK.min,
        ISPC::ModuleBLC::BLC_SENSOR_BLACK.max);
    blc_sensor3->setRange(ISPC::ModuleBLC::BLC_SENSOR_BLACK.min,
        ISPC::ModuleBLC::BLC_SENSOR_BLACK.max);

    params->setDisabled(true);

    progress->setVisible(false);
    save_btn->setEnabled(false);

    pOutput = 0;

    warning_icon->setPixmap(this->style()->standardIcon(WARN_ICON_TW)\
        .pixmap(QSize(WARN_ICON_W, WARN_ICON_H)));
    warning_lbl->setVisible(false);
    warning_icon->setVisible(false);

    advSettLookUp_btn->setEnabled(false);

    connect(advSettLookUp_btn, SIGNAL(clicked()), this,
        SLOT(advSettLookUp()));
    connect(saveError_btn, SIGNAL(clicked()), this,
        SLOT(saveTargetMSE()));

    if (_isVLWidget)
    {
        integrate_btn->setEnabled(false);
        save_btn->hide();
    }
    else
    {
        integrate_btn->hide();
    }
}

/*
 * public
 */

LSHTuning::LSHTuning(GenConfig *genConfig, bool isVLWidget, QWidget *parent)
    : VisionModule(parent)
{
    params = NULL;
    gridView = NULL;
    advSett = NULL;

    _isVLWidget = isVLWidget;

    this->genConfig = genConfig;
    init();
}

LSHTuning::~LSHTuning()
{
    // genConfig is now owned by this object!
    if (pOutput)
    {
        delete pOutput;
    }
}

QString LSHTuning::getOutputFilename()
{
    return params->outputFilename->text();
}

QWidget *LSHTuning::getDisplayWidget()
{
    return this;
}

QWidget *LSHTuning::getResultWidget()
{
    return gridView;
}

void LSHTuning::changeCSS(const QString &css)
{
    _css = css;

    setStyleSheet(css);
}

/*
 * public slots
 */

QString LSHTuning::checkDependencies() const
{
    blc_config blc = genConfig->getBLC();
    QString mess;

    if (!pOutput)
    {
        mess += tr("LSH calibration was not run\n");
        return mess;
    }

    for (int c = 0; c < 4; c++)
    {
        if (inputs.blackLevel[c] != blc.sensorBlack[c])
        {
            mess += tr("BLC sensor black level values changed since LSH "\
                "was computed\n");
            break;
        }
    }

    if (pOutput && !mess.isEmpty())
    {
        warning_lbl->setText(mess);
        warning_lbl->setVisible(true);
        warning_icon->setVisible(true);
    }
    else
    {
        warning_lbl->setVisible(false);
        warning_icon->setVisible(false);
    }

    return mess;
}

int LSHTuning::saveResult(const QString &_filename)
{
    QString filename;
    int r;

    if (_filename.isEmpty())
    {
        filename = QFileDialog::getSaveFileName(this,
            tr("Output FelixArgs file"), QString(), tr("*.txt"));

        /* on linux the filename may not have .txt - disabled because it
         * does not warn if the file is already present */
        /*int len = filename.count();
        if ( !(filename.at(len-4) == '.' && filename.at(len-3) == 't'
        && filename.at(len-2) == 'x' && filename.at(len-1) == 't')  )
        {
        filename += ".txt";
        }*/
    }
    else
    {
        filename = _filename;
    }

    if (filename.isEmpty())
    {
        return EXIT_FAILURE;
    }

    r = saveMatrix(QFileInfo(filename).dir().absolutePath());
    if (EXIT_SUCCESS != r)
    {
        return EXIT_FAILURE;
    }

    r = saveTargetMSE(QFileInfo(filename).dir().absolutePath() + "_error.txt");
    if (EXIT_SUCCESS != r)
    {
        return EXIT_FAILURE;
    }

    ISPC::ParameterList res;
    try
    {
        IMG_RESULT ret;
        saveParameters(res);

        ret = ISPC::ParameterFileParser::saveGrouped(res,
            std::string(filename.toLatin1()));
        if (ret != IMG_SUCCESS)
        {
            throw(QtExtra::Exception(tr("Failed to save FelixParameter "\
                "file with LSH parameters")));
        }
    }
    catch (QtExtra::Exception e)
    {
        e.displayQMessageBox(this);
        return EXIT_FAILURE;
    }

    QString mess = "LSH: saving parameters in " + filename + "\n";
    emit logMessage(mess);

    return EXIT_SUCCESS;
}

int LSHTuning::saveMatrix(const QString &dirPath)
{
    if (dirPath.isEmpty())
    {
        QtExtra::Exception ex(tr("Empty directory path to save matrix"));
        ex.displayQMessageBox(this);
        return EXIT_FAILURE;
    }
    if (!QFileInfo(dirPath).isDir())
    {
        QtExtra::Exception ex(tr("Given path is not a directory"));
        ex.displayQMessageBox(this);
        return EXIT_FAILURE;
    }

    QString lshPath = dirPath;
    lsh_input config = params->getConfig();
    QString lshFilename = params->outputFilename->text();

    QRegExpValidator outputValidator;
    QRegExp exp("\\w+(\\.lsh)+");
    exp.setCaseSensitivity(Qt::CaseInsensitive);
    outputValidator.setRegExp(exp);

    lshPath += "/" + lshFilename;

    int pos;
    if (lshFilename.isEmpty()
        || outputValidator.validate(lshFilename, pos)
        != QValidator::Acceptable)
    {
        QtExtra::Exception ex(tr("Invalid output filename (format: *.lsh): ",
            "followed by filename") + lshFilename);
        ex.displayQMessageBox(this);
        return EXIT_FAILURE;
    }
    if (!pOutput)
    {
        QtExtra::Exception ex(tr("No results to save"));
        ex.displayQMessageBox(this);
        return EXIT_FAILURE;
    }

    LSHCompute::writeLSHMatrix(pOutput->channels, config.tileSize,
        std::string(lshPath.toLatin1()));

    QString mess = "LSH: saving matrix in " + lshPath + "\n";
    emit logMessage(mess);

    return EXIT_SUCCESS;
}

int LSHTuning::saveTargetMSE(const QString &dirFilename)
{
    QString filename(dirFilename);

    if (!pOutput)
    {
        // nothing to save
        return EXIT_FAILURE;
    }

    if (filename.isEmpty())
    {
        filename = QFileDialog::getSaveFileName(this,
            tr("Output Error file"), QString(), tr("*.txt"));
    }

    if (filename.isEmpty())
    {
        return EXIT_FAILURE;
    }

    std::ofstream file(filename.toLatin1().data(), std::ofstream::out);

    if (file.is_open())
    {
        file << "Output error for LSH calibration" << std::endl
            << inputs
            << std::endl;

        for (int c = 0; c < 4; c++)
        {
            file << "channel " << c << std::endl;

            for (int y = 0; y < pOutput->outputMSE[c].rows; y++)
            {
                for (int x = 0; x < pOutput->outputMSE[c].cols; x++)
                {
                    file << " " << pOutput->outputMSE[c].at<float>(y, x);
                }
                file << std::endl;
            }
            file << std::endl;
        }

        file.close();
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

void LSHTuning::saveParameters(ISPC::ParameterList &list) const
throw(QtExtra::Exception)
{
    lsh_input config;
    std::ostringstream os;
    std::string calibInfo = "//LSH ";
    int i;

    if (pOutput == 0)
    {
        throw(QtExtra::Exception(tr("No results to save for LSH")));
    }

    config = params->getConfig();

    ISPC::Parameter file(ISPC::ModuleLSH::LSH_FILE.name,
        std::string(params->outputFilename->text().toLatin1()));
    list.addParameter(file);

    ISPC::Parameter enable(ISPC::ModuleLSH::LSH_MATRIX.name, "0");
    list.addParameter(enable);

    ISPC::Parameter inputImage(calibInfo + "input image",
        this->inputs.imageFilename);
    list.addParameter(inputImage);

    os.str("");
    os << calibInfo << "using margins (CFA)";
    for (i = 0; i < 4; i++)
    {
        os << " " << inputs.margins[i];
    }
    os << std::endl;

    if (inputs.bUseMaxLineSize)
    {
        ISPC::Parameter maxline(calibInfo + "max line size (bytes)",
            std::string(QString::number(inputs.maxLineSize).toLatin1()));
        list.addParameter(maxline);
    }
    else
    {
        ISPC::Parameter maxline(calibInfo + "max line size",
            std::string("not used"));
        list.addParameter(maxline);
    }

    if (inputs.bUseMaxBitsPerDiff)
    {
        ISPC::Parameter maxline(calibInfo + "max bits per diff",
            std::string(QString::number(inputs.maxBitDiff).toLatin1()));
        list.addParameter(maxline);
    }
    else
    {
        ISPC::Parameter maxline(calibInfo + "max bits per diff",
            std::string("not used"));
        list.addParameter(maxline);
    }

    for (i = 0; i < 4; i++)
    {
        os << inputs.blackLevel[i] << " ";
    }
    ISPC::Parameter blcSensorBlack(calibInfo
        + ISPC::ModuleBLC::BLC_SENSOR_BLACK.name + " used", os.str());
    list.addParameter(blcSensorBlack);

    list.addGroup("LSH", ISPC::ModuleLSH::getGroup());
}

int LSHTuning::tuning()
{
    if (this->inputs.imageFilename.empty()
        || this->params->outputFilename->text().isEmpty())
    {
        return EXIT_FAILURE;
    }

    LSHCompute *pCompute = new LSHCompute();

    connect(this, SIGNAL(compute(const lsh_input*)),
        pCompute, SLOT(compute(const lsh_input*)));

    connect(pCompute, SIGNAL(finished(LSHCompute *, lsh_output *)),
        this, SLOT(computationFinished(LSHCompute*, lsh_output *)));
    connect(pCompute, SIGNAL(currentStep(int)),
        progress, SLOT(setValue(int)));
    connect(pCompute, SIGNAL(logMessage(QString)),
        this, SIGNAL(logMessage(QString)));

    progress->setMaximum(LSH_N_STEPS);
    progress->setValue(0);
    progress->setVisible(true);

    save_btn->setEnabled(false);

    warning_icon->setVisible(false);
    warning_lbl->setVisible(false);

    emit compute(&(this->inputs));
    emit changeCentral(getDisplayWidget());
    emit changeResult(getResultWidget());

    if (_isVLWidget)
    {
        gridLayout->addWidget(gridView, 0, gridLayout->columnCount(),
            gridLayout->rowCount(), 1);
    }

    return EXIT_SUCCESS;
}

int LSHTuning::reset()
{
    if (progress->isVisible())  // it's running
    {
        return EXIT_FAILURE;
    }
#if 1
    {
        LSHConfig sconf;

        if (_isVLWidget)
        {
            sconf.changeCSS(_css);
        }

        sconf.setConfig(this->inputs);

        if (sconf.exec() == QDialog::Accepted)
        {
            this->inputs = sconf.getConfig();
            blc_config blc = genConfig->getBLC();
            for (int i = 0; i < 4; i++)
            {
                this->inputs.blackLevel[i] = blc.sensorBlack[i];
            }

            // to display parameters while running
            params->setConfig(this->inputs);
            this->params->setConfig(0);
            params->outputFilename->setText(sconf.outputFilename->text());

            blc_sensor0->setValue(inputs.blackLevel[0]);
            blc_sensor1->setValue(inputs.blackLevel[1]);
            blc_sensor2->setValue(inputs.blackLevel[2]);
            blc_sensor3->setValue(inputs.blackLevel[3]);

            blc_sensor0->setDisabled(false);
            blc_sensor1->setDisabled(false);
            blc_sensor2->setDisabled(false);
            blc_sensor3->setDisabled(false);
            params->setDisabled(false);

            advSettLookUp_btn->setEnabled(true);
        }
        else
        {
            return EXIT_FAILURE;
        }
    }
#endif
    this->tuning();
    return EXIT_SUCCESS;
}

void LSHTuning::computationFinished(LSHCompute *toStop, lsh_output *pResult)
{
    bool allowSaving = true;
    if (inputs.bUseMaxLineSize
        && pResult->lineSize > inputs.maxLineSize)
    {
        QString mess = tr("Computed grid with tile size %n does "\
            "not fit the line size limit.",
            "m1, followed by m2",
            inputs.tileSize);
        QString details = tr("Expected line size %n Byte(s)",
            "d1 followed by d2", pResult->lineSize)
            + tr(" while maximum is %n Byte(s).",
            "d2, leading space important",
            inputs.maxLineSize);

        if (inputs.tileSize < LSH_TILE_MAX)
        {
            int curr = inputs.tileSize;
            inputs.tileSize = inputs.tileSize << 1;

            QtExtra::Exception ex(mess +
                tr(" Grid will be recomputed with a bigger "\
                "tile size %n.",
                "m2, leading space important",
                inputs.tileSize),
                details,
                true);
            ex.displayQMessageBox(this);

            // update what we changed
            this->params->setConfig(this->inputs);
            // reset outputs
            this->params->setConfig(0);

            emit compute(&(this->inputs));

            return;
        }
        else
        {
            QtExtra::Exception ex(mess +
                tr(" This is already the maximum tile size - the grid needs "\
                "manual modification to support the selected maximum line "\
                "size of %n Byte(s)", "", inputs.maxLineSize),
                details);

            ex.displayQMessageBox(this);

            allowSaving = false;
        }
    }

    toStop->deleteLater();
    progress->setVisible(false);
    save_btn->setEnabled(allowSaving);

    if (pOutput)
    {
        delete pOutput;
    }
    this->pOutput = pResult;
    gridView->loadData(pResult, &inputs);

    this->params->setConfig(pOutput);

    // should have been done when starting
    /*blc_sensor0->setValue( inputs.blackLevel[0] );
    blc_sensor1->setValue( inputs.blackLevel[1] );
    blc_sensor2->setValue( inputs.blackLevel[2] );
    blc_sensor3->setValue( inputs.blackLevel[3] );

    blc_sensor0->setDisabled(false);
    blc_sensor1->setDisabled(false);
    blc_sensor2->setDisabled(false);
    blc_sensor3->setDisabled(false);

    params->setDisabled(false);
    */

    if (_isVLWidget)
    {
        integrate_btn->setEnabled(true);
        outFile = params->outputFilename->text();
        params->outputFilename->hide();
        params->output_lbl->hide();
    }
}

void LSHTuning::retranslate()
{
    Ui::LSHTuning::retranslateUi(this);
}

// Displays advance settings lookup window
void LSHTuning::advSettLookUp()
{
    advSett = new LSHConfigAdv();

    if (_isVLWidget)
    {
        advSett->setStyleSheet(_css);
    }

    advSett->setConfig(inputs);  // Set configuration
    advSett->buttonBox->hide();  // Don't need OK/Cancel buttons

    // Disableing all that shloudn't be changed here
    advSett->setReadOnly(true);

    advSettLookUpPreview();  // View image with margin

    connect(advSett->color_cb, SIGNAL(currentIndexChanged(int)), this,
        SLOT(advSettLookUpPreview()));

    advSett->exec();
}

// Displays advance settings lookup image preview
void LSHTuning::advSettLookUpPreview()
{
    // Load the image file
    QString path = params->inputFile->absolutePath();
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
    catch (QtExtra::Exception ex)
    {
        ex.displayQMessageBox(this);
    }

    // Display image
    advSett->pImageView->clearButImage();
    advSett->pImageView->loadBuffer(sBuffer);

    // Margin can't be bigger then half size of shorter side of the picture
    int marginsH[2], marginsV[2];
    marginsH[0] = advSett->marginLeft_sb->value();
    marginsH[1] = advSett->marginRight_sb->value();
    marginsV[0] = advSett->marginTop_sb->value();
    marginsV[1] = advSett->marginBottom_sb->value();

    if (LSHConfig::checkMargins(sBuffer, marginsH, marginsV))
    {
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

