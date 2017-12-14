#ifndef __TCPCOMMAND_H__
#define __TCPCOMMAND_H__

#define ITTD_VERSION                (0x00020004)
#define QSDK_VERSION                (0x00020302)

#define MODID_RANGE                 (9999)
#define MODID_GEN_BASE              (0)
#define MODID_ISP_BASE              (10000)
#define MODID_ISPOST_BASE           (20000)
#define MODID_H264_BASE             (30000)

#define SET_PRECISION               (1000.0f)  //1000000.0f
#define SET_VALUE_PRECISION         (1000)     //1000000
#define SET_SHOW_VALUE_PRECISION    (0.001l)   //0.000001l

#define NOT_SUPPORT_CMD             (0xFF)

enum GEN_ID
{
    GEN_GET_ITTD_VERSION = 1,
    GEN_GET_QSDK_VERSION,
    GEN_SET_DEBUG_INFO_ENABLE,
    GEN_SET_JSON_FILE,
    GEN_GET_JSON_FILE,
    GEN_GET_ISP_FLX_FILE,
    GEN_GET_ISP_RAW_FILE,
    GEN_GET_ISP_YUV_FILE,
    GEN_GET_ISPOST_YUV_FILE,
    GEN_SET_ISP_SETTING_FILE,
    GEN_GET_ISP_SETTING_FILE,
    GEN_GET_ISP_SETTING_SAVE_FILE,
    GEN_GET_GRID_DATA_FILE,
    GEN_SET_GRID_DATA_FILE,
    GEN_GET_ISP_LSH_FILE,
    GEN_SET_ISP_LSH_FILE,
    GEN_SET_VIDEOBOX_CTRL,
    GEN_GET_FILE,
    GEN_SET_FILE,
    GEN_SET_REMOUNT_DEV_PARTITION,
};

enum ISP_ID
{
    ISP_IIF = 1, /**< @brief Imager interface */
    ISP_EXS, /**< @brief Encoder statistics */
    ISP_BLC, /**< @brief Black level correction */
    ISP_RLT, /**< @brief Raw Look Up table correction (HW v2 only) */
    ISP_LSH, /**< @brief Lens shading */
    ISP_WBC, /**< @brief White balance correction (part of LSH block in HW) */
    ISP_FOS, /**< @brief Focus statistics */
    ISP_DNS, /**< @brief Denoiser */
    ISP_DPF, /**< @brief Defective Pixel */
    ISP_ENS, /**< @brief Encoder Statistics */
    ISP_LCA, /**< @brief Lateral Chromatic Aberration */
    ISP_CCM, /**< @brief Color Correction */
    ISP_MGM, /**< @brief Main Gammut Mapper */
    ISP_GMA, /**< @brief Gamma Correction */
    ISP_WBS, /**< @brief White Balance Statistics */
    ISP_HIS, /**< @brief Histogram Statistics */
    ISP_R2Y, /**< @brief RGB to YUV conversion */
    ISP_MIE, /**< @brief Main Image Enhancer (Memory Colour part) */
    ISP_VIB, /**< @brief Vibrancy (part of MIE block in HW) */
    ISP_TNM, /**< @brief Tone Mapper */
    ISP_FLD, /**< @brief Flicker Detection */
    ISP_SHA, /**< @brief Sharpening */
    ISP_ESC, /**< @brief Encoder Scaler */
    ISP_DSC, /**< @brief Display Scaler */
    ISP_Y2R, /**< @brief YUV to RGB conversions */
    ISP_DGM, /**< @brief Display Gammut Maper */
    ISP_WBC_CCM, /**< @brief White balance correction (part of LSH block in HW) */
    ISP_AEC, /**< @brief AE Control */
    ISP_AWB, /**< @brief AWB Control */
    ISP_CMC, /**< @brief Color Mode Control */
    ISP_TNMC, /**< @brief Tone Mapper Control */
    ISP_IIC, /**< @brief Sensor's I2C */
    ISP_OUT, /**< @brief Output format setup (not a HW module but output pipeline setup parameters) */
    ISP_LBC, //???
    ISP_VER,
    ISP_DBG,
};

enum ISPOST_ID
{
    ISPOST_DN = 100, /**< @brief three direction denoise Tools */
    ISPOST_VER,
    ISPOST_DBG,
    ISPOST_GRID_PATH,
};

enum H26X_ID
{
	H26X_ENC = 200, /**< @brief h264/h265 encoder Tools */
	H26X_VER,
	H26X_DBG,
};

enum GRD_ID
{
    GRD_FGT = 500,  /**< @brief Fisheye Grid Tools */
    GRD_SLT,        /**< @brief Single-fisheye Lsh Tools */
    GRD_DCT,        /**< @brief Dual-fisheye Calibration Tools */

    GRD_DCF,        /**< @brief Dual-fisheye Calibration All-Flow */
    GRD_DCF_USER,        /**< @brief Dual-fisheye Calibration All-Flow user mode */
    // add more in this

    GRD_MAX
};

typedef enum _GEN_GET_ITTD_VER_CMD_VER {
    GEN_GET_ITTD_VER_CMD_VER_V0 = 0,
    GEN_GET_ITTD_VER_CMD_VER_MAX
} GEN_GET_ITTD_VER_CMD_VER, *PGEN_GET_ITTD_VER_CMD_VER;

typedef enum _GEN_GET_QSDK_VER_CMD_VER {
    GEN_GET_QSDK_VER_CMD_VER_V0 = 0,
    GEN_GET_QSDK_VER_CMD_VER_MAX
} GEN_GET_QSDK_VER_CMD_VER, *PGEN_GET_QSDK_VER_CMD_VER;

typedef enum _GEN_SET_DEBUG_INFO_ENABLE_CMD_VER {
    GEN_SET_DEBUG_INFO_ENABLE_CMD_VER_V0 = 0,
    GEN_SET_DEBUG_INFO_ENABLE_CMD_VER_MAX
} GEN_SET_DEBUG_INFO_ENABLE_CMD_VER, *PGEN_SET_DEBUG_INFO_ENABLE_CMD_VER;

typedef enum _GEN_SET_JSON_FILE_CMD_VER {
    GEN_SET_JSON_FILE_CMD_VER_V0 = 0,
    GEN_SET_JSON_FILE_CMD_VER_MAX
} GEN_SET_JSON_FILE_CMD_VER, *PGEN_SET_JSON_FILE_CMD_VER;

typedef enum _GEN_GET_JSON_FILE_CMD_VER {
    GEN_GET_JSON_FILE_CMD_VER_V0 = 0,
    GEN_GET_JSON_FILE_CMD_VER_MAX
} GEN_GET_JSON_FILE_CMD_VER, *PGEN_GET_JSON_FILE_CMD_VER;

typedef enum _GEN_GET_ISP_FLX_FILE_CMD_VER {
    GEN_GET_ISP_FLX_FILE_CMD_VER_V0 = 0,
    GEN_GET_ISP_FLX_FILE_CMD_VER_MAX
} GEN_GET_ISP_FLX_FILE_CMD_VER, *PGEN_GET_ISP_FLX_FILE_CMD_VER;

typedef enum _GEN_GET_ISP_RAW_FILE_CMD_VER {
    GEN_GET_ISP_RAW_FILE_CMD_VER_V0 = 0,
    GEN_GET_ISP_RAW_FILE_CMD_VER_MAX
} GEN_GET_ISP_RAW_FILE_CMD_VER, *PGEN_GET_ISP_RAW_FILE_CMD_VER;

typedef enum _GEN_GET_ISP_YUV_FILE_CMD_VER {
    GEN_GET_ISP_YUV_FILE_CMD_VER_V0 = 0,
    GEN_GET_ISP_YUV_FILE_CMD_VER_MAX
} GEN_GET_ISP_YUV_FILE_CMD_VER, *PGEN_GET_ISP_YUV_FILE_CMD_VER;

typedef enum _GEN_GET_ISPOST_YUV_FILE_CMD_VER {
    GEN_GET_ISPOST_YUV_FILE_CMD_VER_V0 = 0,
    GEN_GET_ISPOST_YUV_FILE_CMD_VER_MAX
} GEN_GET_ISPOST_YUV_FILE_CMD_VER, *PGEN_GET_ISPOST_YUV_FILE_CMD_VER;

typedef enum _GEN_SET_ISP_SETTING_FILE_CMD_VER {
    GEN_SET_ISP_SETTING_FILE_CMD_VER_V0 = 0,
    GEN_SET_ISP_SETTING_FILE_CMD_VER_MAX
} GEN_SET_ISP_SETTING_FILE_CMD_VER, *PGEN_SET_ISP_SETTING_FILE_CMD_VER;

typedef enum _GEN_GET_ISP_SETTING_FILE_CMD_VER {
    GEN_GET_ISP_SETTING_FILE_CMD_VER_V0 = 0,
    GEN_GET_ISP_SETTING_FILE_CMD_VER_MAX
} GEN_GET_ISP_SETTING_FILE_CMD_VER, *PGEN_GET_ISP_SETTING_FILE_CMD_VER;

typedef enum _GEN_GET_ISP_SETTING_SAVE_FILE_CMD_VER {
    GEN_GET_ISP_SETTING_SAVE_FILE_CMD_VER_V0 = 0,
    GEN_GET_ISP_SETTING_SAVE_FILE_CMD_VER_MAX
} GEN_GET_ISP_SETTING_SAVE_FILE_CMD_VER, *PGEN_GET_ISP_SETTING_SAVE_FILE_CMD_VER;

typedef enum _GEN_GET_GRID_DATA_FILE_CMD_VER {
    GEN_GET_GRID_DATA_FILE_CMD_VER_V0 = 0,
    GEN_GET_GRID_DATA_FILE_CMD_VER_MAX
} GEN_GET_GRID_DATA_FILE_CMD_VER, *PGEN_GET_GRID_DATA_FILE_CMD_VER;

typedef enum _GEN_SET_GRID_DATA_FILE_CMD_VER {
    GEN_SET_GRID_DATA_FILE_CMD_VER_V0 = 0,
    GEN_SET_GRID_DATA_FILE_CMD_VER_MAX
} GEN_SET_GRID_DATA_FILE_CMD_VER, *PGEN_SET_GRID_DATA_FILE_CMD_VER;

typedef enum _GEN_GET_ISP_LSH_FILE_CMD_VER {
    GEN_GET_ISP_LSH_FILE_CMD_VER_V0 = 0,
    GEN_GET_ISP_LSH_FILE_CMD_VER_MAX
} GEN_GET_ISP_LSH_FILE_CMD_VER, *PGEN_GET_ISP_LSH_FILE_CMD_VER;

typedef enum _GEN_SET_ISP_LSH_FILE_CMD_VER {
    GEN_SET_ISP_LSH_FILE_CMD_VER_V0 = 0,
    GEN_SET_ISP_LSH_FILE_CMD_VER_MAX
} GEN_SET_ISP_LSH_FILE_CMD_VER, *PGEN_SET_ISP_LSH_FILE_CMD_VER;

typedef enum _GEN_SET_VIDEOBOX_CTRL_CMD_VER {
    GEN_SET_VIDEOBOX_CTRL_CMD_VER_V0 = 0,
    GEN_SET_VIDEOBOX_CTRL_CMD_VER_MAX
} GEN_SET_VIDEOBOX_CTRL_CMD_VER, *PGEN_SET_VIDEOBOX_CTRL_CMD_VER;

typedef enum _GEN_GET_FILE_CMD_VER {
    GEN_GET_FILE_CMD_VER_V0 = 0,
    GEN_GET_FILE_CMD_VER_MAX
} GEN_GET_FILE_CMD_VER, *PGEN_GET_FILE_CMD_VER;

