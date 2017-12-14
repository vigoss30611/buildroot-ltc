// Copyright (C) 2016 InfoTM, sam.zhou@infotm.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef IPU_ISPLUS_H
#define IPU_ISPLUS_H
#include <IPU.h>
#include <list>
#include <ispc/Pipeline.h>
#include <ispc/ParameterList.h>
#include <ispc/ParameterFileParser.h>
#include <ispc/ParameterFileParser.h>
#include <ispc/Camera.h>
#include <ispc/Sensor.h>
#include <ispc/ControlAE.h>
#include <ispc/ControlAWB.h>
#include <ispc/ControlAWB_PID.h>
#include <ispc/ControlAWB_Planckian.h>
#include <ispc/ModuleOUT.h>
#include <ispc/ModuleCCM.h>
#include <ispc/ControlDNS.h>
#include <ispc/ControlTNM.h>
#include <ispc/ControlLBC.h>
#include <ispc/ControlCMC.h>
#include <ispc/ModuleR2Y.h>
#include <ispc/ModuleSHA.h>
#include <ispc/ModuleIIF.h>
#include <ispc/ModuleESC.h>
#include <ispc/ModuleWBC.h>
#include <ispc/ModuleTNM.h>
#include <ispc/ModuleLSH.h>
#include <ispc/ModuleGMA.h>
#include <ispc/ControlLSH.h>
#include <ispc/Save.h>
#include <sensorapi/sensorapi.h>
#include <ci/ci_api.h>
#include <ispc/ModuleDPF.h>
#include <ispc/ModuleDNS.h>
#include <semaphore.h>

#include "ddk_sensor_driver.h"

#include <vector>
#include <list>
#include <IPU.h>
#include <hlibispostv2/hlib_ispost.h>
#ifdef MACRO_IPU_ISPLUS_FCE
#include "libgridtools.h"
#endif

#define IPU_ISPOST_GRIDDATA_MAX     4
#define IPU_ISPOST_DN_LEVEL_MAX    12

//#define IPU_ISPOST_LC_DEBUG
//#define IPU_ISPOST_OVERLAY_TEST

#define ISPOST_PT_IN           0x0001
#define ISPOST_PT_LC           0x0002
#define ISPOST_PT_VS           0x0004
#define ISPOST_PT_OV0          0x0008
#define ISPOST_PT_OV1          0x0010
#define ISPOST_PT_OV2          0x0020
#define ISPOST_PT_OV3          0x0040
#define ISPOST_PT_OV4          0x0080
#define ISPOST_PT_OV5          0x0100
#define ISPOST_PT_OV6          0x0200
#define ISPOST_PT_OV7          0x0400
#define ISPOST_PT_DN           0x0800
#define ISPOST_PT_UO           0x1000
#define ISPOST_PT_SS0          0x2000
#define ISPOST_PT_SS1          0x4000

#define ISPOST_PT_OV1_7 (ISPOST_PT_OV1|ISPOST_PT_OV2|ISPOST_PT_OV3|ISPOST_PT_OV4|ISPOST_PT_OV5|ISPOST_PT_OV6|ISPOST_PT_OV7)
#define ISPOST_PT_OV0_7 (ISPOST_PT_OV0|ISPOST_PT_OV1|ISPOST_PT_OV2|ISPOST_PT_OV3|ISPOST_PT_OV4|ISPOST_PT_OV5|ISPOST_PT_OV6|ISPOST_PT_OV7)


#define GRIDDATA_ALIGN_SIZE  64
#define LOADFCEDATA_MAX     1
#define CREATEFCEDATA_MAX   2

//HSBC : Hue, Saturation, Brightness and Contrast
#define IPU_HSBC_MAX      255
#define IPU_HSBC_MIN      0
#define NOTIFY_TIMEOUT_SEC      8


class IPU_ISPLUS: public IPU {

public:

    enum ipu_v2500_status {
        V2500_NONE,
        CAM_CREATE_SUCCESS,
        SENSOR_CREATE_SUCESS,
    };

    typedef enum _ipu_cam_run_status {
        CAM_RUN_ST_IDLE,
        CAM_RUN_ST_CAPTURING,
        CAM_RUN_ST_PREVIEWING,
        CAM_RUN_ST_CAPTURED,
        CAM_RUN_ST_MAX
    }E_IPU_CAM_RUN_STATUS;

    struct ipu_v2500_dn_indicator {
        double gain;
        int dn_id;
    };

    typedef struct _overlay_info {
        char *pAddr; // YUV buffer address
        unsigned int iPaletteTable[4];
        int width;
        int height;
        int disable;
    }ST_OVERLAY_INFO;

