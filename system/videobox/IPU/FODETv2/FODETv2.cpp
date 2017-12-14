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

#include <System.h>
#include "FODETv2.h"


#define FD_LOG(X...) //FodetLog(X);
#define FD_INFO(X...) FodetLog(X);


DYNAMIC_IPU(IPU_FODETV2, "fodetv2");



void IPU_FODETV2::WorkLoop()
{
#if defined(IPU_TEST_FD_PERFORMANCE_TIME_MEASURE)
    struct timeval fd_tv;
    unsigned long fd_val=0;
    unsigned long pre_val=0;
    double fd_fps;
#endif

    int i = 0;

    FodetInitial();

    while (IsInState(ST::Running))
    {
#if defined(IPU_TEST_FD_PERFORMANCE_TIME_MEASURE)
        if((m_FodetUser.ui32FodetCount % 100 ) == 0)
        {
            gettimeofday(&fd_tv, NULL);
            fd_val = (fd_tv.tv_sec  * 1000);
            fd_val += (fd_tv.tv_usec / 1000);
            fd_fps = 1000/((double)(fd_val - pre_val) /100);
            pre_val = fd_val;

            FD_INFO("@@@ FD(%d) : %f fps\n", m_FodetUser.ui32FodetCount, fd_fps);
        }
#endif
        try
        {
            i++;
            if(i%frame_idt != 0){
                m_BufIn = m_pPortIn->GetBuffer(&m_BufIn);
                m_pPortIn->PutBuffer(&m_BufIn);
                if(i >= 100000)
                    i = 0;
                continue;
            }
            FodetClearFRBuffer(m_pIntegralImageBuf);
            FodetClearFRBuffer(m_pHidCascadeBuf);

            m_BufIn = m_pPortIn->GetBuffer(&m_BufIn);
#if defined(IPU_SOURCE_IMG_FROM_JSON_FILE)
            m_ImgBuf = m_pSrcImgBuf[0]->GetBuffer();
#endif
            m_IIsumBuf = m_pIntegralImageBuf->GetBuffer();
            m_DbBuf = m_pFodDatabaseBuf[0]->GetBuffer();
            m_HidccBuf = m_pHidCascadeBuf->GetBuffer();

            m_FodetUser.ui32IISumAddr = m_IIsumBuf.fr_buf.phys_addr;
            m_FodetUser.ui32DatabaseAddr = m_DbBuf.fr_buf.phys_addr;
            m_FodetUser.ui32HIDCCAddr = m_HidccBuf.fr_buf.phys_addr;
            m_FodetUser.ui32SrcImgAddr = m_BufIn.fr_buf.phys_addr;
#if defined(IPU_SOURCE_IMG_FROM_JSON_FILE)
            m_FodetUser.ui32SrcImgAddr = m_ImgBuf.fr_buf.phys_addr;
#endif

            FodetInitGroupingArray();
            FodetObjectDetect();
            FodetGetCoordinate();

            m_pHidCascadeBuf->PutBuffer(&m_HidccBuf);
            m_pFodDatabaseBuf[0]->PutBuffer(&m_DbBuf);
            m_pIntegralImageBuf->PutBuffer(&m_IIsumBuf);
#if defined(IPU_SOURCE_IMG_FROM_JSON_FILE)
            m_pSrcImgBuf[0]->PutBuffer(&m_ImgBuf);
#endif
            m_pPortIn->PutBuffer(&m_BufIn);

            FodetGrouping();

            if(m_pPortOut->IsEnabled()) {
                m_BufOut = m_pPortOut->GetBuffer();
                m_BufOut.fr_buf.size = sizeof(CvRect) * (m_OutputCoordinate[0].x + 1);
                memcpy(m_BufOut.fr_buf.virt_addr, (void *)m_OutputCoordinate, m_BufOut.fr_buf.size);
                //memcpy(m_BufOut.fr_buf.virt_addr, (void *)m_OutputCoordinate, (sizeof(CvRect) * (m_OutputCoordinate[0].x + 1)));
                m_pPortOut->PutBuffer(&m_BufOut);
            }

#if 1
            // print output coordinate
            //if(m_OutputCoordinate[0].x > 0)
            {
                memset(&e_fd, 0, sizeof(e_fd));
                e_fd.num = m_OutputCoordinate[0].x;
                FD_INFO("==== Print OutputCoordinate ==========\n");
                FD_INFO("We found (%d) coordinate\n", m_OutputCoordinate[0].x);

                for(int i=1; i<=m_OutputCoordinate[0].x; i++){
                    FD_INFO("(%d)-(%d,%d,%d,%d)\n", i,
                                m_OutputCoordinate[i].x,
                                m_OutputCoordinate[i].y,
                                m_OutputCoordinate[i].w,
                                m_OutputCoordinate[i].h);
                    e_fd.x[i-1] = m_OutputCoordinate[i].x;
                    e_fd.y[i-1] = m_OutputCoordinate[i].y;
                    e_fd.w[i-1] = m_OutputCoordinate[i].w;
                    e_fd.h[i-1] = m_OutputCoordinate[i].h;
                }
                event_send(EVENT_FODET, (char *)&e_fd, sizeof(e_fd));
            }
#endif
        }catch(const char* err){
            usleep(500000);
        }
    }
}


