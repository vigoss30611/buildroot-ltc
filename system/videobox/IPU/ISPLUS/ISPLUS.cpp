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
#include "ISPLUS.h"
//#include <fstream>
#include "q3ispost.h"
#include "q3denoise.h"
#include <qsdk/sys.h>

extern "C" {
#include <qsdk/items.h>
}

typedef struct
{
    Port *p_port;
    ISPOST_UINT32 flag;
    Buffer *p_buf;
}local_port_t;

DYNAMIC_IPU(IPU_ISPLUS, "isplus");
#define SKIP_FRAME (1)
#define SETUP_PATH  ("/root/.ispddk/")
#define DEV_PATH    ("/dev/ddk_sensor")
#define SENSOR_MAX_NUM 2

using namespace std;
#ifdef MACRO_IPU_ISPLUS_FCE
#pragma message "\n\tenable isplus fce"
#else
#pragma message "\n\tdisable isplus fce"
#endif
#ifdef MACRO_IPU_ISPLUS_FCE_PARA
#pragma message "\n\tplus para on ->"
#pragma message MACRO_IPU_ISPLUS_FCE_PARA
#else
#pragma message "\n\tplus para off"
#endif

static const ISPOST_UINT32 g_dwGridSize[] = { 8, 16, 32 };
const IPU_ISPLUS::ipu_ispost_3d_dn_param IPU_ISPLUS::g_dnParamList[] = {
    {10, 10, 10, 5},
    {10, 10, 10, 4},
    {10, 10, 10, 3},
    {20, 20, 20, 5},
    {20, 20, 20, 4},
    {20, 20, 20, 3},
    {30, 30, 30, 5},
    {30, 30, 30, 4},
    {30, 30, 30, 3},
    {20, 10, 10, 3},
    {30, 20, 20, 3},
    {20, 20, 20, 2}
};

void IPU_ISPLUS::WorkLoop()
{
    Buffer dst;
    Buffer his;

    Port *p = GetPort("out");
    Port *phis = GetPort("his");
    Port *pcap = this->GetPort("cap");
    int ret;
    int ncapture =0 ;
    int preSt = runSt;
    //ISPC::ParameterList parameters;
    IMG_UINT32 ui32DnTargetIdx = 0;

    unsigned int i = 0, skip_fr = 0;
    int frames = 0;
    IMG_UINT64 timestamp = 0;
    IMG_UINT32 CRCStatus = 0;
    bool hasDE = false;
    bool hasSavedData = false;


    m_LastPortEnable = 0;
    m_IspostUser.ui32IspostCount = 0;

    TimePin tp;

    LOGD("isp thread start ============\n");
    while(IsInState(ST::Running)) {
        if (runSt == CAM_RUN_ST_CAPTURING)
        {
            m_iBufUseCnt = 0;
            ncapture ++;
            preSt = runSt;
            if (ncapture == 1)
            {
                LOGD("in capture mode switch from preview to capture\n");
                //this->pCamera->SetCpatureIQFlag(IMG_TRUE); //toedit

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

                if (CheckOutRes(this->ImgW, this->ImgH) < 0)
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

                LOGD("running here: ImgW = %d, ImgH =%d@@@\n",this->ImgW,  this->ImgH);
                pthread_mutex_lock(&mutex_x);
                if (m_psSaveDataParam)
                {
                    hasSavedData = true;
                    if (m_psSaveDataParam->fmt & (CAM_DATA_FMT_BAYER_FLX | CAM_DATA_FMT_BAYER_RAW))
                    {
                        ISPC::ModuleOUT *out = this->pCamera->getModule<ISPC::ModuleOUT>();

                        out->dataExtractionType = BAYER_RGGB_10;
                        out->dataExtractionPoint = CI_INOUT_BLACK_LEVEL;
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
                        ISPC::ModuleOUT *out = this->pCamera->getModule<ISPC::ModuleOUT>();

                        out->dataExtractionType = PXL_NONE;
                        out->dataExtractionPoint = CI_INOUT_NONE;
                        hasDE = false;
                    }
                }

                preSt = runSt;
                skip_fr = 0;
                ncapture = 0;//recover ncapture to 0, for next preview to capture.
                LOGD("Is here running from capture switch preview ##########: runSt =%d, preSt =%d\n", runSt, preSt);
                //this->pCamera->SetCpatureIQFlag(IMG_FALSE); //toedit
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

                if(p)
                {
                    this->ImgH = p->GetHeight();
                    this->ImgW = p->GetWidth();
                    this->pipH = p->GetPipHeight();
                    this->pipW = p->GetPipWidth();
                    this->pipX = p->GetPipX();
                    this->pipY = p->GetPipY();
                }

                if (CheckOutRes(this->ImgW, this->ImgH) < 0)
                {
                    LOGE("ISP output width size is not 64 align\n");
                    throw VBERR_RUNTIME;
                }


                if (0 != this->pipW && 0 != this->pipH)
                {
                    this->pCamera->pipeline->getMCPipeline()->sIIF.ui16ImagerSize[0] = this->pipW/CI_CFA_WIDTH;
                    this->pCamera->pipeline->getMCPipeline()->sIIF.ui16ImagerSize[1] = this->pipH/CI_CFA_WIDTH;
                    this->SetIIFCrop(pipH, pipW, pipX, pipY);
                }
                this->pCamera->setEncoderDimensions(p->GetWidth(), p->GetHeight());
                if (this->ImgW <= in_w && ImgH <= in_h)
                {
                    this->SetEncScaler(this->ImgW, this->ImgH);//Set output scaler by current port resolution;
                }
                else
                {
                    this->SetEncScaler(in_w, in_h);
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
                        LOGE("enqueue shot in isplus ipu thread failed.\n");
                        throw VBERR_RUNTIME;
                        return ;
                    }
                }

            }

            dst = p->GetBuffer();

            if (!WorkLoopInit())
                break;

            if (!WorkLoopGetBuffer())
                break;

            if (!WorkLoopTrigger())
                break;

            pCamera->enqueueShot();
            if (IMG_SUCCESS != ret)
            {
                LOGE("enqueue shot in v2500 ipu thread while loop failed.\n");
                throw VBERR_RUNTIME;
                return ;
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

            if (!WorkLoopCompleteCheck())
                break;


            //TODO
            //file.write((char *)frame.YUV.data, nsize);
            timestamp = m_pstShotFrame[m_iBufferID].metadata.timestamps.ui64CurSystemTS;
            dst.Stamp(timestamp);
            LOGD("system time is ****%lld**** \n",dst.fr_buf.timestamp);
            dst.fr_buf.phys_addr = (uint32_t)m_pstShotFrame[m_iBufferID].YUV.phyAddr;

            pthread_mutex_lock(&mutex_x);
            m_pstPrvData[m_iBufferID].iBufID = m_iBufferID;
            m_pstPrvData[m_iBufferID].state = SHOT_POSTED;
            m_postShots.push_back((void *)&m_pstPrvData[m_iBufferID]);
            pthread_mutex_unlock(&mutex_x);

            dst.fr_buf.priv = (int)&m_pstPrvData[m_iBufferID];

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
            pthread_mutex_unlock(&mutex_x);

            ret = pCamera->releaseShot(m_pstShotFrame[m_iBufferID]);
            if (IMG_SUCCESS != ret) {
                LOGE("failed to release shot\n");
                throw VBERR_RUNTIME;
                return ;
            }

            if (!WorkLoopPutBuffer())
                break;

            frames++;
            if(tp.Peek() > 1)
            {
                //LOGE("current fps = %dfps\n", frames);
                frames = 0;
                tp.Poke();
            }
        }
    }

    LOGE("IPU_ISPLUS thread exit ****************\n");
    runSt = CAM_RUN_ST_PREVIEWING;
}

IPU_ISPLUS::IPU_ISPLUS(std::string name, Json *js)
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
    this->pLBC = NULL;
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
    nBuffers = 1;

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
        pCamera = new ISPC::Camera(this->context, ISPC::Sensor::GetSensorId(psname), mode,
            js->GetObject("flip") ? js->GetInt("flip") : 0, i);
    }
    else
    {
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

    InitIspost(name, js);

    GetPort("out")->Bind(GetPort("in"));

    this->currentStatus = SENSOR_CREATE_SUCESS;
}

int IPU_ISPLUS::ConfigAutoControl()
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

    if (pLBC)
    {
        this->pLBC->enableControl(true);
        if ((!pTNM || !pTNM->isEnabled()) && (!pAE || !pAE->isEnabled()))
        {
             pLBC->setAllowHISConfig(true);
        }
    }

    if (pAWB)
    {
        pAWB->enableControl(true);
    }

    return IMG_SUCCESS;
}

int IPU_ISPLUS::AutoControl()
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
        else
        {
            flDefTargetBrightness = this->pAE->getOriTargetBrightness();
        }
    }

    if (!this->pAWB)
    {
        pAWB = new ISPC::ControlAWB_PID();
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

    if (!this->pLBC)
    {
        pLBC = new ISPC::ControlLBC();
        ret = this->pCamera->registerControlModule(pLBC);
        if(IMG_SUCCESS != ret)
        {
            LOGE("register pLBC module failed\n");
            delete pLBC;
            pLBC = NULL;
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

int IPU_ISPLUS::CheckOutRes(int imgW, int imgH)
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
            this->ImgW = imgH*((float)in_w/in_h);
        }
        else
        {
            this->ImgH = imgW*((float)in_h/in_w);
        }
    }

    LOGD("IPU_ISPLUS check output resolution imgW = %d, imgH = %d\n", this->ImgW, this->ImgH);
    return 0;
}

void IPU_ISPLUS::Prepare1()
{
    // allocate needed memory(reference frame/ snapshots, etc)
    // initialize the hardware
    // get input parameters from PortIn.Width/Height/PixelFormat

    // start work routine
    int fps = 30; //default 30
    SENSOR_STATUS status;
    cam_fps_range_t fps_range;
    ISPC::ModuleGMA *pGMA = pCamera->pipeline->getModule<ISPC::ModuleGMA>();
    Port *p = NULL;
    p= this->GetPort("out");
    ISPC::ParameterList parameters;
    ISPC::ParameterFileParser fileParser;
    parameters = fileParser.parseFile(this->pSetupFile);
    if (!parameters.validFlag)
    {
        LOGE("failed to get parameters from setup file.\n");
        throw VBERR_BADPARAM;
    }

    pCamera->getPipeline()->getCIPipeline()->config.bIsLinkMode = true;
    pCamera->getPipeline()->getCIPipeline()->config.ui32LinkYPhysAddr = 0x90001000;
    pCamera->getPipeline()->getCIPipeline()->config.ui32LinkUVPhysAddr = 0xb0001000;
    pCamera->getPipeline()->getCIPipeline()->config.ui32LinkEncBufSize = 4096;
    pCamera->getPipeline()->getCIPipeline()->config.ui32LinkStride = 4096;

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
        fps = p->GetFPS();
    }

    if (CheckOutRes(this->ImgW, this->ImgH) < 0)
    {
        LOGE("ISP output width size is not 64 align\n");
        throw VBERR_RUNTIME;
    }

    if (0 != this->pipW && 0 != this->pipH)
    {
        this->SetIIFCrop(pipH, pipW, pipX, pipY);
    }

    LOGE("in start method : H = %d, W = %d, fps=%d, in_w = %d, in_h=%d\n", this->ImgH, this->ImgW, fps, in_w, in_h);
    if (this->ImgW <= in_w && ImgH <= in_h)
    {
        this->SetEncScaler(this->ImgW, this->ImgH);//Set output scaler by current port resolution;
    }
    else
    {
        this->SetEncScaler(in_w, in_h);
    }
    if (pCamera->setupModules() != IMG_SUCCESS)
    {
        LOGE("failed to setup camera\n");
        throw VBERR_RUNTIME;
    }

    this->pCamera->setEncoderDimensions(this->ImgW,  this->ImgH);
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


    if (this->m_MinFps == 0 && this->m_MaxFps == 0) {
        // nothing to do
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

    this->pCamera->sensor->GetFlipMirror(status);
    fps = status.fCurrentFps;
    if (fps) {
        this->GetOwner()->SyncPathFps(fps);
    }
}