typedef enum _GEN_SET_FILE_CMD_VER {
    GEN_SET_FILE_CMD_VER_V0 = 0,
    GEN_SET_FILE_CMD_VER_MAX
} GEN_SET_FILE_CMD_VER, *PGEN_SET_FILE_CMD_VER;

typedef enum _GEN_SET_REMOUNT_DEV_PARTITION_CMD_VER {
    GEN_SET_REMOUNT_DEV_PARTITION_CMD_VER_V0 = 0,
    GEN_SET_REMOUNT_DEV_PARTITION_CMD_VER_MAX
} GEN_SET_REMOUNT_DEV_PARTITION_CMD_VER, *PGEN_SET_REMOUNT_DEV_PARTITION_CMD_VER;

typedef enum _ISP_IIF_CMD_VER {
    ISP_IIF_CMD_VER_V0 = 0,
    ISP_IIF_CMD_VER_MAX
} ISP_IIF_CMD_VER, *PISP_IIF_CMD_VER;

typedef enum _ISP_BLC_CMD_VER {
    ISP_BLC_CMD_VER_V0 = 0,
    ISP_BLC_CMD_VER_MAX
} ISP_BLC_CMD_VER, *PISP_BLC_CMD_VER;

typedef enum _ISP_RLT_CMD_VER {
    ISP_RLT_CMD_VER_V0 = 0,
    ISP_RLT_CMD_VER_MAX
} ISP_RLT_CMD_VER, *PISP_RLT_CMD_VER;

typedef enum _ISP_LSH_CMD_VER {
    ISP_LSH_CMD_VER_V0 = 0,
    ISP_LSH_CMD_VER_MAX
} ISP_LSH_CMD_VER, *PISP_LSH_CMD_VER;

typedef enum _ISP_DNS_CMD_VER {
    ISP_DNS_CMD_VER_V0 = 0,
    ISP_DNS_CMD_VER_MAX
} ISP_DNS_CMD_VER, *PISP_DNS_CMD_VER;

typedef enum _ISP_DPF_CMD_VER {
    ISP_DPF_CMD_VER_V0 = 0,
    ISP_DPF_CMD_VER_MAX
} ISP_DPF_CMD_VER, *PISP_DPF_CMD_VER;

typedef enum _ISP_LCA_CMD_VER {
    ISP_LCA_CMD_VER_V0 = 0,
    ISP_LCA_CMD_VER_MAX
} ISP_LCA_CMD_VER, *PISP_LCA_CMD_VER;

typedef enum _ISP_GMA_CMD_VER {
    ISP_GMA_CMD_VER_V0 = 0,
    ISP_GMA_CMD_VER_MAX
} ISP_GMA_CMD_VER, *PISP_GMA_CMD_VER;

typedef enum _ISP_WBS_CMD_VER {
    ISP_WBS_CMD_VER_V0 = 0,
    ISP_WBS_CMD_VER_MAX
} ISP_WBS_CMD_VER, *PISP_WBS_CMD_VER;

typedef enum _ISP_HIS_CMD_VER {
    ISP_HIS_CMD_VER_V0 = 0,
    ISP_HIS_CMD_VER_MAX
} ISP_HIS_CMD_VER, *PISP_HIS_CMD_VER;

typedef enum _ISP_R2Y_CMD_VER {
    ISP_R2Y_CMD_VER_V0 = 0,
    ISP_R2Y_CMD_VER_MAX
} ISP_R2Y_CMD_VER, *PISP_R2Y_CMD_VER;

typedef enum _ISP_TNM_CMD_VER {
    ISP_TNM_CMD_VER_V0 = 0,
    ISP_TNM_CMD_VER_MAX
} ISP_TNM_CMD_VER, *PISP_TNM_CMD_VER;

typedef enum _ISP_SHA_CMD_VER {
    ISP_SHA_CMD_VER_V0 = 0,
    ISP_SHA_CMD_VER_MAX
} ISP_SHA_CMD_VER, *PISP_SHA_CMD_VER;

typedef enum _ISP_WBC_CCM_CMD_VER {
    ISP_WBC_CCM_CMD_VER_V0 = 0,
    ISP_WBC_CCM_CMD_VER_MAX
} ISP_WBC_CCM_CMD_VER, *PISP_WBC_CCM_CMD_VER;

typedef enum _ISP_AEC_CMD_VER {
    ISP_AEC_CMD_VER_V0 = 0,
    ISP_AEC_CMD_VER_MAX
} ISP_AEC_CMD_VER, *PISP_AEC_CMD_VER;

typedef enum _ISP_AWB_CMD_VER {
    ISP_AWB_CMD_VER_V0 = 0,
    ISP_AWB_CMD_VER_MAX
} ISP_AWB_CMD_VER, *PISP_AWB_CMD_VER;

typedef enum _ISP_CMC_CMD_VER {
    ISP_CMC_CMD_VER_V0 = 0,
    ISP_CMC_CMD_VER_MAX
} ISP_CMC_CMD_VER, *PISP_CMC_CMD_VER;

typedef enum _ISP_TNMC_CMD_VER {
    ISP_TNMC_CMD_VER_V0 = 0,
    ISP_TNMC_CMD_VER_MAX
} ISP_TNMC_CMD_VER, *PISP_TNMC_CMD_VER;

typedef enum _ISP_IIC_CMD_VER {
    ISP_IIC_CMD_VER_V0 = 0,
    ISP_IIC_CMD_VER_MAX
} ISP_IIC_CMD_VER, *PISP_IIC_CMD_VER;

typedef enum _ISP_OUT_CMD_VER {
    ISP_OUT_CMD_VER_V0 = 0,
    ISP_OUT_CMD_VER_MAX
} ISP_OUT_CMD_VER, *PISP_OUT_CMD_VER;

typedef enum _ISP_VER_CMD_VER {
    ISP_VER_CMD_VER_V0 = 0,
    ISP_VER_CMD_VER_MAX
} ISP_VER_CMD_VER, *PISP_VER_CMD_VER;

typedef enum _ISP_DBG_CMD_VER {
    ISP_DBG_CMD_VER_V0 = 0,
    ISP_DBG_CMD_VER_MAX
} ISP_DBG_CMD_VER, *PISP_DBG_CMD_VER;

typedef enum _ISPOST_DN_CMD_VER
{
    ISPOST_DN_CMD_VER_V0 = 0,
    ISPOST_DN_CMD_VER_MAX
} ISPOST_DN_CMD_VER, *PISPOST_DN_CMD_VER;

typedef enum _ISPOST_VER_CMD_VER {
    ISPOST_VER_CMD_VER_V0 = 0,
    ISPOST_VER_CMD_VER_MAX
} ISPOST_VER_CMD_VER, *PISPOST_VER_CMD_VER;

typedef enum _ISPOST_DBG_CMD_VER {
    ISPOST_DBG_CMD_VER_V0 = 0,
    ISPOST_DBG_CMD_VER_MAX
} ISPOST_DBG_CMD_VER, *PISPOST_DBG_CMD_VER;

typedef enum _ISPOST_GRID_PATH_CMD_VER {
    ISPOST_GRID_PATH_CMD_VER_V0 = 0,
    ISPOST_GRID_PATH_CMD_VER_MAX
} ISPOST_GRID_PATH_CMD_VER, *PISPOST_GRID_PATH_CMD_VER;

typedef enum _H26X_ENC_CMD_VER {
    H26X_ENC_CMD_VER_V0 = 0,
    H26X_ENC_CMD_VER_MAX
} H26X_ENC_CMD_VER, *PH26X_ENC_CMD_VER;

typedef enum _H26X_VER_CMD_VER {
    H26X_VER_CMD_VER_V0 = 0,
    H26X_VER_CMD_VER_MAX
} H26X_VER_CMD_VER, *PH26X_VER_CMD_VER;

typedef enum _H26X_DBG_CMD_VER {
    H26X_DBG_CMD_VER_V0 = 0,
    H26X_DBG_CMD_VER_MAX
} H26X_DBG_CMD_VER, *PH26X_DBG_CMD_VER;

typedef enum _QSDK_CMD_IDX {
    QSDK_CMD_IDX_GEN_GET_ITTD_VERSION = 0,
    QSDK_CMD_IDX_GEN_GET_QSDK_VERSION,
    QSDK_CMD_IDX_GEN_SET_DEBUG_INFO_ENABLE,
    QSDK_CMD_IDX_GEN_SET_JSON_FILE,
    QSDK_CMD_IDX_GEN_GET_JSON_FILE,
    QSDK_CMD_IDX_GEN_GET_ISP_FLX_FILE,
    QSDK_CMD_IDX_GEN_GET_ISP_RAW_FILE,
    QSDK_CMD_IDX_GEN_GET_ISP_YUV_FILE,
    QSDK_CMD_IDX_GEN_GET_ISPOST_YUV_FILE,
    QSDK_CMD_IDX_GEN_SET_ISP_SETTING_FILE,
    QSDK_CMD_IDX_GEN_GET_ISP_SETTING_FILE,
    QSDK_CMD_IDX_GEN_GET_ISP_SETTING_SAVE_FILE,
    QSDK_CMD_IDX_GEN_GET_GRID_DATA_FILE,
    QSDK_CMD_IDX_GEN_SET_GRID_DATA_FILE,
    QSDK_CMD_IDX_ISP_IIF,
    QSDK_CMD_IDX_ISP_BLC,
    QSDK_CMD_IDX_ISP_RLT,
    QSDK_CMD_IDX_ISP_LSH,
    QSDK_CMD_IDX_ISP_DNS,
    QSDK_CMD_IDX_ISP_DPF,
    QSDK_CMD_IDX_ISP_LCA,
    QSDK_CMD_IDX_ISP_GMA,
    QSDK_CMD_IDX_ISP_WBS,
    QSDK_CMD_IDX_ISP_HIS,
    QSDK_CMD_IDX_ISP_R2Y,
    QSDK_CMD_IDX_ISP_TNM,
    QSDK_CMD_IDX_ISP_SHA,
    QSDK_CMD_IDX_ISP_WBC_CCM,
    QSDK_CMD_IDX_ISP_AEC,
    QSDK_CMD_IDX_ISP_AWB,
    QSDK_CMD_IDX_ISP_CMC,
    QSDK_CMD_IDX_ISP_TNMC,
    QSDK_CMD_IDX_ISP_IIC,
    QSDK_CMD_IDX_ISP_VER,
    QSDK_CMD_IDX_ISP_DBG,
    QSDK_CMD_IDX_ISPOST_DN,
    QSDK_CMD_IDX_ISPOST_VER,
    QSDK_CMD_IDX_ISPOST_DBG,
    QSDK_CMD_IDX_ISPOST_GRID_PATH,
    QSDK_CMD_IDX_H26X_ENC,
    QSDK_CMD_IDX_H26X_VER,
    QSDK_CMD_IDX_H26X_DBG,
    QSDK_CMD_IDX_ISP_OUT,
    QSDK_CMD_IDX_GEN_GET_ISP_LSH_FILE,
    QSDK_CMD_IDX_GEN_SET_ISP_LSH_FILE,
    QSDK_CMD_IDX_GEN_SET_VIDEOBOX_CTRL,
    QSDK_CMD_IDX_GEN_GET_FILE,
    QSDK_CMD_IDX_GEN_SET_FILE,
    QSDK_CMD_IDX_GEN_SET_REMOUNT_DEV_PARTITION,
    QSDK_CMD_IDX_MAX
} QSDK_CMD_IDX, *PQSDK_CMD_IDX;


