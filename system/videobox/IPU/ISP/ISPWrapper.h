#ifndef ISP_WRAPPER_H
#define ISP_WRAPPER_H

#include <ispc/Pipeline.h>
#include <ispc/ParameterList.h>
#include <ispc/ParameterFileParser.h>
#include <ispc/ParameterFileParser.h>
#include <ispc/Camera.h>
#include <ispc/Sensor.h>
#include <ispc/ControlAE.h>
#include <ispc/ControlAWB_PID.h>
#if defined(COMPILE_ISP_V2505) && defined(AWB_ALG_PLANCKIAN)
#include <ispc/ControlAWB_Planckian.h>
#endif
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
#include <ispc/ModuleGMA.h>
#include <ispc/Save.h>
#include <sensorapi/sensorapi.h>
#include <ci/ci_api.h>
#include <ispc/ModuleDPF.h>
#include <ispc/ModuleDNS.h>
#include "ispc/ColorCorrection.h"
#include <ispc/TemperatureCorrection.h>
#include <ispc/ModuleBLC.h>


#ifdef COMPILE_ISP_V2505
#include <ispc/ControlLSH.h>
#include <ispc/ModuleLSH.h>
#endif


#include <System.h>
#include <IPU.h>

#include "ddk_sensor_driver.h"

typedef struct _ST_ISP_Context {
    ISPC::Camera *pCamera;
    ISPC::ControlAE *pAE;
#if defined(COMPILE_ISP_V2505) && defined(AWB_ALG_PLANCKIAN)
    ISPC::ControlAWB_Planckian *pAWB;
#else
    ISPC::ControlAWB_PID *pAWB;
#endif
    ISPC::ControlTNM *pTNM;
    ISPC::ControlDNS *pDNS;
    ISPC::ControlCMC *pCMC;
#ifdef COMPILE_ISP_V2505
    ISPC::ControlLSH *pLSH;
#endif
    ISPC::Shot capShot;
    ISPC::Shot *pstShotFrame;
    int iSensorNum;
    IMG_UINT32 u32SensorMode;
    char szSensorName[64];
    char szSetupFile[128];
	reslist_t resl;
    int Context;
    int iContext;
    int iExslevel;
    int iScenario;
    int iWBMode;
    double flDefTargetBrightness;
    IMG_UINT32 u32BufferNum;
    ISPC::Shot tmp_shot;
} ST_ISP_Context;

#define SETUP_PATH  ("/root/.ispddk/")
#define DEV_PATH    ("/dev/ddk_sensor")
#define MAX_CAM_NB  2
//HSBC : Hue, Saturation, Brightness and Contrast
#define IPU_HSBC_MAX      255
#define IPU_HSBC_MIN      0

const float EPSINON = 0.00001;

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

int IAPI_ISP_Create(Json *js);

bool IAPI_ISP_IsCameraNull(IMG_UINT32 Idx);

bool IAPI_ISP_IsShotFrameHasData(IMG_UINT32 Idx, IMG_UINT32 iBufferID);

int IAPI_ISP_GetSensorNum();

int IAPI_ISP_AutoControl(IMG_UINT32 Idx);

int IAPI_ISP_ConfigAutoControl(IMG_UINT32 Idx);

int IAPI_ISP_LoadParameters(IMG_UINT32 Idx);

int IAPI_ISP_SetCaptureIQFlag(IMG_UINT32 Idx,IMG_BOOL Flag);

int IAPI_ISP_SetMCIIFCrop(IMG_UINT32 Idx,int pipW,int pipH);

int IAPI_ISP_GetMCIIFCrop(IMG_UINT32 Idx,int &width,int &height);

int IAPI_ISP_SetEncoderDimensions(IMG_UINT32 Idx, int imgW, int imgH);

int IAPI_ISP_SetDisplayDimensions(IMG_UINT32 Idx, int s32ImgW, int s32ImgH);

int IAPI_ISP_DisableDsc(IMG_UINT32 Idx);

int IAPI_ISP_SetDscScaler(IMG_UINT32 Idx, int s32ImgW, int s32ImgH);

int IAPI_ISP_SetupModules(IMG_UINT32 Idx);

#ifdef COMPILE_ISP_V2500
int IAPI_ISP_DynamicChangeModuleParam(IMG_UINT32 Idx);
#endif

int IAPI_ISP_DynamicChangeIspParameters(IMG_UINT32 Idx);

