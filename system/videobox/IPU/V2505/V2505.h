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
#include <ispc/ModuleDSC.h>
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


//HSBC : Hue, Saturation, Brightness and Contrast
#define IPU_HSBC_MAX      255
#define IPU_HSBC_MIN      0
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


class IPU_V2500: public IPU {

public:
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
    IPU_V2500(std::string, Json *);
    void Prepare();
    void Unprepare();
    void WorkLoop();
    int UnitControl(std::string, void *);

    // FIXME: CHANGE ALL RETURN VALUE TO int and also the parameters
    int GetSensorsName(cam_list_t * pName);
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
    virtual ~IPU_V2500();

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
    int SetGAMCurve(cam_gamcurve_t *GamCurve);
    int GetDebugInfo(void *pInfo, int *ps32Size);
#if defined(COMPILE_IQ_TUNING_TOOL_APIS)

    int GetIQ(void* pdata);
    int SetIQ(void* pdata);
    int BackupIspParameters(ISPC::ParameterList & Parameters);
    int SaveIspParameters(cam_save_data_t *saveData);
#endif //#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
private:
    char psname[64];
    char pSetupFile[128];
    int ImgH;
    int ImgW;
    int m_s32DscImgH;
    int m_s32DscImgW;
    int m_s32enDsc;
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
#ifdef AWB_ALG_PLANCKIAN
    ISPC::ControlAWB_Planckian *pAWB;
#else
    ISPC::ControlAWB_PID *pAWB;
#endif
    ISPC::ControlTNM *pTNM;
    ISPC::ControlDNS *pDNS;
    ISPC::ControlCMC *pCMC;
    ISPC::ControlLSH *pLSH;
    double flDefTargetBrightness;
    const float EPSINON = 0.00001;
    cam_3d_dns_attr_t dns_3d_attr;
    cam_fps_range_t sFpsRange;
    int ui32SkipFrame;
    cam_save_data_t *m_psSaveDataParam;
    sem_t m_semNotify;
#ifdef COMPILE_IQ_REALTIME_SAVE_RAW
    int m_hasSaveRaw;
#endif
    CI_INOUT_POINTS m_eDataExtractionPoint;

    int SetDscScaler(int s32ImgW = 1920, int s32ImgH = 1088);
    int SetEncScaler(int imgW = 1920, int imgH = 1088);
    int SetIIFCrop(int pipH, int pipW, int pipX, int pipY);
    void BuildPort(int imgW= 1920, int imgH = 1088);
    int AutoControl();
    int ConfigAutoControl();
    int CheckHSBCInputParam(int param);
    int CalcHSBCToISP(double max, double min, int param, double &out_val);
    int CalcHSBCToOut(double max, double min, double param, int &out_val);
    int SetScalerFromPort();
    int CheckOutRes(std::string name, int imgW, int imgH);
    int CheckAEDisabled();
    int CheckSensorClass();
    int SkipFrame(int skipNb);
    int SaveFrame(ISPC::Shot &shotFrame);
    int SaveFrameFile(const char *filePath, cam_data_format_e data_fmt, ISPC::Shot &shotFrame);
    int SaveBayerRaw(const char *fileName, const ISPC::Buffer &bayerBuffer);
#if defined(COMPILE_IQ_TUNING_TOOL_APIS)

    // IQ tuning using function.
    bool m_bIqTuningDebugInfoEnable;
    ISPC::ParameterList m_Parameters;
    //ISP_IIF = 1, /**< @brief Imager interface */
    //ISP_EXS, /**< @brief Encoder statistics */
    int GetBLCParam(void* pData);
    int SetBLCParam(void* pData);
    //ISP_RLT, /**< @brief Raw Look Up table correction (HW v2 only) */
    //ISP_LSH, /**< @brief Lens shading */
    //int GetWBCParam(void* pData);
    //int SetWBCParam(void* pData);
    //ISP_FOS, /**< @brief Focus statistics */
    int GetDNSParam(void* pData);
    int SetDNSParam(void* pData);
    int GetDPFParam(void* pData);
    int SetDPFParam(void* pData);
    //ISP_ENS, /**< @brief Encoder Statistics */
    //ISP_LCA, /**< @brief Lateral Chromatic Aberration */
    //ISP_CCM, /**< @brief Color Correction */
    //ISP_MGM, /**< @brief Main Gammut Mapper */
    int GetGMAParam(void* pData);
    int SetGMAParam(void* pData);
    //ISP_WBS, /**< @brief White Balance Statistics */
    //ISP_HIS, /**< @brief Histogram Statistics */
    int GetR2YParam(void* pData);
    int SetR2YParam(void* pData);
    //ISP_MIE, /**< @brief Main Image Enhancer (Memory Colour part) */
    //ISP_VIB, /**< @brief Vibrancy (part of MIE block in HW) */
    int GetTNMParam(void* pData);
    int SetTNMParam(void* pData);
    //ISP_FLD, /**< @brief Flicker Detection */
    int GetSHAParam(void* pData);
    int SetSHAParam(void* pData);
    //ISP_ESC, /**< @brief Encoder Scaler */
    //ISP_DSC, /**< @brief Display Scaler */
    //ISP_Y2R, /**< @brief YUV to RGB conversions */
    //ISP_DGM, /**< @brief Display Gammut Maper */
    int GetWBCAndCCMParam(void* pData);
    int SetWBCAndCCMParam(void* pData);
    int GetAECParam(void* pData);
    int SetAECParam(void* pData);
    int GetAWBParam(void* pData);
    int SetAWBParam(void* pData);
    int GetCMCParam(void* pData);
    int SetCMCParam(void* pData);
    int GetTNMCParam(void* pData);
    int SetTNMCParam(void* pData);
    int GetIICParam(void* pData);
    int SetIICParam(void* pData);
    int GetOutParam(void* pData);
    int SetOutParam(void* pData);
    int GetIspVersion(void* pData);
    int GetIqTuningDebugInfo(void* pData);
    int SetIqTuningDebugInfo(void* pData);
#endif //#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
    int SetModuleOUTRawFlag(bool enable, CI_INOUT_POINTS DataExtractionPoint);
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
