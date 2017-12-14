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
#include "V2500.h"
//#include <fstream>

extern "C" {
#include <qsdk/items.h>
}
DYNAMIC_IPU(IPU_V2500, "v2500");
#define SKIP_FRAME (3)
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

    IPU_V2500* isp = (IPU_V2500*)arg;
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

void IPU_V2500::WorkLoop()
{
    Buffer BufDst;
    Buffer BufHis;
    Buffer BufHis1;
    Port *pPortOut = GetPort("out");
    Port *pPortHis = GetPort("his");
    Port *pPortHis1 = GetPort("his1");
    Port *pPortCap = GetPort("cap");

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
        SetModuleOUTRawFlag(Idx, false, m_eDataExtractionPoint); //disable raw in normal flow
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
    for (Idx = 0; Idx < m_iSensorNb; Idx++)
    {
        for (i = 0; i < m_nBuffers; i++)
        {
            ret = m_Cam[Idx].pCamera->enqueueShot();
            if (IMG_SUCCESS != ret)
            {
                LOGE("enqueue shot in v2500 ipu thread failed.\n");
                throw VBERR_RUNTIME;
                return ;
            }
        }

        ConfigAutoControl(Idx);
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
                    m_Cam[Idx].pCamera->SetCaptureIQFlag(IMG_TRUE);
                }

                if (pPortCap)
                {
                    m_ImgH = pPortCap->GetHeight();
                    m_ImgW = pPortCap->GetWidth();
                    m_PipH = pPortCap->GetPipHeight();
                    m_PipW = pPortCap->GetPipWidth();
                    m_PipX = pPortCap->GetPipX();
                    m_PipY = pPortCap->GetPipY();
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
                        m_Cam[Idx].pCamera->pipeline->getMCPipeline()->sIIF.ui16ImagerSize[0] = m_PipW / CI_CFA_WIDTH;
                        m_Cam[Idx].pCamera->pipeline->getMCPipeline()->sIIF.ui16ImagerSize[1] = m_PipH / CI_CFA_WIDTH;
                        SetIIFCrop(Idx, m_PipH, m_PipW, m_PipX, m_PipY);
                    }
                    if (CheckOutRes(Idx, m_ImgW, m_ImgH) < 0)
                    {
                        LOGE("ISP output width size is not 64 align\n");
                        throw VBERR_RUNTIME;
                    }

                    if (0 == Idx)
                        Unprepare();

                    m_Cam[Idx].pCamera->setEncoderDimensions(m_ImgW, m_ImgH);
                    if (m_ImgW <= m_InW && m_ImgH <= m_InH)
                    {
                        SetEncScaler(Idx, m_ImgW, m_ImgH); //Set output scaler by current port resolution;
                    }
                    else
                    {
                        SetEncScaler(Idx, m_InW, m_InH);
                    }

                    pthread_mutex_lock(&mutex_x);
                    if (m_psSaveDataParam)
                    {
                        hasSavedData = true;
                        if (m_psSaveDataParam->fmt & (CAM_DATA_FMT_BAYER_FLX | CAM_DATA_FMT_BAYER_RAW))
                        {
                            SetModuleOUTRawFlag(Idx, true, m_eDataExtractionPoint);
                            m_Cam[Idx].pCamera->setDisplayDimensions(m_ImgW, m_ImgH);
                            hasDE = true;
                        }
                    }
                    pthread_mutex_unlock(&mutex_x);
                    m_Cam[Idx].pCamera->setupModules();
                    m_Cam[Idx].pCamera->program();
                    m_Cam[Idx].pCamera->allocateBufferPool(1, m_Cam[Idx].BuffersIDs);
                    LOGD("end of stop preview in capture mode\n");
                }
            } //End of if (nCapture == 1)

            for (Idx = 0; Idx < m_iSensorNb; Idx++)
            {
                LOGD("in capture running : m_RunSt = %d#######\n", m_RunSt);
                m_Cam[Idx].pCamera->startCapture(false);
            }

            do {
                for (Idx = 0; Idx < m_iSensorNb; Idx++)
                {
                    m_Cam[Idx].pCamera->enqueueShot();
                    LOGD("@@@@@ in after enqueueShot in catpure mode @@@@@@@@@@@@@@\n");
    retry:
                    ret = m_Cam[Idx].pCamera->acquireShot(m_Cam[Idx].CapShot, true);
                    if (IMG_SUCCESS != ret) {
                        m_Cam[Idx].pCamera->sensor->disable();
                        usleep(10000);
                        m_Cam[Idx].pCamera->sensor->enable();
                        goto retry;
                    }
                }
                //file.write((char *)frame.YUV.data, nsize);
                //TODO
                if ((skip_fr == SKIP_FRAME ) || nCapture > SKIP_FRAME) { //not continue capture , then put the buffer to next ipu
                    BufDst = pPortCap->GetBuffer();
                    BufDst.fr_buf.phys_addr = (uint32_t)m_Cam[0].CapShot.YUV.phyAddr;
                    pPortCap->PutBuffer(&BufDst);
                    LOGD("put the buffer to the next!\n");
                    if (hasSavedData)
                    {
                        pthread_mutex_lock(&mutex_x);
                        SaveFrame(0, m_Cam[0].CapShot);
                        pthread_mutex_unlock(&mutex_x);
                    }
                }

                for (Idx = 0; Idx < m_iSensorNb; Idx++)
                {
                    m_Cam[Idx].pCamera->releaseShot(m_Cam[Idx].CapShot);
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
                m_Cam[Idx].pCamera->stopCapture(false);
            }
            m_RunSt = CAM_RUN_ST_CAPTURED;
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
                            SetModuleOUTRawFlag(Idx, false, m_eDataExtractionPoint);
                        }
                        hasDE = false;
                    }
                }
                for (Idx = 0; Idx < m_iSensorNb; Idx++)
                {
                    PreSt = m_RunSt;
                    skip_fr = 0;
                    nCapture = 0;//recover ncapture to 0, for next preview to capture.
                    LOGD("Is here running from capture switch preview ##########: runSt =%d, preSt =%d\n", m_RunSt, PreSt);
                    m_Cam[Idx].pCamera->SetCaptureIQFlag(IMG_FALSE);
                    m_Cam[Idx].pCamera->deleteShots();
                    std::list<IMG_UINT32>::iterator it;

                    for (it = m_Cam[Idx].BuffersIDs.begin(); it != m_Cam[Idx].BuffersIDs.end(); it++)
                    {
                        LOGD("getting bufferids = %d\n", *it);
                        if (IMG_SUCCESS != m_Cam[Idx].pCamera->deregisterBuffer(*it))
                        {
                            throw VBERR_RUNTIME;
                        }
                    }
                    m_Cam[Idx].BuffersIDs.clear();

                    m_Cam[Idx].pCamera->getPipeline()->getCIPipeline()->uiTotalPipeline = m_iSensorNb;
                    if (pPortOut)
                    {
                        m_ImgH = pPortOut->GetHeight();
                        m_ImgW = pPortOut->GetWidth();
                        m_PipH = pPortOut->GetPipHeight();
                        m_PipW = pPortOut->GetPipWidth();
                        m_PipX = pPortOut->GetPipX();
                        m_PipY = pPortOut->GetPipY();
                    }

                    if (CheckOutRes(Idx, m_ImgW, m_ImgH) < 0)
                    {
                        LOGE("ISP output width size is not 64 align\n");
                        throw VBERR_RUNTIME;
                    }

                    if (0 != m_PipW && 0 != m_PipH)
                    {
                        m_Cam[Idx].pCamera->pipeline->getMCPipeline()->sIIF.ui16ImagerSize[0] = m_PipW / CI_CFA_WIDTH;
                        m_Cam[Idx].pCamera->pipeline->getMCPipeline()->sIIF.ui16ImagerSize[1] = m_PipH / CI_CFA_HEIGHT;
                        SetIIFCrop(Idx, m_PipH, m_PipW, m_PipX, m_PipY);
                    }

                    if (m_ImgW <= m_InW && m_ImgH <= m_InH)
                    {
                        SetEncScaler(Idx, m_ImgW, m_ImgH);//Set output scaler by current port resolution;
                    }
                    else
                    {
                        SetEncScaler(Idx, m_InW, m_InH);
                    }
                    LOGD("====preview: m_ImgW = %d, m_ImgH =%d, m_PipH = %d, m_PipW = %d@@@\n", m_ImgW,  m_ImgH, m_PipH, m_PipW);
                    m_Cam[Idx].pCamera->setEncoderDimensions(m_ImgW, m_ImgH);
                    m_Cam[Idx].pCamera->setupModules();
                    m_Cam[Idx].pCamera->program();

                    m_Cam[Idx].pCamera->allocateBufferPool(m_nBuffers, m_Cam[Idx].BuffersIDs);
                }

                for (Idx = 0; Idx < m_iSensorNb; Idx++)
                {
                    m_Cam[Idx].pCamera->startCapture(false);

                    for (i = 0; i < m_nBuffers; i++)
                    {
                        ret = m_Cam[Idx].pCamera->enqueueShot();
                        if (IMG_SUCCESS != ret)
                        {
                            //LOGE("enqueue shot in v2500 ipu thread failed.\n");
                            throw VBERR_RUNTIME;
                            return ;
                        }
                    }
                }
                CreateShotEventThread((void*)this);
            }


            BufDst = pPortOut->GetBuffer();

            if ((IMG_UINT32)m_iBufUseCnt >= m_nBuffers)
            {
                ClearAllShot();
                LOGD("recevie callback timeout, clear all shot!\n");
            }

            LOGD("(%s:L%d) m_iBufferID:%d m_iBufUseCnt:%d\n", __func__, __LINE__, m_iBufferID, m_iBufUseCnt);

            //EnqueueReturnShot();
            // sync buffer from shot to GetCBuffer()
            // BufDst.fr_buf.phys_addr = xxx.YUV.physAddr;

#ifdef COMPILE_IQ_REALTIME_SAVE_RAW
retryCapRaw:
#endif
            for (Idx = 0; Idx < m_iSensorNb; Idx++)
            {
retry_2:
                //may be blocking until the frame is received
                ret = m_Cam[Idx].pCamera->acquireShot(m_Cam[Idx].pstShotFrame[m_iBufferID], true);
                if (IMG_SUCCESS != ret)
                {
                    GetPhyStatus(Idx, &CRCStatus);
                    if (CRCStatus != 0) {
                        for (i = 0; i < 5; i++) {
                            m_Cam[Idx].pCamera->sensor->reset();
                            usleep(200000);
                            goto retry_2;
                        }
                        i = 0;
                    }
                    LOGE("failed to get shot %d\n", __LINE__);
                    throw VBERR_RUNTIME;
                }

            }
            //TODO
            for (Idx = 0; Idx < m_iSensorNb; Idx++)
            if (m_iSensorNb > 1)//dual sensor
            {
                m_Cam[Idx].pCamera->DualSensorUpdate(true, m_Cam[0].pstShotFrame[m_iBufferID], m_Cam[1].pstShotFrame[m_iBufferID]);
            }

            timestamp = m_Cam[0].pstShotFrame[m_iBufferID].metadata.timestamps.ui64CurSystemTS;
            BufDst.Stamp(timestamp);
            BufDst.fr_buf.phys_addr = (uint32_t)m_Cam[0].pstShotFrame[m_iBufferID].YUV.phyAddr;

#ifdef COMPILE_IQ_REALTIME_SAVE_RAW
            if (hasCapRaw)
            {
                for (Idx = 0; Idx < m_iSensorNb; Idx++)
                {
                    SetModuleOUTRawFlag(Idx, true, m_eDataExtractionPoint);
                }
                usleep(100 * m_nBuffers * 1000); //for DDR bandwidth, next IPU can not get buffer to work
                if (m_Cam[0].pstShotFrame[m_iBufferID].BAYER.phyAddr
                    && m_Cam[0].pstShotFrame[m_iBufferID].BAYER.data)
                {
                    pthread_mutex_lock(&mutex_x);
                    SaveFrame(0, m_Cam[0].pstShotFrame[m_iBufferID]);
                    m_hasSaveRaw = 0;
                    hasCapRaw = 0;
                    pthread_mutex_unlock(&mutex_x);
                    for (Idx = 0; Idx < m_iSensorNb; Idx++)
                    {
                        SetModuleOUTRawFlag(Idx, false, m_eDataExtractionPoint);
                    }
                }
                for (Idx = 0; Idx < m_iSensorNb; Idx++)
                {
                    m_Cam[Idx].pCamera->releaseShot(m_Cam[Idx].pstShotFrame[m_iBufferID]);
                    m_Cam[Idx].pCamera->enqueueShot();
                    m_Cam[Idx].pstShotFrame[m_iBufferID].YUV.phyAddr = 0;
                    m_Cam[Idx].pstShotFrame[m_iBufferID].BAYER.phyAddr = 0;
                }
                goto retryCapRaw;
            }