//=================================================================================================
// TCPCommand
//=================================================================================================
#define CMD_DIR_MASK                            (0x40000000)
#define CMD_DIR_LSBMASK                         (0x00000001)
#define CMD_DIR_SHIFT                           (30)
#define CMD_DIR_LENGTH                          (1)

#define CMD_TYPE_MASK                           (0x30000000)
#define CMD_TYPE_LSBMASK                        (0x00000003)
#define CMD_TYPE_SHIFT                          (28)
#define CMD_TYPE_LENGTH                         (2)

#define CMD_GROUP_IDX_MASK                      (0x0f000000)
#define CMD_GROUP_IDX_LSBMASK                   (0x0000000f)
#define CMD_GROUP_IDX_SHIFT                     (24)
#define CMD_GROUP_IDX_LENGTH                    (4)

#define CMD_PARAM_ORDER_MASK                    (0x0000ffff)
#define CMD_PARAM_ORDER_LSBMASK                 (0x0000ffff)
#define CMD_PARAM_ORDER_SHIFT                   (0)
#define CMD_PARAM_ORDER_LENGTH                  (16)

#define CMD_DATA_BUF_SIZE                       (1024 * 12)
#define CMD_FILE_BUF_SIZE                       (260)

typedef enum _CMD_DIR {
    CMD_DIR_GET = 0,
    CMD_DIR_SET,
    CMD_DIR_MAX
} CMD_DIR, *PCMD_DIR;

typedef enum _CMD_TYPE {
    CMD_TYPE_SINGLE = 0,
    CMD_TYPE_GROUP,
    CMD_TYPE_GROUP_ASAP,
    CMD_TYPE_ALL,
    CMD_TYPE_FILE,
    CMD_TYPE_MAX
} CMD_TYPE, *PCMD_TYPE;

typedef union _UTLONGLONG {
    long long llValue;
    unsigned long long ullValue;
    double dValue;
    long lValue;
    unsigned long ulValue;
    float fValue;
    short int iValue;
    unsigned short int uiValue;
    char chValue;
    unsigned char byValue;
    bool bValue;
} UTLONGLONG, *PUTLONGLONG;

typedef union _TCP_CMD {
    unsigned long ui32Value;
    struct {
        unsigned long ui16ParamOrder:16;    //[15:0]:parameter order.
        unsigned long ui8Rev16:8;           //[23:16]:revered.
        unsigned long ui4GroupIdx:4;        //[27:24}:group index.
        unsigned long ui3Type:3;            //[30:28]:command type.
        unsigned long ui1Dir:1;             //[31]:command direction.
    };
} TCP_CMD, *PTCP_CMD;

typedef union _UTCMD_APPEND {
    unsigned long aulData[5];
} UTCMD_APPEND, *PUUTCMD_APPEND;

typedef struct {
    unsigned short mod;
    TCP_CMD cmd;
    unsigned long ver;
    unsigned int datasize;
    UTLONGLONG val;
    UTCMD_APPEND app;
} TCPCommand;

typedef struct {
    TCPCommand tcpcmd;
    unsigned char buffer[CMD_DATA_BUF_SIZE];
} TCPCmdData;

typedef struct {
    unsigned long ulVersion;
    unsigned char uchQsdkCmdTbl[256];
} QsdkVersion;

/* Following are IQ tuning structure */
//typedef signed char __int8;
//typedef unsigned char __uint8;
//typedef signed short int __int16;
//typedef unsigned short int __uint16;
//typedef signed int __int32;
//typedef unsigned int __uint32;
//
//typedef __int8          IMG_INT8;
//typedef __int16         IMG_INT16;
//typedef __int32         IMG_INT32;
//
//typedef __uint8         IMG_UINT8;
//typedef __uint16        IMG_UINT16;
//typedef __uint32        IMG_UINT32;
//
//typedef int             IMG_INT;
//typedef unsigned int    IMG_UINT;
//
//#define AWS_MAX_LINES 5
//
//typedef struct curveLine {
//    double fBoundary;
//    double fXcoeff;
//    double fYcoeff;
//    double fOffset;
//} curveLine_t;
//
//typedef struct _ISP_MDLE_AWS {
//    bool enabled;
//    bool debugMode;
//    double fLog2_R_Qeff;
//    double fLog2_B_Qeff;
//    double fRedDarkThresh;
//    double fBlueDarkThresh;
//    double fGreenDarkThresh;
//    double fRedClipThresh;
//    double fBlueClipThresh;
//    double fGreenClipThresh;
//    double fBbDist;
//    IMG_UINT16 ui16GridStartRow;
//    IMG_UINT16 ui16GridStartColumn;
//    IMG_UINT16 ui16TileWidth;
//    IMG_UINT16 ui16TileHeight;
//    int nCurvesNum;
//    curveLine_t aCurves[AWS_MAX_LINES];
//} ISP_MDLE_AWS, *PISP_MDLE_AWS;
//

//=================================================================================================
// BLC module
//=================================================================================================
typedef enum _ISP_MDLE_BLC_ORDER {
    ISP_MDLE_BLC_ORDER_R = 0,
    ISP_MDLE_BLC_ORDER_GR,
    ISP_MDLE_BLC_ORDER_GB,
    ISP_MDLE_BLC_ORDER_B,
    ISP_MDLE_BLC_ORDER_MAX
} ISP_MDLE_BLC_ORDER, *PISP_MDLE_BLC_ORDER;

typedef struct _ISP_MDLE_BLC {
    signed int iSensorBlack[4];
    unsigned int uiSystemBlack;
} ISP_MDLE_BLC, *PISP_MDLE_BLC;

//=================================================================================================
// TemperatureCorrection Auxiliary.
//=================================================================================================
typedef struct _ISP_AUX_TC {
    /** @brief Illuminant temperature this correction correspond to */
    double dTemperature;
    /** @brief Color correction matrix coefficients */
    double dCoefficients[3][3];
    /** @brief Color correction offsets */
    double dOffsets[3];
    /** @brief Channel gains for WB correction */
    double dGains[4];
} ISP_AUX_TC, *PISP_AUX_TC;

//=================================================================================================
// CCM module
//=================================================================================================
typedef enum _ISP_MDLE_CCM_ORDER {
    ISP_MDLE_CCM_ORDER_MATRIX_00_RR = 0,
    ISP_MDLE_CCM_ORDER_MATRIX_01_RG,
    ISP_MDLE_CCM_ORDER_MATRIX_02_RB,
    ISP_MDLE_CCM_ORDER_MATRIX_10_GR,
    ISP_MDLE_CCM_ORDER_MATRIX_11_GG,
    ISP_MDLE_CCM_ORDER_MATRIX_12_GB,
    ISP_MDLE_CCM_ORDER_MATRIX_20_BR,
    ISP_MDLE_CCM_ORDER_MATRIX_21_BG,
    ISP_MDLE_CCM_ORDER_MATRIX_22_BB,
    ISP_MDLE_CCM_ORDER_OFFSET_R,
    ISP_MDLE_CCM_ORDER_OFFSET_G,
    ISP_MDLE_CCM_ORDER_OFFSET_B,
    ISP_MDLE_CCM_ORDER_MAX
} ISP_MDLE_CCM_ORDER, *PISP_MDLE_CCM_ORDER;

typedef struct _ISP_MDLE_CCM {
    double adMatrix[9];
    double adOffset[3];
} ISP_MDLE_CCM, *PISP_MDLE_CCM;

//=================================================================================================
// DNS module
//=================================================================================================
typedef enum _ISP_MDLE_DNS_ORDER {
    ISP_MDLE_DNS_ORDER_COMBINE = 0,
    ISP_MDLE_DNS_ORDER_STRENGTH,
    ISP_MDLE_DNS_ORDER_GREY_SCALE_THRESHOLD,
    ISP_MDLE_DNS_ORDER_SENSOR_GAIN,
    ISP_MDLE_DNS_ORDER_SENSOR_BITDEPTH,
    ISP_MDLE_DNS_ORDER_SENSOR_WELL_DEPTH,
    ISP_MDLE_DNS_ORDER_SENSOR_READ_NOISE,
    ISP_MDLE_DNS_ORDER_MAX
} ISP_MDLE_DNS_ORDER, *PISP_MDLE_DNS_ORDER;

typedef struct _ISP_MDLE_DNS {
    bool bCombine;
    double dStrength;
    double dGreyscaleThreshold;
    double dSensorGain;
    unsigned int uiSensorBitdepth;
    unsigned int uiSensorWellDepth;
    double dSensorReadNoise;
} ISP_MDLE_DNS, *PISP_MDLE_DNS;

//=================================================================================================
// DPF module
//=================================================================================================
typedef enum _ISP_MDLE_DPF_ORDER {
    ISP_MDLE_DPF_ORDER_DETECT = 0,
    ISP_MDLE_DPF_ORDER_THRESHOLD,
    ISP_MDLE_DPF_ORDER_WEIGHT,
    ISP_MDLE_DPF_ORDER_NB_DEFECTS,
    //ISP_MDLE_DPF_ORDER_DEFECT_MAP,
    ISP_MDLE_DPF_ORDER_MAX
} ISP_MDLE_DPF_ORDER, *PISP_MDLE_DPF_ORDER;

typedef struct _ISP_MDLE_DPF {
    bool bDetect;
    //bool bRead;
    //bool bWrite;
    unsigned int uiThreshold;
    double dWeight;
    unsigned int uiNbDefects;
    //std::string readMapFilename;
    //unsigned short int *pDefectMap;
} ISP_MDLE_DPF, *PISP_MDLE_DPF;

//enum ScalerRectType {
//    SCALER_RECT_CLIP = 0,
//    SCALER_RECT_CROP,
//    SCALER_RECT_SIZE
//};
//
//typedef struct _ISP_MDLE_DSC {
//    bool bAdjustTaps;
//    double aPitch[2];
//    enum ScalerRectType eRectType;
//    IMG_UINT32 aRect[4];
//} ISP_MDLE_DSC, *PISP_MDLE_DSC;
//
//typedef struct _ISP_MDLE_ENS {
//    bool bEnable;
//    IMG_UINT32 ui32NLines;
//    IMG_UINT32 ui32SubsamplingFactor;
//} ISP_MDLE_ENS, *PISP_MDLE_ENS;
//
//typedef struct _ISP_MDLE_ESC {
//    bool bAdjustTaps;
//    double aPitch[2];
//    enum ScalerRectType eRectType;
//    IMG_UINT32 aRect[4];
//    bool bChromaInterstitial;
//} ISP_MDLE_ESC, *PISP_MDLE_ESC;
//
//typedef struct _ISP_MDLE_EXS {
//    bool bEnableGlobal;
//    bool bEnableRegion;
//    IMG_UINT32 aGridStart[2];
//    IMG_UINT32 aGridTileSize[2];
//    IMG_INT i32PixelMax;
//} ISP_MDLE_EXS, *PISP_MDLE_EXS;
//
//typedef struct _ISP_MDLE_FLD {
//    bool bEnable;
//    double fFrameRate;
//    IMG_INT32 iVTotal;
//    IMG_INT32 iScheneChangeTH;
//    IMG_INT32 iMinPN;
//    IMG_INT32 iPN;
//    IMG_INT32 iNF_TH;
//    IMG_INT32 iCoeffDiffTH;
//    IMG_INT32 iRShift;
//    bool bReset;
//} ISP_MDLE_FLD, *PISP_MDLE_FLD;
//
//typedef struct _ISP_MDLE_FOS {
//    bool bEnableROI;
//    IMG_UINT32  aRoiStartCoord[2];
//    IMG_UINT32  aRoiEndCoord[2];
//    bool bEnableGrid;
//    IMG_UINT32  aGridStartCoord[2];
//    IMG_UINT32  aGridTileSize[2];
//} ISP_MDLE_FOS, *PISP_MDLE_FOS;
//

