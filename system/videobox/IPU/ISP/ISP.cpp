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

#include <System.h>
#include "ISP.h"

extern "C" {
#include <qsdk/items.h>
}
DYNAMIC_IPU(IPU_ISP, "v2500");
#ifdef COMPILE_ISP_V2505
#define SKIP_FRAME (1)
#elif defined(COMPILE_ISP_V2500)
#define SKIP_FRAME (3)
#endif
#define SETUP_PATH  ("/root/.ispddk/")
#define DEV_PATH    ("/dev/ddk_sensor")
pthread_mutex_t mutex_x= PTHREAD_MUTEX_INITIALIZER;
using namespace std;

enum SHOT_EVENT
{
    SHOT_EVENT_ENQUEUE = 0,
    SHOT_EVENT_STOP,
};

struct shot_event
{
    int event;
    void *arg;
    struct shot_event *next;
};

static struct shot_event* workq =NULL;
static pthread_cond_t qready = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;
static int process_running_status = 0;
extern ST_ISP_Context g_ISP_Camera[MAX_CAM_NB];

static void __EnqueueShotEvent(struct shot_event *ep)
{
    pthread_mutex_lock(&qlock);
    ep->next = workq;
    workq = ep;
    pthread_mutex_unlock(&qlock);
    pthread_cond_signal(&qready);
}

static void EnqueueShotEvent(int event, void* arg)
{
    struct shot_event *ep =
        (struct shot_event *)malloc(sizeof(struct shot_event));
    if (ep && process_running_status)
    {
        LOGD("enquque shot event = %d %s\n", event, (char *)arg);
        ep->event = event;
        ep->arg = arg;
        __EnqueueShotEvent(ep);
    }
    else
    {
        LOGE("shot_event_thread is not running \n");
        free(ep);
    }
}

static void MakeTimeout(struct timespec* tsp, long ms)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    tsp->tv_sec = now.tv_sec;
    tsp->tv_nsec = now.tv_usec * 1000;
    tsp->tv_nsec += ms * 1000 *1000;
}

static void *ProcessShotEvent(void *arg)
{
    uint32_t ret = 0;
    bool blocking = true;

    struct shot_event *ep = NULL;
    struct timespec timeout;

    IPU_ISP* isp = (IPU_ISP*)arg;
    if (!isp)
        return NULL;
    process_running_status = 1;
    printf("[%s] pthread start\n", __func__);

    prctl(PR_SET_NAME,"equeue_shot");/*set thread name*/
    for(;;)
    {
        ep = NULL;
        ret = 0;

        pthread_mutex_lock(&qlock);
        while(workq == NULL)
        {
            if (!blocking)
            {
                MakeTimeout(&timeout, 25); /*now + 25ms*/
                ret = pthread_cond_timedwait(&qready, &qlock, &timeout);
                if (ret == ETIMEDOUT)
                    break;
            }
            else
            {
                (void)timeout;
                /* unlock => blocking --> event => lock */
                ret = pthread_cond_wait(&qready, &qlock);
            }
        }

        if (workq != NULL)
        {
            ep = workq;
            workq = ep->next;
        }

        pthread_mutex_unlock(&qlock);

        if ((ep != NULL&& ep->event == SHOT_EVENT_ENQUEUE) || ret == ETIMEDOUT)
        {
            ret = isp->EnqueueReturnShot();
            if (ep)
                free(ep);
        }
        else if (ep != NULL && ep->event == SHOT_EVENT_STOP)
        {
            printf("[%s] pthread exit\n", __func__);
            free(ep);
            goto thr_exit;
        }
    }

thr_exit:
    process_running_status = 0;
    pthread_exit(0);
    return 0;
}

void CreateShotEventThread(void* isp)
{
    struct sched_param sched;
    pthread_t thread_id;

    if (process_running_status)
        return ;

    pthread_create(&thread_id, NULL, ProcessShotEvent, isp);
    sched.sched_priority = IPU_THREAD_PRIORITY;
    pthread_setschedparam(thread_id, SCHED_RR, &sched);
}

void DestoryShotEventThread(int timeout)
{
    int i;
    EnqueueShotEvent(SHOT_EVENT_STOP, (void*)__func__);
    while (process_running_status != 0 && i < 5)
    {
        usleep(timeout*1000); /*wait for 6ms*/
        i++;
    }
    //TODO: used pthread_join() not usleep
}