    struct ipu_ispost_3d_dn_param {
        int y_threshold;
        int u_threshold;
        int v_threshold;
        int weight;
    };

    typedef struct
    {
        struct fr_buf_info stBufGrid;
        struct fr_buf_info stBufGeo;
    }ST_GRIDDATA_BUF_INFO;

    typedef struct {
        int modeID;
        std::vector<ST_GRIDDATA_BUF_INFO> stBufList;
        std::vector<cam_position_t> stPosDnList;
        std::vector<cam_position_t> stPosUoList;
        std::vector<cam_position_t> stPosSs0List;
        std::vector<cam_position_t> stPosSs1List;
    }ST_FCE_MODE;

    typedef struct {
        int groupID;
        std::list<int> modePendingList;
        std::list<int> modeActiveList;
        std::vector<ST_FCE_MODE> stModeAllList;
    }ST_FCE_MODE_GROUP;

    typedef struct {
        bool hasFce;
        int maxModeGroupNb;
        std::list<int> modeGroupPendingList;
        std::list<int> modeGroupActiveList;
        std::list<int> modeGroupIdleList;
        std::vector<ST_FCE_MODE_GROUP> stModeGroupAllList;
    }ST_FCE;


    ISPC::Camera *pCamera;
    std::list < IMG_UINT32 > buffersIds;
    ISPC::Shot    frame;
    IMG_UINT32 nBuffers;
    ISPC::Shot *m_pstShotFrame;
    int m_iBufferID;
    int m_iBufUseCnt;
    struct ipu_v2500_private_data *m_pstPrvData;
    std::function<int (Buffer*)> m_pFun;
    std::list <void *> m_postShots;
    volatile int runSt;
    enum ipu_v2500_status currentStatus;
//    ST_LINK_INFO m_stLinkInfo;


    IPU_ISPLUS(std::string, Json *);
    virtual ~IPU_ISPLUS();
    void Prepare();
    void Unprepare();
    void WorkLoop();
    int UnitControl(std::string, void *);

    void Prepare1();
    void Unprepare1();

    // FIXME: CHANGE ALL RETURN VALUE TO int and also the parameters
    int GetYUVBrightness(int &out_brightness);
    int SetYUVBrightness(int brightness);
    int GetEnvBrightnessValue(int &out_bv);
    int GetContrast(int &out_contrast);
    int GetSnapResolution(reslist_t *resl);
    int SetSnapResolution(const res_t res);
    int SnapOneShot();
    int SnapExit();
    int SetContrast(int contrast);
    int GetSaturation(int &out_saturation);
    int SetSaturation(int saturation);
    int GetSharpness(int &out_sharpness);
    int SetSharpness(int sharpness);
    int GetHue(int &out_hue);
    int SetHue(int hue);
    int SetAntiFlicker(int freq);
    int GetAntiFlicker(int &freq);
    int SetExposureLevel(int level);
    int GetExposureLevel(int &level);
    int SetSensorISO(int iso);
    int GetSensorISO(int &iso);
    int EnableAE(int enable);
    int EnableLBC(int enable);
    int EnableAWB(int enable);
    int EnableMonochrome(int enable);
    int EnableWDR(int enable);
    int GetMirror(enum cam_mirror_state &dir);
    int SetMirror(enum cam_mirror_state dir);
    int SetResolution(int imgW, int imgH);
    int GetResolution(int &imgW, int &imgH);
    int SetFPS(int fps);
    int GetFPS(int &fps);  // FIXME: int
    int IsWDREnabled();
    int IsAEEnabled();
    int IsAWBEnabled();
    int IsMonchromeEnabled(); // FIXME: wrong spelling
    int SetWB(int mode);
    int GetWB(int &mode);
    //sn =1 low light scenario, else hight light scenario
    int SetScenario(int sn);
    int GetScenario(enum cam_scenario &scen);
    int SetSepiaMode(int mode);
    int SetSensorResolution(const res_t res);
    int ReturnShot(Buffer *pstBuf);
    int EnqueueReturnShot(void);
    int ClearAllShot();

    int SetSensorExposure(int usec);
    int GetSensorExposure(int &usec);

    int SetDpfAttr(const cam_dpf_attr_t *attr);
    int GetDpfAttr(cam_dpf_attr_t *attr);

    int SetDnsAttr(const cam_dns_attr_t *attr);
    int GetDnsAttr(cam_dns_attr_t *attr);

    int SetShaDenoiseAttr(const cam_sha_dns_attr_t *attr);
    int GetShaDenoiseAttr(cam_sha_dns_attr_t *attr);

    int SetWbAttr(const cam_wb_attr_t *attr);
    int GetWbAttr(cam_wb_attr_t *attr);

