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

#include <QGraphicsRectItem>

#include <fstream>
#include <iostream>
#include <string>
#include <QFileDialog>
#include <QtExtra/exception.hpp>

#include "genconfig.hpp"  // NO_LINT
#include "lsh/config.hpp"
#include "lsh/compute.hpp"
#include "lsh/configadv.hpp"
#include "lsh/gridview.hpp"
#include "ispc/ModuleLSH.h"
#include "ispc/ControlLSH.h"
#include "ispc/ModuleBLC.h"
#include "ispc/ParameterFileParser.h"

#include <felix_hw_info.h>  // to get compiled hw version
#define HW_2_X_LSH_VERTEX_INT (2)
#define HW_2_X_LSH_VERTEX_FRAC (10)
#define HW_2_X_LSH_VERTEX_SIGNED (0)
#define HW_2_X_MAX_BIT_DIFF (10)

#define HW_2_6_LSH_VERTEX_INT (3)
#define HW_2_6_LSH_VERTEX_FRAC (10)
#define HW_2_6_LSH_VERTEX_SIGNED (0)
#define HW_2_6_MAX_BIT_DIFF (10)

#if !defined(NDEBUG)

#if FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN < 6
#if HW_2_X_LSH_VERTEX_INT != LSH_VERTEX_INT \
    || HW_2_X_LSH_VERTEX_FRAC != LSH_VERTEX_FRAC \
    || HW_2_X_LSH_VERTEX_SIGNED != LSH_VERTEX_SIGNED
#error wrong value for HW 2.X
#endif
#else
#if HW_2_6_LSH_VERTEX_INT != LSH_VERTEX_INT \
    || HW_2_6_LSH_VERTEX_FRAC != LSH_VERTEX_FRAC \
    || HW_2_6_LSH_VERTEX_SIGNED != LSH_VERTEX_SIGNED
#error wrong value for HW 2.X
#endif
#endif

#endif