void IPU_ISP::WorkLoop()
{
    Buffer BufDst;
    Buffer BufHis;
    Buffer BufHis1;
    Buffer dsc;
    Port *pPortOut = GetPort("out");
    Port *pPortHis = GetPort("his");
    Port *pPortHis1 = GetPort("his1");
    Port *pPortCap = GetPort("cap");
    Port *pdsc = GetPort("dsc");
    int ret;
    IMG_UINT32 Idx = 0, i = 0;
    IMG_UINT32 skip_fr = 0;
    int nCapture =0 ;
    int PreSt = m_RunSt;
    IMG_UINT32 ui32DnTargetIdx = 0;

    int FrameCount = 0;
    IMG_UINT64 timestamp = 0;
    IMG_UINT32 CRCStatus = 0;
    bool hasDE = false;
    bool hasSavedData = false;
#ifdef COMPILE_IQ_REALTIME_SAVE_RAW
    bool hasCapRaw = false;
#endif

    CreateShotEventThread((void *)this);

    for (Idx = 0; Idx < m_iSensorNb; Idx++) {
#ifdef COMPILE_IQ_REALTIME_SAVE_RAW
        IAPI_ISP_SetModuleOUTRawFlag(Idx, false, m_eDataExtractionPoint); //disable raw in normal flow
#endif
        ret = SkipFrame(Idx, ui32SkipFrame);
        if (ret)
        {
            LOGE("(%s:L%d) failed to skip frame !!!\n", __func__, __LINE__);
            throw VBERR_RUNTIME;
            return;
        }
    }

    //int nsize = (p->GetWidth() * p->GetHeight()) * 3 / 2;

    //ofstream file;
    //int cnt = 0;
    //int nsize = (p->GetWidth() * p->GetHeight()) *(3/2);

    //int fd = open(FRING_NODE, O_RDWR);
    //file.open("/mnt/sd0/capture_camera_frame.yuv", ios::out | ios::binary);
    //file2.open("/mnt/sd0/capture_mmaped_camera_frame.yuv", ios::out | ios::binary);
    //prepare enqueue available buffers.
    LOGD("m_iSensorNb:%d,m_nBuffers:%d\n",m_iSensorNb,m_nBuffers);
    for (Idx = 0; Idx < m_iSensorNb; Idx++)
    {
        for (i = 0; i < m_nBuffers; i++)
        {
            IAPI_ISP_EnqueueShot(Idx);
            if (IMG_SUCCESS != ret)
            {
                LOGE("enqueue shot in v2500 ipu thread failed.\n");
                throw VBERR_RUNTIME;
                return ;
            }
        }

#ifdef COMPILE_ISP_V2500
        IAPI_ISP_ConfigAutoControl(Idx);
#endif

    }
    TimePin tp;

    LOGE("start workloop\n");
    while(IsInState(ST::Running)) {
#ifdef COMPILE_IQ_REALTIME_SAVE_RAW
        pthread_mutex_lock(&mutex_x);
        hasCapRaw = m_hasSaveRaw;
        pthread_mutex_unlock(&mutex_x);
#endif
        if (m_RunSt == CAM_RUN_ST_CAPTURING)
        {
            m_iBufUseCnt = 0;
            nCapture ++;
            PreSt = m_RunSt;
            if (nCapture == 1)
            {
                LOGD("in capture mode switch from preview to capture\n");
                for (Idx = 0; Idx < m_iSensorNb; Idx++) {
                    IAPI_ISP_SetCaptureIQFlag(Idx,IMG_TRUE);
                }

                if (pPortCap)
                {
                    m_ImgH = pPortCap->GetHeight();
                    m_ImgW = pPortCap->GetWidth();
                    m_PipH = pPortCap->GetPipHeight();
                    m_PipW = pPortCap->GetPipWidth();
                    m_PipX = pPortCap->GetPipX();
                    m_PipY = pPortCap->GetPipY();
                    m_LockRatio = pPortCap->GetLockRatio();
                }
                else
                {
                    m_RunSt = CAM_RUN_ST_PREVIEWING;
                    nCapture = 0;
                    PreSt = m_RunSt;
                    continue;
                }

                for (Idx = 0; Idx < m_iSensorNb; Idx++)
                {
                    if (0 != m_PipW && 0 != m_PipH)
                    {
                        IAPI_ISP_SetMCIIFCrop(Idx, m_PipW, m_PipH);
                        SetIIFCrop(Idx, m_PipH, m_PipW, m_PipX, m_PipY);
                    }
                    if (CheckOutRes(Idx, pPortOut->GetName(), m_ImgW, m_ImgH) < 0)
                    {
                        LOGE("IPU_ISP output width size is not 64 align\n");
                        throw VBERR_RUNTIME;
                    }

                    if (0 == Idx)
                        Unprepare();

                    IAPI_ISP_SetEncoderDimensions(Idx,m_ImgW,m_ImgH);
                    if (m_ImgW <= m_InW && m_ImgH <= m_InH)
                    {
                        SetEncScaler(Idx, m_ImgW, m_ImgH); //Set output scaler by current port resolution;
                    }
                    else
                    {
                        SetEncScaler(Idx, m_InW, m_InH);
                    }
                    //set display path not work
                    IAPI_ISP_DisableDsc(Idx);
                    pthread_mutex_lock(&mutex_x);
                    if (m_psSaveDataParam)
                    {
                        hasSavedData = true;
                        if (m_psSaveDataParam->fmt &
                            (CAM_DATA_FMT_BAYER_FLX | CAM_DATA_FMT_BAYER_RAW))
                        {
                            IAPI_ISP_SetModuleOUTRawFlag(Idx, true, m_eDataExtractionPoint);
                            IAPI_ISP_SetDisplayDimensions(Idx, m_ImgW, m_ImgH);
                            hasDE = true;
                        }
                    }
                    pthread_mutex_unlock(&mutex_x);
                    IAPI_ISP_SetupModules(Idx);
                    IAPI_ISP_Program(Idx);
                    IAPI_ISP_AllocateBufferPool(Idx,1,BuffersIDs[Idx]);
                    LOGD("end of stop preview in capture mode\n");
                }
            } //End of if (nCapture == 1)

            for (Idx = 0; Idx < m_iSensorNb; Idx++)
            {
                LOGD("in capture running : m_RunSt = %d#######\n", m_RunSt);
                IAPI_ISP_StartCapture(Idx,false);
            }

            do {
                for (Idx = 0; Idx < m_iSensorNb; Idx++)
                {
                    IAPI_ISP_EnqueueShot(Idx);
                    LOGD("@@@@@ "
                        "in after enqueueShot in catpure mode @@@@@@@@@@@@@@\n");
    retry:
                    ret = IAPI_ISP_AcquireShot(Idx,g_ISP_Camera[Idx].capShot,true);
                    if (IMG_SUCCESS != ret) {
                        IAPI_ISP_SensorEnable(Idx,false);
                        usleep(10000);
                        IAPI_ISP_SensorEnable(Idx,true);
                        goto retry;
                    }
                }
                //file.write((char *)frame.YUV.data, nsize);
                //TODO
                if ((skip_fr == SKIP_FRAME ) || nCapture > SKIP_FRAME) {
                    //not continue capture , then put the buffer to next ipu
                    BufDst = pPortCap->GetBuffer();
                    BufDst.fr_buf.phys_addr = (uint32_t)g_ISP_Camera[0].capShot.YUV.phyAddr;
                    pPortCap->PutBuffer(&BufDst);
                    LOGD("put the buffer to the next!\n");
                    if (hasSavedData)
                    {
                        pthread_mutex_lock(&mutex_x);
                        IAPI_ISP_SaveFrame(0,g_ISP_Camera[0].capShot,m_psSaveDataParam);
                        pthread_mutex_unlock(&mutex_x);
                    }
                }

                for (Idx = 0; Idx < m_iSensorNb; Idx++)
                {
                    IAPI_ISP_ReleaseShot(Idx,g_ISP_Camera[Idx].capShot);
                }

                if ( skip_fr == SKIP_FRAME || nCapture > SKIP_FRAME)
                {
                    if (nCapture <= SKIP_FRAME) //not continue capture, skip_fr = 0;
                        skip_fr = 0;
                    LOGD("out from do while===ncapture = %d\n", nCapture);
                    break;
                }
                skip_fr++;
            }while(1);

            for (Idx = 0; Idx < m_iSensorNb; Idx++)
            {
                IAPI_ISP_StopCapture(Idx,false);
            }
            m_RunSt = CAM_RUN_ST_CAPTURED;

            if (hasSavedData) {
                sem_post(&m_semNotify);
            }

            //munmap(temp, nsize);
            LOGD("is in capture mode running: m_RunSt =%d \n", m_RunSt);

        }
        else if (m_RunSt == CAM_RUN_ST_CAPTURED)
        {
            LOGD("Is here running ########## : m_RunSt =%d\n", m_RunSt);
            usleep(16670);
        }
        else if (m_RunSt == CAM_RUN_ST_PREVIEWING)
        {
            //may be blocking until the frame is received
            //this->pCamera->saveParameters(parameters);
            //ISPC::ParameterFileParser::saveGrouped(parameters, "/root/v2505_setupArgs.txt");

            if(PreSt == CAM_RUN_ST_CAPTURING) //if before is capture, then swicth to preview.
            {
                if (hasSavedData)
                {
                    hasSavedData = false;
                    if (hasDE)
                    {
                        for (Idx = 0; Idx < m_iSensorNb; Idx++)
                        {
                            IAPI_ISP_SetModuleOUTRawFlag(Idx, false, m_eDataExtractionPoint);
                        }
                        hasDE = false;
                    }
                }
                for (Idx = 0; Idx < m_iSensorNb; Idx++)
                {
                    PreSt = m_RunSt;
                    skip_fr = 0;
                    nCapture = 0;//recover ncapture to 0, for next preview to capture.
                    LOGD("Is here running from capture switch preview "
                        "##########: runSt =%d, preSt =%d\n", m_RunSt, PreSt);
                    IAPI_ISP_SetCaptureIQFlag(Idx,IMG_FALSE);
                    IAPI_ISP_DeleteShots(Idx);
                    std::list<IMG_UINT32>::iterator it;

                    for (it = BuffersIDs[Idx].begin(); it != BuffersIDs[Idx].end(); it++)
                    {
                        LOGD("getting bufferids = %d\n", *it);
                        if (IMG_SUCCESS != IAPI_ISP_DeregisterBuffer(Idx,*it))
                        {
                            throw VBERR_RUNTIME;
                        }
                    }
                    BuffersIDs[Idx].clear();

                    IAPI_ISP_SetCIPipelineNum(Idx);

                    SetScalerFromPort(Idx);
                    if (0 != m_PipW && 0 != m_PipH)
                    {
                        //IAPI_ISP_SetMCIIFCrop(Idx,m_PipW,m_PipH);
                        SetIIFCrop(Idx, m_PipH, m_PipW, m_PipX, m_PipY);
                    }

                    LOGE("====preview: m_ImgW = %d, "
                        "m_ImgH =%d, m_PipH = %d, m_PipW = %d@@@\n",
                        m_ImgW,  m_ImgH, m_PipH, m_PipW);
                    IAPI_ISP_SetEncoderDimensions(Idx,m_ImgW,m_ImgH);
                    if (pdsc && (m_s32DscImgW != 0) && (m_s32DscImgH != 0))
                    {
                        IAPI_ISP_SetDisplayDimensions(Idx, m_s32DscImgW, m_s32DscImgH);
                    }
                    IAPI_ISP_SetupModules(Idx);
                    IAPI_ISP_Program(Idx);

                    IAPI_ISP_AllocateBufferPool(Idx,m_nBuffers, BuffersIDs[Idx]);
                }

                for (Idx = 0; Idx < m_iSensorNb; Idx++)
                {
                    IAPI_ISP_StartCapture(Idx,false);

                    for (i = 0; i < m_nBuffers; i++)
                    {
                        ret = IAPI_ISP_EnqueueShot(Idx);
                        if (IMG_SUCCESS != ret)
                        {
                            throw VBERR_RUNTIME;
                            return ;
                        }
                    }
                }
                CreateShotEventThread((void*)this);
            }


            BufDst = pPortOut->GetBuffer();
            if (pdsc->IsEnabled())
            {
                dsc = pdsc->GetBuffer();
            }
            int count = 0;
            while((IMG_UINT32)m_iBufUseCnt >= m_nBuffers)
            {
                usleep(2000);
                if (count > 250)
                {
                    ClearAllShot();
                    LOGE("recevie callback timeout, clear all shot!\n");
                    break;
                }
                count++;
            }

            LOGD("(%s:L%d) m_iBufferID:%d m_iBufUseCnt:%d\n",
                __func__, __LINE__, m_iBufferID, m_iBufUseCnt);

            //EnqueueReturnShot();
            // sync buffer from shot to GetCBuffer()
            // BufDst.fr_buf.phys_addr = xxx.YUV.physAddr;

retryCapRaw:
            for (Idx = 0; Idx < m_iSensorNb; Idx++)
            {
                //may be blocking until the frame is received
                ret = IAPI_ISP_AcquireShot(Idx,g_ISP_Camera[Idx].pstShotFrame[m_iBufferID],true);
                if (IMG_SUCCESS != ret)
                {
                    GetPhyStatus(Idx, &CRCStatus);
                    if (CRCStatus != 0) {
                        for (i = 0; i < 5; i++) {
                            IAPI_ISP_SensorReset(Idx);
                            usleep(200000);
                            goto retryCapRaw;
                        }
                        i = 0;
                    }
                    LOGE("failed to get shot %d\n", __LINE__);
                    throw VBERR_RUNTIME;
                }

            }

            //TODO
            for (Idx = 0; Idx < m_iSensorNb; Idx++)
            {
                if (m_iSensorNb > 1)//dual sensor
                {
                    IAPI_ISP_DualSensorUpdate(Idx,
                        g_ISP_Camera[0].pstShotFrame[m_iBufferID],
                        g_ISP_Camera[1].pstShotFrame[m_iBufferID]);
                }
            }


#ifdef COMPILE_IQ_REALTIME_SAVE_RAW
            if (hasCapRaw)
            {
                for (Idx = 0; Idx < m_iSensorNb; Idx++)
                {
                    IAPI_ISP_SetModuleOUTRawFlag(Idx, true, m_eDataExtractionPoint);
                }
                usleep(100 * m_nBuffers * 1000); //for DDR bandwidth, next IPU can not get buffer to work
                if(IAPI_ISP_IsShotFrameHasData(0,m_iBufferID))
                {
                    pthread_mutex_lock(&mutex_x);
                    IAPI_ISP_SaveFrame(0,
                        g_ISP_Camera[0].pstShotFrame[m_iBufferID],
                        m_psSaveDataParam);
                    m_hasSaveRaw = 0;
                    hasCapRaw = 0;
                    pthread_mutex_unlock(&mutex_x);
                    for (Idx = 0; Idx < m_iSensorNb; Idx++)
                    {
                        IAPI_ISP_SetModuleOUTRawFlag(Idx, false, m_eDataExtractionPoint);
                    }
                }
                for (Idx = 0; Idx < m_iSensorNb; Idx++)
                {
                    IAPI_ISP_ReleaseShot(Idx,g_ISP_Camera[Idx].pstShotFrame[m_iBufferID]);
                    IAPI_ISP_EnqueueShot(Idx);
                }

                goto retryCapRaw;
            }
#endif

            timestamp =
                g_ISP_Camera[0].pstShotFrame[m_iBufferID].metadata.timestamps.ui64CurSystemTS;
            BufDst.Stamp(timestamp);
            BufDst.fr_buf.phys_addr =
                (uint32_t)g_ISP_Camera[0].pstShotFrame[m_iBufferID].YUV.phyAddr;
#if 0
if (cnt++ < 8) {
            for (int i = 0; i < 1; ++i) {
                char filename[128];
                int  nsize = 1920*2176*3/2;

                sprintf(filename, "/nfs/camera%d_%dframe%d.yuv", i, cnt, m_iBufferID);
                file.open(filename, ios::out | ios::binary);
                file.write((char *)m_Cam[i].pstShotFrame[m_iBufferID].YUV.data, nsize);
                file.close();
            }
}
#endif

            pthread_mutex_lock(&mutex_x);
            m_pstPrvData[m_iBufferID].iBufID = m_iBufferID;
            m_pstPrvData[m_iBufferID].state = SHOT_POSTED;
            if ((pPortOut->IsEnabled()) && (pPortOut->GetEnableCount() >= 2))
            {
                m_pstPrvData[m_iBufferID].s32EnEsc = 0x01;
                BufDst.fr_buf.stAttr.s32Width = 0x00;
            }
            else
            {
                m_pstPrvData[m_iBufferID].s32EnEsc = 0;
            }

            m_postShots.push_back((void *)&m_pstPrvData[m_iBufferID]);
            pthread_mutex_unlock(&mutex_x);
#if 0
            for(i = 0; i < 3; i++) {
                    struct ipu_v2500_dn_indicator dn_ind_0[] = {
                        { gain: 4.0, dn_id: 0 },
                        { gain: 8.0, dn_id: 1 },
                        { gain: -1, dn_id: 2 },
                    }, dn_ind_1[] = {
                        { gain: 4.0, dn_id: 0 },
                        { gain: 8.0, dn_id: 5 },
                        { gain: -1, dn_id: 6 },
                    };
                struct ipu_v2500_dn_indicator *pdi = m_Cam[0].Scenario ? dn_ind_1 : dn_ind_0;

                if (m_Cam[0].pAE->getNewGain() <= pdi[i].gain || pdi[i].gain == -1)
                {
                    m_pstPrvData[m_iBufferID].iDnID = pdi[i].dn_id;
                    LOGE("Cam: %d, gain: %lf, Scenario: %d ==> set DnCurveID: %d\n", 0,
                            m_Cam[0].pAE->getNewGain(), m_Cam[0].Scenario, pdi[i].dn_id);
                    break;
                }
            }
#endif

            BufDst.fr_buf.priv = (int)&m_pstPrvData[m_iBufferID];
            if ((NULL != pdsc) && (pdsc->IsEnabled()))
            {
                dsc.Stamp(timestamp);
#ifdef COMPILE_ISP_V2505
                dsc.fr_buf.phys_addr = (uint32_t)g_ISP_Camera[0].pstShotFrame[m_iBufferID].DISPLAY.phyAddr;
                dsc.fr_buf.virt_addr = (void *)g_ISP_Camera[0].pstShotFrame[m_iBufferID].DISPLAY.data;
#else
                dsc.fr_buf.phys_addr = (uint32_t)g_ISP_Camera[0].pstShotFrame[m_iBufferID].RGB.phyAddr;
                dsc.fr_buf.virt_addr = (void *)g_ISP_Camera[0].pstShotFrame[m_iBufferID].RGB.data;
#endif
                dsc.fr_buf.priv = (int)&m_pstPrvData[m_iBufferID];
                if ((pPortOut->IsEnabled()) && (pPortOut->GetEnableCount() >= 2))
                {
                    dsc.fr_buf.stAttr.s32Width = 0xFF;
                }
                else
                {
                    dsc.fr_buf.stAttr.s32Width = 0x00;
                }
            }
#ifdef COMPILE_ISP_V2505
            for (Idx = 0; Idx < m_iSensorNb; Idx++)
            {
                IAPI_ISP_GetMirror(Idx,m_pstPrvData[m_iBufferID].mirror);
            }
#endif
            for (Idx = 0; Idx < m_iSensorNb; Idx++)
            {
                if (pPortHis->IsEnabled() && Idx ==0)
                {
                    BufHis = pPortHis->GetBuffer();
                    memcpy(BufHis.fr_buf.virt_addr,
                        (void *)g_ISP_Camera[Idx].pstShotFrame[m_iBufferID].metadata.histogramStats.regionHistograms,
                    sizeof(g_ISP_Camera[Idx].pstShotFrame[m_iBufferID].metadata.histogramStats.regionHistograms));

                    BufHis.fr_buf.size =
                        sizeof(g_ISP_Camera[Idx].pstShotFrame[m_iBufferID].metadata.histogramStats.regionHistograms);
                    BufHis.Stamp();
                    pPortHis->PutBuffer(&BufHis);
                }

                if (pPortHis1->IsEnabled() && Idx == 1)
                {
                    BufHis1 = pPortHis1->GetBuffer();
                    memcpy(BufHis1.fr_buf.virt_addr,
                        (void *)g_ISP_Camera[Idx].pstShotFrame[m_iBufferID].metadata.histogramStats.regionHistograms,
                    sizeof(g_ISP_Camera[Idx].pstShotFrame[m_iBufferID].metadata.histogramStats.regionHistograms));

                    BufHis1.fr_buf.size =
                        sizeof(g_ISP_Camera[Idx].pstShotFrame[m_iBufferID].metadata.histogramStats.regionHistograms);
                    BufHis1.Stamp();
                    pPortHis1->PutBuffer(&BufHis1);
                }
            }
            for (Idx = 0; Idx < m_iSensorNb; Idx++)
            {
                // for Q360_PROJECT, other product will do nothing
#ifdef COMPILE_ISP_V2500
                IAPI_ISP_DynamicChangeModuleParam(Idx);
#endif
                ret = IAPI_ISP_DynamicChangeIspParameters(Idx);
                if (IMG_SUCCESS !=ret ) {
                    LOGE("failed to call DynamicChangeIspParameters().\n");
                    throw VBERR_RUNTIME;
                    return ;
                }

                ret = IAPI_ISP_DynamicChange3DDenoiseIdx(Idx,ui32DnTargetIdx);
                if (IMG_SUCCESS !=ret ) {
                    LOGE("failed to call DynamicChange3DDenoiseIdx().\n");
                    throw VBERR_RUNTIME;
                    return ;
                }
            }

            m_pstPrvData[m_iBufferID].iDnID = ui32DnTargetIdx;
            pthread_mutex_lock(&mutex_x);
            m_pstPrvData[m_iBufferID].dns_3d_attr = dns_3d_attr;
            m_iBufUseCnt++;
            if (m_iBufUseCnt > (int)m_nBuffers)
                m_iBufUseCnt = (int)m_nBuffers;
            pthread_mutex_unlock(&mutex_x);
            pPortOut->PutBuffer(&BufDst);

            if ((NULL != pdsc) && (pdsc->IsEnabled()))
            {
               pdsc->PutBuffer(&dsc);
            }

            m_iBufferID++;
            if ((IMG_UINT32)m_iBufferID >= m_nBuffers)
                m_iBufferID = 0;

            FrameCount++;

            if(tp.Peek() > 1)
            {
                //LOGD("current fps = %dfps\n", frames);
                FrameCount = 0;
                tp.Poke();
            }
        }
    }

    DestoryShotEventThread(6);
    LOGE("V2500 thread exit ****************\n");
    m_RunSt = CAM_RUN_ST_PREVIEWING;
}