    int Set3dDnsAttr(const cam_3d_dns_attr_t *attr);
    int Get3dDnsAttr(cam_3d_dns_attr_t *attr);

    int SetYuvGammaAttr(const cam_yuv_gamma_attr_t *attr);
    int GetYuvGammaAttr(cam_yuv_gamma_attr_t *attr);

    int SetFpsRange(const cam_fps_range_t *fps_range);
    int GetFpsRange(cam_fps_range_t *fps_range);

    int GetDayMode(enum cam_day_mode &out_day_mode);
    int SaveData(cam_save_data_t *saveData);
    int GetPhyStatus(IMG_UINT32 *status);

    bool InitIspost(std::string name, Json* js);
    void DeInitIspost();
    void Prepare2();
    void Unprepare2();
    bool WorkLoopInit();
    bool WorkLoopGetBuffer();
    bool WorkLoopTrigger();
    bool WorkLoopCompleteCheck();
    bool WorkLoopPutBuffer();
    void IspostCheckParameter();
    void IspostDefaultParameter(void);
    bool IspostInitial(void);
//    bool IspostTrigger(void);
    bool IspostUpdateBuf();
    bool IspostHwInit(void);
    bool IspostHwUpdate(void);
    bool IspostHwTrigger(void);
    bool IspostHwCompleteCheck(void);
    int IspostSetLcGridTargetIndex(int Index);
    int IspostGetLcGridTargetIndex();
    int IspostGetLcGridTargetCount();
    void SetOvPaletteTable(ST_OVERLAY_INFO* ov_info, ISPOST_UINT32 flag);
    int IspostSetPipInfo(struct v_pip_info *vpinfo);
    int IspostGetPipInfo(struct v_pip_info *vpinfo);
    int IspostFormatPipInfo(struct v_pip_info *vpinfo);
    void IspostAdjustOvPos(int ov_idx);
private:
    char psname[64];
    char pSetupFile[128];
    int ImgH;
    int ImgW;
    int pipX;
    int pipY;
    int pipW;
    int pipH;
    int lockRatio;
    int in_w;
    int in_h;
    int m_MinFps;
    int m_MaxFps;
    int context;
    int exslevel;
    int scenario;
    int wbMode;
    reslist_t resl;
    ISPC::ControlAE *pAE;
    ISPC::ControlAWB_PID *pAWB;
    ISPC::ControlTNM *pTNM;
    ISPC::ControlDNS *pDNS;
    ISPC::ControlLBC *pLBC;
    ISPC::ControlCMC *pCMC;
    ISPC::ControlLSH *pLSH;
    double flDefTargetBrightness;
    const float EPSINON = 0.00001;
    cam_3d_dns_attr_t dns_3d_attr;
    cam_fps_range_t sFpsRange;
    int ui32SkipFrame;
    cam_save_data_t *m_psSaveDataParam;
    sem_t m_semNotify;
    pthread_mutex_t mutex_x;


    Json* m_pJsonMain;
    ISPOST_USER m_IspostUser;
    char* m_pszLcGridFileName[IPU_ISPOST_GRIDDATA_MAX];
    char* m_pszLcGeometryFileName[IPU_ISPOST_GRIDDATA_MAX];
    char* m_pszOvImgFileName[OVERLAY_CHNL_MAX];
    FRBuffer* m_pLcGridBuf[IPU_ISPOST_GRIDDATA_MAX];
    FRBuffer* m_pLcGeometryBuf[IPU_ISPOST_GRIDDATA_MAX];
    FRBuffer* m_pOvImgBuf[OVERLAY_CHNL_MAX];
    Port *m_pPortIn, *m_pPortOv[OVERLAY_CHNL_MAX];
    Port *m_pPortDn, *m_pPortUo, *m_pPortSs0, *m_pPortSs1;
    Buffer m_BufIn, m_BufOv[OVERLAY_CHNL_MAX];
    Buffer m_BufDn, m_BufUo, m_BufSs0, m_BufSs1;
    int m_Buffers, m_CropX, m_CropY, m_CropW, m_CropH, m_Rotate, /*lc_grid_count,*/grid_target_index_max;
    bool m_IsFisheye;
    bool m_IsLineBufEn;
    ISPOST_UINT32 m_PortEnable, m_PortGetBuf;
    ISPOST_UINT32 m_LastPortEnable;
    bool m_IsDnManual, m_IsLcManual, m_IsOvManual;
    int m_DnManualVal, m_LcManualVal, m_OvManualVal;
    cam_3d_dns_attr_t m_Dns3dAttr;
    pthread_mutex_t m_PipMutex;
    ST_FCE m_stFce;
    pthread_mutex_t m_FceLock;
    bool m_HasFceData;
    bool m_HasFceFile;
    static const struct ipu_ispost_3d_dn_param g_dnParamList[];