std::ostream& operator<<(std::ostream &os, const lsh_input &in)
{
    switch (in.hwVersion)
    {
    case HW_2_X:
        os << "HW: 2.x";
        break;

    case HW_2_4:
        os << "HW: 2.4";
        break;

    case HW_2_6:
        os << "HW: 2.6";
        break;

    case HW_3_X:
        os << "HW: 3.x";
        break;

    case HW_UNKNOWN:
    default:
        os << "HW: unkown";
    }
    os << std::endl << " lsh hw accumulator:";
    if (in.vertex_signed)
    {
        os << " s" << in.vertex_int << "." << in.vertex_frac;
    }
    else
    {
        os << " u" << in.vertex_int << "." << in.vertex_frac;
    }
    os << std::endl;

    lsh_input::const_iterator it;
    for (it = in.inputs.begin(); it != in.inputs.end(); it++)
    {
        os << "image " << it->first << " " << it->second.imageFilename
            << std::endl;
        os << "  margins "
            << it->second.margins[0] << ", "
            << it->second.margins[1] << ", "
            << it->second.margins[2] << ", "
            << it->second.margins[3] << std::endl;
        os << "  granularity " << it->second.granularity << std::endl;
        os << "  smoothing " << it->second.smoothing << std::endl;
    }
	std::list<LSH_TEMP>::const_iterator ot;
	for (ot = in.outputTemperatures.begin(); ot != in.outputTemperatures.end(); ot++)
	{
		os << "output temp " << *ot << std::endl;
	}

    os << "algorithm: ";
    if (in.fitting)
    {
        os << "fitting";
    }
    else
    {
        os << "direct";
    }
    os << std::endl;
    os << "black level: "
        << in.blackLevel[0] << ", "
        << in.blackLevel[1] << ", "
        << in.blackLevel[2] << ", "
        << in.blackLevel[3] << std::endl;
    os << "max gain: ";
    if (in.bUseMaxValue)
    {
        os << in.maxMatrixValue;
    }
    else
    {
        os << "none";
    }
    os << std::endl;
    os << "min gain: ";
    if (in.bUseMinValue)
    {
        os << in.minMatrixValue;
    }
    else
    {
        os << "none";
    }
    os << std::endl;
    os << "max line size: ";
    if (in.bUseMaxLineSize)
    {
        os << in.maxLineSize << "B";
    }
    else
    {
        os << "none";
    }
    os << std::endl;
    os << "max bit per diff: ";
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

lsh_input::lsh_input(enum HW_VERSION hw_version)
{
    hwVersion = hw_version;

    vertex_int = hw_version >= HW_2_6 ?
    HW_2_6_LSH_VERTEX_INT : HW_2_X_LSH_VERTEX_INT;
    vertex_frac = hw_version >= HW_2_6 ?
    HW_2_6_LSH_VERTEX_FRAC : HW_2_X_LSH_VERTEX_FRAC;
    vertex_signed = hw_version >= HW_2_6 ?
    HW_2_6_LSH_VERTEX_SIGNED : HW_2_X_LSH_VERTEX_SIGNED;

    // may need to change from hwv too
    tileSize = LSH_TILE_MAX;
    fitting = false;

    output_prefix = "sensor";

    /* all these parameters are defaults that will be changed by the
     * advanced config according to the selected HW versions */
    unsigned int m = (1 <<
        (vertex_int + vertex_frac - vertex_signed));
    // but the number is a fixed point
    maxMatrixValue = static_cast<double>(m) / (1 << vertex_frac);
    bUseMaxValue = true;
    maxLineSize = SYSMEM_ALIGNMENT;  // 512b
    bUseMaxLineSize = hw_version < HW_2_6;  // not needed since 2.6
    maxBitDiff = hw_version >= HW_2_6 ?
    HW_2_6_MAX_BIT_DIFF : HW_2_X_MAX_BIT_DIFF;
    bUseMaxBitsPerDiff = true;
    bUseMinValue = false;
    minMatrixValue = 1.0;
    interpolate = true;
}

bool lsh_input::addInput(int temperature, const std::string &filename,
    int marginL, int marginT, int marginR, int marginB,
    float smoothing, int granularity)
{
    std::map<int, input_info>::const_iterator it = inputs.find(temperature);
    if (inputs.end() != it)
    {
        return false;
    }
    input_info info;

    info.imageFilename = filename;
    info.margins[MARG_L] = marginL;
    info.margins[MARG_T] = marginT;
    info.margins[MARG_R] = marginR;
    info.margins[MARG_B] = marginB;
    info.granularity = granularity;
    info.smoothing = smoothing;

    inputs[temperature] = info;
    outputTemperatures.push_back(temperature);

    return true;
}


std::string lsh_input::generateFilename(LSH_TEMP t) const
{
    std::ostringstream os;

    if (!output_prefix.empty())
    {
        os << output_prefix << "_";
    }
    if (fitting)
    {
        os << "fitting";
    }
    else
    {
        os << "direct";
    }
    os << "_" << t << "k"
        << "_t" << tileSize
        << ".lsh";

    return os.str();
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
    connect(log_btn, SIGNAL(clicked()), this, SLOT(showLog()));
    connect(preview_btn, SIGNAL(clicked()), this, SLOT(showPreview()));

    params = new LSHConfig(hw_version);
	initInputFilesTable();
    // don`t want "Advance Settings" button in LSH tab
    params->lshConfigAdv_btn->hide();
	params->addFiles_btn->hide();
	params->addTemps_btn->hide();
	params->clear_btn->hide();
	params->outputFiles_tw->show();
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

    warning_icon->setPixmap(this->style()->standardIcon(WARN_ICON_TW)\
        .pixmap(QSize(WARN_ICON_W, WARN_ICON_H)));
    warning_lbl->setVisible(false);
    warning_icon->setVisible(false);

    advSettLookUp_btn->setEnabled(false);

    connect(advSettLookUp_btn, SIGNAL(clicked()), this,
        SLOT(advSettLookUp()));
    connect(saveError_btn, SIGNAL(clicked()), this,
        SLOT(saveTargetMSE()));

    logDockWidget->hide();
    previewDockWidget->hide();

    if (_isVLWidget)
    {
        integrate_btn->setEnabled(false);
        save_btn->hide();
    }
    else
    {
        integrate_btn->hide();
        log_btn->hide();
        preview_btn->hide();
    }

    resize(width(), minimumHeight());
}

void LSHTuning::clearOutput()
{
    std::map<LSH_TEMP, lsh_output*>::const_iterator it;

    if (gridView)
    {
        gridView->connectButtons(false);
        //gridView->clearData();
    }

    for (it = output.begin(); it != output.end(); it++)
    {
        if (it->second)
        {
            delete it->second;
            output[it->first] = NULL;
        }
    }
    output.clear();
}

int LSHTuning::addResult(lsh_output *pResult)
{
    std::map<LSH_TEMP, lsh_output*>::iterator it;
    if (inputs.bUseMaxLineSize
        && pResult->lineSize > inputs.maxLineSize)
    {
        return EXIT_FAILURE;
    }

    it = output.find(pResult->temperature);
    if (output.end() != it)
    {
        emit logMessage(tr("Matrix for temperature %n already exists! "\
            "Replacing it.\n", "followed by temp in kelvin",
            pResult->temperature));
        delete it->second;
    }
    // copy the result by pointer! LSHTuning becomes the owner
    output[pResult->temperature] = pResult;

    gridView->loadData(pResult);

    this->params->setConfig(pResult);

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
        //  support multiple output
        //outFile = params->outputFilename->text();
        //params->outputFilename->hide();
        //params->outputFilename_lbl->hide();
    }
    return EXIT_SUCCESS;
}