IPU_ISP::IPU_ISP(std::string name, Json *js)
{
    int ret = 0;


    Name = name;
    m_nBuffers = 3;
    m_CurrentStatus = V2500_NONE;
    m_RunSt = CAM_RUN_ST_PREVIEWING; //default
    m_iSensorNb = 0;
    m_iCurCamIdx = 0;
    this->ui32SkipFrame = 0;
    m_iBufferID = 0;
    m_iBufUseCnt = 0;
    m_LockRatio = 0;

    memset(&dns_3d_attr, 0, sizeof(dns_3d_attr));
    memset(&sFpsRange, 0, sizeof(sFpsRange));
    m_psSaveDataParam = NULL;
#ifdef COMPILE_IQ_REALTIME_SAVE_RAW
    m_hasSaveRaw = 0;
#endif
    m_eDataExtractionPoint = CI_INOUT_BLACK_LEVEL;
    m_enIPUType = EN_IPU_ISP;

    if (NULL !=js )
    {
        ui32SkipFrame = js->GetInt("skipframe");
        m_nBuffers = js->GetObject("nbuffers") ? js->GetInt("nbuffers") : 3;  //sensor mode always 0
    }


    ret = IAPI_ISP_Create(js);
    m_iSensorNb = IAPI_ISP_GetSensorNum();
    if (ret)
    {
        IAPI_ISP_Destroy();
        throw VBERR_RUNTIME;
        return;
    }

    BuildPort(1920, 1088*m_iSensorNb);

    m_CurrentStatus = SENSOR_CREATE_SUCESS;
#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
    m_bIqTuningDebugInfoEnable = false;
#endif //#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
}