//=================================================================================================
// GMA module
//=================================================================================================
#define ISP_GMA_CURVE_PNT_CNT                   (63)
#define GAMMA_CURVE_MAX                         (1)
#define GAMMA_CURVE_SET_MAX                     (2)
#define GAMMA_CURVE_SHOW_MAX                    (47)

typedef enum _ISP_MDLE_GMA_ORDER {
    ISP_MDLE_GMA_ORDER_ENABLE = 0,
    ISP_MDLE_GMA_ORDER_USE_CUSTOM_GMA,
    ISP_MDLE_GMA_ORDER_CUSTOM_GMA_CURVE,
    ISP_MDLE_GMA_ORDER_MAX
} ISP_MDLE_GMA_ORDER, *PISP_MDLE_GMA_ORDER;

typedef struct _ISP_MDLE_GMA {
    bool bBypass;
    bool bUseCustomGma;
    unsigned short int uiCustomGmaCurve[ISP_GMA_CURVE_PNT_CNT];
} ISP_MDLE_GMA, *PISP_MDLE_GMA;

//typedef struct _ISP_MDLE_HIS {
//    bool bEnableGlobal;
//    bool bEnableROI;
//    IMG_UINT32  ui32InputOffset;
//    IMG_UINT32  ui32InputScale;
//    IMG_UINT32  aGridStartCoord[2];
//    IMG_UINT32  aGridTileSize[2];
//} ISP_MDLE_HIS, *PISP_MDLE_HIS;
//
//typedef struct _ISP_MDLE_IIF {
//    IMG_UINT32 aDecimation[2];
//    IMG_UINT32 aCropTL[2];
//    IMG_UINT32 aCropBR[2];
//} ISP_MDLE_IIF, *PISP_MDLE_IIF;
//
//#define LCA_CURVE_MAX 1024
//#define LCA_COEFFS_NO 3
//
//typedef struct _ISP_MDLE_LCA {
//    double aRedPoly_X[LCA_COEFFS_NO];
//    double aRedPoly_Y[LCA_COEFFS_NO];
//    double aBluePoly_X[LCA_COEFFS_NO];
//    double aBluePoly_Y[LCA_COEFFS_NO];
//    IMG_UINT32 aRedCenter[2];
//    IMG_UINT32 aBlueCenter[2];
//    IMG_UINT32 aShift[2];
//    IMG_UINT32 aDecimation[2];
//} ISP_MDLE_LCA, *PISP_MDLE_LCA;
//
//typedef struct _ISP_MDLE_LSH {
//    bool bEnableMatrix;
//    double aGradientX[4];
//    double aGradientY[4];
//} ISP_MDLE_LSH, *PISP_MDLE_LSH;
//
////#define MIE_VIBRANCY_N 32
//#define MIE_NUM_MEMCOLOURS 3
//#define MIE_NUM_SLICES 4
////#define MIE_GAUSS_SC_N (MIE_NUM_MEMCOLOURS*MIE_NUM_SLICES)
////#define MIE_GAUSS_GN_N (MIE_NUM_MEMCOLOURS*MIE_NUM_SLICES)
////#define MIE_SATMULT_NUMPOINTS_EXP 5
////#define MIE_SATMULT_NUMPOINTS 32
//
//typedef struct _ISP_MDLE_MIE {
//    double fBlackLevel;
//    bool bEnabled[MIE_NUM_MEMCOLOURS];
//    double aYGain[MIE_NUM_MEMCOLOURS][MIE_NUM_SLICES];
//    double aYMin[MIE_NUM_MEMCOLOURS];
//    double aYMax[MIE_NUM_MEMCOLOURS];
//    double aCbCenter[MIE_NUM_MEMCOLOURS];
//    double aCrCenter[MIE_NUM_MEMCOLOURS];
//    double aCExtent[MIE_NUM_MEMCOLOURS][MIE_NUM_SLICES];
//    double aCAspect[MIE_NUM_MEMCOLOURS];
//    double aCRotation[MIE_NUM_MEMCOLOURS];
//    double aOutBrightness[MIE_NUM_MEMCOLOURS];
//    double aOutContrast[MIE_NUM_MEMCOLOURS];
//    double aOutStaturation[MIE_NUM_MEMCOLOURS];
//    double aOutHue[MIE_NUM_MEMCOLOURS];
//} ISP_MDLE_MIE, *PISP_MDLE_MIE;
//
//typedef enum pxlFormat
//{
//    PXL_NONE = 0,
//    YVU_420_PL12_8,
//    YUV_420_PL12_8,
//    YVU_422_PL12_8,
//    YUV_422_PL12_8,
//    YVU_420_PL12_10,
//    YUV_420_PL12_10,
//    YVU_422_PL12_10,
//    YUV_422_PL12_10,
//    RGB_888_24,
//    RGB_888_32,
//    RGB_101010_32,
//    BGR_888_24,
//    BGR_888_32,
//    BGR_101010_32,
//    BGR_161616_64,
//    BAYER_RGGB_8,
//    BAYER_RGGB_10,
//    BAYER_RGGB_12,
//    BAYER_TIFF_10,
//    BAYER_TIFF_12,
//    PXL_ISP_444IL3YCrCb8,
//    PXL_ISP_444IL3YCbCr8,
//    PXL_ISP_444IL3YCrCb10,
//    PXL_ISP_444IL3YCbCr10,
//    PXL_N,
//    PXL_INVALID,
//} ePxlFormat;
//
//enum CI_INOUT_POINTS
//{
//    CI_INOUT_BLACK_LEVEL = 0,
//    CI_INOUT_FILTER_LINESTORE,
//    /*CI_INOUT_DEMOISAICER,
//    CI_INOUT_GAMA_CORRECTION,
//    CI_INOUT_HSBC,
//    CI_INOUT_TONE_MAPPER_LINESTORE,*/
//    CI_INOUT_NONE
//};
//
//typedef struct _ISP_MDLE_OUT {
//    ePxlFormat encoderType;
//    ePxlFormat displayType;
//    ePxlFormat dataExtractionType;
//    ePxlFormat hdrExtractionType;
//    ePxlFormat hdrInsertionType;
//    ePxlFormat raw2DExtractionType;
//    CI_INOUT_POINTS dataExtractionPoint;
//} ISP_MDLE_OUT, *PISP_MDLE_OUT;
//
//=================================================================================================
// R2Y module
//=================================================================================================
typedef enum _ISP_MDLE_R2Y_ORDER {
    ISP_MDLE_R2Y_ORDER_BRIGHTNESS = 0,
    ISP_MDLE_R2Y_ORDER_CONTRAST,
    ISP_MDLE_R2Y_ORDER_SATURATION,
    ISP_MDLE_R2Y_ORDER_HUE,
    ISP_MDLE_R2Y_ORDER_RANGE_MUL_Y,
    ISP_MDLE_R2Y_ORDER_RANGE_MUL_CB,
    ISP_MDLE_R2Y_ORDER_RANGE_MUL_CR,
    ISP_MDLE_R2Y_ORDER_OFFSET_U,
    ISP_MDLE_R2Y_ORDER_OFFSET_V,
    ISP_MDLE_R2Y_ORDER_MATRIX,
    ISP_MDLE_R2Y_ORDER_MAX
} ISP_MDLE_R2Y_ORDER, *PISP_MDLE_R2Y_ORDER;

typedef enum  _ISP_STD_MATRIX {
    ISP_STD_MATRIX_STD_MIN = 0,
    ISP_STD_MATRIX_BT601 = ISP_STD_MATRIX_STD_MIN,
    ISP_STD_MATRIX_BT709,
    ISP_STD_MATRIX_JFIF, /**< @warning Not supported by this module at the moment! */
    ISP_STD_MATRIX_STD_MAX = ISP_STD_MATRIX_JFIF
} ISP_STD_MATRIX, *PISP_STD_MATRIX;

typedef struct _ISP_MDLE_R2Y {
    double dBrightness;     //brief Brightness (offset)
    double dContrast;       //brief Contrast (luma gain)
    double dSaturation;     //brief Saturation (chroma grain)
    double dHue;            //brief [degrees] Hue shift
    double dRangeMult[3];   //brief Gain multipliers for Y,Cb,Cr (before output offset)
    double dOffsetU;        //brief Offset applied to U channel
    double dOffsetV;        //brief Offset applied to V channel
    ISP_STD_MATRIX uiMatrix;
} ISP_MDLE_R2Y, *PISP_MDLE_R2Y;

//#define RLT_SLICE_N_FIELDS (2)
//#define RLT_SLICE_N_ENTRIES (8)
//#define RLT_SLICE_N 4
//#define RLT_SLICE_N_POINTS (RLT_SLICE_N_ENTRIES * RLT_SLICE_N_FIELDS)
//#define RLT_N_POINTS (RLT_SLICE_N_POINTS * RLT_SLICE_N)
//
//enum Mode {
//    RLT_DISABLED = 0,
//    RLT_LINEAR,
//    RLT_CUBIC,
//};
//
//typedef struct _ISP_MDLE_RLT {
//    enum Mode eMode;
//    IMG_UINT16 aLUTPoints[RLT_SLICE_N][RLT_SLICE_N_POINTS];
//} ISP_MDLE_RLT, *PISP_MDLE_RLT;
//
//=================================================================================================
// SHA module
//=================================================================================================
typedef enum _ISP_MDLE_SHA_ORDER {
    ISP_MDLE_SHA_ORDER_RADIUS = 0,
    ISP_MDLE_SHA_ORDER_STRENGTH,
    ISP_MDLE_SHA_ORDER_THRESHOLD,
    ISP_MDLE_SHA_ORDER_DETAIL,
    ISP_MDLE_SHA_ORDER_EDGE_SCALE,
    ISP_MDLE_SHA_ORDER_EDGE_OFFSET,
    ISP_MDLE_SHA_ORDER_BYPASS_DENOISE,
    ISP_MDLE_SHA_ORDER_DENOISE_TAU,
    ISP_MDLE_SHA_ORDER_DENOISE_SIGMA,
    ISP_MDLE_SHA_ORDER_MAX
} ISP_MDLE_SHA_ORDER, *PISP_MDLE_SHA_ORDER;

typedef struct _ISP_MDLE_SHA {
    double dRadius;
    double dfStrength;
    double dThreshold;
    double dDetail;
    double dEdgeScale;
    double dEdgeOffset;
    bool  bBypassDenoise;
    double dDenoiseTau;
    double dDenoiseSigma;
} ISP_MDLE_SHA, *PISP_MDLE_SHA;

//=================================================================================================
// TNM module
//=================================================================================================
#define ISP_TNM_CURVE_NPOINTS                   (63)
#define ISP_TNM_CURVE_NPOINTS_MAX               (ISP_TNM_CURVE_NPOINTS + 1)
#define ISP_TNM_CURVE_MAX                       (1)