int IAPI_ISP_DynamicChange3DDenoiseIdx(IMG_UINT32 Idx,IMG_UINT32 ui32DnTargetIdx);

int IAPI_ISP_Program(IMG_UINT32 Idx);

int IAPI_ISP_AllocateBufferPool(IMG_UINT32 Idx,IMG_UINT32 buffNum,std::list<IMG_UINT32> &bufferIds);

int IAPI_ISP_StartCapture(IMG_UINT32 Idx,const bool enSensor=true);

int IAPI_ISP_StopCapture(IMG_UINT32 Idx,const bool enSensor);

int IAPI_ISP_SensorEnable(IMG_UINT32 Idx,const bool enSensor);

int IAPI_ISP_SensorReset(IMG_UINT32 Idx);

int IAPI_ISP_DualSensorUpdate(IMG_UINT32 Idx,ISPC::Shot &shot, ISPC::Shot &shot2);

int IAPI_ISP_EnqueueShot(IMG_UINT32 Idx);

int IAPI_ISP_AcquireShot(IMG_UINT32 Idx,ISPC::Shot &shot,bool updateControl=true);

int IAPI_ISP_DeleteShots(IMG_UINT32 Idx);

int IAPI_ISP_ReleaseShot(IMG_UINT32 Idx,ISPC::Shot & shot);

int IAPI_ISP_DeregisterBuffer(IMG_UINT32 Idx,IMG_UINT32 bufID);

int IAPI_ISP_SetEncScaler(IMG_UINT32 Idx,int imgW,int imgH);

int IAPI_ISP_SetCIPipelineNum(IMG_UINT32 Idx);

int IAPI_ISP_SetIifCrop(IMG_UINT32 Idx,int PipH,int PipW,int PipX,int PipY);

int IAPI_ISP_SetSepiaMode(IMG_UINT32 Idx,int mode);

int IAPI_ISP_SetScenario(IMG_UINT32 Idx,int sn);

int IAPI_ISP_GetScenario(IMG_UINT32 Idx,enum cam_scenario &scen);

int IAPI_ISP_SetExposureLevel(IMG_UINT32 Idx,int level);

int IAPI_ISP_GetExposureLevel(IMG_UINT32 Idx,int &level);

int IAPI_ISP_GetOriTargetBrightness(IMG_UINT32 Idx);

int IAPI_ISP_SetAntiFlicker(IMG_UINT32 Idx,int freq);

int IAPI_ISP_GetAntiFlicker(IMG_UINT32 Idx,int &freq);

int IAPI_ISP_CheckHSBCInputParam(int param);

int IAPI_ISP_SetWb(IMG_UINT32 Idx,int mode);

int IAPI_ISP_GetWb(IMG_UINT32 Idx,int &mode);

int IAPI_ISP_CalcHSBCToOut(double max, double min, double param, int &out_val);

int IAPI_ISP_CalcHSBCToISP(double max, double min, int param, double &out_val);

int IAPI_ISP_SetYuvBrightness(IMG_UINT32 Idx,int brightness);

int IAPI_ISP_GetYuvBrightness(IMG_UINT32 Idx,int &out_brightness);

int IAPI_ISP_GetEnvBrightness(IMG_UINT32 Idx,int &out_bv);

int IAPI_ISP_SetContrast (IMG_UINT32 Idx,int contrast);

int IAPI_ISP_GetContrast (IMG_UINT32 Idx,int &contrast);

int IAPI_ISP_SetSaturation(IMG_UINT32 Idx,int saturation);

int IAPI_ISP_GetSaturation(IMG_UINT32 Idx,int &saturation);

int IAPI_ISP_SetSharpness(IMG_UINT32 Idx,int sharpness);

int IAPI_ISP_GetSharpness(IMG_UINT32 Idx,int &sharpness);

int IAPI_ISP_SetFps(IMG_UINT32 Idx,int fps);

int IAPI_ISP_GetFps(IMG_UINT32 Idx,int &fps);

int IAPI_ISP_EnableAE(IMG_UINT32 Idx,int enable);

int IAPI_ISP_IsAEEnabled(IMG_UINT32 Idx);

int IAPI_ISP_EnableAWB(IMG_UINT32 Idx,int enable);

int IAPI_ISP_IsAWBEnabled(IMG_UINT32 Idx);

int IAPI_ISP_CheckSensorClass(IMG_UINT32 Idx);

