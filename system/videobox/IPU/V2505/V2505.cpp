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
#include "V2505.h"
//#include <fstream>

extern "C" {
#include <qsdk/items.h>
}
DYNAMIC_IPU(IPU_V2500, "v2500");
#define SKIP_FRAME (1)
#define SETUP_PATH  ("/root/.ispddk/")
#define DEV_PATH    ("/dev/ddk_sensor")
#define SENSOR_MAX_NUM 2

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
    Buffer dst;
    Buffer his;
    Buffer dsc;
    Port *p = GetPort("out");
    Port *phis = GetPort("his");
    Port *pcap = this->GetPort("cap");
    Port *pdsc = GetPort("dsc");
    int ret;
    int ncapture =0 ;
    int preSt = runSt;
    IMG_UINT32 ui32DnTargetIdx = 0;
    unsigned int i = 0, skip_fr = 0;
    int frames = 0;
    IMG_UINT64 timestamp = 0;
    IMG_UINT32 CRCStatus = 0;
    bool hasDE = false;
    bool hasSavedData = false;
#ifdef COMPILE_IQ_REALTIME_SAVE_RAW
    bool hasCapRaw = false;
#endif

    CreateShotEventThread((void *)this);

#ifdef COMPILE_IQ_REALTIME_SAVE_RAW
    SetModuleOUTRawFlag(false, m_eDataExtractionPoint); //disable raw in normal flow
#endif
    ret = SkipFrame(ui32SkipFrame);
    if (ret)
    {
        LOGE("(%s:L%d) failed to skip frame !!!\n", __func__, __LINE__);
        throw VBERR_RUNTIME;
        return;
    }

    //ofstream file;
    //int nsize = (p->GetWidth() * p->GetHeight()) * 3 / 2;
    //file.open("/mnt/capture_camera_frame.yuv", ios::out | ios::binary);
    //prepare enqueue available buffers.
    for(i = 0; i < nBuffers ; i++)
    {
        ret = pCamera->enqueueShot();
        if (IMG_SUCCESS != ret)
        {
            //LOGE("enqueue shot in v2500 ipu thread failed.\n");
            throw VBERR_RUNTIME;
            return ;
        }
    }

    //ConfigAutoControl();
    TimePin tp;

    LOGD("isp thread start = ===========\n");
    while(IsInState(ST::Running)) {
#ifdef COMPILE_IQ_REALTIME_SAVE_RAW
        pthread_mutex_lock(&mutex_x);
        hasCapRaw = m_hasSaveRaw;
        pthread_mutex_unlock(&mutex_x);
#endif
        if (runSt == CAM_RUN_ST_CAPTURING)
        {
            m_iBufUseCnt = 0;
            ncapture ++;
            preSt = runSt;
            if (ncapture == 1)
            {
                LOGD("in capture mode switch from preview to capture\n");
                this->pCamera->SetCaptureIQFlag(IMG_TRUE);

                if(pcap)
                {
                    this->ImgH = pcap->GetHeight();
                    this->ImgW = pcap->GetWidth();
                    this->pipH = pcap->GetPipHeight();
                    this->pipW = pcap->GetPipWidth();
                    this->pipX = pcap->GetPipX();
                    this->pipY = pcap->GetPipY();

                }
                else
                {
                    runSt = CAM_RUN_ST_PREVIEWING;
                    ncapture = 0;
                    preSt = runSt;
                    continue;
                }

                if (0 != this->pipW && 0 != this->pipH)
                {
                    this->pCamera->pipeline->getMCPipeline()->sIIF.ui16ImagerSize[0] = this->pipW/CI_CFA_WIDTH;
                    this->pCamera->pipeline->getMCPipeline()->sIIF.ui16ImagerSize[1] = this->pipH/CI_CFA_WIDTH;
                    this->SetIIFCrop(pipH, pipW, pipX, pipY);
                }

                if (CheckOutRes(p->GetName(), this->ImgW, this->ImgH) < 0)
                {
                    LOGE("ISP output width size is not 64 align\n");
                    throw VBERR_RUNTIME;
                }

                this->Unprepare();
                this->pCamera->setEncoderDimensions(pcap->GetWidth(), pcap->GetHeight());
                if (this->ImgW <= in_w && ImgH <= in_h)
                {
                    this->SetEncScaler(this->ImgW, this->ImgH);//Set output scaler by current port resolution;
                }
                else
                {
                    this->SetEncScaler(in_w, in_h);
                }

                //set display path not work
                ISPC::ModuleOUT *pout = this->pCamera->getModule<ISPC::ModuleOUT>();
                if (pout)
                {
                    pout->dataExtractionType = PXL_NONE;
                    pout->dataExtractionPoint = CI_INOUT_NONE;
                    pout->displayType = PXL_NONE;
                }
                LOGD("running here: ImgW = %d, ImgH =%d@@@\n",this->ImgW,  this->ImgH);
                pthread_mutex_lock(&mutex_x);
                if (m_psSaveDataParam)
                {
                    hasSavedData = true;
                    if (m_psSaveDataParam->fmt & (CAM_DATA_FMT_BAYER_FLX | CAM_DATA_FMT_BAYER_RAW))
                    {
                        SetModuleOUTRawFlag(true, m_eDataExtractionPoint);
                        pCamera->setDisplayDimensions(pcap->GetWidth(), pcap->GetHeight());
                        hasDE = true;
                    }
                }
                pthread_mutex_unlock(&mutex_x);
                this->pCamera->setupModules();
                this->pCamera->program();
                this->pCamera->allocateBufferPool(1, this->buffersIds);
            }
            LOGD("in capture running : runSt = %d\n", runSt);
            this->pCamera->startCapture(false);
            do {
                this->pCamera->enqueueShot();
                LOGD("in after enqueueShot in catpure mode\n");
retry:
                ret = this->pCamera->acquireShot(frame, true);
                if (IMG_SUCCESS != ret) {
                    pCamera->sensor->disable();
                    usleep(10000);
                    pCamera->sensor->enable();
                    goto retry;
                }
                //file.write((char *)frame.YUV.data, nsize);
                //TODO
                if ((skip_fr == SKIP_FRAME ) || ncapture > SKIP_FRAME) { //not continue capture , then put the buffer to next ipu
                    dst = pcap->GetBuffer();
                    dst.fr_buf.phys_addr = (uint32_t)frame.YUV.phyAddr;
                    pcap->PutBuffer(&dst);
                    LOGD("put the buffer to the next!\n");

                    if (hasSavedData)
                    {
                        pthread_mutex_lock(&mutex_x);
                        SaveFrame(frame);
                        pthread_mutex_unlock(&mutex_x);
                    }
                }
                this->pCamera->releaseShot(frame);
                if ( skip_fr == SKIP_FRAME || ncapture > SKIP_FRAME)
                {
                    if (ncapture <= SKIP_FRAME) //not continue capture, skip_fr = 0;
                        skip_fr = 0;
                    LOGD("out from do while===ncapture = %d\n", ncapture);
                    break;
                }
                skip_fr++;
            }while(1);
            this->pCamera->stopCapture(false);
            runSt = CAM_RUN_ST_CAPTURED;
            //munmap(temp, nsize);
            LOGD("is in capture mode running: runSt =%d \n", runSt);
        }
        else if (runSt == CAM_RUN_ST_CAPTURED)
        {

            LOGD("Is here running ########## : runSt =%d\n", runSt);
            usleep(16670);

        }
        else if (runSt == CAM_RUN_ST_PREVIEWING)
        {
            //may be blocking until the frame is received
            //this->pCamera->saveParameters(parameters);
            //ISPC::ParameterFileParser::saveGrouped(parameters, "/root/v2505_setupArgs.txt");

            if(preSt == CAM_RUN_ST_CAPTURING) //if before is capture, then swicth to preview.
            {
                if (hasSavedData)
                {
                    hasSavedData = false;
                    if (hasDE)
                    {
                        SetModuleOUTRawFlag(false, m_eDataExtractionPoint);
                        hasDE = false;
                    }
                }

                preSt = runSt;
                skip_fr = 0;
                ncapture = 0;//recover ncapture to 0, for next preview to capture.
                LOGD("Is here running from capture switch preview ##########: runSt =%d, preSt =%d\n", runSt, preSt);
                this->pCamera->SetCaptureIQFlag(IMG_FALSE);
                this->pCamera->deleteShots();
                std::list<IMG_UINT32>::iterator it;

                for (it = this->buffersIds.begin(); it != this->buffersIds.end(); it++)
                {
                    LOGD("getting bufferids = %d\n", *it);
                    if (IMG_SUCCESS != pCamera->deregisterBuffer(*it))
                    {
                        throw VBERR_RUNTIME;
                    }
                }
                buffersIds.clear();

                SetScalerFromPort();
                if (0 != this->pipW && 0 != this->pipH)
                {
                    //change to setScalerFromPort()
                    //this->pCamera->pipeline->getMCPipeline()->sIIF.ui16ImagerSize[0] = this->pipW/CI_CFA_WIDTH;
                    //this->pCamera->pipeline->getMCPipeline()->sIIF.ui16ImagerSize[1] = this->pipH/CI_CFA_WIDTH;
                    this->SetIIFCrop(pipH, pipW, pipX, pipY);
                }
                this->pCamera->setEncoderDimensions(p->GetWidth(), p->GetHeight());
                if (pdsc && (m_s32DscImgW != 0) && (m_s32DscImgH != 0) )
                {
                   this->pCamera->setDisplayDimensions(m_s32DscImgW,  m_s32DscImgH);
                }

                this->pCamera->setupModules();

                this->pCamera->program();

                this->pCamera->allocateBufferPool(this->nBuffers, this->buffersIds);
                this->pCamera->startCapture(false);

                for(i = 0; i < nBuffers; i++)
                {
                    ret = pCamera->enqueueShot();
                    if (IMG_SUCCESS != ret)
                    {
                        LOGE("enqueue shot in v2500 ipu thread failed.\n");
                        throw VBERR_RUNTIME;
                        return ;
                    }
                }

                CreateShotEventThread((void*)this);
            }

            dst = p->GetBuffer();
            if (pdsc->IsEnabled())
            {
                dsc = pdsc->GetBuffer();
            }
            int count = 0;
            while((IMG_UINT32)m_iBufUseCnt >= nBuffers)
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

retry_1:
            ret = pCamera->acquireShot(m_pstShotFrame[m_iBufferID], true);
            if (IMG_SUCCESS != ret) {
                LOGE("failed to get shot\n");
                GetPhyStatus(&CRCStatus);
                if (CRCStatus != 0) {
                    for (i = 0; i < 5; i++) {
                    pCamera->sensor->reset();
                    usleep(200000);
                    goto retry_1;
                    }
                    i = 0;
                }
                throw VBERR_RUNTIME;
            }

#ifdef COMPILE_IQ_REALTIME_SAVE_RAW
            if (hasCapRaw)
            {
                SetModuleOUTRawFlag(true, m_eDataExtractionPoint);
                usleep(100 * nBuffers * 1000); //for DDR bandwidth, next IPU can not get buffer to work
                if (m_pstShotFrame[m_iBufferID].BAYER.phyAddr
                    && m_pstShotFrame[m_iBufferID].BAYER.data)
                {
                    pthread_mutex_lock(&mutex_x);
                    SaveFrame(m_pstShotFrame[m_iBufferID]);
                    m_hasSaveRaw = 0;
                    hasCapRaw = 0;
                    pthread_mutex_unlock(&mutex_x);

                    SetModuleOUTRawFlag(false, m_eDataExtractionPoint);
                }
                this->pCamera->releaseShot(m_pstShotFrame[m_iBufferID]);
                this->pCamera->enqueueShot();
                m_pstShotFrame[m_iBufferID].YUV.phyAddr = 0;
                m_pstShotFrame[m_iBufferID].BAYER.phyAddr = 0;
                goto retry_1;
            }
#endif

            //TODO
            //file.write((char *)frame.YUV.data, nsize);
            timestamp = m_pstShotFrame[m_iBufferID].metadata.timestamps.ui64CurSystemTS;
            dst.Stamp(timestamp);
            LOGD("system time is ****%lld**** \n",dst.fr_buf.timestamp);
            dst.fr_buf.phys_addr = (uint32_t)m_pstShotFrame[m_iBufferID].YUV.phyAddr;
            pthread_mutex_lock(&mutex_x);
            m_pstPrvData[m_iBufferID].iBufID = m_iBufferID;
            m_pstPrvData[m_iBufferID].state = SHOT_POSTED;
            if ((p->IsEnabled()) && (p->GetEnableCount() >= 2))
            {
                m_pstPrvData[m_iBufferID].s32EnEsc = 0x01;
                dst.fr_buf.stAttr.s32Width = 0x00;
            }
            else
            {
                m_pstPrvData[m_iBufferID].s32EnEsc = 0x00;
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
                    struct ipu_v2500_dn_indicator *pdi = this->scenario?
                        dn_ind_1: dn_ind_0;

                    if(pAE->getNewGain() <= pdi[i].gain || pdi[i].gain == -1) {
                        m_pstPrvData[m_iBufferID].iDnID = pdi[i].dn_id;
    //                    LOGD("gain: %lf, scenario: %d ==> set DnCurveID: %d\n",
    //                        pAE->getNewGain(), this->scenario, pdi[i].dn_id);
                        break;
                    }
            }
#endif
            dst.fr_buf.priv = (int)&m_pstPrvData[m_iBufferID];
            if ((NULL != pdsc) && (pdsc->IsEnabled()))
            {
                dsc.Stamp(timestamp);
                dsc.fr_buf.phys_addr = (uint32_t)m_pstShotFrame[m_iBufferID].DISPLAY.phyAddr;
                dsc.fr_buf.virt_addr = (void *)m_pstShotFrame[m_iBufferID].DISPLAY.data;
                dsc.fr_buf.priv = (int)&m_pstPrvData[m_iBufferID];
                if ((p->IsEnabled()) && (p->GetEnableCount() >= 2))
                {
                    dsc.fr_buf.stAttr.s32Width = 0xFF;
                }
                else
                {
                    dsc.fr_buf.stAttr.s32Width = 0x00;
                }
            }

            GetMirror(m_pstPrvData[m_iBufferID].mirror);

            if(phis->IsEnabled()) {
                his = phis->GetBuffer();
                memcpy(his.fr_buf.virt_addr, (void *)m_pstShotFrame[m_iBufferID].metadata.histogramStats.regionHistograms,
                sizeof(m_pstShotFrame[m_iBufferID].metadata.histogramStats.regionHistograms));
                his.fr_buf.size = sizeof(m_pstShotFrame[m_iBufferID].metadata.histogramStats.regionHistograms);
                his.Stamp();
                phis->PutBuffer(&his);
            }


            ret = pCamera->DynamicChange3DDenoiseIdx(ui32DnTargetIdx);
            if (IMG_SUCCESS !=ret ) {
                LOGE("failed to call DynamicChange3DDenoiseIdx().\n");
                throw VBERR_RUNTIME;
                return ;
            }
            ret = pCamera->DynamicChangeIspParameters();
            if (IMG_SUCCESS !=ret ) {
                LOGE("failed to call DynamicChangeIspParameters().\n");
                throw VBERR_RUNTIME;
                return ;
            }
            m_pstPrvData[m_iBufferID].iDnID = ui32DnTargetIdx;
            pthread_mutex_lock(&mutex_x);
            m_pstPrvData[m_iBufferID].dns_3d_attr = dns_3d_attr;
            m_iBufUseCnt++;
            if (m_iBufUseCnt > (int)nBuffers)
                m_iBufUseCnt = (int)nBuffers;
            pthread_mutex_unlock(&mutex_x);

            p->PutBuffer(&dst);
            if ((NULL != pdsc) && (pdsc->IsEnabled()))
            {
                pdsc->PutBuffer(&dsc);
            }

            m_iBufferID++;
            if ((IMG_UINT32)m_iBufferID >= nBuffers)
                m_iBufferID = 0;

            frames++;

            if(tp.Peek() > 1)
            {
                //LOGE("current fps = %dfps\n", frames);
                frames = 0;
                tp.Poke();
            }
        }
    }

    DestoryShotEventThread(6);
    LOGE("V2505 thread exit ****************\n");
    runSt = CAM_RUN_ST_PREVIEWING;
}