#endif

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
            for (Idx = 0; Idx < m_iSensorNb; Idx++)
            {
                if (pPortHis->IsEnabled() && Idx == 0)
                {
                    BufHis = pPortHis->GetBuffer();
                    memcpy(BufHis.fr_buf.virt_addr, (void *)m_Cam[Idx].pstShotFrame[m_iBufferID].metadata.histogramStats.regionHistograms,
                    sizeof(m_Cam[Idx].pstShotFrame[m_iBufferID].metadata.histogramStats.regionHistograms));
                    BufHis.fr_buf.size = sizeof(m_Cam[Idx].pstShotFrame[m_iBufferID].metadata.histogramStats.regionHistograms);
                    BufHis.Stamp();
                    pPortHis->PutBuffer(&BufHis);
                }

                if (pPortHis->IsEnabled() && Idx == 1)
                {
                    BufHis1 = pPortHis1->GetBuffer();
                    memcpy(BufHis1.fr_buf.virt_addr, (void *)m_Cam[Idx].pstShotFrame[m_iBufferID].metadata.histogramStats.regionHistograms,
                    sizeof(m_Cam[Idx].pstShotFrame[m_iBufferID].metadata.histogramStats.regionHistograms));
                    BufHis1.fr_buf.size = sizeof(m_Cam[Idx].pstShotFrame[m_iBufferID].metadata.histogramStats.regionHistograms);
                    BufHis1.Stamp();
                    pPortHis1->PutBuffer(&BufHis1);
                }
            }

            for (Idx = 0; Idx < m_iSensorNb; Idx++)
            {
                // for Q360_PROJECT, other product will do nothing
                m_Cam[Idx].pCamera->DynamicChangeModuleParam();

                ret = m_Cam[Idx].pCamera->DynamicChangeIspParameters();
                if (IMG_SUCCESS !=ret ) {
                    LOGE("failed to call DynamicChangeIspParameters().\n");
                    throw VBERR_RUNTIME;
                    return ;
                }
            }

            ret = m_Cam[0].pCamera->DynamicChange3DDenoiseIdx(ui32DnTargetIdx);
            if (IMG_SUCCESS !=ret ) {
                LOGE("failed to call DynamicChange3DDenoiseIdx().\n");
                throw VBERR_RUNTIME;
                return ;
            }

            m_pstPrvData[m_iBufferID].iDnID = ui32DnTargetIdx;
            pthread_mutex_lock(&mutex_x);
            m_pstPrvData[m_iBufferID].dns_3d_attr = dns_3d_attr;
            m_iBufUseCnt++;
            pthread_mutex_unlock(&mutex_x);
            pPortOut->PutBuffer(&BufDst);


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

int IPU_V2500::InitCamera(Json *js)
{
    char tmp[128];
    IMG_UINT32 Idx = 0;
    IMG_UINT32 nExist = 0;
    char sn_name[MAX_CAM_NB][64];
    char idx_sensor[MAX_CAM_NB][64];
    int mode[MAX_CAM_NB];
    int which[MAX_CAM_NB];
    char dev_nm[64];
    char path[128];
    int fd = 0;

    memset((void *)mode, 0, sizeof(mode));
    for (Idx = 0; Idx < MAX_CAM_NB; Idx++)
    {
        m_Cam[Idx].pCamera = NULL;
        m_Cam[Idx].pAE = NULL;
        m_Cam[Idx].pDNS = NULL;
        m_Cam[Idx].pAWB = NULL;
        m_Cam[Idx].pTNM = NULL;
        m_Cam[Idx].pCMC = NULL;
        m_Cam[Idx].SensorMode = 0;
        memset(m_Cam[Idx].szSensorName, 0, sizeof(m_Cam[Idx].szSensorName));
        memset(m_Cam[Idx].szSetupFile, 0, sizeof(m_Cam[Idx].szSetupFile));
        m_Cam[Idx].Exslevel = 0;
        m_Cam[Idx].Scenario = 0;
        m_Cam[Idx].WBMode = 0;
        m_Cam[Idx].flDefTargetBrightness = 0.0;

        sprintf(tmp, "%s%d.%s", "sensor", Idx, "name");
       // printf("tmp = %s\n", tmp);
        if (item_exist(tmp))
        {
            item_string(sn_name[nExist], tmp, 0);
            sprintf(tmp, "%s%d", "sensor", Idx);
            memcpy(idx_sensor[nExist], tmp, sizeof(idx_sensor[nExist]));
            which[nExist] = Idx;
            sprintf(tmp, "%s%d.%s", "sensor", Idx, "mode");
            if(item_exist(tmp))
            {
                mode[nExist] = item_integer(tmp, 0);
            }
            nExist++;
        }

    }

    Idx = 0;
    while(Idx < nExist)
    {
        m_Cam[Idx].Context = Idx;

        memcpy(m_Cam[Idx].szSensorName, sn_name[Idx], sizeof(sn_name[Idx]));

        memset(dev_nm, 0, sizeof(dev_nm));
        memset(path, 0, sizeof(path));
        sprintf(dev_nm, "%s%d", DEV_PATH, Idx);
        fd = open(dev_nm, O_RDWR);
        if (fd < 0) {
            LOGE("open %s error\n", dev_nm);
        } else {
            ioctl(fd, GETISPPATH, path);
            close(fd);
        }

#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
        //Overwrite isp-config.txt with tmp/sensor0-isp-config.txt, add by BU2 for IQ Tunning Tool
        memset((void *)tmp, 0, sizeof(tmp));
        sprintf(tmp, "%s%d-%s", "/tmp/sensor", Idx, "isp-config.txt");
        if (access(tmp, 0) != -1) {
            LOGE("Overwrite %s ==> %s \n", path, tmp);
            strncpy(path, tmp, 128);
        } else {
            LOGE("Cannot access  %s\n", tmp);
        }

#endif //#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
        if (path[0] == 0) {
            memset((void *)tmp, 0, sizeof(tmp));
            sprintf(tmp, "%s%s-%s", SETUP_PATH, idx_sensor[Idx], "isp-config.txt");
            strncpy(m_Cam[Idx].szSetupFile, tmp, sizeof(m_Cam[Idx].szSetupFile) - 1);
        } else {
            strncpy(m_Cam[Idx].szSetupFile, path, sizeof(m_Cam[Idx].szSetupFile) - 1);
        }

        if (NULL != js )
        {

            m_Cam[Idx].pCamera = new ISPC::Camera(m_Cam[Idx].Context, ISPC::Sensor::GetSensorId(m_Cam[Idx].szSensorName), mode[Idx],
                                                js->GetObject("flip") ? js->GetInt("flip") : 0, which[Idx]);
        }
        else
        {
            m_Cam[Idx].pCamera = new ISPC::Camera(m_Cam[Idx].Context, ISPC::Sensor::GetSensorId(m_Cam[Idx].szSensorName), mode[Idx], 0, which[Idx]);
        }
        if (!m_Cam[Idx].pCamera)
        {
            LOGE("new a camera instance failed\n.");
            return -1;
        }
        if (ISPC::CAM_ERROR == m_Cam[Idx].pCamera->state)
        {
            LOGE("failed to create a camera instance\n");
            return -1;
        }

        m_CurrentStatus = CAM_CREATE_SUCCESS;
        if (!m_Cam[Idx].pCamera->bOwnSensor)
        {
            m_CurrentStatus = V2500_NONE;
            LOGE("Camera own a sensor failed\n");
            return -1;
        }
        ISPC::CameraFactory::populateCameraFromHWVersion(*m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->getSensor());

        m_Cam[Idx].pstShotFrame = (ISPC::Shot*)malloc(m_nBuffers * sizeof(ISPC::Shot));
        if (m_Cam[Idx].pstShotFrame == NULL)
        {
            LOGE("camera allocate Shot Frame buffer failed\n");
            return -1;
        }
        memset((char*)m_Cam[Idx].pstShotFrame, 0, m_nBuffers * sizeof(ISPC::Shot));
        Idx++;

    }

    m_iSensorNb = Idx;
    LOGE("(%s:L%d) m_iSensorNb=%d \n", __func__, __LINE__, m_iSensorNb);
    if (0 == m_iSensorNb)
        return -1;
    return 0;
}

IPU_V2500::IPU_V2500(std::string name, Json *js)
{
    int ret = 0;


    Name = name;
    m_nBuffers = 3;
    m_CurrentStatus = V2500_NONE;
    m_RunSt = CAM_RUN_ST_PREVIEWING; //default
    m_iSensorNb = 0;
    m_iCurCamIdx = 0;
	this->ui32SkipFrame = 0;

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


    ret = InitCamera(js);
    if (ret)
    {
        DeinitCamera();
        throw VBERR_RUNTIME;
        return;
    }

    BuildPort(1920, 1088*m_iSensorNb);

    m_iBufferID = 0;
    m_iBufUseCnt = 0;
    m_CurrentStatus = SENSOR_CREATE_SUCESS;
#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
    m_bIqTuningDebugInfoEnable = false;
#endif //#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
}

int IPU_V2500::ConfigAutoControl(IMG_UINT32 Idx)
{
    if (m_Cam[Idx].pAE)
    {
        m_Cam[Idx].pAE->enableFlickerRejection(true);
    }

    if (m_Cam[Idx].pTNM)
    {
        m_Cam[Idx].pTNM->enableControl(true);
        m_Cam[Idx].pTNM->enableLocalTNM(true);
        if (!m_Cam[Idx].pAE || !m_Cam[Idx].pAE->isEnabled())
        {
            m_Cam[Idx].pTNM->setAllowHISConfig(true);
        }
    }

    if (m_Cam[Idx].pDNS)
    {
        m_Cam[Idx].pDNS->enableControl(true);
    }

    if (m_Cam[Idx].pAWB)
    {
        m_Cam[Idx].pAWB->enableControl(true);
    }

    return IMG_SUCCESS;
}

int IPU_V2500::AutoControl(IMG_UINT32 Idx)
{
    int ret = IMG_SUCCESS;

    if (!m_Cam[Idx].pAE)
    {
        m_Cam[Idx].pAE = new ISPC::ControlAE();
        ret = m_Cam[Idx].pCamera->registerControlModule(m_Cam[Idx].pAE);
		if (m_iSensorNb > 1)
			m_Cam[Idx].pAE->addPipeline(m_Cam[(Idx == 0 ? 1 : 0)].pCamera->getPipeline());

        if(IMG_SUCCESS != ret)
        {
            LOGE("register AE module failed\n");
            delete m_Cam[Idx].pAE;
            m_Cam[Idx].pAE = NULL;
        }
    }
    if (!m_Cam[Idx].pAWB)
    {
        m_Cam[Idx].pAWB = new ISPC::ControlAWB_PID();
        ret = m_Cam[Idx].pCamera->registerControlModule(m_Cam[Idx].pAWB);
        if (m_iSensorNb > 1)
            m_Cam[Idx].pAWB->addPipeline(m_Cam[(Idx == 0 ? 1 : 0)].pCamera->getPipeline());

        if(IMG_SUCCESS != ret)
        {
            LOGE("register AWB module failed\n");
            delete m_Cam[Idx].pAWB;
            m_Cam[Idx].pAWB = NULL;
        }
    }
    if (!m_Cam[Idx].pTNM)
    {
        m_Cam[Idx].pTNM = new ISPC::ControlTNM();
        ret = m_Cam[Idx].pCamera->registerControlModule(m_Cam[Idx].pTNM);
        if (m_iSensorNb > 1)
            m_Cam[Idx].pTNM->addPipeline(m_Cam[(Idx == 0 ? 1 : 0)].pCamera->getPipeline());

        if(IMG_SUCCESS != ret)
        {
            LOGE("register TNM module failed\n");
            delete m_Cam[Idx].pTNM;
            m_Cam[Idx].pTNM = NULL;
        }
    }
    if (!m_Cam[Idx].pDNS)
    {
        m_Cam[Idx].pDNS = new ISPC::ControlDNS();
        ret = m_Cam[Idx].pCamera->registerControlModule(m_Cam[Idx].pDNS);
        if (m_iSensorNb > 1)
                m_Cam[Idx].pDNS->addPipeline(m_Cam[(Idx == 0 ? 1 : 0)].pCamera->getPipeline());

        if(IMG_SUCCESS != ret)
        {
            LOGE("register DNS module failed\n");
            delete m_Cam[Idx].pDNS;
            m_Cam[Idx].pDNS = NULL;
        }
    }
    if (!m_Cam[Idx].pCMC)
    {
        m_Cam[Idx].pCMC = new ISPC::ControlCMC();
        ret = m_Cam[Idx].pCamera->registerControlModule(m_Cam[Idx].pCMC);
        if(IMG_SUCCESS != ret)
        {
            LOGE("register m_Cam[Idx].pCMC module failed\n");
            delete m_Cam[Idx].pCMC;
            m_Cam[Idx].pCMC = NULL;
        }
    }
    return ret;
}

int IPU_V2500::SetResolution(IMG_UINT32 Idx, int imgW, int imgH)
{
    m_ImgW = imgW;
    m_ImgH = imgH;
    SetEncScaler(Idx, imgW, imgH);
    return IMG_SUCCESS;
}

