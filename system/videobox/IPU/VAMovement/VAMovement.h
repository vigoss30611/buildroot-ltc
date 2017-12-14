// Copyright (C) 2016 InfoTM, warits.wang@infotm.com
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

#ifndef IPU_VAMovement_H
#define IPU_VAMovement_H
#include <IPU.h>

// these settings is ported from imagination DDK
#define HIS_GLOBAL_BINS 64
#define HIS_REGION_VTILES 7
#define HIS_REGION_HTILES 7
#define HIS_REGION_BINS 16

typedef struct {
//    uint32_t resv[HIS_GLOBAL_BINS];
    uint32_t d[HIS_REGION_VTILES][HIS_REGION_HTILES][HIS_REGION_BINS];
} histgram_t;


#define TCC(_y, _x) TileChangeCount[_y][_x]
#define GET_FT SensitiveData[Sensitivity][0]
#define GET_TT SensitiveData[Sensitivity][1]
#define GET_NUM SensitiveData[Sensitivity][2]
#define GET_DIFF(i) SensitiveData[Sensitivity][3+i]

class IPU_VAMovement: public IPU {

private:
public:
    histgram_t *HCurrent;
    histgram_t *HLast;
    int HistogramSize;
    int TileChangeCount[HIS_REGION_VTILES][HIS_REGION_HTILES];
    int Sensitivity;
    int SendBlkInfo;
    uint32_t VATargetFps;
    uint64_t VALastTimeStamp;
    static int SensitiveData[3][7];
    struct event_vamovement EV;
    uint32_t m_u32ResoIndex;
    int m_u32Width;
    int m_u32Height;

    IPU_VAMovement(std::string, Json *);
    void Prepare();
    void Unprepare();
    void WorkLoop();
    int UnitControl(std::string, void*);
    uint32_t CalculateBC(uint32_t f0[], uint32_t f1[]);
    int SetSensitivity(int level);
    int GetSensitivity();
    int GetStatus(int i, int j);
    int SetSampleFps(int target_fps);
    int GetSampleFps();
};

#endif // IPU_VAMovement_H
