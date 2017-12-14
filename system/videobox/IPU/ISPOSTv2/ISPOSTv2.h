// Copyright (C) 2016 InfoTM, jazz.chang@infotm.com,
//    yong.yan@infotm.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef IPU_ISPOSTV2_H
#define IPU_ISPOSTV2_H

#include <vector>
#include <list>
#include <IPU.h>
#include <hlibispostv2/hlib_ispost.h>
#ifdef MACRO_IPU_ISPOSTV2_FCE
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

#ifdef INFOTM_HW_BLEND
#define INFOTM_HW_BLENDING
#endif

#if defined(INFOTM_HW_BLENDING) || defined(INFOTM_SW_BLENDING)
  #if defined(INFOTM_HW_BLENDING)
#define INFOTM_OV_WIDTH         (32)
#define INFOTM_OV_HEIGHT        (960)
#define INFOTM_OV_Y_ALPHA_PWR   (5)
#define INFOTM_OV_UV_ALPHA_PWR  (INFOTM_OV_Y_ALPHA_PWR - 1)
  #elif defined(INFOTM_SW_BLENDING)
#define INFOTM_OV_WIDTH         (16)
#define INFOTM_OV_HEIGHT        (960)
#define INFOTM_OV_Y_ALPHA_PWR   (4)
#define INFOTM_OV_UV_ALPHA_PWR  (INFOTM_OV_Y_ALPHA_PWR - 1)
  #endif //#if defined(INFOTM_HW_BLENDING)

typedef enum _BLENDING_SIDE {
    BLENDING_SIDE_LEFT = 0,
    BLENDING_SIDE_RIGHT,
    BLENDING_SIDE_MAX
} BLENDING_SIDE, *PBLENDING_SIDE;

typedef union _ISPOST_ULONGLONG {
    unsigned long long ullValue;
    unsigned char byValue[8];
} ISPOST_ULONGLONG, *PISPOST_ULONGLONG;

typedef struct _BUF_INFO {
    ISPOST_UINT32 ui32YAddr;
    ISPOST_UINT32 ui32UVAddr;
    ISPOST_UINT16 ui16Stride;
    ISPOST_UINT16 ui16Width;
    ISPOST_UINT16 ui16Height;
    ISPOST_UINT16 ui16OffsetX;
    ISPOST_UINT16 ui16OffsetY;
} BUF_INFO, *PBUF_INFO;

#endif //#if defined(INFOTM_HW_BLENDING) || defined(INFOTM_SW_BLENDING)


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

typedef struct
{
    int modeID;
    int x;
    int y;
    int width;
    int height;
    int stride;
    int overlayTiming;
    struct fr_buf_info stExtBuf;
}ST_EXT_BUF_INFO;