void LSHTuning::initInputFilesTable()
{
	if (!params)
	{
		return;
	}

	QStringList labels;
	labels << "File" << "Temperature" << "Configuration" << "Preview";
	params->inputFiles_tw->setColumnCount(labels.size());
	params->inputFiles_tw->setRowCount(0);
	params->inputFiles_tw->setHorizontalHeaderLabels(labels);

	params->inputFiles_tw->setEditTriggers(false);
}

/*
 * public
 */

std::map<int, QString> LSHTuning::getOutputFiles()
{
    std::map<int, QString> ret;
    
    std::map<LSH_TEMP, lsh_output*>::const_iterator it;
    for (it = output.begin(); it != output.end(); it++)
    {
        ret.insert(std::make_pair((int)it->first, QString::fromLatin1(it->second->filename.c_str())));
    }

    return ret;
}

unsigned int LSHTuning::getBitsPerDiff(LSH_TEMP temp) const
{ 
    return output.at(temp)->bitsPerDiff; 
}

unsigned int LSHTuning::getWBScale(LSH_TEMP temp) const
{
    return output.at(temp)->rescaled;
}

LSHTuning::LSHTuning(GenConfig *genConfig, enum HW_VERSION hwv, bool isVLWidget, QWidget *parent)
    : VisionModule(parent), hw_version(hwv), inputs(hwv)
{
    params = NULL;
    gridView = NULL;
    advSett = NULL;

	currentFileIndex = -1;

    _isVLWidget = isVLWidget;

    this->genConfig = genConfig;
    if (HW_UNKNOWN == hw_version)
    {
#if FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN < 4
        hw_version = HW_2_X;
#elif FELIX_VERSION_MAJ == 2 && FELIX_VERSION_MIN < 6
        hw_version = HW_2_4;
#elif FELIX_VERSION_MAJ == 2
        hw_version = HW_2_6;
#elif FELIX_VERSION_MAJ == 3
        hw_version = HW_3_X;
#else
#error unkown HW version for default
#endif
        inputs = lsh_input(hw_version);
    }
    init();
}

LSHTuning::~LSHTuning()
{
    // genConfig is now owned by this object!
    clearOutput();
}