void IPU_ISPLUS::Prepare()
{
   Pixel::Format enPixelFormat;

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
    Prepare2();
    Prepare1();
}
void IPU_ISPLUS::Unprepare1()
{
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
    buffersIds.clear();
    m_postShots.clear();
}
void IPU_ISPLUS::Unprepare()
{
    Port *pIpuPort= NULL;

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
    if (m_pPortIn->IsEnabled())
    {
         m_pPortIn->FreeVirtualResource();
    }
    if (m_pPortUo->IsEnabled())
    {
         m_pPortUo->FreeVirtualResource();
    }
    if (m_pPortDn->IsEnabled())
    {
         m_pPortDn->FreeVirtualResource();
    }
    if (m_pPortSs0->IsEnabled())
    {
         m_pPortSs0->FreeVirtualResource();
    }
    if (m_pPortSs1->IsEnabled())
    {
         m_pPortSs1->FreeVirtualResource();
    }
    for(int i = 0; i < 8; i++)
    {
        if (m_pPortOv[i]->IsEnabled())
        {
             m_pPortOv[i]->FreeVirtualResource();
        }
    }

    Unprepare1();
    Unprepare2();
}
void IPU_ISPLUS::BuildPort(int imgW, int imgH)
{

    Port *p = NULL, *phis = NULL, *pcap = NULL;
    p = CreatePort("out", Port::Out);
    phis = CreatePort("his", Port::Out);
    pcap = CreatePort("cap", Port::Out);
    if(NULL != p)
    {
        this->ImgH = imgH;
        this->ImgW = imgW;
        printf("isp build port this->nBuffers = %d \n", this->nBuffers);
        p->SetBufferType(FRBuffer::Type::VACANT, this->nBuffers);
        p->SetResolution((int)imgW, (int)imgH);
        p->SetPixelFormat(Pixel::Format::NV21);
        p->SetFPS(30);    //FIXME!!!
        p->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);
    }

    if(NULL != phis)
    {
        phis->SetBufferType(FRBuffer::Type::FLOAT, 128*1024, 4*1024);
        p->SetPixelFormat(Pixel::Format::VAM);
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

int IPU_ISPLUS::ReturnShot(Buffer *pstBuf)
{
    struct ipu_v2500_private_data *pstPrvData;

    pthread_mutex_lock(&mutex_x);
    if ((pCamera == NULL) || (m_iBufUseCnt < 1)){
        LOGD("pCamera:%p m_iBufUseCnt:%d\n", pCamera, m_iBufUseCnt);
         pthread_mutex_unlock(&mutex_x);
        return -1;
    }
    pstPrvData = (struct ipu_v2500_private_data *)pstBuf->fr_buf.priv;

    if ((IMG_UINT32)pstPrvData->iBufID >= nBuffers)
    {
        LOGE("Buf ID over flow\n");
        pthread_mutex_unlock(&mutex_x);
        return -1;
    }

    LOGE("(%s:L%d) BufID =%d \n", __func__, __LINE__, pstPrvData->iBufID);

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

    pthread_mutex_unlock(&mutex_x);
    return 0;
}


void IPU_ISPLUS::SetOvPaletteTable(
        ST_OVERLAY_INFO* ov_info, ISPOST_UINT32 flag)
{
    int i = 1;
    if (ov_info == NULL)
        throw VBERR_FR_HANDLE;

    switch(flag)
    {
        case ISPOST_PT_OV1:
            i = 1;
            break;

        case ISPOST_PT_OV2:
            i = 2;
            break;

        case ISPOST_PT_OV3:
            i = 3;
            break;

        case ISPOST_PT_OV4:
            i = 4;
            break;

        case ISPOST_PT_OV5:
            i = 5;
            break;

        case ISPOST_PT_OV6:
            i = 6;
            break;

        case ISPOST_PT_OV7:
            i = 7;
            break;

        default:
            LOGE("The PT[0x%x] can not set ov palette\n", flag);
            i = 0;
            break;
    }

    if (i > 0)
    {
        LOGD("Ov%d:A[1]: 0x%x A[2]:0x%x A[3]:0x%x [4]:0x%x\n", i,
            ov_info->iPaletteTable[0], ov_info->iPaletteTable[1],
            ov_info->iPaletteTable[2], ov_info->iPaletteTable[3]);

        m_IspostUser.stOvInfo.stOvAlpha[i].stValue0.ui32Alpha = ov_info->iPaletteTable[0];
        m_IspostUser.stOvInfo.stOvAlpha[i].stValue1.ui32Alpha = ov_info->iPaletteTable[1];
        m_IspostUser.stOvInfo.stOvAlpha[i].stValue2.ui32Alpha = ov_info->iPaletteTable[2];
        m_IspostUser.stOvInfo.stOvAlpha[i].stValue3.ui32Alpha = ov_info->iPaletteTable[3];
    }

}

bool IPU_ISPLUS::WorkLoopInit()
{

    LOGD("Port IN:%d DN:%d, UO:%d SS0:%d SS1:%d OV:%d \n",
        m_pPortIn->IsEnabled(), m_pPortDn->IsEnabled(),
        m_pPortUo->IsEnabled(), m_pPortSs0->IsEnabled(),
        m_pPortSs1->IsEnabled(), m_pPortOv[0]->IsEnabled());
    LOGD("IsLcManual:%d LcManualVal:%d \n", m_IsLcManual, m_LcManualVal);
    LOGD("IsDnManual:%d DnManualVal:%d \n", m_IsDnManual, m_DnManualVal);
    LOGD("IsOvManual:%d OvManualVal:%d \n", m_IsOvManual, m_OvManualVal);


    m_PortEnable = m_PortGetBuf = 0;
    m_PortEnable |= (m_pPortIn->IsEnabled()) ? ISPOST_PT_IN : 0;
    //force lc working, otherwise error.
    if (m_IsLcManual)
        m_PortEnable |= (m_LcManualVal) ? ISPOST_PT_LC : 0;
    else
        m_PortEnable |= (m_pPortIn->IsEnabled()) ? ISPOST_PT_LC : 0;

    if (m_IsOvManual)
        m_PortEnable |= ((m_OvManualVal & 0x1) && m_pPortOv[0]->IsEnabled()) ? ISPOST_PT_OV0 : 0;
    else
        m_PortEnable |= (m_pPortOv[0]->IsEnabled()) ? ISPOST_PT_OV0 : 0;

    if (m_IsOvManual)
        m_PortEnable |= ((m_OvManualVal>>1 & 0x1) && m_pPortOv[1]->IsEnabled()) ? ISPOST_PT_OV1 : 0;
    else
        m_PortEnable |= (m_pPortOv[1]->IsEnabled()) ? ISPOST_PT_OV1 : 0;

    if (m_IsOvManual)
        m_PortEnable |= ((m_OvManualVal>>2 & 0x1) && m_pPortOv[2]->IsEnabled()) ? ISPOST_PT_OV2 : 0;
    else
        m_PortEnable |= (m_pPortOv[2]->IsEnabled()) ? ISPOST_PT_OV2 : 0;

    if (m_IsOvManual)
        m_PortEnable |= ((m_OvManualVal>>3 & 0x1) && m_pPortOv[3]->IsEnabled()) ? ISPOST_PT_OV3 : 0;
    else
        m_PortEnable |= (m_pPortOv[3]->IsEnabled()) ? ISPOST_PT_OV3 : 0;

    if (m_IsOvManual)
        m_PortEnable |= ((m_OvManualVal>>4 & 0x1) && m_pPortOv[4]->IsEnabled()) ? ISPOST_PT_OV4 : 0;
    else
        m_PortEnable |= (m_pPortOv[4]->IsEnabled()) ? ISPOST_PT_OV4 : 0;

    if (m_IsOvManual)
        m_PortEnable |= ((m_OvManualVal>>5 & 0x1) && m_pPortOv[5]->IsEnabled()) ? ISPOST_PT_OV5 : 0;
    else
        m_PortEnable |= (m_pPortOv[5]->IsEnabled()) ? ISPOST_PT_OV5 : 0;

    if (m_IsOvManual)
        m_PortEnable |= ((m_OvManualVal>>6 & 0x1) && m_pPortOv[6]->IsEnabled()) ? ISPOST_PT_OV6 : 0;
    else
        m_PortEnable |= (m_pPortOv[6]->IsEnabled()) ? ISPOST_PT_OV6 : 0;

    if (m_IsOvManual)
        m_PortEnable |= ((m_OvManualVal>>7 & 0x1) && m_pPortOv[7]->IsEnabled()) ? ISPOST_PT_OV7 : 0;
    else
        m_PortEnable |= (m_pPortOv[7]->IsEnabled()) ? ISPOST_PT_OV7 : 0;

    if (m_IsDnManual)
        m_PortEnable |= (m_DnManualVal && m_pPortDn->IsEnabled()) ? ISPOST_PT_DN : 0;
    else
        m_PortEnable |= (m_pPortDn->IsEnabled()) ? ISPOST_PT_DN : 0;
    m_PortEnable |= (m_pPortUo->IsEnabled()) ? ISPOST_PT_UO : 0;
    m_PortEnable |= (m_pPortSs0->IsEnabled()) ? ISPOST_PT_SS0 : 0;
    m_PortEnable |= (m_pPortSs1->IsEnabled()) ? ISPOST_PT_SS1 : 0;

    // update hw from port settings
    if (m_LastPortEnable != m_PortEnable)
    {
        if (!IspostInitial())
        {
            LOGE("IspostInitial Fail!\n");
            return false;
        }
        m_LastPortEnable = m_PortEnable;
    }

    LOGD("m_PortEnable 0x%lx \n", m_PortEnable);

    return true;
}

bool IPU_ISPLUS::WorkLoopGetBuffer()
{
    static int ErrorCount = 0;
    int StepCount = 0;
    int i = 0;
    const int max_port_set = 13;
    local_port_t PortSet[max_port_set] =
    {
        {m_pPortDn, ISPOST_PT_DN, &m_BufDn},
        {m_pPortOv[0], ISPOST_PT_OV0, &m_BufOv[0]},
        {m_pPortOv[1], ISPOST_PT_OV1, &m_BufOv[1]},
        {m_pPortOv[2], ISPOST_PT_OV2, &m_BufOv[2]},
        {m_pPortOv[3], ISPOST_PT_OV3, &m_BufOv[3]},
        {m_pPortOv[4], ISPOST_PT_OV4, &m_BufOv[4]},
        {m_pPortOv[5], ISPOST_PT_OV5, &m_BufOv[5]},
        {m_pPortOv[6], ISPOST_PT_OV6, &m_BufOv[6]},
        {m_pPortOv[7], ISPOST_PT_OV7, &m_BufOv[7]},
        {m_pPortUo, ISPOST_PT_UO, &m_BufUo},
        {m_pPortSs0, ISPOST_PT_SS0, &m_BufSs0},
        {m_pPortSs1, ISPOST_PT_SS1, &m_BufSs1},
        {NULL, 0}
    };
    local_port_t *pLocalPort = NULL;

Again:
    try
    {
        StepCount++;
        if (m_PortEnable & ISPOST_PT_IN)
        {
            if (m_pPortDn->IsEmbezzling() || m_pPortSs0->IsEmbezzling() || m_pPortSs1->IsEmbezzling()
                || m_pPortUo->IsEmbezzling())
                m_BufIn = m_pPortIn->GetBuffer();
            else
                m_BufIn = m_pPortIn->GetBuffer(&m_BufIn);
            m_BufIn.Stamp(); //for link mode, the timestamp also apply to UO, SS0, SS1, DN
            m_PortGetBuf |= ISPOST_PT_IN;
            if (m_PortEnable & ISPOST_PT_LC)
                m_PortGetBuf |= ISPOST_PT_LC;

            if (m_PortEnable & ISPOST_PT_DN)
            {
                IspostProcPrivateData((ipu_v2500_private_data_t*)m_BufIn.fr_buf.priv);
            }
        }
        for (i = 0; i < max_port_set - 1; ++i)
        {
            pLocalPort = &PortSet[i];
            if (pLocalPort
                && pLocalPort->p_port
                && pLocalPort->p_buf)
            {
                try
                {
                    if (m_PortEnable & pLocalPort->flag)
                    {
                        if (pLocalPort->p_port->IsEmbezzling())
                            (*pLocalPort->p_buf) = pLocalPort->p_port->GetBuffer(pLocalPort->p_buf);
                        else
                            (*pLocalPort->p_buf) = pLocalPort->p_port->GetBuffer();
                        m_PortGetBuf |= pLocalPort->flag;

                       if (pLocalPort->flag & ISPOST_PT_OV1_7)
                            SetOvPaletteTable((ST_OVERLAY_INFO*)((*pLocalPort->p_buf).fr_buf.priv), pLocalPort->flag);
                    }
                }
                catch(const char* err)
                {
                    if (!strncmp(err, VBERR_FR_INTERRUPTED, strlen(VBERR_FR_INTERRUPTED) - 1)) {
                        LOGE("videobox interrupted out 1.\n");
                        return false;
                    }
                    if (m_IspostUser.ui32IspostCount%60000 == 0)
                        LOGE("Waring: %s cannot get buffer\n", pLocalPort->p_port->GetName().c_str());
                    usleep(100);
                }
            }
        }
    }
    catch (const char* err)
    {
        usleep(20000);
        ErrorCount++;
        if (!strncmp(err, VBERR_FR_INTERRUPTED, strlen(VBERR_FR_INTERRUPTED) - 1)) {
            LOGE("videobox interrupted out 2.\n");
            return false;
        }
        if (StepCount == 1 && (ErrorCount % 500 == 0))
            LOGE("Warning: Thread Port In cannot get buffer, ErrorCnt: %d. StepCount: %d\n", ErrorCount, StepCount);
        goto Again;
    }

    LOGD("m_PortGetBuf=0x%lx \n",\
            m_PortGetBuf);

    return true;
}

bool IPU_ISPLUS::WorkLoopTrigger()
{
    if (m_PortGetBuf != 0)
    {
        // update hw from port settings
        if (0 == m_IspostUser.ui32IspostCount)
        {
            // ISPOST initialize.
            if (!IspostHwInit())
            {
                LOGE("=== IspostHwInit() failed! ===.\n");
                return false;
            }
        }
        if (!IspostHwUpdate())
        {
            LOGE("=== IspostHwUpdate failed! ===\n");
            return false;
        }
        //IspostDumpUserInfo();
        if (!IspostHwTrigger())
        {
            LOGE("=== IspostHwTrigger() failed! ===\n");
            return false;
        }
    }

    return true;
}
bool IPU_ISPLUS::WorkLoopPutBuffer()
{
        if (m_PortGetBuf & ISPOST_PT_SS1)
        {
            m_BufSs1.SyncStamp(&m_BufIn);
            m_pPortSs1->PutBuffer(&m_BufSs1);
        }
        if (m_PortGetBuf & ISPOST_PT_SS0)
        {
            m_BufSs0.SyncStamp(&m_BufIn);
            m_pPortSs0->PutBuffer(&m_BufSs0);
        }
        if (m_PortGetBuf & ISPOST_PT_UO)
        {
            m_BufUo.SyncStamp(&m_BufIn);
            m_pPortUo->PutBuffer(&m_BufUo);
        }
        if (m_PortGetBuf & ISPOST_PT_DN)
        {
            m_BufDn.SyncStamp(&m_BufIn);
            m_pPortDn->PutBuffer(&m_BufDn);
        }

        if (m_PortGetBuf & ISPOST_PT_OV0)
        {
            m_pPortOv[0]->PutBuffer(&m_BufOv[0]);
        }
        if (m_PortGetBuf & ISPOST_PT_OV1)
        {
            m_pPortOv[1]->PutBuffer(&m_BufOv[1]);
        }
        if (m_PortGetBuf & ISPOST_PT_OV2)
        {
            m_pPortOv[2]->PutBuffer(&m_BufOv[2]);
        }
        if (m_PortGetBuf & ISPOST_PT_OV3)
        {
            m_pPortOv[3]->PutBuffer(&m_BufOv[3]);
        }
        if (m_PortGetBuf & ISPOST_PT_OV4)
        {
            m_pPortOv[4]->PutBuffer(&m_BufOv[4]);
        }
        if (m_PortGetBuf & ISPOST_PT_OV5)
        {
            m_pPortOv[5]->PutBuffer(&m_BufOv[5]);
        }
        if (m_PortGetBuf & ISPOST_PT_OV6)
        {
            m_pPortOv[6]->PutBuffer(&m_BufOv[6]);
        }
        if (m_PortGetBuf & ISPOST_PT_OV7)
        {
            m_pPortOv[7]->PutBuffer(&m_BufOv[7]);
        }

        if (m_PortGetBuf & ISPOST_PT_IN)
        {
            m_pPortIn->PutBuffer(&m_BufIn);
        }

    return true;
}

bool IPU_ISPLUS::WorkLoopCompleteCheck()
{
    if (m_IspostUser.stCtrl0.ui1EnIspL)
    {
        if (m_PortGetBuf != 0)
        {
            if (!IspostHwCompleteCheck())
            {
                LOGE("=== IspostHwCompleteCheck() failed! ===\n");
                return false;
            }

        }
    }

    return true;
}
bool IPU_ISPLUS::InitIspost(std::string name, Json* js)
{
    int i, Value;
    char* pszString;
    char szItemName[80];
    int hasGridFile = 0;
    soc_info_t s_info;


    m_pJsonMain = js;
    for (i = 0; i < IPU_ISPOST_GRIDDATA_MAX; i++)
    {
        m_pszLcGridFileName[i] = NULL;
        m_pszLcGeometryFileName[i] = NULL;
        m_pLcGridBuf[i] = NULL;
        m_pLcGeometryBuf[i] = NULL;
    }
    for (i = 0; i < OVERLAY_CHNL_MAX; i++)
    {
        m_pszOvImgFileName[i] = NULL;
        m_pOvImgBuf[i] = NULL;
    }
    m_Buffers = 3;
    m_CropX = m_CropY = m_CropW = m_CropH = m_Rotate = 0; /* lc_grid_count =*/
    grid_target_index_max = 1;
    m_IsFisheye = 0;
    m_IsLineBufEn =0;
    m_IsDnManual = 0;
    m_DnManualVal = 0;
    m_IsLcManual = 0;
    m_LcManualVal = 0;
    m_IsOvManual = 0;
    m_OvManualVal = 0;
    memset(&m_Dns3dAttr, 0, sizeof(m_Dns3dAttr));
    m_Dns3dAttr.y_threshold = g_dnParamList[0].y_threshold;
    m_Dns3dAttr.u_threshold = g_dnParamList[0].u_threshold;
    m_Dns3dAttr.v_threshold = g_dnParamList[0].v_threshold;
    m_Dns3dAttr.weight = g_dnParamList[0].weight;

    pthread_mutex_init( &m_PipMutex, NULL);
    IspostDefaultParameter();
    pthread_mutex_init(&m_FceLock, NULL);

    //----------------------------------------------------------------------------------------------
    // Parse IPU parameter
    if (m_pJsonMain != NULL)
    {
        if (IspostJsonGetInt("buffers", Value))
            m_Buffers = Value;
        if (IspostJsonGetInt("crop_x", Value))
            m_CropX = Value;
        if (IspostJsonGetInt("crop_y", Value))
            m_CropY = Value;
        if (IspostJsonGetInt("crop_w", Value))
            m_CropW = Value;
        if (IspostJsonGetInt("crop_h", Value))
            m_CropH = Value;
        if (IspostJsonGetInt("rotate", Value))
            m_Rotate = Value;
        if (IspostJsonGetInt("fisheye", Value)) {
            m_IsFisheye = (1 == Value) ? (1) : (0);
            if (m_IsFisheye) {
                m_IspostUser.stLcInfo.stPxlCacheMode.ui1BurstLenSet = (BURST_LEN_4QW_FISHEYE);
                m_IspostUser.stLcInfo.stPxlCacheMode.ui2CacheDataShapeSel = (CCH_DAT_SHAPE_3);
            } else {
                m_IspostUser.stLcInfo.stPxlCacheMode.ui1BurstLenSet = (BURST_LEN_8QW_BARREL);
                m_IspostUser.stLcInfo.stPxlCacheMode.ui2CacheDataShapeSel = (CCH_DAT_SHAPE_1);
            }
        }
        if (IspostJsonGetInt("line_buffer_en", Value)) {
            m_IsLineBufEn = (1 == Value) ? (1) : (0);
            m_IspostUser.stLcInfo.stGridBufInfo.ui8LineBufEnable = m_IsLineBufEn;
        }
        //----------------------------------------------------------------------------------------------
        // Parse lc_config parameter
        for (i = 0; i < IPU_ISPOST_GRIDDATA_MAX; i++)
        {
            sprintf(szItemName, "lc_grid_file_name%d", i + 1);
            if (IspostJsonGetString(szItemName, pszString))
            {
                m_pszLcGridFileName[i] = strdup(pszString);
                hasGridFile = 1;
            }
            sprintf(szItemName, "lc_geometry_file_name%d", i + 1);
            if (IspostJsonGetString(szItemName, pszString))
                m_pszLcGeometryFileName[i] = strdup(pszString);
        }
        if (IspostJsonGetInt("lc_enable", Value))
        {
            m_IsLcManual = 1;
            m_LcManualVal = Value;
            if (m_LcManualVal && !hasGridFile)
                LOGE("Error: No grid files \n");
        }
        if(system_get_soc_type(&s_info) == 0)
        {
            if(s_info.chipid == 0x3420010 || s_info.chipid == 0x3420310)//disable lc function for Q3420P and C20.
            {
                m_IsLcManual = 1;
                m_LcManualVal = 0;
                LOGE("force disable lc function.\n");
            }
        }
        if (IspostJsonGetInt("lc_grid_target_index", Value))
            m_IspostUser.stLcInfo.stGridBufInfo.ui32GridTargetIdx = Value;
//        if (IspostJsonGetInt("lc_grid_count", Value))
//        {
//            lc_grid_count = Value;
//            if (lc_grid_count < 0)
//                lc_grid_count = 0;
//        }
        if (IspostJsonGetInt("lc_grid_line_buf_enable", Value))
            m_IspostUser.stLcInfo.stGridBufInfo.ui8LineBufEnable = Value;
        if (IspostJsonGetInt("lc_cbcr_swap", Value))
            m_IspostUser.stLcInfo.stPxlCacheMode.ui1CBCRSwap = Value;
        if (IspostJsonGetInt("lc_scan_mode_scan_w", Value))
            m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanW = Value;
        if (IspostJsonGetInt("lc_scan_mode_scan_h", Value))
            m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanH = Value;
        if (IspostJsonGetInt("lc_scan_mode_scan_m", Value))
            m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanM = Value;
        if (IspostJsonGetInt("lc_fill_y", Value))
            m_IspostUser.stLcInfo.stBackFillColor.ui8Y = Value;
        if (IspostJsonGetInt("lc_fill_cb", Value))
            m_IspostUser.stLcInfo.stBackFillColor.ui8CB = Value;
        if (IspostJsonGetInt("lc_fill_cr", Value))
            m_IspostUser.stLcInfo.stBackFillColor.ui8CR = Value;

        //----------------------------------------------------------------------------------------------
        // Parse ov_config parameter
        if (IspostJsonGetInt("ov_enable", Value))
        {
            m_IsOvManual = 1;
            m_OvManualVal = Value;//ov enable bitmap [7][6][5][4][3][2][1][0]
        }
        if (IspostJsonGetInt("ov_mode", Value))
            m_IspostUser.stOvInfo.stOverlayMode.ui32OvMode = Value;
        if (IspostJsonGetInt("ov_img_x", Value))
            m_IspostUser.stOvInfo.stOvOffset[0].ui16X = Value;
        if (IspostJsonGetInt("ov_img_y", Value))
            m_IspostUser.stOvInfo.stOvOffset[0].ui16Y = Value;
        if (IspostJsonGetInt("ov_img_w", Value))
            m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Width = Value;
        if (IspostJsonGetInt("ov_img_h", Value))
            m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Height = Value;
        if (IspostJsonGetInt("ov_img_s", Value))
            m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Stride = Value;
        for (i = 1; i < OVERLAY_CHNL_MAX; i++)
        {
            sprintf(szItemName, "ov%d_img_x", i );
            if (IspostJsonGetInt(szItemName, Value))
                m_IspostUser.stOvInfo.stOvOffset[i].ui16X = Value;

            sprintf(szItemName, "ov%d_img_y", i);
            if (IspostJsonGetInt(szItemName, Value))
                m_IspostUser.stOvInfo.stOvOffset[i].ui16Y = Value;

            sprintf(szItemName, "ov%d_img_w", i);
            if (IspostJsonGetInt(szItemName, Value))
                m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Width = Value;

            sprintf(szItemName, "ov%d_img_h", i);
            if (IspostJsonGetInt(szItemName, Value))
            m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Height = Value;

            sprintf(szItemName, "ov%d_img_s", i);
            if (IspostJsonGetInt(szItemName, Value))
                m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Stride = Value;

            sprintf(szItemName, "ov%d_alpha0", i);
            if (IspostJsonGetInt(szItemName, Value))
                m_IspostUser.stOvInfo.stOvAlpha[i].stValue0.ui32Alpha = Value;

            sprintf(szItemName, "ov%d_alpha1", i);
            if (IspostJsonGetInt(szItemName, Value))
                m_IspostUser.stOvInfo.stOvAlpha[i].stValue1.ui32Alpha = Value;

            sprintf(szItemName, "ov%d_alpha2", i);
            if (IspostJsonGetInt(szItemName, Value))
                m_IspostUser.stOvInfo.stOvAlpha[i].stValue2.ui32Alpha = Value;

            sprintf(szItemName, "ov%d_alpha3", i);
            if (IspostJsonGetInt(szItemName, Value))
                m_IspostUser.stOvInfo.stOvAlpha[i].stValue3.ui32Alpha = Value;

            sprintf(szItemName, "ov_img_file_name%d", i);
            if (IspostJsonGetString(szItemName, pszString))
                m_pszOvImgFileName[i] = strdup(pszString);
        }

        //----------------------------------------------------------------------------------------------
        // Parse dn_config parameter
        if (IspostJsonGetInt("dn_enable", Value))
        {
            m_IsDnManual = 1;
            m_DnManualVal = Value;
        }
        if (IspostJsonGetInt("dn_target_index", Value))
            m_IspostUser.stDnInfo.ui32DnTargetIdx = Value;
    }

    if (m_Buffers <= 0 || m_Buffers > 20)
    {
        LOGE("the buffers number is over.\n");
        throw VBERR_BADPARAM;
    }

    if (((m_CropX < 0) || (m_CropX > 8192) || (m_CropX % 2 != 0)) ||
        (m_CropY < 0) || (m_CropY > 8192) || (m_CropY % 2 != 0) ||
        (m_CropW < 0) || (m_CropW > 8192) || (m_CropW % 8 != 0) ||
        (m_CropH < 0) || (m_CropH > 8192) || (m_CropH % 8 != 0))
    {
        LOGE("the Crop(x,y,w,h) has to be a 8 multiple value.\n");
        throw VBERR_BADPARAM;
    }

    //----------------------------------------------------------------------------------------------
    // Create ISPOST support port
    m_pPortIn = CreatePort("in", Port::In);
    m_pPortOv[0] = CreatePort("ov0", Port::In);
    m_pPortOv[1] = CreatePort("ov1", Port::In);
    m_pPortOv[2] = CreatePort("ov2", Port::In);
    m_pPortOv[3] = CreatePort("ov3", Port::In);
    m_pPortOv[4] = CreatePort("ov4", Port::In);
    m_pPortOv[5] = CreatePort("ov5", Port::In);
    m_pPortOv[6] = CreatePort("ov6", Port::In);
    m_pPortOv[7] = CreatePort("ov7", Port::In);
    m_pPortUo = CreatePort("uo", Port::Out);
    m_pPortDn = CreatePort("dn", Port::Out);
    m_pPortSs0 = CreatePort("ss0", Port::Out);
    m_pPortSs1 = CreatePort("ss1", Port::Out);
    for (i = 0; i < 8; i++)
        m_pPortOv[i]->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_StopNeeded);

    m_pPortIn->SetPipInfo(m_CropX, m_CropY, m_CropW, m_CropH);
    LOGD("Crop Info: %d, %d, %d, %d\n", m_pPortIn->GetPipX(), m_pPortIn->GetPipY(), m_pPortIn->GetPipWidth(), m_pPortIn->GetPipHeight());

    m_IspostUser.stCtrl0.ui1EnDN = ISPOST_ENABLE;
    //m_pPortDn->SetBufferType(FRBuffer::Type::FIXED, m_Buffers);
    m_pPortDn->SetBufferType(FRBuffer::Type::FIXED, 1);
    m_pPortDn->SetResolution(1920, 1088);
    m_pPortDn->SetPixelFormat(Pixel::Format::NV12);
    m_pPortDn->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_Forbidden);
    //force dn enabled, otherwise error.
    m_DnManualVal = 1;
    if (m_DnManualVal)
    {
        // export PortDn if dn function is enabled, which will
        // make system allocate buffer for PortDn
        m_pPortDn->Export();
    }

    m_pPortUo->SetBufferType(FRBuffer::Type::FIXED, m_Buffers);
    m_pPortUo->SetResolution(1920, 1088);
    m_pPortUo->SetPixelFormat(Pixel::Format::NV12);
    m_pPortUo->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);

    m_pPortSs0->SetBufferType(FRBuffer::Type::FIXED, m_Buffers);
    m_pPortSs0->SetResolution(1280, 720);
    m_pPortSs0->SetPixelFormat(Pixel::Format::NV12);
    m_pPortSs0->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);

    m_pPortSs1->SetBufferType(FRBuffer::Type::FIXED, m_Buffers);
    m_pPortSs1->SetResolution(640, 480);
    m_pPortSs1->SetPixelFormat(Pixel::Format::NV12);
    m_pPortSs1->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);

    ispost_open();

    m_pJsonMain = NULL;

    LOGE("IPU_ISPLUS() end\n");
    return true;
}

