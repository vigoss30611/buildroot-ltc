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

#ifndef IPU_V2500_H
#define IPU_V2500_H
#include <IPU.h>
#include <list>
#include <ispc/Pipeline.h>
#include <ispc/ParameterList.h>
#include <ispc/ParameterFileParser.h>
#include <ispc/ParameterFileParser.h>
#include <ispc/Camera.h>
#include <ispc/Sensor.h>
#include <ispc/ControlAE.h>
#include <ispc/ControlAWB_PID.h>
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
#include <ispc/ModuleGMA.h>
#include <ispc/Save.h>
#include <sensorapi/sensorapi.h>
#include <ci/ci_api.h>
#include <ispc/ModuleDPF.h>
#include <ispc/ModuleDNS.h>
#include <semaphore.h>

#include "ddk_sensor_driver.h"

//HSBC : Hue, Saturation, Brightness and Contrast
#define IPU_HSBC_MAX      255
#define IPU_HSBC_MIN      0
#define MAX_CAM_NB  2
#define NOTIFY_TIMEOUT_SEC      8

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

typedef struct _V2500_CAM {
    ISPC::Camera *pCamera;
    ISPC::ControlAE *pAE;
    ISPC::ControlAWB_PID *pAWB;
    ISPC::ControlTNM *pTNM;
    ISPC::ControlDNS *pDNS;
    ISPC::ControlCMC *pCMC;
    ISPC::Shot CapShot;
    ISPC::Shot *pstShotFrame;
    std::list < IMG_UINT32 > BuffersIDs;
    IMG_UINT32 SensorMode;
    char szSensorName[64];
    char szSetupFile[128];
	reslist_t resl;
    int Context;
    int Exslevel;
    int Scenario;
    int WBMode;
    double flDefTargetBrightness;
} V2500_CAM;

class IPU_V2500: public IPU {

public:
    V2500_CAM m_Cam[MAX_CAM_NB];
    IMG_UINT32 m_nBuffers;
    ISPC::Shot *m_pstShotFrame;
    int m_iBufferID;
    int m_iBufUseCnt;
    struct ipu_v2500_private_data *m_pstPrvData;
    std::function<int (Buffer*)> m_pFun;
    std::list <void *> m_postShots;
    volatile int m_RunSt;
    enum ipu_v2500_status m_CurrentStatus;

    IPU_V2500(std::string, Json *);
    void Prepare();
    void Unprepare();
    void WorkLoop();
    int UnitControl(std::string, void *);

    // FIXME: CHANGE ALL RETURN VALUE TO int and also the parameters
    int GetSensorsName(IMG_UINT32 Idx, cam_list_t * pName);
    int GetYUVBrightness(IMG_UINT32 Idx, int &out_brightness);
    int SetYUVBrightness(IMG_UINT32 Idx, int brightness);
    int GetEnvBrightnessValue(IMG_UINT32 Idx, int &out_bv);
    int GetContrast(IMG_UINT32 Idx, int &out_contrast);
    int GetSnapResolution(IMG_UINT32 Idx, reslist_t *resl);
    int SetSnapResolution(IMG_UINT32 Idx, const res_t res);
    int SnapOneShot();
    int SnapExit();
    int SetContrast(IMG_UINT32 Idx, int contrast);
    int GetSaturation(IMG_UINT32 Idx, int &out_saturation);
    int SetSaturation(IMG_UINT32 Idx, int saturation);
    int GetSharpness(IMG_UINT32 Idx, int &out_sharpness);
    int SetSharpness(IMG_UINT32 Idx, int sharpness);
    int GetHue(IMG_UINT32 Idx, int &out_hue);
    int SetHue(IMG_UINT32 Idx, int hue);
    int SetAntiFlicker(IMG_UINT32 Idx, int freq);
    int GetAntiFlicker(IMG_UINT32 Idx, int &freq);
    int SetExposureLevel(IMG_UINT32 Idx, int level);
    int GetExposureLevel(IMG_UINT32 Idx, int &level);
    int SetSensorISO(IMG_UINT32 Idx, int iso);
    int GetSensorISO(IMG_UINT32 Idx, int &iso);
    int EnableAE(IMG_UINT32 Idx, int enable);
    int EnableAWB(IMG_UINT32 Idx, int enable);
    int EnableMonochrome(IMG_UINT32 Idx, int enable);
    int EnableWDR(IMG_UINT32 Idx, int enable);
    int GetMirror(IMG_UINT32 Idx, enum cam_mirror_state &dir);
    int SetMirror(IMG_UINT32 Idx, enum cam_mirror_state dir);
    int SetResolution(IMG_UINT32 Idx, int imgW, int imgH);
    int GetResolution(int &imgW, int &imgH);
    int SetFPS(IMG_UINT32 Idx, int fps);
    int GetFPS(IMG_UINT32 Idx, int &fps);  // FIXME: int
    int IsWDREnabled(IMG_UINT32 Idx);
    int IsAEEnabled(IMG_UINT32 Idx);
    int IsAWBEnabled(IMG_UINT32 Idx);
    int IsMonchromeEnabled(IMG_UINT32 Idx); // FIXME: wrong spelling
    int SetWB(IMG_UINT32 Idx, int mode);
    int GetWB(IMG_UINT32 Idx, int &mode);
    //sn =1 low light scenario, else hight light scenario
    int SetScenario(IMG_UINT32 Idx, int sn);
    int GetScenario(IMG_UINT32 Idx, enum cam_scenario &scen);
    int SetSepiaMode(IMG_UINT32 Idx, int mode);
    int SetSensorResolution(IMG_UINT32 Idx, const res_t res);
    int ReturnShot(Buffer *pstBuf);
    int EnqueueReturnShot(void);
    int ClearAllShot();
    virtual ~IPU_V2500();