typedef struct {
    int modeID;
    int inW;
    int inH;
    int outW;
    int outH;
    std::vector<ST_GRIDDATA_BUF_INFO> stBufList;
    std::vector<ST_EXT_BUF_INFO> stExtBufList;
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

class IPU_ISPOSTV2: public IPU
{
private:
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
    int m_Buffers, m_Rotate, grid_target_index_max;
    bool m_IsFisheye;
    bool m_IsBlendScale;
    bool m_IsLineBufEn;
    ISPOST_UINT32 m_PortEnable, m_PortGetBuf;
    bool m_IsDnManual, m_IsLcManual, m_IsOvManual;
    int m_DnManualVal, m_LcManualVal, m_OvManualVal;
    cam_3d_dns_attr_t m_Dns3dAttr;
    cam_common_mode_e m_eDnsModePrev;
    pthread_mutex_t m_PipMutex;
    ST_FCE m_stFce;
    pthread_mutex_t m_FceLock;
    bool m_HasFceData;
    bool m_HasFceFile;
    int m_LinkMode;
    int m_LinkBuffers;
    std::function<int (Buffer*)> m_pBufFunc;
    std::list<struct fr_buf_info> m_stInBufList;
    pthread_mutex_t m_BufLock;
    ISPOST_UINT32 m_ui32DnRefImgBufYaddr;
    int m_RefBufSerial;
    cam_fcefile_param_t m_stFceJsonParam;
    int m_s32LcOutImgWidth,m_s32LcOutImgHeight;
#if defined(COMPILE_ISPOSTV2_IQ_TUNING_TOOL_APIS)
    bool m_bIqTuningDebugInfoEnable;
    char m_szGridFilePath[2][256];
#endif

    int m_SnapMode;
    int m_SnapBufMode;
    Buffer m_SnapUoBuf;
    pthread_mutex_t m_SnapBufLock;
    int m_SnapInW, m_SnapInH;
    int m_SnapOutW, m_SnapOutH;
    std::function<int (Buffer*)> m_pSnapBufFunc;

    int ReturnBuf(Buffer *pstBuf);
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
#if defined(COMPILE_ISPOSTV2_IQ_TUNING_TOOL_APIS)
    int IspostGetDnParamCount(void);
    ipu_ispost_3d_dn_param * IspostGetDnParamList(void);
    int GetDNParam(void * pData);
    int SetDNParam(void * pData);
    int GetIspostVersion(void * pData);
    int GetIqTuningDebugInfo(void * pData);
    int SetIqTuningDebugInfo(void * pData);
    int GetGridFilePath(void* pData);
    int GetIspost(void * pdata);
    int SetIspost(void * pdata);
#endif //#if defined(COMPILE_ISPOSTV2_IQ_TUNING_TOOL_APIS)
    int IspostCreateAndFireFceData(cam_fcedata_param_t  *pstParam);
    int IspostLoadAndFireFceData(cam_fcefile_param_t *pstParam);
    int UpdateDnUserInfo(bool hasFce, cam_position_t *pstPos, ISPOST_UINT32 *pRefBufYAddr);
    int UpdateUoUserInfo(bool hasFce, cam_position_t *pstPos);
    int UpdateSs0UserInfo(bool hasFce, cam_position_t *pstPos);
    int UpdateLcUserInfo(bool hasFce, cam_position_t *pstPos);
    int UpdateSs1UserInfo(bool hasFce, cam_position_t *pstPos);
    int IspostSetFceMode(int mode);
    int IspostSetFceModebyResolution(int inW, int inH, int outW, int outH);
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

#ifdef MACRO_IPU_ISPOSTV2_FCE
    void ParseFceDataParam(cam_fisheye_correction_t *pstFrom, TFceGenGridDataParam *pstTo);
#endif
    int ParseFceFileJson(char *fileName, cam_fcefile_param_t *pstParam);
    int SetupLcInfo();



public:
    IPU_ISPOSTV2(std::string name, Json* js);
    ~IPU_ISPOSTV2();
    void Prepare();
    void Unprepare();
    void WorkLoop();
    void IspostCheckParameter();
    void IspostDefaultParameter(void);
    bool IspostInitial(void);
    bool IspostTrigger(void);
    int UnitControl(std::string func, void* arg);
    int IspostSetLcGridTargetIndex(int Index);
    int IspostGetLcGridTargetIndex();
    int IspostGetLcGridTargetCount();
    void SetOvPaletteTable(ST_OVERLAY_INFO* ov_info, ISPOST_UINT32 flag);
    int IspostSetPipInfo(struct v_pip_info *vpinfo);
    int IspostGetPipInfo(struct v_pip_info *vpinfo);
    int IspostFormatPipInfo(struct v_pip_info *vpinfo);
    void IspostAdjustOvPos(int ov_idx);
    int ReturnSnapBuf(Buffer *pstBuf);
    int IspostGetSnapMaxSize(int &maxSize, int &modeID);
    void PutAllBuffer(void);
    void IspostSaveDataFromUO(IMG_INFO *stImgInfo, ST_EXT_BUF_INFO *stExtBufInfo);
#if defined(INFOTM_HW_BLENDING)
    void IspostAdjustOvAlphaValue(ISPOST_UINT8 ui8Side, void* pVirtAddr, ISPOST_UINT16 ui16Stride, ISPOST_UINT16 ui16Height);
    void IspostExtBufForHWBlending(ISPOST_UINT8 ui8location, ST_EXT_BUF_INFO *stExtBufInfo);
#endif //#if defined(INFOTM_HW_BLENDING)
#if defined(INFOTM_SW_BLENDING)
    void IspostSwOverlay(ISPOST_UINT8 ui8Side, BUF_INFO stImgBufInfo, BUF_INFO stOvBufInfo);
    void IspostExtBufForSWBlending(ISPOST_UINT8 ui8location, ST_EXT_BUF_INFO *stExtBufInfo);
#endif //#if defined(INFOTM_SW_BLENDING)
};
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

#endif // IPU_ISPOSTV2_H