IPU_V2500::IPU_V2500(std::string name, Json *js)
{
    char tmp[128];
    char dev_nm[64];
    char path[128];
    int fd = 0;
    int i = 0;
    int mode = 0;
    Name = name;
    this->context = 0;
    this->exslevel = 0;
    this->scenario = 0;
    this->wbMode = 0;
    this->pCamera = NULL;
    this->pAE = NULL;
    this->pDNS = NULL;
    this->pAWB = NULL;
    this->pTNM = NULL;
    this->pCMC = NULL;
    this->pLSH = NULL;
    this->currentStatus = V2500_NONE;
    this->runSt = CAM_RUN_ST_PREVIEWING; //default
    //this->BuildPort();move to below
    flDefTargetBrightness = 0.0;
    memset(&dns_3d_attr, 0, sizeof(dns_3d_attr));
    memset(&sFpsRange, 0, sizeof(sFpsRange));
    this->ui32SkipFrame = 0;
    this->m_psSaveDataParam = NULL;
    memset((void *)tmp, 0, sizeof(tmp));
#ifdef COMPILE_IQ_REALTIME_SAVE_RAW
    m_hasSaveRaw = 0;
#endif
    m_eDataExtractionPoint = CI_INOUT_BLACK_LEVEL;
    m_enIPUType = EN_IPU_ISP;

    /*Note: In order to support sensor1 only mode*/
    for (i =0; i < SENSOR_MAX_NUM; i++)
    {
        sprintf(tmp, "%s%d.%s", "sensor", i, "name");
        if(item_exist(tmp))
        {
            item_string(psname, tmp, 0);
            sprintf(tmp, "%s%d.%s", "sensor", i, "mode");
            if(item_exist(tmp))
            {
                mode = item_integer(tmp, 0);
            }

            break;
        }

    }
    /*Note: Can't find one sensor itm*/
    if (i == SENSOR_MAX_NUM)
    {
        LOGE("sensor name not in items file.\n");
        throw VBERR_RUNTIME;
        return ;
    }

    if (NULL !=js )
    {
        ui32SkipFrame = js->GetInt("skipframe");
        nBuffers = js->GetObject("nbuffers") ? js->GetInt("nbuffers") : 3;  //sensor mode always 0
        pCamera = new ISPC::Camera(this->context, ISPC::Sensor::GetSensorId(psname), mode,
            js->GetObject("flip") ? js->GetInt("flip") : 0, i);
    }
    else
    {
        nBuffers = 3;
        pCamera = new ISPC::Camera(this->context, ISPC::Sensor::GetSensorId(psname), mode, 0, i);
    }

    memset(dev_nm, 0, sizeof(dev_nm));
    memset(path, 0, sizeof(path));
    sprintf(dev_nm, "%s%d", DEV_PATH, i);
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
    sprintf(tmp, "%s%d-%s", "/tmp/sensor", i, "isp-config.txt");
    if (access(tmp, 0) != -1) {
        LOGE("Overwrite %s ==> %s \n", path, tmp);
        strncpy(path, tmp, 128);
    } else {
        LOGE("Cannot access  %s\n", tmp);
    }

#endif //#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
    if (path[0] == 0) {
        memset((void *)tmp, 0, sizeof(tmp));
        sprintf(tmp, "%s%s%d-%s", SETUP_PATH, "sensor", i, "isp-config.txt");
        strncpy(pSetupFile, tmp, 128);
        LOGD("setupfile = %s, nBuffers = %d\n", pSetupFile, nBuffers);
    } else {
        strncpy(pSetupFile, path, 128);
    }

    this->BuildPort();
    if (!pCamera)
    {
     LOGE("new a camera instance failed\n.");
     throw VBERR_RUNTIME;
     return;
    }

    if (ISPC::Camera::CAM_ERROR == pCamera->state)
    {
     LOGE("failed to create a camera instance\n");
     delete pCamera;
     pCamera = NULL;
     throw VBERR_RUNTIME;
     return ;
    }

    this->currentStatus = CAM_CREATE_SUCCESS;
    if(!pCamera->bOwnSensor)
    {
     delete pCamera;
     this->currentStatus = V2500_NONE;
     LOGE("Camera own a sensor failed\n");
     throw VBERR_RUNTIME;
     return ;
    }
    ISPC::CameraFactory::populateCameraFromHWVersion(*pCamera, pCamera->getSensor());
    pCamera->bUpdateASAP = true;

    this->currentStatus = SENSOR_CREATE_SUCESS;
#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
    m_bIqTuningDebugInfoEnable = false;
#endif //#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
}