    int SetSensorExposure(IMG_UINT32 Idx, int usec);
    int GetSensorExposure(IMG_UINT32 Idx, int &usec);

    int SetDpfAttr(IMG_UINT32 Idx, const cam_dpf_attr_t *attr);
    int GetDpfAttr(IMG_UINT32 Idx, cam_dpf_attr_t *attr);

    int SetDnsAttr(IMG_UINT32 Idx, const cam_dns_attr_t *attr);
    int GetDnsAttr(IMG_UINT32 Idx, cam_dns_attr_t *attr);

    int SetShaDenoiseAttr(IMG_UINT32 Idx, const cam_sha_dns_attr_t *attr);
    int GetShaDenoiseAttr(IMG_UINT32 Idx, cam_sha_dns_attr_t *attr);

    int SetWbAttr(IMG_UINT32 Idx, const cam_wb_attr_t *attr);
    int GetWbAttr(IMG_UINT32 Idx, cam_wb_attr_t *attr);

    int Set3dDnsAttr(const cam_3d_dns_attr_t *attr);
    int Get3dDnsAttr(cam_3d_dns_attr_t *attr);

    int SetYuvGammaAttr(IMG_UINT32 Idx, const cam_yuv_gamma_attr_t *attr);
    int GetYuvGammaAttr(IMG_UINT32 Idx, cam_yuv_gamma_attr_t *attr);

    int SetFpsRange(IMG_UINT32 Idx, const cam_fps_range_t *fps_range);
    int GetFpsRange(IMG_UINT32 Idx, cam_fps_range_t *fps_range);
    int GetDayMode(IMG_UINT32 Idx, enum cam_day_mode &out_day_mode);
    int SaveData(cam_save_data_t *saveData);
    int GetDebugInfo(void *pInfo, int *ps32Size);
#if defined(COMPILE_IQ_TUNING_TOOL_APIS)

    int GetIQ(IMG_UINT32 Idx, void* pdata);
    int SetIQ(IMG_UINT32 Idx, void* pdata);
    int BackupIspParameters(IMG_UINT32 Idx, ISPC::ParameterList & Parameters);
    int SaveIspParameters(IMG_UINT32 Idx, cam_save_data_t *saveData);
#endif //#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
private:
    int ui32SkipFrame;
    int m_ImgW, m_ImgH;
    int m_PipX, m_PipY, m_PipW, m_PipH;
    int m_LockRatio;
    int m_InW;
    int m_InH;
    int m_MinFps;
    int m_MaxFps;
    const float EPSINON = 0.00001;
    cam_3d_dns_attr_t dns_3d_attr;
    cam_fps_range_t sFpsRange;
    IMG_UINT32 m_iSensorNb;
    IMG_UINT32 m_iCurCamIdx;
    cam_save_data_t* m_psSaveDataParam;
    sem_t m_semNotify;
#ifdef COMPILE_IQ_REALTIME_SAVE_RAW
    int m_hasSaveRaw;
#endif
    CI_INOUT_POINTS m_eDataExtractionPoint;

    int SetEncScaler(IMG_UINT32 Idx, int imgW = 1920, int imgH = 1088);
    int SetIIFCrop(IMG_UINT32 Idx, int pipH, int pipW, int pipX, int pipY);
    void BuildPort(int imgW = 1920, int imgH = 1088);
    int AutoControl(IMG_UINT32 Idx);
    int ConfigAutoControl(IMG_UINT32 Idx);
    int CheckHSBCInputParam(int param);
    int CalcHSBCToISP(double max, double min, int param, double &out_val);
    int CalcHSBCToOut(double max, double min, double param, int &out_val);

