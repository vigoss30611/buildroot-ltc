// Copyright (C) 2016 InfoTM, jazz.chang@infotm.com,
//    raven.wang@infotm.com
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

#ifndef IPU_FODETV2_H
#define IPU_FODETV2_H

#include <IPU.h>
#include <hlibfodetv2/hlib_fodet.h>

#define IPU_FODET_DATABASE_MAX        3
//#define IPU_SOURCE_IMG_FROM_JSON_FILE
#ifdef IPU_SOURCE_IMG_FROM_JSON_FILE
#define IPU_FODET_SRC_IMG_MAX		3
#endif

//#define IPU_TEST_FD_PERFORMANCE_TIME_MEASURE

#define FODET_PT_IN           0x0001
#define BASE_SF 1.1f

class IPU_FODETV2: public IPU
{
private:
    int frame_idt;
    Json* m_pJsonMain;
    FODET_USER m_FodetUser;

#if defined(IPU_SOURCE_IMG_FROM_JSON_FILE)
    char* m_pszSrcImgFileName[IPU_FODET_SRC_IMG_MAX];
    FRBuffer* m_pSrcImgBuf[IPU_FODET_SRC_IMG_MAX];
    Buffer m_ImgBuf;
#endif
    char* m_pszFodDatabaseFileName[IPU_FODET_DATABASE_MAX];
    FRBuffer* m_pFodDatabaseBuf[IPU_FODET_DATABASE_MAX];
    Buffer m_DbBuf;

    FRBuffer* m_pIntegralImageBuf;
    Buffer m_IIsumBuf;

    FRBuffer* m_pHidCascadeBuf;
    Buffer m_HidccBuf;

    Port *m_pPortIn;
    Buffer m_BufIn;
    Port *m_pPortOut;
    Buffer m_BufOut;

    CvAvgComp m_avgComps1[MAX_ARY_SCAN_ALL];
    CvAvgComp m_avgComps2[MAX_ARY_SCAN_ALL];
    CvPTreeNode m_treeNode[MAX_ARY_SCAN_ALL];
    FODET_INT32 m_i32FilterIndex;
    CvRect m_OutputCoordinate[MAX_ARY_SCAN_ALL];
    OR_DATA stOR_Data[25];
    struct event_fodet e_fd;

    bool FodetJsonGetInt(const char* pszItem, int& Value);
    bool FodetJsonGetString(const char* pszItem, char*& pszString);
    void FodetLog(const char* format, ...);
    FODET_UINT32 FodetGetFileSize(FILE* pFileID);
    bool FodetLoadBinaryFile(const char* pszBufName, FRBuffer** ppBuffer, const char* pszFilenane);
    void FodetCalcWinParameter(void);
    void FodetCalcPlanarAddress(int w, int h, FODET_UINT32& Yaddr, FODET_UINT32& UVaddr);
    void FodetCalcOffsetAddress(int x, int y, int Stride, FODET_UINT32& Yaddr, FODET_UINT32& UVaddr);
    void FodetClearFRBuffer(FRBuffer* fr_buf);
    void FodetDumpUserInfo(void);
    int cvRound(double value);
    int is_equal(const void* _r1, const void* _r2);
    bool FodetGetCoordinate(void);
    bool FodetGrouping(void);

public:
    IPU_FODETV2(std::string name, Json *js);
    ~IPU_FODETV2();
    void Prepare();
    void Unprepare();
    void WorkLoop();

    void FodetDefaultParameter(void);
    void FodetInitGroupingArray(void);

    bool FodetInitial(void);
    bool FodetObjectDetect(void);
    void FodetGetOutput(CvRect* pArray);

    int UnitControl(std::string func, void* arg);
};

#endif // IPU_FODETV2_H