int IPU_V2500::ConfigAutoControl()
{
    if (this->pAE)
    {
        pAE->enableFlickerRejection(true);
    }

    if (this->pTNM)
    {
        pTNM->enableControl(true);
        pTNM->enableLocalTNM(true);
        if (!pAE || !pAE->isEnabled())
        {
            pTNM->setAllowHISConfig(true);
        }
    }

    if (pDNS)
    {
        pDNS->enableControl(true);
    }

    if (pAWB)
    {
        pAWB->enableControl(true);
    }

    return IMG_SUCCESS;
}

int IPU_V2500::AutoControl()
{
    int ret = IMG_SUCCESS;
    if (!this->pAE)
    {
        pAE = new ISPC::ControlAE();
        ret = this->pCamera->registerControlModule(pAE);
        if(IMG_SUCCESS != ret)
        {
            LOGE("register AE module failed\n");
            delete pAE;
            pAE = NULL;
        }
    }

    if (!this->pAWB)
    {
#ifdef AWB_ALG_PLANCKIAN
        pAWB = new ISPC::ControlAWB_Planckian();
#else
        pAWB = new ISPC::ControlAWB_PID();
#endif
        //pAWB = new ISPC::ControlAWB_PID();
        //pAWB = new ISPC::ControlAWB_Planckian();
        ret = this->pCamera->registerControlModule(pAWB);
        if(IMG_SUCCESS != ret)
        {
            LOGE("register AWB module failed\n");
            delete pAWB;
            pAWB = NULL;
        }
    }

    if (!this->pLSH && this->pAWB)
    {
        pLSH = new ISPC::ControlLSH();
        ret = this->pCamera->registerControlModule(pLSH);
        if(IMG_SUCCESS !=ret)
        {
            LOGE("register pLSH module failed\n");
            delete pLSH;
            pLSH = NULL;
        }

        if (this->pLSH)
        {
            pLSH->registerCtrlAWB(pAWB);
            pLSH->enableControl(true);
        }
    }


    if (!this->pTNM)
    {
        pTNM = new ISPC::ControlTNM();
        ret = this->pCamera->registerControlModule(pTNM);
        if(IMG_SUCCESS != ret)
        {
            LOGE("register TNM module failed\n");
            delete pTNM;
            pTNM = NULL;
        }
    }


    if (!this->pDNS)
    {
        pDNS = new ISPC::ControlDNS();
        ret = this->pCamera->registerControlModule(pDNS);
        if(IMG_SUCCESS != ret)
        {
            LOGE("register DNS module failed\n");
            delete pDNS;
            pDNS = NULL;
        }
    }

    if (!this->pCMC) {
        pCMC = new ISPC::ControlCMC();
        ret = this->pCamera->registerControlModule(pCMC);
        if(IMG_SUCCESS != ret)
        {
            LOGE("register pCMC module failed\n");
            delete pCMC;
            pCMC = NULL;
        }
    }
    return ret;
}

int IPU_V2500::SetResolution(int imgW, int imgH)
{
    this->ImgH = imgH;
    this->ImgW = imgW;
    this->SetEncScaler(this->ImgW, this->ImgH);
    return IMG_SUCCESS;
}

//1 Sepia color
int IPU_V2500::SetSepiaMode(int mode)
{
    if(this->pCamera)
    {
        this->pCamera->setSepiaMode(mode);
    }

    return IMG_SUCCESS;
}

int IPU_V2500::SetScenario(int sn)
{
    this->scenario = sn;
    ISPC::ModuleR2Y *pR2Y = this->pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
    //ISPC::ModuleTNM *pTNM = pCamera->pipeline->getModule<ISPC::ModuleTNM>();
    ISPC::ModuleSHA *pSHA = pCamera->pipeline->getModule<ISPC::ModuleSHA>();
    ISPC::ControlCMC* pCMC = pCamera->getControlModule<ISPC::ControlCMC>();
    //ISPC::ControlTNM* pTNMC = pCamera->getControlModule<ISPC::ControlTNM>();
    IMG_UINT32 ui32DnTargetIdx;

    //sn =1 low light scenario, else hight light scenario
    if(this->scenario < 1)
    {
        FlatModeStatusSet(false);

        pR2Y->fBrightness = pCMC->fBrightness_acm;
        pR2Y->fContrast = pCMC->fContrast_acm;
        pR2Y->aRangeMult[0] = pCMC->aRangeMult_acm[0];
        pR2Y->aRangeMult[1] = pCMC->aRangeMult_acm[1];
        pR2Y->aRangeMult[2] = pCMC->aRangeMult_acm[2];

        pAE->setTargetBrightness(pCMC->targetBrightness_acm);
        pCamera->sensor->setMaxGain(pCMC->fSensorMaxGain_acm);

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

        pAE->setTargetBrightness(pCMC->targetBrightness_fcm);
        pCamera->sensor->setMaxGain(pCMC->fSensorMaxGain_fcm);

        pR2Y->fOffsetU = 0;
        pR2Y->fOffsetV = 0;
    }
    this->pCamera->DynamicChange3DDenoiseIdx(ui32DnTargetIdx, true);
    this->pCamera->DynamicChangeIspParameters(true);

    //pTNM->requestUpdate();
    pR2Y->requestUpdate();
    pSHA->requestUpdate();

    return IMG_SUCCESS;
}

int IPU_V2500::CheckOutRes(std::string name, int imgW, int imgH)
{
    if (imgW % 64)
    {
        return -1;
    }
    MC_PIPELINE *pMCPipeline = NULL;

    pMCPipeline = this->pCamera->pipeline->getMCPipeline();
    in_w = pMCPipeline->sIIF.ui16ImagerSize[0]*CI_CFA_WIDTH;
    in_h = pMCPipeline->sIIF.ui16ImagerSize[1]*CI_CFA_HEIGHT;

    LOGD("lockRatio is setting : %d \n", this->lockRatio);
    if (((imgW < in_w - this->pipX) && (imgH < in_h - this->pipY)) && (this->lockRatio))
    {
        if (imgH < imgW)
        {
            if (!strcmp(name.c_str(), "out"))
            {
                this->ImgW = imgH*((float)in_w/in_h);
            }
            else
            {
                this->m_s32DscImgW = imgH*((float)in_w/in_h);
            }
        }
        else
        {
            if (!strcmp(name.c_str(), "out"))
            {
                this->ImgH = imgW*((float)in_h/in_w);
            }
            else
            {
                this->m_s32DscImgH = imgW*((float)in_h/in_w);
            }
        }
    }

    LOGD("V2505 check output resolution imgW = %d, imgH = %d\n", this->ImgW, this->ImgH);
    return 0;
}

void IPU_V2500::Prepare()
{
    // allocate needed memory(reference frame/ snapshots, etc)
    // initialize the hardware
    // get input parameters from PortIn.Width/Height/PixelFormat

    // start work routine
    int fps = 30; //default 30
    SENSOR_STATUS status;
    cam_fps_range_t fps_range;
    ISPC::ParameterList parameters;
    ISPC::ParameterFileParser fileParser;
    ISPC::ModuleGMA *pGMA = pCamera->pipeline->getModule<ISPC::ModuleGMA>();
    Port *p =  this->GetPort("out");
    Port *pdsc = GetPort("dsc");

    parameters = fileParser.parseFile(this->pSetupFile);
    Pixel::Format enPixelFormat;

    enPixelFormat = p->GetPixelFormat();
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

    if (!parameters.validFlag)
    {
        LOGE("failed to get parameters from setup file.\n");
        throw VBERR_BADPARAM;
    }

    if (this->pCamera->loadParameters(parameters) != IMG_SUCCESS)
    {
        LOGE("failed to load camera configuration.\n");
        throw VBERR_BADPARAM;
    }

    if (pCamera->program() != IMG_SUCCESS)
    {
        LOGE("failed to program camera %d\n", __LINE__);
        throw VBERR_RUNTIME;
    }

    this->AutoControl();

    if (this->pCamera->loadControlParameters(parameters) != IMG_SUCCESS)
    {
        LOGE("Failed to load control module parameters\n");
        throw VBERR_RUNTIME;
    }
#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
    BackupIspParameters(m_Parameters);
#endif //#if defined(COMPILE_IQ_TUNING_TOOL_APIS)
    if (this->pAE)
    {
        flDefTargetBrightness = this->pAE->getOriTargetBrightness();
        LOGD("(%s:L%d) flDefTargetBrightness=%f\n", __func__, __LINE__, flDefTargetBrightness);
    }

    if (this->pLSH)
    {
        ISPC::ModuleLSH *pLSHmod = this->pCamera->getModule<ISPC::ModuleLSH>();

        if (pLSHmod)
        {
            pLSH->enableControl(pLSHmod->bEnableMatrix);
            if (pLSHmod->bEnableMatrix)
            {
                // load a default matrix before start
                pLSH->configureDefaultMatrix();
            }
        }
    }

    // set scaler from port configuration.
    SetScalerFromPort();

    if (0 != this->pipW && 0 != this->pipH)
    {
        this->SetIIFCrop(pipH, pipW, pipX, pipY);
    }

    LOGE("in start method : H = %d, W = %d, fps=%d, in_w = %d, in_h=%d\n", this->ImgH, this->ImgW, fps, in_w, in_h);

#ifdef COMPILE_IQ_REALTIME_SAVE_RAW
    SetModuleOUTRawFlag(true, m_eDataExtractionPoint); //for allocating raw buffer in allocateBufferPool function
#endif
    if (pCamera->setupModules() != IMG_SUCCESS)
    {
        LOGE("failed to setup camera\n");
        throw VBERR_RUNTIME;
    }

    this->pCamera->setEncoderDimensions(this->ImgW,  this->ImgH);
    if (pdsc && (m_s32DscImgW != 0) && (m_s32DscImgH != 0))
    {
        this->pCamera->setDisplayDimensions(m_s32DscImgW,  m_s32DscImgH);
    }

    if (pCamera->program() != IMG_SUCCESS)
    {
        LOGE("failed to program camera %d\n", __LINE__);
        throw VBERR_RUNTIME;
    }

    pGMA = pCamera->getModule<ISPC::ModuleGMA>();
    if (pGMA != NULL)
    {
        if (pGMA->useCustomGam)
        {

            if (this->SetGAMCurve(NULL) != IMG_SUCCESS)
            {
                LOGE("failed to set gam curve %d\n", __LINE__);
                throw VBERR_RUNTIME;
            }
        }
    }
    if (buffersIds.size() > 0)
    {
        LOGE(" Buffer all exist, camera start before.\n");
        throw VBERR_BADLOGIC;
    }

    if (this->pCamera->allocateBufferPool(this->nBuffers, buffersIds) != IMG_SUCCESS)
    {
        LOGE("camera allocate buffer poll failed\n");
        throw VBERR_RUNTIME;
    }

    if (pCamera->startCapture() != IMG_SUCCESS)
    {
        LOGE("start capture failed.\n");
        throw VBERR_RUNTIME;
    }

    m_pFun = std::bind(&IPU_V2500::ReturnShot,this,std::placeholders::_1);
    p->frBuffer->RegCbFunc(m_pFun);
    if (pdsc->GetWidth())
    {
        pdsc->frBuffer->RegCbFunc(m_pFun);
    }
    m_pstShotFrame = (ISPC::Shot*)malloc(nBuffers * sizeof(ISPC::Shot));
    if (m_pstShotFrame == NULL)
    {
        LOGE("camera allocate Shot Frame buffer failed\n");
        throw VBERR_RUNTIME;
    }
    memset((char*)m_pstShotFrame, 0, nBuffers * sizeof(ISPC::Shot));
    m_pstPrvData = (struct ipu_v2500_private_data *)malloc(nBuffers * sizeof(struct ipu_v2500_private_data));
    if (m_pstPrvData == NULL)
    {
        LOGE("camera allocate private_data failed\n");
        throw VBERR_RUNTIME;
    }
    memset((char*)m_pstPrvData, 0, nBuffers * sizeof(struct ipu_v2500_private_data));
    IMG_UINT32 i = 0;
    for (i = 0; i < nBuffers; i++) {
        m_pstPrvData[i].state = SHOT_INITED;
    }
    m_postShots.clear();
    m_iBufferID = 0;
    m_iBufUseCnt = 0;

    if (NULL != p)
    {
        fps = p->GetFPS();
    }
    if (this->m_MinFps == 0 && this->m_MaxFps == 0) {
        this->pCamera->sensor->GetFlipMirror(status);
        fps = status.fCurrentFps;
        if (fps) {
            this->GetOwner()->SyncPathFps(fps);
        }
    } else if (this->m_MinFps != 0 && this->m_MaxFps == 0) {
        GetFpsRange(&fps_range);
        if (this->m_MinFps > fps_range.max_fps) {
            LOGE("warning: the MinFps is more than the max_fps\n");
        } else {
            fps_range.min_fps = this->m_MinFps;
            SetFpsRange(&fps_range);
        }
    } else if (this->m_MinFps == 0 && this->m_MaxFps != 0) {
        GetFpsRange(&fps_range);
        if (this->m_MaxFps < fps_range.min_fps) {
            LOGE("warning: the min_fps is more than the m_MaxFps\n");
        } else {
            fps_range.max_fps = this->m_MaxFps;
            SetFpsRange(&fps_range);
        }
    } else if (this->m_MinFps <=  this->m_MaxFps) {
        fps_range.min_fps = this->m_MinFps;
        fps_range.max_fps = this->m_MaxFps;
        SetFpsRange(&fps_range);
    } else if (this->m_MinFps > this->m_MaxFps) {
        LOGE("warning: the MinFps is more than the MaxFps\n");
        fps_range.min_fps = this->m_MaxFps;
        fps_range.max_fps = this->m_MinFps;
        SetFpsRange(&fps_range);
    }
}