//1 Sepia color
int IPU_V2500::SetSepiaMode(IMG_UINT32 Idx, int mode)
{
    if (m_Cam[Idx].pCamera)
    {
        m_Cam[Idx].pCamera->setSepiaMode(mode);
    }

    return IMG_SUCCESS;
}

int IPU_V2500::SetScenario(IMG_UINT32 Idx, int sn)
{
    m_Cam[Idx].Scenario = sn;
    ISPC::ModuleR2Y *pR2Y = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
    //ISPC::ModuleTNM *pTNM = pCamera->pipeline->getModule<ISPC::ModuleTNM>();
    ISPC::ModuleSHA *pSHA = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleSHA>();
    ISPC::ControlCMC* pCMC = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlCMC>();
    //ISPC::ControlTNM* pTNMC = pCamera->getControlModule<ISPC::ControlTNM>();
    IMG_UINT32 ui32DnTargetIdx;

    //sn =1 low light scenario, else hight light scenario
    if (m_Cam[Idx].Scenario < 1)
    {
        FlatModeStatusSet(false);

        pR2Y->fBrightness = pCMC->fBrightness_acm;
        pR2Y->fContrast = pCMC->fContrast_acm;
        pR2Y->aRangeMult[0] = pCMC->aRangeMult_acm[0];
        pR2Y->aRangeMult[1] = pCMC->aRangeMult_acm[1];
        pR2Y->aRangeMult[2] = pCMC->aRangeMult_acm[2];

        m_Cam[Idx].pAE->setTargetBrightness(pCMC->targetBrightness_acm);
        m_Cam[Idx].pCamera->sensor->setMaxGain(pCMC->fSensorMaxGain_acm);

        pR2Y->fOffsetU = 0;
        pR2Y->fOffsetV = 0;
    }
    else
    {
        FlatModeStatusSet(true);

        pR2Y->fBrightness = pCMC->fBrightness_fcm;
        pR2Y->fContrast = pCMC->fContrast_fcm;
        pR2Y->aRangeMult[0] = pCMC->aRangeMult_fcm[0];
        pR2Y->aRangeMult[1] = pCMC->aRangeMult_fcm[1];
        pR2Y->aRangeMult[2] = pCMC->aRangeMult_fcm[2];

        m_Cam[Idx].pAE->setTargetBrightness(pCMC->targetBrightness_fcm);
        m_Cam[Idx].pCamera->sensor->setMaxGain(pCMC->fSensorMaxGain_fcm);

        pR2Y->fOffsetU = 0;
        pR2Y->fOffsetV = 0;
    }

    m_Cam[Idx].pCamera->DynamicChange3DDenoiseIdx(ui32DnTargetIdx, true);
    m_Cam[Idx].pCamera->DynamicChangeIspParameters(true);

    //pTNM->requestUpdate();
    pR2Y->requestUpdate();
    pSHA->requestUpdate();

    return IMG_SUCCESS;
}

int IPU_V2500::CheckOutRes(IMG_UINT32 Idx, int imgW, int imgH)
{
    if (imgW % 64)
    {
        return -1;
    }
    MC_PIPELINE *pMCPipeline = NULL;

    pMCPipeline = m_Cam[Idx].pCamera->pipeline->getMCPipeline();
    m_InW = pMCPipeline->sIIF.ui16ImagerSize[0] * CI_CFA_WIDTH;
    m_InH = pMCPipeline->sIIF.ui16ImagerSize[1] * CI_CFA_HEIGHT;

    LOGD("LockRatio is setting : %d \n", m_LockRatio);
    if (((imgW < m_InW - m_PipX) && (imgH < m_InH - m_PipY)) && (m_LockRatio))
    {
        if (imgH < imgW)
        {
            m_ImgW = imgH * ((float)m_InW / m_InH);
        }
        else
        {
            m_ImgH = imgW * ((float)m_InH / m_InW);
        }
    }

    LOGE("m_InW=%d, m_InH=%d, m_ImgW=%d, m_ImgH=%d\n", m_InW, m_InH, m_ImgW, m_ImgH);

    return 0;
}

