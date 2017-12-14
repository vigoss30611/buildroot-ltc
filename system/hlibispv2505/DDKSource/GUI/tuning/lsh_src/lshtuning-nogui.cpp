/**
******************************************************************************
 @file lshtuning-nogui.cpp

 @brief Modified LSHTuning class. Used in command line mode.

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
#include "lsh/config.hpp"
#include "lsh/compute.hpp"
#include "ispc/ModuleLSH.h"
#include "ispc/ControlLSH.h"
#include "ispc/ModuleBLC.h"
#include "clitune.hpp"

#include <felix_hw_info.h>

#include <QDebug>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QMap>


LSHTuningNoGUI::LSHTuningNoGUI(const struct commonParams &params,
    QObject *parent) :
    imageFile("i"), imageTemperature("T"), tileSize("s"),
    interTemperature("it"), algorithm("a"), saveTxt("saveTxt"),
    commonParams(params)
{
    this->pCompute = 0;
    this->inputs = 0;
    this->saveAsTxt = false;
}

LSHTuningNoGUI::~LSHTuningNoGUI()
{
    if (pCompute != 0)
    {
        delete pCompute;
    }
    if (inputs != 0)
    {
        delete inputs;
    }
}

void LSHTuningNoGUI::parseCliArgs(const QStringList &cliArgs)
{
    initTuneSpecificParams();
    parser.parse(cliArgs);

    if (validateTuneSpecificParams())
    {
        pCompute = new LSHCompute();

        connect(this, SIGNAL(compute(const lsh_input*)),
            pCompute, SLOT(compute(const lsh_input*)));
        connect(pCompute, SIGNAL(finished(LSHCompute *)),
            this, SLOT(computationFinished(LSHCompute*)));
        connect(pCompute, SIGNAL(logMessage(const QString &)),
            this, SIGNAL(logMessage(const QString &)));
    }
    else
    {
        emit logMessage("Invalid or missing parameters.\n"\
            "Add option -h for more help");
        //emit logMessage(parser.helpText());
    }
}

int LSHTuningNoGUI::calculateLSHmatrix()
{
    if (pCompute)
    {
        emit compute(this->inputs);
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}

void LSHTuningNoGUI::computationFinished(LSHCompute *lshComp)
{
    std::map<LSH_TEMP, lsh_output*>::const_iterator
        it = (lshComp->output).begin();
    lsh_output *pResult = 0;
    uint temperature = 0;
    bool allowSaving = true;

    for (; it != (lshComp->output).end(); it++)
    {
        lsh_output *pResult = it->second;
        temperature = it->first;

        if (inputs->bUseMaxLineSize
            && pResult->lineSize > inputs->maxLineSize)
        {
            QString mess = tr("Computed grid with tile size %n does "\
                "not fit the line size limit.",
                "m1, followed by m2",
                inputs->tileSize);
            QString details = tr("Expected line size %n Byte(s)",
                "d1 followed by d2", pResult->lineSize)
                + tr(" while maximum is %n Byte(s).",
                "d2, leading space important",
                inputs->maxLineSize);

            if (inputs->tileSize < LSH_TILE_MAX)
            {
                int curr = inputs->tileSize;
                inputs->tileSize = inputs->tileSize << 1;

                mess = mess +
                    tr(" Grid will be recomputed with a bigger "\
                    "tile size %n.",
                    "m2, leading space important",
                    inputs->tileSize) +
                    details;
                emit logMessage(mess);

                emit compute(inputs);

                return;
            }
            else
            {
                mess = mess +
                    tr(" This is already the maximum tile size - "\
                    "the grid needs manual modification to support the "\
                    "selected maximum line "\
                    "size of %n Byte(s)", "", inputs->maxLineSize) +
                    details;
                emit logMessage(mess);

                allowSaving = false;
            }
        }


        if (allowSaving)
        {
            try
            {
                LSHCompute::writeLSHMatrix(pResult->channels, inputs->tileSize,
                    pResult->filename);
            }
            catch (std::exception ex)
            {
                QString mess = tr("Failed to save the matrix: ") + ex.what();
                emit logMessage(mess);
            }

            if (saveAsTxt)
            {
                try
                {
                    LSHCompute::writeLSHMatrix(pResult->channels,
                        inputs->tileSize,
                        pResult->filename + ".txt", true);
                }
                catch (std::exception ex)
                {
                    QString mess = tr("Failed to save the matrix to texte: ")
                        + ex.what();
                    emit logMessage(mess);
                }
            }
        }
    }
    emit finished();
}

void LSHTuningNoGUI::initTuneSpecificParams()
{
    parser.setApplicationDescription("Command Line Interface for" \
        " VisionTuning"\
        "\n\nexample usage: VisionTuning -t LSH -b \"8 8 8 8\" -v 2.6 -s 32 "\
        "-i image_2800.flx -T 2800 -i image_5000.flx -T 5000 "\
        "--interTemp 3500 --interTemp 6500"\
        "\n\nNote that temperatures order should respond to "\
        "calibration images order");
    parser.addHelpOption();

    parser.addPositionalArgument("-t/--tuning LSH", "");

    imageFile = QCommandLineOption(QStringList() << "i" << "imageFile",
        "Calibration image in *.flx format", "file");
    parser.addOption(imageFile);

    imageTemperature = QCommandLineOption(QStringList() << "T" << "Temp",
        "Illuminant's temperature [K] for calibration image",
        "temperature");
    parser.addOption(imageTemperature);

    interTemperature = QCommandLineOption("interTemp",
        "Illuminant's temperature [K] for generation interpolated deshading "
        "grid.\nDoesn't require calibration image.",
        "temperature");
    parser.addOption(interTemperature);

    tileSize = QCommandLineOption(QStringList() << "s" << "tileSize",
        "Defines number of CFAs between interpolation points", "size");
    parser.addOption(tileSize);

    algorithm = QCommandLineOption(QStringList() << "a" << "algorithm",
        "Use 'direct' or 'fitting' curve", "[direct|fitting]");
    parser.addOption(algorithm);

    saveTxt = QCommandLineOption("saveTxt",
        "Save generated matrices also in CSV format");
    parser.addOption(saveTxt);
}

bool LSHTuningNoGUI::validateTuneSpecificParams()
{
    bool paramsOK = false;

    QStringList imageFileVals = parser.values(imageFile);
    QStringList imageTempVals = parser.values(imageTemperature);
    QStringList interTempVals = parser.values(interTemperature);
    QString tileSizeVal = parser.value(tileSize);

    if (parser.isSet("h") || parser.isSet("help"))
    {
        // forces app exit also
        parser.showHelp();
        return false;
    }

    if (!imageFileVals.isEmpty() && !imageTempVals.isEmpty())
    {
        paramsOK = true;

        // --- validate tile size ---
        int tile = tileSizeVal.toInt();
        if (tile < LSH_TILE_MIN) tile = LSH_TILE_MIN;
        else if (tile > LSH_TILE_MAX) tile = LSH_TILE_MAX;
        if (tile != tileSizeVal.toInt())
        {
            QString msg("WARNING: tileSize rounded to ");
            msg += QString::number(tile);
            emit logMessage(msg);
        }
        // --- validate tile size ---

        inputs = new lsh_input(commonParams.hwVer);
        inputs->tileSize = tile;
        for (int i = 0, tmp = 0; i < 4; i++)
        {
            inputs->blackLevel[i] = commonParams.blackLevel[i];
        }

        QMap<uint, QString> tempFileMap;
        QStringList::iterator tempIt = imageTempVals.begin();
        QStringList::iterator fileIt = imageFileVals.begin();
        for (; tempIt != imageTempVals.end(); tempIt++, fileIt++)
        {
            if (fileIt == imageFileVals.end()) break;
            tempFileMap[(*tempIt).toInt()] = *fileIt;
            //qDebug() << *tempIt << "->" << *fileIt;
        }

        QFileInfo fileInfo;
        QMap<uint, QString>::const_iterator it = tempFileMap.constBegin();
        uint temp = 0;
        for (; it != tempFileMap.constEnd(); it++)
        {
            //qDebug() << it.key() << ":" << it.value();
            // --- validate input file ---
            if (!fileInfo.exists(it.value()))
            {
                QString msg = "WARNING: File " + it.value() +
                    " doesn't exist";
                emit logMessage(msg);
                continue;
            }
            // --- validate input file ---

            // --- validate temperature ---
            temp = it.key();
            if (temp > 100000)
            {
                temp = 100000;
                QString msg = "WARNING: Temperature limited to ";
                msg += QString::number(temp);
                msg += "K";
                emit logMessage(msg);
            }
            // --- validate temperature ---
            inputs->addInput(temp, (it.value()).toStdString());
        }
        if (inputs->inputs.size() == 0)
        {
            paramsOK = false;
            emit logMessage("ERROR: No input files found");
        }

        // --- validate temperatures for interpolated grids ---
        QStringList::const_iterator itIT = interTempVals.begin();
        for (; itIT != interTempVals.end(); itIT++)
        {
            temp = itIT->toInt();
            if (temp > 100000)
            {
                temp = 100000;
                QString msg = "WARNING: Temperature limited to ";
                msg += QString::number(temp);
                msg += "K";
                emit logMessage(msg);
            }
            (inputs->outputTemperatures).push_back(temp);
        }
        // --- validate temperatures for interpolated grids ---

        QString algo = parser.value(algorithm);
        if (algo == "fitting")
        {
            inputs->fitting = true;
        }

        if (parser.isSet(saveTxt))
        {
            saveAsTxt = true;
        }
    }

    return paramsOK;
}