void IPU_V2500::Unprepare()
{
    unsigned int i = 0;
    unsigned int iShotListSize = m_postShots.size();
    Port *pIpuPort  =NULL;

    // Shot is aquired, but not release
    for (i = 0; i < iShotListSize; i++)
    {
        struct ipu_v2500_private_data *returnPrivData = NULL;
        if (!m_postShots.empty()) {
            struct ipu_v2500_private_data *privData =
                (struct ipu_v2500_private_data *)(m_postShots.front());
            returnPrivData = privData;
            m_postShots.pop_front();
        }
        if (returnPrivData != NULL) {
            int bufID = returnPrivData->iBufID;
            pCamera->releaseShot(m_pstShotFrame[bufID]);
            m_pstShotFrame[bufID].YUV.phyAddr = 0;
        }
    }

    // Shot is enqueued, but not aquire & release
    for (i = 0; i < (nBuffers - iShotListSize); i++) {
        ISPC::Shot tmp_shot;
        if (pCamera->acquireShot(tmp_shot) != IMG_SUCCESS)
        {
            LOGE("acqureshot:%d before stopping failed: %d\n", i+1, __FILE__);
            throw VBERR_RUNTIME;
        }

        pCamera->releaseShot(tmp_shot);
        LOGD("in unprepare release shot i =%d\n", i);
    }

    if (pCamera->stopCapture(this->runSt - 1) != IMG_SUCCESS)
    {
        LOGE("failed to stop the capture.\n");
        throw VBERR_RUNTIME;
    }

    this->pCamera->deleteShots();
    std::list<IMG_UINT32>::iterator it;

    for (it = this->buffersIds.begin(); it != this->buffersIds.end(); it++)
    {
        LOGD("in unprepare free buffer id = %d\n", *it);
        if (IMG_SUCCESS != pCamera->deregisterBuffer(*it))
        {
            throw VBERR_RUNTIME;
        }
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

    buffersIds.clear();
    m_postShots.clear();
}

void IPU_V2500::BuildPort(int imgW, int imgH)
{

    Port *p = NULL, *phis = NULL, *pcap = NULL, *pdsc = NULL;
    p = CreatePort("out", Port::Out);
    phis = CreatePort("his", Port::Out);
    pcap = CreatePort("cap", Port::Out);
    pdsc = CreatePort("dsc", Port::Out);
    if(NULL != p)
    {
        // if out port has no w, h, use default value 1920x1088
        this->ImgH = imgH;
        this->ImgW = imgW;
        LOGD("isp build out port nBuffers = %d \n", this->nBuffers);
        p->SetBufferType(FRBuffer::Type::VACANT, this->nBuffers);
        p->SetResolution((int)imgW, (int)imgH);
        p->SetPixelFormat(Pixel::Format::NV21);
        p->SetFPS(30);    //FIXME!!!
        p->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);
    }

    if(NULL != pdsc)
    {
        // if dsc port has no w, h, use default value 1920x1088
        this->ImgH = imgH;
        this->ImgW = imgW;
        LOGD("isp build dsc port nBuffers = %d \n", this->nBuffers);
        pdsc->SetBufferType(FRBuffer::Type::VACANT, this->nBuffers);
        pdsc->SetPixelFormat(Pixel::Format::RGBA8888);
        pdsc->SetFPS(30);    //FIXME!!!
        pdsc->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);
    }

    if(NULL != phis)
    {
        phis->SetBufferType(FRBuffer::Type::FLOAT, 128*1024, 4*1024);
        phis->SetPixelFormat(Pixel::Format::VAM);
        phis->SetResolution((int)imgW, (int)imgH);
        //float : no need to set resolution
        //p->SetResolution((int)imgH, (int)imgW);
        //how to set format to his

    }

    if(NULL != pcap)
    {
        pcap->SetBufferType(FRBuffer::Type::VACANT, 1);
        pcap->SetResolution((int)imgW, (int)imgH);
        pcap->SetPixelFormat(Pixel::Format::NV12);
        pcap->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_Dynamic);
    }
}