void IPU_ISPLUS::DeInitIspost()
{
    int i;


    ispost_close();
    for (i = 0; i < IPU_ISPOST_GRIDDATA_MAX; i++)
    {
        if (m_pszLcGridFileName[i] != NULL)
            free(m_pszLcGridFileName[i]);
        if (m_pszLcGeometryFileName[i] != NULL)
            free(m_pszLcGeometryFileName[i]);
        if (m_pLcGridBuf[i] != NULL)
            delete m_pLcGridBuf[i];
        if (m_pLcGeometryBuf[i] != NULL)
            delete m_pLcGeometryBuf[i];
    }
    for (i = 0; i < OVERLAY_CHNL_MAX; i++)
    {
        if (m_pszOvImgFileName[i] != NULL)
            free(m_pszOvImgFileName[i]);
        if (m_pOvImgBuf[i] != NULL)
            delete m_pOvImgBuf[i];
    }
    pthread_mutex_destroy( &m_PipMutex);
    pthread_mutex_destroy(&m_FceLock);
}

int IPU_ISPLUS::IspostFormatPipInfo(struct v_pip_info *vpinfo)
{
    int idx = 0;
    int dn_w, dn_h;
    int ov_w, ov_h;
    dn_w = m_pPortDn->GetWidth() - 10;
    dn_h = m_pPortDn->GetHeight() - 10;

    if(strncmp(vpinfo->portname, "ov0", 3) == 0)
        idx = 0;
    else if(strncmp(vpinfo->portname, "ov1", 3) == 0)
        idx = 1;
    else if(strncmp(vpinfo->portname, "ov2", 3) == 0)
        idx = 2;
    else if(strncmp(vpinfo->portname, "ov3", 3) == 0)
        idx = 3;
    else if(strncmp(vpinfo->portname, "ov4", 3) == 0)
        idx = 4;
    else if(strncmp(vpinfo->portname, "ov5", 3) == 0)
        idx = 5;
    else if(strncmp(vpinfo->portname, "ov6", 3) == 0)
        idx = 6;
    else if(strncmp(vpinfo->portname, "ov7", 3) == 0)
        idx = 7;
    else
        return 0;

    ov_w = m_pPortOv[idx]->GetWidth();
    ov_h = m_pPortOv[idx]->GetHeight();

    if (ov_w + vpinfo->x >= dn_w)
        vpinfo->x = dn_w - ov_w ;

    if (ov_h + vpinfo->y >= dn_h)
        vpinfo->y = dn_h - ov_h;

    if (vpinfo->x < 0)
        vpinfo->x = 0;

    if (vpinfo->y < 0)
        vpinfo->y = 0;

    if (idx == 0) {
        vpinfo->x -= vpinfo->x%4;
        vpinfo->y -= vpinfo->y%4;
    }

    LOGD("FormatPipInfo %s X=%d Y=%d W=%d H=%d\n",vpinfo->portname , vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);

return 0;
}

void IPU_ISPLUS::IspostAdjustOvPos(int ov_idx)
{
    int dn_w, dn_h;
    int ov_w, ov_h;
    int ov_x, ov_y;

    dn_w = m_pPortDn->GetWidth() - 10;
    dn_h = m_pPortDn->GetHeight() - 10;

    ov_w = m_pPortOv[ov_idx]->GetWidth();
    ov_h = m_pPortOv[ov_idx]->GetHeight();

    ov_x = m_pPortOv[ov_idx]->GetPipX();
    ov_y = m_pPortOv[ov_idx]->GetPipY();

    if (ov_w + ov_x >= dn_w)
        ov_x = dn_w - ov_w ;

    if (ov_h + ov_y >= dn_h)
        ov_y = dn_h - ov_h;

    if (ov_x < 0)
        ov_x = 0;

    if (ov_y < 0)
        ov_y = 0;

    if (ov_idx == 0) {
        ov_x -= ov_x%4;
        ov_y -= ov_y%4;
    }

    m_pPortOv[ov_idx]->SetPipInfo(ov_x, ov_y, 0, 0);
}

int IPU_ISPLUS::IspostSetPipInfo(struct v_pip_info *vpinfo)
{
    IspostFormatPipInfo(vpinfo);
    pthread_mutex_lock( &m_PipMutex);
    {
        if(strncmp(vpinfo->portname, "dn", 2) == 0)
            m_pPortDn->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "uo", 2) == 0)
            m_pPortUo->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "ss0", 3) == 0)
            m_pPortSs0->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "ss1", 3) == 0)
            m_pPortSs1->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "in", 2) == 0)
            m_pPortIn->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "ov0", 3) == 0)
            m_pPortOv[0]->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "ov1", 3) == 0)
            m_pPortOv[1]->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "ov2", 3) == 0)
            m_pPortOv[2]->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "ov3", 3) == 0)
            m_pPortOv[3]->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "ov4", 3) == 0)
            m_pPortOv[4]->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "ov5", 3) == 0)
            m_pPortOv[5]->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "ov6", 3) == 0)
            m_pPortOv[6]->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "ov7", 3) == 0)
            m_pPortOv[7]->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
    }
    pthread_mutex_unlock( &m_PipMutex);

    LOGD("SetPipInfo %s X=%d Y=%d W=%d H=%d\n",vpinfo->portname , vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);

    return 0;
}

int IPU_ISPLUS::IspostGetPipInfo(struct v_pip_info *vpinfo)
{
    if(strncmp(vpinfo->portname, "dn", 2) == 0) {
        vpinfo->x = m_pPortDn->GetPipX();
        vpinfo->y = m_pPortDn->GetPipY();
        vpinfo->w = m_pPortDn->GetWidth();
        vpinfo->h = m_pPortDn->GetHeight();
    } else if(strncmp(vpinfo->portname, "uo", 2) == 0) {
        vpinfo->x = m_pPortUo->GetPipX();
        vpinfo->y = m_pPortUo->GetPipY();
        vpinfo->w = m_pPortUo->GetWidth();
        vpinfo->h = m_pPortUo->GetHeight();
    } else if(strncmp(vpinfo->portname, "ss0", 3) == 0) {
        vpinfo->x = m_pPortSs0->GetPipX();
        vpinfo->y = m_pPortSs0->GetPipY();
        vpinfo->w = m_pPortSs0->GetWidth();
        vpinfo->h = m_pPortSs0->GetHeight();
    }else if(strncmp(vpinfo->portname, "ss1", 3) == 0) {
        vpinfo->x = m_pPortSs1->GetPipX();
        vpinfo->y = m_pPortSs1->GetPipY();
        vpinfo->w = m_pPortSs1->GetWidth();
        vpinfo->h = m_pPortSs1->GetHeight();
    } else if(strncmp(vpinfo->portname, "in", 2) == 0) {
        vpinfo->x = m_pPortIn->GetPipX();
        vpinfo->y = m_pPortIn->GetPipY();
        vpinfo->w = m_pPortIn->GetWidth();
        vpinfo->h = m_pPortIn->GetHeight();
    } else if(strncmp(vpinfo->portname, "ov0", 3) == 0) {
        vpinfo->x = m_pPortOv[0]->GetPipX();
        vpinfo->y = m_pPortOv[0]->GetPipY();
        vpinfo->w = m_pPortOv[0]->GetWidth();
        vpinfo->h = m_pPortOv[0]->GetHeight();
    }else if(strncmp(vpinfo->portname, "ov1", 3) == 0) {
        vpinfo->x = m_pPortOv[1]->GetPipX();
        vpinfo->y = m_pPortOv[1]->GetPipY();
        vpinfo->w = m_pPortOv[1]->GetWidth();
        vpinfo->h = m_pPortOv[1]->GetHeight();
    }else if(strncmp(vpinfo->portname, "ov2", 3) == 0) {
        vpinfo->x = m_pPortOv[2]->GetPipX();
        vpinfo->y = m_pPortOv[2]->GetPipY();
        vpinfo->w = m_pPortOv[2]->GetWidth();
        vpinfo->h = m_pPortOv[2]->GetHeight();
    } else if(strncmp(vpinfo->portname, "ov3", 3) == 0) {
        vpinfo->x = m_pPortOv[3]->GetPipX();
        vpinfo->y = m_pPortOv[3]->GetPipY();
        vpinfo->w = m_pPortOv[3]->GetWidth();
        vpinfo->h = m_pPortOv[3]->GetHeight();
    } else if(strncmp(vpinfo->portname, "ov4", 3) == 0) {
        vpinfo->x = m_pPortOv[4]->GetPipX();
        vpinfo->y = m_pPortOv[4]->GetPipY();
        vpinfo->w = m_pPortOv[4]->GetWidth();
        vpinfo->h = m_pPortOv[4]->GetHeight();
    } else if(strncmp(vpinfo->portname, "ov5", 3) == 0) {
        vpinfo->x = m_pPortOv[5]->GetPipX();
        vpinfo->y = m_pPortOv[5]->GetPipY();
        vpinfo->w = m_pPortOv[5]->GetWidth();
        vpinfo->h = m_pPortOv[5]->GetHeight();
    } else if(strncmp(vpinfo->portname, "ov6", 3) == 0) {
        vpinfo->x = m_pPortOv[6]->GetPipX();
        vpinfo->y = m_pPortOv[6]->GetPipY();
        vpinfo->w = m_pPortOv[6]->GetWidth();
        vpinfo->h = m_pPortOv[6]->GetHeight();
    }else if(strncmp(vpinfo->portname, "ov7", 3) == 0) {
        vpinfo->x = m_pPortOv[7]->GetPipX();
        vpinfo->y = m_pPortOv[7]->GetPipY();
        vpinfo->w = m_pPortOv[7]->GetWidth();
        vpinfo->h = m_pPortOv[7]->GetHeight();
    }

    LOGD("GetPipInfo %s X=%d Y=%d W=%d H=%d\n",vpinfo->portname ,
             vpinfo->x,
             vpinfo->y,
             vpinfo->w,
             vpinfo->h);

    return 0;
}