int IPU_ISP::SetResolution(IMG_UINT32 Idx, int imgW, int imgH)
{
    SetEncScaler(Idx, imgW, imgH);
    return IMG_SUCCESS;
}

//1 Sepia color
int IPU_ISP::SetSepiaMode(IMG_UINT32 Idx, int mode)
{
    return IAPI_ISP_SetSepiaMode(Idx,mode);
}

int IPU_ISP::SetScenario(IMG_UINT32 Idx, int sn)
{
    return IAPI_ISP_SetScenario(Idx,sn);
}

int IPU_ISP::CheckOutRes(IMG_UINT32 Idx, std::string name, int imgW, int imgH)
{
    if (imgW % 64)
    {
        return -1;
    }

    int inW = 0;
    int inH = 0;
    IAPI_ISP_GetMCIIFCrop(Idx,inW,inH);
    m_InW = inW * CI_CFA_WIDTH;
    m_InH = inH * CI_CFA_HEIGHT;

    LOGD("LockRatio is setting : %d , m_InW= %d, m_InH =%d \n", m_LockRatio, m_InW, m_InH);
    if (((imgW < m_InW - m_PipX) && (imgH < m_InH - m_PipY)) && (m_LockRatio))
    {
        if (imgH < imgW)
        {
            if (!strcmp(name.c_str(), "out"))
            {
                m_ImgW = imgH*((float)m_InW/m_InH);
            }
            else
            {
                m_s32DscImgW = imgH*((float)m_InW/m_InH);
            }
        }
        else
        {
            if (!strcmp(name.c_str(), "out"))
            {
                m_ImgH = imgW*((float)m_InH/m_InW);
            }
            else
            {
                m_s32DscImgH = imgW*((float)m_InH/m_InW);
            }
        }
    }

    LOGD("m_InW=%d, m_InH=%d, m_ImgW=%d, m_ImgH=%d\n", m_InW, m_InH, m_ImgW, m_ImgH);

    return 0;
}

void IPU_ISP::Prepare()
{
    // allocate needed memory(reference frame/ snapshots, etc)
    // initialize the hardware
    // get input parameters from PortIn.Width/Height/PixelFormat

    Port *pPortOut = NULL;
    Port *pdsc = NULL;
    IMG_UINT32 Idx = 0;
    cam_fps_range_t fps_range;
#ifdef COMPILE_ISP_V2505
    int fps = 30;
#endif
    Pixel::Format enPixelFormat;

    pPortOut = GetPort("out");
    pdsc = GetPort("dsc");
    enPixelFormat = pPortOut->GetPixelFormat();
    if(enPixelFormat != Pixel::Format::NV12 && enPixelFormat != Pixel::Format::NV21
        && enPixelFormat != Pixel::Format::RGBA8888)
    {
        LOGE("out port Format params error\n");
        throw VBERR_BADPARAM;
    }
    enPixelFormat = GetPort("cap")->GetPixelFormat();
    if(enPixelFormat != Pixel::Format::NV12 && enPixelFormat != Pixel::Format::NV21
        && enPixelFormat != Pixel::Format::RGBA8888)
    {
        LOGE("cap port output Format params error\n");
        throw VBERR_BADPARAM;
    }
    enPixelFormat = GetPort("his")->GetPixelFormat();
    if(enPixelFormat != Pixel::Format::VAM)
    {
        LOGE("his port output Format params error\n");
        throw VBERR_BADPARAM;
    }

    m_pFun = std::bind(&IPU_ISP::ReturnShot,this,std::placeholders::_1);
    pPortOut->frBuffer->RegCbFunc(m_pFun);
    if (pdsc->GetWidth())
    {
        pdsc->frBuffer->RegCbFunc(m_pFun);
    }
    for (Idx = 0; Idx < m_iSensorNb; Idx++)
    {
        IAPI_ISP_SetCIPipelineNum(Idx);
#ifdef COMPILE_ISP_V2500
        IAPI_ISP_AutoControl(Idx);
        IAPI_ISP_LoadParameters(Idx);
#elif defined(COMPILE_ISP_V2505)
        IAPI_ISP_LoadParameters(Idx);

        if (IAPI_ISP_Program(Idx)!= IMG_SUCCESS)
        {
            LOGE("failed to program camera %d\n", __LINE__);
            throw VBERR_RUNTIME;
        }
        IAPI_ISP_AutoControl(Idx);
#endif
#ifdef COMPILE_ISP_V2505
        IAPI_ISP_LoadControlParameters(Idx);
        IAPI_ISP_EnableLSH(Idx);
#endif
#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
        BackupIspParameters(Idx, m_Parameters[Idx]);
#endif //#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
        IAPI_ISP_GetOriTargetBrightness(Idx);

        SetScalerFromPort(Idx);
        if (0 != m_PipW && 0 != m_PipH)
        {
            SetIIFCrop(Idx, m_PipH, m_PipW, m_PipX, m_PipY);
        }

#ifdef COMPILE_IQ_REALTIME_SAVE_RAW
        IAPI_ISP_SetModuleOUTRawFlag(Idx, true, m_eDataExtractionPoint); //for allocating raw buffer in allocateBufferPool function
#endif
        if (IAPI_ISP_SetupModules(Idx)!= IMG_SUCCESS)
        {
            LOGE("failed to setup camera\n");
            throw VBERR_RUNTIME;
        }
        LOGE("in start method : H = %d, W = %d\n", m_ImgH, m_ImgW);
        IAPI_ISP_SetEncoderDimensions(Idx, m_ImgW, m_ImgH);
        if (pdsc && (m_s32DscImgW != 0) && (m_s32DscImgH != 0))
        {
            IAPI_ISP_SetDisplayDimensions(Idx, m_s32DscImgW, m_s32DscImgH);
        }
        if (IAPI_ISP_Program(Idx)!= IMG_SUCCESS)
        {
            LOGE("failed to program camera %d\n", __LINE__);
            throw VBERR_RUNTIME;
        }

        IAPI_ISP_SetCustomGMA(Idx);
        if (BuffersIDs[Idx].size() > 0)
        {
            LOGE(" Buffer all exist, camera start before.\n");
            throw VBERR_BADLOGIC;
        }

        if (IAPI_ISP_AllocateBufferPool(Idx,m_nBuffers, BuffersIDs[Idx]) != IMG_SUCCESS)
        {
            LOGE("camera allocate buffer poll failed\n");
            throw VBERR_RUNTIME;
        }
    }

#ifdef COMPILE_ISP_V2505
    if (NULL != pPortOut)
    {
        fps = pPortOut->GetFPS();
    }
    else if (NULL != pdsc)
    {
        fps = pdsc->GetFPS();
    }
#endif
    for (Idx = 0; Idx < m_iSensorNb; Idx++)
    {
        if (IAPI_ISP_StartCapture(Idx)!= IMG_SUCCESS)
        {
            LOGE("start capture failed.\n");
            throw VBERR_RUNTIME;
        }

        if (m_MinFps == 0 && m_MaxFps == 0) {
#ifdef COMPILE_ISP_V2505
            IAPI_ISP_GetCurrentFps(Idx,fps);
            if (fps) {
                this->GetOwner()->SyncPathFps(fps);
            }
#endif
        } else if (m_MinFps != 0 && m_MaxFps == 0) {
            GetFpsRange(Idx, &fps_range);
            if (m_MinFps > fps_range.max_fps) {
                LOGE("warning: the m_MinFps is more than the max_fps\n");
            } else {
                fps_range.min_fps = m_MinFps;
                SetFpsRange(Idx, &fps_range);
            }
        } else if (m_MinFps == 0 && m_MaxFps != 0) {
            GetFpsRange(Idx, &fps_range);
            if (m_MaxFps < fps_range.min_fps) {
                LOGE("warning: the min_fps is more than the m_MaxFps\n");
            } else {
                fps_range.max_fps = m_MaxFps;
                SetFpsRange(Idx, &fps_range);
            }
        } else if (m_MinFps <=  m_MaxFps) {
            fps_range.min_fps = m_MinFps;
            fps_range.max_fps = m_MaxFps;
            SetFpsRange(Idx, &fps_range);
        } else if (m_MinFps > m_MaxFps) {
            LOGE("warning: the MinFps is more than the MaxFps\n");
            fps_range.min_fps = m_MaxFps;
            fps_range.max_fps = m_MinFps;
            SetFpsRange(Idx, &fps_range);
        }
    }

    m_pstPrvData = (struct ipu_v2500_private_data *)malloc(m_nBuffers * sizeof(struct ipu_v2500_private_data));
    if (m_pstPrvData == NULL)
    {
        LOGE("camera allocate private_data failed\n");
        throw VBERR_RUNTIME;
    }
    memset((char*)m_pstPrvData, 0, m_nBuffers * sizeof(struct ipu_v2500_private_data));
    IMG_UINT32 i = 0;
    for (i = 0; i < m_nBuffers; i++) {
        m_pstPrvData[i].state = SHOT_INITED;
    }

    m_postShots.clear();
    LOGE("(%s:L%d) end \n", __func__, __LINE__);
}

