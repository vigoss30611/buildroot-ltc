// Copyright (C) 2016 InfoTM, yong.yan@infotm.com
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

#ifndef IPU_SWC_H
#define IPU_SWC_H
#include <IPU.h>
#include <ppapi.h>
#include <hlibispostv1/hlib_ispost.h>

#define IPU_SWC_GRIDDATA_MAX 4

struct ipu_swc_3d_dn_param {
    int y_threshold;
    int u_threshold;
    int v_threshold;
    int weight;
};
typedef enum _COMPOSER_TOOL
{
    PP_G1 = 0,
    ISPOST_V1,
    ISPOST_V2,
}E_COMPOSER_TOOL;

class IPU_SWC : public IPU {

private:
    int channel_flag;
    enum v_nvr_display_mode mode;
    E_COMPOSER_TOOL composertool;

    PPInst pPpInst;
    PPConfig pPpConf;

    cam_3d_dns_attr_t m_Dns3dAttr;
    ISPOST_USER m_IspostUser;
    char* m_pszLcGridFileName[IPU_SWC_GRIDDATA_MAX];
    FRBuffer* m_pLcGridBuf[IPU_SWC_GRIDDATA_MAX];
    int grid_target_index_max;
    Port *m_pPortDn, *m_pPortOut;

    int IspostSet3dDnsCurve(const cam_3d_dns_attr_t *attr, int is_force);
    int IspostProcPrivateData(ipu_v2500_private_data_t *pdata);
    bool IspostJsonGetString(const char*, char*&, Json*);
    ISPOST_UINT32 IspostGetFileSize(FILE* fp);
    bool IspostLoadBinaryFile(const char* pszBufName, FRBuffer** ppBuffer, const char* pszFilenane);
    ISPOST_UINT32 IspostParseGirdSize(const char* pszFileName);
    ISPOST_UINT32 IspostGetGridTargetIndexMax(int width, int height, int sz);

    void IspostCalcOffsetAddress(int x, int y, int stride, ISPOST_UINT32& Yaddr, ISPOST_UINT32& UVaddr);
    void IspostCheckParameter(Port *pin);

    bool IspostInitial();
    void IspostDefaultParameter(void);
    bool IspostTrigger(Buffer& BufDn);
    void SetOutputParamIspost(Port *pt,Buffer &buf, struct v_pip_info *info);
    void SetInputParamIspost(Port *pt,uint32_t phys_addr,struct v_pip_info *infoin);
    void IspostDumpUserInfo(void);
  
    bool CheckParameterPp(struct v_pip_info *infoin,struct v_pip_info *infoout);
    void SetOutputParamPp(Port *pt,Buffer &buf, struct v_pip_info *info);
    void SetInputParamPp(Port *pt,uint32_t phys_addr,struct v_pip_info *infoin);

    void CalculateDispRect(int index,struct v_pip_info *pipinfo,enum v_nvr_display_mode);
    bool CheckPortAttribute();
    void SetNvrMode(struct v_nvr_info *nvr_mode);

public:

    IPU_SWC(std::string, Json*);
    ~IPU_SWC();
    void Prepare();
    void Unprepare();
    void WorkLoop();
    int UnitControl(std::string func, void *arg);
};
#endif //IPU_SWC_H