void IPU_ISPLUS::Prepare2()
{
    int i;
    Buffer buf;
    char szBufName[20];
    bool Ret;


    m_IspostUser.stCtrl0.ui1EnIspL = ISPOST_ENABLE;
    IspostCheckParameter();

    //Load LcGridBuf
    for (i = 0; i < IPU_ISPOST_GRIDDATA_MAX; i++)
    {
        if (m_pszLcGridFileName[i] != NULL)
        {
            sprintf(szBufName, "LcGridBuf%d", i + 1);
            m_IspostUser.stLcInfo.stGridBufInfo.ui8Size = IspostParseGirdSize(m_pszLcGridFileName[i]);
            Ret = IspostLoadBinaryFile(szBufName, &m_pLcGridBuf[i], m_pszLcGridFileName[i]);
            LOGE("m_pszLcGridFileName[%d]: %s, GridSize: %d, Ret: %d\n", i, m_pszLcGridFileName[i], m_IspostUser.stLcInfo.stGridBufInfo.ui8Size, Ret);
        }

        if (m_pszLcGeometryFileName[i] != NULL)
        {
            sprintf(szBufName, "LcGeometryBuf%d", i + 1);
            Ret = IspostLoadBinaryFile(szBufName, &m_pLcGeometryBuf[i], m_pszLcGeometryFileName[i]);
            LOGE("m_pszLcGeometryFileName[%d]: %s, Ret: %d\n", i, m_pszLcGeometryFileName[i], Ret);
        }
    }
    if (m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanW > m_IspostUser.stLcInfo.stGridBufInfo.ui8Size) {
        m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanW = m_IspostUser.stLcInfo.stGridBufInfo.ui8Size;
    }
    if (m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanH > m_IspostUser.stLcInfo.stGridBufInfo.ui8Size) {
        m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanH = m_IspostUser.stLcInfo.stGridBufInfo.ui8Size;
    }

#if IPU_ISPOST_LC_ROTATE_DEBUG
    //=============================================================
    // Remember you have put normal grid data into array index 0,
    // and put rotate 90 degree grid data into array index 1,
    // finally put rotate 270 degree grid data into array index 2.
    //=============================================================
    if (90 == m_Rotate) {
        i = 1;
    } else if (270 == m_Rotate){
        i = 2;
    } else {
        i = 0;
    }
#else
    i = 0;
#endif

    if(m_pLcGridBuf[i] != NULL)
    {
        buf = m_pLcGridBuf[i]->GetBuffer();
        m_IspostUser.stLcInfo.stGridBufInfo.ui32BufAddr = buf.fr_buf.phys_addr;
        m_IspostUser.stLcInfo.stGridBufInfo.ui32BufLen = buf.fr_buf.size;
        m_pLcGridBuf[i]->PutBuffer(&buf);
    }

    if(m_pLcGeometryBuf[i] != NULL)
    {
        buf = m_pLcGeometryBuf[i]->GetBuffer();
        m_IspostUser.stLcInfo.stGridBufInfo.ui32GeoBufAddr = (ISPOST_UINT32)buf.fr_buf.phys_addr;
        m_IspostUser.stLcInfo.stGridBufInfo.ui32GeoBufLen = buf.fr_buf.size;
        m_pLcGeometryBuf[i]->PutBuffer(&buf);
    }
    m_IspostUser.stCtrl0.ui1EnOV = ISPOST_DISABLE;
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV0 = 0;
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV1 = 0;
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV2 = 0;
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV3 = 0;
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV4 = 0;
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV5 = 0;
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV6 = 0;
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV7 = 0;

#ifdef IPU_ISPOST_OVERLAY_TEST
    m_IspostUser.stCtrl0.ui1EnOV = ISPOST_ENABLE;
    m_IspostUser.stOvInfo.stOverlayMode.ui2OVM = OVERLAY_MODE_YUV;
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV0 = 1;
    m_IspostUser.stOvInfo.stOvOffset[0].ui16X = 64;
    m_IspostUser.stOvInfo.stOvOffset[0].ui16Y = 64;
    m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Width = 128;
    m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Height = 64;
    m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Stride = 128;

    Ret = IspostLoadBinaryFile("OvImgBuf0", &m_pOvImgBuf[0], "/root/.ispost/ov_fpga_128x64.bin");
    buf = m_pOvImgBuf[0]->GetBuffer();
    m_IspostUser.stOvInfo.stOvImgInfo[0].ui32YAddr = buf.fr_buf.phys_addr;
    IspostCalcPlanarAddress(m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Width, m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Height,
                        m_IspostUser.stOvInfo.stOvImgInfo[0].ui32YAddr,  m_IspostUser.stOvInfo.stOvImgInfo[0].ui32UVAddr);
    m_pOvImgBuf[0]->PutBuffer(&buf);

    Ret = IspostLoadBinaryFile("OvImgBuf1", &m_pOvImgBuf[1], "/root/.ispost/Overlay_1-7_Yuv_Alpha_1920x1080.bin");
    // Overlay channel 1.
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV1 = 0;
    buf = m_pOvImgBuf[1]->GetBuffer();
    m_IspostUser.stOvInfo.stOvImgInfo[1].ui32YAddr = buf.fr_buf.phys_addr;
    m_pOvImgBuf[1]->PutBuffer(&buf);
    m_IspostUser.stOvInfo.stOvImgInfo[1].ui16Stride = 1920 / 4;
    m_IspostUser.stOvInfo.stOvImgInfo[1].ui16Width = 192;
    m_IspostUser.stOvInfo.stOvImgInfo[1].ui16Height = 108;
    m_IspostUser.stOvInfo.stOvOffset[1].ui16X = 320;
    m_IspostUser.stOvInfo.stOvOffset[1].ui16Y = 32;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue0.ui8Alpha3 = 0x33;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue0.ui8Alpha2 = 0x00;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue0.ui8Alpha1 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue0.ui8Alpha0 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue1.ui8Alpha3 = 0x77;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue1.ui8Alpha2 = 0x40;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue1.ui8Alpha1 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue1.ui8Alpha0 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue2.ui8Alpha3 = 0xbb;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue2.ui8Alpha2 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue2.ui8Alpha1 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue2.ui8Alpha0 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue3.ui8Alpha3 = 0xff;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue3.ui8Alpha2 = 0xc0;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue3.ui8Alpha1 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue3.ui8Alpha0 = 0xcc;
    // Overlay channel 2.
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV2 = 0;
    buf = m_pOvImgBuf[1]->GetBuffer();
    m_IspostUser.stOvInfo.stOvImgInfo[2].ui32YAddr = buf.fr_buf.phys_addr;
    m_pOvImgBuf[1]->PutBuffer(&buf);
    m_IspostUser.stOvInfo.stOvImgInfo[2].ui16Stride = 1920 / 4;
    m_IspostUser.stOvInfo.stOvImgInfo[2].ui16Width = 192;
    m_IspostUser.stOvInfo.stOvImgInfo[2].ui16Height = 108;
    m_IspostUser.stOvInfo.stOvOffset[2].ui16X = 64;
    m_IspostUser.stOvInfo.stOvOffset[2].ui16Y = 140;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue0.ui8Alpha3 = 0x33;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue0.ui8Alpha2 = 0x00;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue0.ui8Alpha1 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue0.ui8Alpha0 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue1.ui8Alpha3 = 0x77;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue1.ui8Alpha2 = 0x40;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue1.ui8Alpha1 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue1.ui8Alpha0 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue2.ui8Alpha3 = 0xbb;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue2.ui8Alpha2 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue2.ui8Alpha1 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue2.ui8Alpha0 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue3.ui8Alpha3 = 0xff;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue3.ui8Alpha2 = 0xc0;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue3.ui8Alpha1 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue3.ui8Alpha0 = 0x80;
    // Overlay channel 3.
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV3 = 0;
    buf = m_pOvImgBuf[1]->GetBuffer();
    m_IspostUser.stOvInfo.stOvImgInfo[3].ui32YAddr = buf.fr_buf.phys_addr;
    m_pOvImgBuf[1]->PutBuffer(&buf);
    m_IspostUser.stOvInfo.stOvImgInfo[3].ui16Stride = 1920 / 4;
    m_IspostUser.stOvInfo.stOvImgInfo[3].ui16Width = 192;
    m_IspostUser.stOvInfo.stOvImgInfo[3].ui16Height = 108;
    m_IspostUser.stOvInfo.stOvOffset[3].ui16X = 320;
    m_IspostUser.stOvInfo.stOvOffset[3].ui16Y = 140;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue0.ui8Alpha3 = 0x33;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue0.ui8Alpha2 = 0x00;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue0.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue0.ui8Alpha0 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue1.ui8Alpha3 = 0x77;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue1.ui8Alpha2 = 0x40;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue1.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue1.ui8Alpha0 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue2.ui8Alpha3 = 0xbb;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue2.ui8Alpha2 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue2.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue2.ui8Alpha0 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue3.ui8Alpha3 = 0xff;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue3.ui8Alpha2 = 0xc0;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue3.ui8Alpha1 = 0x80;
    // Overlay channel 4.
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV4 = 0;
    buf = m_pOvImgBuf[1]->GetBuffer();
    m_IspostUser.stOvInfo.stOvImgInfo[4].ui32YAddr = buf.fr_buf.phys_addr;
    m_pOvImgBuf[1]->PutBuffer(&buf);
    m_IspostUser.stOvInfo.stOvImgInfo[4].ui16Stride = 1920 / 4;
    m_IspostUser.stOvInfo.stOvImgInfo[4].ui16Width = 192;
    m_IspostUser.stOvInfo.stOvImgInfo[4].ui16Height = 108;
    m_IspostUser.stOvInfo.stOvOffset[4].ui16X = 64;
    m_IspostUser.stOvInfo.stOvOffset[4].ui16Y = 256;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue0.ui8Alpha3 = 0x33;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue0.ui8Alpha2 = 0x00;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue0.ui8Alpha1 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue0.ui8Alpha0 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue1.ui8Alpha3 = 0x77;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue1.ui8Alpha2 = 0x40;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue1.ui8Alpha1 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue1.ui8Alpha0 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue2.ui8Alpha3 = 0xbb;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue2.ui8Alpha2 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue2.ui8Alpha1 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue2.ui8Alpha0 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue3.ui8Alpha3 = 0xff;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue3.ui8Alpha2 = 0xc0;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue3.ui8Alpha1 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue3.ui8Alpha0 = 0x20;
    // Overlay channel 5.
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV5 = 0;
    buf = m_pOvImgBuf[1]->GetBuffer();
    m_IspostUser.stOvInfo.stOvImgInfo[5].ui32YAddr = buf.fr_buf.phys_addr;
    m_pOvImgBuf[1]->PutBuffer(&buf);
    m_IspostUser.stOvInfo.stOvImgInfo[5].ui16Stride = 1920 / 4;
    m_IspostUser.stOvInfo.stOvImgInfo[5].ui16Width = 192;
    m_IspostUser.stOvInfo.stOvImgInfo[5].ui16Height = 108;
    m_IspostUser.stOvInfo.stOvOffset[5].ui16X = 320;
    m_IspostUser.stOvInfo.stOvOffset[5].ui16Y = 256;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue0.ui8Alpha3 = 0x33;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue0.ui8Alpha2 = 0x00;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue0.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue0.ui8Alpha0 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue1.ui8Alpha3 = 0x77;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue1.ui8Alpha2 = 0x40;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue1.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue1.ui8Alpha0 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue2.ui8Alpha3 = 0xbb;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue2.ui8Alpha2 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue2.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue2.ui8Alpha0 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue3.ui8Alpha3 = 0xff;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue3.ui8Alpha2 = 0xc0;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue3.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue3.ui8Alpha0 = 0x20;
    // Overlay channel 6.
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV6 = 0;
    buf = m_pOvImgBuf[1]->GetBuffer();
    m_IspostUser.stOvInfo.stOvImgInfo[6].ui32YAddr = buf.fr_buf.phys_addr;
    m_pOvImgBuf[1]->PutBuffer(&buf);
    m_IspostUser.stOvInfo.stOvImgInfo[6].ui16Stride = 1920 / 4;
    m_IspostUser.stOvInfo.stOvImgInfo[6].ui16Width = 192;
    m_IspostUser.stOvInfo.stOvImgInfo[6].ui16Height = 108;
    m_IspostUser.stOvInfo.stOvOffset[6].ui16X = 64;
    m_IspostUser.stOvInfo.stOvOffset[6].ui16Y = 360;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue0.ui8Alpha3 = 0x33;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue0.ui8Alpha2 = 0x00;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue0.ui8Alpha1 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue0.ui8Alpha0 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue1.ui8Alpha3 = 0x77;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue1.ui8Alpha2 = 0x40;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue1.ui8Alpha1 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue1.ui8Alpha0 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue2.ui8Alpha3 = 0xbb;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue2.ui8Alpha2 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue2.ui8Alpha1 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue2.ui8Alpha0 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue3.ui8Alpha3 = 0xff;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue3.ui8Alpha2 = 0xc0;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue3.ui8Alpha1 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue3.ui8Alpha0 = 0x80;
    // Overlay channel 7.
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV7 = 0;
    buf = m_pOvImgBuf[1]->GetBuffer();
    m_IspostUser.stOvInfo.stOvImgInfo[7].ui32YAddr = buf.fr_buf.phys_addr;
    m_pOvImgBuf[1]->PutBuffer(&buf);
    m_IspostUser.stOvInfo.stOvImgInfo[7].ui16Stride = 1920 / 4;
    m_IspostUser.stOvInfo.stOvImgInfo[7].ui16Width = 192;
    m_IspostUser.stOvInfo.stOvImgInfo[7].ui16Height = 108;
    m_IspostUser.stOvInfo.stOvOffset[7].ui16X = 320;
    m_IspostUser.stOvInfo.stOvOffset[7].ui16Y = 360;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue0.ui8Alpha3 = 0xaf;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue0.ui8Alpha2 = 0x00;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue0.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue0.ui8Alpha0 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue1.ui8Alpha3 = 0xbf;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue1.ui8Alpha2 = 0x40;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue1.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue1.ui8Alpha0 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue2.ui8Alpha3 = 0xcf;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue2.ui8Alpha2 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue2.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue2.ui8Alpha0 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue3.ui8Alpha3 = 0xdf;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue3.ui8Alpha2 = 0xc0;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue3.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue3.ui8Alpha0 = 0x80;
#endif //IPU_ISPOST_OVERLAY_TEST


    m_stFce.maxModeGroupNb = 0;
    m_stFce.hasFce = false;
    m_stFce.stModeGroupAllList.clear();
    m_stFce.modeGroupActiveList.clear();
    m_stFce.modeGroupPendingList.clear();
    m_stFce.modeGroupIdleList.clear();

    m_HasFceData = false;
    m_HasFceFile = false;
}

void IPU_ISPLUS::Unprepare2()
{
    if (m_stFce.hasFce)
    {
        FreeFce();
        m_stFce.hasFce = false;
    }
    m_HasFceData = false;
    m_HasFceFile = false;
}

bool IPU_ISPLUS::IspostInitial(void)
{
    int i = 0;
    // IN ----------------------------------------
    if (m_pPortIn->IsEnabled())
    {
        if (m_IsLcManual && m_LcManualVal == 0) {
            m_IspostUser.stVsInfo.stSrcImgInfo.ui16Width = m_pPortIn->GetWidth();
            m_IspostUser.stVsInfo.stSrcImgInfo.ui16Height = m_pPortIn->GetHeight();
            m_IspostUser.stVsInfo.stSrcImgInfo.ui16Stride = m_pPortIn->GetWidth();
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_IspostUser.stVsInfo.stSrcImgInfo.ui16Width;
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = m_IspostUser.stVsInfo.stSrcImgInfo.ui16Height;
        } else {
            m_IspostUser.stLcInfo.stSrcImgInfo.ui16Width = m_pPortIn->GetWidth();
            m_IspostUser.stLcInfo.stSrcImgInfo.ui16Height = m_pPortIn->GetHeight();
            m_IspostUser.stLcInfo.stSrcImgInfo.ui16Stride = m_IspostUser.stLcInfo.stSrcImgInfo.ui16Width;
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_IspostUser.stLcInfo.stSrcImgInfo.ui16Width;
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = m_IspostUser.stLcInfo.stSrcImgInfo.ui16Height;
            if (m_Rotate) {
                m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_IspostUser.stLcInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stLcInfo.stOutImgInfo.ui16Height;
                m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = m_IspostUser.stLcInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stLcInfo.stOutImgInfo.ui16Height;
                m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_IspostUser.stLcInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stLcInfo.stOutImgInfo.ui16Height;
            }
        }
        if (m_IspostUser.stCtrl0.ui1EnIspL) {
            //--------------------------------------------------------
            m_IspostUser.stCtrl0.ui1EnLcLb = ISPOST_ENABLE;
            //--------------------------------------------------------
            m_IspostUser.stLcInfo.stSrcImgInfo.ui32YAddr = 0x90001000;
            m_IspostUser.stLcInfo.stSrcImgInfo.ui32UVAddr = 0xb0001000;
            m_IspostUser.stLcInfo.stSrcImgInfo.ui16Stride = m_IspostUser.stLcInfo.stSrcImgInfo.ui16Width;
            //--------------------------------------------------------
            m_IspostUser.stLcInfo.stGridBufInfo.ui8Size = GRID_SIZE_16_16;
            //--------------------------------------------------------
            m_IspostUser.stLcInfo.stGridBufInfo.ui8LineBufEnable = ISPOST_DISABLE;
            m_IspostUser.stLcInfo.stPxlCacheMode.ui1BurstLenSet = (BURST_LEN_8QW_BARREL);
            m_IspostUser.stLcInfo.stPxlCacheMode.ui2CacheDataShapeSel = (CCH_DAT_SHAPE_1);
            //--------------------------------------------------------

            m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_IspostUser.stLcInfo.stSrcImgInfo.ui16Width;
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = m_IspostUser.stLcInfo.stSrcImgInfo.ui16Height;

        }
    }
    // OV ----------------------------------------
    for (i = 0; i < OVERLAY_CHNL_MAX; i++)
    {
        if (m_pPortOv[i]->IsEnabled())
        {
            m_IspostUser.stOvInfo.stOverlayMode.ui2OVM = OVERLAY_MODE_YUV;
            m_IspostUser.stOvInfo.stOvOffset[i].ui16X = m_pPortOv[i]->GetPipX();
            m_IspostUser.stOvInfo.stOvOffset[i].ui16Y = m_pPortOv[i]->GetPipY();
            m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Width = m_pPortOv[i]->GetWidth();
            m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Height = m_pPortOv[i]->GetHeight();
            if (i > 0)
                m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Stride = m_pPortOv[i]->GetWidth()/4;
            else
                m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Stride = m_pPortOv[i]->GetWidth();

            LOGE("OV%d X=%d Y=%d W=%d H=%d S=%d\n",i ,
                        m_pPortOv[i]->GetPipX(),
                        m_pPortOv[i]->GetPipY(),
                        m_pPortOv[i]->GetWidth(),
                        m_pPortOv[i]->GetHeight(),
                        m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Stride);
        }
    }

    // DN ----------------------------------------
    if (m_pPortDn->IsEnabled())
    {
        //m_IspostUser.stCtrl0.ui1EnDN = ISPOST_ENABLE;
        if (m_pPortDn->IsEmbezzling())
        {
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_pPortDn->GetPipWidth();
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = m_pPortDn->GetPipHeight();
        }
        else
        {
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_pPortDn->GetWidth();
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = m_pPortDn->GetHeight();
        }

        if (m_pPortIn->GetPipWidth() != 0 && m_pPortIn->GetPipHeight() != 0)  //Is Crop
        {
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_pPortIn->GetPipWidth();
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = m_pPortIn->GetPipHeight();
        }

        m_IspostUser.stDnInfo.stOutImgInfo.ui16Stride = m_pPortDn->GetWidth();
        m_IspostUser.stDnInfo.stRefImgInfo.ui16Stride = m_pPortDn->GetWidth();
        if (m_Rotate) {
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_IspostUser.stLcInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stLcInfo.stOutImgInfo.ui16Height;
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = m_IspostUser.stLcInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stLcInfo.stOutImgInfo.ui16Height;
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_IspostUser.stLcInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stLcInfo.stOutImgInfo.ui16Height;
            m_IspostUser.stDnInfo.stRefImgInfo.ui16Stride = m_pPortDn->GetHeight();
            m_IspostUser.stDnInfo.stOutImgInfo.ui16Stride = m_pPortDn->GetHeight();
        }
    }
    else
    {
        //m_IspostUser.stCtrl0.ui1EnDN = ISPOST_DISABLE;
        m_IspostUser.stDnInfo.stOutImgInfo.ui32YAddr = 0x0;
        m_IspostUser.stDnInfo.stOutImgInfo.ui32UVAddr = 0x0;
        m_IspostUser.stDnInfo.stRefImgInfo.ui32YAddr = 0x0;
        m_IspostUser.stDnInfo.stRefImgInfo.ui32UVAddr = 0x0;
    }

    // UO ----------------------------------------
    if (m_pPortUo->IsEnabled())
    {
        m_IspostUser.stUoInfo.ui8ScanH = SCAN_HEIGHT_16;
        if (m_pPortUo->IsEmbezzling())
        {
            m_IspostUser.stUoInfo.stOutImgInfo.ui16Width = m_pPortUo->GetPipWidth();
            m_IspostUser.stUoInfo.stOutImgInfo.ui16Height = m_pPortUo->GetPipHeight();
            m_IspostUser.stUoInfo.stOutImgInfo.ui16Stride =  m_pPortUo->GetWidth();
            LOGD("SS0 PipW: %d, PipH: %d, Stride: %d", m_pPortUo->GetPipWidth(), m_pPortUo->GetPipHeight(), m_pPortUo->GetWidth());
        }
        else
        {
            m_IspostUser.stUoInfo.stOutImgInfo.ui16Width = m_pPortUo->GetWidth();
            m_IspostUser.stUoInfo.stOutImgInfo.ui16Height = m_pPortUo->GetHeight();
            m_IspostUser.stUoInfo.stOutImgInfo.ui16Stride = m_IspostUser.stUoInfo.stOutImgInfo.ui16Width;
        }
        if (m_Rotate) {
            m_IspostUser.stUoInfo.stOutImgInfo.ui16Width = m_IspostUser.stUoInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stUoInfo.stOutImgInfo.ui16Height;
            m_IspostUser.stUoInfo.stOutImgInfo.ui16Height = m_IspostUser.stUoInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stUoInfo.stOutImgInfo.ui16Height;
            m_IspostUser.stUoInfo.stOutImgInfo.ui16Width = m_IspostUser.stUoInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stUoInfo.stOutImgInfo.ui16Height;
            m_IspostUser.stUoInfo.stOutImgInfo.ui16Stride = m_IspostUser.stUoInfo.stOutImgInfo.ui16Width;
        }
    }
    else
    {
        m_IspostUser.stUoInfo.stOutImgInfo.ui32YAddr = 0xFFFFFFFF;
        m_IspostUser.stUoInfo.stOutImgInfo.ui32UVAddr = 0xFFFFFFFF;
    }

    // SS0 ----------------------------------------
    if (m_pPortSs0->IsEnabled())
    {
        if (m_pPortSs0->IsEmbezzling())
        {
            m_IspostUser.stSS0Info.stOutImgInfo.ui16Width = m_pPortSs0->GetPipWidth();
            m_IspostUser.stSS0Info.stOutImgInfo.ui16Height = m_pPortSs0->GetPipHeight();
            m_IspostUser.stSS0Info.stOutImgInfo.ui16Stride =  m_pPortSs0->GetWidth();
            LOGD("SS0 PipW: %d, PipH: %d, Stride: %d", m_pPortSs0->GetPipWidth(), m_pPortSs0->GetPipHeight(), m_pPortSs0->GetWidth());
        }
        else
        {
            m_IspostUser.stSS0Info.stOutImgInfo.ui16Width = m_pPortSs0->GetWidth();
            m_IspostUser.stSS0Info.stOutImgInfo.ui16Height = m_pPortSs0->GetHeight();
            m_IspostUser.stSS0Info.stOutImgInfo.ui16Stride = m_IspostUser.stSS0Info.stOutImgInfo.ui16Width;
        }
    }
    else
    {
        m_IspostUser.stSS0Info.stOutImgInfo.ui32YAddr = 0xFFFFFFFF;
        m_IspostUser.stSS0Info.stOutImgInfo.ui32UVAddr = 0xFFFFFFFF;
    }

    // SS1 ----------------------------------------
    if (m_pPortSs1->IsEnabled())
    {
        if (m_pPortSs1->IsEmbezzling())
        {
            m_IspostUser.stSS1Info.stOutImgInfo.ui16Width = m_pPortSs1->GetPipWidth();
            m_IspostUser.stSS1Info.stOutImgInfo.ui16Height = m_pPortSs1->GetPipHeight();
            m_IspostUser.stSS1Info.stOutImgInfo.ui16Stride =  m_pPortSs1->GetWidth();
        }
        else
        {
            m_IspostUser.stSS1Info.stOutImgInfo.ui16Width = m_pPortSs1->GetWidth();
            m_IspostUser.stSS1Info.stOutImgInfo.ui16Height = m_pPortSs1->GetHeight();
            m_IspostUser.stSS1Info.stOutImgInfo.ui16Stride = m_IspostUser.stSS1Info.stOutImgInfo.ui16Width;
        }
    }
    else
    {
        m_IspostUser.stSS1Info.stOutImgInfo.ui32YAddr = 0xFFFFFFFF;
        m_IspostUser.stSS1Info.stOutImgInfo.ui32UVAddr = 0xFFFFFFFF;
    }

    m_IspostUser.ui32IspostCount = 0;

    return true;
}