int IAPI_ISP_SetSensorIso(IMG_UINT32 Idx,int iso);

int IAPI_ISP_GetSensorIso(IMG_UINT32 Idx,int &iso);

int IAPI_ISP_EnableMonochrome(IMG_UINT32 Idx,int enable);

int IAPI_ISP_IsMonochromeEnabled(IMG_UINT32 Idx);

int IAPI_ISP_EnableWdr(IMG_UINT32 Idx,int enable);

int IAPI_ISP_IsWdrEnabled(IMG_UINT32 Idx);

int IAPI_ISP_SetMirror(IMG_UINT32 Idx,enum cam_mirror_state dir);

int IAPI_ISP_GetMirror(IMG_UINT32 Idx,enum cam_mirror_state &dir);

int IAPI_ISP_SetSensorResolution(IMG_UINT32 Idx,const res_t res);

int IAPI_ISP_GetSnapResolution (IMG_UINT32 Idx, reslist_t *presl);

int IAPI_ISP_SetHue(IMG_UINT32 Idx, int hue);

int IAPI_ISP_GetHue(IMG_UINT32 Idx, int &hue);

bool IAPI_ISP_UseCustomGam(IMG_UINT32 Idx);

int IAPI_ISP_SetGMA(IMG_UINT32 Idx,cam_gamcurve_t *pGmaCurve);

int IAPI_ISP_SetCustomGMA(IMG_UINT32 Idx);

int IAPI_ISP_SetSensorExposure(IMG_UINT32 Idx, int usec);

int IAPI_ISP_GetSensorExposure(IMG_UINT32 Idx, int &usec);

int IAPI_ISP_SetDpfAttr(IMG_UINT32 Idx, const cam_dpf_attr_t *attr);

int IAPI_ISP_GetDpfAtrr(IMG_UINT32 Idx, cam_dpf_attr_t *attr);

int IAPI_ISP_SetShaDenoiseAttr(IMG_UINT32 Idx, const cam_sha_dns_attr_t *attr);

int IAPI_ISP_GetShaDenoiseAtrr(IMG_UINT32 Idx, cam_sha_dns_attr_t *attr);

int IAPI_ISP_SetDnsAttr(IMG_UINT32 Idx, const cam_dns_attr_t *attr);

int IAPI_ISP_GetDnsAtrr(IMG_UINT32 Idx, cam_dns_attr_t *attr);

int IAPI_ISP_SetWbAttr(IMG_UINT32 Idx, const cam_wb_attr_t *attr);

int IAPI_ISP_GetWbAtrr(IMG_UINT32 Idx, cam_wb_attr_t *attr);

int IAPI_ISP_SetYuvGammaAttr(IMG_UINT32 Idx, const cam_yuv_gamma_attr_t *attr);

int IAPI_ISP_GetYuvGammaAtrr(IMG_UINT32 Idx, cam_yuv_gamma_attr_t *attr);

int IAPI_ISP_SetFpsRange(IMG_UINT32 Idx,const cam_fps_range_t *fps_range);

int IAPI_ISP_GetDayMode(IMG_UINT32 Idx, enum cam_day_mode &day_mode);

int IAPI_ISP_SkipFrame(IMG_UINT32 Idx, int skipNb);

int IAPI_ISP_GetSensorsName(IMG_UINT32 Idx,cam_list_t *pName);

int IAPI_ISP_GetPhyStatus(IMG_UINT32 Idx,IMG_UINT32 *status);

int IAPI_ISP_Destroy();

int IAPI_ISP_SaveFrameFile(const char *fileName, cam_data_format_e dataFmt, int idx, ISPC::Shot &shotFrame);

int IAPI_ISP_SaveBayerRaw(const char *fileName, const ISPC::Buffer &bayerBuffer);

int IAPI_ISP_SaveFrame(int idx, ISPC::Shot &shotFrame,cam_save_data_t* psSaveDataParam);

#ifdef COMPILE_ISP_V2505
int IAPI_ISP_GetCurrentFps(IMG_UINT32 Idx, int &out_fps);
int IAPI_ISP_LoadControlParameters(IMG_UINT32 Idx);
int IAPI_ISP_EnableLSH(IMG_UINT32 Idx);
#endif

int IAPI_ISP_SetModuleOUTRawFlag(IMG_UINT32 Idx, bool enable, CI_INOUT_POINTS DataExtractionPoint);

int IAPI_ISP_GetDebugInfo(void *info);
#endif
