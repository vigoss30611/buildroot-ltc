/**
*******************************************************************************
@file lsh/tuning.hpp

@brief Display results and parameters for LSH computing

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
#ifndef LSHTUNING_HPP
#define LSHTUNING_HPP
#include "ui/ui_lshtuning.h"

#include <QWidget>
#include <string>
#include "visiontuning.hpp"
#include "clitune.hpp"

class GenConfig;
class LSHConfig;  // "lsh/config.hpp"
class LSHCompute;  // "lsh/compute.hpp"
struct lsh_output;  // "lsh/compute.hpp"
class LSHGridView;  // "lsh/gridview.hpp"

class LSHConfigAdv;

#define MARG_L 0  // left index in margins
#define MARG_T 1  // top index in margins
#define MARG_R 2  // right index in margins
#define MARG_B 3  // bottom index in margins

#define LSH_DEF_TEMP 6500

/** @brief temperature in kelvin relative to the LSH grid */
typedef int LSH_TEMP;

/**
 * @ingroup VISION_TUNING
 */
struct lsh_input
{
    struct input_info
    {
        /** @brief input image */
        std::string imageFilename;
        /**
         * @brief margins to apply on the image in order: Left, Top, Right,
         * Bottom
         */
        int margins[4];
        int granularity;
        /** @brief to smooth noise out */
        float smoothing;
    };

    typedef std::map<LSH_TEMP, input_info> info_list;
    typedef info_list::const_iterator const_iterator;
    typedef info_list::iterator iterator;

    /** @brief WH version */
    HW_VERSION hwVersion;

    /** @brief map of input images with temperature as key (in Kelvin) */
    info_list inputs;
    /**
     * @brief this is the list of output temperatures required
     *
     * if @ref interpolate is enabled it will be used to generate these
     * matrices as output.
     *
     * @warning this list should contain the input temperatures or they will
     * not be outputed.
     */
    std::list<LSH_TEMP> outputTemperatures;
    /** @brief output filename prefix */
    std::string output_prefix;

    /** @brief output tile size */
    int tileSize;

    /** @brief black level to substract per channel */
    int blackLevel[4];
    /** @brief algorithm choice */
    bool fitting;
    /** @brief max output matrix value (see @ref bUseMaxValue) */
    double maxMatrixValue;
    /** @brief use the max output value or not */
    bool bUseMaxValue;
    /** @brief min output matrix value (see @ref bUseMinValue) */
    double minMatrixValue;
    /** @brief use the min output value or not */
    bool bUseMinValue;
    /** @brief maximum line size in Bytes (see @ref bUseMaxLineSize) */
    unsigned int maxLineSize;
    /** @brief use the max line size value or not */
    bool bUseMaxLineSize;
    /** @brief maximum number of bits for differencial encoding */
    unsigned int maxBitDiff;
    /** @brief enfore the max bits per difference in HW format */
    bool bUseMaxBitsPerDiff;

    /**
     * @brief use the @ref outputTemperatures list to interpolate more LSH
     * matrices than are available in the @ref inputs list
     */
    bool interpolate;

    int vertex_frac;
    int vertex_int;
    int vertex_signed;

    lsh_input(enum HW_VERSION hw_version);

    /**
     * @brief add an input information and adds the temperature to the output
     *
     * If the temperature is already present in the @ref inputs list it
     * returns false and nothing is added.
     * The user is expected to have to override the element from the
     * @ref inputs map.
     *
     * On success the temperature is pushed back into the
     * @ref outputTemperatures
     */
    bool addInput(int temperature, const std::string &filename,
        int marginL = 0, int marginT = 0, int marginR = 0, int marginB = 0,
        float smoothing = 0.0001f, int granularity = 8);

    /** @brief generate a filename from the temperature and inputs param */
    std::string generateFilename(LSH_TEMP t) const;
};

/**
 * @ingroup VISION_TUNING
 */
std::ostream& operator<<(std::ostream &os, const lsh_input &in);

/**
 * @ingroup VISION_TUNING
 */
class LSHTuning : public VisionModule, public Ui::LSHTuning
{
    Q_OBJECT

private:
    GenConfig *genConfig;  // from mainwindow
    enum HW_VERSION hw_version;
    lsh_input inputs;
    std::map<LSH_TEMP, lsh_output*> output;
    LSHConfig *params;
    LSHGridView *gridView;
    LSHConfigAdv *advSett;

    ISPC::ParameterList *pResult;

    bool _isVLWidget;
    QString _css;