int IPU_ISPLUS::UpdateDnUserInfo(bool hasFce, cam_position_t *pstPos, ISPOST_UINT32 *pRefBufYAddr )
{
    int pipX = 0;
    int pipY = 0;


    if (NULL == pRefBufYAddr)
    {
        LOGE("(%s:L%d) param is NULL !!!\n", __func__, __LINE__);
        return -1;
    }
    if (hasFce && pstPos)
    {
        pipX = pstPos->x;
        pipY = pstPos->y;
        m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = pstPos->width;
        m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = pstPos->height;
        LOGD("(%s:%d) DN pipX=%d, pipY=%d %dx%d \n", __func__, __LINE__, pipX, pipY, pstPos->width, pstPos->height);
    }
    else if (m_pPortDn->IsEmbezzling())
    {
        pipX = m_pPortDn->GetPipX();
        pipY = m_pPortDn->GetPipY();
    }
    m_IspostUser.stCtrl0.ui1EnDN = ISPOST_ENABLE;
    m_IspostUser.stDnInfo.stOutImgInfo.ui32YAddr = m_BufDn.fr_buf.phys_addr;
    m_IspostUser.stDnInfo.stOutImgInfo.ui32UVAddr = m_IspostUser.stDnInfo.stOutImgInfo.ui32YAddr + (m_pPortDn->GetWidth() * m_pPortDn->GetHeight());

    m_IspostUser.stDnInfo.stRefImgInfo.ui32YAddr = *pRefBufYAddr;
    m_IspostUser.stDnInfo.stRefImgInfo.ui32UVAddr = m_IspostUser.stDnInfo.stRefImgInfo.ui32YAddr + (m_pPortDn->GetWidth() * m_pPortDn->GetHeight());
    if (hasFce)
         *pRefBufYAddr = m_IspostUser.stDnInfo.stOutImgInfo.ui32YAddr;
    if (m_pPortDn->IsEmbezzling() || hasFce)
    {
        IspostCalcOffsetAddress(pipX, pipY, m_IspostUser.stDnInfo.stOutImgInfo.ui16Stride,
                                m_IspostUser.stDnInfo.stOutImgInfo.ui32YAddr, m_IspostUser.stDnInfo.stOutImgInfo.ui32UVAddr);

        IspostCalcOffsetAddress(pipX, pipY, m_IspostUser.stDnInfo.stRefImgInfo.ui16Stride,
                               m_IspostUser.stDnInfo.stRefImgInfo.ui32YAddr, m_IspostUser.stDnInfo.stRefImgInfo.ui32UVAddr);
    }
    if (m_pPortIn->GetPipWidth() != 0 && m_pPortIn->GetPipHeight() != 0)
    {
        IspostCalcOffsetAddress(m_pPortIn->GetPipX(), m_pPortIn->GetPipY(), m_IspostUser.stLcInfo.stSrcImgInfo.ui16Stride,
                                m_IspostUser.stLcInfo.stSrcImgInfo.ui32YAddr, m_IspostUser.stLcInfo.stSrcImgInfo.ui32UVAddr);
    }
    if (m_pPortDn->IsEmbezzling())
         *pRefBufYAddr = m_IspostUser.stDnInfo.stOutImgInfo.ui32YAddr;

    return 0;
}

int IPU_ISPLUS::UpdateUoUserInfo(bool hasFce, cam_position_t *pstPos)
{
    int pipX = 0;
    int pipY = 0;


    if (hasFce && pstPos)
    {
        pipX = pstPos->x;
        pipY = pstPos->y;
        m_IspostUser.stUoInfo.stOutImgInfo.ui16Width = pstPos->width;
        m_IspostUser.stUoInfo.stOutImgInfo.ui16Height = pstPos->height;
        LOGD("(%s:%d) UO pipX=%d, pipY=%d %dx%d \n", __func__, __LINE__, pipX, pipY, pstPos->width, pstPos->height);
    }
    else if (m_pPortUo->IsEmbezzling())
    {
        pipX = m_pPortUo->GetPipX();
        pipY = m_pPortUo->GetPipY();
    }
    m_IspostUser.stCtrl0.ui1EnUO = ISPOST_ENABLE;
    m_IspostUser.stUoInfo.stOutImgInfo.ui32YAddr = m_BufUo.fr_buf.phys_addr;
    m_IspostUser.stUoInfo.stOutImgInfo.ui32UVAddr = m_IspostUser.stUoInfo.stOutImgInfo.ui32YAddr + (m_pPortUo->GetWidth() * m_pPortUo->GetHeight());
    if (m_pPortUo->IsEmbezzling() || hasFce)
    {
        IspostCalcOffsetAddress(pipX, pipY, m_IspostUser.stUoInfo.stOutImgInfo.ui16Stride,
                            m_IspostUser.stUoInfo.stOutImgInfo.ui32YAddr, m_IspostUser.stUoInfo.stOutImgInfo.ui32UVAddr);
    }

    return 0;
}

int IPU_ISPLUS::UpdateSs0UserInfo(bool hasFce, cam_position_t *pstPos)
{
    int pipX = 0;
    int pipY = 0;


    if (hasFce && pstPos)
    {
        pipX = pstPos->x;
        pipY = pstPos->y;
        m_IspostUser.stSS0Info.stOutImgInfo.ui16Width = pstPos->width;
        m_IspostUser.stSS0Info.stOutImgInfo.ui16Height = pstPos->height;
        LOGD("(%s:%d) SS0 pipX=%d, pipY=%d, %dx%d \n", __func__, __LINE__, pipX, pipY, pstPos->width, pstPos->height);
    }
    else if (m_pPortSs0->IsEmbezzling())
    {
        pipX = m_pPortSs0->GetPipX();
        pipY = m_pPortSs0->GetPipY();
    }
    m_IspostUser.stCtrl0.ui1EnSS0 = ISPOST_ENABLE;
    m_IspostUser.stSS0Info.stOutImgInfo.ui32YAddr = m_BufSs0.fr_buf.phys_addr;
    m_IspostUser.stSS0Info.stOutImgInfo.ui32UVAddr = m_IspostUser.stSS0Info.stOutImgInfo.ui32YAddr + (m_pPortSs0->GetWidth() * m_pPortSs0->GetHeight());
    if (m_pPortSs0->IsEmbezzling() || hasFce)
    {
            IspostCalcOffsetAddress(pipX, pipY, m_IspostUser.stSS0Info.stOutImgInfo.ui16Stride,
                                m_IspostUser.stSS0Info.stOutImgInfo.ui32YAddr, m_IspostUser.stSS0Info.stOutImgInfo.ui32UVAddr);
    }

    return 0;
}
int IPU_ISPLUS::UpdateSs1UserInfo(bool hasFce, cam_position_t *pstPos)
{
    int pipX = 0;
    int pipY = 0;


    if (hasFce && pstPos)
    {
        pipX = pstPos->x;
        pipY = pstPos->y;
        m_IspostUser.stSS1Info.stOutImgInfo.ui16Width = pstPos->width;
        m_IspostUser.stSS1Info.stOutImgInfo.ui16Height = pstPos->height;
        LOGD("(%s:%d) SS1 pipX=%d, pipY=%d, %dx%d \n", __func__, __LINE__, pipX, pipY, pstPos->width, pstPos->height);
    }
    else if (m_pPortSs1->IsEmbezzling())
    {
        pipX = m_pPortSs1->GetPipX();
        pipY = m_pPortSs1->GetPipY();
    }
    m_IspostUser.stCtrl0.ui1EnSS1 = ISPOST_ENABLE;
    m_IspostUser.stSS1Info.stOutImgInfo.ui32YAddr = m_BufSs1.fr_buf.phys_addr;
    m_IspostUser.stSS1Info.stOutImgInfo.ui32UVAddr = m_IspostUser.stSS1Info.stOutImgInfo.ui32YAddr + (m_pPortSs1->GetWidth() * m_pPortSs1->GetHeight());
    if (m_pPortSs1->IsEmbezzling() || hasFce)
    {
        IspostCalcOffsetAddress(pipX, pipY, m_IspostUser.stSS1Info.stOutImgInfo.ui16Stride,
                            m_IspostUser.stSS1Info.stOutImgInfo.ui32YAddr, m_IspostUser.stSS1Info.stOutImgInfo.ui32UVAddr);
    }

    return 0;
}

bool IPU_ISPLUS::IspostHwInit(void)
{
    int ret = 0;

    IspostUpdateBuf();
    ret = ispost_init(&m_IspostUser);

    return (ret >= 0) ? true : false;
}

bool IPU_ISPLUS::IspostHwUpdate(void)
{
    int ret = 0;

    IspostUpdateBuf();
    ret = ispost_update(&m_IspostUser);

    return (ret >= 0) ? true : false;
}

bool IPU_ISPLUS::IspostHwTrigger(void)
{
    int ret = 0;

    ret = ispost_trigger(&m_IspostUser);

    m_IspostUser.ui32IspostCount++;

    return (ret >= 0) ? true : false;
}

bool IPU_ISPLUS::IspostHwCompleteCheck(void)
{
    int ret = 0;

    ret = ispost_wait_complete(&m_IspostUser);

    return (ret >= 0) ? true : false;
}