typedef enum _ISP_MDLE_TNM_ORDER {
    ISP_MDLE_TNM_ORDER_BYPASS = 0,
    ISP_MDLE_TNM_ORDER_IN_Y_MIN,
    ISP_MDLE_TNM_ORDER_IN_Y_MAX,
    ISP_MDLE_TNM_ORDER_OUT_Y_MIN,
    ISP_MDLE_TNM_ORDER_OUT_Y_MAX,
    ISP_MDLE_TNM_ORDER_WEIGHT_LOCAL,
    ISP_MDLE_TNM_ORDER_WEIGHT_LINE,
    ISP_MDLE_TNM_ORDER_FLAT_FACTOR,
    ISP_MDLE_TNM_ORDER_FLAT_MIN,
    ISP_MDLE_TNM_ORDER_COLOUR_CONFIDENCE,
    ISP_MDLE_TNM_ORDER_COLOUR_SATURATION,
    ISP_MDLE_TNM_ORDER_CURVE,
    ISP_MDLE_TNM_ORDER_MAX
} ISP_MDLE_TNM_ORDER, *PISP_MDLE_TNM_ORDER;

typedef struct _ISP_MDLE_TNM {
    bool bBypass;
    double adInY[2];
    double adOutY[2];
    double adOutC[2];
    double dWeightLocal;
    double dWeightLine;
    double dFlatFactor;
    double dFlatMin;
    double dColourConfidence;
    double dColourSaturation;
    double adCurve[ISP_TNM_CURVE_NPOINTS];
    bool bStaticCurve;
} ISP_MDLE_TNM, *PISP_MDLE_TNM;

//#define MIE_SATMULT_NUMPOINTS 32
//
//typedef struct _ISP_MDLE_VIB {
//    bool bVibrancyOn;
//    double aSaturationCurve[MIE_SATMULT_NUMPOINTS];
//} ISP_MDLE_VIB, *PISP_MDLE_VIB;
//
//=================================================================================================
// WBC module
//=================================================================================================
typedef enum _ISP_MDLE_WBC_ORDER {
    ISP_MDLE_WBC_ORDER_WB_GAIN_R = 0,
    ISP_MDLE_WBC_ORDER_WB_GAIN_GR,
    ISP_MDLE_WBC_ORDER_WB_GAIN_GB,
    ISP_MDLE_WBC_ORDER_WB_GAIN_B,
    ISP_MDLE_WBC_ORDER_WB_CLIP_R,
    ISP_MDLE_WBC_ORDER_WB_CLIP_GR,
    ISP_MDLE_WBC_ORDER_WB_CLIP_GB,
    ISP_MDLE_WBC_ORDER_WB_CLIP_B,
    ISP_MDLE_WBC_ORDER_MAX
} ISP_MDLE_WBC_ORDER, *PISP_MDLE_WBC_ORDER;

typedef struct _ISP_MDLE_WBC {
    double adWBGain[4];
    double adWBClip[4];
} ISP_MDLE_WBC, *PISP_MDLE_WBC;

//=================================================================================================
// WBC and CCM modules
//=================================================================================================
#define TEMPERATURE_CORRECTION_CNT_MAX          (6)
#define TEMPERATURE_CORRECTION_CNT_USE          (4)

typedef enum _ISP_MDLE_WBC_CCM_ORDER {
    ISP_MDLE_WBC_CCM_ORDER_WB_GAIN_R = 0,
    ISP_MDLE_WBC_CCM_ORDER_WB_GAIN_GR,
    ISP_MDLE_WBC_CCM_ORDER_WB_GAIN_GB,
    ISP_MDLE_WBC_CCM_ORDER_WB_GAIN_B,
    ISP_MDLE_WBC_CCM_ORDER_WB_CLIP_R,
    ISP_MDLE_WBC_CCM_ORDER_WB_CLIP_GR,
    ISP_MDLE_WBC_CCM_ORDER_WB_CLIP_GB,
    ISP_MDLE_WBC_CCM_ORDER_WB_CLIP_B,
    ISP_MDLE_WBC_CCM_ORDER_MATRIX_00_RR,
    ISP_MDLE_WBC_CCM_ORDER_MATRIX_01_RG,
    ISP_MDLE_WBC_CCM_ORDER_MATRIX_02_RB,
    ISP_MDLE_WBC_CCM_ORDER_MATRIX_10_GR,
    ISP_MDLE_WBC_CCM_ORDER_MATRIX_11_GG,
    ISP_MDLE_WBC_CCM_ORDER_MATRIX_12_GB,
    ISP_MDLE_WBC_CCM_ORDER_MATRIX_20_BR,
    ISP_MDLE_WBC_CCM_ORDER_MATRIX_21_BG,
    ISP_MDLE_WBC_CCM_ORDER_MATRIX_22_BB,
    ISP_MDLE_WBC_CCM_ORDER_OFFSET_R,
    ISP_MDLE_WBC_CCM_ORDER_OFFSET_G,
    ISP_MDLE_WBC_CCM_ORDER_OFFSET_B,
    ISP_MDLE_WBC_CCM_ORDER_MAX
} ISP_MDLE_WBC_CCM_ORDER, *PISP_MDLE_WBC_CCM_ORDER;

typedef struct _ISP_MDLE_WBC_CCM {
    ISP_MDLE_WBC WBC;
    ISP_MDLE_CCM CCM;
    unsigned int uiTemperatureCorrectionCount;
    ISP_AUX_TC TC[TEMPERATURE_CORRECTION_CNT_MAX];
} ISP_MDLE_WBC_CCM, *PISP_MDLE_WBC_CCM;

//enum WBC_MODES {
//    WBC_SATURATION = 0,
//    WBC_THRESHOLD,
//};
//
//typedef struct _ISP_MDLE_WBC_2_6 {
//    double afRGBGain[3];
//    double afRGBThreshold[3];
//    enum WBC_MODES eRGBThresholdMode;
//} ISP_MDLE_WBC_2_6, *PISP_MDLE_WBC_2_6;
//
//#define WBS_NUM_ROI 2
//
//typedef struct _ISP_MDLE_WBS {
//    IMG_UINT32 ui32NROIEnabled;
//    double  aRedMaxTH[WBS_NUM_ROI];
//    double  aGreenMaxTH[WBS_NUM_ROI];
//    double  aBlueMaxTH[WBS_NUM_ROI];
//    double  aYHLWTH[WBS_NUM_ROI];
//    IMG_UINT32 aROIStartCoord[WBS_NUM_ROI][2];
//    IMG_UINT32 aROIEndCoord[WBS_NUM_ROI][2];
//} ISP_MDLE_WBS, *PISP_MDLE_WBS;
//
//typedef struct _ISP_MDLE_Y2R {
//    double fBrightness;
//    double fContrast;
//    double fSaturation;
//    double fHue;
//    double aRangeMult[3];
//    double fOffsetU;
//    double fOffsetV;
//    StdMatrix eMatrix;
//} ISP_MDLE_Y2R, *PISP_MDLE_Y2R;
//
//=================================================================================================
// AE control
//=================================================================================================
#define ISP_HIS_REGION_HTILES                   (7)
#define ISP_HIS_REGION_VTILES                   (7)
#define AE_GRID_MAX                             (1)
#define AE_GRID_ITEM_MAX                        (7)

typedef enum _ISP_CTRL_AEC_ORDER {
    ISP_CTRL_AEC_ORDER_ENABLE,
    ISP_CTRL_AEC_ORDER_CURRENT_BRIGHTNESS,
    ISP_CTRL_AEC_ORDER_ORIGINAL_TARGET_BRIGHTNESS,
    ISP_CTRL_AEC_ORDER_TARGET_BRIGHTNESS,
    ISP_CTRL_AEC_ORDER_UPDATE_SPEED,
    ISP_CTRL_AEC_ORDER_FLICKER_REJECTION,
    ISP_CTRL_AEC_ORDER_AUTO_FLICKER_REJECTION,
    //ISP_CTRL_AEC_ORDER_AUTO_FLICKER_DETECTION_SUPPORTED,
    ISP_CTRL_AEC_ORDER_FLICKER_FREQ_CONFIG,
    ISP_CTRL_AEC_ORDER_FLICKER_FREQ,
    ISP_CTRL_AEC_ORDER_TARGET_BRACKET,
    ISP_CTRL_AEC_ORDER_MAX_AE_GAIN,
    ISP_CTRL_AEC_ORDER_MAX_SENSOR_GAIN,
    ISP_CTRL_AEC_ORDER_TARGET_AE_GAIN,
    ISP_CTRL_AEC_ORDER_MAX_AE_EXPOSURE,
    ISP_CTRL_AEC_ORDER_MAX_SENSOR_EXPOSURE,
    //ISP_CTRL_AEC_ORDER_SENSOR_FRAME_DURATION,
    ISP_CTRL_AEC_ORDER_GAIN,
    ISP_CTRL_AEC_ORDER_EXPOSURE,
    ISP_CTRL_AEC_ORDER_USE_FIXED_AE_GAIN,
    ISP_CTRL_AEC_ORDER_FIXED_AE_GAIN,
    ISP_CTRL_AEC_ORDER_USE_FIXED_AE_EXPOSURE,
    ISP_CTRL_AEC_ORDER_FIXED_AE_EXPOSURE,
    ISP_CTRL_AEC_ORDER_DO_AE,
    //ISP_CTRL_AEC_ORDER_WEIGHT_7X7,
    //ISP_CTRL_AEC_ORDER_WEIGHT_7X7_A,
    //ISP_CTRL_AEC_ORDER_WEIGHT_7X7_B,
    //ISP_CTRL_AEC_ORDER_BACKLIGHT_7X7,
    ISP_CTRL_AEC_ORDER_REGION_BRIGHTNESS,
    //ISP_CTRL_AEC_ORDER_REGION_OVEREXPOSE,
    //ISP_CTRL_AEC_ORDER_REGION_UNDEREXPOSE,
    //ISP_CTRL_AEC_ORDER_REGION_RATIO,
    ISP_CTRL_AEC_ORDER_OVER_UNDER_EXPOSE_DIFF,
    ISP_CTRL_AEC_ORDER_OVEREXPOSE_RATIO,
    ISP_CTRL_AEC_ORDER_MIDDLE_OVEREXPOSE_RATIO,
    ISP_CTRL_AEC_ORDER_UNDEREXPOSE_RATIO,
    ISP_CTRL_AEC_ORDER_AE_TARGET_MOVE_ENABLE,
    ISP_CTRL_AEC_ORDER_AE_TARGET_MOVE_METHOD,
    ISP_CTRL_AEC_ORDER_AE_TARGET_MAX,
    ISP_CTRL_AEC_ORDER_AE_TARGET_MIN,
    ISP_CTRL_AEC_ORDER_AE_TARGET_GAIN_NORMAL_MODE_MAX_LMT,
    ISP_CTRL_AEC_ORDER_AE_TARGET_MOVE_STEP,
    ISP_CTRL_AEC_ORDER_AE_TARGET_MIN_LMT,
    ISP_CTRL_AEC_ORDER_AE_TARGET_UP_OVER_THRESHOLD,
    ISP_CTRL_AEC_ORDER_AE_TARGET_UP_UNDER_THRESHOLD,
    ISP_CTRL_AEC_ORDER_AE_TARGET_DN_OVER_THRESHOLD,
    ISP_CTRL_AEC_ORDER_AE_TARGET_DN_UNDER_THRESHOLD,
    ISP_CTRL_AEC_ORDER_AE_EXPOSURE_METHOD,
    ISP_CTRL_AEC_ORDER_AE_TARGET_LOW_LUX_GAIN_ENTER,
    ISP_CTRL_AEC_ORDER_AE_TARGET_LOW_LUX_GAIN_EXIT,
    ISP_CTRL_AEC_ORDER_AE_TARGET_LOW_LUX_EXPOSURE_ENTER,
    ISP_CTRL_AEC_ORDER_AE_TARGET_LOW_LUX_FPS,
    ISP_CTRL_AEC_ORDER_AE_TARGET_NORMAL_FPS,
    ISP_CTRL_AEC_ORDER_AE_TARGET_MAX_FPS_LOCK_ENABLE,
    ISP_CTRL_AEC_ORDER_AE_BRIGHTNESS_METERING_METHOD,
    ISP_CTRL_AEC_ORDER_AE_REGION_DUCE,
    ISP_CTRL_AEC_ORDER_MAX
} ISP_CTRL_AEC_ORDER, *PISP_CTRL_AEC_ORDER;