//QString LSHTuning::getOutputFilename()
//{
//    return params->outputFilename->text();
//}

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

    if (output.size() == 0)
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

    if (!mess.isEmpty())
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
    //QString lshFilename = params->outputFilename->text();

    /*QRegExpValidator outputValidator;
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
    }*/
    /*if (output.size() == 0)
    {
        QtExtra::Exception ex(tr("No results to save"));
        ex.displayQMessageBox(this);
        return EXIT_FAILURE;
    }*/

    lshPath += "/";
    std::map<LSH_TEMP, lsh_output*>::const_iterator it;
    it = output.begin();
    for (it = output.begin(); it != output.end(); it++)
    {
        QString target = lshPath + QString::fromLatin1(it->second->filename.c_str());
        try
        {
            LSHCompute::writeLSHMatrix(it->second->channels, config.tileSize,
                std::string(target.toLatin1()));

            QString mess = "LSH: saving matrix in " + lshPath + "\n";
            emit logMessage(mess);
        }
        catch (std::exception ex)
        {
            QtExtra::Exception qex(tr("Failed to save output for temp %n", "",
                it->first), QString::fromStdString(ex.what()));
            qex.displayQMessageBox(this);
        }
    }

    return EXIT_SUCCESS;
}

int LSHTuning::saveTargetMSE(const QString &dirFilename)
{
    QString filename(dirFilename);

    if (output.size() == 0)
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
        std::map<LSH_TEMP, lsh_output*>::const_iterator it;
        file << "Output error for LSH calibration" << std::endl
            << inputs
            << std::endl;

        for (it = output.begin(); it != output.end(); it++)
        {
            file << "Output for temperature " << it->first << std::endl;
            for (int c = 0; c < 4; c++)
            {
                file << "channel " << c << std::endl;

                for (int y = 0; y < it->second->outputMSE[c].rows; y++)
                {
                    for (int x = 0; x < it->second->outputMSE[c].cols; x++)
                    {
                        file << " " << it->second->outputMSE[c].at<float>(y, x);
                    }
                    file << std::endl;
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
    lsh_input config(hw_version);
    std::ostringstream os, val;
    std::string calibInfo = "//LSH ";

    if (output.size() == 0)
    {
        throw(QtExtra::Exception(tr("No results to save for LSH")));
    }
    
    config = params->getConfig();

    std::map<LSH_TEMP, lsh_output*>::const_iterator it;
    it = output.begin();
    unsigned int maxBitsPerDiff = 0;
    int i = 0;
    for (it = output.begin(); it != output.end(); it++)
    {
        if (!it->second->interpolated)
        {
            lsh_input::const_iterator in_info
                = this->inputs.inputs.find(it->second->temperature);
            if (this->inputs.inputs.end() == in_info)
            {
                throw(QtExtra::Exception(tr("No input info found for an output")));
            }

            os.str("");
            os << calibInfo + "input image " << i << " for temperature "
                << in_info->first;
            ISPC::Parameter inputImage(os.str(),
                in_info->second.imageFilename);
            list.addParameter(inputImage);

            os.str("");
            os << calibInfo + "using margins (CFA) for image " << i;
            val.str("");
            for (int c = 0; c < 4; c++)
            {
                val << " " << in_info->second.margins[c];
            }
            ISPC::Parameter margins(os.str(), val.str());
            list.addParameter(margins);

            os.str("");
            os << calibInfo + "using smoothing " << i << "  "
                << in_info->second.smoothing;
            ISPC::Parameter smoothing(os.str(), os.str());
            list.addParameter(smoothing);
        }
        else
        {
            os.str("");
            os << calibInfo + "input image " << i << " for temperature "
                << it->second->temperature;
            val.str("");
            val << "is interpolated";
            ISPC::Parameter interpolated(os.str(), val.str());
            list.addParameter(interpolated);
        }

        os.str("");
        os << ISPC::ControlLSH::LSH_FILE_S.name << "_" << i;
        ISPC::Parameter file(os.str(), it->second->filename);
        list.addParameter(file);

        os.str("");
        os << ISPC::ControlLSH::LSH_CTRL_TEMPERATURE_S.name << "_" << i;

        val.str("");
        val << it->second->temperature;
        ISPC::Parameter temperature(os.str(), val.str());
        list.addParameter(temperature);

        os.str("");
        os << calibInfo + "rescaled matrix " << i << " by "
            << it->second->rescaled;
        val.str("");
        val << it->second->rescaled;
        ISPC::Parameter rescaled(os.str(), val.str());
        list.addParameter(rescaled);

        os.str("");
        os << ISPC::ControlLSH::LSH_CTRL_SCALE_WB_S.name << "_" << i;
        val.str("");
        val << 1.0 / it->second->rescaled;
        ISPC::Parameter wbrescale(os.str(), val.str());
        list.addParameter(wbrescale);

        // they should all be the same but it's not an expensive task
        maxBitsPerDiff = IMG_MAX_INT(it->second->bitsPerDiff, maxBitsPerDiff);

        i++;
    }  // for each output

    val.str("");
    val << maxBitsPerDiff;
    ISPC::Parameter bitspdiff(ISPC::ControlLSH::LSH_CTRL_BITS_DIFF.name,
        val.str());
    list.addParameter(bitspdiff);

    val.str("");
    val << i;
    ISPC::Parameter ncorr(ISPC::ControlLSH::LSH_CORRECTIONS.name, val.str());
    list.addParameter(ncorr);

    // disabled by default
    ISPC::Parameter enable(ISPC::ModuleLSH::LSH_MATRIX.name, "0");
    list.addParameter(enable);

    if (inputs.bUseMaxLineSize)
    {
        val.str("");
        val << inputs.maxLineSize;
        ISPC::Parameter maxline(calibInfo + "max line size (bytes)",
            val.str());
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
        val.str("");
        val << inputs.maxBitDiff;
        ISPC::Parameter maxline(calibInfo + "max bits per diff",
            val.str());
        list.addParameter(maxline);
    }
    else
    {
        ISPC::Parameter maxline(calibInfo + "max bits per diff",
            std::string("not used"));
        list.addParameter(maxline);
    }

    val.str("");
    for (i = 0; i < 4; i++)
    {
        val << inputs.blackLevel[i] << " ";
    }
    ISPC::Parameter blcSensorBlack(calibInfo
        + ISPC::ModuleBLC::BLC_SENSOR_BLACK.name + " used", val.str());
    list.addParameter(blcSensorBlack);

    list.addGroup("LSH", ISPC::ModuleLSH::getGroup());
}

int LSHTuning::tuning()
{
    if (this->inputs.inputs.empty()
        /*|| this->params->outputFilename->text().isEmpty()*/)
    {
        // no input or not output
        return EXIT_FAILURE;
    }

    clearOutput();

    LSHCompute *pCompute = new LSHCompute();

    connect(this, SIGNAL(compute(const lsh_input*)),
        pCompute, SLOT(compute(const lsh_input*)));

    connect(pCompute, SIGNAL(finished(LSHCompute *)),
        this, SLOT(computationFinished(LSHCompute*)));
    connect(pCompute, SIGNAL(currentStep(int)),
        progress, SLOT(setValue(int)));
    connect(pCompute, SIGNAL(logMessage(QString)),
        this, SIGNAL(logMessage(QString)));
    connect(pCompute, SIGNAL(logMessage(QString)),
        this, SLOT(logMessage(QString)));

    progress->setMaximum(inputs.inputs.size() * LSH_N_STEPS);
    progress->setValue(0);
    progress->setVisible(true);

    save_btn->setEnabled(false);

    warning_icon->setVisible(false);
    warning_lbl->setVisible(false);

    // enabled back when the computation is finished
    //params->outputFilename->setReadOnly(true);

    emit compute(&(this->inputs));
    emit changeCentral(getDisplayWidget());
    emit changeResult(getResultWidget());

    if (_isVLWidget)
    {
        previewDockWidget->setWidget(gridView);
        previewDockWidget->setVisible(true);
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
        LSHConfig sconf(hw_version);

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

			//params->inputFiles_tw->clear();
			initInputFilesTable();

            // to display parameters while running
            params->setConfig(this->inputs, true);
            this->params->setConfig(NULL); // no output config
            //params->outputFilename->setText(sconf.outputFilename->text());
            //params->setConfig(inputs, true);

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

			// Fill input table
			std::list<LSH_TEMP>::const_iterator tempIt;
			for (tempIt = this->inputs.outputTemperatures.begin(); tempIt != this->inputs.outputTemperatures.end(); tempIt++)
			{
				params->inputFiles_tw->insertRow(params->inputFiles_tw->rowCount());
				QString fileName = QString::fromStdString((this->inputs.inputs.find(*tempIt) != this->inputs.inputs.end()) ?
					this->inputs.inputs.find(*tempIt)->second.imageFilename : std::string());
				params->inputFiles_tw->setItem(params->inputFiles_tw->rowCount() - 1, 0, new QTableWidgetItem(fileName));
				
				QSpinBox *temp = new QSpinBox();
				temp->setRange(0, 100000);
				temp->setButtonSymbols(QAbstractSpinBox::NoButtons);
				temp->setAlignment(Qt::AlignHCenter);
				temp->setValue(*tempIt);
				temp->setReadOnly(true);
				params->inputFiles_tw->setCellWidget(params->inputFiles_tw->rowCount() - 1, 1, temp);

				if (!fileName.isEmpty())
				{
					QPushButton *config = new QPushButton("Configuration LookUp");
					connect(config, SIGNAL(clicked()), this, SLOT(lookUpConfiguration()));
					params->inputFiles_tw->setCellWidget(params->inputFiles_tw->rowCount() - 1, 2, config);

					QPushButton *preview = new QPushButton("Preview");
					connect(preview, SIGNAL(clicked()), this, SLOT(previewInputFile()));
					params->inputFiles_tw->setCellWidget(params->inputFiles_tw->rowCount() - 1, 3, preview);
				}
			}
			params->inputFiles_tw->resizeColumnsToContents();
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

void LSHTuning::computationFinished(LSHCompute *toStop)
{
    bool allowSaving = false;
    std::map<LSH_TEMP, lsh_output*>::const_iterator it;

    if (toStop == NULL)
    {
        // should not happen
        return;
    }

    gridView->changeInput(inputs);
    gridView->clearData();
    // clearing the output should have been done at start of processing

    for (it = toStop->output.begin(); it != toStop->output.end(); it++)
    {
        if (addResult(it->second) != EXIT_SUCCESS)
        {
            QString mess = tr("Computed grid with tile size %n does "\
                "not fit the line size limit.",
                "m1, followed by m2",
                inputs.tileSize);
            QString details = tr("Expected line size %n Byte(s)",
                "d1 followed by d2", it->second->lineSize)
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
                this->params->setConfig(this->inputs, true);
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
        else
        {
            // we added at least 1 result
            allowSaving = true;
        }
    }

    gridView->connectButtons(true);
    gridView->channelChanged(0);

    if (toStop->output.size() == 0)
    {
        QtExtra::Exception ex(tr("Failed to generate the LSH grids "\
            "- check the log tab for details."));

        ex.displayQMessageBox(this);
        params->setReadOnly(true);
    }
    // we copied the pointers
    toStop->output.clear();

    toStop->deleteLater();
    progress->setVisible(false);
    save_btn->setEnabled(allowSaving);
    // disabled when starting the computation
    //params->outputFilename->setReadOnly(false);


	// Fill output files table
	params->initOutputFilesTable();
	for (it = output.begin(); it != output.end(); it++)
	{
		params->outputFiles_tw->insertRow(params->outputFiles_tw->rowCount());
		params->outputFiles_tw->setItem(params->outputFiles_tw->rowCount() - 1, 0, new QTableWidgetItem(QString::fromLatin1((*it).second->filename.c_str())));
		QSpinBox *temp = new QSpinBox();
		temp->setRange(0, 100000);
		temp->setValue((*it).first);
		temp->setReadOnly(true);
		temp->setButtonSymbols(QAbstractSpinBox::NoButtons);
		temp->setAlignment(Qt::AlignHCenter);
		params->outputFiles_tw->setCellWidget(params->outputFiles_tw->rowCount() - 1, 1, temp);
	}
	params->outputFiles_tw->resizeColumnsToContents();
}

void LSHTuning::retranslate()
{
    Ui::LSHTuning::retranslateUi(this);
}

// Displays advance settings lookup window
void LSHTuning::advSettLookUp(bool adv)
{
    advSett = new LSHConfigAdv(hw_version);

    if (_isVLWidget)
    {
        advSett->setStyleSheet(_css);
    }

    //advSett->setConfig(inputs);  // Set configuration
    advSett->buttonBox->hide();  // Don't need OK/Cancel buttons

    // Disableing all that shloudn't be changed here
    advSett->setReadOnly(true);

	if (adv)
	{
		advSett->fileConfig_gb->hide();

		advSett->maxMatrixValue_sb->setValue(inputs.maxMatrixValue);
		advSett->maxMatrixValueUsed_cb->setChecked(inputs.bUseMaxValue);
		advSett->minMatrixValue_sb->setValue(inputs.minMatrixValue);
		advSett->minMatrixValueUsed_cb->setChecked(inputs.bUseMinValue);
		advSett->maxLineSize_sb->setValue(inputs.maxLineSize);
		advSett->maxLineSize_cb->setChecked(inputs.bUseMaxLineSize);
		advSett->maxBitDiff_sb->setValue(inputs.maxBitDiff);
		advSett->maxBitDiff_cb->setChecked(inputs.bUseMaxBitsPerDiff);
	}
	else
	{
		advSett->advParams_gb->hide();

		int temp = ((QSpinBox *)params->inputFiles_tw->cellWidget(currentFileIndex, 1))->value();
		advSett->marginLeft_sb->setValue(inputs.inputs.at(temp).margins[0]);
		advSett->marginTop_sb->setValue(inputs.inputs.at(temp).margins[1]);
		advSett->marginRight_sb->setValue(inputs.inputs.at(temp).margins[2]);
		advSett->marginBottom_sb->setValue(inputs.inputs.at(temp).margins[3]);
		advSett->smoothing_sb->setValue(inputs.inputs.at(temp).smoothing);

		advSettLookUpPreview();  // View image with margin

		connect(advSett->color_cb, SIGNAL(currentIndexChanged(int)), this,
			SLOT(advSettLookUpPreview()));
	}

    advSett->exec();
}

// Displays advance settings lookup image preview
void LSHTuning::advSettLookUpPreview()
{
	if (currentFileIndex < 0 || !params->inputFiles_tw)
	{
		return;
	}

    // Load the image file
	QString path = params->inputFiles_tw->item(currentFileIndex, 0)->text();
    //QString path = params->inputFile->absolutePath();
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

void LSHTuning::lookUpConfiguration()
{
	for (int i = 0; i < params->inputFiles_tw->rowCount(); i++)
	{
		if ((QPushButton *)params->inputFiles_tw->cellWidget(i, 2) == (QPushButton *)sender())
		{
			currentFileIndex = i;
			advSettLookUp(false);
			return;
		}
	}
}

void LSHTuning::previewInputFile()
{
	if (!params->inputFiles_tw)
	{
		return;
	}

	QString path;

	for (int i = 0; i < params->inputFiles_tw->rowCount(); i++)
	{
		if (params->inputFiles_tw->cellWidget(i, 3) == (QPushButton *)sender())
		{
			path = params->inputFiles_tw->item(i, 0)->text();
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
	// pDiag->setModal(true);
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

void LSHTuning::logMessage(const QString &message)
{
    log_te->append(message);
}

void LSHTuning::showLog()
{
    if (logDockWidget)
    {
        logDockWidget->setVisible(!logDockWidget->isVisible());
    }
}

void LSHTuning::showPreview()
{
    if (previewDockWidget)
    {
        previewDockWidget->setVisible(!previewDockWidget->isVisible());
    }
}