    int SkipFrame(IMG_UINT32 Idx, int skipNb);
    int CheckAEDisabled(IMG_UINT32 Idx);
    int CheckSensorClass(IMG_UINT32 Idx);
    int CheckOutRes(IMG_UINT32 Idx, int imgW, int imgH);
    int InitCamera(Json *js);
    void DeinitCamera();
    int SetCameraIdx(int Idx);
    int GetCameraIdx(int &Idx);
    int GetPhyStatus(IMG_UINT32 Idx, IMG_UINT32 *status);
    int SetGAMCurve(IMG_UINT32 Idx,cam_gamcurve_t *GamCurve);
    int SaveFrame(int idx, ISPC::Shot &shotFrame);
    int SaveFrameFile(const char *fileName, cam_data_format_e dataFmt, int idx, ISPC::Shot &shotFrame);
    int SaveBayerRaw(const char *fileName, const ISPC::Buffer &bayerBuffer);
#if defined(COMPILE_IQ_TUNING_TOOL_APIS)

    // IQ tuning using function.
    bool m_bIqTuningDebugInfoEnable;
    ISPC::ParameterList m_Parameters[MAX_CAM_NB];
    //ISP_IIF = 1, /**< @brief Imager interface */
    //ISP_EXS, /**< @brief Encoder statistics */
    int GetBLCParam(IMG_UINT32 Idx, void* pData);
    int SetBLCParam(IMG_UINT32 Idx, void* pData);
    //ISP_RLT, /**< @brief Raw Look Up table correction (HW v2 only) */
    //ISP_LSH, /**< @brief Lens shading */
    //int GetWBCParam(IMG_UINT32 Idx, void* pData);
    //int SetWBCParam(IMG_UINT32 Idx, void* pData);
    //ISP_FOS, /**< @brief Focus statistics */
    int GetDNSParam(IMG_UINT32 Idx, void* pData);
    int SetDNSParam(IMG_UINT32 Idx, void* pData);
    int GetDPFParam(IMG_UINT32 Idx, void* pData);
    int SetDPFParam(IMG_UINT32 Idx, void* pData);
    //ISP_ENS, /**< @brief Encoder Statistics */
    //ISP_LCA, /**< @brief Lateral Chromatic Aberration */
    //ISP_CCM, /**< @brief Color Correction */
    //ISP_MGM, /**< @brief Main Gammut Mapper */
    int GetGMAParam(IMG_UINT32 Idx, void* pData);
    int SetGMAParam(IMG_UINT32 Idx, void* pData);
    //ISP_WBS, /**< @brief White Balance Statistics */
    //ISP_HIS, /**< @brief Histogram Statistics */
    int GetR2YParam(IMG_UINT32 Idx, void* pData);
    int SetR2YParam(IMG_UINT32 Idx, void* pData);
    //ISP_MIE, /**< @brief Main Image Enhancer (Memory Colour part) */
    //ISP_VIB, /**< @brief Vibrancy (part of MIE block in HW) */
    int GetTNMParam(IMG_UINT32 Idx, void* pData);
    int SetTNMParam(IMG_UINT32 Idx, void* pData);
    //ISP_FLD, /**< @brief Flicker Detection */
    int GetSHAParam(IMG_UINT32 Idx, void* pData);
    int SetSHAParam(IMG_UINT32 Idx, void* pData);
    //ISP_ESC, /**< @brief Encoder Scaler */
    //ISP_DSC, /**< @brief Display Scaler */
    //ISP_Y2R, /**< @brief YUV to RGB conversions */
    //ISP_DGM, /**< @brief Display Gammut Maper */
    int GetWBCAndCCMParam(IMG_UINT32 Idx, void* pData);
    int SetWBCAndCCMParam(IMG_UINT32 Idx, void* pData);
    int GetAECParam(IMG_UINT32 Idx, void* pData);
    int SetAECParam(IMG_UINT32 Idx, void* pData);
    int GetAWBParam(IMG_UINT32 Idx, void* pData);
    int SetAWBParam(IMG_UINT32 Idx, void* pData);
    int GetCMCParam(IMG_UINT32 Idx, void* pData);
    int SetCMCParam(IMG_UINT32 Idx, void* pData);
    int GetTNMCParam(IMG_UINT32 Idx, void* pData);
    int SetTNMCParam(IMG_UINT32 Idx, void* pData);
    int GetIICParam(IMG_UINT32 Idx, void* pData);
    int SetIICParam(IMG_UINT32 Idx, void* pData);
    int GetOutParam(IMG_UINT32 Idx, void* pData);
    int SetOutParam(IMG_UINT32 Idx, void* pData);
    int GetIspVersion(IMG_UINT32 Idx, void* pData);
    int GetIqTuningDebugInfo(IMG_UINT32 Idx, void* pData);
    int SetIqTuningDebugInfo(IMG_UINT32 Idx, void* pData);
#endif //#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
    int SetModuleOUTRawFlag(IMG_UINT32 idx, bool enable, CI_INOUT_POINTS DataExtractionPoint);
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

#endif /* IPU_V2500_H */