bool IPU_ISPLUS::IspostUpdateBuf(void)
{
    int ret = 0;
    static ISPOST_UINT32 ui32DnRefImgBufYaddr = 0;
    int fileNumber = 1;
    bool hasOv = false;


    if (m_IspostUser.ui32IspostCount == 0)
        ui32DnRefImgBufYaddr =  0xFFFFFFFF;

    m_IspostUser.stCtrl0.ui1EnLC = ISPOST_ENABLE;

    m_IspostUser.stCtrl0.ui1EnOV = ISPOST_DISABLE;
    if (m_PortGetBuf & ISPOST_PT_OV0_7)
    {
        if (m_PortGetBuf & ISPOST_PT_OV0)
        {
            if (m_BufOv[0].fr_buf.phys_addr != 0)
            {
                hasOv = true;
                m_IspostUser.stCtrl0.ui1EnOV = ISPOST_ENABLE;
                m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV0 = ISPOST_ENABLE;

                ST_OVERLAY_INFO* ov_info = NULL;
                ov_info = (ST_OVERLAY_INFO*)(m_BufOv[0].fr_buf.priv);
                if (ov_info)
                {
                    if (ov_info->height > 0 && ov_info->width > 0)
                    {
                        if (ov_info->width != m_pPortOv[0]->GetWidth() ||ov_info->height != m_pPortOv[0]->GetHeight())
                            LOGE("\nOV0 [BUF] W=%d H=%d\n", ov_info->width,ov_info->height);
                        m_pPortOv[0]->SetResolution(ov_info->width,ov_info->height);
                    }

                    if (ov_info->disable == 1)
                        m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV0 = ISPOST_DISABLE;
                }
                m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Width = m_pPortOv[0]->GetWidth();
                m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Height =m_pPortOv[0]->GetHeight();
                m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Stride = m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Width;

                pthread_mutex_lock( &m_PipMutex);
                IspostAdjustOvPos(0);
                m_IspostUser.stOvInfo.stOvOffset[0].ui16X = m_pPortOv[0]->GetPipX();
                m_IspostUser.stOvInfo.stOvOffset[0].ui16Y = m_pPortOv[0]->GetPipY();
                pthread_mutex_unlock( &m_PipMutex);

                m_IspostUser.stOvInfo.stOvImgInfo[0].ui32YAddr = m_BufOv[0].fr_buf.phys_addr;
                IspostCalcPlanarAddress(m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Stride, m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Height,
                    m_IspostUser.stOvInfo.stOvImgInfo[0].ui32YAddr,  m_IspostUser.stOvInfo.stOvImgInfo[0].ui32UVAddr);
            }
            else
            {
                m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV0 = ISPOST_DISABLE;
            }
        }

        if (m_PortGetBuf & ISPOST_PT_OV1)
        {
            LOGD("OV1 fr_buf phys_addr = 0x%x\n", m_BufOv[1].fr_buf.phys_addr);
            if (m_BufOv[1].fr_buf.phys_addr != 0)
            {
                hasOv = true;
                m_IspostUser.stCtrl0.ui1EnOV = ISPOST_ENABLE;
                m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV1 = ISPOST_ENABLE;

                ST_OVERLAY_INFO* ov_info = NULL;
                ov_info = (ST_OVERLAY_INFO*)(m_BufOv[1].fr_buf.priv);
                if (ov_info)
                {
                    if (ov_info->height > 0 && ov_info->width > 0)
                    {
                        if (ov_info->width != m_pPortOv[1]->GetWidth() ||ov_info->height != m_pPortOv[1]->GetHeight())
                            LOGE("\nOV1 [BUF] W=%d H=%d\n", ov_info->width,ov_info->height);
                        m_pPortOv[1]->SetResolution(ov_info->width,ov_info->height);
                    }

                    if (ov_info->disable == 1)
                        m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV1 = ISPOST_DISABLE;
                }
                m_IspostUser.stOvInfo.stOvImgInfo[1].ui16Width = m_pPortOv[1]->GetWidth() ;
                m_IspostUser.stOvInfo.stOvImgInfo[1].ui16Height =m_pPortOv[1]->GetHeight();
                m_IspostUser.stOvInfo.stOvImgInfo[1].ui16Stride = m_IspostUser.stOvInfo.stOvImgInfo[1].ui16Width>>2;

                pthread_mutex_lock( &m_PipMutex);
                IspostAdjustOvPos(1);
                m_IspostUser.stOvInfo.stOvOffset[1].ui16X = m_pPortOv[1]->GetPipX();
                m_IspostUser.stOvInfo.stOvOffset[1].ui16Y = m_pPortOv[1]->GetPipY();
                pthread_mutex_unlock( &m_PipMutex);

                m_IspostUser.stOvInfo.stOvImgInfo[1].ui32YAddr = m_BufOv[1].fr_buf.phys_addr;
            }
            else
            {
                m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV1 = ISPOST_DISABLE;
            }
        }

        if (m_PortGetBuf & ISPOST_PT_OV2)
        {
            if (m_BufOv[2].fr_buf.phys_addr != 0)
            {
                hasOv = true;
                m_IspostUser.stCtrl0.ui1EnOV = ISPOST_ENABLE;
                m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV2 = ISPOST_ENABLE;

                ST_OVERLAY_INFO* ov_info = NULL;
                ov_info = (ST_OVERLAY_INFO*)(m_BufOv[2].fr_buf.priv);
                if (ov_info)
                {
                    if (ov_info->height > 0 && ov_info->width > 0)
                    {
                        if (ov_info->width != m_pPortOv[2]->GetWidth() ||ov_info->height != m_pPortOv[2]->GetHeight())
                            LOGE("\nOV2 [BUF] W=%d H=%d\n", ov_info->width,ov_info->height);
                        m_pPortOv[2]->SetResolution(ov_info->width,ov_info->height);
                    }

                    if (ov_info->disable == 1)
                        m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV2 = ISPOST_DISABLE;
                }
                m_IspostUser.stOvInfo.stOvImgInfo[2].ui16Width = m_pPortOv[2]->GetWidth() ;
                m_IspostUser.stOvInfo.stOvImgInfo[2].ui16Height =m_pPortOv[2]->GetHeight();
                m_IspostUser.stOvInfo.stOvImgInfo[2].ui16Stride = m_IspostUser.stOvInfo.stOvImgInfo[2].ui16Width>>2;

                pthread_mutex_lock( &m_PipMutex);
                IspostAdjustOvPos(2);
                m_IspostUser.stOvInfo.stOvOffset[2].ui16X = m_pPortOv[2]->GetPipX();
                m_IspostUser.stOvInfo.stOvOffset[2].ui16Y = m_pPortOv[2]->GetPipY();
                pthread_mutex_unlock( &m_PipMutex);

                m_IspostUser.stOvInfo.stOvImgInfo[2].ui32YAddr = m_BufOv[2].fr_buf.phys_addr;
            }
            else
            {
                m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV2 = ISPOST_DISABLE;
            }
        }

        if (m_PortGetBuf & ISPOST_PT_OV3)
        {
            if (m_BufOv[3].fr_buf.phys_addr != 0)
            {
                hasOv = true;
                m_IspostUser.stCtrl0.ui1EnOV = ISPOST_ENABLE;
                m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV3 = ISPOST_ENABLE;

                ST_OVERLAY_INFO* ov_info = NULL;
                ov_info = (ST_OVERLAY_INFO*)(m_BufOv[3].fr_buf.priv);
                if (ov_info)
                {
                    if (ov_info->height > 0 && ov_info->width > 0)
                    {
                        if (ov_info->width != m_pPortOv[3]->GetWidth() ||ov_info->height != m_pPortOv[3]->GetHeight())
                            LOGE("\nOV3 [BUF] W=%d H=%d\n", ov_info->width,ov_info->height);
                        m_pPortOv[3]->SetResolution(ov_info->width,ov_info->height);
                    }

                    if (ov_info->disable == 1)
                        m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV3 = ISPOST_DISABLE;
                }
                m_IspostUser.stOvInfo.stOvImgInfo[3].ui16Width = m_pPortOv[3]->GetWidth() ;
                m_IspostUser.stOvInfo.stOvImgInfo[3].ui16Height =m_pPortOv[3]->GetHeight();
                m_IspostUser.stOvInfo.stOvImgInfo[3].ui16Stride = m_IspostUser.stOvInfo.stOvImgInfo[3].ui16Width>>2;

                pthread_mutex_lock( &m_PipMutex);
                IspostAdjustOvPos(3);
                m_IspostUser.stOvInfo.stOvOffset[3].ui16X = m_pPortOv[3]->GetPipX();
                m_IspostUser.stOvInfo.stOvOffset[3].ui16Y = m_pPortOv[3]->GetPipY();
                pthread_mutex_unlock( &m_PipMutex);

                m_IspostUser.stOvInfo.stOvImgInfo[3].ui32YAddr = m_BufOv[3].fr_buf.phys_addr;
            }
            else
            {
                m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV3 = ISPOST_DISABLE;
            }
        }

        if (m_PortGetBuf & ISPOST_PT_OV4)
        {
            if (m_BufOv[4].fr_buf.phys_addr != 0)
            {
                hasOv = true;
                m_IspostUser.stCtrl0.ui1EnOV = ISPOST_ENABLE;
                m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV4 = ISPOST_ENABLE;

                ST_OVERLAY_INFO* ov_info = NULL;
                ov_info = (ST_OVERLAY_INFO*)(m_BufOv[4].fr_buf.priv);
                if (ov_info)
                {
                    if (ov_info->height > 0 && ov_info->width > 0)
                    {
                        if (ov_info->width != m_pPortOv[4]->GetWidth() ||ov_info->height != m_pPortOv[4]->GetHeight())
                            LOGE("\nOV4 [BUF] W=%d H=%d\n", ov_info->width, ov_info->height);
                        m_pPortOv[4]->SetResolution(ov_info->width,ov_info->height);
                    }

                    if (ov_info->disable == 1)
                        m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV4 = ISPOST_DISABLE;
                }
                m_IspostUser.stOvInfo.stOvImgInfo[4].ui16Width = m_pPortOv[4]->GetWidth() ;
                m_IspostUser.stOvInfo.stOvImgInfo[4].ui16Height =m_pPortOv[4]->GetHeight();
                m_IspostUser.stOvInfo.stOvImgInfo[4].ui16Stride = m_IspostUser.stOvInfo.stOvImgInfo[4].ui16Width>>2;

                pthread_mutex_lock( &m_PipMutex);
                IspostAdjustOvPos(4);
                m_IspostUser.stOvInfo.stOvOffset[4].ui16X = m_pPortOv[4]->GetPipX();
                m_IspostUser.stOvInfo.stOvOffset[4].ui16Y = m_pPortOv[4]->GetPipY();
                pthread_mutex_unlock( &m_PipMutex);

                m_IspostUser.stOvInfo.stOvImgInfo[4].ui32YAddr = m_BufOv[4].fr_buf.phys_addr;
            }
            else
            {
                m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV4 = ISPOST_DISABLE;
            }
        }

        if (m_PortGetBuf & ISPOST_PT_OV5)
        {
            if (m_BufOv[5].fr_buf.phys_addr != 0)
            {
                hasOv = true;
                m_IspostUser.stCtrl0.ui1EnOV = ISPOST_ENABLE;
                m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV5 = ISPOST_ENABLE;

                ST_OVERLAY_INFO* ov_info = NULL;
                ov_info = (ST_OVERLAY_INFO*)(m_BufOv[5].fr_buf.priv);
                if (ov_info)
                {
                    if (ov_info->height > 0 && ov_info->width > 0)
                    {
                        if (ov_info->width != m_pPortOv[5]->GetWidth() ||ov_info->height != m_pPortOv[5]->GetHeight())
                            LOGE("\nOV5 [BUF] W=%d H=%d\n", ov_info->width, ov_info->height);
                        m_pPortOv[5]->SetResolution(ov_info->width,ov_info->height);
                    }

                    if (ov_info->disable == 1)
                        m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV5 = ISPOST_DISABLE;
                }
                m_IspostUser.stOvInfo.stOvImgInfo[5].ui16Width = m_pPortOv[5]->GetWidth() ;
                m_IspostUser.stOvInfo.stOvImgInfo[5].ui16Height =m_pPortOv[5]->GetHeight();
                m_IspostUser.stOvInfo.stOvImgInfo[5].ui16Stride = m_IspostUser.stOvInfo.stOvImgInfo[5].ui16Width>>2;

                pthread_mutex_lock( &m_PipMutex);
                IspostAdjustOvPos(5);
                m_IspostUser.stOvInfo.stOvOffset[5].ui16X = m_pPortOv[5]->GetPipX();
                m_IspostUser.stOvInfo.stOvOffset[5].ui16Y = m_pPortOv[5]->GetPipY();
                pthread_mutex_unlock( &m_PipMutex);

                m_IspostUser.stOvInfo.stOvImgInfo[5].ui32YAddr = m_BufOv[5].fr_buf.phys_addr;
            }
            else
            {
                m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV5 = ISPOST_DISABLE;
            }
        }

        if (m_PortGetBuf & ISPOST_PT_OV6)
        {
            if (m_BufOv[6].fr_buf.phys_addr != 0)
            {
                hasOv = true;
                m_IspostUser.stCtrl0.ui1EnOV = ISPOST_ENABLE;
                m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV6 = ISPOST_ENABLE;

                ST_OVERLAY_INFO* ov_info = NULL;
                ov_info = (ST_OVERLAY_INFO*)(m_BufOv[6].fr_buf.priv);
                if (ov_info)
                {
                    if (ov_info->height > 0 && ov_info->width > 0)
                    {
                        if (ov_info->width != m_pPortOv[6]->GetWidth() ||ov_info->height != m_pPortOv[6]->GetHeight())
                            LOGE("\nOV6 [BUF] W=%d H=%d\n", ov_info->width,ov_info->height);
                        m_pPortOv[6]->SetResolution(ov_info->width,ov_info->height);
                    }

                    if (ov_info->disable == 1)
                        m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV6 = ISPOST_DISABLE;
                }
                m_IspostUser.stOvInfo.stOvImgInfo[6].ui16Width = m_pPortOv[6]->GetWidth() ;
                m_IspostUser.stOvInfo.stOvImgInfo[6].ui16Height =m_pPortOv[6]->GetHeight();
                m_IspostUser.stOvInfo.stOvImgInfo[6].ui16Stride = m_IspostUser.stOvInfo.stOvImgInfo[6].ui16Width>>2;

                pthread_mutex_lock( &m_PipMutex);
                IspostAdjustOvPos(6);
                m_IspostUser.stOvInfo.stOvOffset[6].ui16X = m_pPortOv[6]->GetPipX();
                m_IspostUser.stOvInfo.stOvOffset[6].ui16Y = m_pPortOv[6]->GetPipY();
                pthread_mutex_unlock( &m_PipMutex);

                m_IspostUser.stOvInfo.stOvImgInfo[6].ui32YAddr = m_BufOv[6].fr_buf.phys_addr;
            }
            else
            {
                m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV6 = ISPOST_DISABLE;
            }
        }

        if (m_PortGetBuf & ISPOST_PT_OV7)
        {
            if (m_BufOv[7].fr_buf.phys_addr != 0)
            {
                hasOv = true;
                m_IspostUser.stCtrl0.ui1EnOV = ISPOST_ENABLE;
                m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV7 = ISPOST_ENABLE;

                ST_OVERLAY_INFO* ov_info = NULL;
                ov_info = (ST_OVERLAY_INFO*)(m_BufOv[7].fr_buf.priv);
                if (ov_info)
                {
                    if (ov_info->height > 0 && ov_info->width > 0)
                    {
                        if (ov_info->width != m_pPortOv[7]->GetWidth() ||ov_info->height != m_pPortOv[7]->GetHeight())
                            LOGE("\nOV7 [BUF] W=%d H=%d\n", ov_info->width,ov_info->height);
                        m_pPortOv[7]->SetResolution(ov_info->width,ov_info->height);
                    }

                    if (ov_info->disable == 1)
                        m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV7 = ISPOST_DISABLE;
                }
                m_IspostUser.stOvInfo.stOvImgInfo[7].ui16Width = m_pPortOv[7]->GetWidth() ;
                m_IspostUser.stOvInfo.stOvImgInfo[7].ui16Height =m_pPortOv[7]->GetHeight();
                m_IspostUser.stOvInfo.stOvImgInfo[7].ui16Stride = m_IspostUser.stOvInfo.stOvImgInfo[7].ui16Width>>2;

                pthread_mutex_lock( &m_PipMutex);
                IspostAdjustOvPos(7);
                m_IspostUser.stOvInfo.stOvOffset[7].ui16X = m_pPortOv[7]->GetPipX();
                m_IspostUser.stOvInfo.stOvOffset[7].ui16Y = m_pPortOv[7]->GetPipY();
                pthread_mutex_unlock( &m_PipMutex);

                m_IspostUser.stOvInfo.stOvImgInfo[7].ui32YAddr = m_BufOv[7].fr_buf.phys_addr;
            }
            else
            {
                m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV7 = ISPOST_DISABLE;
            }
        }
    }
#ifdef IPU_ISPOST_OVERLAY_TEST
    else
    {
        m_BufOv[0] = m_pOvImgBuf[0]->GetBuffer();
        m_IspostUser.stOvInfo.stOvImgInfo[0].ui32YAddr = m_BufOv[0].fr_buf.phys_addr;
        IspostCalcPlanarAddress(m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Stride, m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Height,
                                m_IspostUser.stOvInfo.stOvImgInfo[0].ui32YAddr,  m_IspostUser.stOvInfo.stOvImgInfo[0].ui32UVAddr);
        m_pOvImgBuf[0]->PutBuffer(&m_BufOv[0]);
        m_BufOv[1] = m_pOvImgBuf[1]->GetBuffer();
        m_IspostUser.stOvInfo.stOvImgInfo[1].ui32YAddr = m_BufOv[1].fr_buf.phys_addr;
        m_IspostUser.stOvInfo.stOvImgInfo[2].ui32YAddr = m_BufOv[1].fr_buf.phys_addr;
        m_IspostUser.stOvInfo.stOvImgInfo[3].ui32YAddr = m_BufOv[1].fr_buf.phys_addr;
        m_IspostUser.stOvInfo.stOvImgInfo[4].ui32YAddr = m_BufOv[1].fr_buf.phys_addr;
        m_IspostUser.stOvInfo.stOvImgInfo[5].ui32YAddr = m_BufOv[1].fr_buf.phys_addr;
        m_IspostUser.stOvInfo.stOvImgInfo[6].ui32YAddr = m_BufOv[1].fr_buf.phys_addr;
        m_IspostUser.stOvInfo.stOvImgInfo[7].ui32YAddr = m_BufOv[1].fr_buf.phys_addr;
        m_pOvImgBuf[1]->PutBuffer(&m_BufOv[1]);
    }
#endif //IPU_ISPOST_OVERLAY_TEST

    ipu_v2500_private_data_t *priv = (ipu_v2500_private_data_t*)m_BufIn.fr_buf.priv;
    if(priv)
    {
        enum cam_mirror_state mirror = priv->mirror;
        int in_w,in_h;
        in_w = m_pPortIn->GetWidth();
        in_h = m_pPortIn->GetHeight();
        for (int i = 0; i < OVERLAY_CHNL_MAX; i++)
        {
            if (m_pPortOv[i]->IsEnabled())
            {
                if(mirror == CAM_MIRROR_H)
                {
                    m_IspostUser.stOvInfo.stOvOffset[i].ui16X = in_w - m_pPortOv[i]->GetPipX() - m_pPortOv[i]->GetWidth();
                    m_IspostUser.stOvInfo.stOvOffset[i].ui16Y = m_pPortOv[i]->GetPipY();
                }
                else if(mirror == CAM_MIRROR_V)
                {
                    m_IspostUser.stOvInfo.stOvOffset[i].ui16X = m_pPortOv[i]->GetPipX();
                    m_IspostUser.stOvInfo.stOvOffset[i].ui16Y = in_h - m_pPortOv[i]->GetPipY()- m_pPortOv[i]->GetHeight();
                }
                else if(mirror == CAM_MIRROR_HV)
                {
                    m_IspostUser.stOvInfo.stOvOffset[i].ui16X = in_w - m_pPortOv[i]->GetPipX() - m_pPortOv[i]->GetWidth();
                    m_IspostUser.stOvInfo.stOvOffset[i].ui16Y = in_h - m_pPortOv[i]->GetPipY()- m_pPortOv[i]->GetHeight();
                }
                else
                {

                }
            }
        }
    }

    bool hasFce = false;
    cam_position_t stDnPos;
    cam_position_t stUoPos;
    cam_position_t stSs0Pos;
    cam_position_t stSs1Pos;
    ST_FCE_MODE stActiveMode;
    ST_GRIDDATA_BUF_INFO stBufInfo;


    pthread_mutex_lock(&m_FceLock);
    memset(&stBufInfo, 0, sizeof(stBufInfo));
    if (m_stFce.hasFce)
    {
        ret = GetFceActiveMode(&stActiveMode);
        if (0 == ret)
        {
            hasFce = true;
            fileNumber = stActiveMode.stBufList.size();
        }
    }


    int i = 0;
    do
    {
        if (hasFce)
        {
            stBufInfo = stActiveMode.stBufList.at(i);

            if (!stActiveMode.stPosDnList.empty())
                stDnPos = stActiveMode.stPosDnList.at(i);
            if (!stActiveMode.stPosUoList.empty())
                stUoPos = stActiveMode.stPosUoList.at(i);
            if (!stActiveMode.stPosSs0List.empty())
                stSs0Pos = stActiveMode.stPosSs0List.at(i);
            if (!stActiveMode.stPosSs1List.empty())
                stSs1Pos = stActiveMode.stPosSs1List.at(i);
        }

        if (stBufInfo.stBufGrid.phys_addr)
        {
            m_IspostUser.stLcInfo.stGridBufInfo.ui32BufAddr = stBufInfo.stBufGrid.phys_addr;
            m_IspostUser.stLcInfo.stGridBufInfo.ui32BufLen = stBufInfo.stBufGrid.size;
            if (stBufInfo.stBufGeo.phys_addr)
            {
                m_IspostUser.stLcInfo.stGridBufInfo.ui32GeoBufAddr = stBufInfo.stBufGeo.phys_addr;
                m_IspostUser.stLcInfo.stGridBufInfo.ui32GeoBufLen = stBufInfo.stBufGeo.size;
            }
        }
        if (hasOv)
        {
            if (i == (fileNumber - 1))
                m_IspostUser.stCtrl0.ui1EnOV = ISPOST_ENABLE;
            else
                m_IspostUser.stCtrl0.ui1EnOV = ISPOST_DISABLE;
        }
        if (m_PortGetBuf & ISPOST_PT_DN)
        {
            UpdateDnUserInfo(hasFce, &stDnPos, &ui32DnRefImgBufYaddr);
        }
        else
        {
            m_IspostUser.stCtrl0.ui1EnDN = ISPOST_DISABLE;
        }
        if (m_PortGetBuf & ISPOST_PT_UO)
        {
            UpdateUoUserInfo(hasFce, &stUoPos);
        }
        else
        {
            m_IspostUser.stCtrl0.ui1EnUO = ISPOST_DISABLE;
        }

        if (m_PortGetBuf & ISPOST_PT_SS0)
        {
            UpdateSs0UserInfo(hasFce, &stSs0Pos);
        }
        else
        {
            m_IspostUser.stCtrl0.ui1EnSS0 = ISPOST_DISABLE;
        }
        if (m_PortGetBuf & ISPOST_PT_SS1)
        {
            UpdateSs1UserInfo(hasFce, &stSs1Pos);
        }
        else
        {
            m_IspostUser.stCtrl0.ui1EnSS1 = ISPOST_DISABLE;
        }

        //IspostDumpUserInfo();

    }while(++i < fileNumber);

//    m_IspostUser.ui32IspostCount++;
    LOGD("Trigger ctrl0 LC:%d, OV:%d, DN:%d, SS0:%d, SS1:%d, UO:%d  \n",
                m_IspostUser.stCtrl0.ui1EnLC,
                m_IspostUser.stCtrl0.ui1EnOV,
                m_IspostUser.stCtrl0.ui1EnDN,
                m_IspostUser.stCtrl0.ui1EnSS0,
                m_IspostUser.stCtrl0.ui1EnSS1,
                m_IspostUser.stCtrl0.ui1EnUO
            );
#ifdef IPU_ISPOST_LC_DEBUG
    if (m_IsFisheye) {
        if (0 == (m_IspostUser.ui32IspostCount % 5)) {
            m_IspostUser.stLcInfo.stGridBufInfo.ui32GridTargetIdx++;
            if (m_IspostUser.stLcInfo.stGridBufInfo.ui32GridTargetIdx >= (ISPOST_UINT32)grid_target_index_max)
                m_IspostUser.stLcInfo.stGridBufInfo.ui32GridTargetIdx = 0;
            m_IspostUser.stLcInfo.stGridBufInfo.ui32GridTargetIdx++;
        }
    }
#endif

#if 0  //for test trigger after ispost image
    if (m_PortEnable & ISPOST_PT_SS1 && m_IspostUser.ui32IspostCount == 20)
        WriteFile(m_BufSs1.fr_buf.virt_addr, m_BufSs1.fr_buf.size, "/nfs/ispost_ss1_%d.yuv", m_IspostUser.ui32IspostCount);
    if (m_PortEnable & ISPOST_PT_SS0 && m_IspostUser.ui32IspostCount == 30)
        WriteFile(m_BufSs0.fr_buf.virt_addr, m_BufSs0.fr_buf.size, "/nfs/ispost_ss0_%d.yuv", m_IspostUser.ui32IspostCount);
    if (m_PortEnable & ISPOST_PT_UO && m_IspostUser.ui32IspostCount == 40)
        WriteFile(m_BufUo.fr_buf.virt_addr, m_BufUo.fr_buf.size, "/nfs/ispost_uo_%d.yuv", m_IspostUser.ui32IspostCount);
    if (m_PortEnable & ISPOST_PT_DN && m_IspostUser.ui32IspostCount == 50)
        WriteFile(m_BufDn.fr_buf.virt_addr, m_BufDn.fr_buf.size, "/nfs/ispost_dn_%d.yuv", m_IspostUser.ui32IspostCount);
    //if (m_PortEnable & ISPOST_PT_OV0 && m_IspostUser.ui32IspostCount == 60)
    //  WriteFile(m_BufOv[0].fr_buf.virt_addr, m_BufOv[0].fr_buf.size, "/nfs/ispost_ol_%d.yuv", m_IspostUser.ui32IspostCount);
    //if (m_PortEnable & ISPOST_PT_IN && m_IspostUser.ui32IspostCount % 100 == 0)
    //  WriteFile(m_BufIn.fr_buf.virt_addr, m_BufIn.fr_buf.size, "/nfs/ispost_in_%d.yuv", m_IspostUser.ui32IspostCount);
#endif //0


    if (hasFce)
        UpdateFcePendingStatus();
    pthread_mutex_unlock(&m_FceLock);

    return (ret >= 0) ? true : false;
}


