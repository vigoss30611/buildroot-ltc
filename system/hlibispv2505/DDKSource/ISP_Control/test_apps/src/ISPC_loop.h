
#ifndef ISPC_LOOP_H_
#define ISPC_LOOP_H_

#include <img_types.h>

#include <ispc/Camera.h>
#include <ispc/ControlAWB.h>
#include <ispc/ControlAWB_Planckian.h>

/**
 * @return seconds for the FPS calculation
 */
double currTime();
void printUsage();
/**
 * @brief Enable mode to allow proper key capture in the application
 */
void enableRawMode(struct termios &orig_term_attr);
/**
 * @brief Disable mode to allow proper key capture in the application
 */
void disableRawMode(struct termios &orig_term_attr);
/**
 * @brief read key
 *
 * Could use fgetc but does not work with some versions of uclibc
 */
char getch(void);
int run(int argc, char *argv[]);

struct DemoConfiguration
{
    typedef enum {
        AE_FLICKER_OFF=0,
        AE_FLICKER_AUTO,
        AE_FLICKER_50HZ,
        AE_FLICKER_60HZ,
        AE_FLICKER_MAX
    } aeFlickerSetting_t;

    typedef enum {
        WB_NONE = ISPC::ControlAWB::WB_NONE,
        WB_AC = ISPC::ControlAWB::WB_AC,  /**< @brief Average colour */
        WB_WP = ISPC::ControlAWB::WB_WP,  /**< @brief White Patch */
        WB_HLW = ISPC::ControlAWB::WB_HLW,  /**< @brief High Luminance White */
        WB_COMBINED = ISPC::ControlAWB::WB_COMBINED, /**< @brief Combined AC + WP + HLW */
        WB_PLANCKIAN /** <@brief Planckian locus method, available on HW 2.6 */
    } awbControlAlgorithm_t;

    // Auto Exposure
    bool controlAE;             ///< @brief Create a control AE object or not
    double targetBrightness;    ///< @brief target value for the ControlAE brightness
    bool flickerRejection;      ///< @brief Enable/Disable Flicker Rejection
    bool monitorAE;             ///< @brief Enable AE algorithm efficiency monitoring
    bool autoFlickerRejection;  ///< @brief Enable/Disable usage of HW detected flicker frequency
    double flickerRejectionFreq; ///< @brief configured flicker frequency for manual mode
    aeFlickerSetting_t currentAeFlicker; ///< @brief current flicker rejection mode

    // Tone Mapper
    bool controlTNM;            ///< @brief Create a control TNM object or not
    bool autoToneMapping;       ///< @brief Enable computation of the tone mapping curve
    bool localToneMapping;      ///< @brief Enable/Disable local tone mapping
    bool adaptiveToneMapping;       ///< @brief Enable/Disable tone mapping auto strength control

    // Denoiser
    bool controlDNS;            ///< @brief Create a control DNS object or not
    bool autoDenoiserISO;       ///< @brief Enable/Disable denoiser ISO control

    // Light Based Controls
    bool controlLBC;            ///< @brief Create a control LBC object or not
    bool autoLBC;               ///< @brief Enable/Disable Light Based Control

    // Auto Focus
    bool controlAF;             ///< @brief Create a control AF object or not
    bool monitorAF;             ///< @brief Enable AF algorithm efficiency monitoring

    // White Balance
    awbControlAlgorithm_t controlModuleAWB;
    bool controlAWB;            ///< @brief Create a control AWB object or not
    bool enableAWB;             ///< @brief Enable/Disable AWB Control
    int targetAWB;              ///< @brief forces one of the AWB as CCM (index from 1) (if 0 forces identity, if <0 does not change)
    awbControlAlgorithm_t AWBAlgorithm; ///< @brief choose which algorithm to use - default is combined
    /** @brief Enabled WBTS mode */
    bool WBTSEnabled;
    bool WBTSEnabledOverride;
    /** @brief base for geometric weight generation algorithm */
    float WBTSWeightsBase;
    bool WBTSWeightsBaseOverride;
    /** @brief time to settle awb temporally */
    int WBTSTemporalStretch;
    bool WBTSTemporalStretchOverride;
    /** @brief choose features e.g. FlashFiltering, snap if close etc. */
    unsigned int WBTSFeatures;
    bool WBTSFeaturesOverride;
    bool monitorAWB; ///< @brief Enable AWB algorithm efficiency monitoring

    // Lensh de-Shading
    bool controlLSH;    ///< @brief Create a control LSH object or not
    bool bEnableLSH;    ///< @brief Enable/Disable LSH Control

