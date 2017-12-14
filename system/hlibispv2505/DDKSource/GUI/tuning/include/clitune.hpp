/**
******************************************************************************
 @file clitune.hpp

 @brief Header file for VisionTuning command line interface

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

#ifndef CLI_TUNE_HPP
#define CLI_TUNE_HPP
#include "visiontuning.hpp"

#include <QObject>
#include <QDebug>
#include <QCoreApplication>
#include <QCommandLineParser>

class GenConfig;
class LSHTuningNoGUI;
struct lsh_input;

/** @brief Stores parameters common for all kinds of tuning */
struct commonParams
{
    HW_VERSION hwVer;
    int blackLevel[4];
};

/** @brief Recognizes a tuning mode and creates appropriate object
 *  for tuning operation
 *
 * @ingroup VISION_TUNING
 */
class CliTune : public QObject
{
    Q_OBJECT

public:
    /** @brief flag used for indicate which tuning mode run */
    static const char INVALID_MODE;
    /** @brief flag used for indicate which tuning mode run */
    static const char LSH_MODE;
    /** @brief flag used for indicate which tuning mode run */
    static const char LCA_MODE;
    /** @brief flag used for indicate which tuning mode run */
    static const char CCM_MODE;
private:
    QCoreApplication *app;
    /** @brief CLI options in the form of QStringList */
    QStringList cliArgs;
    /** @brief Used to extract command line options which are common
     *  for all tuning-no-gui objects  */
    QCommandLineParser parser;

    // common params - as command line options
    /** @brief Registered in @ref parser. Used to parse CLI options */
    QCommandLineOption tuning;
    /** @brief Registered in @ref parser. Used to parse CLI options */
    QCommandLineOption hwVerParam;
    /** @brief Registered in @ref parser. Used to parse CLI options */
    QCommandLineOption blackLevel;

    /** @brief Parameters common for all tuning sub-classes */
    struct commonParams commonParams;

    /** @brief Stores *_MODE flags, determines which tuning modes run */
    char tuningMode;
    /** @brief Points to object which calculates LSH deshading matrices */
    LSHTuningNoGUI *pLSH;
    /** @brief Instance of QDebug with disabled QString quoting. Used for
     *  flush @ref outBuf before application quit */
    QDebug cliOut;
    /** @brief Buffer for logs from tuning-no-gui objects */
    QString outBuf;
    /** @brief Stream for logs from tuning-no-gui objects. USes @ref outBuf
     *  for logs storing */
    QTextStream outBufStr;

    /** @brief Inits parameters that @ref parser will be aware of */
    void initCommonParams();

    /** @brief Validates CLI paramters that are common to all tuning modes, then
     *  stores them in @ref commonParams
     *  @return true if parameters are valid
     *  */
    bool validateCommonParams();

    /** @brief Translates HW version from string to enum */
    HW_VERSION translateHwVer(QString &ver);

public:
    explicit CliTune(QObject *parent = 0);

    /** @brief Inits object members and parses CLI parameters (these which are
     * common for all tuning methods) */
    void init(void);

    /** @brief Checks if application was run with parameters specific for
     *  command line interface
     *
     *  @return 'true' if application should start without GUI */
     bool commandLineMode();

signals:
    /** @brief Sent to main app, causes app exit */
    void finished();

public slots:

    /** @brief  start computation for output values */
    void run();

    /** @brief Flush log buffer then exit app  */
    void quit();

    /** @brief Receives log messages from tuning-nogui objects
     *  and stores them in log buffer */
	void logMsg(const QString &message);
};

#endif // CLI_TUNE_HPP