typedef struct _ISP_CTRL_AE {
    bool bEnable;
    double dCurrentBrightness;
    double dOriginalTargetBrightness;
    double dTargetBrightness;
    double dUpdateSpeed;
    bool bFlickerRejection;
    bool bAutoFlickerRejection;
    //bool bAutoFlickerDetectionSupported;
    double dFlickerFreqConfig;
    double dFlickerFreq;
    double dTargetBracket;
#ifdef INFOTM_SUPPORT_DAUL_SENSOR_AE_PATCH
    bool underexposed;
#endif
    //bool configured;
    double dMaxAeGain;
    double dMaxSensorGain;
    double dTargetAeGain;
    unsigned int uiMaxAeExposure;
    unsigned int uiMaxSensorExposure;
    //double dSensorFrameDuration;
    double dGain;
    unsigned int uiExposure;
    bool bUseFixedAeGain;
    double dFixedAeGain;
    bool bUseFixedAeExposure;
    unsigned int uiFixedAeExposure;
#ifdef INFOTM_SUPPORT_DAUL_SENSOR_AE_PATCH
    bool useDualAE;
    double fBrightnessDiffThreshold,
    double fGainDiffThreshold,
    double fCurDaulBrightness[2];
    double fNewDaulGain[2];
    microseconds_t fNewDaulExposure[2];
#endif
    bool bDoAE;
    //static const double WEIGHT_7X7[ISP_HIS_REGION_VTILES][ISP_HIS_REGION_HTILES];
    //static const double WEIGHT_7X7_A[ISP_HIS_REGION_VTILES][ISP_HIS_REGION_HTILES];
    //static const double WEIGHT_7X7_B[ISP_HIS_REGION_VTILES][ISP_HIS_REGION_HTILES];
    //static const double BACKLIGHT_7X7[ISP_HIS_REGION_VTILES][ISP_HIS_REGION_HTILES];
    double adRegionBrightness[ISP_HIS_REGION_VTILES][ISP_HIS_REGION_HTILES];
    //double adRegionOverExp[ISP_HIS_REGION_VTILES][ISP_HIS_REGION_HTILES];
    //double adRegionUnderExp[ISP_HIS_REGION_VTILES][ISP_HIS_REGION_HTILES];
    //double adRegionRatio[ISP_HIS_REGION_VTILES][ISP_HIS_REGION_HTILES];
    double dOverUnderExpDiff;
    double dOverexposeRatio;
    double dMiddleOverexposeRatio;
    double dUnderexposeRatio;
    bool bAeTargetMoveEnable;
    int iAeTargetMoveMethod;
    double dAeTargetMax;
    double dAeTargetMin;
    double dAeTargetGainNormalModeMaxLmt;
    double dAeTargetMoveStep;
    double dAeTargetMinLmt;
    double dAeTargetUpOverThreshold;
    double dAeTargetUpUnderThreshold;
    double dAeTargetDnOverThreshold;
    double dAeTargetDnUnderThreshold;
    int iAeExposureMethod;
    double dAeTargetLowluxGainEnter;
    double dAeTargetLowluxGainExit;
    int iAeTargetLowluxExposureEnter;
    double dAeTargetLowluxFPS;
    double dAeTargetNormalFPS;
    bool bAeTargetMaxFpsLockEnable;
    int iAeBrightnessMeteringMethod;
    double dAeRegionDuce;
} ISP_CTRL_AE, *PISP_CTRL_AE;

//=================================================================================================
// AWB control
//=================================================================================================
#define SUM_GRID_MAX                (3)
#define SUM_GRID_ITEM_MAX           (16)

typedef enum _ISP_CTRL_AWB_ORDER {
    ISP_CTRL_AWB_ORDER_ENABLE = 0,
    ISP_CTRL_AWB_ORDER_DO_AWB,
    ISP_CTRL_AWB_ORDER_CORRECTION_TYPES,
    ISP_CTRL_AWB_ORDER_R_GAIN,
    ISP_CTRL_AWB_ORDER_B_GAIN,
    ISP_CTRL_AWB_ORDER_TEMPERATURE,
    ISP_CTRL_AWB_ORDER_SW_AWB_ENABLE,
    ISP_CTRL_AWB_ORDER_USE_ORIGINAL_CCM,
    ISP_CTRL_AWB_ORDER_IMG_R_GAIN,
    ISP_CTRL_AWB_ORDER_IMG_B_GAIN,
    ISP_CTRL_AWB_ORDER_SW_R_GAIN,
    ISP_CTRL_AWB_ORDER_SW_B_GAIN,
    ISP_CTRL_AWB_ORDER_SW_AWB_USE_ORI_CCM_NRML_GAIN_LMT,
    ISP_CTRL_AWB_ORDER_SW_AWB_USE_ORI_CCM_NRML_BRIGHTNESS_LMT,
    ISP_CTRL_AWB_ORDER_SW_AWB_USE_ORI_CCM_NRML_UNDEREXPOSE_LMT,
    ISP_CTRL_AWB_ORDER_SW_AWB_USE_STD_CCM_LOW_LUX_GAIN_LMT,
    ISP_CTRL_AWB_ORDER_SW_AWB_USE_STD_CCM_DARK_GAIN_LMT,
    ISP_CTRL_AWB_ORDER_SW_AWB_USE_STD_CCM_LOW_LUX_BRIGHTNESS_LMT,
    ISP_CTRL_AWB_ORDER_SW_AWB_USE_STD_CCM_LOW_LUX_UNDEREXPOSE_LMT,
    ISP_CTRL_AWB_ORDER_SW_AWB_USE_STD_CCM_DARK_BRIGHTNESS_LMT,
    ISP_CTRL_AWB_ORDER_SW_AWB_USE_STD_CCM_DARK_UNDEREXPOSE_LMT,
    ISP_CTRL_AWB_ORDER_HW_AWB_ENABLE,
    ISP_CTRL_AWB_ORDER_HW_AWB_METHOD,
    ISP_CTRL_AWB_ORDER_HW_AWB_FIRST_PIXEL,
    ISP_CTRL_AWB_ORDER_HW_AWB_STATISTIC,
    ISP_CTRL_AWB_ORDER_HW_SW_AWB_UPDATE_SPEED,
    ISP_CTRL_AWB_ORDER_HW_SW_AWB_DROP_RATIO,
    ISP_CTRL_AWB_ORDER_HW_SW_AWB_WEIGHTING_TABLE,
    ISP_CTRL_AWB_ORDER_R_GAIN_MIN,
    ISP_CTRL_AWB_ORDER_R_GAIN_MAX,
    ISP_CTRL_AWB_ORDER_B_GAIN_MIN,
    ISP_CTRL_AWB_ORDER_B_GAIN_MAX,
    ISP_CTRL_AWB_ORDER_FRAMES_TO_SKIP,
    ISP_CTRL_AWB_ORDER_FRAMES_SKIPPED,
    ISP_CTRL_AWB_ORDER_RESET_STATES,
    ISP_CTRL_AWB_ORDER_MARGIN,
    ISP_CTRL_AWB_ORDER_PID_KP,
    ISP_CTRL_AWB_ORDER_PID_KD,
    ISP_CTRL_AWB_ORDER_PID_KI,
    ISP_CTRL_AWB_ORDER_MAX
} ISP_CTRL_AWB_ORDER, *PISP_CTRL_AWB_ORDER;

typedef enum  _ISP_CORRECTION_TYPES
{
    ISP_CORRECTION_TYPES_WB_NONE = 0,
    ISP_CORRECTION_TYPES_WB_AC,  /**< @brief Average colour */
    ISP_CORRECTION_TYPES_WB_WP,  /**< @brief White Patch */
    ISP_CORRECTION_TYPES_WB_HLW,  /**< @brief High Luminance White */
    ISP_CORRECTION_TYPES_WB_COMBINED, /**< @brief Combined AC + WP + HLW */
    ISP_CORRECTION_TYPES_MAX
} ISP_CORRECTION_TYPES, *PISP_CORRECTION_TYPES;

typedef enum _ISP_HW_AWB_METHOD
{
    ISP_HW_AWB_METHOD_PIXEL_SUMMATION,
    ISP_HW_AWB_METHOD_WEIGHTED_GRW_GBW_W,
    ISP_HW_AWB_METHOD_WEIGHTED_RW_GW_BW,
    ISP_HW_AWB_METHOD_MAX
} ISP_HW_AWB_METHOD, *PISP_HW_AWB_METHOD;

typedef enum _ISP_HW_AWB_FIRST_PIXEL
{
    ISP_HW_AWB_FIRST_PIXEL_R,
    ISP_HW_AWB_FIRST_PIXEL_GR,
    ISP_HW_AWB_FIRST_PIXEL_GB,
    ISP_HW_AWB_FIRST_PIXEL_B,
    ISP_HW_AWB_FIRST_PIXEL_MAX
} ISP_HW_AWB_FIRST_PIXEL, *PISP_HW_AWB_FIRST_PIXEL;

typedef struct _ISP_HW_AWB_STAT
{
    double fGR;
    double fGB;
    unsigned int uiQualCnt;
} ISP_HW_AWB_STAT, *PISP_HW_AWB_STAT;

typedef struct _ISP_CTRL_AWB_PID {
    bool bEnable;
    bool bDoAwb;
    ISP_CORRECTION_TYPES uiCorrectionMode;
    double dRGain;
    double dBGain;
    double dTemperature;
    bool bSwAwbEnable;
    bool bUseOriginalCCM;
    double dImgRGain;
    double dImgBGain;
    double dSwRGain;
    double dSwBGain;
    double dSwAwbUseOriCcmNrmlGainLmt;
    double dSwAwbUseOriCcmNrmlBrightnessLmt;
    double dSwAwbUseOriCcmNrmlUnderExposeLmt;
    double dSwAwbUseStdCcmLowLuxGainLmt;
    double dSwAwbUseStdCcmDarkGainLmt;
    double dSwAwbUseStdCcmLowLuxBrightnessLmt;
    double dSwAwbUseStdCcmLowLuxUnderExposeLmt;
    double dSwAwbUseStdCcmDarkBrightnessLmt;
    double dSwAwbUseStdCcmDarkUnderExposeLmt;
    bool bHwAwbEnable;
    unsigned int uiHwAwbMethod;
    unsigned int uiHwAwbFirstPixel;
    ISP_HW_AWB_STAT stHwAwbStatistic[16][16];
    double dHwSwAwbUpdateSpeed;
    double dHwSwAwbDropRatio;
    unsigned int auiHwSwAwbWeightingTable[32][32];
    double dRGainMin;
    double dRGainMax;
    double dBGainMin;
    double dBGainMax;
    unsigned int uiFramesToSkip;
    unsigned int uiFramesSkipped;
    bool bResetStates;
    double dMargin;
    double dPidKP;
    double dPidKD;
    double dPidKI;
} ISP_CTRL_AWB_PID, *PISP_CTRL_AWB_PID;