void IPU_V2500::Prepare()
{
    // allocate needed memory(reference frame/ snapshots, etc)
    // initialize the hardware
    // get input parameters from PortIn.Width/Height/PixelFormat

    ISPC::ModuleGMA *pGMA;
    Port *pPortOut = NULL;
    IMG_UINT32 Idx = 0;
    ISPC::ParameterList parameters;
    ISPC::ParameterFileParser fileParser;
    cam_fps_range_t fps_range;
    Pixel::Format enPixelFormat;

    pPortOut = GetPort("out");
    enPixelFormat = pPortOut->GetPixelFormat();
    if(enPixelFormat != Pixel::Format::NV12 && enPixelFormat != Pixel::Format::NV21
        && enPixelFormat != Pixel::Format::RGBA8888)
    {
        LOGE("Out Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }
    enPixelFormat = GetPort("cap")->GetPixelFormat();
    if(enPixelFormat != Pixel::Format::NV12 && enPixelFormat != Pixel::Format::NV21
        && enPixelFormat != Pixel::Format::RGBA8888)
    {
        LOGE("cap Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }
    enPixelFormat = GetPort("his")->GetPixelFormat();
    if(enPixelFormat != Pixel::Format::VAM)
    {
        LOGE("his Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }

    if (pPortOut)
    {
        m_ImgW = pPortOut->GetWidth();
        m_ImgH = pPortOut->GetHeight();
        m_PipX = pPortOut->GetPipX();
        m_PipY = pPortOut->GetPipY();
        m_PipW = pPortOut->GetPipWidth();
        m_PipH = pPortOut->GetPipHeight();
        m_MinFps = pPortOut->GetMinFps();
        m_MaxFps = pPortOut->GetMaxFps();
    }

    m_pFun = std::bind(&IPU_V2500::ReturnShot,this,std::placeholders::_1);
    pPortOut->frBuffer->RegCbFunc(m_pFun);


    for (Idx = 0; Idx < m_iSensorNb; Idx++)
    {
        m_Cam[Idx].pCamera->getPipeline()->getCIPipeline()->uiTotalPipeline = m_iSensorNb;

        AutoControl(Idx);

        parameters = fileParser.parseFile(m_Cam[Idx].szSetupFile);
        if (!parameters.validFlag)
        {
            LOGE("failed to get parameters from setup file.\n");
            throw VBERR_BADPARAM;
        }
        if (m_Cam[Idx].pCamera->loadParameters(parameters) != IMG_SUCCESS)
        {
            LOGE("failed to load camera configuration.\n");
            throw VBERR_BADPARAM;
        }
#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
        BackupIspParameters(Idx, m_Parameters[Idx]);
#endif //#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
        m_Cam[Idx].flDefTargetBrightness = m_Cam[Idx].pAE->getOriTargetBrightness();
        if (CheckOutRes(Idx, m_ImgW, m_ImgH) < 0)
        {
            LOGE("ISP output width size is not 64 align\n");
            throw VBERR_RUNTIME;
        }

        if (0 != m_PipW && 0 != m_PipH)
        {
            SetIIFCrop(Idx, m_PipH, m_PipW, m_PipX, m_PipY);
        }

        if (m_ImgW <= m_InW && m_ImgH <= m_InH)
        {
            SetEncScaler(Idx, m_ImgW, m_ImgH);//Set output scaler by current port resolution;
        }
        else
        {
            SetEncScaler(Idx, m_InW, m_InH);
        }

#ifdef COMPILE_IQ_REALTIME_SAVE_RAW
        SetModuleOUTRawFlag(Idx, true, m_eDataExtractionPoint); //for allocating raw buffer in allocateBufferPool function
#endif
        if (m_Cam[Idx].pCamera->setupModules() != IMG_SUCCESS)
        {
            LOGE("failed to setup camera\n");
            throw VBERR_RUNTIME;
        }
        LOGE("in start method : H = %d, W = %d\n", m_ImgH, m_ImgW);
        m_Cam[Idx].pCamera->setEncoderDimensions(m_ImgW, m_ImgH);
        if (m_Cam[Idx].pCamera->program() != IMG_SUCCESS)
        {
            LOGE("failed to program camera %d\n", __LINE__);
            throw VBERR_RUNTIME;
        }

        pGMA = m_Cam[Idx].pCamera->getModule<ISPC::ModuleGMA>();
        if (pGMA != NULL)
        {
            if (pGMA->useCustomGam)
            {
                if (SetGAMCurve(Idx, NULL) != IMG_SUCCESS)
                {
                    LOGE("failed to set gam curve %d\n", __LINE__);
                    throw VBERR_RUNTIME;
                }
            }
        }
        if (m_Cam[Idx].BuffersIDs.size() > 0)
        {
            LOGE(" Buffer all exist, camera start before.\n");
            throw VBERR_BADLOGIC;
        }

        if (m_Cam[Idx].pCamera->allocateBufferPool(m_nBuffers, m_Cam[Idx].BuffersIDs) != IMG_SUCCESS)
        {
            LOGE("camera allocate buffer poll failed\n");
            throw VBERR_RUNTIME;
        }
    }

    for (Idx = 0; Idx < m_iSensorNb; Idx++)
    {
        if (m_Cam[Idx].pCamera->startCapture() != IMG_SUCCESS)
        {
            LOGE("start capture failed.\n");
            throw VBERR_RUNTIME;
        }

        if (m_MinFps == 0 && m_MaxFps == 0) {
            // nothing to do
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

        //m_Cam[Idx].pCamera->sensor->setFlipMirror(SENSOR_FLIP_HORIZONTAL/*|SENSOR_FLIP_VERTICAL*/);
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
    LOGD("(%s:L%d) end \n", __func__, __LINE__);
}

void IPU_V2500::Unprepare()
{
    // stop hardwarePrepare()
    // free allocated memory
    IMG_UINT32 Idx = 0;
    unsigned int i = 0;
    Port *pIpuPort = NULL;

    ISPC::Shot tmp_shot;
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
                m_Cam[Idx].pCamera->releaseShot(m_Cam[Idx].pstShotFrame[bufID]);
                m_Cam[Idx].pstShotFrame[bufID].YUV.phyAddr = 0;
            }
        }
    }

    for (Idx = 0; Idx < m_iSensorNb; Idx++)
    {
        // Shot is enqueued, but not aquire & release
        for (i = 0; i < (m_nBuffers - iShotListSize); i++)
        {
            if (m_Cam[Idx].pCamera->acquireShot(tmp_shot) != IMG_SUCCESS)
            {
                LOGE("acqureshot:%d before stopping failed: %d\n", i+1, __FILE__);
                throw VBERR_RUNTIME;
            }

            m_Cam[Idx].pCamera->releaseShot(tmp_shot);
            LOGD("in unprepare release shot i =%d, Idx = %d\n", i, Idx);
        }
        if (m_Cam[Idx].pCamera->stopCapture(m_RunSt - 1) != IMG_SUCCESS)
        {
            LOGE("failed to stop the capture.\n");
            throw VBERR_RUNTIME;
        }
        m_Cam[Idx].pCamera->deleteShots();
        for (it = m_Cam[Idx].BuffersIDs.begin(); it != m_Cam[Idx].BuffersIDs.end(); it++)
        {
            LOGE("in unprepare free buffer id = %d\n", *it);
            if (IMG_SUCCESS != m_Cam[Idx].pCamera->deregisterBuffer(*it))
            {
                throw VBERR_RUNTIME;
            }
        }
        m_Cam[Idx].BuffersIDs.clear();
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

void IPU_V2500::BuildPort(int imgW, int imgH)
{

    Port *pPortOut = NULL, *pPortHis = NULL, *pPortCap = NULL, *pPortHis1 = NULL;
    pPortOut = CreatePort("out", Port::Out);
    pPortHis = CreatePort("his", Port::Out);
    pPortHis1 = CreatePort("his1", Port::Out);
    pPortCap = CreatePort("cap", Port::Out);
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

//set exposure level
int IPU_V2500::SetExposureLevel(IMG_UINT32 Idx, int level)
{
    double exstarget = 0;
    double cur_target = 0;
    double target_brightness = 0;


    if (level > 4 || level < -4)
    {
        LOGE("the level %d is out of the range.\n", level);
        return -1;
    }

    if (m_Cam[Idx].pAE)
    {
        m_Cam[Idx].Exslevel = level;
        exstarget = (double)level / 4.0;
        cur_target = m_Cam[Idx].flDefTargetBrightness;

        target_brightness = cur_target + exstarget;
        if (target_brightness > 1.0)
            target_brightness = 1.0;
        if (target_brightness < -1.0)
            target_brightness = -1.0;

        m_Cam[Idx].pAE->setOriTargetBrightness(target_brightness);
    }
    else
    {
        LOGE("the AE is not register\n");
        return -1;
    }
    return IMG_SUCCESS;
}

int IPU_V2500::GetExposureLevel(IMG_UINT32 Idx, int &level)
{
    level = m_Cam[Idx].Exslevel;
    return IMG_SUCCESS;
}

int  IPU_V2500::SetAntiFlicker(IMG_UINT32 Idx, int freq)
{
    if (m_Cam[Idx].pAE)
    {
        m_Cam[Idx].pAE->enableFlickerRejection(true, freq);
    }

    return IMG_SUCCESS;
}

int  IPU_V2500::GetAntiFlicker(IMG_UINT32 Idx, int &freq)
{
    if (m_Cam[Idx].pAE)
    {
        freq = m_Cam[Idx].pAE->getFlickerRejectionFrequency();
    }

    return IMG_SUCCESS;
}

int IPU_V2500::SetEncScaler(IMG_UINT32 Idx, int imgW, int imgH)
{
    ISPC::ModuleESC *pESC= NULL;
    MC_PIPELINE *pMCPipeline = NULL;
    IMG_UINT32 owidth, oheight;
    double esc_pitch[2];
    ISPC::ScalerRectType format;
    IMG_INT32 rect[4];
    m_ImgH = imgH;
    m_ImgW = imgW;
    //set isp output encoder scaler

    if (m_Cam[Idx].pCamera)
    {
        pESC = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleESC>();
        pMCPipeline = m_Cam[Idx].pCamera->pipeline->getMCPipeline();
        if ( NULL != pESC && NULL != pMCPipeline)
        {
            owidth = (pMCPipeline->sIIF.ui16ImagerSize[0] * CI_CFA_WIDTH);
            oheight = (pMCPipeline->sIIF.ui16ImagerSize[1] * CI_CFA_HEIGHT);
            LOGD("cfa widht = %d, cfa height= %d\n", pMCPipeline->sIIF.ui16ImagerSize[0], pMCPipeline->sIIF.ui16ImagerSize[1]);
            LOGD("owidht = %d, oheight = %d\n", owidth, oheight);
            esc_pitch[0] = (float)imgW / owidth;
            esc_pitch[1] = (float)imgH / oheight;
            LOGD("esc_pitch[0] =%f, esc_pitch[1] = %f\n", esc_pitch[0], esc_pitch[1]);
            if (esc_pitch[0] <= 1.0 && esc_pitch[1] <= 1.0)
            {
                pESC->aPitch[0] = esc_pitch[0];
                pESC->aPitch[1] = esc_pitch[1];
                pESC->aRect[0] = rect[0] = 0; //offset x
                pESC->aRect[1] = rect[1] = 0; //offset y
                pESC->aRect[2] = rect[2] = imgW;
                pESC->aRect[3] = rect[3] = imgH;
                pESC->eRectType = format = ISPC::SCALER_RECT_SIZE;
                pESC->requestUpdate();
            }
            else
            {
                LOGE("the resolution can't support!\n");
                return -1;
            }

        }
    }

    //BuildPort(m_ImgH, m_ImgW);
    return IMG_SUCCESS;
}


int IPU_V2500::SetIIFCrop(IMG_UINT32 Idx, int PipH, int PipW, int PipX, int PipY)
{
    ISPC::ModuleIIF *pIIF= NULL;
    MC_PIPELINE *pMCPipeline = NULL;
    IMG_UINT32 owidth, oheight;
    double iif_decimation[2];
    IMG_INT32 rect[4];
    m_PipH = PipH;
    m_PipW = PipW;
    m_PipX = PipX;
    m_PipY = PipY;

    if (m_Cam[Idx].pCamera)
    {
        pIIF= m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleIIF>();
        pMCPipeline = m_Cam[Idx].pCamera->pipeline->getMCPipeline();
        if ( NULL != pIIF && NULL != pMCPipeline)
        {
            owidth = (pMCPipeline->sIIF.ui16ImagerSize[0] * CI_CFA_WIDTH);
            oheight = (pMCPipeline->sIIF.ui16ImagerSize[1] * CI_CFA_HEIGHT);
            //printf("cfa2 width = %d, cfa2 height= %d\n", pMCPipeline->sIIF.ui16ImagerSize[0], pMCPipeline->sIIF.ui16ImagerSize[1]);
            //printf("owidth = %d, oheight = %d\n", owidth, oheight);
            //printf("imgW = %d, imgH = %d\n", m_PipW, m_PipH);
            //printf("offsetx = %d, offsety = %d\n", m_PipX, m_PipY);
            iif_decimation[1] = 0;
            iif_decimation[0] = 0;
            if (((float)(m_PipX + m_PipW) / owidth) <= 1.0 && ((float)(m_PipY + m_PipH) / oheight) <= 1.0)
            {
                pIIF->aDecimation[0] = iif_decimation[0];
                pIIF->aDecimation[1] = iif_decimation[1];
                pIIF->aCropTL[0] = rect[0] = m_PipX; //offset x
                pIIF->aCropTL[1] = rect[1] = m_PipY; //offset y
                pIIF->aCropBR[0] = rect[2] = m_PipW + m_PipX - 2;
                pIIF->aCropBR[1] = rect[3] = m_PipH + m_PipY - 2;
                pIIF->requestUpdate();
            }
            else
            {
                LOGE("the resolution can't support!\n");
                return -1;
            }
        }
    }

    //BuildPort(m_ImgH, m_ImgW);
    return IMG_SUCCESS;
}


int IPU_V2500::SetWB(IMG_UINT32 Idx, int mode)
{
    ISPC::ModuleCCM *pCCM = m_Cam[Idx].pCamera->getModule<ISPC::ModuleCCM>();
    ISPC::ModuleWBC *pWBC = m_Cam[Idx].pCamera->getModule<ISPC::ModuleWBC>();
    const ISPC::TemperatureCorrection &tmpccm = m_Cam[Idx].pAWB->getTemperatureCorrections();
    m_Cam[Idx].WBMode = mode;
    if (mode  == 0)
    {
        EnableAWB(Idx, true);
    }
    else if (mode > 0 && mode -1 < tmpccm.size())
    {

        EnableAWB(Idx, false);
        ISPC::ColorCorrection ccm = tmpccm.getCorrection(mode - 1);
        for (int i=0;i<3;i++)
        {
            for (int j=0;j<3;j++)
            {
                pCCM->aMatrix[i*3+j] = ccm.coefficients[i][j];
            }
        }

        //apply correction offset
        pCCM->aOffset[0] = ccm.offsets[0][0];
        pCCM->aOffset[1] = ccm.offsets[0][1];
        pCCM->aOffset[2] = ccm.offsets[0][2];

        pCCM->requestUpdate();

        pWBC->aWBGain[0] = ccm.gains[0][0];
        pWBC->aWBGain[1] = ccm.gains[0][1];
        pWBC->aWBGain[2] = ccm.gains[0][2];
        pWBC->aWBGain[3] = ccm.gains[0][3];

        pWBC->requestUpdate();

    }
    else
    {
        LOGE("(%s,L%d) The mode %d is not supported!\n", __FUNCTION__, __LINE__, mode);
        return -1;
    }

    return IMG_SUCCESS;
}

int IPU_V2500::GetWB(IMG_UINT32 Idx, int &mode)
{
    mode = m_Cam[Idx].WBMode;

    return 0;
}

int IPU_V2500::GetYUVBrightness(IMG_UINT32 Idx, int &out_brightness)
{
    int ret = 0;
    ISPC::ModuleR2Y *pR2Y = NULL;
    ISPC::ControlTNM *pTNMC = NULL;
    double light = 0.0;
    bool bEnable = false;


    pTNMC = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlTNM>();
    if (pTNMC)
    {
        bEnable = pTNMC->isTnmCrvSimBrightnessContrastEnabled();
    }
    if (pTNMC && bEnable)
    {
        light = pTNMC->getTnmCrvBrightness();
        ret = CalcHSBCToOut(pTNMC->TNMC_CRV_BRIGNTNESS.max, pTNMC->TNMC_CRV_BRIGNTNESS.min, light, out_brightness);
    }
    else
    {
        LOGE("(%s,L%d) pTNMC is NULL error or system disabled using TNM curve to simulation brightness!\n", __FUNCTION__, __LINE__);
        pR2Y = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
        if (NULL == pR2Y)
        {
            LOGE("(%s,L%d) pR2Y is NULL error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
        light = pR2Y->fBrightness;
        ret = CalcHSBCToOut(pR2Y->R2Y_BRIGHTNESS.max, pR2Y->R2Y_BRIGHTNESS.min, light, out_brightness);
    }
    if (ret)
        return ret;

    return IMG_SUCCESS;
}

int IPU_V2500::SetYUVBrightness(IMG_UINT32 Idx, int brightness)
{
    int ret = 0;
    ISPC::ModuleR2Y *pR2Y = NULL;
    ISPC::ControlTNM *pTNMC = NULL;
    double light = 0.0;
    bool bEnable = false;


    ret = CheckHSBCInputParam(brightness);
    if (ret)
        return ret;

    pTNMC = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlTNM>();
    if (pTNMC)
    {
        bEnable = pTNMC->isTnmCrvSimBrightnessContrastEnabled();
    }
    if (pTNMC && bEnable)
    {
        ret = CalcHSBCToISP(pTNMC->TNMC_CRV_BRIGNTNESS.max, pTNMC->TNMC_CRV_BRIGNTNESS.min, brightness, light);
        if (ret)
            return ret;

        pTNMC->setTnmCrvBrightness(light);
    }
    else
    {
        LOGE("(%s,L%d) pTNMC is NULL error or system disabled using TNM curve to simulation brightness!\n", __FUNCTION__, __LINE__);
        pR2Y = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
        if (NULL == pR2Y)
        {
            LOGE("(%s,L%d) pR2Y is NULL error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
        ret = CalcHSBCToISP(pR2Y->R2Y_BRIGHTNESS.max, pR2Y->R2Y_BRIGHTNESS.min, brightness, light);
        if (ret)
            return ret;

        pR2Y->fBrightness = light;
        pR2Y->requestUpdate();
    }

    return IMG_SUCCESS;
}

int IPU_V2500::GetContrast(IMG_UINT32 Idx, int &contrast)
{
    int ret = 0;
    ISPC::ModuleR2Y *pR2Y = NULL;
    ISPC::ControlTNM *pTNMC = NULL;
    double cst = 0.0;
    bool bEnable = false;


    pTNMC = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlTNM>();
    if (pTNMC)
    {
        bEnable = pTNMC->isTnmCrvSimBrightnessContrastEnabled();
    }
    if (pTNMC && bEnable)
    {
        cst = pTNMC->getTnmCrvContrast();
        ret = CalcHSBCToOut(pTNMC->TNMC_CRV_CONTRAST.max, pTNMC->TNMC_CRV_CONTRAST.min, cst, contrast);
    }
    else
    {
        LOGE("(%s,L%d) pTNMC is NULL error or system disabled using TNM curve to simulation contrast!\n", __FUNCTION__, __LINE__);
        pR2Y = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
        if (NULL == pR2Y)
        {
            LOGE("(%s,L%d) pR2Y is NULL error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
        cst = pR2Y->fContrast;
        ret = CalcHSBCToOut(pR2Y->R2Y_CONTRAST.max, pR2Y->R2Y_CONTRAST.min, cst, contrast);
    }
    if (ret)
        return ret;

    return IMG_SUCCESS;
}

int IPU_V2500::SetContrast(IMG_UINT32 Idx, int contrast)
{
    int ret = 0;
    ISPC::ModuleR2Y *pR2Y = NULL;
    ISPC::ControlTNM *pTNMC = NULL;
    double cst = 0.0;
    bool bEnable = false;


    ret = CheckHSBCInputParam(contrast);
    if (ret)
        return ret;

    pTNMC = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlTNM>();
    if (pTNMC)
    {
        bEnable = pTNMC->isTnmCrvSimBrightnessContrastEnabled();
    }
    if (pTNMC && bEnable)
    {
        ret = CalcHSBCToISP(pTNMC->TNMC_CRV_CONTRAST.max, pTNMC->TNMC_CRV_CONTRAST.min, contrast, cst);
        if (ret)
            return ret;

        pTNMC->setTnmCrvContrast(cst);
    }
    else
    {
        LOGE("(%s,L%d) pTNMC is NULL error or system disabled using TNM curve to simulation contrast!\n", __FUNCTION__, __LINE__);
        pR2Y = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
        if (NULL == pR2Y)
        {
            LOGE("(%s,L%d) pR2Y is NULL error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
        ret = CalcHSBCToISP(pR2Y->R2Y_CONTRAST.max, pR2Y->R2Y_CONTRAST.min, contrast, cst);
        if (ret)
            return ret;

        pR2Y->fContrast = cst;
        pR2Y->requestUpdate();
    }

    return IMG_SUCCESS;
}

int IPU_V2500::GetSaturation(IMG_UINT32 Idx, int &saturation)
{
    int ret = 0;
#if defined(USE_R2Y_INTERFACE)
    ISPC::ModuleR2Y *pr2y = NULL;
#else
    ISPC::ControlCMC *pCMC = NULL;
#endif
    double sat = 0.0;


#if defined(USE_R2Y_INTERFACE)
    pr2y = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
    if (NULL == pr2y)
    {
        LOGE("(%s,L%d) pR2Y is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    sat = pr2y->fSaturation;
    ret = CalcHSBCToOut(pr2y->R2Y_SATURATION.max, pr2y->R2Y_SATURATION.min, sat, saturation);
#else
    pCMC = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlCMC>();
    if (NULL == pCMC)
    {
        LOGE("(%s,L%d) pCMC is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }
    sat = pCMC->fR2YSaturationExpect;
    ret = CalcHSBCToOut(pCMC->CMC_R2Y_SATURATION_EXPECT.max, pCMC->CMC_R2Y_SATURATION_EXPECT.min, sat, saturation);
#endif

    if (ret)
        return ret;

    return IMG_SUCCESS;
}

int IPU_V2500::SetSaturation(IMG_UINT32 Idx, int saturation)
{
    int ret = 0;

#if defined(USE_R2Y_INTERFACE)
    ISPC::ModuleR2Y *pr2y = NULL;
#else
    ISPC::ControlCMC *pCMC = NULL;
#endif

    double sat = 0.0;

    ret = CheckHSBCInputParam(saturation);
    if (ret)
        return ret;

#if defined(USE_R2Y_INTERFACE)
    pR2Y = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
    if (NULL == pR2Y)
    {
        LOGE("(%s,L%d) pR2Y is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ret = CalcHSBCToISP(pr2y->R2Y_SATURATION.max, pr2y->R2Y_SATURATION.min, saturation, sat);
#else
    pCMC = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlCMC>();
    if (NULL == pCMC)
    {
        LOGE("(%s,L%d) pCMC is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }
    ret = CalcHSBCToISP(pCMC->CMC_R2Y_SATURATION_EXPECT.max, pCMC->CMC_R2Y_SATURATION_EXPECT.min, saturation, sat);
#endif

    if (ret)
        return ret;

#if defined(USE_R2Y_INTERFACE)
    pr2y->fSaturation = sat;
    pr2y->requestUpdate();
#else
    pCMC->fR2YSaturationExpect = sat;
    m_Cam[Idx].pCamera->DynamicChangeIspParameters(true);
#endif

    return IMG_SUCCESS;
}

int IPU_V2500::GetSharpness(IMG_UINT32 Idx, int &sharpness)
{
    int ret = 0;
    ISPC::ModuleSHA *psha = NULL;
    ISPC::ControlCMC* pCMC = NULL;

    double fShap = 0.0;


    psha = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleSHA>();
    if (NULL == psha)
    {
        LOGE("(%s,L%d) pSHA is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    pCMC = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlCMC>();
    if (NULL == pCMC) {
        LOGE("(%s,L%d) pCMC is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }

#if 0
    fShap = psha->fStrength;
#else
    pCMC->GetShaStrengthExpect(fShap);
#endif
    ret = CalcHSBCToOut(psha->SHA_STRENGTH.max, psha->SHA_STRENGTH.min, fShap, sharpness);

    if (ret)
        return ret;

    return IMG_SUCCESS;
}

int IPU_V2500::SetSharpness(IMG_UINT32 Idx, int sharpness)
{
    int ret = 0;

    ISPC::ModuleSHA *psha = NULL;
    ISPC::ControlCMC* pCMC = NULL;

    double shap = 0.0;


    ret = CheckHSBCInputParam(sharpness);
    if (ret)
        return ret;

    psha = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleSHA>();
    if (NULL == psha)
    {
        LOGE("(%s,L%d) pSHA is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    pCMC = m_Cam[Idx].pCamera->getControlModule<ISPC::ControlCMC>();
    if (NULL == pCMC) {
        LOGE("(%s,L%d) pCMC is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ret = CalcHSBCToISP(psha->SHA_STRENGTH.max, psha->SHA_STRENGTH.min, sharpness, shap);
    if (ret)
        return ret;

#if 0
    psha->fStrength = shap;
    psha->requestUpdate();
#else
    pCMC->SetShaStrengthExpect(shap);
#endif

    return IMG_SUCCESS;
}

int IPU_V2500::SetFPS(IMG_UINT32 Idx, int fps)
{

    if (fps < 0)
    {
        if (IsAEEnabled(Idx) && m_Cam[Idx].pAE)
            m_Cam[Idx].pAE->setAeExposureMethod(ISPC::ControlAE::AEMHD_EXP_LOWLUX);
    }
    else
    {
        if (IsAEEnabled(Idx) && m_Cam[Idx].pAE)
            m_Cam[Idx].pAE->setAeExposureMethod(ISPC::ControlAE::AEMHD_EXP_DEFAULT);

        m_Cam[Idx].pCamera->sensor->SetFPS(fps);
        this->GetOwner()->SyncPathFps(fps);
    }

    return 0;
}

int IPU_V2500::GetFPS(IMG_UINT32 Idx, int &fps)
{
    SENSOR_STATUS status;

    if (m_Cam[Idx].pCamera)
    {
        m_Cam[Idx].pCamera->sensor->GetFlipMirror(status);
        fps = status.fCurrentFps;
        return 0;
    }
    else
    {
        return -1;
    }
}

int IPU_V2500::GetScenario(IMG_UINT32 Idx, enum cam_scenario &scen)
{
    scen = (enum cam_scenario)m_Cam[Idx].Scenario;

    return 0;
}

int IPU_V2500::GetResolution(int &imgW, int &imgH)
{
    imgH = m_ImgH;
    imgW = m_ImgW;

    return IMG_SUCCESS;
}

//get current frame statistics lumination value in AE
// enviroment brightness value
int IPU_V2500::GetEnvBrightnessValue(IMG_UINT32 Idx, int &out_bv)
{
    double brightness = 0.0;

    if (m_Cam[Idx].pAE)
    {
        brightness = m_Cam[Idx].pAE->getCurrentBrightness();
        LOGD("brightness = %f\n", brightness);

        out_bv = (int)(brightness*127 + 127);
    }
    else
    {
        return -1;
    }
    return 0;
}

//control AE enable
int IPU_V2500::EnableAE(IMG_UINT32 Idx, int enable)
{
    if (m_Cam[Idx].pAE)
    {
        m_Cam[Idx].pAE->enableAE(enable);
        return IMG_SUCCESS;
    }
    else
    {
        return -1;
    }

}

int IPU_V2500::EnableAWB(IMG_UINT32 Idx, int enable)
{
    if(m_Cam[Idx].pAWB)
    {
        m_Cam[Idx].pAWB->enableControl(enable);
        return IMG_SUCCESS;
    }
    else
    {
        return -1;
    }

}

int IPU_V2500::IsAWBEnabled(IMG_UINT32 Idx)
{
    if (m_Cam[Idx].pAWB)
    {
        return m_Cam[Idx].pAWB->IsAWBenabled();
    }
    else
    {
        return 0;
    }
}

int IPU_V2500::IsAEEnabled(IMG_UINT32 Idx)
{
    if (m_Cam[Idx].pAE)
    {
        return m_Cam[Idx].pAE->IsAEenabled();
    }
    else
    {
        return 0;
    }

}

int IPU_V2500::IsMonchromeEnabled(IMG_UINT32 Idx)
{
    return m_Cam[Idx].pCamera->IsMonoMode();
}

int IPU_V2500::SetSensorISO(IMG_UINT32 Idx,int iso)
{
    double fiso = 0.0;
    int ret = -1;
    double max_gain = 0.0;
    double min_gain = 0.0;
    bool isAeEnabled = false;

    ret = CheckHSBCInputParam(iso);
    if (ret)
        return ret;

    isAeEnabled = IsAEEnabled(Idx);
    min_gain = m_Cam[Idx].pCamera->sensor->getMinGain();
    if (m_Cam[Idx].pAE && isAeEnabled)
    {
        max_gain = m_Cam[Idx].pAE->getMaxOrgAeGain();
        if (iso > 0)
        {
            fiso = (double)iso;
            if (fiso < min_gain)
                fiso = min_gain;
            if (fiso > max_gain)
                fiso = max_gain;
            m_Cam[Idx].pAE->setMaxManualAeGain(fiso, true);
        }
        else if (0 == iso)
        {
            m_Cam[Idx].pAE->setMaxManualAeGain(max_gain, false);
        }
    }
    else
    {
        ret = CheckSensorClass(Idx);
        if (ret)
            return ret;

        max_gain = m_Cam[Idx].pCamera->sensor->getMaxGain();
        min_gain = m_Cam[Idx].pCamera->sensor->getMinGain();
        fiso = (double)iso;
        if (fiso < min_gain)
            fiso = min_gain;
        if (fiso > max_gain)
            fiso = max_gain;
        if (iso > 0)
            m_Cam[Idx].pCamera->sensor->setGain(fiso);
    }
    LOGD("%s maxgain=%f, mingain=%f fiso=%f\n", __func__, max_gain, min_gain, fiso);

    return IMG_SUCCESS;
}

int IPU_V2500::GetSensorISO(IMG_UINT32 Idx, int &iso)
{
    int ret = 0;
    bool isAeEnabled = false;

    ret = CheckSensorClass(Idx);
    if (ret)
        return ret;

    isAeEnabled = IsAEEnabled(Idx);
    if (m_Cam[Idx].pAE && isAeEnabled)
    {
        iso = (int)m_Cam[Idx].pAE->getMaxManualAeGain();
    } else {
        iso = (int)m_Cam[Idx].pCamera->sensor->getGain();
    }

    return IMG_SUCCESS;
}

//control monochrome
int IPU_V2500::EnableMonochrome(IMG_UINT32 Idx, int enable)
{
    m_Cam[Idx].pCamera->updateTNMSaturation(enable?0:1);
    return 0;
}

int IPU_V2500::EnableWDR(IMG_UINT32 Idx, int enable)
{
    if (m_Cam[Idx].pTNM)
    {
        //m_Cam[Idx].pTNM->enableAdaptiveTNM(enable);
        m_Cam[Idx].pTNM->enableLocalTNM(enable);
        return IMG_SUCCESS;
    }
    else
    {
        return -1;
    }
}

int IPU_V2500::IsWDREnabled(IMG_UINT32 Idx)
{
    if (m_Cam[Idx].pTNM)
    {
        return m_Cam[Idx].pTNM->getLocalTNM();//(m_Cam[Idx].pTNM->getAdaptiveTNM() && m_Cam[Idx].pTNM->getLocalTNM());
    }
    else
    {
        return 0;
    }
}

int IPU_V2500::GetMirror(IMG_UINT32 Idx, enum cam_mirror_state &dir)
{
    SENSOR_STATUS status;


    if (m_Cam[Idx].pCamera)
    {
        m_Cam[Idx].pCamera->sensor->GetFlipMirror(status);
        dir =  (enum cam_mirror_state)status.ui8Flipping;
        return 0;
    }
    else
    {
        return -1;
    }
}

int IPU_V2500::SetMirror(IMG_UINT32 Idx, enum cam_mirror_state dir)
{
    int flag = 0;

    switch (dir)
    {
        case cam_mirror_state::CAM_MIRROR_NONE: //0
        {
            if(m_Cam[Idx].pCamera)
            {
                flag = SENSOR_FLIP_NONE;
                m_Cam[Idx].pCamera->sensor->setFlipMirror(flag);
            }
            else
            {
                LOGE("Setting sensor mirror flip failed\n");
                return -1;
            }
            break;
        }
        case cam_mirror_state::CAM_MIRROR_H: //1
        {
            if(m_Cam[Idx].pCamera)
            {
                flag = SENSOR_FLIP_HORIZONTAL;
                m_Cam[Idx].pCamera->sensor->setFlipMirror(flag);
            }
            else
            {
                LOGE("Setting sensor mirror flip failed\n");
                return -1;
            }
            break;
        }
        case cam_mirror_state::CAM_MIRROR_V: //2
        {
            if(m_Cam[Idx].pCamera)
            {
                flag = SENSOR_FLIP_VERTICAL;
                m_Cam[Idx].pCamera->sensor->setFlipMirror(flag);
            }
            else
            {
                LOGE("Setting sensor mirror flip failed\n");
                return -1;
            }
            break;
        }
        case cam_mirror_state::CAM_MIRROR_HV:
        {
            if(m_Cam[Idx].pCamera)
            {
                flag = SENSOR_FLIP_BOTH;
                m_Cam[Idx].pCamera->sensor->setFlipMirror(flag);
            }
            else
            {
                LOGE("Setting sensor mirror flip failed\n");
                return -1;
            }
            break;
        }
        default :
        {
            LOGE("(%s,L%d) The mirror direction %d is not supported!\n", __FUNCTION__, __LINE__, dir);
            break;
            return -1;
        }

    };

    return IMG_SUCCESS;
}

int IPU_V2500::SetSensorResolution(IMG_UINT32 Idx, const res_t res)
{
    if (m_Cam[Idx].pCamera)
    {
        return m_Cam[Idx].pCamera->sensor->SetResolution(res.W, res.H);
    }
    else
    {
        return -1;
    }
}

int IPU_V2500::SetSnapResolution(IMG_UINT32 Idx, const res_t res)
{
    return SetResolution(Idx, res.W, res.H);
}

int IPU_V2500::SnapOneShot()
{
    pthread_mutex_lock(&mutex_x);
    m_RunSt = CAM_RUN_ST_CAPTURING;
    pthread_mutex_unlock(&mutex_x);
    return IMG_SUCCESS;
}

int IPU_V2500::SnapExit()
{
    pthread_mutex_lock(&mutex_x);
    m_RunSt = CAM_RUN_ST_PREVIEWING;
    pthread_mutex_unlock(&mutex_x);
    return IMG_SUCCESS;
}

int IPU_V2500::GetSnapResolution(IMG_UINT32 Idx, reslist_t *presl)
{

    reslist_t res;

    if (m_Cam[Idx].pCamera)
    {
        if (m_Cam[Idx].pCamera->sensor->GetSnapResolution(res) != IMG_SUCCESS)
        {
            LOGE("Camera get snap resolution failed\n");
            return IMG_ERROR_FATAL;
        }
    }

    if (presl != NULL)
    {
        memcpy((void *)presl, &res, sizeof(reslist_t));
    }

    return IMG_SUCCESS;
}

void IPU_V2500::DeinitCamera()
{
    IMG_UINT32 Idx = 0;

    for (Idx = 0; Idx < MAX_CAM_NB; Idx++)
    {
        if (m_Cam[Idx].pAE)
        {
            delete m_Cam[Idx].pAE;
            m_Cam[Idx].pAE = NULL;
        }
        if (m_Cam[Idx].pAWB)
        {
            delete m_Cam[Idx].pAWB;
            m_Cam[Idx].pAWB = NULL;
        }
        if (m_Cam[Idx].pTNM)
        {
            delete m_Cam[Idx].pTNM;
            m_Cam[Idx].pTNM = NULL;
        }
        if (m_Cam[Idx].pDNS)
        {
            delete m_Cam[Idx].pDNS;
            m_Cam[Idx].pDNS = NULL;
        }
        if (m_Cam[Idx].pCMC)
        {
            delete m_Cam[Idx].pCMC;
            m_Cam[Idx].pCMC = NULL;
        }
        if (m_Cam[Idx].pstShotFrame)
        {
            free(m_Cam[Idx].pstShotFrame);
            m_Cam[Idx].pstShotFrame = NULL;
        }
        if (m_Cam[Idx].pCamera)
        {
            delete m_Cam[Idx].pCamera;
            m_Cam[Idx].pCamera = NULL;
        }
    }

}

IPU_V2500::~IPU_V2500()
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

int IPU_V2500::CheckHSBCInputParam(int param)
{
    if (param < IPU_HSBC_MIN || param > IPU_HSBC_MAX)
    {
        LOGE("(%s,L%d) param %d is error!\n", __FUNCTION__, __LINE__, param);
        return -1;
    }
    return 0;
}

int IPU_V2500::CalcHSBCToISP(double max, double min, int param, double &out_val)
{
    if (max <= min)
    {
        LOGE("(%s,L%d) max %f or min %f is error!\n", __FUNCTION__, __LINE__, max, min);
        return -1;
    }

    out_val = ((double)param) * (max - min) / (IPU_HSBC_MAX - IPU_HSBC_MIN) + min;
    return 0;
}

int IPU_V2500::CalcHSBCToOut(double max, double min, double param, int &out_val)
{
    if (max <= min)
    {
        LOGE("(%s,L%d) max %f or min %f is error!\n", __FUNCTION__, __LINE__, max, min);
        return -1;
    }
    out_val = (int)floor((param - min) * (IPU_HSBC_MAX - IPU_HSBC_MIN) / (max - min) + 0.5);

    return 0;
}

int IPU_V2500::GetHue(IMG_UINT32 Idx, int &out_hue)
{
    int ret = 0;
    ISPC::ModuleR2Y *pR2Y = NULL;
    double fHue = 0.0;


    pR2Y = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
    if (NULL == pR2Y) {
        LOGE("(%s,L%d) pR2Y is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }
    fHue = pR2Y->fHue;
    ret = CalcHSBCToOut(pR2Y->R2Y_HUE.max, pR2Y->R2Y_HUE.min, fHue, out_hue);
    if (ret)
        return ret;

    LOGD("(%s,L%d) out_hue=%d\n", __FUNCTION__, __LINE__, out_hue);
    return IMG_SUCCESS;
}

int IPU_V2500::SetHue(IMG_UINT32 Idx, int hue)
{
    int ret = 0;
    ISPC::ModuleR2Y *pR2Y = NULL;
    double fHue = 0.0;


    ret = CheckHSBCInputParam(hue);
    if (ret)
        return ret;
    pR2Y = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
    if (NULL == pR2Y)
    {
        LOGE("(%s,L%d) pR2Y is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }
    ret = CalcHSBCToISP(pR2Y->R2Y_HUE.max, pR2Y->R2Y_HUE.min, hue, fHue);
    if (ret)
        return ret;

    pR2Y->fHue = fHue;
    pR2Y->requestUpdate();
    return IMG_SUCCESS;
}

int IPU_V2500::CheckAEDisabled(IMG_UINT32 Idx)
{
    int ret = -1;


    if (IsAEEnabled(Idx))
    {
        LOGE("(%s,L%d) Error: Not disable AE !!!\n", __func__, __LINE__);
        return ret;
    }
    return 0;
}

int IPU_V2500::CheckSensorClass(IMG_UINT32 Idx)
{
    int ret = -1;


    if (NULL == m_Cam[Idx].pCamera || NULL == m_Cam[Idx].pCamera->sensor)
    {
        LOGE("(%s,L%d) Error: param %p %p are NULL !!!\n", __func__, __LINE__,
                m_Cam[Idx].pCamera, m_Cam[Idx].pCamera->sensor);
        return ret;
    }

    return 0;
}

int IPU_V2500::SetSensorExposure(IMG_UINT32 Idx, int usec)
{
    int ret = -1;


    ret = CheckAEDisabled(Idx);
    if (ret)
        return ret;

    ret = CheckSensorClass(Idx);
    if (ret)
        return ret;


    LOGD("(%s,L%d) usec=%d \n", __func__, __LINE__,
            usec);

    ret = m_Cam[Idx].pCamera->sensor->setExposure(usec);

    return ret;
}

int IPU_V2500::GetSensorExposure(IMG_UINT32 Idx, int &usec)
{
    int ret = 0;


    ret = CheckSensorClass(Idx);
    if (ret)
        return ret;

    usec = m_Cam[Idx].pCamera->sensor->getExposure();
    return IMG_SUCCESS;
}

int IPU_V2500::SetDpfAttr(IMG_UINT32 Idx, const cam_dpf_attr_t *attr)
{
    int ret = -1;
    ISPC::ModuleDPF *pdpf = NULL;
    cam_common_t *pcmn = NULL;
    bool detect_en = false;

    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(m_Cam[Idx].pCamera);
    if (ret)
        return ret;

    pcmn = (cam_common_t*)&attr->cmn;
    pdpf = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleDPF>();

    if (pcmn->mdl_en)
    {
        detect_en = true;
        pdpf->ui32Threshold = attr->threshold;
        pdpf->fWeight = attr->weight;
    }
    else
    {
        detect_en = false;
    }
    pdpf->bDetect = detect_en;

    pdpf->requestUpdate();
    return ret;
}


int IPU_V2500::GetDpfAttr(IMG_UINT32 Idx, cam_dpf_attr_t *attr)
{
    int ret = -1;
    ISPC::ModuleDPF *pdpf = NULL;
    cam_common_t *pcmn = NULL;


    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(m_Cam[Idx].pCamera);
    if (ret)
        return ret;

    pcmn = &attr->cmn;
    pdpf = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleDPF>();

    pcmn->mdl_en = pdpf->bDetect;
    attr->threshold = pdpf->ui32Threshold;
    attr->weight = pdpf->fWeight;

    return ret;
}

int IPU_V2500::SetShaDenoiseAttr(IMG_UINT32 Idx, const cam_sha_dns_attr_t *attr)
{
    int ret = -1;
    ISPC::ModuleSHA *psha = NULL;
    cam_common_t *pcmn = NULL;
    bool bypass = 0;
    bool is_req = true;


    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(m_Cam[Idx].pCamera);
    if (ret)
        return ret;

    pcmn = (cam_common_t*)&attr->cmn;
    psha = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleSHA>();

    if (pcmn->mdl_en)
    {
        bypass = false;
        if (CAM_CMN_MANUAL == pcmn->mode)
        {
            psha->fDenoiseSigma = attr->strength;
            psha->fDenoiseTau = attr->smooth;
        }
    }
    else
    {
        bypass = true;
    }
    psha->bBypassDenoise = bypass;

    /*When module sha update, auto calculate in the setup function.
    * So should check the auto control module for fDenoiseSigma element.
    */
    if(is_req)
        psha->requestUpdate();

    LOGD("(%s,L%d) mdl_en=%d mode=%d sigma=%f tau=%f \n",
        __func__, __LINE__,
        attr->cmn.mdl_en, attr->cmn.mode,
        attr->strength, attr->smooth
    );


    return ret;
}


int IPU_V2500::GetShaDenoiseAttr(IMG_UINT32 Idx, cam_sha_dns_attr_t *attr)
{
    int ret = -1;
    ISPC::ModuleSHA *psha = NULL;
    cam_common_t *pcmn = NULL;
    cam_common_mode_e mode = CAM_CMN_AUTO;
    bool mdl_en = 0;
    bool autoctrl_en = 0;


    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(m_Cam[Idx].pCamera);
    if (ret)
        return ret;

    pcmn = &attr->cmn;
    psha = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleSHA>();

    if (psha->bBypassDenoise)
        mdl_en = 0;
    else
        mdl_en = 1;

    if (autoctrl_en)
        mode = CAM_CMN_AUTO;
    else
        mode = CAM_CMN_MANUAL;

    pcmn->mdl_en = mdl_en;
    pcmn->mode = mode;
    attr->strength = psha->fDenoiseSigma;
    attr->smooth = psha->fDenoiseTau;

    return ret;
}

int IPU_V2500::SetDnsAttr(IMG_UINT32 Idx, const cam_dns_attr_t *attr)
{
    int ret = -1;
    ISPC::ModuleDNS *pdns = NULL;
    cam_common_t *pcmn = NULL;
    bool autoctrl_en = 0;
    bool is_req = false;

    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(m_Cam[Idx].pCamera);
    if (ret)
        return ret;

    pdns = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleDNS>();
    pcmn = (cam_common_t*)&attr->cmn;

    if (pcmn->mdl_en)
    {
        if (CAM_CMN_AUTO == pcmn->mode)
        {
            autoctrl_en = true;
        }
        else
        {
            autoctrl_en = false;
            pdns->fStrength = attr->strength;
            pdns->bCombine = attr->is_combine_green;
            pdns->fGreyscaleThreshold = attr->greyscale_threshold;
            pdns->fSensorGain = attr->sensor_gain;
            pdns->ui32SensorBitdepth = attr->sensor_bitdepth;
            pdns->ui32SensorWellDepth = attr->sensor_well_depth;
            pdns->fSensorReadNoise = attr->sensor_read_noise;

            is_req = true;
        }
    }
    else
    {
        autoctrl_en = false;
        pdns->fStrength = 0;
        is_req = true;
    }
    if (m_Cam[Idx].pDNS)
    {
        m_Cam[Idx].pDNS->enableControl(autoctrl_en);
    }
    if (is_req)
        pdns->requestUpdate();


    LOGD("(%s,L%d) mdl_en=%d mode=%d str=%f gain=%f bit=%d noise=%f \n",
        __func__, __LINE__,
        attr->cmn.mdl_en, attr->cmn.mode,
        attr->strength,attr->sensor_gain,
        attr->sensor_bitdepth,
        attr->sensor_read_noise
    );


    return ret;
}


int IPU_V2500::GetDnsAttr(IMG_UINT32 Idx, cam_dns_attr_t *attr)
{
    int ret = -1;
    ISPC::ModuleDNS *pdns = NULL;
    cam_common_t *pcmn = NULL;
    cam_common_mode_e mode = CAM_CMN_AUTO;
    bool mdl_en = 0;
    bool autoctrl_en = 0;


    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(m_Cam[Idx].pCamera);
    if (ret)
        return ret;

    pdns = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleDNS>();
    pcmn = &attr->cmn;


    if (pdns->fStrength >= -EPSINON && pdns->fStrength <= EPSINON)
    {
        mdl_en = false;
    }
    else
    {
        mdl_en = true;
        if (m_Cam[Idx].pDNS)
        {
            autoctrl_en = m_Cam[Idx].pDNS->isEnabled();
        }
        if (autoctrl_en)
            mode = CAM_CMN_AUTO;
        else
            mode = CAM_CMN_MANUAL;
        attr->strength = pdns->fStrength;
        attr->is_combine_green = pdns->bCombine;
        attr->greyscale_threshold = pdns->fGreyscaleThreshold;
        attr->sensor_gain = pdns->fSensorGain;
        attr->sensor_bitdepth = pdns->ui32SensorBitdepth;
        attr->sensor_well_depth = pdns->ui32SensorWellDepth;
        attr->sensor_read_noise = pdns->fSensorReadNoise;
    }
    pcmn->mdl_en = mdl_en;
    pcmn->mode = mode;


    LOGD("(%s,L%d) mdl_en=%d mode=%d str=%f gain=%f bit=%d noise=%f \n",
        __func__, __LINE__,
        attr->cmn.mdl_en, attr->cmn.mode,
        attr->strength,attr->sensor_gain,
        attr->sensor_bitdepth,
        attr->sensor_read_noise
    );


    return ret;
}

int IPU_V2500::EnqueueReturnShot(void)
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
            m_Cam[Idx].pCamera->releaseShot(m_Cam[Idx].pstShotFrame[bufID]);
            m_Cam[Idx].pCamera->enqueueShot();
            m_Cam[Idx].pstShotFrame[bufID].YUV.phyAddr = 0;
        }
        returnPrivData = NULL;
    }
    pthread_mutex_unlock(&mutex_x);

    return 0;
}

int IPU_V2500::ClearAllShot()
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
                    m_postShots.pop_front();
                }
            }

            if (returnPrivData != NULL) {
                int bufID = returnPrivData->iBufID;

                for (Idx = 0; Idx < m_iSensorNb; ++Idx) {
                    m_Cam[Idx].pCamera->releaseShot(m_Cam[Idx].pstShotFrame[bufID]);
                    m_Cam[Idx].pCamera->enqueueShot();
                    m_Cam[Idx].pstShotFrame[bufID].YUV.phyAddr = 0;
                }
            }
        }
    }

    pthread_mutex_unlock(&mutex_x);
    return 0;
}

int IPU_V2500::ReturnShot(Buffer *pstBuf)
{
    struct ipu_v2500_private_data *pstPrvData;
    IMG_UINT32 Idx = 0, BufID = 0;


    pthread_mutex_lock(&mutex_x);

    if ((pstBuf == NULL) || (m_iBufUseCnt < 1)){
        LOGD("pCamera:%p m_iBufUseCnt:%d\n", pstBuf, m_iBufUseCnt);
         pthread_mutex_unlock(&mutex_x);
        return -1;
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
        if (m_Cam[Idx].pCamera == NULL)
        {
            pthread_mutex_unlock(&mutex_x);
            return -1;
        }

        if (m_Cam[Idx].pstShotFrame[BufID].YUV.phyAddr == 0)
        {
            LOGD("Shot phyAddr is NULL iBufID:%d \n", pstPrvData->iBufID);
            pthread_mutex_unlock(&mutex_x);
            return 0;
        }
    }
    if (pstPrvData->state != SHOT_POSTED) {
        LOGD("Shot is wrong state:%d\n", pstPrvData->state);
        pthread_mutex_unlock(&mutex_x);
        return 0;
    }
    pstPrvData->state = SHOT_RETURNED;

    pthread_mutex_unlock(&mutex_x);
    EnqueueShotEvent(SHOT_EVENT_ENQUEUE, (void*)__func__);
    return 0;
}

int IPU_V2500::SetWbAttr(IMG_UINT32 Idx, const cam_wb_attr_t *attr)
{
    int ret = -1;
    cam_common_t *pcmn = NULL;
    ISPC::ModuleWBC *pwbc = NULL;
    bool autoctrl_en = 0;
    bool is_req = false;


    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(m_Cam[Idx].pCamera);
    if (ret)
        return ret;

    pcmn = (cam_common_t*)&attr->cmn;
    pwbc = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleWBC>();


    if (pcmn->mdl_en)
    {
        if (CAM_CMN_MANUAL == pcmn->mode)
        {
            autoctrl_en = false;
            pwbc->aWBGain[0] = attr->man.rgb.r;
            pwbc->aWBGain[1] = attr->man.rgb.g;
            pwbc->aWBGain[2] = attr->man.rgb.g;
            pwbc->aWBGain[3] = attr->man.rgb.b;

            is_req = true;
        }
        else
        {
            autoctrl_en = true;
        }
    }
    else
    {
        autoctrl_en = false;
    }

    LOGD("(%s,L%d) mdl_en=%d mode=%d r=%f g=%f b=%f \n",
        __func__, __LINE__,
        attr->cmn.mdl_en, attr->cmn.mode,
        attr->man.rgb.r, attr->man.rgb.g, attr->man.rgb.b
        );


    if (m_Cam[Idx].pAWB)
        m_Cam[Idx].pAWB->enableControl(autoctrl_en);
    if (is_req)
        pwbc->requestUpdate();


    return ret;
}

int IPU_V2500::GetWbAttr(IMG_UINT32 Idx, cam_wb_attr_t *attr)
{
    int ret = -1;
    cam_common_t *pcmn = NULL;
    ISPC::ModuleWBC *pwbc = NULL;
    bool mdl_en = 1;
    bool autoctrl_en = 0;
    cam_common_mode_e mode = CAM_CMN_AUTO;


    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(m_Cam[Idx].pCamera);
    if (ret)
        return ret;

    pcmn = &attr->cmn;
    pwbc = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleWBC>();


    if (m_Cam[Idx].pAWB)
    {
        autoctrl_en = m_Cam[Idx].pAWB->isEnabled();
    }

    if (!autoctrl_en)
    {
        mode = CAM_CMN_MANUAL;
        attr->man.rgb.r = pwbc->aWBGain[0];
        attr->man.rgb.g = pwbc->aWBGain[1];
        attr->man.rgb.b = pwbc->aWBGain[3];
    }
    pcmn->mdl_en = mdl_en;
    pcmn->mode = mode;

    return ret;
}

int IPU_V2500::Set3dDnsAttr(const cam_3d_dns_attr_t *attr)
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

int IPU_V2500::Get3dDnsAttr(cam_3d_dns_attr_t *attr)
{
    int ret = -1;


    ret = CheckParam(attr);
    if (ret)
        return ret;
    this->dns_3d_attr.cmn.mdl_en = 1;
    *attr = this->dns_3d_attr;

    return ret;
}

int IPU_V2500::SetYuvGammaAttr(IMG_UINT32 Idx, const cam_yuv_gamma_attr_t *attr)
{
    int ret = -1;
    cam_common_t *pcmn = NULL;
    ISPC::ModuleTNM *ptnm = NULL;
    bool bypass = false;
    bool autoctrl_en = false;


    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(m_Cam[Idx].pCamera);
    if (ret)
        return ret;

    ptnm = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleTNM>();
    pcmn = (cam_common_t*)&attr->cmn;

    if (pcmn->mdl_en)
    {
        bypass = false;
        if (CAM_CMN_MANUAL == pcmn->mode)
        {
            autoctrl_en = false;
            ptnm->bBypass = bypass;

            for(int i = 0; i < CAM_ISP_YUV_GAMMA_COUNT; i++)
                ptnm->aCurve[i] = attr->curve[i];
#if 0
{
            const int line_count = 10;
            char temp_str[30];

            for(int i = 0; i < CAM_ISP_Y_GAMMA_COUNT; i++)
            {
                if ((0 == (i % line_count))
                    && (i != 0) )
                    printf("\n");
                if (0 == (i % line_count))
                    LOGE(" (%s,L%d) ", __func__, __LINE__);
                sprintf(temp_str, "%f", ptnm->aCurve[i]);
                printf("%8s  ", temp_str);
            }
            printf("\n");
}
#endif

            if (m_Cam[Idx].pTNM)
                m_Cam[Idx].pTNM->enableControl(autoctrl_en);
            ptnm->requestUpdate();
        }
        else
        {
            autoctrl_en = true;
            ptnm->bBypass = bypass;

            ptnm->requestUpdate();
            if (m_Cam[Idx].pTNM)
                m_Cam[Idx].pTNM->enableControl(autoctrl_en);
        }
    }
    else
    {
        bypass = true;
        autoctrl_en = false;
        ptnm->bBypass = bypass;

        if (m_Cam[Idx].pTNM)
            m_Cam[Idx].pTNM->enableControl(autoctrl_en);
        ptnm->requestUpdate();
    }


    return ret;
}

int IPU_V2500::GetYuvGammaAttr(IMG_UINT32 Idx, cam_yuv_gamma_attr_t *attr)
{
    int ret = -1;
    cam_common_t *pcmn = NULL;
    ISPC::ModuleTNM *ptnm = NULL;
    cam_common_mode_e mode = CAM_CMN_AUTO;
    bool mdl_en = false;
    bool autoctrl_en = false;


    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(m_Cam[Idx].pCamera);
    if (ret)
        return ret;

    ptnm = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleTNM>();
    pcmn = &attr->cmn;


    if (!ptnm->bBypass)
    {
        mdl_en = true;

        for(int i = 0; i < CAM_ISP_YUV_GAMMA_COUNT; i++)
            attr->curve[i] = ptnm->aCurve[i];
#if 0
{
            const int line_count = 10;
            char temp_str[30];

            for(int i = 0; i < CAM_ISP_Y_GAMMA_COUNT; i++)
            {
                if ((0 == (i % line_count))
                    && (i != 0) )
                    printf("\n");
                if (0 == (i % line_count))
                    LOGE(" (%s,L%d) ", __func__, __LINE__);
                sprintf(temp_str, "%f", attr->curve[i]);
                printf("%8s  ", temp_str);
            }
            printf("\n");
}
#endif
    }
    else
    {
        mdl_en = false;
    }
    if (m_Cam[Idx].pTNM)
        autoctrl_en = m_Cam[Idx].pTNM->isEnabled();
    if (autoctrl_en)
        mode = CAM_CMN_AUTO;
    else
        mode = CAM_CMN_MANUAL;
    pcmn->mdl_en = mdl_en;
    pcmn->mode = mode;

    return ret;
}

int IPU_V2500::SetFpsRange(IMG_UINT32 Idx, const cam_fps_range_t *fps_range)
{
    int ret = -1;


    ret = CheckParam(fps_range);
    if (ret)
        return ret;

    ret = CheckParam(m_Cam[Idx].pCamera);
    if (ret)
        return ret;
    if (fps_range->min_fps > fps_range->max_fps)
    {
        LOGE("(%s,L%d) Error min %f is bigger than max %f \n",
                __func__, __LINE__, fps_range->min_fps, fps_range->max_fps);
        return -1;
    }

    if (fps_range->max_fps == fps_range->min_fps) {
        if (IsAEEnabled(Idx) && m_Cam[Idx].pAE) {
            m_Cam[Idx].pAE->setAeExposureMethod(ISPC::ControlAE::AEMHD_EXP_DEFAULT);
        }

        m_Cam[Idx].pCamera->sensor->SetFPS((double)fps_range->max_fps);
        //clear highest bit of this byte means camera in default fps mode, assumes fps is not higher than 128
        this->GetOwner()->SyncPathFps(fps_range->max_fps);
        LOGE("fps: %d, rangemode: 0\n", (int)fps_range->max_fps);
    } else {
        if (IsAEEnabled(Idx) && m_Cam[Idx].pAE) {
            m_Cam[Idx].pAE->setFpsRange(fps_range->max_fps, fps_range->min_fps);
            m_Cam[Idx].pAE->setAeExposureMethod(ISPC::ControlAE::AEMHD_EXP_LOWLUX);
            this->GetOwner()->SyncPathFps(0x80|(int)fps_range->max_fps); //set highest bit of this byte means camera in range fps mode
            LOGE("fps: %d, rangemode: 1\n", (int)fps_range->max_fps);
        }
    }

    sFpsRange = *fps_range;

    return ret;
}

int IPU_V2500::GetFpsRange(IMG_UINT32 Idx, cam_fps_range_t *fps_range)
{
    int ret = -1;

    ret = CheckParam(fps_range);
    if (ret)
        return ret;

    *fps_range = sFpsRange;

    return ret;
}

int IPU_V2500::GetDayMode(IMG_UINT32 Idx, enum cam_day_mode &out_day_mode)
{
    int ret = -1;

    ret = CheckParam(m_Cam[Idx].pCamera);
    if (ret)
        return ret;

    ret = m_Cam[Idx].pCamera->DayModeDetect((int &)out_day_mode);
    if (ret)
        return -1;

    return 0;
}

int IPU_V2500::SkipFrame(IMG_UINT32 Idx, int skipNb)
{
    int ret = 0;
    int skipCnt = 0;
    ISPC::Shot frame;
    IMG_UINT32 CRCStatus = 0;
    int i = 0;


    while (skipCnt++ < skipNb)
    {
        m_Cam[Idx].pCamera->enqueueShot();
retry_1:
        ret = m_Cam[Idx].pCamera->acquireShot(frame, true);
        if (IMG_SUCCESS != ret)
        {
            GetPhyStatus(Idx, &CRCStatus);
            if (CRCStatus != 0) {
                for (i = 0; i < 5; i++) {
                m_Cam[Idx].pCamera->sensor->reset();
                usleep(200000);
                goto retry_1;
                }
                i = 0;
            }
            LOGE("(%s:L%d) failed to get shot\n", __func__, __LINE__);
            return ret;
        }

        LOGD("(%s:L%d) skipcnt=%d phyAddr=0x%X \n", __func__, __LINE__, skipCnt, frame.YUV.phyAddr);
        m_Cam[Idx].pCamera->releaseShot(frame);
    }


    return ret;
}

int IPU_V2500::SetCameraIdx(int Idx)
{
    m_iCurCamIdx = Idx;
    return IMG_SUCCESS;
}

int IPU_V2500::GetCameraIdx(int &Idx)
{
    Idx = m_iCurCamIdx;
    return IMG_SUCCESS;
}

int IPU_V2500::GetPhyStatus(IMG_UINT32 Idx, IMG_UINT32 *status)
{
    IMG_UINT32 Gasket;
    CI_GASKET_INFO pGasketInfo;
    int ret = 0;

    memset(&pGasketInfo, 0, sizeof(CI_GASKET_INFO));
    CI_CONNECTION *conn  = m_Cam[Idx].pCamera->getConnection();
    m_Cam[Idx].pCamera->sensor->GetGasketNum(&Gasket);

    ret = CI_GasketGetInfo(&pGasketInfo, Gasket, conn);
    if (IMG_SUCCESS == ret) {
        *status = pGasketInfo.ui8MipiEccError || pGasketInfo.ui8MipiCrcError;
        LOGD("*****pGasketInfo = 0x%x***0x%x*** \n", pGasketInfo.ui8MipiEccError,pGasketInfo.ui8MipiCrcError);
        return ret;
    }

    return -1;
}

int IPU_V2500::SetGAMCurve(IMG_UINT32 Idx,cam_gamcurve_t *GamCurve)
{
    ISPC::ModuleGMA *pGMA = m_Cam[Idx].pCamera->pipeline->getModule<ISPC::ModuleGMA>();
    CI_MODULE_GMA_LUT glut;
    CI_CONNECTION *conn  = m_Cam[Idx].pCamera->getConnection();

    if (pGMA == NULL) {
        return -1;
    }

    if (GamCurve != NULL) {
        pGMA->useCustomGam = 1;
        memcpy((void *)pGMA->customGamCurve, (void *)GamCurve->curve, sizeof(pGMA->customGamCurve));
    }

    memcpy(glut.aRedPoints, pGMA->customGamCurve, sizeof(pGMA->customGamCurve));
    memcpy(glut.aGreenPoints, pGMA->customGamCurve, sizeof(pGMA->customGamCurve));
    memcpy(glut.aBluePoints, pGMA->customGamCurve, sizeof(pGMA->customGamCurve));

    if(CI_DriverSetGammaLUT(conn, &glut) != IMG_SUCCESS)
    {
        LOGE("SetGAMCurve failed\n");
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;

}


int IPU_V2500::SaveBayerRaw(const char *fileName, const ISPC::Buffer &bayerBuffer)
{
    IMG_UINT32 totalSize = 0;
    IMG_UINT32 cnt = 0;
    IMG_SIZE writeSize = 0;
    int ret = 0;
    FILE *fd = NULL;
    IMG_UINT16 rgr[3];
    IMG_UINT32 *p32 = NULL;


    if (bayerBuffer.isTiled)
    {
        LOGE("cannot save tiled bayer buffer\n");
        return -1;
    }

    writeSize = sizeof(rgr);
    cnt = 0;
    totalSize = bayerBuffer.stride * bayerBuffer.vstride;
    p32 = (IMG_UINT32 *)bayerBuffer.firstData();
    fd = fopen(fileName, "wb");
    if (fd)
    {
        while (cnt < totalSize)
        {
            rgr[0] = (*p32) & 0x03FF;
            rgr[1] = ((*p32) >> 10) & 0x03FF;
            rgr[2] = ((*p32) >> 20) & 0x03FF;
            ++p32;
            cnt += 4;
            fwrite(rgr, writeSize, 1, fd);
        }
        fclose(fd);
        ret = 0;
    }
    else
    {
        ret = -1;
        LOGE("(%s:L%d) failed to open file %s !!!\n", __func__, __LINE__, fileName);
    }

    return ret;
}

int IPU_V2500::SaveFrameFile(const char *fileName, cam_data_format_e dataFmt, int idx, ISPC::Shot &shotFrame)
{
    int ret = -1;
    ISPC::Save saveFile;


    if (CAM_DATA_FMT_BAYER_FLX == dataFmt)
    {
         ret = saveFile.open(ISPC::Save::Bayer, *(m_Cam[idx].pCamera->getPipeline()),
            fileName);
    }
    else if (CAM_DATA_FMT_YUV == dataFmt)
    {
        ret = saveFile.open(ISPC::Save::YUV, *(m_Cam[idx].pCamera->getPipeline()),
            fileName);
    }
    else
    {
        LOGE("(%s:L%d) Not support %d \n", __func__, __LINE__, dataFmt);
        return -1;
    }

    if (IMG_SUCCESS != ret)
    {
        LOGE("(%s:L%d) failed to open file %s! \n", __func__, __LINE__, fileName);
        return -1;
    }

    if (shotFrame.bFrameError)
    {
        LOGE("(%s:L%d) Warning frame has error\n", __func__, __LINE__);
    }
    ret = saveFile.save(shotFrame);

    saveFile.close();

    return ret;
}

int IPU_V2500::SaveFrame(int idx, ISPC::Shot &shotFrame)
{
    int ret = 0;
    cam_data_format_e dataFmt = CAM_DATA_FMT_NONE;
    struct timeval stTv;
    struct tm* stTm;
    ISPC::Save saveFile;
    char fileName[260];
    char preName[100];
    char fmtName[40];


    ret = CheckParam(m_psSaveDataParam);
    if (ret)
    {
        return -1;
    }
    gettimeofday(&stTv,NULL);
    stTm = localtime(&stTv.tv_sec);
    if (strlen(m_psSaveDataParam->file_name) > 0)
    {
        strncpy(preName, m_psSaveDataParam->file_name, sizeof(preName));
    }
    else
    {
        snprintf(preName,sizeof(preName), "%02d%02d-%02d%02d%02d-%03d",
                stTm->tm_mon + 1, stTm->tm_mday,
                stTm->tm_hour, stTm->tm_min, stTm->tm_sec,
                (int)(stTv.tv_usec / 1000));
    }

    if (m_psSaveDataParam->fmt & CAM_DATA_FMT_YUV)
    {
        dataFmt = CAM_DATA_FMT_YUV;
        snprintf(m_psSaveDataParam->file_name, sizeof(m_psSaveDataParam->file_name), "%s_%dx%d_%s.yuv",
                        preName, shotFrame.YUV.width, shotFrame.YUV.height,
                        FormatString(shotFrame.YUV.pxlFormat));
        snprintf(fileName, sizeof(fileName), "%s%s", m_psSaveDataParam->file_path, m_psSaveDataParam->file_name);
        ret = SaveFrameFile(fileName, dataFmt, idx, shotFrame);
    }

    if (m_psSaveDataParam->fmt & CAM_DATA_FMT_BAYER_RAW)
    {
        dataFmt = CAM_DATA_FMT_BAYER_RAW;
        snprintf(fmtName, sizeof(fmtName),"%s%d", strchr(MosaicString(m_Cam[idx].pCamera->sensor->eBayerFormat), '_'),
                        m_Cam[idx].pCamera->sensor->uiBitDepth);
        snprintf(m_psSaveDataParam->file_name, sizeof(m_psSaveDataParam->file_name), "%s_%dx%d%s.raw",
                        preName, shotFrame.BAYER.width, shotFrame.BAYER.height,
                        fmtName);
        snprintf(fileName, sizeof(fileName), "%s%s", m_psSaveDataParam->file_path, m_psSaveDataParam->file_name);
        ret = SaveBayerRaw(fileName, shotFrame.BAYER);
    }

    if (m_psSaveDataParam->fmt & CAM_DATA_FMT_BAYER_FLX)
    {
        dataFmt = CAM_DATA_FMT_BAYER_FLX;
        snprintf(m_psSaveDataParam->file_name, sizeof(m_psSaveDataParam->file_name), "%s_%dx%d_%s.flx",
                        preName, shotFrame.BAYER.width, shotFrame.BAYER.height,
                        FormatString(shotFrame.BAYER.pxlFormat));
        snprintf(fileName, sizeof(fileName), "%s%s", m_psSaveDataParam->file_path, m_psSaveDataParam->file_name);
        ret = SaveFrameFile(fileName, dataFmt, idx, shotFrame);
    }

    sem_post(&m_semNotify);

    return ret;
}

int IPU_V2500::SaveData(cam_save_data_t *saveData)
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
int IPU_V2500::GetSensorsName(IMG_UINT32 Idx, cam_list_t * pName)
{
    if (NULL == pName)
    {
        LOGE("pName is invalid pointer\n");
        return -1;
    }

    std::list<std::pair<std::string, int> > sensors;
    std::list<std::pair<std::string, int> >::const_iterator it;
    int i = 0;
    m_Cam[Idx].pCamera->getSensor()->GetSensorNames(sensors);

    for(it = sensors.begin(); it != sensors.end(); it++)
    {
        strncpy(pName->name[i], it->first.c_str(), it->first.length());
        i++;
    }

    return IMG_SUCCESS;
}

int IPU_V2500::SetModuleOUTRawFlag(IMG_UINT32 idx, bool enable, CI_INOUT_POINTS DataExtractionPoint)
{
    int ret = 0;
    ISPC::ModuleOUT *out = m_Cam[idx].pCamera->getModule<ISPC::ModuleOUT>();

    if (NULL == out)
    {
        LOGE("(%s:L%d) module out is NULL error !!!\n", __func__, __LINE__);
        return -1;
    }
    if (enable)
    {
        switch (m_Cam[idx].pCamera->sensor->uiBitDepth)
        {
        case 8:
            out->dataExtractionType = BAYER_RGGB_8;
            break;
        case 12:
            out->dataExtractionType = BAYER_RGGB_12;
            break;
        case 10:
        default:
            out->dataExtractionType = BAYER_RGGB_10;
            break;
        }
        out->dataExtractionPoint = DataExtractionPoint;
    }
    else
    {
        out->dataExtractionType = PXL_NONE;
        out->dataExtractionPoint = CI_INOUT_NONE;
    }
    if (m_Cam[idx].pCamera->setupModule(ISPC::STP_OUT) != IMG_SUCCESS)
    {
        LOGE("(%s:L%d) failed to setup OUT module\n", __func__, __LINE__);
        ret = -1;
    }

    return ret;
}

int IPU_V2500::GetDebugInfo(void *pInfo, int *ps32Size)
{
   int i = 0;
    ST_ISP_DBG_INFO *pISPInfo = NULL;

    pISPInfo = (ST_ISP_DBG_INFO *)pInfo;
    *ps32Size = sizeof(ST_ISP_DBG_INFO);

    for (i = 0; i < N_CONTEXT; i++)
    {
        if (!m_Cam[i].pCamera)
            continue;

        pISPInfo->s32CtxN = i+1;
        ISPC::ModuleTNM *pIspTNM = \
            m_Cam[i].pCamera->pipeline->getModule<ISPC::ModuleTNM>();

        pISPInfo->s32AEEn[i] = IsAEEnabled(i);
        pISPInfo->s32AWBEn[i] = IsAWBEnabled(i);

    #ifdef INFOTM_HW_AWB_METHOD
        pISPInfo->s32HWAWBEn[i] = m_Cam[i].pAWB->bHwAwbEnable;
    #else
        pISPInfo->s32HWAWBEn[i] = -1;
    #endif

        GetDayMode(i, (enum cam_day_mode&)pISPInfo->s32DayMode[i]);
        GetMirror(i, (enum cam_mirror_state&)pISPInfo->s32MirrorMode[i]);

        pISPInfo->s32TNMStaticCurve[i] = pIspTNM->bStaticCurve;

        pISPInfo->u32Exp[i] = m_Cam[i].pCamera->sensor->getExposure();
        pISPInfo->u32MinExp[i] = m_Cam[i].pCamera->sensor->getMinExposure();
        pISPInfo->u32MaxExp[i] = m_Cam[i].pCamera->sensor->getMaxExposure();
        LOGD("%d exp=%u min=%u max=%u\n",
            i, pISPInfo->u32Exp[i] , pISPInfo->u32MinExp[i], pISPInfo->u32MaxExp[i]);

        pISPInfo->f64Gain[i] = m_Cam[i].pCamera->sensor->getGain();
        pISPInfo->f64MinGain[i] = m_Cam[i].pCamera->sensor->getMinGain();
        pISPInfo->f64MaxGain[i] = m_Cam[i].pCamera->sensor->getMaxGain();
        LOGD("%d gain=%f min=%f max=%f\n",
            i, pISPInfo->f64Gain[i] , pISPInfo->f64MinGain[i], pISPInfo->f64MaxGain[i]);
    }

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