void IPU_ISP::Unprepare()
{
    // stop hardwarePrepare()
    // free allocated memory
    IMG_UINT32 Idx = 0;
    unsigned int i = 0;
    Port *pIpuPort= NULL;


    std::list<IMG_UINT32>::iterator it;
    struct ipu_v2500_private_data *returnPrivData = NULL;
    struct ipu_v2500_private_data *privData = NULL;
    int bufID = 0;
    unsigned int iShotListSize = m_postShots.size();

    // Shot is aquired, but not release
    for (i = 0; i < iShotListSize; i++)
    {
        returnPrivData = NULL;
        if (!m_postShots.empty()) {
            privData = (struct ipu_v2500_private_data *)(m_postShots.front());
            returnPrivData = privData;
            m_postShots.pop_front();
        }
        if (returnPrivData != NULL) {
            bufID = returnPrivData->iBufID;

            for (Idx = 0; Idx < m_iSensorNb; Idx++)
            {
                IAPI_ISP_ReleaseShot(Idx,g_ISP_Camera[Idx].pstShotFrame[bufID]);
            }
        }
    }

    for (Idx = 0; Idx < m_iSensorNb; Idx++)
    {
        // Shot is enqueued, but not aquire & release
        for (i = 0; i < (m_nBuffers - iShotListSize); i++)
        {
            if (IAPI_ISP_AcquireShot(Idx,g_ISP_Camera[Idx].tmp_shot)!= IMG_SUCCESS)
            {
                LOGE("acqureshot:%d before stopping failed: %d\n", i+1, __FILE__);
                throw VBERR_RUNTIME;
            }

            IAPI_ISP_ReleaseShot(Idx,g_ISP_Camera[Idx].tmp_shot);
            LOGD("in unprepare release shot i =%d, Idx = %d\n", i, Idx);
        }
        if (IAPI_ISP_StopCapture(Idx,m_RunSt-1)!= IMG_SUCCESS)
        {
            LOGE("failed to stop the capture.\n");
            throw VBERR_RUNTIME;
        }
        IAPI_ISP_DeleteShots(Idx);
        for (it = BuffersIDs[Idx].begin(); it != BuffersIDs[Idx].end(); it++)
        {
            LOGD("in unprepare free buffer id = %d\n", *it);
            if (IMG_SUCCESS != IAPI_ISP_DeregisterBuffer(Idx,*it))
            {
                throw VBERR_RUNTIME;
            }
        }
        BuffersIDs[Idx].clear();
        LOGD("end ++++++++++++++++++++++++++, %d\n", Idx);
    }
    pIpuPort = GetPort("out");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    pIpuPort = GetPort("his");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    pIpuPort = GetPort("cap");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }

    LOGD("end of unprapare in v2505\n");
	m_postShots.clear();
}

void IPU_ISP::BuildPort(int imgW, int imgH)
{

    Port *pPortOut = NULL, *pPortHis = NULL, *pPortCap = NULL, *pdsc = NULL, *pPortHis1 = NULL;
    pPortOut = CreatePort("out", Port::Out);
    pPortHis = CreatePort("his", Port::Out);
    pPortHis1 = CreatePort("his1", Port::Out);
    pPortCap = CreatePort("cap", Port::Out);
    pdsc = CreatePort("dsc", Port::Out);
    if (NULL != pPortOut)
    {
        m_ImgH = imgH;
        m_ImgW = imgW;

        pPortOut->SetBufferType(FRBuffer::Type::VACANT, m_nBuffers);
        pPortOut->SetResolution((int)imgW, (int)imgH);
        pPortOut->SetPixelFormat(Pixel::Format::NV21);
        pPortOut->SetFPS(30);  //FIXME!!!
        pPortOut->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);
    }

    if(NULL != pdsc)
    {
        // if dsc port has no w, h, use default value 1920x1088
        m_ImgH = (m_ImgH == 0 ? imgH : m_ImgH);
        m_ImgW = (m_ImgW == 0 ? imgW : m_ImgW);
        LOGD("isp build dsc port nBuffers = %d \n", m_nBuffers);
        pdsc->SetBufferType(FRBuffer::Type::VACANT, m_nBuffers);
        pdsc->SetPixelFormat(Pixel::Format::RGBA8888);
        pdsc->SetFPS(30);    //FIXME!!!
        pdsc->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);
    }

    if (NULL != pPortHis)
    {
        pPortHis->SetBufferType(FRBuffer::Type::FLOAT, 128*1024, 4*1024);
        pPortHis->SetPixelFormat(Pixel::Format::VAM);
        pPortHis->SetResolution((int)imgW, (int)imgH);

    }

    if (NULL != pPortHis1)
    {
        pPortHis1->SetBufferType(FRBuffer::Type::FLOAT, 128*1024, 4*1024);
        pPortHis1->SetPixelFormat(Pixel::Format::VAM);
        pPortHis1->SetResolution((int)imgW, (int)imgH);

    }

    if (NULL != pPortCap)
    {
        pPortCap->SetBufferType(FRBuffer::Type::VACANT, 1);
        pPortCap->SetResolution((int)imgW, (int)imgH);
        pPortCap->SetPixelFormat(Pixel::Format::NV12);
        pPortCap->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_Dynamic);
    }
}

int IPU_ISP::SetScalerFromPort(int s32Idx)
{
    Port *p =  GetPort("out");
    Port *pdsc = GetPort("dsc");
    if(p)
    {
        m_ImgW = p->GetWidth();
        m_ImgH = p->GetHeight();
        m_PipX = p->GetPipX();
        m_PipY = p->GetPipY();
        m_PipW = p->GetPipWidth();
        m_PipH = p->GetPipHeight();
        m_MinFps = p->GetMinFps();
        m_MaxFps = p->GetMaxFps();
        m_LockRatio = p->GetLockRatio();
        if (m_PipW != 0 && m_PipH != 0)
        {
            IAPI_ISP_SetMCIIFCrop(s32Idx, m_PipW, m_PipH);
        }
        if (CheckOutRes(s32Idx, p->GetName(), m_ImgW, m_ImgH) < 0)
        {
            LOGE("ISP output width size is not 64 align\n");
            throw VBERR_RUNTIME;
        }

        if (m_ImgW <= m_InW && m_ImgH <= m_InH)
        {
            this->SetEncScaler(s32Idx, m_ImgW, m_ImgH);
        }
        else
        {
            this->SetEncScaler(s32Idx, m_InW, m_InH);
        }

    }

    if (pdsc)
    {
        m_PipX = (m_PipX == 0 ? pdsc->GetPipX() : m_PipX);
        m_PipY = (m_PipY == 0 ? pdsc->GetPipY() : m_PipY);
        m_PipW = (m_PipW == 0 ? pdsc->GetPipWidth() : m_PipW);
        m_PipH = (m_PipH == 0 ? pdsc->GetPipHeight() : m_PipH);
        m_MinFps = (m_MinFps == 0 ? pdsc->GetMinFps() : m_MinFps);
        m_MaxFps = (m_MaxFps == 0 ? pdsc->GetMaxFps() : m_MaxFps);
        m_LockRatio = (m_LockRatio == 0 ? pdsc->GetLockRatio() : m_LockRatio);
        m_s32DscImgW = pdsc->GetWidth();
        m_s32DscImgH = pdsc->GetHeight();
        if (m_PipW != 0 && m_PipH != 0)
        {
            IAPI_ISP_SetMCIIFCrop(s32Idx, m_PipW, m_PipH);
        }
        LOGD("m_s32DscImgW = %d, m_s32DscImgH = %d", m_s32DscImgW, m_s32DscImgH);
        if (CheckOutRes(s32Idx, pdsc->GetName(), m_s32DscImgW, m_s32DscImgH) < 0)
        {
           LOGE("ISP output width size is not 64 align\n");
           throw VBERR_RUNTIME;
        }

        if (m_s32DscImgW != 0 && m_s32DscImgW <= m_InW && m_s32DscImgH  <= m_InH)
        {
            this->SetDscScaler(s32Idx, m_s32DscImgW, m_s32DscImgH);
        }
        else
        {
            this->SetDscScaler(s32Idx, m_InW, m_InH);
        }

        if ((m_s32DscImgW != 0) && (m_s32DscImgH != 0))
        {
            ISPC::ModuleOUT *pout = g_ISP_Camera[s32Idx].pCamera->getModule<ISPC::ModuleOUT>();
            if (pout)
            {
                pout->dataExtractionType = PXL_NONE;
                pout->dataExtractionPoint = CI_INOUT_NONE;
                pout->displayType = (pdsc->GetPixelFormat() == Pixel::RGBA8888) ? BGR_888_32 : BGR_888_24;
            }
        }
    }

    return 0;
}