    /** @brief Enable xxx module performance measurement */
    bool measureAAA;
    bool measureAF;
    bool measureAE;
    bool measureAWB;
    bool measureMODULES;

    // if true the output will be generated with every perf update, if false only summary at the end
    bool measureVerbose;

    // always enable display output unless saving DE
    bool alwaysHasDisplay;
    // enable update ASAP in the ISPC::Camera
    bool updateASAP;

    // to know if we should allow other formats than RGB - changes when a key is pressed, RGB is always savable
    bool bSaveDisplay;
    bool bSaveEncoder;
    bool bSaveDE;
    bool bSaveHDR;
    bool bSaveRaw2D;

    // other controls
    bool bEnableDPF;

    bool bResetCapture; // to know we need to reset to original output

    bool bTestStopStart; // test stop/start capture glitch after enqueue

    unsigned long ulEnqueueDelay;
    unsigned long ulRandomDelayLow;
    unsigned long ulRandomDelayHigh;

    // original formats loaded from the config file
    ePxlFormat originalYUV;
    ePxlFormat originalRGB; // we force RGB to be RGB_888_32 to be displayable on PDP
    ePxlFormat originalBayer;
    CI_INOUT_POINTS originalDEPoint;
    ePxlFormat originalHDR; // HDR only has 1 format
    ePxlFormat originalTiff;

    // other info from command line
    int nBuffers;
    int sensorMode;
    IMG_BOOL8 sensorFlip[2]; // horizontal and vertical
    unsigned int uiSensorInitExposure; // from parameters and also used when restarting to store current exposure
    // using IMG_FLOAT to be available in DYNCMD otherwise would be double
    IMG_FLOAT flSensorInitGain; // from parameters and also used when restarting to store current gain

    char *pszFelixSetupArgsFile;
    char *pszSensor;
    char *pszInputFLX; // for data generators only
    unsigned int gasket; // for data generator only
    unsigned int aBlanking[2]; // for data generator only

    IMG_BOOL8 useLocalBuffers;

    DemoConfiguration();

    ~DemoConfiguration();

    ISPC::ControlAWB * instantiateControlAwb(IMG_UINT8 hwMajor, IMG_UINT8 hwMinor);

    ISPC::ControlAWB * getControlAwb(ISPC::Camera* camera) const;

    // generateDefault if format is PXL_NONE
    void loadFormats(const ISPC::ParameterList &parameters, bool generateDefault=true);

    /**
     * @brief Apply a configuration to the control loop algorithms
     * @param
     */
    void applyConfiguration(ISPC::Camera &cam);

    int loadParameters(int argc, char *argv[]);

    /**
     * @brief loop through all defined flicker rejection modes
     */
    void nextAeFlickerMode();

    /**
     * @brief get string for current flicker rejection mode
     */
    std::string getFlickerModeStr() const;

    /**
     * @brief obtain current mode from ControlAE settings
     */
    aeFlickerSetting_t getFlickerModeFromState();
};

struct LoopCamera
{
    ISPC::Camera *camera;
    std::list<IMG_UINT32> buffersIds;
#ifdef USE_DMABUF
    std::list<IMG_HANDLE> buffers;
#endif
    std::vector<IMG_UINT32> listMatrixId;
    IMG_UINT32 currMatrixId;  // 0 means no matrix loaded

    const DemoConfiguration &config;
    unsigned int missedFrames;

    IMG_UINT8 hwMajor;
    IMG_UINT8 hwMinor;

    enum IsDataGenerator { NO_DG = 0, INT_DG, EXT_DG };
    enum IsDataGenerator isDatagen;

    LoopCamera(DemoConfiguration &_config);

    ~LoopCamera();

    void printInfo();

    int startCapture();

    int stopCapture();

    void saveResults(const ISPC::Shot &shot, DemoConfiguration &config);

    void printInfo(const ISPC::Shot &shot, const DemoConfiguration &config, double clockMhz = 40.0); // cannot be const because modifies missedFrames

#ifdef USE_DMABUF
    IMG_SIZE calcBufferSize(const PIXELTYPE& pixelType, bool bTiling=false);
    IMG_RESULT allocAndImport(const PIXELTYPE& bufferType,
            const CI_BUFFTYPE eBuffer,
            bool isTiled,
            IMG_HANDLE* buffer = NULL,
            IMG_UINT32* bufferId = NULL);
#endif
};

#endif /* ISPC_LOOP_H_ */