int IPU_V2500::SetScalerFromPort()
{
    Port *p =  this->GetPort("out");
    Port *pdsc = GetPort("dsc");
    if(p)
    {
        this->ImgH = p->GetHeight();
        this->ImgW = p->GetWidth();
        this->pipH = p->GetPipHeight();
        this->pipW = p->GetPipWidth();
        this->pipX = p->GetPipX();
        this->pipY = p->GetPipY();
        this->m_MinFps = p->GetMinFps();
        this->m_MaxFps = p->GetMaxFps();
        this->lockRatio = p->GetLockRatio();
        if ((this->pipW != 0) && (this->pipH != 0))
        {
            this->pCamera->pipeline->getMCPipeline()->sIIF.ui16ImagerSize[0] = this->pipW/CI_CFA_WIDTH;
            this->pCamera->pipeline->getMCPipeline()->sIIF.ui16ImagerSize[1] = this->pipH/CI_CFA_WIDTH;
        }
        if (CheckOutRes(p->GetName(), this->ImgW, this->ImgH) < 0)
        {
            LOGE("ISP output width size is not 64 align\n");
            throw VBERR_RUNTIME;
        }

        if (this->ImgW <= in_w && ImgH <= in_h)
        {
            this->SetEncScaler(this->ImgW, this->ImgH);
        }
        else
        {
            this->SetEncScaler(in_w, in_h);
        }

    }

    if (pdsc)
    {
        m_s32DscImgW = pdsc->GetWidth();
        m_s32DscImgH = pdsc->GetHeight();
        LOGD("m_s32DscImgW = %d, m_s32DscImgH = %d", m_s32DscImgW, m_s32DscImgH);
        if (CheckOutRes(p->GetName(), m_s32DscImgW, m_s32DscImgH) < 0)
        {
           LOGE("ISP output width size is not 64 align\n");
           throw VBERR_RUNTIME;
        }

        if ( m_s32DscImgW != 0 && m_s32DscImgW <= in_w && m_s32DscImgH  <= in_h)
        {
            this->SetDscScaler(m_s32DscImgW, m_s32DscImgH);
        }
        else
        {
            this->SetDscScaler(in_w, in_h);
        }

        if ((m_s32DscImgW != 0) && (m_s32DscImgH != 0))
        {
            ISPC::ModuleOUT *pout = this->pCamera->getModule<ISPC::ModuleOUT>();
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
int IPU_V2500::SetExposureLevel(int level)
{
    double exstarget = 0;
    double cur_target = 0;
    double target_brightness = 0;


    if(level > 4 || level < -4)
    {
        LOGE("the level %d is out of the range.\n", level);
        return -1;
    }

    if(this->pAE)
    {
        this->exslevel = level;
        exstarget = (double)level / 4.0;
        cur_target = flDefTargetBrightness;

        target_brightness = cur_target + exstarget;
        if (target_brightness > 1.0)
            target_brightness = 1.0;
        if (target_brightness < -1.0)
            target_brightness = -1.0;

        this->pAE->setOriTargetBrightness(target_brightness);
    }
    else
    {
        LOGE("the AE is not register\n");
        return -1;
    }
    return IMG_SUCCESS;
}

int IPU_V2500::GetExposureLevel(int &level)
{
    level = this->exslevel;
    return IMG_SUCCESS;
}

int  IPU_V2500::SetAntiFlicker(int freq)
{
    if (this->pAE)
    {
        this->pAE->enableFlickerRejection(true, freq);
    }

    return IMG_SUCCESS;
}

int  IPU_V2500::GetAntiFlicker(int &freq)
{
    if (this->pAE)
    {
        freq = this->pAE->getFlickerRejectionFrequency();
    }

    return IMG_SUCCESS;
}

int IPU_V2500::SetEncScaler(int imgW, int imgH)
{
    ISPC::ModuleESC *pesc= NULL;
    MC_PIPELINE *pMCPipeline = NULL;
    IMG_UINT32 owidth, oheight;
    double esc_pitch[2];
    ISPC::ScalerRectType format;
    IMG_INT32 rect[4];

    // set isp output encoder scaler
    if(this->pCamera)
    {
        pesc = this->pCamera->pipeline->getModule<ISPC::ModuleESC>();
        pMCPipeline = this->pCamera->pipeline->getMCPipeline();
        if( NULL != pesc && NULL != pMCPipeline)
        {
            owidth = (pMCPipeline->sIIF.ui16ImagerSize[0]*CI_CFA_WIDTH);
            oheight = (pMCPipeline->sIIF.ui16ImagerSize[1]*CI_CFA_HEIGHT);
            esc_pitch[0] = (float)imgW/owidth;
            esc_pitch[1] = (float)imgH/oheight;
            LOGD("esc_pitch[0] =%f, esc_pitch[1] = %f\n", esc_pitch[0], esc_pitch[1]);
            if (esc_pitch[0] <= 1.0 && esc_pitch[1] <= 1.0)
            {
                pesc->aPitch[0] = esc_pitch[0];
                pesc->aPitch[1] = esc_pitch[1];
                pesc->aRect[0] = rect[0] = 0; //offset x
                pesc->aRect[1] = rect[1] = 0; //offset y
                pesc->aRect[2] = rect[2] = ImgW;
                pesc->aRect[3] = rect[3] = ImgH;
                pesc->eRectType = format = ISPC::SCALER_RECT_SIZE;
                pesc->requestUpdate();
            }
            else
            {
                LOGE("the resolution can't support!\n");
                return -1;
            }

        }
    }

    return IMG_SUCCESS;
}

int IPU_V2500::SetDscScaler(int s32ImgW, int s32ImgH)
{
    ISPC::ModuleDSC *pdsc= NULL;
    MC_PIPELINE *pMCPipeline = NULL;
    IMG_UINT32 owidth, oheight;
    double dsc_pitch[2];
    ISPC::ScalerRectType format;
    IMG_INT32 rect[4];

    // set isp output display scaler
    if(this->pCamera)
    {
        pdsc = this->pCamera->pipeline->getModule<ISPC::ModuleDSC>();
        pMCPipeline = this->pCamera->pipeline->getMCPipeline();
        if( NULL != pdsc && NULL != pMCPipeline)
        {
            owidth = (pMCPipeline->sIIF.ui16ImagerSize[0]*CI_CFA_WIDTH);
            oheight = (pMCPipeline->sIIF.ui16ImagerSize[1]*CI_CFA_HEIGHT);
            dsc_pitch[0] = (float)s32ImgW / owidth;
            dsc_pitch[1] = (float)s32ImgH / oheight;
            LOGD("dsc_pitch[0] =%f, dsc_pitch[1] = %f\n", dsc_pitch[0], dsc_pitch[1]);
            if (dsc_pitch[0] <= 1.0 && dsc_pitch[1] <= 1.0)
            {
                pdsc->aPitch[0] = dsc_pitch[0];
                pdsc->aPitch[1] = dsc_pitch[1];
                pdsc->aRect[0] = rect[0] = 0; //offset x
                pdsc->aRect[1] = rect[1] = 0; //offset y
                pdsc->aRect[2] = rect[2] = s32ImgW;
                pdsc->aRect[3] = rect[3] = s32ImgH;
                pdsc->eRectType = format = ISPC::SCALER_RECT_SIZE;
                pdsc->requestUpdate();
            }
            else
            {
                LOGE("the resolution can't support!\n");
                return -1;
            }

        }
    }

    return IMG_SUCCESS;
}

int IPU_V2500::SetIIFCrop(int pipH, int pipW, int pipX, int pipY)
{
    ISPC::ModuleIIF *piif= NULL;
    MC_PIPELINE *pMCPipeline = NULL;
    IMG_UINT32 owidth, oheight;
    double iif_decimation[2];
    IMG_INT32 rect[4];
    this->pipH = pipH;
    this->pipW = pipW;
    this->pipX = pipX;
    this->pipY = pipY;

    if(this->pCamera)
    {
        piif= this->pCamera->pipeline->getModule<ISPC::ModuleIIF>();
        pMCPipeline = this->pCamera->pipeline->getMCPipeline();
        if( NULL != piif && NULL != pMCPipeline)
        {
            owidth = (pMCPipeline->sIIF.ui16ImagerSize[0]*CI_CFA_WIDTH);
            oheight = (pMCPipeline->sIIF.ui16ImagerSize[1]*CI_CFA_HEIGHT);
            //printf("cfa2 width = %d, cfa2 height= %d\n", pMCPipeline->sIIF.ui16ImagerSize[0], pMCPipeline->sIIF.ui16ImagerSize[1]);
            //printf("owidth = %d, oheight = %d\n", owidth, oheight);
            //printf("imgW = %d, imgH = %d\n", this->pipW, this->pipH);
            //printf("offsetx = %d, offsety = %d\n", this->pipX, this->pipY);
            iif_decimation[1] = 0;
            iif_decimation[0] = 0;
            if (((float)(pipX+pipW)/owidth)<= 1.0 && ((float)(pipY+pipH)/oheight)<= 1.0)
            {
                piif->aDecimation[0] = iif_decimation[0];
                piif->aDecimation[1] = iif_decimation[1];
                piif->aCropTL[0] = rect[0] = this->pipX; //offset x
                piif->aCropTL[1] = rect[1] = this->pipY; //offset y
                piif->aCropBR[0] = rect[2] = this->pipW+this->pipX-2;
                piif->aCropBR[1] = rect[3] = this->pipH+this->pipY-2;
                piif->requestUpdate();
            }
            else
            {
                LOGE("the resolution can't support!\n");
                return -1;
            }

        }
    }

    return IMG_SUCCESS;
}


int IPU_V2500::SetWB(int mode)
{
    ISPC::ModuleCCM *pCCM = this->pCamera->getModule<ISPC::ModuleCCM>();
    ISPC::ModuleWBC *pWBC = this->pCamera->getModule<ISPC::ModuleWBC>();
    const ISPC::TemperatureCorrection &tmpccm = this->pAWB->getTemperatureCorrections();
    this->wbMode = mode;
    if(mode  == 0)
    {
        this->EnableAWB(true);
    }
    else if (mode > 0 && mode -1 < tmpccm.size())
    {

        this->EnableAWB(false);
        ISPC::ColorCorrection ccm = tmpccm.getCorrection(mode - 1);
        for(int i=0;i<3;i++)
        {
            for(int j=0;j<3;j++)
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

int IPU_V2500::GetWB(int &mode)
{
    mode = this->wbMode;

    return 0;
}

int IPU_V2500::GetYUVBrightness(int &out_brightness)
{
    int ret = 0;
    ISPC::ModuleR2Y *pr2y = NULL;
    ISPC::ControlTNM *pTNMC = NULL;
    double light = 0.0;
    bool bEnable = false;


    pTNMC = pCamera->getControlModule<ISPC::ControlTNM>();
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
        pr2y = this->pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
        if (NULL == pr2y)
        {
            LOGE("(%s,L%d) pr2y is NULL error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
        light = pr2y->fBrightness;
        ret = CalcHSBCToOut(pr2y->R2Y_BRIGHTNESS.max, pr2y->R2Y_BRIGHTNESS.min, light, out_brightness);
    }
    if (ret)
        return ret;

    return IMG_SUCCESS;
}

int IPU_V2500::SetYUVBrightness(int brightness)
{
    int ret = 0;
    ISPC::ModuleR2Y *pr2y = NULL;
    ISPC::ControlTNM *pTNMC = NULL;
    double light = 0.0;
    bool bEnable = false;


    ret = CheckHSBCInputParam(brightness);
    if (ret)
        return ret;

    pTNMC = pCamera->getControlModule<ISPC::ControlTNM>();
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
        pr2y = this->pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
        if (NULL == pr2y)
        {
            LOGE("(%s,L%d) pr2y is NULL error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
        ret = CalcHSBCToISP(pr2y->R2Y_BRIGHTNESS.max, pr2y->R2Y_BRIGHTNESS.min, brightness, light);
        if (ret)
            return ret;

        pr2y->fBrightness = light;
        pr2y->requestUpdate();
    }

    return IMG_SUCCESS;
}

int IPU_V2500::GetContrast(int &contrast)
{
    int ret = 0;
    ISPC::ModuleR2Y *pr2y = NULL;
    ISPC::ControlTNM *pTNMC = NULL;
    double cst = 0.0;
    bool bEnable = false;


    pTNMC = pCamera->getControlModule<ISPC::ControlTNM>();
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
        pr2y = this->pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
        if (NULL == pr2y)
        {
            LOGE("(%s,L%d) pr2y is NULL error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
        cst = pr2y->fContrast;
        ret = CalcHSBCToOut(pr2y->R2Y_CONTRAST.max, pr2y->R2Y_CONTRAST.min, cst, contrast);
    }
    if (ret)
        return ret;

    return IMG_SUCCESS;
}

int IPU_V2500::SetContrast(int contrast)
{
    int ret = 0;
    ISPC::ModuleR2Y *pr2y = NULL;
    ISPC::ControlTNM *pTNMC = NULL;
    double cst = 0.0;
    bool bEnable = false;


    ret = CheckHSBCInputParam(contrast);
    if (ret)
        return ret;

    pTNMC = pCamera->getControlModule<ISPC::ControlTNM>();
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
        pr2y = this->pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
        if (NULL == pr2y)
        {
            LOGE("(%s,L%d) pr2y is NULL error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
        ret = CalcHSBCToISP(pr2y->R2Y_CONTRAST.max, pr2y->R2Y_CONTRAST.min, contrast, cst);
        if (ret)
            return ret;

        pr2y->fContrast = cst;
        pr2y->requestUpdate();
    }

    return IMG_SUCCESS;
}

int IPU_V2500::GetSaturation(int &saturation)
{
    int ret = 0;
#if defined(USE_R2Y_INTERFACE)
    ISPC::ModuleR2Y *pr2y = NULL;
#else
    ISPC::ControlCMC *pCMC = NULL;
#endif
    double sat = 0.0;


#if defined(USE_R2Y_INTERFACE)
    pr2y = this->pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
    if (NULL == pr2y)
    {
        LOGE("(%s,L%d) pr2y is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }
    sat = pr2y->fSaturation;
    ret = CalcHSBCToOut(pr2y->R2Y_SATURATION.max, pr2y->R2Y_SATURATION.min, sat, saturation);
#else
    pCMC = pCamera->getControlModule<ISPC::ControlCMC>();
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

int IPU_V2500::SetSaturation(int saturation)
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
    pr2y = this->pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
    if (NULL == pr2y)
    {
        LOGE("(%s,L%d) pr2y is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }
    ret = CalcHSBCToISP(pr2y->R2Y_SATURATION.max, pr2y->R2Y_SATURATION.min, saturation, sat);
#else
    pCMC = pCamera->getControlModule<ISPC::ControlCMC>();
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
    pCamera->DynamicChangeIspParameters(true);
#endif

    return IMG_SUCCESS;
}

int IPU_V2500::GetSharpness(int &sharpness)
{
    int ret = 0;
    ISPC::ModuleSHA *psha = NULL;
    ISPC::ControlCMC* pCMC = NULL;
    double fShap = 0.0;


    psha = this->pCamera->pipeline->getModule<ISPC::ModuleSHA>();
    if (NULL == psha)
    {
        LOGE("(%s,L%d) psha is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }
    pCMC = this->pCamera->getControlModule<ISPC::ControlCMC>();
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

int IPU_V2500::SetSharpness(int sharpness)
{
    int ret = 0;
    ISPC::ModuleSHA *psha = NULL;
    ISPC::ControlCMC* pCMC = NULL;
    double shap = 0.0;


    ret = CheckHSBCInputParam(sharpness);
    if (ret)
        return ret;

    psha = this->pCamera->pipeline->getModule<ISPC::ModuleSHA>();
    if (NULL == psha)
    {
        LOGE("(%s,L%d) psha is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }
    pCMC = this->pCamera->getControlModule<ISPC::ControlCMC>();
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

int IPU_V2500::SetFPS(int fps)
{

    if (fps < 0)
    {
        if (IsAEEnabled() && pAE)
            this->pAE->setAeExposureMethod(ISPC::ControlAE::AEMHD_EXP_LOWLUX);
    }
    else
    {
        if (IsAEEnabled() && pAE)
            this->pAE->setAeExposureMethod(ISPC::ControlAE::AEMHD_EXP_DEFAULT);

        this->pCamera->sensor->SetFPS(fps);
        this->GetOwner()->SyncPathFps(fps);
    }

    return 0;
}

int IPU_V2500::GetFPS(int &fps)
{
    SENSOR_STATUS status;

    if(this->pCamera)
    {
        this->pCamera->sensor->GetFlipMirror(status);
        fps = status.fCurrentFps;
        return 0;
    }
    else
    {
        return -1;
    }
}

int IPU_V2500::GetScenario(enum cam_scenario &scen)
{
    scen = (enum cam_scenario)this->scenario;

    return 0;
}

int IPU_V2500::GetResolution(int &imgW, int &imgH)
{
    imgH = this->ImgH;
    imgW = this->ImgW;

    return IMG_SUCCESS;
}

//get current frame statistics lumination value in AE
// enviroment brightness value
int IPU_V2500::GetEnvBrightnessValue(int &out_bv)
{
    double brightness = 0.0;


    if(this->pAE)
    {
        brightness = this->pAE->getCurrentBrightness();
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
int IPU_V2500::EnableAE(int enable)
{
    if(this->pAE)
    {
        this->pAE->enableAE(enable);
        return IMG_SUCCESS;
    }
    else
    {
        return -1;
    }

}

int IPU_V2500::EnableAWB(int enable)
{
    if(this->pAWB)
    {
        this->pAWB->enableControl(enable);
        return IMG_SUCCESS;
    }
    else
    {
        return -1;
    }

}

int IPU_V2500::IsAWBEnabled()
{
    if(this->pAWB)
    {
        return this->pAWB->IsAWBenabled();
    }
    else
    {
        return 0;
    }
}

int IPU_V2500::IsAEEnabled()
{
    if(this->pAE)
    {
        return this->pAE->IsAEenabled();
    }
    else
    {
        return 0;
    }

}

int IPU_V2500::IsMonchromeEnabled()
{
    return this->pCamera->IsMonoMode();
}

int IPU_V2500::SetSensorISO(int iso)
{
    double fiso = 0.0;
    int ret = -1;
    double max_gain = 0.0;
    double min_gain = 0.0;
    bool isAeEnabled = false;

    ret = CheckHSBCInputParam(iso);
    if (ret)
        return ret;

    isAeEnabled = IsAEEnabled();
    min_gain = this->pCamera->sensor->getMinGain();
    if (pAE && isAeEnabled)
    {
        max_gain = this->pAE->getMaxOrgAeGain();
        if (iso > 0)
        {
            fiso = (double)iso;
            if (fiso < min_gain)
                fiso = min_gain;
            if (fiso > max_gain)
                fiso = max_gain;
            pAE->setMaxManualAeGain(fiso, true);
        }
        else if (0 == iso)
        {
            pAE->setMaxManualAeGain(max_gain, false);
        }
    }
    else
    {
        ret = CheckSensorClass();
        if (ret)
            return ret;

        max_gain = this->pCamera->sensor->getMaxGain();
        min_gain = this->pCamera->sensor->getMinGain();
        fiso = (double)iso;
        if (fiso < min_gain)
            fiso = min_gain;
        if (fiso > max_gain)
            fiso = max_gain;
        if (iso > 0)
            this->pCamera->sensor->setGain(fiso);
    }
    LOGD("%s maxgain=%f, mingain=%f fiso=%f\n", __func__, max_gain, min_gain, fiso);

    return IMG_SUCCESS;
}

int IPU_V2500::GetSensorISO(int &iso)
{
    int ret = 0;
    bool isAeEnabled = false;

    ret = CheckSensorClass();
    if (ret)
        return ret;

    isAeEnabled = IsAEEnabled();
    if (pAE && isAeEnabled)
    {
        iso = (int)pAE->getMaxManualAeGain();
    } else {
        iso = (int)this->pCamera->sensor->getGain();
    }

    return IMG_SUCCESS;
}

//control monochrome
int IPU_V2500::EnableMonochrome(int enable)
{
    this->pCamera->updateTNMSaturation(enable?0:1);
    return 0;
}

int IPU_V2500::EnableWDR(int enable)
{
    if(this->pTNM)
    {
        //this->pTNM->enableAdaptiveTNM(enable);
        this->pTNM->enableLocalTNM(enable);
        return IMG_SUCCESS;
    }
    else
    {
        return -1;
    }
}

int IPU_V2500::IsWDREnabled()
{
    if(this->pTNM)
    {
        return this->pTNM->getLocalTNM();//(this->pTNM->getAdaptiveTNM() && this->pTNM->getLocalTNM());
    }
    else
    {
        return 0;
    }
}

int IPU_V2500::GetMirror(enum cam_mirror_state &dir)
{
    SENSOR_STATUS status;


    if(this->pCamera)
    {
        this->pCamera->sensor->GetFlipMirror(status);
        dir =  (enum cam_mirror_state)status.ui8Flipping;
        return 0;
    }
    else
    {
        return -1;
    }
}

int IPU_V2500::SetMirror(enum cam_mirror_state dir)
{
    int flag = 0;

    switch(dir)
    {
        case cam_mirror_state::CAM_MIRROR_NONE: //0
        {
            if(this->pCamera)
            {
                flag = SENSOR_FLIP_NONE;
                this->pCamera->sensor->setFlipMirror(flag);
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
            if(this->pCamera)
            {
                flag = SENSOR_FLIP_HORIZONTAL;
                this->pCamera->sensor->setFlipMirror(flag);
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
            if(this->pCamera)
            {
                flag = SENSOR_FLIP_VERTICAL;
                this->pCamera->sensor->setFlipMirror(flag);
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
            if(this->pCamera)
            {
                flag = SENSOR_FLIP_BOTH;
                this->pCamera->sensor->setFlipMirror(flag);
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

int IPU_V2500::SetSensorResolution(const res_t res)
{
    if(this->pCamera)
    {
        return this->pCamera->sensor->SetResolution(res.W, res.H);
    }
    else
    {
        return -1;
    }
}

int IPU_V2500::SetSnapResolution(const res_t res)
{
    return SetResolution(res.W, res.H);
}

int IPU_V2500::SnapOneShot()
{
    //this->runSt = 1;
    pthread_mutex_lock(&mutex_x);
    runSt = CAM_RUN_ST_CAPTURING;
    pthread_mutex_unlock(&mutex_x);
    return IMG_SUCCESS;
}

int IPU_V2500::SnapExit()
{
    //pthread_mutex_unlock(&capture_done_lock);
    //this->runSt = 2;
    pthread_mutex_lock(&mutex_x);
    runSt = CAM_RUN_ST_PREVIEWING;
    pthread_mutex_unlock(&mutex_x);
    return IMG_SUCCESS;
}

int IPU_V2500::GetSnapResolution(reslist_t *presl)
{

    reslist_t res;

    if (this->pCamera)
    {
        if (pCamera->sensor->GetSnapResolution(res) != IMG_SUCCESS)
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


IPU_V2500::~IPU_V2500()
{
    if (pCamera)
    {
        delete pCamera;
        pCamera = NULL;
    }

    if (this->pAE)
    {
        delete pAE;
        pAE = NULL;
    }

    if (this->pDNS)
    {
        delete pDNS;
        pDNS = NULL;
    }

    if (this->pAWB)
    {
        delete pAWB;
        pAWB = NULL;
    }

    if (this->pTNM)
    {
        delete pTNM;
        pTNM= NULL;
    }

    if (this->pLSH)
    {
        delete pLSH;
        pLSH = NULL;
    }

    if (m_pstShotFrame != NULL)
    {
        free(m_pstShotFrame);
        m_pstShotFrame = NULL;
    }
    if (m_pstPrvData != NULL)
    {
        free(m_pstPrvData);
        m_pstPrvData = NULL;
    }
    //release v2500 port
    this->DestroyPort("out");
    this->DestroyPort("hist");
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

int IPU_V2500::GetHue(int &out_hue)
{
    int ret = 0;
    ISPC::ModuleR2Y *pr2y = NULL;
    double fHue = 0.0;


    pr2y = this->pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
    if (NULL == pr2y) {
        LOGE("(%s,L%d) pr2y is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }
    fHue = pr2y->fHue;
    ret = CalcHSBCToOut(pr2y->R2Y_HUE.max, pr2y->R2Y_HUE.min, fHue, out_hue);
    if (ret)
        return ret;

    LOGD("(%s,L%d) out_hue=%d\n", __FUNCTION__, __LINE__, out_hue);
    return IMG_SUCCESS;
}

int IPU_V2500::SetHue(int hue)
{
    int ret = 0;
    ISPC::ModuleR2Y *pr2y = NULL;
    double fHue = 0.0;


    ret = CheckHSBCInputParam(hue);
    if (ret)
        return ret;
    pr2y = this->pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
    if (NULL == pr2y)
    {
        LOGE("(%s,L%d) pr2y is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }
    ret = CalcHSBCToISP(pr2y->R2Y_HUE.max, pr2y->R2Y_HUE.min, hue, fHue);
    if (ret)
        return ret;

    pr2y->fHue = fHue;
    pr2y->requestUpdate();
    return IMG_SUCCESS;
}

int IPU_V2500::CheckAEDisabled()
{
    int ret = -1;


    if (IsAEEnabled())
    {
        LOGE("(%s,L%d) Error: Not disable AE !!!\n", __func__, __LINE__);
        return ret;
    }
    return 0;
}

int IPU_V2500::CheckSensorClass()
{
    int ret = -1;


    if (NULL == this->pCamera || NULL == this->pCamera->sensor)
    {
        LOGE("(%s,L%d) Error: param %p %p are NULL !!!\n", __func__, __LINE__,
                this->pCamera, this->pCamera->sensor);
        return ret;
    }

    return 0;
}

int IPU_V2500::SetSensorExposure(int usec)
{
    int ret = -1;


    ret = CheckAEDisabled();
    if (ret)
        return ret;

    ret = CheckSensorClass();
    if (ret)
        return ret;


    LOGD("(%s,L%d) usec=%d \n", __func__, __LINE__,
            usec);

    ret = this->pCamera->sensor->setExposure(usec);

    return ret;
}

int IPU_V2500::GetSensorExposure(int &usec)
{
    int ret = 0;


    ret = CheckSensorClass();
    if (ret)
        return ret;

    usec = this->pCamera->sensor->getExposure();
    return IMG_SUCCESS;
}

int IPU_V2500::SetDpfAttr(const cam_dpf_attr_t *attr)
{
    int ret = -1;
    ISPC::ModuleDPF *pdpf = NULL;
    cam_common_t *pcmn = NULL;
    bool detect_en = false;

    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(pCamera);
    if (ret)
        return ret;

    pcmn = (cam_common_t*)&attr->cmn;
    pdpf = this->pCamera->pipeline->getModule<ISPC::ModuleDPF>();

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


int IPU_V2500::GetDpfAttr(cam_dpf_attr_t *attr)
{
    int ret = -1;
    ISPC::ModuleDPF *pdpf = NULL;
    cam_common_t *pcmn = NULL;


    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(pCamera);
    if (ret)
        return ret;

    pcmn = &attr->cmn;
    pdpf = this->pCamera->pipeline->getModule<ISPC::ModuleDPF>();

    pcmn->mdl_en = pdpf->bDetect;
    attr->threshold = pdpf->ui32Threshold;
    attr->weight = pdpf->fWeight;

    return ret;
}

int IPU_V2500::SetShaDenoiseAttr(const cam_sha_dns_attr_t *attr)
{
    int ret = -1;
    ISPC::ModuleSHA *psha = NULL;
    cam_common_t *pcmn = NULL;
    bool bypass = 0;
    bool is_req = true;


    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(pCamera);
    if (ret)
        return ret;

    pcmn = (cam_common_t*)&attr->cmn;
    psha = this->pCamera->pipeline->getModule<ISPC::ModuleSHA>();

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


int IPU_V2500::GetShaDenoiseAttr(cam_sha_dns_attr_t *attr)
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
    ret = CheckParam(pCamera);
    if (ret)
        return ret;

    pcmn = &attr->cmn;
    psha = this->pCamera->pipeline->getModule<ISPC::ModuleSHA>();

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

int IPU_V2500::SetDnsAttr(const cam_dns_attr_t *attr)
{
    int ret = -1;
    ISPC::ModuleDNS *pdns = NULL;
    cam_common_t *pcmn = NULL;
    bool autoctrl_en = 0;
    bool is_req = false;

    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(pCamera);
    if (ret)
        return ret;

    pdns = this->pCamera->pipeline->getModule<ISPC::ModuleDNS>();
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
    if (pDNS)
    {
        pDNS->enableControl(autoctrl_en);
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


int IPU_V2500::GetDnsAttr(cam_dns_attr_t *attr)
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
    ret = CheckParam(pCamera);
    if (ret)
        return ret;

    pdns = this->pCamera->pipeline->getModule<ISPC::ModuleDNS>();
    pcmn = &attr->cmn;


    if (pdns->fStrength >= -EPSINON && pdns->fStrength <= EPSINON)
    {
        mdl_en = false;
    }
    else
    {
        mdl_en = true;
        if (pDNS)
        {
            autoctrl_en = pDNS->isEnabled();
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
    struct ipu_v2500_private_data *privData = NULL;
    int bufID = 0;
    IMG_UINT32 i;

    for (i = 0; i < nBuffers; i++)
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
            pCamera->releaseShot(m_pstShotFrame[bufID]);
            pCamera->enqueueShot();
            m_pstShotFrame[bufID].YUV.phyAddr = 0;
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
                    pCamera->releaseShot(m_pstShotFrame[bufID]);
                    pCamera->enqueueShot();
                    m_pstShotFrame[bufID].YUV.phyAddr = 0;
                    privData = NULL;
                }
            }
            pthread_mutex_unlock(&mutex_x);
            break;
        }
    }
    return 0;
}

int IPU_V2500::ClearAllShot()
{
    pthread_mutex_lock(&mutex_x);
    IMG_UINT32 i =0;
    if (m_postShots.size() == nBuffers) {
        //NOTE: here we only release first shot in the list
        for (i = 0; i < nBuffers; i++) {
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
                pCamera->releaseShot(m_pstShotFrame[bufID]);
                pCamera->enqueueShot();
                m_pstShotFrame[bufID].YUV.phyAddr = 0;
            }
        }
    }

    pthread_mutex_unlock(&mutex_x);
    return 0;
}

int IPU_V2500::ReturnShot(Buffer *pstBuf)
{
    struct ipu_v2500_private_data *pstPrvData;
    int s32EnEsc = 0;

    pthread_mutex_lock(&mutex_x);
    if ((pCamera == NULL) || (m_iBufUseCnt < 1)){
        LOGD("pCamera:%p m_iBufUseCnt:%d\n", pCamera, m_iBufUseCnt);
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

    if ((IMG_UINT32)pstPrvData->iBufID >= nBuffers)
    {
        LOGE("Buf ID over flow\n");
        pthread_mutex_unlock(&mutex_x);
        return -1;
    }

    if (m_pstShotFrame[pstPrvData->iBufID].YUV.phyAddr == 0)
    {
        LOGD("Shot phyAddr is NULL iBufID:%d \n", pstPrvData->iBufID);
        pthread_mutex_unlock(&mutex_x);
        return 0;
    }

    if (pstPrvData->state != SHOT_POSTED) {
        LOGD("Shot is wrong state:%d\n", pstPrvData->state);
        pthread_mutex_unlock(&mutex_x);
        return 0;
    }

    pstPrvData->state = SHOT_RETURNED;
    m_iBufUseCnt--;
    if (m_iBufUseCnt < 0)
        m_iBufUseCnt = 0;

    pthread_mutex_unlock(&mutex_x);
    EnqueueShotEvent(SHOT_EVENT_ENQUEUE, (void*)__func__);
    return 0;
}

int IPU_V2500::SetWbAttr(const cam_wb_attr_t *attr)
{
    int ret = -1;
    cam_common_t *pcmn = NULL;
    ISPC::ModuleWBC2_6 *pwbc2 = NULL;
    bool autoctrl_en = 0;
    bool is_req = false;


    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(pCamera);
    if (ret)
        return ret;

    pcmn = (cam_common_t*)&attr->cmn;
    pwbc2 = this->pCamera->pipeline->getModule<ISPC::ModuleWBC2_6>();


    if (pcmn->mdl_en)
    {
        if (CAM_CMN_MANUAL == pcmn->mode)
        {
            autoctrl_en = false;
            pwbc2->aWBGain[0] = attr->man.rgb.r;
            pwbc2->aWBGain[1] = attr->man.rgb.g;
            pwbc2->aWBGain[2] = attr->man.rgb.g;
            pwbc2->aWBGain[3] = attr->man.rgb.b;

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


    if (pAWB)
        pAWB->enableControl(autoctrl_en);
    if (is_req)
        pwbc2->requestUpdate();


    return ret;
}

int IPU_V2500::GetWbAttr(cam_wb_attr_t *attr)
{
    int ret = -1;
    cam_common_t *pcmn = NULL;
    ISPC::ModuleWBC2_6 *pwbc2 = NULL;
    bool mdl_en = 1;
    bool autoctrl_en = 0;
    cam_common_mode_e mode = CAM_CMN_AUTO;


    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(pCamera);
    if (ret)
        return ret;

    pcmn = &attr->cmn;
    pwbc2 = this->pCamera->pipeline->getModule<ISPC::ModuleWBC2_6>();


    if (pAWB)
    {
        autoctrl_en = pAWB->isEnabled();
    }

    if (!autoctrl_en)
    {
        mode = CAM_CMN_MANUAL;
        attr->man.rgb.r = pwbc2->aWBGain[0];
        attr->man.rgb.g = pwbc2->aWBGain[1];
        attr->man.rgb.b = pwbc2->aWBGain[3];
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

int IPU_V2500::SetYuvGammaAttr(const cam_yuv_gamma_attr_t *attr)
{
    int ret = -1;
    cam_common_t *pcmn = NULL;
    ISPC::ModuleTNM *ptnm = NULL;
    bool bypass = false;
    bool autoctrl_en = false;


    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(pCamera);
    if (ret)
        return ret;

    ptnm = this->pCamera->pipeline->getModule<ISPC::ModuleTNM>();
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

            if (this->pTNM)
                pTNM->enableControl(autoctrl_en);
            ptnm->requestUpdate();
        }
        else
        {
            autoctrl_en = true;
            ptnm->bBypass = bypass;

            ptnm->requestUpdate();
            if (this->pTNM)
                pTNM->enableControl(autoctrl_en);
        }
    }
    else
    {
        bypass = true;
        autoctrl_en = false;
        ptnm->bBypass = bypass;

        if (this->pTNM)
            pTNM->enableControl(autoctrl_en);
        ptnm->requestUpdate();
    }


    return ret;
}

int IPU_V2500::GetYuvGammaAttr(cam_yuv_gamma_attr_t *attr)
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
    ret = CheckParam(pCamera);
    if (ret)
        return ret;

    ptnm = this->pCamera->pipeline->getModule<ISPC::ModuleTNM>();
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
    if (pTNM)
        autoctrl_en = pTNM->isEnabled();
    if (autoctrl_en)
        mode = CAM_CMN_AUTO;
    else
        mode = CAM_CMN_MANUAL;
    pcmn->mdl_en = mdl_en;
    pcmn->mode = mode;

    return ret;
}

int IPU_V2500::SetFpsRange(const cam_fps_range_t *fps_range)
{
    int ret = -1;


    ret = CheckParam(fps_range);
    if (ret)
        return ret;

    ret = CheckParam(pCamera);
    if (ret)
        return ret;
    if (fps_range->min_fps > fps_range->max_fps)
    {
        LOGE("(%s,L%d) Error min %f is bigger than max %f \n",
                __func__, __LINE__, fps_range->min_fps, fps_range->max_fps);
        return -1;
    }

    if (fps_range->max_fps == fps_range->min_fps) {
        if (IsAEEnabled() && pAE) {
            this->pAE->setAeExposureMethod(ISPC::ControlAE::AEMHD_EXP_DEFAULT);
        }

        this->pCamera->sensor->SetFPS((double)fps_range->max_fps);
        //clear highest bit of this byte means camera in default fps mode, assumes fps is not higher than 128
        this->GetOwner()->SyncPathFps(fps_range->max_fps);
        LOGE("fps: %d, rangemode: 0\n", (int)fps_range->max_fps);
    } else {
        if (IsAEEnabled() && pAE) {
            pAE->setFpsRange(fps_range->max_fps, fps_range->min_fps);
            this->pAE->setAeExposureMethod(ISPC::ControlAE::AEMHD_EXP_LOWLUX);
            this->GetOwner()->SyncPathFps(0x80|(int)fps_range->max_fps); //set highest bit of this byte means camera in range fps mode
            LOGE("fps: %d, rangemode: 1\n", (int)fps_range->max_fps);
        }
    }

    sFpsRange = *fps_range;

    return ret;
}

int IPU_V2500::GetFpsRange(cam_fps_range_t *fps_range)
{
    int ret = -1;

    ret = CheckParam(fps_range);
    if (ret)
        return ret;

    *fps_range = sFpsRange;

    return ret;
}

int IPU_V2500::GetDayMode(enum cam_day_mode &out_day_mode)
{
    int ret = -1;

    ret = CheckParam(pCamera);
    if (ret)
        return ret;

    ret = this->pCamera->DayModeDetect((int &)out_day_mode);
    if (ret)
        return -1;

    return 0;
}

int IPU_V2500::SkipFrame(int skipNb)
{
    int ret = 0;
    int i = 0;
    int skipCnt = 0;
    ISPC::Shot frame;
    IMG_UINT32 CRCStatus = 0;


    while (skipCnt++ < skipNb)
    {
        pCamera->enqueueShot();
retry_2:
        ret = pCamera->acquireShot(frame, true);
        if (IMG_SUCCESS != ret)
        {
            GetPhyStatus(&CRCStatus);
            if (CRCStatus != 0) {
                for (i = 0; i < 5; i++) {
                    pCamera->sensor->reset();
                    usleep(200000);
                    goto retry_2;
                }
                    i = 0;
            }
            return ret;
        }

        LOGD("(%s:L%d) skipcnt=%d phyAddr=0x%X \n", __func__, __LINE__, skipCnt, frame.YUV.phyAddr);
        pCamera->releaseShot(frame);
    }


    return ret;
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

int IPU_V2500::SaveFrameFile(const char *fileName, cam_data_format_e dataFmt, ISPC::Shot &shotFrame)
{
    int ret = -1;
    ISPC::Save saveFile;


    if (CAM_DATA_FMT_BAYER_FLX == dataFmt)
    {
         ret = saveFile.open(ISPC::Save::Bayer, *(pCamera->getPipeline()),
            fileName);
    }
    else if (CAM_DATA_FMT_YUV == dataFmt)
    {
        ret = saveFile.open(ISPC::Save::YUV, *(pCamera->getPipeline()),
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

int IPU_V2500::SaveFrame(ISPC::Shot &shotFrame)
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
        ret = SaveFrameFile(fileName, dataFmt, shotFrame);
    }

    if (m_psSaveDataParam->fmt & CAM_DATA_FMT_BAYER_RAW)
    {
        dataFmt = CAM_DATA_FMT_BAYER_RAW;
        snprintf(fmtName, sizeof(fmtName),"%s%d", strchr(MosaicString(pCamera->sensor->eBayerFormat), '_'),
                        pCamera->sensor->uiBitDepth);
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
        ret = SaveFrameFile(fileName, dataFmt, shotFrame);
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

int IPU_V2500::GetPhyStatus(IMG_UINT32 *status)
{
    IMG_UINT32 Gasket;
    CI_GASKET_INFO pGasketInfo;
    int ret = 0;

    memset(&pGasketInfo, 0, sizeof(CI_GASKET_INFO));
    CI_CONNECTION *conn  = this->pCamera->getConnection();
    pCamera->sensor->GetGasketNum(&Gasket);

    ret = CI_GasketGetInfo(&pGasketInfo, Gasket, conn);
    if (IMG_SUCCESS == ret) {
        *status = pGasketInfo.ui8MipiEccError || pGasketInfo.ui8MipiCrcError;
        LOGD("*****pGasketInfo = 0x%x***0x%x*** \n", pGasketInfo.ui8MipiEccError,pGasketInfo.ui8MipiCrcError);
        return ret;
    }

    return -1;
}

int IPU_V2500::GetSensorsName(cam_list_t * pName)
{
    if (NULL == pName)
    {
        LOGE("pName is invalid pointer\n");
        return -1;
    }

    std::list<std::pair<std::string, int> > sensors;
    std::list<std::pair<std::string, int> >::const_iterator it;
    int i = 0;
    pCamera->getSensor()->GetSensorNames(sensors);

    for(it = sensors.begin(); it != sensors.end(); it++)
    {
        strncpy(pName->name[i], it->first.c_str(), it->first.length());
        i++;
    }

    return IMG_SUCCESS;
}

int IPU_V2500::SetGAMCurve(cam_gamcurve_t *GamCurve)
{
    ISPC::ModuleGMA *pGMA = this->pCamera->pipeline->getModule<ISPC::ModuleGMA>();
    CI_MODULE_GMA_LUT glut;
    CI_CONNECTION *conn  = this->pCamera->getConnection();

    if (pGMA == NULL) {
        return -1;
    }

    if(GamCurve != NULL) {
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

int IPU_V2500::SetModuleOUTRawFlag(bool enable, CI_INOUT_POINTS DataExtractionPoint)
{
    int ret = 0;
    ISPC::ModuleOUT *out = this->pCamera->getModule<ISPC::ModuleOUT>();

    if (NULL == out)
    {
        LOGE("(%s:L%d) module out is NULL error !!!\n", __func__, __LINE__);
        return -1;
    }
    if (enable)
    {
        switch (pCamera->sensor->uiBitDepth)
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
    if (this->pCamera->setupModule(ISPC::STP_OUT) != IMG_SUCCESS)
    {
        LOGE("(%s:L%d) failed to setup OUT module\n", __func__, __LINE__);
        ret = -1;
    }

    return ret;
}

int IPU_V2500::GetDebugInfo(void *pInfo, int *ps32Size)
{
    ST_ISP_DBG_INFO *pISPInfo = NULL;

    pISPInfo = (ST_ISP_DBG_INFO *)pInfo;
    *ps32Size = sizeof(ST_ISP_DBG_INFO);

    ISPC::ModuleTNM *pIspTNM = \
        this->pCamera->pipeline->getModule<ISPC::ModuleTNM>();

    pISPInfo->s32CtxN = 1;
    pISPInfo->s32AEEn[0] = IsAEEnabled();
    pISPInfo->s32AWBEn[0] = IsAWBEnabled();

#ifdef INFOTM_HW_AWB_METHOD
    pISPInfo->s32HWAWBEn[0] = this->pAWB->bHwAwbEnable;
    LOGD("HWAWB=%d\n", this->pAWB->bHwAwbEnable);
#else
    pISPInfo->s32HWAWBEn[0] = -1;
#endif

    GetDayMode((enum cam_day_mode&)pISPInfo->s32DayMode[0]);
    GetMirror((enum cam_mirror_state&)pISPInfo->s32MirrorMode[0]);

    pISPInfo->s32TNMStaticCurve[0] = pIspTNM->bStaticCurve;

    pISPInfo->u32Exp[0] = this->pCamera->sensor->getExposure();
    pISPInfo->u32MinExp[0] = this->pCamera->sensor->getMinExposure();
    pISPInfo->u32MaxExp[0] = this->pCamera->sensor->getMaxExposure();
    LOGD("exp=%u min=%u max=%u\n",
        pISPInfo->u32Exp[0] , pISPInfo->u32MinExp[0], pISPInfo->u32MaxExp[0]);

    pISPInfo->f64Gain[0] = this->pCamera->sensor->getGain();
    pISPInfo->f64MinGain[0] = this->pCamera->sensor->getMinGain();
    pISPInfo->f64MaxGain[0] = this->pCamera->sensor->getMaxGain();
    LOGD("gain=%f min=%f max=%f\n",
        pISPInfo->f64Gain[0] , pISPInfo->f64MinGain[0], pISPInfo->f64MaxGain[0]);

    Port *pcap = this->GetPort("cap");
    pISPInfo->s32CapW = pcap->GetWidth();
    pISPInfo->s32CapH = pcap->GetHeight();
    LOGD("cap w =%d h =%d\n", pISPInfo->s32CapW, pISPInfo->s32CapH);

    Port *p = GetPort("out");
    pISPInfo->s32OutW = p->GetWidth();
    pISPInfo->s32OutH = p->GetHeight();
    LOGD("out w =%d h =%d\n", pISPInfo->s32OutW, pISPInfo->s32OutH);

    return IMG_SUCCESS;
}