//set exposure level
int IPU_ISP::SetExposureLevel(IMG_UINT32 Idx, int level)
{
    return IAPI_ISP_SetExposureLevel(Idx,level);
}

int IPU_ISP::GetExposureLevel(IMG_UINT32 Idx, int &level)
{
    return IAPI_ISP_GetExposureLevel(Idx,level);
}

int  IPU_ISP::SetAntiFlicker(IMG_UINT32 Idx, int freq)
{
    return IAPI_ISP_SetAntiFlicker(Idx,freq);
}

int  IPU_ISP::GetAntiFlicker(IMG_UINT32 Idx, int &freq)
{
    return IAPI_ISP_GetAntiFlicker(Idx,freq);
}

int IPU_ISP::SetEncScaler(IMG_UINT32 Idx, int imgW, int imgH)
{
    //m_ImgH = imgH;
    //m_ImgW = imgW;
    return IAPI_ISP_SetEncScaler(Idx,imgW,imgH);
}

int IPU_ISP::SetDscScaler(IMG_UINT32 Idx, int s32ImgW, int s32ImgH)
{
    //m_ImgH = s32ImgH;
    //m_ImgW = s32ImgW;
    return IAPI_ISP_SetDscScaler(Idx,s32ImgW,s32ImgH);
}

int IPU_ISP::SetIIFCrop(IMG_UINT32 Idx, int PipH, int PipW, int PipX, int PipY)
{
    m_PipH = PipH;
    m_PipW = PipW;
    m_PipX = PipX;
    m_PipY = PipY;

    return IAPI_ISP_SetIifCrop(Idx,PipH,PipW,PipX,PipY);
}


int IPU_ISP::SetWB(IMG_UINT32 Idx, int mode)
{
    return IAPI_ISP_SetWb(Idx,mode);
}

int IPU_ISP::GetWB(IMG_UINT32 Idx, int &mode)
{
    return IAPI_ISP_GetWb(Idx,mode);
}

int IPU_ISP::GetYUVBrightness(IMG_UINT32 Idx, int &out_brightness)
{
    return IAPI_ISP_GetYuvBrightness(Idx,out_brightness);
}

int IPU_ISP::SetYUVBrightness(IMG_UINT32 Idx, int brightness)
{
    return IAPI_ISP_SetYuvBrightness(Idx,brightness);
}

int IPU_ISP::GetContrast(IMG_UINT32 Idx, int &contrast)
{
    return IAPI_ISP_GetContrast(Idx,contrast);
}

int IPU_ISP::SetContrast(IMG_UINT32 Idx, int contrast)
{
    return IAPI_ISP_SetContrast(Idx,contrast);
}

int IPU_ISP::GetSaturation(IMG_UINT32 Idx, int &saturation)
{
    return IAPI_ISP_GetSaturation(Idx,saturation);
}

int IPU_ISP::SetSaturation(IMG_UINT32 Idx, int saturation)
{
    return IAPI_ISP_SetSaturation(Idx,saturation);
}

int IPU_ISP::GetSharpness(IMG_UINT32 Idx, int &sharpness)
{
    return IAPI_ISP_GetSharpness(Idx,sharpness);
}

int IPU_ISP::SetSharpness(IMG_UINT32 Idx, int sharpness)
{
    return IAPI_ISP_SetSharpness(Idx,sharpness);
}

int IPU_ISP::SetFPS(IMG_UINT32 Idx, int fps)
{
    int ret = IAPI_ISP_SetFps(Idx,fps);
    if((ret == 0) && (fps >= 0))
        this->GetOwner()->SyncPathFps(fps);

    return ret;
}

int IPU_ISP::GetFPS(IMG_UINT32 Idx, int &fps)
{
    return IAPI_ISP_GetFps(Idx,fps);
}

int IPU_ISP::GetScenario(IMG_UINT32 Idx, enum cam_scenario &scen)
{
    return IAPI_ISP_GetScenario(Idx,scen);
}

int IPU_ISP::GetResolution(int &imgW, int &imgH)
{
    imgH = m_ImgH;
    imgW = m_ImgW;

    return IMG_SUCCESS;
}

//get current frame statistics lumination value in AE
// enviroment brightness value
int IPU_ISP::GetEnvBrightnessValue(IMG_UINT32 Idx, int &out_bv)
{
    return IAPI_ISP_GetEnvBrightness(Idx,out_bv);
}

//control AE enable
int IPU_ISP::EnableAE(IMG_UINT32 Idx, int enable)
{
    return IAPI_ISP_EnableAE(Idx,enable);
}

int IPU_ISP::EnableAWB(IMG_UINT32 Idx, int enable)
{
    return IAPI_ISP_EnableAWB(Idx,enable);
}

int IPU_ISP::IsAWBEnabled(IMG_UINT32 Idx)
{
    return IAPI_ISP_IsAWBEnabled(Idx);
}

int IPU_ISP::IsAEEnabled(IMG_UINT32 Idx)
{
    return IAPI_ISP_IsAEEnabled(Idx);
}

int IPU_ISP::IsMonchromeEnabled(IMG_UINT32 Idx)
{
    //return m_Cam[Idx].pCamera->IsMonoMode();
    return IAPI_ISP_IsMonochromeEnabled(Idx);
}

int IPU_ISP::SetSensorISO(IMG_UINT32 Idx,int iso)
{
    return IAPI_ISP_SetSensorIso(Idx,iso);
}

int IPU_ISP::GetSensorISO(IMG_UINT32 Idx, int &iso)
{
    return IAPI_ISP_GetSensorIso(Idx,iso);
}

//control monochrome
int IPU_ISP::EnableMonochrome(IMG_UINT32 Idx, int enable)
{
    return IAPI_ISP_EnableMonochrome(Idx,enable);
}

int IPU_ISP::EnableWDR(IMG_UINT32 Idx, int enable)
{
    return IAPI_ISP_EnableWdr(Idx,enable);
}

int IPU_ISP::IsWDREnabled(IMG_UINT32 Idx)
{
    return IAPI_ISP_IsWdrEnabled(Idx);
}

int IPU_ISP::GetMirror(IMG_UINT32 Idx, enum cam_mirror_state &dir)
{
    return IAPI_ISP_GetMirror(Idx,dir);
}

int IPU_ISP::SetMirror(IMG_UINT32 Idx, enum cam_mirror_state dir)
{
    return IAPI_ISP_SetMirror(Idx,dir);
}

int IPU_ISP::SetSensorResolution(IMG_UINT32 Idx, const res_t res)
{
    return IAPI_ISP_SetSensorResolution(Idx,res);
}

int IPU_ISP::SetSnapResolution(IMG_UINT32 Idx, const res_t res)
{
    return SetResolution(Idx, res.W, res.H);
}

int IPU_ISP::SnapOneShot()
{
    pthread_mutex_lock(&mutex_x);
    m_RunSt = CAM_RUN_ST_CAPTURING;
    pthread_mutex_unlock(&mutex_x);
    return IMG_SUCCESS;
}

int IPU_ISP::SnapExit()
{
    pthread_mutex_lock(&mutex_x);
    m_RunSt = CAM_RUN_ST_PREVIEWING;
    pthread_mutex_unlock(&mutex_x);
    return IMG_SUCCESS;
}

int IPU_ISP::GetSnapResolution(IMG_UINT32 Idx, reslist_t *presl)
{
    return IAPI_ISP_GetSnapResolution(Idx,presl);
}

void IPU_ISP::DeinitCamera()
{
    IAPI_ISP_Destroy();
}