    void init();
    void clearOutput();
    // returns EXIT_FAILURE if loading failed because line size is too big
    int addResult(lsh_output *pResult);
	void initInputFilesTable();

	int currentFileIndex;

public:
    //QString outFile;
    std::map<int, QString> getOutputFiles();
    unsigned int getBitsPerDiff(LSH_TEMP temp) const;
    unsigned int getWBScale(LSH_TEMP temp) const;
    /** if hwv is HW_UNKNOWN will choose a hw_version from compiled values */
    explicit LSHTuning(GenConfig *genConfig,
    enum HW_VERSION hwv, bool isVLWidget = false,
        QWidget *parent = 0);
    virtual ~LSHTuning();

    //QString getOutputFilename();

    virtual QWidget *getDisplayWidget();
    virtual QWidget *getResultWidget();

    void changeCSS(const QString &css);

signals:
    void compute(const lsh_input *inputs);

public slots:
    virtual QString checkDependencies() const;
    int saveMatrix(const QString &dirPath);
    int saveTargetMSE(const QString &filename = "");
    virtual int saveResult(const QString &filename = "");

    virtual void saveParameters(ISPC::ParameterList &list) const
        throw(QtExtra::Exception);

    int tuning();
    int reset();
    void computationFinished(LSHCompute *toStop);

	void advSettLookUp(bool adv = true);
    void advSettLookUpPreview();

	void lookUpConfiguration();
	void previewInputFile();

    void logMessage(const QString &message);
    void showLog();
    void showPreview();

    void retranslate();
};

/** @brief Used for generation LSH deshading grids in command line mode.
 *  Inherits from @ref tuningParser and uses it for parsing CLI options
 *
 * @ingroup VISION_TUNING
 */
class LSHTuningNoGUI : public QObject
{
    Q_OBJECT

private:
    /** @brief Parses class specific parameters from CLI */
    QCommandLineParser parser;

    /** @brief Parameters common for all tuning sub-classes */
    const struct commonParams &commonParams;


    /** @brief  input paramteres provided for @ref LSHCompute */
    lsh_input *inputs;

    /** @brief LSHCopmute does all necesary calculations, emits finished signal
     *  when computation is ready, and returns result matrices in output map */
    LSHCompute *pCompute;

    /** @brief If set to 'true' casues that copy of LSH deshading matrix is
     *  stored in CSV form  */
    bool saveAsTxt;

    // --- LSH specific command line options ---
    /** @brief Calibration image of flat white surface. Can appear many times in
     *  CLI. */
    QCommandLineOption imageFile;

    /** @brief Iluminant temperature used for image shot.
     *  Can appear many times in CLI. Order of appearing reflects order of
     *  image files. */
    QCommandLineOption imageTemperature;

    /** @brief Iluminant temperature for which interpolated deshading grid will
     *  be generated on the base of at least 2 known calibration images */
    QCommandLineOption interTemperature;

    QCommandLineOption tileSize;

    /** @brief Algorithm used for generation deshading grid */
    QCommandLineOption algorithm;

    /** @brief If provided from CLI forces saving a copy of deshading grid in
     *  CSV format */
    QCommandLineOption saveTxt;
    // --- LSH specific command line options ---


    /** @brief Inits parameters that @ref parser will be aware of */
    void initTuneSpecificParams();

    /** @brief Validates CLI paramters specific for LSHTuningNoGUI, then
     *  stores them in @ref lsh_input
     *  @return true if parameters are valid
     *  */
    bool validateTuneSpecificParams();

public:
    explicit LSHTuningNoGUI(const struct commonParams &params,
        QObject *parent = 0);

    ~LSHTuningNoGUI();

    /** @brief Initiates LSH calculation
     *  @return EXIT_FAILURE if calculation cannot be start
     * */
    int calculateLSHmatrix();

    /** @brief Parses CLI options, instatiates @ref pCompute, prepares all stuf
     *  needed to start LSH calculation */
    void parseCliArgs(const QStringList &cliArgs);

signals:
    /** @brief signal sent to LSHCompute which starts deshading grid
     *  calculation */
    void compute(const lsh_input *inputs);

    /** @brief  signal sent to CliTune object, when  computations are finished
     *  and results are saved */
    void finished();

    /** @brief Forwards log messages to CliTune */
	void logMessage(const QString &message);

public slots:
    /** @brief slot for LSHCompute used when computations are finished */
    void computationFinished(LSHCompute *compute);
};

#endif /* LSHTUNING_HPP */
