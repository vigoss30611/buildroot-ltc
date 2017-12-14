/**
******************************************************************************
 @file clitune.cpp

 @brief Point of entry of the VisionTuning command line interface

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

#include "clitune.hpp"
#include "genconfig.hpp"
#include "lsh/tuning.hpp"
#include "ispc/ModuleLSH.h"
#include "ispc/ModuleBLC.h"
#include <felix_hw_info.h>

#include <QCommandLineParser>
#include <QTimer>
#include <QThread>

const char CliTune::INVALID_MODE = 0x00;
const char CliTune::LSH_MODE = 0x01;
const char CliTune::LCA_MODE = LSH_MODE << 1;
const char CliTune::CCM_MODE = LSH_MODE << 2;

CliTune::CliTune(QObject *parent) :
    QObject(parent),
    cliOut(QDebug(QtDebugMsg)),
    tuning("t"), hwVerParam("v"), blackLevel("b")
{
    app = QCoreApplication::instance();
}

void CliTune::init(void)
{
    pLSH = 0;
    outBufStr.setString(&outBuf);

    cliOut.noquote();

    cliArgs = app->arguments();
    initCommonParams();
    parser.parse(cliArgs);
}

void CliTune::run()
{
    bool exitNow = false; // don't calculate anything, just exit app

    if (!validateCommonParams())
    {
        // forces app exit also
        parser.showHelp();
    }

    if (tuningMode == INVALID_MODE)
    {
        exitNow = true;
    }

    if (!exitNow && (tuningMode & LSH_MODE))
    {
        pLSH = new LSHTuningNoGUI(commonParams);
        if (pLSH)
        {
            connect(pLSH, SIGNAL(finished()), this, SLOT(quit()));
            connect(pLSH, SIGNAL(logMessage(const QString &)),
                this, SLOT(logMsg(const QString &)));
            pLSH->parseCliArgs(cliArgs);
            if (pLSH->calculateLSHmatrix() != EXIT_SUCCESS)
            {
                exitNow = true;
            }
        }
        else
        {
            logMsg("Cannot allocate memory for LSHTuningNoGUI");
            exitNow = true;
        }
    }

#if 0
    if (!exitNow && (tuningMode & LCA_MODE))
    {

    }

    if (!exitNow && (tuningMode & CCM_MODE))
    {

    }
#endif

    if (exitNow)
    {
        // wait a while until QT messaging engine starts, then quit
        QTimer::singleShot(10, this, SLOT(quit()));
    }
}

void CliTune::quit()
{
    if (pLSH != 0) delete pLSH;
    cliOut << outBuf;
    emit finished();
}

// run CLI mode if -h or -t is present
bool CliTune::commandLineMode()
{
    if (parser.isSet(this->tuning))
    {
        QString tuning = parser.value(this->tuning);
        if(!tuning.isEmpty())
        {
            return true;
        }
    }
    else if (parser.isSet("h") || parser.isSet("help"))
    {
        // forces app exit also
        parser.showHelp();
        return true;
    }
    // not in an else to fix compiler's warning
    return false;
}

void CliTune::logMsg(const QString &message)
{
    if (!message.endsWith("\n"))
    {
        outBufStr << message << "\n";
    }
    else
    {
        outBufStr << message;
    }
    //cliOut << message;
}


void CliTune::initCommonParams()
{
    parser.setApplicationDescription("Command Line Interface for" \
        " VisionTuning"\
        "\n\nexample usage: VisionTuning -t LSH -b \"8 8 8 8\" -v 2.6 "\
        "<tuning specific options>");
    parser.addHelpOption();

    tuning = QCommandLineOption(QStringList() << "t" << "tuning",
        "Chooses a tunning mode (LSH, LCA, CCM)", "mode");
    parser.addOption(tuning);

    hwVerParam = QCommandLineOption(QStringList() << "v" << "hwVer",
        "Provides HW version (determines max gain)", "ver");
    parser.addOption(hwVerParam);

    blackLevel = QCommandLineOption(QStringList() << "b" << "blackLevel",
        "Sensor's black level (in CFA order)\n" \
        "example: --blackLevel \"8 8 8 8\"",
        "values");
    parser.addOption(blackLevel);

    parser.addPositionalArgument("args", "tuning specific arguments");
}

bool CliTune::validateCommonParams()
{
    tuningMode = INVALID_MODE;

    if (parser.isSet(tuning))
    {
        QString tuning = parser.value(this->tuning);
        if(!tuning.isEmpty())
        {
            if (tuning.compare("LSH") == 0)
            {
                tuningMode |= LSH_MODE;
            }
            else if (tuning.compare("LCA") == 0)
            {
                tuningMode |= LCA_MODE;
            }
            else if (tuning.compare("CCM") == 0)
            {
               tuningMode |= CCM_MODE;
            }

        }

        if (tuningMode == INVALID_MODE)
        {
            return false;
        }
    }

    QString hwVerVal = parser.value(hwVerParam);
    QString blackLevelVal = parser.value(blackLevel);

    if (!hwVerVal.isEmpty() && !blackLevelVal.isEmpty())
    {
        commonParams.hwVer = translateHwVer(hwVerVal);

        // --- validate black levels  ---
        int blackLevelMin = ISPC::ModuleBLC::BLC_SENSOR_BLACK.min;
        int blackLevelMax = ISPC::ModuleBLC::BLC_SENSOR_BLACK.max;
        QStringList blackLevelsList = blackLevelVal.split(" ");
        for (int i = 0, tmp = 0; i < 4; i++)
        {
            tmp = (blackLevelsList.at(i)).toInt();
            if (tmp < blackLevelMin) tmp = blackLevelMin;
            else if (tmp > blackLevelMax) tmp = blackLevelMax;
            if ((float)tmp != (blackLevelsList.at(i)).toFloat())
            {
                QString msg("WARNING: blackLevel[");
                msg += QString::number(i);
                msg += "] changed to ";
                msg += QString::number(tmp);
                logMsg(msg);
            }
            commonParams.blackLevel[i] = tmp;
        }
        // --- validate black levels  ---
    }
    else
    {
        return false;
    }

    return true;
}

HW_VERSION CliTune::translateHwVer(QString &ver)
{
    HW_VERSION enumVer = HW_UNKNOWN;

    if (ver.contains("2."))
    {
        enumVer = HW_2_X;

        if (ver.endsWith("4"))
        {
            enumVer = HW_2_4;
        }
        else if (ver.endsWith("6"))
        {
            enumVer = HW_2_6;
        }
        //else if (ver.endsWith("7"))
        //{
        //    enumVer = HW_2_7;
        //}
    }
    else if (ver.contains("3."))
    {
        enumVer = HW_3_X;
    }
    return enumVer;
}
