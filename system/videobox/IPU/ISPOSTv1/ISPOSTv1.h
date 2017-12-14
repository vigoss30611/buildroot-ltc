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

#ifndef IPU_ISPOSTV1_H
#define IPU_ISPOSTV1_H

#include <vector>
#include <list>
#include <IPU.h>
#include <hlibispostv1/hlib_ispost.h>
#ifdef MACRO_IPU_ISPOSTV1_FCE
#include "libgridtools.h"
#endif

#define IPU_ISPOST_GRIDDATA_MAX     4
#define IPU_ISPOST_DN_LEVEL_MAX    12

//#define IPU_ISPOST_LC_DEBUG
//#define IPU_ISPOST_OVERLAY_TEST

#define GRIDDATA_ALIGN_SIZE  64
#define LOADFCEDATA_MAX     1
#define CREATEFCEDATA_MAX   2

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

class IPU_ISPOSTV1: public IPU
{
private:
    Json* m_pJsonMain;
    ISPOST_USER m_IspostUser;
    char* m_pszLcGridFileName[IPU_ISPOST_GRIDDATA_MAX];
    char* m_pszOvImgFileName;
    char* m_pszOvFontFileName;
    FRBuffer* m_pLcGridBuf[IPU_ISPOST_GRIDDATA_MAX];
    FRBuffer* m_pOvImageBuf;
    FRBuffer* m_pOvFontBuf;
    Port *m_pPortIn, *m_pPortOl;
    Port *m_pPortDn, *m_pPortSs0, *m_pPortSs1;
    int m_Buffers, m_Rotate, grid_target_index_max;
    bool g_portDn_flag;
    bool DnEnableFlag, SS0EnableFlag, SS1EnableFlag, OlEnableFlag, InEnableFlag, OlAvailable;
    cam_3d_dns_attr_t m_Dns3dAttr;
    cam_common_mode_e m_eDnsModePrev;
    pthread_mutex_t m_PipMutex;
    ST_FCE m_stFce;
    pthread_mutex_t m_FceLock;
    bool m_HasFceData;
    bool m_HasFceFile;
    ISPOST_UINT32 m_ui32DnRefImgBufYaddr;
    cam_fcefile_param_t m_stFceJsonParam;
    int m_s32LcOutImgWidth,m_s32LcOutImgHeight;
#if defined(COMPILE_ISPOSTV1_IQ_TUNING_TOOL_APIS)
    bool m_bIqTuningDebugInfoEnable;
    char m_szGridFilePath[2][512];
#endif

    bool IspostJsonGetInt(const char* pszItem, int& Value);
    bool IspostJsonGetString(const char* pszItem, char*& pszString);
    ISPOST_UINT32 IspostGetFileSize(FILE* pFileID);
    bool IspostLoadBinaryFile(const char* pszBufName, FRBuffer** ppBuffer, const char* pszFilenane);
    ISPOST_UINT32 IspostParseGirdSize(const char* pszFileName);
    ISPOST_UINT32 IspostGetGridTargetIndexMax(int width, int height, int sz);
    void IspostCalcPlanarAddress(int w, int h, ISPOST_UINT32& Yaddr, ISPOST_UINT32& UVaddr);
    void IspostCalcOffsetAddress(int x, int y, int stride, ISPOST_UINT32& Yaddr, ISPOST_UINT32& UVaddr);
    void IspostDumpUserInfo(void);
    int IspostSet3dDnsCurve(const cam_3d_dns_attr_t *attr, int is_force);
    int IspostProcPrivateData(ipu_v2500_private_data_t *pdata);
#if defined(COMPILE_ISPOSTV1_IQ_TUNING_TOOL_APIS)
    ipu_ispost_3d_dn_param * IspostGetDnParamList(void);
    int IspostGetDnParamCount(void);
    int GetDNParam(void * pData);
    int SetDNParam(void * pData);
    int GetIspostVersion(void * pData);
    int GetIqTuningDebugInfo(void * pData);
    int SetIqTuningDebugInfo(void * pData);
    int GetGridFilePath(void* pData);
    int GetIspost(void * pdata);
    int SetIspost(void * pdata);
#endif
    int IspostCreateAndFireFceData(cam_fcedata_param_t  *pstParam);
    int IspostLoadAndFireFceData(cam_fcefile_param_t *pstParam);
    int UpdateDnUserInfo(bool hasFce, cam_position_t *pstPos, ISPOST_UINT32 *pRefBufYAddr, Buffer *pPortBuf);
    int UpdateLcUserInfo(bool hasFce, cam_position_t *pstPos);
    int UpdateSs0UserInfo(bool hasFce, cam_position_t *pstPos, Buffer *pPortBuf);
    int UpdateSs1UserInfo(bool hasFce, cam_position_t *pstPos, Buffer *pPortBuf);
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
#ifdef MACRO_IPU_ISPOSTV1_FCE
    void ParseFceDataParam(cam_fisheye_correction_t *pstFrom, TFceGenGridDataParam *pstTo);
#endif
    int ParseFceFileJson(char *fileName, cam_fcefile_param_t *pstParam);
    int SetupLcInfo();

public:
    IPU_ISPOSTV1(std::string name, Json *js);
    ~IPU_ISPOSTV1();
    void Prepare();
    void Unprepare();
    void WorkLoop();
    void IspostCheckParameter();
    void IspostDefaultParameter(void);
    bool IspostInitial(void);
    bool IspostTrigger(Buffer& BufIn, Buffer& BufOl, Buffer& BufDn, Buffer& BufSs0, Buffer& BufSs1);
    int UnitControl(std::string func, void *arg);
    int IspostSetLcTargetIndex(int index);
    int IspostGetLcTargetIndex();
    int IspostGetLcTargetCount();
    int IspostSetPipInfo(struct v_pip_info *vpinfo);
    int IspostGetPipInfo(struct v_pip_info *vpinfo);
    int IspostFormatPipInfo(struct v_pip_info *vpinfo);
    void IspostAdjustOvPos();
};
    #define IspostCheckParam(param) \
    ({ int ret = 0; \
        \
        if (NULL == param) \
        { \
            LOGE("(%s,L%d) Error: %s is NULL !!!\n", __func__, __LINE__, #param); \
            ret = -1; \
        } \
        ret; \
    })

#endif // IPU_ISPOSTV1_H