IPU_ISP::~IPU_ISP()
{
    DeinitCamera();

    if (m_pstPrvData != NULL)
    {
        free(m_pstPrvData);
        m_pstPrvData = NULL;
    }

    //release v2500 port
    DestroyPort("out");
    DestroyPort("hist");
}

int IPU_ISP::CheckHSBCInputParam(int param)
{
    if (param < IPU_HSBC_MIN || param > IPU_HSBC_MAX)
    {
        LOGE("(%s,L%d) param %d is error!\n", __FUNCTION__, __LINE__, param);
        return -1;
    }
    return 0;
}

int IPU_ISP::CalcHSBCToISP(double max, double min, int param, double &out_val)
{
    if (max <= min)
    {
        LOGE("(%s,L%d) max %f or min %f is error!\n", __FUNCTION__, __LINE__, max, min);
        return -1;
    }

    out_val = ((double)param) * (max - min) / (IPU_HSBC_MAX - IPU_HSBC_MIN) + min;
    return 0;
}

int IPU_ISP::CalcHSBCToOut(double max, double min, double param, int &out_val)
{
    if (max <= min)
    {
        LOGE("(%s,L%d) max %f or min %f is error!\n", __FUNCTION__, __LINE__, max, min);
        return -1;
    }
    out_val = (int)floor((param - min) * (IPU_HSBC_MAX - IPU_HSBC_MIN) / (max - min) + 0.5);

    return 0;
}

int IPU_ISP::GetHue(IMG_UINT32 Idx, int &out_hue)
{
    return IAPI_ISP_GetHue(Idx,out_hue);
}

int IPU_ISP::SetHue(IMG_UINT32 Idx, int hue)
{
    return IAPI_ISP_SetHue(Idx,hue);
}

int IPU_ISP::CheckAEDisabled(IMG_UINT32 Idx)
{
    int ret = -1;

    if(IAPI_ISP_IsAEEnabled(Idx))
    {
        LOGE("(%s,L%d) Error: Not disable AE !!!\n", __func__, __LINE__);
        return ret;
    }
    return 0;
}

int IPU_ISP::CheckSensorClass(IMG_UINT32 Idx)
{
    return IAPI_ISP_CheckSensorClass(Idx);
}

int IPU_ISP::SetSensorExposure(IMG_UINT32 Idx, int usec)
{
    return IAPI_ISP_SetSensorExposure(Idx,usec);
}

int IPU_ISP::GetSensorExposure(IMG_UINT32 Idx, int &usec)
{
    return IAPI_ISP_GetSensorExposure(Idx,usec);
}

int IPU_ISP::SetDpfAttr(IMG_UINT32 Idx, const cam_dpf_attr_t *attr)
{
    return IAPI_ISP_SetDpfAttr(Idx,attr);
}


int IPU_ISP::GetDpfAttr(IMG_UINT32 Idx, cam_dpf_attr_t *attr)
{
    return IAPI_ISP_GetDpfAtrr(Idx,attr);
}

int IPU_ISP::SetShaDenoiseAttr(IMG_UINT32 Idx, const cam_sha_dns_attr_t *attr)
{
    return IAPI_ISP_SetShaDenoiseAttr(Idx,attr);
}


int IPU_ISP::GetShaDenoiseAttr(IMG_UINT32 Idx, cam_sha_dns_attr_t *attr)
{
    return IAPI_ISP_GetShaDenoiseAtrr(Idx,attr);
}

int IPU_ISP::SetDnsAttr(IMG_UINT32 Idx, const cam_dns_attr_t *attr)
{
    return IAPI_ISP_SetDnsAttr(Idx,attr);
}


int IPU_ISP::GetDnsAttr(IMG_UINT32 Idx, cam_dns_attr_t *attr)
{
    return IAPI_ISP_GetDnsAtrr(Idx,attr);
}

#ifdef COMPILE_ISP_V2500
int IPU_ISP::EnqueueReturnShot(void)
{
    struct ipu_v2500_private_data *returnPrivData = NULL;
    IMG_UINT32 Idx = 0;


    pthread_mutex_lock(&mutex_x);
    if (!m_postShots.empty()) {
        struct ipu_v2500_private_data *privData = (struct ipu_v2500_private_data *)(m_postShots.front());
        //NOTE: only SHOT_RETURNED will be releae
        //TODO: release all shot before SHOT_RETURNED
        if (privData->state == SHOT_RETURNED) {
            privData->state = SHOT_INITED;
            returnPrivData = privData;
            m_iBufUseCnt--;
            m_postShots.pop_front();
        }
    }

    if (returnPrivData != NULL) {
        int bufID = returnPrivData->iBufID;

        for (Idx = 0; Idx < m_iSensorNb; Idx++)
        {
            IAPI_ISP_ReleaseShot(Idx,g_ISP_Camera[Idx].pstShotFrame[bufID]);
            IAPI_ISP_EnqueueShot(Idx);
        }
        returnPrivData = NULL;
    }
    pthread_mutex_unlock(&mutex_x);

    return 0;
}
#else //COMPILE_ISP_V2505)
int IPU_ISP::EnqueueReturnShot(void)
{
    struct ipu_v2500_private_data *returnPrivData = NULL;
    struct ipu_v2500_private_data *privData = NULL;
    int bufID = 0;
    IMG_UINT32 Idx = 0;
    IMG_UINT32 i;

    for (i = 0; i < m_nBuffers; i++)
    {
        pthread_mutex_lock(&mutex_x);
        if (!m_postShots.empty())
        {
            privData = (struct ipu_v2500_private_data *)(m_postShots.front());
            //NOTE: only SHOT_RETURNED will be releae
            //TODO: release all shot before SHOT_RETURNED
            if (privData->state == SHOT_RETURNED)
            {
                privData->state = SHOT_INITED;
                returnPrivData = privData;
                m_postShots.pop_front();
            }
        }
        pthread_mutex_unlock(&mutex_x);

        if (returnPrivData != NULL)
        {
            bufID = returnPrivData->iBufID;
            for (Idx = 0; Idx < m_iSensorNb; Idx++)
            {
                IAPI_ISP_ReleaseShot(Idx,g_ISP_Camera[Idx].pstShotFrame[bufID]);
                IAPI_ISP_EnqueueShot(Idx);
            }
            returnPrivData = NULL;
        }
        else
        {
            int j = 0;
            int return_found = 0;
            int return_idx = 0;
            std::list<void*>::iterator it;
            pthread_mutex_lock(&mutex_x);
            if (m_postShots.size() >= 2)
            {
                for(it = m_postShots.begin(); it!=m_postShots.end(); ++it)
                {
                    privData = (struct ipu_v2500_private_data *)*it;
                    if (privData->state == SHOT_RETURNED && !return_found)
                    {
                        return_idx = j;
                        return_found = 1;
                        LOGE("@@@ Shot Returned Index = %d \n", return_idx);
                    }
                    j++;
                }
            }

            if (!return_found)
            {
                pthread_mutex_unlock(&mutex_x);
                break;
            }

            for(j = 0; j <= return_idx; j++)
            {
                privData = (struct ipu_v2500_private_data *)(m_postShots.front());
                LOGE("@@@ pop %d %p status=%d id=%d\n",
                    j, privData, privData->state, privData->iBufID);

                if (privData->state == SHOT_POSTED)
                {
                    m_iBufUseCnt--;
                    if (m_iBufUseCnt < 0)
                        m_iBufUseCnt = 0;
                }
                privData->state = SHOT_INITED;
                returnPrivData = privData;
                m_postShots.pop_front();

                if (privData != NULL)
                {
                    bufID = privData->iBufID;
                    for (Idx = 0; Idx < m_iSensorNb; Idx++)
                    {
                        IAPI_ISP_ReleaseShot(Idx,g_ISP_Camera[Idx].pstShotFrame[bufID]);
                        IAPI_ISP_EnqueueShot(Idx);
                    }
                    privData = NULL;
                }
            }
            pthread_mutex_unlock(&mutex_x);
            break;
        }
    }
    return 0;
}
#endif

int IPU_ISP::ClearAllShot()
{
    IMG_UINT32 Idx = 0;
    IMG_UINT32 i = 0;


    pthread_mutex_lock(&mutex_x);
	if (m_postShots.size() == m_nBuffers) {
        //NOTE: here we only release first shot in the list
        for (i = 0; i < m_nBuffers; i++) {
            struct ipu_v2500_private_data *returnPrivData = NULL;
            if (!m_postShots.empty()) {
                struct ipu_v2500_private_data *privData =
                    (struct ipu_v2500_private_data *)(m_postShots.front());
                //NOTE: SHOT_POSTED & SHOT_RETURNED will be release
                if (privData->state != SHOT_INITED) {
                    privData->state = SHOT_INITED;
                    returnPrivData = privData;
                    m_iBufUseCnt--;
                    if (m_iBufUseCnt < 0)
                        m_iBufUseCnt = 0;
                    m_postShots.pop_front();
                }
            }

            if (returnPrivData != NULL) {
                int bufID = returnPrivData->iBufID;

                for (Idx = 0; Idx < m_iSensorNb; ++Idx) {
                    IAPI_ISP_ReleaseShot(Idx,g_ISP_Camera[Idx].pstShotFrame[bufID]);
                    IAPI_ISP_EnqueueShot(Idx);
                }
            }
        }
    }

    pthread_mutex_unlock(&mutex_x);
    return 0;
}