void IPU_ISPLUS::IspostDefaultParameter(void)
{
    ISPOST_UINT8 ui8Idx;

    memset(&m_IspostUser, 0, sizeof(ISPOST_USER));

    m_IspostUser.ui32UpdateFlag = 0;
    m_IspostUser.ui32PrintFlag = 0; //ISPOST_PRN_ALL;
    m_IspostUser.ui32PrintStep = 1000;
    m_IspostUser.ui32IspostCount = 0;

    //Ispost control
    m_IspostUser.stCtrl0.ui1EnLC = 0;
    m_IspostUser.stCtrl0.ui1EnVSR = 0;
    m_IspostUser.stCtrl0.ui1EnOV = 0;
    m_IspostUser.stCtrl0.ui1EnDN = 0;
    m_IspostUser.stCtrl0.ui1EnSS0 = 0;
    m_IspostUser.stCtrl0.ui1EnSS1 = 0;
    m_IspostUser.stCtrl0.ui1EnUO = 0;

    //-----------------------------------------------------------------
    //Lens correction information
    m_IspostUser.stLcInfo.stSrcImgInfo.ui32YAddr = 0;
    m_IspostUser.stLcInfo.stSrcImgInfo.ui32UVAddr = 0;
    m_IspostUser.stLcInfo.stSrcImgInfo.ui16Stride = 0;
    m_IspostUser.stLcInfo.stSrcImgInfo.ui16Width = 0;
    m_IspostUser.stLcInfo.stSrcImgInfo.ui16Height = 0;

    m_IspostUser.stLcInfo.stBackFillColor.ui8Y = LC_BACK_FILL_Y;
    m_IspostUser.stLcInfo.stBackFillColor.ui8CB = LC_BACK_FILL_CB;
    m_IspostUser.stLcInfo.stBackFillColor.ui8CR = LC_BACK_FILL_CR;

    m_IspostUser.stLcInfo.stGridBufInfo.ui32BufAddr = 0;
    m_IspostUser.stLcInfo.stGridBufInfo.ui32BufLen = 0; //No use
    m_IspostUser.stLcInfo.stGridBufInfo.ui16Stride = 0; //driver setting
    m_IspostUser.stLcInfo.stGridBufInfo.ui8Size = 0;
    m_IspostUser.stLcInfo.stGridBufInfo.ui8LineBufEnable = 0;
    m_IspostUser.stLcInfo.stGridBufInfo.ui32GridTargetIdx = 0;
    m_IspostUser.stLcInfo.stGridBufInfo.ui32GeoBufAddr = 0;
    m_IspostUser.stLcInfo.stGridBufInfo.ui32GeoBufLen = 0; //No use

    m_IspostUser.stLcInfo.stPxlCacheMode.ui1CBCRSwap = CBCR_SWAP_MODE_SWAP;
    m_IspostUser.stLcInfo.stPxlCacheMode.ui2PreFetchCtrl = LC_CACHE_MODE_CTRL_CFG;
    m_IspostUser.stLcInfo.stPxlCacheMode.ui1BurstLenSet = LC_CACHE_MODE_BURST_LEN;
    m_IspostUser.stLcInfo.stPxlCacheMode.ui2CacheDataShapeSel = LC_CACHE_MODE_DAT_SHAPE;
    m_IspostUser.stLcInfo.stPxlCacheMode.ui2LineBufFormat = LC_CACHE_MODE_LBFMT;
    m_IspostUser.stLcInfo.stPxlCacheMode.ui1LineBufFifoType = LC_CACHE_MODE_FIFO_TYPE;
    m_IspostUser.stLcInfo.stPxlCacheMode.ui1WaitLineBufEnd = LC_CACHE_MODE_WLBE;

    m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanW = SCAN_WIDTH_32;
    m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanH = SCAN_HEIGHT_32;
    m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanM = SCAN_MODE_LR_TB;

    m_IspostUser.stLcInfo.stOutImgInfo.ui32YAddr = 0; //No use
    m_IspostUser.stLcInfo.stOutImgInfo.ui32UVAddr = 0; //No use
    m_IspostUser.stLcInfo.stOutImgInfo.ui16Stride = 0; //No use
    m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = 0;
    m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = 0;

    m_IspostUser.stLcInfo.stAxiId.ui8SrcReqLmt = 4;
    m_IspostUser.stLcInfo.stAxiId.ui8GBID = 1;
    m_IspostUser.stLcInfo.stAxiId.ui8SRCID = 0;

    //-----------------------------------------------------------------
    //Video stabilization information
    m_IspostUser.stVsInfo.stSrcImgInfo.ui32YAddr = 0;
    m_IspostUser.stVsInfo.stSrcImgInfo.ui32UVAddr = 0;
    m_IspostUser.stVsInfo.stSrcImgInfo.ui16Stride = 0;
    m_IspostUser.stVsInfo.stSrcImgInfo.ui16Width = 0;
    m_IspostUser.stVsInfo.stSrcImgInfo.ui16Height = 0;
    m_IspostUser.stVsInfo.stRegOffset.ui16Roll = 0;
    m_IspostUser.stVsInfo.stRegOffset.ui16HOff = 0;
    m_IspostUser.stVsInfo.stRegOffset.ui8VOff = 0;
    m_IspostUser.stVsInfo.stAxiId.ui8FWID = 0;
    m_IspostUser.stVsInfo.stAxiId.ui8RDID = 2;
    m_IspostUser.stVsInfo.stCtrl0.ui32Ctrl = 0;

    //-----------------------------------------------------------------
    //Overlay image information
    m_IspostUser.stOvInfo.stOverlayMode.ui2OVM = OVERLAY_MODE_YUV;
    for (ui8Idx = 0; ui8Idx < OVERLAY_CHNL_MAX; ui8Idx++)
    {
        m_IspostUser.stOvInfo.ui8OvIdx[ui8Idx] = 0;

        m_IspostUser.stOvInfo.stOvImgInfo[ui8Idx].ui32YAddr = 0;
        m_IspostUser.stOvInfo.stOvImgInfo[ui8Idx].ui32UVAddr = 0;
        m_IspostUser.stOvInfo.stOvImgInfo[ui8Idx].ui16Stride = 0;
        m_IspostUser.stOvInfo.stOvImgInfo[ui8Idx].ui16Width = 0;
        m_IspostUser.stOvInfo.stOvImgInfo[ui8Idx].ui16Height = 0;

        m_IspostUser.stOvInfo.stOvOffset[ui8Idx].ui16X = 0;
        m_IspostUser.stOvInfo.stOvOffset[ui8Idx].ui16Y = 0;

        m_IspostUser.stOvInfo.stOvAlpha[ui8Idx].stValue0.ui32Alpha = 0x33221100; //0xFFFFFFFF
        m_IspostUser.stOvInfo.stOvAlpha[ui8Idx].stValue1.ui32Alpha = 0x77665544; //0xFFFFFFFF
        m_IspostUser.stOvInfo.stOvAlpha[ui8Idx].stValue2.ui32Alpha = 0xBBAA9988; //0xFFFFFFFF
        m_IspostUser.stOvInfo.stOvAlpha[ui8Idx].stValue3.ui32Alpha = 0xFFEEDDCC; //0xFFFFFFFF

        m_IspostUser.stOvInfo.stAxiId[ui8Idx].ui8OVID = 8;
    }

    //-----------------------------------------------------------------
    //3D-denoise information
    m_IspostUser.stDnInfo.stOutImgInfo.ui32YAddr = 0;
    m_IspostUser.stDnInfo.stOutImgInfo.ui32UVAddr = 0;
    m_IspostUser.stDnInfo.stOutImgInfo.ui16Stride = 0;
    m_IspostUser.stDnInfo.stRefImgInfo.ui32YAddr = 0;
    m_IspostUser.stDnInfo.stRefImgInfo.ui32UVAddr = 0;
    m_IspostUser.stDnInfo.stRefImgInfo.ui16Stride = 0;
    m_IspostUser.stDnInfo.ui32DnTargetIdx = 0;
    IspostSet3dDnsCurve(&m_Dns3dAttr, 1);
    m_IspostUser.stDnInfo.stAxiId.ui8RefWrID = 3;
    m_IspostUser.stDnInfo.stAxiId.ui8RefRdID = 3;

    //-----------------------------------------------------------------
    //Un-Scaled output information
    m_IspostUser.stUoInfo.stOutImgInfo.ui32YAddr = 0;
    m_IspostUser.stUoInfo.stOutImgInfo.ui32UVAddr = 0;
    m_IspostUser.stUoInfo.stOutImgInfo.ui16Stride = 0;
    m_IspostUser.stUoInfo.ui8ScanH = SCAN_HEIGHT_16;
    m_IspostUser.stUoInfo.stAxiId.ui8RefWrID = 4;

    //-----------------------------------------------------------------
    //Scaling stream 0 information
    m_IspostUser.stSS0Info.ui8StreamSel = SCALING_STREAM_0;
    m_IspostUser.stSS0Info.stOutImgInfo.ui32YAddr = 0;
    m_IspostUser.stSS0Info.stOutImgInfo.ui32UVAddr = 0;
    m_IspostUser.stSS0Info.stOutImgInfo.ui16Stride = 0;
    m_IspostUser.stSS0Info.stOutImgInfo.ui16Width = 0;
    m_IspostUser.stSS0Info.stOutImgInfo.ui16Height = 0;
    m_IspostUser.stSS0Info.stHSF.ui16SF = 0;
    m_IspostUser.stSS0Info.stHSF.ui8SM = SCALING_MODE_SCALING_DOWN;
    m_IspostUser.stSS0Info.stVSF.ui16SF = 0;
    m_IspostUser.stSS0Info.stVSF.ui8SM = SCALING_MODE_SCALING_DOWN;
    m_IspostUser.stSS0Info.stAxiId.ui8SSWID = 1;

    //-----------------------------------------------------------------
    //Scaling stream 1 information
    m_IspostUser.stSS1Info.ui8StreamSel = SCALING_STREAM_1;
    m_IspostUser.stSS1Info.stOutImgInfo.ui32YAddr = 0;
    m_IspostUser.stSS1Info.stOutImgInfo.ui32UVAddr = 0;
    m_IspostUser.stSS1Info.stOutImgInfo.ui16Stride = 0;
    m_IspostUser.stSS1Info.stOutImgInfo.ui16Width = 0;
    m_IspostUser.stSS1Info.stOutImgInfo.ui16Height = 0;
    m_IspostUser.stSS1Info.stHSF.ui16SF = 0;
    m_IspostUser.stSS1Info.stHSF.ui8SM = SCALING_MODE_SCALING_DOWN;
    m_IspostUser.stSS1Info.stVSF.ui16SF = 0;
    m_IspostUser.stSS1Info.stVSF.ui8SM = SCALING_MODE_SCALING_DOWN;
    m_IspostUser.stSS1Info.stAxiId.ui8SSWID = 2;


    //-----------------------------------------------------------------
    // ISP-Link control register.
    //-----------------------------------------------------------------
    // ISP-Link control register 0.
    // UV address mask.
    m_IspostUser.ui32FirstLcSrcImgBufAddr = 0x90001000;
    m_IspostUser.ui32FirstLcSrcImgBufUvAddrMSW = 0x00000000;
    m_IspostUser.ui32FirstLcSrcImgBufUvAddr = 0xb0001000;
    m_IspostUser.ui32FirstLcSrcImgBufUvAddrMask = 0x002a0a88;
    m_IspostUser.ui32NextLcSrcImgBufAddr = 0x90001000;
    m_IspostUser.ui32NextLcSrcImgBufUvAddrMSW = 0x00000000;
    m_IspostUser.ui32NextLcSrcImgBufUvAddr = 0xb0001000;
    m_IspostUser.ui32NextLcSrcImgBufUvAddrMask = 0x002a0a88;
    //-----------------------------------------------------------------
    // Isp-Link control register 1.
    m_IspostUser.stIspLinkInfo.stCtrl1.ui8ReOrdSeqSel0 = 4;
    m_IspostUser.stIspLinkInfo.stCtrl1.ui8ReOrdSeqSel1 = 1;
    m_IspostUser.stIspLinkInfo.stCtrl1.ui8ReOrdSeqLast = 4;
    m_IspostUser.stIspLinkInfo.stCtrl1.ui1ReOrdMode = REODER_MODE_ADDR_FILTER;
    m_IspostUser.stIspLinkInfo.stCtrl1.ui1ReOrdEn = ISPOST_ENABLE;
    m_IspostUser.stIspLinkInfo.stCtrl1.ui1InsertBurstsEn = ISPOST_ENABLE;
    m_IspostUser.stIspLinkInfo.stCtrl1.ui1PadAllBurstsTo4W = ISPOST_ENABLE;
    m_IspostUser.stIspLinkInfo.stCtrl1.ui1InvertIspAddr = ISPOST_ENABLE;
    //-----------------------------------------------------------------
    // Isp-Link control register 2.
    m_IspostUser.stIspLinkInfo.stCtrl2.ui12BurstInsertW = 0x5a;
    m_IspostUser.stIspLinkInfo.stCtrl2.ui8BurstInsertLen = 3;
}

void IPU_ISPLUS::IspostCheckParameter()
{
    if (m_pPortIn->GetPipX() > m_pPortIn->GetWidth() ||
        m_pPortIn->GetPipY() > m_pPortIn->GetHeight() ||
        m_pPortIn->GetPipWidth() > m_pPortIn->GetWidth() ||
        m_pPortIn->GetPipHeight() > m_pPortIn->GetHeight() ||
        m_pPortIn->GetPipX() + m_pPortIn->GetPipWidth() > m_pPortIn->GetWidth() ||
        m_pPortIn->GetPipY() + m_pPortIn->GetPipHeight() > m_pPortIn->GetHeight())
    {
        LOGE("ISPost Crop params error\n");
        throw VBERR_BADPARAM;
    }

    if ((m_pPortIn->GetPixelFormat() != Pixel::Format::NV12)
        && (m_pPortIn->GetPixelFormat() != Pixel::Format::NV21))
    {
        LOGE("ISPost PortIn format params error\n");
        throw VBERR_BADPARAM;
    }

    if (m_pPortOv[0]->IsEnabled())
    {
        if ((m_pPortOv[0]->GetPixelFormat() != Pixel::Format::NV12)
            && (m_pPortOv[0]->GetPixelFormat() != Pixel::Format::NV21))
        {
            LOGE("ISPost PortOv0 format params error\n");
            throw VBERR_BADPARAM;
        }
    }
    for(int i = 1; i < OVERLAY_CHNL_MAX; i++)
    {
        if (m_pPortOv[i]->IsEnabled())
        {
            if (m_pPortOv[i]->GetPixelFormat() != Pixel::Format::BPP2)
            {
                LOGE("Ov%d Port Pixel Format Params Error Not Support\n", i);
                throw VBERR_BADPARAM;
            }
        }
    }

    if (m_pPortDn->IsEnabled())
    {
        if ((m_pPortDn->GetPixelFormat() != Pixel::Format::NV12)
            && (m_pPortDn->GetPixelFormat() != Pixel::Format::NV21))
        {
            LOGE("ISPost PortDn format params error\n");
            throw VBERR_BADPARAM;
        }
        if (m_pPortDn->IsEmbezzling())
        {
            if (m_pPortDn->GetPipX() < 0 ||
                m_pPortDn->GetPipY() < 0 ||
                (m_pPortDn->GetPipX() + m_pPortDn->GetPipWidth()) > m_pPortDn->GetWidth() ||
                (m_pPortDn->GetPipY() + m_pPortDn->GetPipHeight()) > m_pPortDn->GetHeight())
            {
                LOGE("ISPost PortDn Embezzling params error\n");
                LOGE("PortDn(W:%d, H:%d), PortDn(X:%d, Y:%d, W:%d, H%d)\n",
                    m_pPortDn->GetWidth(), m_pPortDn->GetHeight(),
                    m_pPortDn->GetPipX(), m_pPortDn->GetPipY(),
                    m_pPortDn->GetPipWidth(), m_pPortDn->GetPipHeight());
                throw VBERR_BADPARAM;
            }
        }
        else
        {
            m_pPortDn->SetPipInfo(0, 0, m_pPortDn->GetWidth(), m_pPortDn->GetHeight());
        }
        if (m_pPortDn->GetPipWidth() < 16 || m_pPortDn->GetPipWidth() > 4096 ||
            m_pPortDn->GetPipHeight() < 16 || m_pPortDn->GetPipHeight() > 4096)
        {
            LOGE("ISPost not embezzling PortDn 16 > pip > 4096\n");
            throw VBERR_BADPARAM;
        }
    }

    if (m_pPortSs0->IsEnabled())
    {
        if ((m_pPortSs0->GetPixelFormat() != Pixel::Format::NV12)
            && (m_pPortSs0->GetPixelFormat() != Pixel::Format::NV21))
        {
            LOGE("ISPost PortSs0 format params error\n");
            throw VBERR_BADPARAM;
        }
        if (m_pPortSs0->IsEmbezzling())
        {
            if (m_pPortSs0->GetPipX() < 0 ||
                m_pPortSs0->GetPipY() < 0 ||
                (m_pPortSs0->GetPipX() + m_pPortSs0->GetPipWidth()) > m_pPortSs0->GetWidth() ||
                (m_pPortSs0->GetPipY() + m_pPortSs0->GetPipHeight()) > m_pPortSs0->GetHeight())
            {
                LOGE("ISPost PortSs0 Embezzling params error\n");
                LOGE("PortSs0(W:%d, H:%d), Pip(X:%d, Y:%d, W:%d, H%d)\n",
                     m_pPortSs0->GetWidth(), m_pPortSs0->GetHeight(),
                    m_pPortSs0->GetPipX(), m_pPortSs0->GetPipY(),
                    m_pPortSs0->GetPipWidth(), m_pPortSs0->GetPipHeight());
                throw VBERR_BADPARAM;
            }
        }
        else
        {
            m_pPortSs0->SetPipInfo(0, 0, m_pPortSs0->GetWidth(), m_pPortSs0->GetHeight());
        }
        if (m_pPortSs0->GetPipWidth() < 16 || m_pPortSs0->GetPipWidth() > 4096 ||
            m_pPortSs0->GetPipHeight() < 16 || m_pPortSs0->GetPipHeight() > 4096)
        {
            LOGE("ISPost not embezzling PortSS0 16 > pip > 4096\n");
            throw VBERR_BADPARAM;
        }
    }

    if (m_pPortSs1->IsEnabled())
    {
        if ((m_pPortSs1->GetPixelFormat() != Pixel::Format::NV12)
            && (m_pPortSs1->GetPixelFormat() != Pixel::Format::NV21))
        {
            LOGE("ISPost PortSs1 format params error\n");
            throw VBERR_BADPARAM;
        }
        if (m_pPortSs1->IsEmbezzling())
        {
            if (m_pPortSs1->GetPipX() < 0 ||
                m_pPortSs1->GetPipY() < 0 ||
                (m_pPortSs1->GetPipX() + m_pPortSs1->GetPipWidth()) > m_pPortSs1->GetWidth() ||
                (m_pPortSs1->GetPipY() + m_pPortSs1->GetPipHeight()) > m_pPortSs1->GetHeight())
            {
                LOGE("ISPost PortSs1 Embezzling params error\n");
                LOGE("PortSs1(W:%d, H:%d), Pip(X:%d, Y:%d, W:%d, H%d)\n",
                     m_pPortSs1->GetWidth(), m_pPortSs1->GetHeight(),
                    m_pPortSs1->GetPipX(), m_pPortSs1->GetPipY(),
                    m_pPortSs1->GetPipWidth(), m_pPortSs1->GetPipHeight());
                throw VBERR_BADPARAM;
            }
        }
        else
        {
            m_pPortSs1->SetPipInfo(0, 0, m_pPortSs1->GetWidth(), m_pPortSs1->GetHeight());
        }
        if (m_pPortSs1->GetPipWidth() < 16 || m_pPortSs1->GetPipWidth() > 4096 ||
            m_pPortSs1->GetPipHeight() < 16 || m_pPortSs1->GetPipHeight() > 4096)
        {
            LOGE("ISPost not embezzling PortSs1 16 > pip > 4096\n");
            throw VBERR_BADPARAM;
        }
    }

}

bool IPU_ISPLUS::IspostJsonGetInt(const char* pszItem, int& Value)
{
    Value = 0;

    if (m_pJsonMain == NULL)
        return false;

    Json* pSub = m_pJsonMain->GetObject(pszItem);
    if (pSub == NULL)
        return false;

    Value = pSub->valueint;
    LOGD("GetInt: %s = %d\n", pszItem, Value);
    return true;
}

bool IPU_ISPLUS::IspostJsonGetString(const char* pszItem, char*& pszString)
{
    pszString = NULL;

    if (m_pJsonMain == NULL)
        return false;

    Json* pSub = m_pJsonMain->GetObject(pszItem);
    if (pSub == NULL)
        return false;

    pszString = pSub->valuestring;
    LOGD("GetStr: %s = %s\n", pszItem, pszString);
    return true;
}


ISPOST_UINT32 IPU_ISPLUS::IspostGetGridTargetIndexMax(int width, int height, ISPOST_UINT8 sz, ISPOST_UINT32 nload)
{
    ISPOST_UINT32 ui32Stride;
    ISPOST_UINT32 ui32GridSize;
    ISPOST_UINT32 ui32Width;
    ISPOST_UINT32 ui32Height;
    ISPOST_UINT32 multi;

    if (GRID_SIZE_MAX <= sz)
    {
        sz = GRID_SIZE_8_8;
    }

    ui32GridSize = g_dwGridSize[sz];
    ui32Width = (ISPOST_UINT32)width / ui32GridSize;
    if ((ISPOST_UINT32)width > (ui32Width * ui32GridSize))
    {
        ui32Width += 2;
    }
    else
    {
        ui32Width++;
    }
    ui32Stride = ui32Width * 16;

    ui32Height = (ISPOST_UINT32)height / ui32GridSize;
    if ((ISPOST_UINT32)height > (ui32Height * ui32GridSize))
    {
        ui32Height += 2;
    }
    else
    {
        ui32Height++;
    }

    multi = ui32Height * ui32Stride;
    if(multi >= nload)
        return 1;

    return (multi + 63) & 0xFFFFFFC0;
}


ISPOST_UINT32 IPU_ISPLUS::IspostGetFileSize(FILE* pFileID)
{
    ISPOST_UINT32 pos = ftell(pFileID);
    ISPOST_UINT32 len = 0;
    fseek(pFileID, 0L, SEEK_END);
    len = ftell(pFileID);
    fseek(pFileID, pos, SEEK_SET);
    return len;
}

bool IPU_ISPLUS::IspostLoadBinaryFile(const char* pszBufName, FRBuffer** ppBuffer, const char* pszFilenane)
{
    ISPOST_UINT32 nLoaded = 0, temp = 0;
    FILE* f = NULL;
    Buffer buf;

    if (*ppBuffer != NULL)
    {
        delete(*ppBuffer);
        *ppBuffer = NULL;
    }

    if ((f = fopen(pszFilenane, "rb")) == NULL){
        LOGE("failed to open %s!\n", pszFilenane);
        return false;
    }

    nLoaded = IspostGetFileSize(f);
    if(strncmp(pszBufName, "LcGridBuf", 9) == 0){
        temp = IspostGetGridTargetIndexMax(m_pPortUo->GetWidth(), m_pPortUo->GetHeight(), m_IspostUser.stLcInfo.stGridBufInfo.ui8Size, nLoaded);
        grid_target_index_max = (temp == 1) ? temp:(nLoaded / temp);

        LOGE("nLoaded:%d, temp:%d, sz:%d, width:%d, height:%d, grid_target_index_max:%d\n", nLoaded, temp, m_IspostUser.stLcInfo.stGridBufInfo.ui8Size, m_pPortDn->GetWidth(), m_pPortDn->GetHeight(), grid_target_index_max);
    }

    *ppBuffer = new FRBuffer(pszBufName, FRBuffer::Type::FIXED, 1, nLoaded);
    if (*ppBuffer ==  NULL){
        fclose(f);
        LOGE("failed to allocate buffer.\n");
        return false;
    }

    buf = (*ppBuffer)->GetBuffer();
    fread(buf.fr_buf.virt_addr, nLoaded, 1, f);
    (*ppBuffer)->PutBuffer(&buf);

    fclose(f);

    LOGD("success load file %s with %d size.\n", pszFilenane, nLoaded);

    return true;
}