IPU_FODETV2::IPU_FODETV2(std::string name, Json *js)
{
    int i;
    int Value;
    char *pszString;
    char szItemName[80];

    m_pJsonMain = js;
    Name = name;//"fodet";
#if defined(IPU_SOURCE_IMG_FROM_JSON_FILE)
    for (i = 0; i < IPU_FODET_SRC_IMG_MAX; i++)
    {
        m_pszSrcImgFileName[i] = NULL;
        m_pSrcImgBuf[i] = NULL;
    }
#endif

    for (i = 0; i < IPU_FODET_DATABASE_MAX; i++)
    {
        m_pszFodDatabaseFileName[i] = NULL;
        m_pFodDatabaseBuf[i] = NULL;
    }
    m_pIntegralImageBuf = NULL;
    m_pHidCascadeBuf = NULL;

    FodetDefaultParameter();
    FodetInitGroupingArray();

    //----------------------------------------------------------------------------------------------
    // Parse IPU parameter
    if (m_pJsonMain != NULL)
    {
#if defined(IPU_SOURCE_IMG_FROM_JSON_FILE)
        // Parse source image parameter
        for (i = 0; i < IPU_FODET_SRC_IMG_MAX; i++)
        {
            sprintf(szItemName, "fd_img_file_name%d", i+1);
            if (FodetJsonGetString(szItemName, pszString))
                m_pszSrcImgFileName[i] = strdup(pszString);
        }
#endif
        if (FodetJsonGetInt("src_img_wd", Value))
            m_FodetUser.ui16ImgWidth = Value;
        if (FodetJsonGetInt("src_img_ht", Value))
            m_FodetUser.ui16ImgHeight = Value;
        if (FodetJsonGetInt("src_img_st", Value))
            m_FodetUser.ui16ImgStride = Value;
        if (FodetJsonGetInt("src_img_ost", Value))
            m_FodetUser.ui32ImgOffset = Value;
        if (FodetJsonGetInt("src_img_fmt", Value))
            m_FodetUser.ui16ImgFormat = Value;

        if (FodetJsonGetInt("crop_x_ost", Value))
            m_FodetUser.ui16CropXost= Value;
        if (FodetJsonGetInt("crop_y_ost", Value))
            m_FodetUser.ui16CropYost = Value;
        if (FodetJsonGetInt("crop_wd", Value))
            m_FodetUser.ui16CropWidth= Value;
        if (FodetJsonGetInt("crop_ht", Value))
            m_FodetUser.ui16CropHeight = Value;

        if (FodetJsonGetInt("iiout_offset", Value))
            m_FodetUser.ui16IIoutOffset= Value;
        if (FodetJsonGetInt("iiout_stride", Value))
            m_FodetUser.ui16IIoutStride= Value;

        // Parse database parameter
        for (i = 0; i < IPU_FODET_DATABASE_MAX; i++)
        {
            sprintf(szItemName, "fd_db_file_name%d", i+1);
            if (FodetJsonGetString(szItemName, pszString))
                m_pszFodDatabaseFileName[i] = strdup(pszString);
         }

        if (FodetJsonGetInt("idx_start", Value))
            m_FodetUser.ui16StartIndex= Value;
        if (FodetJsonGetInt("idx_end", Value))
            m_FodetUser.ui16EndIndex = Value;
        if (FodetJsonGetInt("scan_win_wd", Value))
            m_FodetUser.ui16EqWinWd= Value;
        if (FodetJsonGetInt("scan_win_ht", Value))
            m_FodetUser.ui16EqWinHt = Value;

        if (FodetJsonGetInt("fd_use_cache", Value))
            m_FodetUser.ui8CacheFlag = Value;
        if (FodetJsonGetInt("frame_idt", Value))
            frame_idt = Value;
    }

    //----------------------------------------------------------------------------------------------
    // Create FODET support port
    m_pPortIn = CreatePort("in", Port::In);
    m_pPortOut = CreatePort("ORout", Port::Out);

    m_pPortOut->SetBufferType(FRBuffer::Type::FIXED, 1);
    m_pPortOut->SetResolution(16, 200);
    m_pPortOut->SetPixelFormat(Pixel::Format::FODET);
    m_pPortOut->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_Dynamic);

    fodet_open();

    m_pJsonMain = NULL;
}