int IPU_ISP::ReturnShot(Buffer *pstBuf)
{
    struct ipu_v2500_private_data *pstPrvData;
    IMG_UINT32 Idx = 0, BufID = 0;
    int s32EnEsc = 0;


    pthread_mutex_lock(&mutex_x);
    if ((pstBuf == NULL) || (m_iBufUseCnt < 1)){
        LOGD("pCamera:%p m_iBufUseCnt:%d\n", pstBuf, m_iBufUseCnt);
         pthread_mutex_unlock(&mutex_x);
        return -1;
    }

    s32EnEsc= ((struct ipu_v2500_private_data *)pstBuf->fr_buf.priv)->s32EnEsc;
    LOGD("in return shot s32EnEsc = %d pstBuf->fr_buf.stAttr.s32Width = 0x%x@@@\n", \
               s32EnEsc, pstBuf->fr_buf.stAttr.s32Width);
    if (s32EnEsc == 0x01 && (unsigned int)pstBuf->fr_buf.stAttr.s32Width == 0xFF)
    {
        LOGD("Prepare end@@@\n");
        pthread_mutex_unlock(&mutex_x);
        return 0;
    }

    pstPrvData = (struct ipu_v2500_private_data *)pstBuf->fr_buf.priv;

    if ((IMG_UINT32)pstPrvData->iBufID >= m_nBuffers)
    {
        LOGE("Buf ID over flow\n");
        pthread_mutex_unlock(&mutex_x);
        return -1;
    }

    BufID = pstPrvData->iBufID;

    for (Idx = 0; Idx < m_iSensorNb; Idx++)
    {
        if(IAPI_ISP_IsCameraNull(Idx))
        {
            pthread_mutex_unlock(&mutex_x);
            return -1;
        }

        if(g_ISP_Camera[Idx].pstShotFrame[BufID].YUV.phyAddr == 0)
        {
            LOGD("Shot phyAddr is NULL iBufID:%d \n", pstPrvData->iBufID);
            pthread_mutex_unlock(&mutex_x);
            return 0;
        }
    }
    if (pstPrvData->state != SHOT_POSTED) {
        LOGE("Shot is wrong state:%d\n", pstPrvData->state);
        pthread_mutex_unlock(&mutex_x);
        return 0;
    }
    pstPrvData->state = SHOT_RETURNED;
#ifdef COMPILE_ISP_V2505
    m_iBufUseCnt--;
    if (m_iBufUseCnt < 0)
        m_iBufUseCnt = 0;
#endif
    pthread_mutex_unlock(&mutex_x);
    EnqueueShotEvent(SHOT_EVENT_ENQUEUE, (void*)__func__);
    return 0;
}

int IPU_ISP::SetWbAttr(IMG_UINT32 Idx, const cam_wb_attr_t *attr)
{
    return IAPI_ISP_SetWbAttr(Idx,attr);
}

int IPU_ISP::GetWbAttr(IMG_UINT32 Idx, cam_wb_attr_t *attr)
{
    return IAPI_ISP_GetWbAtrr(Idx,attr);
}

int IPU_ISP::Set3dDnsAttr(const cam_3d_dns_attr_t *attr)
{
    int ret = -1;


    ret = CheckParam(attr);
    if (ret)
        return ret;

    pthread_mutex_lock(&mutex_x);
    this->dns_3d_attr = *attr;
    pthread_mutex_unlock(&mutex_x);

    return ret;
}

int IPU_ISP::Get3dDnsAttr(cam_3d_dns_attr_t *attr)
{
    int ret = -1;


    ret = CheckParam(attr);
    if (ret)
        return ret;
    this->dns_3d_attr.cmn.mdl_en = 1;
    *attr = this->dns_3d_attr;

    return ret;
}

int IPU_ISP::SetYuvGammaAttr(IMG_UINT32 Idx, const cam_yuv_gamma_attr_t *attr)
{
    return IAPI_ISP_SetYuvGammaAttr(Idx,attr);
}

int IPU_ISP::GetYuvGammaAttr(IMG_UINT32 Idx, cam_yuv_gamma_attr_t *attr)
{
    return IAPI_ISP_GetYuvGammaAtrr(Idx,attr);
}

int IPU_ISP::SetFpsRange(IMG_UINT32 Idx, const cam_fps_range_t *fps_range)
{
    int ret = -1;
    ret = IAPI_ISP_SetFpsRange(Idx,fps_range);
    if(ret == 1)
    {
        this->GetOwner()->SyncPathFps(fps_range->max_fps);
        sFpsRange = *fps_range;
    }
    else if(ret == 2)
    {
        this->GetOwner()->SyncPathFps(0x80|(int)fps_range->max_fps); //set highest bit of this byte means camera in range fps mode
        sFpsRange = *fps_range;
    }
    else
    {

    }

    return ret;
}

int IPU_ISP::GetFpsRange(IMG_UINT32 Idx, cam_fps_range_t *fps_range)
{
    int ret = -1;

    ret = CheckParam(fps_range);
    if (ret)
        return ret;

    *fps_range = sFpsRange;

    return ret;
}

int IPU_ISP::GetDayMode(IMG_UINT32 Idx, enum cam_day_mode &out_day_mode)
{
    return IAPI_ISP_GetDayMode(Idx,out_day_mode);
}

int IPU_ISP::SkipFrame(IMG_UINT32 Idx, int skipNb)
{
    return IAPI_ISP_SkipFrame(Idx,skipNb);
}

int IPU_ISP::SetCameraIdx(int Idx)
{
    m_iCurCamIdx = Idx;
    return IMG_SUCCESS;
}

int IPU_ISP::GetCameraIdx(int &Idx)
{
    Idx = m_iCurCamIdx;
    return IMG_SUCCESS;
}

int IPU_ISP::GetPhyStatus(IMG_UINT32 Idx, IMG_UINT32 *status)
{
    return IAPI_ISP_GetPhyStatus(Idx,status);
}

int IPU_ISP::SetGAMCurve(IMG_UINT32 Idx,cam_gamcurve_t *GamCurve)
{
    return IAPI_ISP_SetGMA(Idx,GamCurve);
}

int IPU_ISP::SaveData(cam_save_data_t *saveData)
{
    int ret = 0;
#if 0
    struct timespec stTs;
    int maxWaitSec = NOTIFY_TIMEOUT_SEC;
#endif

    ret = CheckParam(saveData);
    if (ret)
        return ret;
    pthread_mutex_lock(&mutex_x);
    m_psSaveDataParam = saveData;
    pthread_mutex_unlock(&mutex_x);
    LOGD("(%s:L%d) %s, fmt=0x%x\n",
            __func__, __LINE__, m_psSaveDataParam->file_path, m_psSaveDataParam->fmt);

    sem_init(&m_semNotify, 0, 0);
#ifdef COMPILE_IQ_REALTIME_SAVE_RAW
    m_hasSaveRaw = 1;
#else
    SnapOneShot();
#endif

#if 0
    clock_gettime(CLOCK_REALTIME, &stTs);
    stTs.tv_sec += maxWaitSec;
    ret = sem_timedwait(&m_semNotify, &stTs);
#endif
    ret = sem_wait(&m_semNotify);
    if (ret)
    {
        if (errno == ETIMEDOUT)
        {
            LOGE("(%s:L%d) Warning timeout \n", __func__, __LINE__);
        }
    }
#ifdef COMPILE_IQ_REALTIME_SAVE_RAW
#else
    SnapExit();
#endif
    sem_destroy(&m_semNotify);
    pthread_mutex_lock(&mutex_x);
    m_psSaveDataParam = NULL;
    pthread_mutex_unlock(&mutex_x);

    return ret;
}

int IPU_ISP::GetSensorsName(IMG_UINT32 Idx, cam_list_t * pName)
{
    return IAPI_ISP_GetSensorsName(Idx,pName);
}

int IPU_ISP::GetDebugInfo(void *info, int *size)
{
   ST_ISP_DBG_INFO *pISPInfo = NULL;

    pISPInfo = (ST_ISP_DBG_INFO *)info;
    *size = sizeof(ST_ISP_DBG_INFO);

    IAPI_ISP_GetDebugInfo(info);

    Port *pcap = this->GetPort("cap");
    pISPInfo->s32CapW =  pcap->GetWidth();
    pISPInfo->s32CapH = pcap->GetHeight();
    LOGD("cap w =%d h =%d\n", pISPInfo->s32CapW, pISPInfo->s32CapH);

    Port *p = GetPort("out");
    pISPInfo->s32OutW =p->GetWidth();
    pISPInfo->s32OutH = p->GetHeight();
    LOGD("out w =%d h =%d\n", pISPInfo->s32OutW, pISPInfo->s32OutH);

    return IMG_SUCCESS;
}