//=================================================================================================
// CMC control
//=================================================================================================
#define ISP_CMC_GAIN_LEVEL_MAX                  (5)
#define ISP_CMC_INTERPOLATION_GMA_PNT_CNT       (256)
#define ISP_CMC_GAIN_LEVEL_USE                  (ISP_CMC_GAIN_LEVEL_MAX - 2)

typedef enum _ISP_CMC_COLOR_MODE {
    ISP_CMC_COLOR_MODE_ADVANCE = 0,
    ISP_CMC_COLOR_MODE_FLAT,
    ISP_CMC_COLOR_MODE_MAX
} ISP_CMC_COLOR_MODE, *PISP_CMC_COLOR_MODE;

typedef enum _ISP_CMC_DAY_MODE {
    ISP_CMC_DAY_MODE_NIGHT = 0,
    ISP_CMC_DAY_MODE_DAY,
    ISP_CMC_DAY_MODE_MAX
} ISP_CMC_DAY_MODE, *PISP_CMC_DAY_MODE;

typedef struct _ISP_CMC_NIGHT_MODE_DETECT_PARAM {
    double dCmcNightModeDetectBrightnessEnter;
    double dCmcNightModeDetectBrightnessExit;
    double dCmcNightModeDetectGainEnter;
    double dCmcNightModeDetectGainExit;
    double dCmcNightModeDetectWeighting;
} ISP_CMC_NIGHT_MODE_DETECT_PARAM, *PISP_CMC_NIGHT_MODE_DETECT_PARAM;

#define ISP_TNMC_WDR_SEGMENT_CNT                (8)
#define ISP_TNMC_WDR_MIN                        (0.0)
#define ISP_TNMC_WDR_MAX                        (5.0)

typedef struct _ISP_CMC_ITEM {
    unsigned int uiNoOfGainLevelUsed;                                   // Number of gain level be use for Advance/Flat mode.

    double dTargetBrightness;                                           // Target brightness of AE control.
    double dAeTargetMin;                                                // AE target minimum of AE control.
    double dSensorMaxGain;                                              // Sensor maximum gain of AE control.

    double dBrightness;                                                 // Brightness of R2Y module.
    double adRangeMult[3];                                              // Range Multiple of R2Y module.

    bool bEnableGamma;                                                  // Enable/Disable gamma type tone mapper curve of TNM control.

    unsigned int uiDnTargetIdx[ISP_CMC_GAIN_LEVEL_MAX];                 // 3D-Denoise target index.

    int iBlcSensorBlack[ISP_CMC_GAIN_LEVEL_MAX][4];                     // SENSOR black level of BLC module.

    double dDnsStrength[ISP_CMC_GAIN_LEVEL_MAX];                        // Primary denoise strength of DNS module.

    double dRadius[ISP_CMC_GAIN_LEVEL_MAX];                             // Radius of SHA module.
    double dStrength[ISP_CMC_GAIN_LEVEL_MAX];                           // Strength of SHA module.
    double dThreshold[ISP_CMC_GAIN_LEVEL_MAX];                          // Threshold of SHA module.
    double dDetail[ISP_CMC_GAIN_LEVEL_MAX];                             // Detail of SHA module.
    double dEdgeScale[ISP_CMC_GAIN_LEVEL_MAX];                          // Edge scale of SHA module.
    double dEdgeOffset[ISP_CMC_GAIN_LEVEL_MAX];                         // Edge offset of SHA module.
    bool bBypassDenoise[ISP_CMC_GAIN_LEVEL_MAX];                        // Bypass second denoise of SHA module.
    double dDenoiseTau[ISP_CMC_GAIN_LEVEL_MAX];                         // Edge avoid of second denoise of SHA module.
    double dDenoiseSigma[ISP_CMC_GAIN_LEVEL_MAX];                       // Strength of second denoise of SHA module.

    double dContrast[ISP_CMC_GAIN_LEVEL_MAX];                           // Contrast of R2Y module.
    double dSaturation[ISP_CMC_GAIN_LEVEL_MAX];                         // Saturation of R2Y module.

    int iGridStartCoords[ISP_CMC_GAIN_LEVEL_MAX][2];                    // position of the upper-left corner of the region in pixels of HIS module.
    int iGridTileDimensions[ISP_CMC_GAIN_LEVEL_MAX][2];                 // dimensions of the region in pixels of HIS module.

    double dDpfWeight[ISP_CMC_GAIN_LEVEL_MAX];                          // Weighting of DPF module.
    int iDpfThreshold[ISP_CMC_GAIN_LEVEL_MAX];                          // Threshold of DPF module.

    int iInY[ISP_CMC_GAIN_LEVEL_MAX][2];                                // Valid range of input luma values of TNM module.
    int iOutY[ISP_CMC_GAIN_LEVEL_MAX][2];                               // Valid range of output luma values of TNM module.
    int iOutC[ISP_CMC_GAIN_LEVEL_MAX][2];                               // Valid range of output chroma values of TNM module.
    double dFlatFactor[ISP_CMC_GAIN_LEVEL_MAX];                         // Flat factor of TNM module.
    double dWeightLine[ISP_CMC_GAIN_LEVEL_MAX];                         // Weight line of TNM module.

    double dColourConfidence[ISP_CMC_GAIN_LEVEL_MAX];                   // Colour confidence of TNM control.
    double dColourSaturation[ISP_CMC_GAIN_LEVEL_MAX];                   // Colour saturation of TNM control.

    double dEqualBrightSuppressRatio[ISP_CMC_GAIN_LEVEL_MAX];           // Bright suppress ration of equalization type tone mapper curve of TNMC control.
    double dEqualDarkSuppressRatio[ISP_CMC_GAIN_LEVEL_MAX];             // Dark suppress ration of equalization type tone mapper curve of TNMC control.
    double dOvershotThreshold[ISP_CMC_GAIN_LEVEL_MAX];                  // Overshot threshold of equalization type tone mapper curve of TNMC control.
    double dWdrCeiling[ISP_CMC_GAIN_LEVEL_MAX][ISP_TNMC_WDR_SEGMENT_CNT];   // WDR ceiling of equalization type tone mapper curve of TNMC control.
    double dWdrFloor[ISP_CMC_GAIN_LEVEL_MAX][ISP_TNMC_WDR_SEGMENT_CNT]; // WDR floor of equalization type tone mapper curve of TNMC control.

    int iGammaCrvMode[ISP_CMC_GAIN_LEVEL_MAX];                          // Gamma curve mode of gamma type tone mapper curve of TNM control.
    double dTnmGamma[ISP_CMC_GAIN_LEVEL_MAX];                           // Gamma value of gamma type tone mapper curve of TNM control.
    double dBezierCtrlPnt[ISP_CMC_GAIN_LEVEL_MAX];                      // Bezier control point Value of gamma type tone mapper curve of TNM control.
} ISP_CMC_ITEM, *PISP_CMC_ITEM;

typedef struct _ISP_CTRL_CMC {
    //bool bEnable;                                                   // Enable/Disable whole of ControlCMC.
    bool bCmcEnable;                                                // Enable/Disable CMC control function.
    bool bDnTargetIdxChageEnable;                                   // Enable/Disable 3D-Denoise target index change.

    bool bSensorGainMedianFilterEnable;                             // Enable/Disable Sensor Gain median filter.

    bool bSensorGainLevelCtrl;                                      // Enable/Disable Sensor Gain level feature.

    double dSensorGainLevel[ISP_CMC_GAIN_LEVEL_MAX];                // Sensor Gain level.
    double dSensorGainLevelInterpolation[ISP_CMC_GAIN_LEVEL_MAX];   // Sensor Gain level interpolation.
    double dCcmAttenuation[ISP_CMC_GAIN_LEVEL_MAX];                 // CCM attenuation level.

    bool bInterpolationGammaStyleEnable;                            // Enable/Disable using gamma method to do the gain level interpolation.
    double dInterpolationGamma[ISP_CMC_GAIN_LEVEL_MAX];             // Gamma value for the gain level interpolation.
#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)
    float afGammaLUT[ISP_CMC_GAIN_LEVEL_MAX][ISP_CMC_INTERPOLATION_GMA_PNT_CNT]; // Gamma curve for the gain level interpolation.
#endif //#if defined(CMC_INTERPOLATION_GAMMA_LUT_ENABLE)

    double dR2YSaturationExpect;                                    // For user mode saturation adjustment.

    double dShaStrengthExpect;                                      // For user mode sharpness adjustment.
    //bool bShaStrengthChange;
    double dShaStrengthOffset[ISP_CMC_COLOR_MODE_MAX][ISP_CMC_GAIN_LEVEL_MAX];

    // For night mode detection.
    bool bCmcNightModeDetectEnable;
    double dCmcNightModeDetectBrightnessEnter;
    double dCmcNightModeDetectBrightnessExit;
    double dCmcNightModeDetectGainEnter;
    double dCmcNightModeDetectGainExit;
    double dCmcNightModeDetectWeighting;

    bool bCaptureModeIqParamChangeEnable;
    double dCaptureModeCCMRatio;
    double adCaptureModeR2YRangeMult[3];
    int aiCaptureModeTnmInY[2];
    int aiCaptureModeTnmOutY[2];
    int aiCaptureModeTnmOutC[2];

    ISP_CMC_ITEM stCMC[ISP_CMC_COLOR_MODE_MAX];
} ISP_CTRL_CMC, *PISP_CTRL_CMC;

//=================================================================================================
// TNMC control
//=================================================================================================
#define ISP_TNMC_N_CURVE                        (65)
#define ISP_TNMC_N_HIST                         (64)
#define ISP_TNMC_N_CURVE_MAX                    (ISP_TNMC_N_CURVE + 1)
#define ISP_TNMC_CURVE_MAX                      (1)

typedef enum _ISP_TNMC_GMA_CRV_MODE {
    ISP_TNMC_GMA_CRV_MODE_GAMMA = 0,
    ISP_TNMC_GMA_CRV_MODE_BEZIER_LINEAR_QUADRATIC,
    ISP_TNMC_GMA_CRV_MODE_BEZIER_QUADRATIC,
    ISP_TNMC_GMA_CRV_MODE_MAX
} ISP_TNMC_GMA_CRV_MODE, *PISP_TNMC_GMA_CRV_MODE;

typedef struct _ISP_CTRL_TNM {
    bool bEnable;
    double dAdaptiveStrength;
    double dHistMin;
    double dHistMax;
    double dSmoothing;
    double dTempering;
    double dUpdateSpeed;
    double dHistogram[ISP_TNMC_N_HIST];
    double dMappingCurve[ISP_TNMC_N_CURVE];
    bool bLocalTNM;
    bool bAdaptiveTNM;
    bool bAdaptiveTNM_FCM;
    double dLocalStrength;
    bool bConfigureHis;
    int iSmoothHistogramMethod;
    bool bEnableEqualization;
    bool bEnableGamma;
    double dEqualMaxBrightSupress;
    double dEqualMaxDarkSupress;
    double dEqualBrightSupressRatio;
    double dEqualDarkSupressRatio;
    double dEqualFactor;
    double dOvershotThreshold;
    double dWdrCeiling[ISP_TNMC_WDR_SEGMENT_CNT];
    double dWdrFloor[ISP_TNMC_WDR_SEGMENT_CNT];
    double dMapCurveUpdateDamp;
    double dMapCurvePowerValue;
    int iGammaCrvMode;
    double dGammaACM;
    double dGammaFCM;
    double dBezierCtrlPnt;
    bool bTnmCrvSimBrightnessContrastEnable;
    double dTnmCrvBrightness;
    double dTnmCrvContrast;
} ISP_CTRL_TNM, *PISP_CTRL_TNM;