ISPOST_UINT32 IPU_ISPLUS::IspostParseGirdSize(const char* pszFileName)
{
    char* pszHermite;
    pszHermite = strstr(pszFileName, "hermite32");
    if (pszHermite != NULL)
        return GRID_SIZE_32_32;
    pszHermite = strstr(pszFileName, "hermite16");
    if (pszHermite != NULL)
        return GRID_SIZE_16_16;
    pszHermite = strstr(pszFileName, "hermite8");
    if (pszHermite != NULL)
        return GRID_SIZE_8_8;
    return GRID_SIZE_8_8;
}

void IPU_ISPLUS::IspostCalcPlanarAddress(int w, int h, ISPOST_UINT32& Yaddr, ISPOST_UINT32& UVaddr)
{
    UVaddr = Yaddr + (w * h);
}

void IPU_ISPLUS::IspostCalcOffsetAddress(int x, int y, int Stride, ISPOST_UINT32& Yaddr, ISPOST_UINT32& UVaddr)
{
    Yaddr += ((y * Stride) + x);
    UVaddr += (((y / 2) * Stride) + x);
}

void IPU_ISPLUS::IspostDumpUserInfo(void)
{
    ISPOST_UINT8 ui8Idx;

    LOGE("--------------------------------------------------------\n");
    LOGE("ui32UpdateFlag: 0x%04X\n", m_IspostUser.ui32UpdateFlag);
    LOGE("ui32PrintFlag: 0x%04X\n", m_IspostUser.ui32PrintFlag);
    LOGE("ui32IspostCount: %d\n", m_IspostUser.ui32IspostCount);

    LOGE("--------------------------------------------------------\n");
    LOGE("LC(%d), W(%d) x H(%d)\n", m_IspostUser.stCtrl0.ui1EnLC, m_IspostUser.stLcInfo.stSrcImgInfo.ui16Width, m_IspostUser.stLcInfo.stSrcImgInfo.ui16Height);
    LOGE("OV0(%d), W(%d) x H(%d)\n", m_IspostUser.stCtrl0.ui1EnOV, m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Width, m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Height);
    LOGE("DN(%d), W(%d) x H(%d)\n", m_IspostUser.stCtrl0.ui1EnDN, m_IspostUser.stLcInfo.stOutImgInfo.ui16Width, m_IspostUser.stLcInfo.stOutImgInfo.ui16Height);
    LOGE("UO(%d), W(%d) x H(%d)\n", m_IspostUser.stCtrl0.ui1EnUO, m_IspostUser.stLcInfo.stOutImgInfo.ui16Width, m_IspostUser.stLcInfo.stOutImgInfo.ui16Height);
    LOGE("SS0(%d), W(%d) x H(%d)\n", m_IspostUser.stCtrl0.ui1EnSS0, m_IspostUser.stSS0Info.stOutImgInfo.ui16Width, m_IspostUser.stSS0Info.stOutImgInfo.ui16Height);
    LOGE("SS1(%d), W(%d) x H(%d)\n", m_IspostUser.stCtrl0.ui1EnSS1, m_IspostUser.stSS1Info.stOutImgInfo.ui16Width, m_IspostUser.stSS1Info.stOutImgInfo.ui16Height);

    LOGE("--------------------------------------------------------\n");
    LOGE("stLcInfo.stSrcImgInfo.ui32YAddr: 0x%X\n", m_IspostUser.stLcInfo.stSrcImgInfo.ui32YAddr);
    LOGE("stLcInfo.stSrcImgInfo.ui32UVAddr: 0x%X\n", m_IspostUser.stLcInfo.stSrcImgInfo.ui32UVAddr);
    LOGE("stLcInfo.stSrcImgInfo.ui16Width: %d\n", m_IspostUser.stLcInfo.stSrcImgInfo.ui16Width);
    LOGE("stLcInfo.stSrcImgInfo.ui16Height: %d\n", m_IspostUser.stLcInfo.stSrcImgInfo.ui16Height);
    LOGE("stLcInfo.stSrcImgInfo.ui16Stride: %d\n", m_IspostUser.stLcInfo.stSrcImgInfo.ui16Stride);
    LOGE("stLcInfo.stBackFillColor.ui8Y: %d\n", m_IspostUser.stLcInfo.stBackFillColor.ui8Y);
    LOGE("stLcInfo.stBackFillColor.ui8CB: %d\n", m_IspostUser.stLcInfo.stBackFillColor.ui8CB);
    LOGE("stLcInfo.stBackFillColor.ui8CR: %d\n", m_IspostUser.stLcInfo.stBackFillColor.ui8CR);
    LOGE("stLcInfo.stGridBufInfo.ui32BufAddr: 0x%X\n", m_IspostUser.stLcInfo.stGridBufInfo.ui32BufAddr);
    LOGE("stLcInfo.stGridBufInfo.ui16Stride: %d\n", m_IspostUser.stLcInfo.stGridBufInfo.ui16Stride);
    LOGE("stLcInfo.stGridBufInfo.ui8LineBufEnable: %d\n", m_IspostUser.stLcInfo.stGridBufInfo.ui8LineBufEnable);
    LOGE("stLcInfo.stGridBufInfo.ui8Size: %d\n", m_IspostUser.stLcInfo.stGridBufInfo.ui8Size);
    LOGE("stLcInfo.stGridBufInfo.ui32GridTargetIdx: %d\n", m_IspostUser.stLcInfo.stGridBufInfo.ui32GridTargetIdx);
    LOGE("stLcInfo.stPxlCacheMode.ui32PxlCacheMode: %d\n", m_IspostUser.stLcInfo.stPxlCacheMode.ui32PxlCacheMode);
    LOGE("stLcInfo.stPxlScanMode.ui8ScanW: %d\n", m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanW);
    LOGE("stLcInfo.stPxlScanMode.ui8ScanH: %d\n", m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanH);
    LOGE("stLcInfo.stPxlScanMode.ui8ScanM: %d\n", m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanM);
    LOGE("stLcInfo.stOutImgInfo.ui16Width: %d\n", m_IspostUser.stLcInfo.stOutImgInfo.ui16Width);
    LOGE("stLcInfo.stOutImgInfo.ui16Height: %d\n", m_IspostUser.stLcInfo.stOutImgInfo.ui16Height);
    LOGE("stLcInfo.stAxiId.ui8SrcReqLmt: %d\n", m_IspostUser.stLcInfo.stAxiId.ui8SrcReqLmt);
    LOGE("stLcInfo.stAxiId.ui8GBID: %d\n", m_IspostUser.stLcInfo.stAxiId.ui8GBID);
    LOGE("stLcInfo.stAxiId.ui8SRCID: %d\n", m_IspostUser.stLcInfo.stAxiId.ui8SRCID);

    LOGE("--------------------------------------------------------\n");
    LOGE("stOvInfo.ui8OverlayMode: %d\n", m_IspostUser.stOvInfo.stOverlayMode.ui32OvMode);
    for (ui8Idx = 0; ui8Idx < OVERLAY_CHNL_MAX; ui8Idx++)
    {
        LOGE("--------------------------------------------------------\n");
        LOGE("stOvInfo.ui8OvIdx[0]: %d\n", ui8Idx);
        LOGE("stOvInfo.stOvImgInfo[%d].ui32YAddr: 0x%X\n", ui8Idx, m_IspostUser.stOvInfo.stOvImgInfo[ui8Idx].ui32YAddr);
        LOGE("stOvInfo.stOvImgInfo[%d].ui32UVAddr: 0x%X\n", ui8Idx, m_IspostUser.stOvInfo.stOvImgInfo[ui8Idx].ui32UVAddr);
        LOGE("stOvInfo.stOvImgInfo[%d].ui16Stride: %d\n", ui8Idx, m_IspostUser.stOvInfo.stOvImgInfo[ui8Idx].ui16Stride);
        LOGE("stOvInfo.stOvImgInfo[%d].ui16Width: %d\n", ui8Idx, m_IspostUser.stOvInfo.stOvImgInfo[ui8Idx].ui16Width);
        LOGE("stOvInfo.stOvImgInfo[%d].ui16Height: %d\n", ui8Idx, m_IspostUser.stOvInfo.stOvImgInfo[ui8Idx].ui16Height);
        LOGE("stOvInfo.stOvOffset[%d].ui16X: %d\n", ui8Idx, m_IspostUser.stOvInfo.stOvOffset[ui8Idx].ui16X);
        LOGE("stOvInfo.stOvOffset[%d].ui16Y: %d\n", ui8Idx, m_IspostUser.stOvInfo.stOvOffset[ui8Idx].ui16Y);
        LOGE("stOvInfo.stOvAlpha[%d].stValue0: 0x%08X\n", ui8Idx, m_IspostUser.stOvInfo.stOvAlpha[ui8Idx].stValue0.ui32Alpha);
        LOGE("stOvInfo.stOvAlpha[%d].stValue1: 0x%08X\n", ui8Idx, m_IspostUser.stOvInfo.stOvAlpha[ui8Idx].stValue1.ui32Alpha);
        LOGE("stOvInfo.stOvAlpha[%d].stValue2: 0x%08X\n", ui8Idx, m_IspostUser.stOvInfo.stOvAlpha[ui8Idx].stValue2.ui32Alpha);
        LOGE("stOvInfo.stOvAlpha[%d].stValue3: 0x%08X\n", ui8Idx, m_IspostUser.stOvInfo.stOvAlpha[ui8Idx].stValue3.ui32Alpha);
        LOGE("stOvInfo.stAxiId[%d].ui8OVID: %d\n", ui8Idx, m_IspostUser.stOvInfo.stAxiId[ui8Idx].ui8OVID);
    }


    LOGE("--------------------------------------------------------\n");
    LOGE("stDnInfo.stOutImgInfo.ui32YAddr: 0x%X\n", m_IspostUser.stDnInfo.stOutImgInfo.ui32YAddr);
    LOGE("stDnInfo.stOutImgInfo.ui32UVAddr: 0x%X\n", m_IspostUser.stDnInfo.stOutImgInfo.ui32UVAddr);
    LOGE("stDnInfo.stOutImgInfo.ui16Stride: %d\n", m_IspostUser.stDnInfo.stOutImgInfo.ui16Stride);
    LOGE("stDnInfo.stRefImgInfo.ui32YAddr: 0x%X\n", m_IspostUser.stDnInfo.stRefImgInfo.ui32YAddr);
    LOGE("stDnInfo.stRefImgInfo.ui32UVAddr: 0x%X\n", m_IspostUser.stDnInfo.stRefImgInfo.ui32UVAddr);
    LOGE("stDnInfo.stRefImgInfo.ui16Stride: %d\n", m_IspostUser.stDnInfo.stRefImgInfo.ui16Stride);
    LOGE("stDnInfo.ui32DnTargetIdx: %d\n", m_IspostUser.stDnInfo.ui32DnTargetIdx);
    LOGE("stDnInfo.stAxiId.ui8RefWrID: %d\n", m_IspostUser.stDnInfo.stAxiId.ui8RefWrID);
    LOGE("stDnInfo.stAxiId.ui8RefRdID: %d\n", m_IspostUser.stDnInfo.stAxiId.ui8RefRdID);

    LOGE("--------------------------------------------------------\n");
    LOGE("stUoInfo.stOutImgInfo.ui32YAddr: 0x%X\n", m_IspostUser.stUoInfo.stOutImgInfo.ui32YAddr);
    LOGE("stUoInfo.stOutImgInfo.ui32UVAddr: 0x%X\n", m_IspostUser.stUoInfo.stOutImgInfo.ui32UVAddr);
    LOGE("stUoInfo.stOutImgInfo.ui16Stride: %d\n", m_IspostUser.stUoInfo.stOutImgInfo.ui16Stride);
    LOGE("stUoInfo.ui8ScanH: %d\n", m_IspostUser.stUoInfo.ui8ScanH);
    LOGE("stUoInfo.stAxiId.ui8RefWrID: %d\n", m_IspostUser.stUoInfo.stAxiId.ui8RefWrID);

    LOGE("--------------------------------------------------------\n");
    LOGE("stSS0Info.stOutImgInfo.ui32YAddr: 0x%X\n", m_IspostUser.stSS0Info.stOutImgInfo.ui32YAddr);
    LOGE("stSS0Info.stOutImgInfo.ui32UVAddr: 0x%X\n", m_IspostUser.stSS0Info.stOutImgInfo.ui32UVAddr);
    LOGE("stSS0Info.stOutImgInfo.ui16Stride: %d\n", m_IspostUser.stSS0Info.stOutImgInfo.ui16Stride);
    LOGE("stSS0Info.stOutImgInfo.ui16Width: %d\n", m_IspostUser.stSS0Info.stOutImgInfo.ui16Width);
    LOGE("stSS0Info.stOutImgInfo.ui16Height: %d\n", m_IspostUser.stSS0Info.stOutImgInfo.ui16Height);
    LOGE("stSS0Info.stHSF.ui16SF: %d\n", m_IspostUser.stSS0Info.stHSF.ui16SF);
    LOGE("stSS0Info.stHSF.ui8SM: %d\n", m_IspostUser.stSS0Info.stHSF.ui8SM);
    LOGE("stSS0Info.stVSF.ui16SF: %d\n", m_IspostUser.stSS0Info.stVSF.ui16SF);
    LOGE("stSS0Info.stVSF.ui8SM: %d\n", m_IspostUser.stSS0Info.stVSF.ui8SM);
    LOGE("stSS0Info.stAxiId.ui8SSWID: %d\n", m_IspostUser.stSS0Info.stAxiId.ui8SSWID);

    LOGE("--------------------------------------------------------\n");
    LOGE("stSS1Info.stOutImgInfo.ui32YAddr: 0x%X\n", m_IspostUser.stSS1Info.stOutImgInfo.ui32YAddr);
    LOGE("stSS1Info.stOutImgInfo.ui32UVAddr: 0x%X\n", m_IspostUser.stSS1Info.stOutImgInfo.ui32UVAddr);
    LOGE("stSS1Info.stOutImgInfo.ui16Stride: %d\n", m_IspostUser.stSS1Info.stOutImgInfo.ui16Stride);
    LOGE("stSS1Info.stOutImgInfo.ui16Width: %d\n", m_IspostUser.stSS1Info.stOutImgInfo.ui16Width);
    LOGE("stSS1Info.stOutImgInfo.ui16Height: %d\n", m_IspostUser.stSS1Info.stOutImgInfo.ui16Height);
    LOGE("stSS1Info.stHSF.ui16SF: %d\n", m_IspostUser.stSS1Info.stHSF.ui16SF);
    LOGE("stSS1Info.stHSF.ui8SM: %d\n", m_IspostUser.stSS1Info.stHSF.ui8SM);
    LOGE("stSS1Info.stVSF.ui16SF: %d\n", m_IspostUser.stSS1Info.stVSF.ui16SF);
    LOGE("stSS1Info.stVSF.ui8SM: %d\n", m_IspostUser.stSS1Info.stVSF.ui8SM);
    LOGE("stSS1Info.stAxiId.ui8SSWID: %d\n", m_IspostUser.stSS1Info.stAxiId.ui8SSWID);
    LOGE("--------------------------------------------------------\n");
}

int IPU_ISPLUS::IspostSetLcGridTargetIndex(int Index)
{
        if (Index < 0) return -1;
    return m_IspostUser.stLcInfo.stGridBufInfo.ui32GridTargetIdx
            = Index % grid_target_index_max;
}

int IPU_ISPLUS::IspostGetLcGridTargetIndex()
{
    return m_IspostUser.stLcInfo.stGridBufInfo.ui32GridTargetIdx;
}

int IPU_ISPLUS::IspostGetLcGridTargetCount()
{
    return grid_target_index_max;
}

int IPU_ISPLUS::IspostSet3dDnsCurve(const cam_3d_dns_attr_t *attr, int is_force)
{
    int ret = -1;
    unsigned char weighting = 1;
    unsigned char threshold[3]={20,20,20};
    Q3_REG_VALUE hermiteTable[Q3_DENOISE_REG_NUM];
    ISPOST_UINT32 i = 0;
    ISPOST_UINT32 j = 0;
    int idx = 0;


    ret = IspostCheckParam(attr);
    if (ret)
        return ret;

    if (is_force || (m_Dns3dAttr.y_threshold != attr->y_threshold)
        || (m_Dns3dAttr.u_threshold != attr->u_threshold)
        || (m_Dns3dAttr.v_threshold != attr->v_threshold)
        || (m_Dns3dAttr.weight != attr->weight))
    {
        memset(hermiteTable, 0, sizeof(hermiteTable));

        threshold[0] = attr->y_threshold;
        threshold[1] = attr->u_threshold;
        threshold[2] = attr->v_threshold;
        weighting = attr->weight;

        ret = DN_HermiteGenerator(threshold, weighting, hermiteTable);
        if (ret)
        {
            LOGE("(%s:%d) Error: HermiteGenerator ret =%d !!!\n", __func__, __LINE__, ret);
            return ret;
        }


        LOGD("(%s:%d) thr0=%d thr1=%d thr2=%d w=%d \n",__func__, __LINE__,
                threshold[0], threshold[1], threshold[2], weighting);

        for (i = 0; i < DN_MSK_CRV_IDX_MAX; i++)
        {
            m_IspostUser.stDnInfo.stDnMskCrvInfo[i].ui8MskCrvIdx = DN_MSK_CRV_IDX_Y + i;
            for (j = 0; j < DN_MSK_CRV_DNMC_MAX; j++)
            {
                idx = 1 + i + i * DN_MSK_CRV_DNMC_MAX + j;
                m_IspostUser.stDnInfo.stDnMskCrvInfo[i].stMskCrv[j].ui32MskCrv = hermiteTable[idx].value;
            }
        }
        m_Dns3dAttr = *attr;
    }

    return ret;
}

int IPU_ISPLUS::IspostProcPrivateData(ipu_v2500_private_data_t *pdata)
{
    int ret = -1;
    cam_3d_dns_attr_t *pattr = NULL;
    cam_common_t *pcmn = NULL;
    ISPOST_UINT32 autoCtrlIdx = 0;
    ISPOST_UINT32 maxDnCnt = 0;
    cam_3d_dns_attr_t autoDnsAttr;


    ret = IspostCheckParam(pdata);
    if (ret)
        return ret;

    pattr = &pdata->dns_3d_attr;
    ret = IspostCheckParam(pattr);
    if (ret)
        return ret;
    pcmn = (cam_common_t*)&pattr->cmn;

    if (CAM_CMN_AUTO == pcmn->mode)
    {
        maxDnCnt = sizeof(g_dnParamList) / sizeof(g_dnParamList[0]);
        memset(&autoDnsAttr, 0, sizeof(autoDnsAttr));

        autoCtrlIdx = pdata->iDnID;
        if ((m_IspostUser.stDnInfo.ui32DnTargetIdx != autoCtrlIdx)
            && (autoCtrlIdx < maxDnCnt))
        {
            autoDnsAttr.y_threshold = g_dnParamList[autoCtrlIdx].y_threshold;
            autoDnsAttr.u_threshold = g_dnParamList[autoCtrlIdx].u_threshold;
            autoDnsAttr.v_threshold = g_dnParamList[autoCtrlIdx].v_threshold;
            autoDnsAttr.weight = g_dnParamList[autoCtrlIdx].weight;
            pattr = &autoDnsAttr;

            m_IspostUser.stDnInfo.ui32DnTargetIdx = autoCtrlIdx;
        }
        else
        {
            pattr = &m_Dns3dAttr;
        }
    }

    ret = IspostSet3dDnsCurve(pattr, 0);

    return ret;
}