IPU_FODETV2::~IPU_FODETV2()
{
    int i;
#if defined(IPU_SOURCE_IMG_FROM_JSON_FILE)
    for (i = 0; i < IPU_FODET_SRC_IMG_MAX; i++)
    {
        if (m_pszSrcImgFileName[i] != NULL)
            free(m_pszSrcImgFileName[i]);
        if (m_pSrcImgBuf[i] != NULL)
            delete m_pSrcImgBuf[i];
    }
#endif

    for (i = 0; i < IPU_FODET_DATABASE_MAX; i++)
    {
        if (m_pszFodDatabaseFileName[i] != NULL)
            free(m_pszFodDatabaseFileName[i]);
        if (m_pFodDatabaseBuf[i] != NULL)
            delete m_pFodDatabaseBuf[i];
    }

    if (m_pIntegralImageBuf != NULL)
        delete m_pIntegralImageBuf;

    if (m_pHidCascadeBuf != NULL)
        delete m_pHidCascadeBuf;

    fodet_close();
}

void IPU_FODETV2::Prepare()
{
    bool ret;
    Buffer buf;
    Pixel::Format enPixelFormat = m_pPortIn->GetPixelFormat();

    if(enPixelFormat != Pixel::Format::NV12 && enPixelFormat != Pixel::Format::NV21)
    {
        LOGE("In Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }
    enPixelFormat = m_pPortOut->GetPixelFormat();
    if(enPixelFormat != Pixel::Format::FODET)
    {
        LOGE("Out Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }

#if defined(IPU_SOURCE_IMG_FROM_JSON_FILE)
    //  1. prepare source image.
    if(m_pszSrcImgFileName[0] == NULL){
        ret = FodetLoadBinaryFile("SourceImgBuf", &m_pSrcImgBuf[0], "/root/.fodet/fodet_img_lena_123x92x640.bin");  // q3
        if(ret == false)
            LOGE(" == ERROR == : load source image binary fail!\n");
        LOGE("====== Load default src img binary file.====\n");
    }else{
        ret = FodetLoadBinaryFile("SourceImgBuf", &m_pSrcImgBuf[0], m_pszSrcImgFileName[0]);
        if(ret == false)
            LOGE(" == ERROR == : load [%s] binary fail!\n", m_pszSrcImgFileName[0]);
        LOGE("====== Load src img binary file by Json.====\n[%s]\n", m_pszSrcImgFileName[0]);
    }
    buf = m_pSrcImgBuf[0]->GetBuffer();
    m_FodetUser.ui32SrcImgAddr = buf.fr_buf.phys_addr;
    m_FodetUser.ui32SrcImgLength = buf.fr_buf.size;
    m_pSrcImgBuf[0]->PutBuffer(&buf);
#endif

    //  2. prepare integral image buffer.
    m_pIntegralImageBuf = new FRBuffer("IntegralImgBuf", FRBuffer::Type::FIXED, 1, FOD_INTGRL_IMG_OUT_SIZE);
    if (m_pIntegralImageBuf ==  NULL){
        LOGE("failed to allocate integral image buffer.\n");
        return;
    }
    buf = m_pIntegralImageBuf->GetBuffer();
    m_FodetUser.ui32IISumAddr = buf.fr_buf.phys_addr;
    m_FodetUser.ui32IISumLength = buf.fr_buf.size;
    m_pIntegralImageBuf->PutBuffer(&buf);

    //  3. prepare database for classifier.
    if(m_pszFodDatabaseFileName[0] == NULL){
        ret = FodetLoadBinaryFile("DatabaseBuf", &m_pFodDatabaseBuf[0], "/root/.fodet/fodet_db_haar.bin");
        if(ret == false)
            LOGE(" == ERROR == : load [/root/.fodet/fodet_db_haar.bin] binary fail!\n");
        FD_LOG("===== Load default database ====\n");
    }else{
        ret = FodetLoadBinaryFile("DatabaseBuf", &m_pFodDatabaseBuf[0], m_pszFodDatabaseFileName[0]);
        if(ret == false)
            LOGE(" == ERROR == : load [%s] binary fail!\n", m_pszFodDatabaseFileName[0]);
        FD_LOG("====== Load database by Json.====\n[%s]\n", m_pszFodDatabaseFileName[0]);
    }
    buf = m_pFodDatabaseBuf[0]->GetBuffer();
    m_FodetUser.ui32DatabaseAddr = buf.fr_buf.phys_addr;
    m_FodetUser.ui32DatabaseLength = buf.fr_buf.size;
    m_pFodDatabaseBuf[0]->PutBuffer(&buf);  //	// memo address

    //  4. prepare Hid-cascade buffer.
    m_pHidCascadeBuf = new FRBuffer("HidCascadBuf", FRBuffer::Type::FIXED, 1, FOD_CLSFR_HID_CSCD_OUT_SIZE);
    if (m_pHidCascadeBuf ==  NULL){
        LOGE("failed to allocate hid-cascad buffer.\n");
        return;
    }
    buf = m_pHidCascadeBuf->GetBuffer();
    m_FodetUser.ui32HIDCCAddr= buf.fr_buf.phys_addr;
    m_FodetUser.ui32HIDCCLength = buf.fr_buf.size;
    m_pHidCascadeBuf->PutBuffer(&buf);
}

void IPU_FODETV2::Unprepare()
{
    Port *pIpuPort = NULL;;

    pIpuPort = GetPort("in");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    pIpuPort = GetPort("ORout");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
}

int IPU_FODETV2::UnitControl(std::string func, void *arg)
{
    return 0;
}

bool IPU_FODETV2::FodetJsonGetInt(const char* pszItem, int& Value)
{
    Value = 0;

    if (m_pJsonMain == NULL)
        return false;

    Json* pSub = m_pJsonMain->GetObject(pszItem);
    if (pSub == NULL)
        return false;

    Value = pSub->valueint;
    FD_LOG("GetInt: %s = %d\n", pszItem, Value);
    return true;
}

bool IPU_FODETV2::FodetJsonGetString(const char* pszItem, char*& pszString)
{
    pszString = NULL;

    if (m_pJsonMain == NULL)
        return false;

    Json* pSub = m_pJsonMain->GetObject(pszItem);
    if (pSub == NULL)
        return false;

    pszString = pSub->valuestring;
    FD_LOG("FodetJsonGetStr: %s = %s\n", pszItem, pszString);
    return true;
}

void IPU_FODETV2::FodetLog(const char* format, ...)
{
    char szMessage[256];
    va_list args;
    va_start(args, format);
    vsprintf(szMessage, format, args);
    va_end(args);

    printf("@%s@ %s", Name.c_str(), szMessage);

    fflush(stdout);
}

FODET_UINT32 IPU_FODETV2::FodetGetFileSize(FILE* pFileID)
{
    FODET_UINT32 pos = ftell(pFileID);
    FODET_UINT32 len = 0;

    fseek(pFileID, 0L, SEEK_END);
    len = ftell(pFileID);
    fseek(pFileID, pos, SEEK_SET);

    return len;
}

bool IPU_FODETV2::FodetLoadBinaryFile(const char* pszBufName, FRBuffer** ppBuffer, const char* pszFilenane)
{
    FODET_UINT32 nLoaded = 0;
    FILE* f = NULL;
    Buffer buf;

    if (*ppBuffer != NULL)
    {
        delete (*ppBuffer);
        *ppBuffer = NULL;
    }

    if ( (f = fopen(pszFilenane, "rb")) == NULL )
    {
        LOGE("failed to open %s!\n", pszFilenane);
        return false;
    }

    nLoaded = FodetGetFileSize(f);

    *ppBuffer = new FRBuffer(pszBufName, FRBuffer::Type::FIXED, 1, nLoaded);
    if (*ppBuffer ==  NULL)
    {
        fclose(f);
        LOGE("failed to allocate buffer.\n");
        return false;
    }

    buf = (*ppBuffer)->GetBuffer();
    fread(buf.fr_buf.virt_addr, nLoaded, 1, f);
    (*ppBuffer)->PutBuffer(&buf);

    fclose(f);

    FD_LOG("success load file %s with %d size.\n", pszFilenane, nLoaded);

    return true;
}


void IPU_FODETV2::FodetCalcWinParameter(void)
{
    int EqWinWd, EqWinHt;
    double baseSF = 1.1f;
    double tempSF = 1.0f;
    int i;

    memset(stOR_Data, 0, sizeof(OR_DATA)*25);
    EqWinWd = m_FodetUser.ui16EqWinWd;
    EqWinHt = m_FodetUser.ui16EqWinHt;

    for(i=0; i<=24; i++)
    {
        stOR_Data[i].scaling_factor = (int)(tempSF * 1024);
        stOR_Data[i].eq_win_wd= (int)(tempSF * EqWinWd);
        stOR_Data[i].eq_win_ht = (int)(tempSF * EqWinHt);
        stOR_Data[i].real_wd= stOR_Data[i].eq_win_wd+2;
        stOR_Data[i].real_ht= stOR_Data[i].eq_win_ht+2;
        if((i <= 15) && (i >= 1))
            stOR_Data[i].step = 4;
        else if(i >= 20)
            stOR_Data[i].step = 10;
        else if((i <= 19) && (i >= 18))
            stOR_Data[i].step = 8;
        else if((i <= 17) && (i >= 16))
            stOR_Data[i].step = 6;
        else
            stOR_Data[i].step = 2;
#if 0
        LOGE("%2.6f --", tempSF);
        LOGE("(%08X, %03d, %03d, %02d, %03d, %03d)\n",
                stOR_Data[i].scaling_factor,
                stOR_Data[i].eq_win_wd,
                stOR_Data[i].eq_win_ht,
                stOR_Data[i].step,
                stOR_Data[i].real_wd,
                stOR_Data[i].real_ht);
#endif
        tempSF *= baseSF;
    }
}


void IPU_FODETV2::FodetCalcPlanarAddress(int w, int h, FODET_UINT32& Yaddr, FODET_UINT32& UVaddr)
{
    UVaddr = Yaddr + (w * h);
}

void IPU_FODETV2::FodetCalcOffsetAddress(int x, int y, int stride, FODET_UINT32& Yaddr, FODET_UINT32& UVaddr)
{
    Yaddr += ((y * stride) + x);
    UVaddr += (((y / 2) * stride) + x);
}

void IPU_FODETV2::FodetClearFRBuffer(FRBuffer* fr_buf)
{
    Buffer buf;

    buf = fr_buf->GetBuffer();
    memset(buf.fr_buf.virt_addr, 0, buf.fr_buf.size);
    fr_buf->PutBuffer(&buf);
}
void IPU_FODETV2::FodetDumpUserInfo(void)
{

    FD_LOG("\n\n====FodetDumpUserInfo==========================\n");

    FD_LOG("FODET_INFO_CTRL: [0x%04X]\n", m_FodetUser.stInfoCtrl.ui32Ctrl);

    FD_LOG("\nSource Image info : \n");
    FD_LOG("Wd, Ht, St: (%d, %d, %d)\n", m_FodetUser.ui16ImgWidth, m_FodetUser.ui16ImgHeight, m_FodetUser.ui16ImgStride);
    FD_LOG("Offset, format: (%d, %d)\n", m_FodetUser.ui32ImgOffset, m_FodetUser.ui16ImgFormat);

    FD_LOG("\nCrop Image info : \n");
    FD_LOG("Crop Xost, Yost: (%d, %d)\n", m_FodetUser.ui16CropXost, m_FodetUser.ui16CropYost);
    FD_LOG("Crop Wd, Ht: (%d, %d)\n", m_FodetUser.ui16CropWidth, m_FodetUser.ui16CropHeight);

    FD_LOG("\nBuffer address info : \n");
    FD_LOG("Source Img: [0x%08X](%d)\n", m_FodetUser.ui32SrcImgAddr, m_FodetUser.ui32SrcImgLength);
    FD_LOG("II sum: [0x%08X](%d)\n", m_FodetUser.ui32IISumAddr, m_FodetUser.ui32IISumLength);
    FD_LOG("Database: [0x%08X](%d)\n", m_FodetUser.ui32DatabaseAddr, m_FodetUser.ui32DatabaseLength);
    FD_LOG("Hid-cascade: [0x%08X](%d)\n", m_FodetUser.ui32HIDCCAddr, m_FodetUser.ui32HIDCCLength);

    FD_LOG("\nIntegral image output info : \n");
    FD_LOG("II out Offset: (%d)\n", m_FodetUser.ui16IIoutOffset);
    FD_LOG("II out Stride: (%d)\n", m_FodetUser.ui16IIoutStride);

    FD_LOG("\nScan window info : \n");
    FD_LOG("Idx Start-End: (%d - %d)\n", m_FodetUser.ui16StartIndex, m_FodetUser.ui16EndIndex);
    FD_LOG("SF, Shift: [0x%08X], (%d)\n", m_FodetUser.ui16ScaleFactor, m_FodetUser.ui16ShiftStep);
    FD_LOG("Eq Win Wd, Ht: (%d, %d)\n", m_FodetUser.ui16EqWinWd, m_FodetUser.ui16EqWinHt);
    FD_LOG("Stop Win Wd, Ht: (%d, %d)\n", m_FodetUser.ui16StopWd, m_FodetUser.ui16StopHt);
    FD_LOG("RWA :[0x%08X]\n", m_FodetUser.ui32RWA);

    FD_LOG("\nCache address info : \n");
    FD_LOG("Cache Flag : Turn %s Cache.\n", m_FodetUser.ui8CacheFlag? "On":"Off");
    FD_LOG("II-Cache Addr :[0x%08X]\n", m_FodetUser.ui32IICacheAddr);
    FD_LOG("HID-Cache Addr :[0x%08X]\n", m_FodetUser.ui32HIDCacheAddr);

    FD_LOG("CoordinateNum: (%d)\n", m_FodetUser.ui16CoordinateNum);
    FD_LOG("Coordinate Addr :[0x%08X]\n", m_FodetUser.ui32CoordinateAddr);
    FD_LOG("OR-Data Addr :[0x%08X]\n", m_FodetUser.ui32ORDataAddr);

    FD_LOG("==============================\n\n");
}


void IPU_FODETV2::FodetDefaultParameter(void)
{
    memset(&m_FodetUser, 0, sizeof(FODET_USER));

    m_FodetUser.stInfoCtrl.ui32Ctrl = 0;

    // source image info
    m_FodetUser.ui16ImgWidth = FOD_SRC_IMG_WIDTH;
    m_FodetUser.ui16ImgHeight = FOD_SRC_IMG_HEIGHT;
    m_FodetUser.ui16ImgStride = FOD_SRC_IMG_STRIDE;
    m_FodetUser.ui32ImgOffset = 0;
    m_FodetUser.ui16ImgFormat = 1;    // YUV422 = 0, YUV411 = 1 .

    // Crop Image info
    m_FodetUser.ui16CropXost = 0;
    m_FodetUser.ui16CropYost = 0;
    m_FodetUser.ui16CropWidth = FOD_SRC_IMG_WIDTH;
    m_FodetUser.ui16CropHeight = FOD_SRC_IMG_HEIGHT;

    // II,  integral image
    m_FodetUser.ui32SrcImgAddr = 0;
    m_FodetUser.ui32SrcImgLength = 0;
    m_FodetUser.ui32IISumAddr = 0;
    m_FodetUser.ui32IISumLength = 0;

    // Integral image output info
    m_FodetUser.ui16IIoutOffset = FOD_INTGRL_IMG_OUT_OFFSET;
    m_FodetUser.ui16IIoutStride = FOD_INTGRL_IMG_OUT_STRIDE;

    // RR, renew run
    m_FodetUser.ui32DatabaseAddr = 0;
    m_FodetUser.ui32DatabaseLength = 0;
    m_FodetUser.ui32HIDCCAddr = 0;
    m_FodetUser.ui32HIDCCLength = 0;

    // scan win
    m_FodetUser.ui16StartIndex = FOD_TOTAL_IDX_START;
    m_FodetUser.ui16EndIndex = FOD_TOTAL_IDX_END;
    m_FodetUser.ui16ScaleFactor = 0xFFFF;
    m_FodetUser.ui16EqWinWd = 0;
    m_FodetUser.ui16EqWinHt = 0;
    m_FodetUser.ui16ShiftStep = 0;
    m_FodetUser.ui16StopWd = 0;
    m_FodetUser.ui16StopHt = 0;
    m_FodetUser.ui32RWA = 0xFFFFFFFF;

    // cache
    m_FodetUser.ui8CacheFlag = 0;
    m_FodetUser.ui32IICacheAddr = 0;
    m_FodetUser.ui32HIDCacheAddr = 0;

    // coordinate
    m_FodetUser.ui16CoordinateNum = 0;
    m_FodetUser.ui32CoordinateAddr = 0;
    m_FodetUser.ui32ORDataAddr = 0;

    frame_idt = 5;
}



void IPU_FODETV2::FodetInitGroupingArray(void)
{
    m_i32FilterIndex = 0;
    memset(m_avgComps1, 0, sizeof(CvAvgComp)*MAX_ARY_SCAN_ALL);
    memset(m_avgComps2, 0, sizeof(CvAvgComp)*MAX_ARY_SCAN_ALL);
    memset(m_treeNode, 0, sizeof(CvPTreeNode)*MAX_ARY_SCAN_ALL);
    memset(m_OutputCoordinate, 0, sizeof(CvRect)*MAX_ARY_SCAN_ALL);
}


bool IPU_FODETV2::FodetGetCoordinate(void)
{
    int ret = 0;
    int temp =0;

    // Get coordinate
    m_FodetUser.ui16CoordinateNum = 0xa5a5;
    m_FodetUser.ui32CoordinateAddr = (FODET_UINT32)m_avgComps1;
    ret = fodet_get_coordinate(&m_FodetUser);

    if(m_FodetUser.ui16CoordinateNum == 0xa5a5){
        FD_LOG("Warning : No coordinate\n");
        ret = -1;
    }else{
        for(temp=0; temp < m_FodetUser.ui16CoordinateNum; temp++)
        {
            FD_LOG("Rect(%03d/%d)=(%d,%d,%d,%d)\n", temp, m_FodetUser.ui16CoordinateNum,
            m_avgComps1[temp].rect.x, m_avgComps1[temp].rect.y,
            m_avgComps1[temp].rect.w, m_avgComps1[temp].rect.h);
            m_treeNode[temp].pParent = 0;
            m_treeNode[temp].pElement = (char*)&(m_avgComps1[temp].rect);
            m_treeNode[temp].iRank = 0;
            m_treeNode[temp].iIdxClass = -1;
        }
    }

    return (ret >= 0) ? true : false;
}

int IPU_FODETV2::cvRound(double value)
{
    int i = (int)(value + 0.5f);
    return i;
}
int IPU_FODETV2::is_equal(const void* _r1, const void* _r2)
{
    const CvRect* r1= (const CvRect*)_r1;
    const CvRect* r2= (const CvRect*)_r2;
    int iDistance = cvRound(r1->w * 0.2f);

    return r2->x <= r1->x + iDistance && r2->x >= r1->x - iDistance
        && r2->y <= r1->y + iDistance && r2->y >= r1->y - iDistance
        && r2->w <= cvRound(r1->w * 1.2f)
        && cvRound(r2->w * 1.2f) >= r1->w;
}

bool IPU_FODETV2::FodetGrouping(void)
{
    int32_t ret=0;
    int iCandidateCount=0;
    int iMinNeighbors = 2;
    int iIdxClass = 0;
    int iIdxbounding = 0;
    int iIdxFilter = 0;
    int iDistance;
    int i,j,iFlag;

    iCandidateCount = m_FodetUser.ui16CoordinateNum;
    if((iCandidateCount <=1) || (iCandidateCount > MAX_ARY_SCAN_ALL)){
        FD_LOG("(%X)not Grouping\n",iCandidateCount);
        m_i32FilterIndex=0;
        return FODET_TRUE;
    }

    //==== Start Grouping ...=============================================================================
    FD_LOG("\n\n<J>Start Grouping ...\n");
    FD_LOG("Group iCandidateCount=(%d)\n", iCandidateCount);
    // group retrieved rectangles in order to filter out noise
    for (i = 0; i < iCandidateCount; i++)
    {
        CvPTreeNode* node = (CvPTreeNode*)&m_treeNode[i];
        CvPTreeNode* root = node;
        if (!node->pElement)
            continue;

        // find root
        while (root->pParent)
        	root = root->pParent;

        for (j = 0; j < iCandidateCount; j++)
        {
            CvPTreeNode* node2 = (CvPTreeNode*)&m_treeNode[j];
            if (node2->pElement && node2 != node && is_equal(node->pElement, node2->pElement)){
                CvPTreeNode* root2 = node2;

                // unite both trees
                while (root2->pParent)
                    root2 = root2->pParent;

                if (root2 != root){
                    if (root->iRank > root2->iRank)
                        root2->pParent = root;
                    else{
                        root->pParent = root2;
                        root2->iRank += root->iRank == root2->iRank;
                        root = root2;
                    }

                    // compress path from node2 to the root
                    while (node2->pParent)
                    {
                        CvPTreeNode* temp = node2;
                        node2 = node2->pParent;
                        temp->pParent = root;
                    }

                    // compress path from node to the root
                    node2 = node;
                    while (node2->pParent)
                    {
                        CvPTreeNode* temp = node2;
                        node2 = node2->pParent;
                        temp->pParent = root;
                    }
                }
            }
        }
    }

    for (i = 0; i < iCandidateCount; i++)
    {
        CvPTreeNode* node = (CvPTreeNode*)&m_treeNode[i];
        if (node->pElement)
        {
            while (node->pParent)
            node = node->pParent;
            if (node->iRank >= 0)
                node->iRank = ~iIdxClass++;
            m_treeNode[i].iIdxClass = ~node->iRank;
        }
    }

    // count number of neighbors
    for (i = 0; i < iCandidateCount; i++)
    {
        j = m_treeNode[i].iIdxClass;
        m_avgComps2[j].neighbors++;
        m_avgComps2[j].rect.x += m_avgComps1[i].rect.x;
        m_avgComps2[j].rect.y += m_avgComps1[i].rect.y;
        m_avgComps2[j].rect.w += m_avgComps1[i].rect.w;
        m_avgComps2[j].rect.h += m_avgComps1[i].rect.h;
    }

    // calculate average bounding box
    for (i = 0; i < iIdxClass; i++)
    {
        j = m_avgComps2[i].neighbors;
        if (j >= iMinNeighbors)
        {
            m_avgComps1[iIdxbounding].rect.x = (m_avgComps2[i].rect.x*2 + j) / (2*j);
            m_avgComps1[iIdxbounding].rect.y = (m_avgComps2[i].rect.y*2 + j) / (2*j);
            m_avgComps1[iIdxbounding].rect.w = (m_avgComps2[i].rect.w*2 + j) / (2*j);
            m_avgComps1[iIdxbounding].rect.h = (m_avgComps2[i].rect.h*2 + j) / (2*j);
            m_avgComps1[iIdxbounding].neighbors = m_avgComps2[i].neighbors;
            iIdxbounding++;
        }
    }
    FD_LOG("Grouping iIdxbounding=(%d)\n", iIdxbounding);

    // filter out small face rectangles inside large face rectangles
    for (i = 0; i < iIdxbounding; i++)
    {
        iFlag = 1;
        for (j = 0; j < iIdxbounding; j++)
        {
            if ((((float)m_avgComps1[j].rect.w / (float)m_avgComps1[i].rect.w) +0.5) >= 2)
                iDistance = cvRound(m_avgComps1[j].rect.w * 0.7);
            else if ((((float)m_avgComps1[i].rect.w / (float)m_avgComps1[j].rect.w) +0.5) >= 2)
                iDistance = cvRound(m_avgComps1[i].rect.w * 0.7);
            else
                iDistance = cvRound(m_avgComps1[j].rect.w * 0.2);
            if (i != j &&
                m_avgComps1[i].rect.x >= m_avgComps1[j].rect.x - iDistance &&
                m_avgComps1[i].rect.y >= m_avgComps1[j].rect.y - iDistance &&
                m_avgComps1[i].rect.x + m_avgComps1[i].rect.w <= m_avgComps1[j].rect.x + m_avgComps1[j].rect.w + iDistance &&
                m_avgComps1[i].rect.y + m_avgComps1[i].rect.h <= m_avgComps1[j].rect.y + m_avgComps1[j].rect.h + iDistance &&
                //(m_avgComps1[j].neighbors > MAX( 3, m_avgComps1[i].neighbors ) || m_avgComps1[i].neighbors < 3) )
                (m_avgComps1[j].neighbors > 2 || m_avgComps1[i].neighbors < 2))
            {
                iFlag = 0;
                break;
            }
        }

        if (iFlag)
        {
            //m_avgComps2[iIdxFilter].rect.x = m_avgComps1[i].rect.x;
            //m_avgComps2[iIdxFilter].rect.y = m_avgComps1[i].rect.y;
            //m_avgComps2[iIdxFilter].rect.w = m_avgComps1[i].rect.w;
            //m_avgComps2[iIdxFilter].rect.h = m_avgComps1[i].rect.h;
            //m_avgComps2[iIdxFilter].neighbors = m_avgComps1[i].neighbors;
            m_OutputCoordinate[iIdxFilter+1].x = m_avgComps1[i].rect.x;
            m_OutputCoordinate[iIdxFilter+1].y = m_avgComps1[i].rect.y;
            m_OutputCoordinate[iIdxFilter+1].w = m_avgComps1[i].rect.w;
            m_OutputCoordinate[iIdxFilter+1].h = m_avgComps1[i].rect.h;

            //==================================================================
            //======================================================================
            FD_LOG("\n<J%d> x=%d, y=%d, w=%d, h=%d, n=%d\n", iIdxFilter,
                        m_avgComps2[iIdxFilter].rect.x,
                        m_avgComps2[iIdxFilter].rect.y,
                        m_avgComps2[iIdxFilter].rect.w,
                        m_avgComps2[iIdxFilter].rect.h,
                        m_avgComps2[iIdxFilter].neighbors);
                        iIdxFilter++;
        }
    }

    m_i32FilterIndex = iIdxFilter;
    m_OutputCoordinate[0].x = iIdxFilter;
    FD_LOG("Comps2 IdxFilter=(%d)\n", iIdxFilter);
    FD_LOG("<J>End Grouping !!!\n\n");

    return (ret >= 0) ? true : false;
}


bool IPU_FODETV2::FodetInitial(void)
{
    int ret;

    // IN ----------------------------------------
    if (m_pPortIn->IsEnabled())
    {
        if((m_FodetUser.ui16ImgWidth > m_FodetUser.ui16CropWidth)
            || (m_FodetUser.ui16ImgHeight > m_FodetUser.ui16CropHeight) )
        {
            m_FodetUser.ui16ImgWidth = m_FodetUser.ui16CropWidth;
            m_FodetUser.ui16ImgHeight = m_FodetUser.ui16CropHeight;
            m_FodetUser.ui16ImgStride = m_pPortIn->GetWidth();
            m_FodetUser.ui32ImgOffset = (m_FodetUser.ui16CropYost * m_FodetUser.ui16ImgStride)
                                                           + m_FodetUser.ui16CropXost;
        }else{
            m_FodetUser.ui16ImgWidth = m_pPortIn->GetWidth();
            m_FodetUser.ui16ImgHeight = m_pPortIn->GetHeight();
            m_FodetUser.ui16ImgStride = m_pPortIn->GetWidth();
        }
    }
    //--------------------------------------------
    FodetCalcWinParameter();
    m_FodetUser.ui32ORDataAddr = (FODET_UINT32)stOR_Data;
    m_FodetUser.ui32FodetCount = 0;
    FodetDumpUserInfo();
    ret = fodet_initial(&m_FodetUser);

    return (ret >= 0) ? true : false;
}

bool IPU_FODETV2::FodetObjectDetect(void)
{
    int ret = 0;

#if 0
    if (m_FodetUser.ui32FodetCount == 10){
        WriteFile(m_BufIn.fr_buf.virt_addr, m_BufIn.fr_buf.size, "/nfs/ispost_ss0_fd_in_%d.yuv", m_FodetUser.ui32FodetCount);
    }
    if (m_FodetUser.ui32FodetCount % 10 == 0)
        FodetDumpUserInfo();
#endif

    ret = fodet_object_detect(&m_FodetUser);
    m_FodetUser.ui32FodetCount++;
    return (ret >= 0) ? true : false;
}

void IPU_FODETV2::FodetGetOutput(CvRect* pArray)
{
    pArray = m_OutputCoordinate;
}