//=================================================================================================
// IIC module
//=================================================================================================
typedef struct _IIC_MDLE_IIC {
    int iDebug;
    int iBus;
    int iAddrNumOfBytes;
    int iDataNumOfBytes;
    int iSalveAddr;
    int iRegAddr;
    int iRegValue;
} IIC_MDLE_IIC, *PIIC_MDLE_IIC;

//=================================================================================================
// OUT module
//=================================================================================================
typedef enum _ISP_MDLE_OUT_ORDER {
    ISP_MDLE_OUT_ORDER_EXTRACT_PNT = 0,
    ISP_MDLE_OUT_ORDER_MAX
} ISP_MDLE_OUT_ORDER, *PISP_MDLE_OUT_ORDER;

typedef enum _ISP_CI_INOUT_POINTS {
    ISP_CI_INOUT_BLACK_LEVEL = 0,
    ISP_CI_INOUT_FILTER_LINESTORE,
    //ISP_CI_INOUT_DEMOISAICER,
    //ISP_CI_INOUT_GAMA_CORRECTION,
    //ISP_CI_INOUT_HSBC,
    //ISP_CI_INOUT_TONE_MAPPER_LINESTORE,
    ISP_CI_INOUT_NONE   /** @brief Not a valid point - only counts number of points */
} ISP_CI_INOUT_POINTS, *PISP_CI_INOUT_POINTS;

typedef struct _ISP_MDLE_OUT {
    int iDataExtractionPoint;
} ISP_MDLE_OUT, *PISP_MDLE_OUT;

//=================================================================================================
// ISP Version.
//=================================================================================================
typedef enum _ISP_VERSION {
    ISP_VERSION_V2500 = 0,
    ISP_VERSION_V2505,
    ISP_VERSION_CORONA,
    ISP_VERSION_MAX
} ISP_VERSION, *PISP_VERSION;

typedef struct _ISP_MDLE_VER {
    int iVersion;
} ISP_MDLE_VER, *PISP_MDLE_VER;

//=================================================================================================
// ISP Debug Information Control.
//=================================================================================================
typedef struct _ISP_MDLE_DBG {
    bool bDebugEnable;
} ISP_MDLE_DBG, *PISP_MDLE_DBG;

//=================================================================================================
// ISPOST Denoise module
//=================================================================================================
#define ISPOST_DN_LEVEL_MAX                     (15)
#define ISPOST_DN_LEVEL_USE                     (ISPOST_DN_LEVEL_MAX-3)

typedef struct _ISPOST_DN_PARAM {
    int iThresholdY;
    int iThresholdU;
    int iThresholdV;
    int iWeight;
} ISPOST_DN_PARAM, *PISPOST_DN_PARAM;

typedef struct _ISPOST_MDLE_DN {
    char szIspostInstance[128];     // ispost = "ispost0" for Q3, "ispost" for Q3F
    bool bEnable;
    int iDnUsedCnt;
    int iDnManualMode;              // 1: Manual, 0: Auto made;
    ISPOST_DN_PARAM stManualDnParam;
    ISPOST_DN_PARAM stDnParam[ISPOST_DN_LEVEL_MAX];
} ISPOST_MDLE_DN, *PISPOST_MDLE_DN;

//=================================================================================================
// ISPOST Version.
//=================================================================================================
typedef enum _ISPOST_VERSION {
    ISPOST_VERSION_V1 = 0,
    ISPOST_VERSION_V2,
    ISPOST_VERSION_MAX
} ISPOST_VERSION, *PISPOST_VERSION;

typedef struct _ISPOST_MDLE_VER {
    char szIspostInstance[128];     // ispost = "ispost0" for Q3, "ispost" for Q3F
    int iVersion;
} ISPOST_MDLE_VER, *PISPOST_MDLE_VER;

//=================================================================================================
// ISPOST Debug Information Control.
//=================================================================================================
typedef struct _ISPOST_MDLE_DBG {
    char szIspostInstance[128];     // ispost = "ispost0" for Q3, "ispost" for Q3F
    bool bDebugEnable;
} ISPOST_MDLE_DBG, *PISPOST_MDLE_DBG;

//=================================================================================================
// ISPOST Grid Data.
//=================================================================================================
typedef struct _ISPOST_MDLE_GRID_FILE_PATH {
    char szIspostInstance[128];     // ispost = "ispost0" for Q3, "ispost" for Q3F
    char szGridFilePath[2][256];
} ISPOST_MDLE_GRID_FILE_PATH, *PISPOST_MDLE_GRID_FILE_PATH;

//=================================================================================================
// H264/H265 Encoder module
//=================================================================================================
typedef enum _ENC_V_ENC_PROFILE {
    ENC_V_ENC_PROFILE_BASELINE,
    ENC_V_ENC_PROFILE_MAIN,
    ENC_V_ENC_PROFILE_HIGH,
} ENC_V_ENC_PROFILE, *PENC_V_ENC_PROFILE;

typedef enum _ENC_V_STREAM_TYPE {
    ENC_V_STREAM_TYPE_BYTESTREAM,
    ENC_V_STREAM_TYPE_NALSTREAM,
} ENC_V_STREAM_TYPE, *PENC_V_STREAM_TYPE;

typedef enum _ENC_V_MEDIA_TYPE {
    ENC_V_MEDIA_TYPE_NONE,
    ENC_V_MEDIA_TYPE_MJPEG,
    ENC_V_MEDIA_TYPE_H264 ,
    ENC_V_MEDIA_TYPE_HEVC
} ENC_V_MEDIA_TYPE, *PENC_V_MEDIA_TYPE;

typedef enum _ENC_V_RATE_CTRL_MODE {
    ENC_V_RATE_CTRL_MODE_CBR_MODE = 0,
    ENC_V_RATE_CTRL_MODE_VBR_MODE,
    ENC_V_RATE_CTRL_MODE_FIXQP_MODE,
} ENC_V_RATE_CTRL_MODE, *PENC_V_RATE_CTRL_MODE;

typedef enum _ENC_V_ENC_FRAME_TYPE {
    ENC_V_ENC_FRAME_TYPE_HEADER_NO_IN_FRAME_MODE = 0,
    ENC_V_ENC_FRAME_TYPE_HEADER_IN_FRAME_MODE,
} ENC_V_ENC_FRAME_TYPE, *PENC_V_ENC_FRAME_TYPE;

typedef enum _ENC_V_NVR_DISPLAY_MODE {
    ENC_V_NVR_DISPLAY_MODE_QUARTER_SCREEN_MODE = 0,
    ENC_V_NVR_DISPLAY_MODE_FULL_SCREEN_MODE,
    ENC_V_NVR_DISPLAY_MODE_ONE_THREE_MODE,
    ENC_V_NVR_DISPLAY_MODE_INVALID_MODE = 0xFF,
} ENC_V_NVR_DISPLAY_MODE, *PENC_V_NVR_DISPLAY_MODE;

typedef struct _ENC_V_BASIC_INFO {
    ENC_V_MEDIA_TYPE stMediaType ;
    ENC_V_ENC_PROFILE stProfile;
    ENC_V_STREAM_TYPE stStreamType;
} ENC_V_BASIC_INFO, *PENC_V_BASIC_INFO;

typedef struct _ENC_V_VBR_INFO {
    int iQpMax;
    int iQpMin;
    int iMaxBitRate;
    int iThreshold;
    int iQpDelta;
} ENC_V_VBR_INFO, *PENC_V_VBR_INFO;

typedef struct _ENC_V_CBR_INFO {
    int iQpMax;
    int iQpMin;
    int iBitRate;
    int iQpDelta; //will affect i frame size
    int iQpHdr;   //when rc_mode is 0,affect first I frame qp,when rc_mode is 1,affect whole frame qp
} ENC_V_CBR_INFO, *PENC_V_CBR_INFO;

typedef struct _ENC_V_FIXQP_INFO {
    int iQpFix;
} ENC_V_FIXQP_INFO, *PENC_V_FIXQP_INFO;

typedef struct _ENC_V_RATE_CTRL_INFO {
    char szName[32];
    ENC_V_RATE_CTRL_MODE eRateCtrlMode; //rc_mode = 1(VBR Mode) rc_mode = 0(CBR Mode)
    int iGopLength;
    int iPictureSkip;
    int iIdrInterval;
    int iHrd;
    int iHrdCpbSize;
    int iRefreshInterval; //longterm case p_refresh internaval must  < idr_interval
    int iMacroBlockRateCtrl;
    int iMacroBlockQpAdjustment;
    union {
        ENC_V_FIXQP_INFO stFixQp;
        ENC_V_VBR_INFO stVBR;
        ENC_V_CBR_INFO stCBR;
    };
    unsigned long long ullBps;
} ENC_V_RATE_CTRL_INFO, *PENC_V_RATE_CTRL_INFO;

typedef struct _H26X_MDLE_ENC {
    char szEncoderChannel[128];         // EncoderChannel = "encvga-stream", "enc720p-stream", "enc1080p-stream", "h1264-stream"
    ENC_V_RATE_CTRL_INFO stVideoRateCtrl;
} H26X_MDLE_ENC, *PH26X_MDLE_ENC;

//=================================================================================================
// H264/H265 Encoder Version.
//=================================================================================================
//typedef enum _H26X_VER {
//    H26X_VER_V1 = 0,
//    H26X_VER_V2,
//    H26X_VER_MAX
//} ISPOST_VER, *PISPOST_VER;

typedef struct _H26X_MDLE_VER {
    char szEncoderChannel[128];         // EncoderChannel = "encvga-stream", "enc720p-stream", "enc1080p-stream", "h1264-stream"
    int iVersion;
} H26X_MDLE_VER, *PH26X_MDLE_VER;

//=================================================================================================
// H264/H265 Encoder Debug Information Control.
//=================================================================================================
typedef struct _H26X_MDLE_DBG {
    char szEncoderChannel[128];         // EncoderChannel = "encvga-stream", "enc720p-stream", "enc1080p-stream", "h1264-stream"
    bool bDebugEnable;
} H26X_MDLE_DBG, *PH26X_MDLE_DBG;

//=================================================================================================
// Videobox Control
//=================================================================================================
typedef enum _VIDEOBOX_CTRL_MODE {
    VIDEOBOX_CTRL_MODE_START,
    VIDEOBOX_CTRL_MODE_STOP,
    VIDEOBOX_CTRL_MODE_REBIND,
    VIDEOBOX_CTRL_MODE_REPATH,
    VIDEOBOX_CTRL_MODE_MAX,
} VIDEOBOX_CTRL_MODE, *PVIDEOBOX_CTRL_MODE;

typedef struct _VIDEOBOX_CTRL {
    int iMode;
    char szJsonFilePath[512];
    char szPort[128];
    char szTargetPort[128];
} VIDEOBOX_CTRL, *PVIDEOBOX_CTRL;

//=================================================================================================
// Other Control
//=================================================================================================
typedef enum _RE_MOUNT_METHOD {
    RE_MOUNT_METHOD_EXECUTE_SCRIPT,
    RE_MOUNT_METHOD_DEV_PARTITION_NAME,
    RE_MOUNT_METHOD_MAX,
} RE_MOUNT_METHOD, *PRE_MOUNT_METHOD;

typedef struct _RE_MOUNT_DEV_CTRL {
    int iMethod;
    char szScriptDevPartitionName[512];
} RE_MOUNT_DEV_CTRL, *PRE_MOUNT_DEV_CTRL;


#endif //__TCPCOMMAND_H__