    int SetGAMCurve(cam_gamcurve_t *GamCurve);
    int SetEncScaler(int imgW = 1920, int imgH = 1088);
    int SetIIFCrop(int pipH, int pipW, int pipX, int pipY);
    void BuildPort(int imgW= 1920, int imgH = 1088);
    int AutoControl();
    int ConfigAutoControl();
    int CheckHSBCInputParam(int param);
    int CalcHSBCToISP(double max, double min, int param, double &out_val);
    int CalcHSBCToOut(double max, double min, double param, int &out_val);

    int CheckOutRes(int imgW, int imgH);
    int CheckAEDisabled();
    int CheckSensorClass();
    int SkipFrame(int skipNb);
    int SaveFrame(ISPC::Shot &shotFrame);
    int SaveFrameFile(const char *filePath, cam_data_format_e data_fmt, ISPC::Shot &shotFrame);
    int SaveBayerRaw(const char *fileName, const ISPC::Buffer &bayerBuffer);

    bool IspostJsonGetInt(const char* pszItem, int& Value);
    bool IspostJsonGetString(const char* pszItem, char*& pszString);
    ISPOST_UINT32 IspostGetFileSize(FILE* pFileID);
    bool IspostLoadBinaryFile(const char* pszBufName, FRBuffer** ppBuffer, const char* pszFilenane);
    ISPOST_UINT32 IspostParseGirdSize(const char* pszFileName);
    ISPOST_UINT32 IspostGetGridTargetIndexMax(int width, int height, ISPOST_UINT8 sz, ISPOST_UINT32 nload);
    void IspostCalcPlanarAddress(int w, int h, ISPOST_UINT32& Yaddr, ISPOST_UINT32& UVaddr);
    void IspostCalcOffsetAddress(int x, int y, int Stride, ISPOST_UINT32& Yaddr, ISPOST_UINT32& UVaddr);
    void IspostDumpUserInfo(void);
    int IspostSet3dDnsCurve(const cam_3d_dns_attr_t *attr, int is_force);
    int IspostProcPrivateData(ipu_v2500_private_data_t *pdata);
    int IspostCreateAndFireFceData(cam_fcedata_param_t  *pstParam);
    int IspostLoadAndFireFceData(cam_fcefile_param_t *pstParam);
    int UpdateDnUserInfo(bool hasFce, cam_position_t *pstPos, ISPOST_UINT32 *pRefBufYAddr);
    int UpdateUoUserInfo(bool hasFce, cam_position_t *pstPos);
    int UpdateSs0UserInfo(bool hasFce, cam_position_t *pstPos);
    int UpdateSs1UserInfo(bool hasFce, cam_position_t *pstPos);
    int IspostSetFceMode(int mode);
    int IspostGetFceMode(int *pMode);
    int IspostGetFceTotalModes(int *pModeCount);
    int IspostSaveFceData(cam_save_fcedata_param_t *pstParam);
    int UpdateFceModeGroup(ST_FCE *pstFce, ST_FCE_MODE_GROUP *pstGroup);
    int GetFceActiveMode(ST_FCE_MODE *pstMode);
    int UpdateFcePendingStatus();
    int ClearFceModeGroup(ST_FCE_MODE_GROUP *pstModeGroup);
    int ParseFceFileParam(ST_FCE_MODE_GROUP *pstModeGroup, int groupID, cam_fcefile_param_t *pstParam);
    void FreeFce();
    void PrintAllFce(ST_FCE *pstFce, const char *pszFileName, int line);
    int SetFceDefaultPos(Port *pPort, int fileNumber, std::vector<cam_position_t> *pPosList);
#ifdef MACRO_IPU_ISPLUS_FCE
    void ParseFceDataParam(cam_fisheye_correction_t *pstFrom, TFceGenGridDataParam *pstTo);
    int CalcGridDataSize(TFceCalcGDSizeParam *pstDataSize);
    int GenGridData(TFceGenGridDataParam *stGridParam);
#endif
};

    #define CheckParam(param) \
    ({ int ret = 0; \
        \
        if (NULL == param) \
        { \
            LOGE("(%s,L%d) Error: %s is NULL !!!\n", __func__, __LINE__, #param); \
            ret = -1; \
        } \
        ret; \
    })


    #define IspostCheckParam(param) \
    ({ int ret = 0; \
        \
        if (NULL == param) \
        { \
            LOGD("(%s,L%d) Error: %s is NULL !!!\n", __func__, __LINE__, #param); \
            ret = -1; \
        } \
        ret; \
    })

#endif /* IPU_V2500_H */
