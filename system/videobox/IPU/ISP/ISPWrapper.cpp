#include "ISPWrapper.h"
extern "C" {
#include <qsdk/items.h>
}

ST_ISP_Context g_ISP_Camera[MAX_CAM_NB];

int IAPI_ISP_Create(Json *js)
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

    g_ISP_Camera[0].u32BufferNum = 3;

    if (NULL != js )
    {
        g_ISP_Camera[0].u32BufferNum = js->GetObject("nbuffers") ? js->GetInt("nbuffers") : 3;  //sensor mode always 0
    }

    for (Idx = 0; Idx < MAX_CAM_NB; Idx++)
    {
        g_ISP_Camera[Idx].pCamera = NULL;
        g_ISP_Camera[Idx].pAE = NULL;
        g_ISP_Camera[Idx].pDNS = NULL;
        g_ISP_Camera[Idx].pAWB = NULL;
        g_ISP_Camera[Idx].pTNM = NULL;
        g_ISP_Camera[Idx].pCMC = NULL;
#ifdef COMPILE_ISP_V2505
        g_ISP_Camera[Idx].pLSH= NULL;
#endif
        g_ISP_Camera[Idx].u32SensorMode = 0;
        memset(g_ISP_Camera[Idx].szSensorName, 0, sizeof(g_ISP_Camera[Idx].szSensorName));
        memset(g_ISP_Camera[Idx].szSetupFile, 0, sizeof(g_ISP_Camera[Idx].szSetupFile));
        g_ISP_Camera[Idx].iExslevel = 0;
        g_ISP_Camera[Idx].iScenario = 0;
        g_ISP_Camera[Idx].iWBMode = 0;
        g_ISP_Camera[Idx].flDefTargetBrightness = 0.0;

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

    /*Note: Can't find one sensor itm*/
    if (nExist == 0)
    {
        LOGE("sensor name not in items file.\n");
        throw VBERR_RUNTIME;
    }

    Idx = 0;
    while(Idx < nExist)
    {
        g_ISP_Camera[Idx].Context = Idx;

        memcpy(g_ISP_Camera[Idx].szSensorName, sn_name[Idx], sizeof(sn_name[Idx]));

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
            strncpy(g_ISP_Camera[Idx].szSetupFile, tmp, sizeof(g_ISP_Camera[Idx].szSetupFile) - 1);
        } else {
            strncpy(g_ISP_Camera[Idx].szSetupFile, path, sizeof(g_ISP_Camera[Idx].szSetupFile) - 1);
        }

        if (NULL != js )
        {
            g_ISP_Camera[Idx].pCamera = new ISPC::Camera(g_ISP_Camera[Idx].Context, ISPC::Sensor::GetSensorId(g_ISP_Camera[Idx].szSensorName), mode[Idx],
                                                js->GetObject("flip") ? js->GetInt("flip") : 0, which[Idx]);
        }
        else
        {
            g_ISP_Camera[Idx].pCamera = new ISPC::Camera(g_ISP_Camera[Idx].Context, ISPC::Sensor::GetSensorId(g_ISP_Camera[Idx].szSensorName), mode[Idx], 0, which[Idx]);
        }
        if (!g_ISP_Camera[Idx].pCamera)
        {
            LOGE("new a camera instance failed\n.");
            return -1;
        }
#ifdef COMPILE_ISP_V2500
        if (ISPC::CAM_ERROR == g_ISP_Camera[Idx].pCamera->state)
#else //COMPILE_ISP_V2505
        if (ISPC::Camera::CAM_ERROR == g_ISP_Camera[Idx].pCamera->state)
#endif
        {
            LOGE("failed to create a camera instance\n");
            return -1;
        }

        if (!g_ISP_Camera[Idx].pCamera->bOwnSensor)
        {
            LOGE("Camera own a sensor failed\n");
            return -1;
        }
        ISPC::CameraFactory::populateCameraFromHWVersion(*g_ISP_Camera[Idx].pCamera, g_ISP_Camera[Idx].pCamera->getSensor());
#ifdef COMPILE_ISP_V2505
        g_ISP_Camera[Idx].pCamera->bUpdateASAP = true;
#endif
        g_ISP_Camera[Idx].pstShotFrame = (ISPC::Shot*)malloc(g_ISP_Camera[0].u32BufferNum * sizeof(ISPC::Shot));
        if (g_ISP_Camera[Idx].pstShotFrame == NULL)
        {
            LOGE("camera allocate Shot Frame buffer failed\n");
            return -1;
        }
        memset((char*)g_ISP_Camera[Idx].pstShotFrame, 0, g_ISP_Camera[0].u32BufferNum * sizeof(ISPC::Shot));
        Idx++;
    }

    g_ISP_Camera[0].iSensorNum = Idx;
    LOGE("(%s:L%d) m_iSensorNb=%d \n", __func__, __LINE__, g_ISP_Camera[0].iSensorNum);
    if (0 == g_ISP_Camera[0].iSensorNum)
        return -1;

    return 0;
}

bool IAPI_ISP_IsCameraNull(IMG_UINT32 Idx)
{
    return (g_ISP_Camera[Idx].pCamera == NULL);
}

bool IAPI_ISP_IsShotFrameHasData(IMG_UINT32 Idx, IMG_UINT32 iBufferID)
{
    if (g_ISP_Camera[Idx].pstShotFrame[iBufferID].BAYER.phyAddr
        && g_ISP_Camera[Idx].pstShotFrame[iBufferID].BAYER.data)
    {
        return true;
    }
    else
    {
        return false;
    }
}


int IAPI_ISP_GetSensorNum()
{
    return g_ISP_Camera[0].iSensorNum;
}

int IAPI_ISP_AutoControl(IMG_UINT32 Idx)
{
    int ret = IMG_SUCCESS;

    if(!g_ISP_Camera[Idx].pAE)
    {
        g_ISP_Camera[Idx].pAE = new ISPC::ControlAE();
        ret = g_ISP_Camera[Idx].pCamera->registerControlModule(g_ISP_Camera[Idx].pAE);
#ifdef COMPILE_ISP_V2500
		if(g_ISP_Camera[0].iSensorNum > 1)
			g_ISP_Camera[Idx].pAE->addPipeline(g_ISP_Camera[(Idx == 0 ? 1 : 0)].pCamera->getPipeline());
#endif
        if(IMG_SUCCESS != ret)
        {
            LOGE("register AE module failed\n");
            delete g_ISP_Camera[Idx].pAE;
            g_ISP_Camera[Idx].pAE = NULL;
        }
        else
        {
            IAPI_ISP_GetOriTargetBrightness(Idx);
        }
    }
    if(!g_ISP_Camera[Idx].pAWB)
    {
#if defined(COMPILE_ISP_V2505) && defined(AWB_ALG_PLANCKIAN)
        g_ISP_Camera[Idx].pAWB = new ISPC::ControlAWB_Planckian();
#else
        g_ISP_Camera[Idx].pAWB = new ISPC::ControlAWB_PID();
#endif
        ret = g_ISP_Camera[Idx].pCamera->registerControlModule(g_ISP_Camera[Idx].pAWB);
        if(IMG_SUCCESS != ret)
        {
            LOGE("register AWB module failed\n");
            delete g_ISP_Camera[Idx].pAWB;
            g_ISP_Camera[Idx].pAWB = NULL;
        }
#ifdef COMPILE_ISP_V2500
        if(g_ISP_Camera[0].iSensorNum > 1)
            g_ISP_Camera[Idx].pAWB->addPipeline(g_ISP_Camera[(Idx == 0 ? 1 : 0)].pCamera->getPipeline());
#endif
    }
#ifdef COMPILE_ISP_V2505
    if (!g_ISP_Camera[Idx].pLSH && g_ISP_Camera[Idx].pAWB)
    {
        g_ISP_Camera[Idx].pLSH = new ISPC::ControlLSH();
        ret = g_ISP_Camera[Idx].pCamera->registerControlModule(g_ISP_Camera[Idx].pLSH);
        if(IMG_SUCCESS !=ret)
        {
            LOGE("register pLSH module failed\n");
            delete g_ISP_Camera[Idx].pLSH;
            g_ISP_Camera[Idx].pLSH = NULL;
        }

        if (g_ISP_Camera[Idx].pLSH)
        {
            g_ISP_Camera[Idx].pLSH->registerCtrlAWB(g_ISP_Camera[Idx].pAWB);
            g_ISP_Camera[Idx].pLSH->enableControl(true);
        }
    }
#endif
    if(!g_ISP_Camera[Idx].pTNM)
    {
        g_ISP_Camera[Idx].pTNM = new ISPC::ControlTNM();
        ret = g_ISP_Camera[Idx].pCamera->registerControlModule(g_ISP_Camera[Idx].pTNM);
        if(g_ISP_Camera[0].iSensorNum > 1)
            g_ISP_Camera[Idx].pTNM->addPipeline(g_ISP_Camera[(Idx == 0 ? 1 : 0)].pCamera->getPipeline());

        if(IMG_SUCCESS != ret)
        {
            LOGE("register TNM module failed\n");
            delete g_ISP_Camera[Idx].pTNM;
            g_ISP_Camera[Idx].pTNM = NULL;
        }
    }
    if(!g_ISP_Camera[Idx].pDNS)
    {
        g_ISP_Camera[Idx].pDNS = new ISPC::ControlDNS();
        ret = g_ISP_Camera[Idx].pCamera->registerControlModule(g_ISP_Camera[Idx].pDNS);
#ifdef COMPILE_ISP_V2500
        if(g_ISP_Camera[0].iSensorNum > 1)
                g_ISP_Camera[Idx].pDNS->addPipeline(g_ISP_Camera[(Idx == 0 ? 1 : 0)].pCamera->getPipeline());
#endif
        if(IMG_SUCCESS != ret)
        {
            LOGE("register DNS module failed\n");
            delete g_ISP_Camera[Idx].pDNS;
            g_ISP_Camera[Idx].pDNS = NULL;
        }
    }
    if(!g_ISP_Camera[Idx].pCMC)
    {
        g_ISP_Camera[Idx].pCMC = new ISPC::ControlCMC();
        ret = g_ISP_Camera[Idx].pCamera->registerControlModule(g_ISP_Camera[Idx].pCMC);
        if(IMG_SUCCESS != ret)
        {
            LOGE("register m_Cam[Idx].pCMC module failed\n");
            delete g_ISP_Camera[Idx].pCMC;
            g_ISP_Camera[Idx].pCMC = NULL;
        }
    }
    return ret;
}

int IAPI_ISP_ConfigAutoControl(IMG_UINT32 Idx)
{
    if (g_ISP_Camera[Idx].pAE)
    {
        g_ISP_Camera[Idx].pAE->enableFlickerRejection(true);
    }

    if (g_ISP_Camera[Idx].pTNM)
    {
        g_ISP_Camera[Idx].pTNM->enableControl(true);
        g_ISP_Camera[Idx].pTNM->enableLocalTNM(true);
        if (!g_ISP_Camera[Idx].pAE || !g_ISP_Camera[Idx].pAE->isEnabled())
        {
            g_ISP_Camera[Idx].pTNM->setAllowHISConfig(true);
        }
    }

    if (g_ISP_Camera[Idx].pDNS)
    {
        g_ISP_Camera[Idx].pDNS->enableControl(true);
    }

    if (g_ISP_Camera[Idx].pAWB)
    {
        g_ISP_Camera[Idx].pAWB->enableControl(true);
    }

    return IMG_SUCCESS;
}

int IAPI_ISP_LoadParameters(IMG_UINT32 Idx)
{
    ISPC::ParameterList parameters;
    ISPC::ParameterFileParser fileParser;

    parameters = fileParser.parseFile(g_ISP_Camera[Idx].szSetupFile);
    if (!parameters.validFlag)
    {
        LOGE("failed to get parameters from setup file.\n");
        throw VBERR_BADPARAM;
    }
    if (g_ISP_Camera[Idx].pCamera->loadParameters(parameters) != IMG_SUCCESS)
    {
        LOGE("failed to load camera configuration.\n");
        throw VBERR_BADPARAM;
    }

    return 0;
}


int IAPI_ISP_SetCaptureIQFlag(IMG_UINT32 Idx,IMG_BOOL Flag)
{
    return g_ISP_Camera[Idx].pCamera->SetCaptureIQFlag(Flag);
}

int IAPI_ISP_SetMCIIFCrop(IMG_UINT32 Idx,int pipW,int pipH)
{
    g_ISP_Camera[Idx].pCamera->pipeline->getMCPipeline()->sIIF.ui16ImagerSize[0] = pipW / CI_CFA_WIDTH;
    g_ISP_Camera[Idx].pCamera->pipeline->getMCPipeline()->sIIF.ui16ImagerSize[1] = pipH / CI_CFA_HEIGHT;
    return 0;
}

int IAPI_ISP_GetMCIIFCrop(IMG_UINT32 Idx,int &width,int &height)
{
    width = g_ISP_Camera[Idx].pCamera->pipeline->getMCPipeline()->sIIF.ui16ImagerSize[0];
    height = g_ISP_Camera[Idx].pCamera->pipeline->getMCPipeline()->sIIF.ui16ImagerSize[1];
    return 0;
}


int IAPI_ISP_SetEncoderDimensions(IMG_UINT32 Idx, int imgW, int imgH)
{
    return g_ISP_Camera[Idx].pCamera->setEncoderDimensions(imgW, imgH);
}

int IAPI_ISP_SetDisplayDimensions(IMG_UINT32 Idx, int s32ImgW, int s32ImgH)
{
    return g_ISP_Camera[Idx].pCamera->setDisplayDimensions(s32ImgW, s32ImgH);
}

int IAPI_ISP_DisableDsc(IMG_UINT32 Idx)
{

    ISPC::ModuleOUT *pout = g_ISP_Camera[Idx].pCamera->getModule<ISPC::ModuleOUT>();
    if (pout)
    {
        pout->dataExtractionType = PXL_NONE;
        pout->dataExtractionPoint = CI_INOUT_NONE;
        pout->displayType = PXL_NONE;
    }
    return 0;
}

int IAPI_ISP_SetupModules(IMG_UINT32 Idx)
{
    g_ISP_Camera[Idx].pCamera->setupModules();
    return 0;
}

#ifdef COMPILE_ISP_V2500
int IAPI_ISP_DynamicChangeModuleParam(IMG_UINT32 Idx)
{
    int ret = g_ISP_Camera[Idx].pCamera->DynamicChangeModuleParam();

    return ret;
}
#endif

int IAPI_ISP_DynamicChangeIspParameters(IMG_UINT32 Idx)
{
    int ret = g_ISP_Camera[Idx].pCamera->DynamicChangeIspParameters();

    return ret;
}

int IAPI_ISP_DynamicChange3DDenoiseIdx(IMG_UINT32 Idx,IMG_UINT32 ui32DnTargetIdx)
{
    int ret = g_ISP_Camera[Idx].pCamera->DynamicChange3DDenoiseIdx(ui32DnTargetIdx);

    return ret;
}


int IAPI_ISP_Program(IMG_UINT32 Idx)
{
    g_ISP_Camera[Idx].pCamera->program();
    return 0;
}

int IAPI_ISP_AllocateBufferPool(IMG_UINT32 Idx,IMG_UINT32 buffNum,std::list<IMG_UINT32> &bufferIds)
{
    g_ISP_Camera[Idx].pCamera->allocateBufferPool(buffNum,bufferIds);
    return 0;
}

int IAPI_ISP_StartCapture(IMG_UINT32 Idx,const bool enSensor)
{
    g_ISP_Camera[Idx].pCamera->startCapture(enSensor);
    return 0;
}

int IAPI_ISP_StopCapture(IMG_UINT32 Idx,const bool enSensor)
{
    g_ISP_Camera[Idx].pCamera->stopCapture(enSensor);
    return 0;
}

int IAPI_ISP_SensorEnable(IMG_UINT32 Idx,const bool enSensor)
{
    if(enSensor)
        g_ISP_Camera[Idx].pCamera->sensor->enable();
    else
        g_ISP_Camera[Idx].pCamera->sensor->disable();
    return 0;
}

int IAPI_ISP_SensorReset(IMG_UINT32 Idx)
{
    g_ISP_Camera[Idx].pCamera->sensor->reset();
    return 0;
}

int IAPI_ISP_DualSensorUpdate(IMG_UINT32 Idx,ISPC::Shot &shot, ISPC::Shot &shot2)
{
#ifdef COMPILE_ISP_V2500
    g_ISP_Camera[Idx].pCamera->DualSensorUpdate(true, shot, shot2);
#else //COMPILE_ISP_V2505
    //todo
#endif
    return 0;
}

int IAPI_ISP_EnqueueShot(IMG_UINT32 Idx)
{
    return g_ISP_Camera[Idx].pCamera->enqueueShot();
}

int IAPI_ISP_AcquireShot(IMG_UINT32 Idx,ISPC::Shot &shot,bool updateControl)
{
    return g_ISP_Camera[Idx].pCamera->acquireShot(shot,updateControl);
}

int IAPI_ISP_DeleteShots(IMG_UINT32 Idx)
{
    g_ISP_Camera[Idx].pCamera->deleteShots();

    return 0;
}

int IAPI_ISP_ReleaseShot(IMG_UINT32 Idx,ISPC::Shot & shot)
{
    g_ISP_Camera[Idx].pCamera->releaseShot(shot);
    shot.YUV.phyAddr = 0;
    shot.BAYER.phyAddr = 0;
    return 0;
}

int IAPI_ISP_DeregisterBuffer(IMG_UINT32 Idx,IMG_UINT32 bufID)
{
    return g_ISP_Camera[Idx].pCamera->deregisterBuffer(bufID);
}

int IAPI_ISP_SetEncScaler(IMG_UINT32 Idx,int imgW,int imgH)
{
    ISPC::ModuleESC *pESC= NULL;
    MC_PIPELINE *pMCPipeline = NULL;
    IMG_UINT32 owidth, oheight;
    double esc_pitch[2];
    ISPC::ScalerRectType format;
    IMG_INT32 rect[4];

    //set isp output encoder scaler
    if (g_ISP_Camera[Idx].pCamera)
    {
        pESC = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleESC>();
        pMCPipeline = g_ISP_Camera[Idx].pCamera->pipeline->getMCPipeline();
        if ( NULL != pESC && NULL != pMCPipeline)
        {
            owidth = (pMCPipeline->sIIF.ui16ImagerSize[0] * CI_CFA_WIDTH);
            oheight = (pMCPipeline->sIIF.ui16ImagerSize[1] * CI_CFA_HEIGHT);
            LOGD("cfa widht = %d, cfa height= %d\n", pMCPipeline->sIIF.ui16ImagerSize[0], pMCPipeline->sIIF.ui16ImagerSize[1]);
            LOGD("owidht = %d, oheight = %d\n", owidth, oheight);
            esc_pitch[0] = (float)imgW / owidth;
            esc_pitch[1] = (float)imgH / oheight;
            LOGE("esc_pitch[0] =%f, esc_pitch[1] = %f\n", esc_pitch[0], esc_pitch[1]);
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

    return IMG_SUCCESS;

}

int IAPI_ISP_SetDscScaler(IMG_UINT32 Idx, int s32ImgW, int s32ImgH)
{
    ISPC::ModuleDSC *pdsc= NULL;
    MC_PIPELINE *pMCPipeline = NULL;
    IMG_UINT32 owidth, oheight;
    double dsc_pitch[2];
    ISPC::ScalerRectType format;
    IMG_INT32 rect[4];

    // set isp output display scaler
    if(g_ISP_Camera[Idx].pCamera)
    {
        pdsc = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleDSC>();
        pMCPipeline = g_ISP_Camera[Idx].pCamera->pipeline->getMCPipeline();
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

int IAPI_ISP_SetCIPipelineNum(IMG_UINT32 Idx)
{
#ifdef COMPILE_ISP_V2500
    g_ISP_Camera[Idx].pCamera->getPipeline()->getCIPipeline()->uiTotalPipeline = g_ISP_Camera[0].iSensorNum;
#else //COMPILE_ISP_V2505
    //todo
#endif
    return 0;
}


int IAPI_ISP_SetIifCrop(IMG_UINT32 Idx,int PipH,int PipW,int PipX,int PipY)
{
    ISPC::ModuleIIF *pIIF= NULL;
    MC_PIPELINE *pMCPipeline = NULL;
    IMG_UINT32 owidth, oheight;
    double iif_decimation[2];
    IMG_INT32 rect[4];

    if (g_ISP_Camera[Idx].pCamera)
    {
        pIIF= g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleIIF>();
        pMCPipeline = g_ISP_Camera[Idx].pCamera->pipeline->getMCPipeline();
        if ( NULL != pIIF && NULL != pMCPipeline)
        {
            owidth = (pMCPipeline->sIIF.ui16ImagerSize[0] * CI_CFA_WIDTH);
            oheight = (pMCPipeline->sIIF.ui16ImagerSize[1] * CI_CFA_HEIGHT);
            LOGD("cfa2 width = %d, cfa2 height= %d\n", pMCPipeline->sIIF.ui16ImagerSize[0], pMCPipeline->sIIF.ui16ImagerSize[1]);
            //printf("owidth = %d, oheight = %d\n", owidth, oheight);
            //printf("imgW = %d, imgH = %d\n", m_PipW, m_PipH);
            //printf("offsetx = %d, offsety = %d\n", m_PipX, m_PipY);
            iif_decimation[1] = 0;
            iif_decimation[0] = 0;
            if (((float)(PipX + PipW) / owidth) <= 1.0 && ((float)(PipY + PipH) / oheight) <= 1.0)
            {
                pIIF->aDecimation[0] = iif_decimation[0];
                pIIF->aDecimation[1] = iif_decimation[1];
                pIIF->aCropTL[0] = rect[0] = PipX; //offset x
                pIIF->aCropTL[1] = rect[1] = PipY; //offset y
                pIIF->aCropBR[0] = rect[2] = PipW + PipX - 2;
                pIIF->aCropBR[1] = rect[3] = PipH + PipY - 2;
                pIIF->requestUpdate();
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

int IAPI_ISP_SetSepiaMode(IMG_UINT32 Idx,int mode)
{
    if (g_ISP_Camera[Idx].pCamera)
    {
        g_ISP_Camera[Idx].pCamera->setSepiaMode(mode);
    }

    return IMG_SUCCESS;
}

int IAPI_ISP_SetScenario(IMG_UINT32 Idx,int sn)
{
    g_ISP_Camera[Idx].iScenario = sn;
    ISPC::ModuleR2Y *pR2Y = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
    ISPC::ModuleSHA *pSHA = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleSHA>();
    ISPC::ControlCMC* pCMC = g_ISP_Camera[Idx].pCamera->getControlModule<ISPC::ControlCMC>();
    IMG_UINT32 ui32DnTargetIdx;

    //sn =1 low light scenario, else hight light scenario
    if (g_ISP_Camera[Idx].iScenario < 1)
    {
        FlatModeStatusSet(false);

        pR2Y->fBrightness = pCMC->fBrightness_acm;
        pR2Y->fContrast = pCMC->fContrast_acm;
        pR2Y->aRangeMult[0] = pCMC->aRangeMult_acm[0];
        pR2Y->aRangeMult[1] = pCMC->aRangeMult_acm[1];
        pR2Y->aRangeMult[2] = pCMC->aRangeMult_acm[2];

        g_ISP_Camera[Idx].pAE->setTargetBrightness(pCMC->targetBrightness_acm);
        g_ISP_Camera[Idx].pCamera->sensor->setMaxGain(pCMC->fSensorMaxGain_acm);

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

        g_ISP_Camera[Idx].pAE->setTargetBrightness(pCMC->targetBrightness_fcm);
        g_ISP_Camera[Idx].pCamera->sensor->setMaxGain(pCMC->fSensorMaxGain_fcm);

        pR2Y->fOffsetU = 0;
        pR2Y->fOffsetV = 0;
    }

    g_ISP_Camera[Idx].pCamera->DynamicChange3DDenoiseIdx(ui32DnTargetIdx, true);
    g_ISP_Camera[Idx].pCamera->DynamicChangeIspParameters(true);

    pR2Y->requestUpdate();
    pSHA->requestUpdate();

    return IMG_SUCCESS;
}

int IAPI_ISP_GetScenario(IMG_UINT32 Idx,enum cam_scenario &scen)
{
    scen = (enum cam_scenario)g_ISP_Camera[Idx].iScenario;

    return 0;
}

int IAPI_ISP_SetExposureLevel(IMG_UINT32 Idx,int level)
{
    double exstarget = 0;
    double cur_target = 0;
    double target_brightness = 0;


    if (level > 4 || level < -4)
    {
        LOGE("the level %d is out of the range.\n", level);
        return -1;
    }

    if (g_ISP_Camera[Idx].pAE)
    {
        g_ISP_Camera[Idx].iExslevel = level;
        exstarget = (double)level / 4.0;
        cur_target = g_ISP_Camera[Idx].flDefTargetBrightness;

        target_brightness = cur_target + exstarget;
        if (target_brightness > 1.0)
            target_brightness = 1.0;
        if (target_brightness < -1.0)
            target_brightness = -1.0;

        g_ISP_Camera[Idx].pAE->setOriTargetBrightness(target_brightness);
    }
    else
    {
        LOGE("the AE is not register\n");
        return -1;
    }
    return IMG_SUCCESS;

}

int IAPI_ISP_GetOriTargetBrightness(IMG_UINT32 Idx)
{
    if (g_ISP_Camera[Idx].pAE)
    {
        g_ISP_Camera[Idx].flDefTargetBrightness = g_ISP_Camera[Idx].pAE->getOriTargetBrightness();
    }

    LOGD("sensor %d (%s:L%d) flDefTargetBrightness=%f\n",Idx, __func__, __LINE__, g_ISP_Camera[Idx].flDefTargetBrightness);

    return IMG_SUCCESS;
}

int IAPI_ISP_GetExposureLevel(IMG_UINT32 Idx,int &level)
{
    level = g_ISP_Camera[Idx].iExslevel;
    return IMG_SUCCESS;
}

int IAPI_ISP_SetAntiFlicker(IMG_UINT32 Idx,int freq)
{
    if (g_ISP_Camera[Idx].pAE)
    {
        g_ISP_Camera[Idx].pAE->enableFlickerRejection(true, freq);
    }

    return IMG_SUCCESS;
}

int IAPI_ISP_GetAntiFlicker(IMG_UINT32 Idx,int &freq)
{
    if (g_ISP_Camera[Idx].pAE)
    {
        freq = g_ISP_Camera[Idx].pAE->getFlickerRejectionFrequency();
    }

    return IMG_SUCCESS;
}

int IAPI_ISP_SetWb(IMG_UINT32 Idx,int mode)
{
    ISPC::ModuleCCM *pCCM = g_ISP_Camera[Idx].pCamera->getModule<ISPC::ModuleCCM>();
    ISPC::ModuleWBC *pWBC = g_ISP_Camera[Idx].pCamera->getModule<ISPC::ModuleWBC>();
    const ISPC::TemperatureCorrection &tmpccm = g_ISP_Camera[Idx].pAWB->getTemperatureCorrections();
    g_ISP_Camera[Idx].iWBMode = mode;
    if (mode  == 0)
    {
        IAPI_ISP_EnableAWB(Idx, true);
    }
    else if (mode > 0 && mode -1 < tmpccm.size())
    {
        IAPI_ISP_EnableAWB(Idx, false);
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

int IAPI_ISP_GetWb(IMG_UINT32 Idx,int &mode)
{
    mode = g_ISP_Camera[Idx].iWBMode;

    return 0;
}

int IAPI_ISP_CheckHSBCInputParam(int param)
{
    if (param < IPU_HSBC_MIN || param > IPU_HSBC_MAX)
    {
        LOGE("(%s,L%d) param %d is error!\n", __FUNCTION__, __LINE__, param);
        return -1;
    }
    return 0;
}


int IAPI_ISP_SetYuvBrightness(IMG_UINT32 Idx,int brightness)
{
    int ret = 0;
    ISPC::ModuleR2Y *pR2Y = NULL;
    ISPC::ControlTNM *pTNMC = NULL;
    double light = 0.0;
    bool bEnable = false;


    ret = IAPI_ISP_CheckHSBCInputParam(brightness);
    if (ret)
        return ret;

    pTNMC = g_ISP_Camera[Idx].pCamera->getControlModule<ISPC::ControlTNM>();
    if (pTNMC)
    {
        bEnable = pTNMC->isTnmCrvSimBrightnessContrastEnabled();
    }
    if (pTNMC && bEnable)
    {
        ret = IAPI_ISP_CalcHSBCToISP(pTNMC->TNMC_CRV_BRIGNTNESS.max, pTNMC->TNMC_CRV_BRIGNTNESS.min, brightness, light);
        if (ret)
            return ret;

        pTNMC->setTnmCrvBrightness(light);
    }
    else
    {
        LOGE("(%s,L%d) pTNMC is NULL error or system disabled using TNM curve to simulation brightness!\n", __FUNCTION__, __LINE__);
        pR2Y = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
        if (NULL == pR2Y)
        {
            LOGE("(%s,L%d) pR2Y is NULL error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
        ret = IAPI_ISP_CalcHSBCToISP(pR2Y->R2Y_BRIGHTNESS.max, pR2Y->R2Y_BRIGHTNESS.min, brightness, light);
        if (ret)
            return ret;

        pR2Y->fBrightness = light;
        pR2Y->requestUpdate();
    }

    return IMG_SUCCESS;
}

int IAPI_ISP_CalcHSBCToOut(double max, double min, double param, int &out_val)
{
    if (max <= min)
    {
        LOGE("(%s,L%d) max %f or min %f is error!\n", __FUNCTION__, __LINE__, max, min);
        return -1;
    }
    out_val = (int)floor((param - min) * (IPU_HSBC_MAX - IPU_HSBC_MIN) / (max - min) + 0.5);

    return 0;
}

int IAPI_ISP_CalcHSBCToISP(double max, double min, int param, double &out_val)
{
    if (max <= min)
    {
        LOGE("(%s,L%d) max %f or min %f is error!\n", __FUNCTION__, __LINE__, max, min);
        return -1;
    }

    out_val = ((double)param) * (max - min) / (IPU_HSBC_MAX - IPU_HSBC_MIN) + min;
    return 0;
}

int IAPI_ISP_GetYuvBrightness(IMG_UINT32 Idx,int &out_brightness)
{
    int ret = 0;
    ISPC::ModuleR2Y *pR2Y = NULL;
    ISPC::ControlTNM *pTNMC = NULL;
    double light = 0.0;
    bool bEnable = false;


    pTNMC = g_ISP_Camera[Idx].pCamera->getControlModule<ISPC::ControlTNM>();
    if (pTNMC)
    {
        bEnable = pTNMC->isTnmCrvSimBrightnessContrastEnabled();
    }
    if (pTNMC && bEnable)
    {
        light = pTNMC->getTnmCrvBrightness();
        ret = IAPI_ISP_CalcHSBCToOut(pTNMC->TNMC_CRV_BRIGNTNESS.max, pTNMC->TNMC_CRV_BRIGNTNESS.min, light, out_brightness);
    }
    else
    {
        LOGD("(%s,L%d) pTNMC is NULL error or system disabled using TNM curve to simulation brightness!\n", __FUNCTION__, __LINE__);
        pR2Y = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
        if (NULL == pR2Y)
        {
            LOGE("(%s,L%d) pR2Y is NULL error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
        light = pR2Y->fBrightness;
        ret = IAPI_ISP_CalcHSBCToOut(pR2Y->R2Y_BRIGHTNESS.max, pR2Y->R2Y_BRIGHTNESS.min, light, out_brightness);
    }
    if (ret)
        return ret;

    return IMG_SUCCESS;
}

int IAPI_ISP_GetEnvBrightness(IMG_UINT32 Idx,int &out_bv)
{
    double brightness = 0.0;

    if (g_ISP_Camera[Idx].pAE)
    {
        brightness = g_ISP_Camera[Idx].pAE->getCurrentBrightness();
        LOGD("brightness = %f\n", brightness);

        out_bv = (int)(brightness*127 + 127);
    }
    else
    {
        return -1;
    }
    return 0;
}

int IAPI_ISP_SetContrast (IMG_UINT32 Idx,int contrast)
{
    int ret = 0;
    ISPC::ModuleR2Y *pR2Y = NULL;
    ISPC::ControlTNM *pTNMC = NULL;
    double cst = 0.0;
    bool bEnable = false;


    ret = IAPI_ISP_CheckHSBCInputParam(contrast);
    if (ret)
        return ret;

    pTNMC = g_ISP_Camera[Idx].pCamera->getControlModule<ISPC::ControlTNM>();
    if (pTNMC)
    {
        bEnable = pTNMC->isTnmCrvSimBrightnessContrastEnabled();
    }
    if (pTNMC && bEnable)
    {
        ret = IAPI_ISP_CalcHSBCToISP(pTNMC->TNMC_CRV_CONTRAST.max, pTNMC->TNMC_CRV_CONTRAST.min, contrast, cst);
        if (ret)
            return ret;

        pTNMC->setTnmCrvContrast(cst);
    }
    else
    {
        LOGE("(%s,L%d) pTNMC is NULL error or system disabled using TNM curve to simulation contrast!\n", __FUNCTION__, __LINE__);
        pR2Y = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
        if (NULL == pR2Y)
        {
            LOGE("(%s,L%d) pR2Y is NULL error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
        ret = IAPI_ISP_CalcHSBCToISP(pR2Y->R2Y_CONTRAST.max, pR2Y->R2Y_CONTRAST.min, contrast, cst);
        if (ret)
            return ret;

        pR2Y->fContrast = cst;
        pR2Y->requestUpdate();
    }

    return IMG_SUCCESS;
}

int IAPI_ISP_GetContrast (IMG_UINT32 Idx,int &contrast)
{
    int ret = 0;
    ISPC::ModuleR2Y *pR2Y = NULL;
    ISPC::ControlTNM *pTNMC = NULL;
    double cst = 0.0;
    bool bEnable = false;


    pTNMC = g_ISP_Camera[Idx].pCamera->getControlModule<ISPC::ControlTNM>();
    if (pTNMC)
    {
        bEnable = pTNMC->isTnmCrvSimBrightnessContrastEnabled();
    }
    if (pTNMC && bEnable)
    {
        cst = pTNMC->getTnmCrvContrast();
        ret = IAPI_ISP_CalcHSBCToOut(pTNMC->TNMC_CRV_CONTRAST.max, pTNMC->TNMC_CRV_CONTRAST.min, cst, contrast);
    }
    else
    {
        LOGE("(%s,L%d) pTNMC is NULL error or system disabled using TNM curve to simulation contrast!\n", __FUNCTION__, __LINE__);
        pR2Y = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
        if (NULL == pR2Y)
        {
            LOGE("(%s,L%d) pR2Y is NULL error!\n", __FUNCTION__, __LINE__);
            return -1;
        }
        cst = pR2Y->fContrast;
        ret = IAPI_ISP_CalcHSBCToOut(pR2Y->R2Y_CONTRAST.max, pR2Y->R2Y_CONTRAST.min, cst, contrast);
    }
    if (ret)
        return ret;

    return IMG_SUCCESS;

}

int IAPI_ISP_SetSaturation(IMG_UINT32 Idx,int saturation)
{
    int ret = 0;

#if defined(USE_R2Y_INTERFACE)
    ISPC::ModuleR2Y *pr2y = NULL;
#else
    ISPC::ControlCMC *pCMC = NULL;
#endif

    double sat = 0.0;

    ret = IAPI_ISP_CheckHSBCInputParam(saturation);
    if (ret)
        return ret;

#if defined(USE_R2Y_INTERFACE)
    pr2y = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
    if (NULL == pr2y)
    {
        LOGE("(%s,L%d) pR2Y is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ret = IAPI_ISP_CalcHSBCToISP(pr2y->R2Y_SATURATION.max, pr2y->R2Y_SATURATION.min, saturation, sat);
#else
    pCMC = g_ISP_Camera[Idx].pCamera->getControlModule<ISPC::ControlCMC>();
    if (NULL == pCMC)
    {
        LOGE("(%s,L%d) pCMC is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }
    ret = IAPI_ISP_CalcHSBCToISP(pCMC->CMC_R2Y_SATURATION_EXPECT.max, pCMC->CMC_R2Y_SATURATION_EXPECT.min, saturation, sat);
#endif

    if (ret)
        return ret;

#if defined(USE_R2Y_INTERFACE)
    pr2y->fSaturation = sat;
    pr2y->requestUpdate();
#else
    pCMC->fR2YSaturationExpect = sat;
    g_ISP_Camera[Idx].pCamera->DynamicChangeIspParameters(true);
#endif

    return IMG_SUCCESS;
}

int IAPI_ISP_GetSaturation(IMG_UINT32 Idx,int &saturation)
{
    int ret = 0;
#if defined(USE_R2Y_INTERFACE)
    ISPC::ModuleR2Y *pr2y = NULL;
#else
    ISPC::ControlCMC *pCMC = NULL;
#endif
    double sat = 0.0;

#if defined(USE_R2Y_INTERFACE)
    pr2y = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
    if (NULL == pr2y)
    {
        LOGE("(%s,L%d) pR2Y is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    sat = pr2y->fSaturation;
    ret = IAPI_ISP_CalcHSBCToOut(pr2y->R2Y_SATURATION.max, pr2y->R2Y_SATURATION.min, sat, saturation);
#else
    pCMC = g_ISP_Camera[Idx].pCamera->getControlModule<ISPC::ControlCMC>();
    if (NULL == pCMC)
    {
        LOGE("(%s,L%d) pCMC is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }
    sat = pCMC->fR2YSaturationExpect;
    ret = IAPI_ISP_CalcHSBCToOut(pCMC->CMC_R2Y_SATURATION_EXPECT.max, pCMC->CMC_R2Y_SATURATION_EXPECT.min, sat, saturation);
#endif

    if (ret)
        return ret;

    return IMG_SUCCESS;
}

int IAPI_ISP_SetSharpness(IMG_UINT32 Idx,int sharpness)
{
    int ret = 0;

    ISPC::ModuleSHA *psha = NULL;
    ISPC::ControlCMC* pCMC = NULL;

    double shap = 0.0;


    ret = IAPI_ISP_CheckHSBCInputParam(sharpness);
    if (ret)
        return ret;

    psha = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleSHA>();
    if (NULL == psha)
    {
        LOGE("(%s,L%d) pSHA is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    pCMC = g_ISP_Camera[Idx].pCamera->getControlModule<ISPC::ControlCMC>();
    if (NULL == pCMC) {
        LOGE("(%s,L%d) pCMC is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    ret = IAPI_ISP_CalcHSBCToISP(psha->SHA_STRENGTH.max, psha->SHA_STRENGTH.min, sharpness, shap);
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

int IAPI_ISP_GetSharpness(IMG_UINT32 Idx,int &sharpness)
{
    int ret = 0;
    ISPC::ModuleSHA *psha = NULL;
    ISPC::ControlCMC* pCMC = NULL;

    double fShap = 0.0;


    psha = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleSHA>();
    if (NULL == psha)
    {
        LOGE("(%s,L%d) pSHA is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    pCMC = g_ISP_Camera[Idx].pCamera->getControlModule<ISPC::ControlCMC>();
    if (NULL == pCMC) {
        LOGE("(%s,L%d) pCMC is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }
#if 0
    fShap = psha->fStrength;
#else
    pCMC->GetShaStrengthExpect(fShap);
#endif
    ret = IAPI_ISP_CalcHSBCToOut(psha->SHA_STRENGTH.max, psha->SHA_STRENGTH.min, fShap, sharpness);

    if (ret)
        return ret;

    return IMG_SUCCESS;
}

int IAPI_ISP_SetFps(IMG_UINT32 Idx,int fps)
{
    if (fps < 0)
    {
        if (IAPI_ISP_IsAEEnabled(Idx) && g_ISP_Camera[Idx].pAE)
            g_ISP_Camera[Idx].pAE->setAeExposureMethod(ISPC::ControlAE::AEMHD_EXP_LOWLUX);
    }
    else
    {
        if (IAPI_ISP_IsAEEnabled(Idx) && g_ISP_Camera[Idx].pAE)
            g_ISP_Camera[Idx].pAE->setAeExposureMethod(ISPC::ControlAE::AEMHD_EXP_DEFAULT);

        g_ISP_Camera[Idx].pCamera->sensor->SetFPS(fps);
    }

    return 0;
}

int IAPI_ISP_GetFps(IMG_UINT32 Idx,int &fps)
{
    SENSOR_STATUS status;

    if (g_ISP_Camera[Idx].pCamera)
    {
        g_ISP_Camera[Idx].pCamera->sensor->GetFlipMirror(status);
        fps = status.fCurrentFps;
        return 0;
    }
    else
    {
        return -1;
    }
}

int IAPI_ISP_EnableAE(IMG_UINT32 Idx,int enable)
{
    if (g_ISP_Camera[Idx].pAE)
    {
        g_ISP_Camera[Idx].pAE->enableAE(enable);
        return IMG_SUCCESS;
    }
    else
    {
        return -1;
    }
}

int IAPI_ISP_IsAEEnabled(IMG_UINT32 Idx)
{
    if (g_ISP_Camera[Idx].pAE)
    {
        return g_ISP_Camera[Idx].pAE->IsAEenabled();
    }
    else
    {
        return 0;
    }
}

int IAPI_ISP_EnableAWB(IMG_UINT32 Idx,int enable)
{
    if(g_ISP_Camera[Idx].pAWB)
    {
        g_ISP_Camera[Idx].pAWB->enableControl(enable);
        return IMG_SUCCESS;
    }
    else
    {
        return -1;
    }
}

int IAPI_ISP_IsAWBEnabled(IMG_UINT32 Idx)
{
    if (g_ISP_Camera[Idx].pAWB)
    {
        return g_ISP_Camera[Idx].pAWB->IsAWBenabled();
    }
    else
    {
        return 0;
    }
}

int IAPI_ISP_CheckSensorClass(IMG_UINT32 Idx)
{
    int ret = -1;

    if (NULL == g_ISP_Camera[Idx].pCamera || NULL == g_ISP_Camera[Idx].pCamera->sensor)
    {
        LOGE("(%s,L%d) Error: param %p %p are NULL !!!\n", __func__, __LINE__,
                g_ISP_Camera[Idx].pCamera, g_ISP_Camera[Idx].pCamera->sensor);
        return ret;
    }

    return 0;
}

int IAPI_ISP_SetSensorIso(IMG_UINT32 Idx,int iso)
{
    double fiso = 0.0;
    int ret = -1;
    double max_gain = 0.0;
    double min_gain = 0.0;
    bool isAeEnabled = false;

    ret = IAPI_ISP_CheckHSBCInputParam(iso);
    if (ret)
        return ret;

    isAeEnabled = IAPI_ISP_IsAEEnabled(Idx);
    min_gain = g_ISP_Camera[Idx].pCamera->sensor->getMinGain();
    if (g_ISP_Camera[Idx].pAE && isAeEnabled)
    {
        max_gain = g_ISP_Camera[Idx].pAE->getMaxOrgAeGain();
        if (iso > 0)
        {
            fiso = (double)iso;
            if (fiso < min_gain)
                fiso = min_gain;
            if (fiso > max_gain)
                fiso = max_gain;
            g_ISP_Camera[Idx].pAE->setMaxManualAeGain(fiso, true);
        }
        else if (0 == iso)
        {
            g_ISP_Camera[Idx].pAE->setMaxManualAeGain(max_gain, false);
        }
    }
    else
    {
        ret = IAPI_ISP_CheckSensorClass(Idx);
        if (ret)
            return ret;

        max_gain = g_ISP_Camera[Idx].pCamera->sensor->getMaxGain();
        min_gain = g_ISP_Camera[Idx].pCamera->sensor->getMinGain();
        fiso = (double)iso;
        if (fiso < min_gain)
            fiso = min_gain;
        if (fiso > max_gain)
            fiso = max_gain;
        if (iso > 0)
            g_ISP_Camera[Idx].pCamera->sensor->setGain(fiso);
    }
    LOGD("%s maxgain=%f, mingain=%f fiso=%f\n", __func__, max_gain, min_gain, fiso);

    return IMG_SUCCESS;
}

int IAPI_ISP_GetSensorIso(IMG_UINT32 Idx,int &iso)
{
    int ret = 0;
    bool bAeEnabled = false;

    ret = IAPI_ISP_CheckSensorClass(Idx);
    if (ret)
        return ret;

    bAeEnabled = IAPI_ISP_IsAEEnabled(Idx);
    if (g_ISP_Camera[Idx].pAE && bAeEnabled)
    {
        iso = (int)g_ISP_Camera[Idx].pAE->getMaxManualAeGain();
    }
    else
    {
        iso = (int)g_ISP_Camera[Idx].pCamera->sensor->getGain();
    }
    return IMG_SUCCESS;
}

int IAPI_ISP_EnableMonochrome(IMG_UINT32 Idx,int enable)
{
    g_ISP_Camera[Idx].pCamera->updateTNMSaturation(enable?0:1);
    return 0;
}

int IAPI_ISP_IsMonochromeEnabled(IMG_UINT32 Idx)
{
    return g_ISP_Camera[Idx].pCamera->IsMonoMode();
}

int IAPI_ISP_EnableWdr(IMG_UINT32 Idx,int enable)
{
    if (g_ISP_Camera[Idx].pTNM)
    {
        g_ISP_Camera[Idx].pTNM->enableLocalTNM(enable);
        return IMG_SUCCESS;
    }
    else
    {
        return -1;
    }
}

int IAPI_ISP_IsWdrEnabled(IMG_UINT32 Idx)
{
    if (g_ISP_Camera[Idx].pTNM)
    {
        return g_ISP_Camera[Idx].pTNM->getLocalTNM();//(m_Cam[Idx].pTNM->getAdaptiveTNM() && m_Cam[Idx].pTNM->getLocalTNM());
    }
    else
    {
        return 0;
    }
}

int IAPI_ISP_SetMirror(IMG_UINT32 Idx,enum cam_mirror_state dir)
{
    int flag = 0;

    switch (dir)
    {
        case cam_mirror_state::CAM_MIRROR_NONE: //0
        {
            if(g_ISP_Camera[Idx].pCamera)
            {
                flag = SENSOR_FLIP_NONE;
                g_ISP_Camera[Idx].pCamera->sensor->setFlipMirror(flag);
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
            if(g_ISP_Camera[Idx].pCamera)
            {
                flag = SENSOR_FLIP_HORIZONTAL;
                g_ISP_Camera[Idx].pCamera->sensor->setFlipMirror(flag);
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
            if(g_ISP_Camera[Idx].pCamera)
            {
                flag = SENSOR_FLIP_VERTICAL;
                g_ISP_Camera[Idx].pCamera->sensor->setFlipMirror(flag);
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
            if(g_ISP_Camera[Idx].pCamera)
            {
                flag = SENSOR_FLIP_BOTH;
                g_ISP_Camera[Idx].pCamera->sensor->setFlipMirror(flag);
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

int IAPI_ISP_GetMirror(IMG_UINT32 Idx,enum cam_mirror_state &dir)
{
    SENSOR_STATUS status;

    if (g_ISP_Camera[Idx].pCamera)
    {
        g_ISP_Camera[Idx].pCamera->sensor->GetFlipMirror(status);
        dir =  (enum cam_mirror_state)status.ui8Flipping;
        return 0;
    }
    else
    {
        return -1;
    }
}

int IAPI_ISP_SetSensorResolution(IMG_UINT32 Idx,const res_t res)
{
    if (g_ISP_Camera[Idx].pCamera)
    {
        return g_ISP_Camera[Idx].pCamera->sensor->SetResolution(res.W, res.H);
    }
    else
    {
        return -1;
    }
}

int IAPI_ISP_GetSnapResolution (IMG_UINT32 Idx, reslist_t *presl)
{
    reslist_t res;

    if (g_ISP_Camera[Idx].pCamera)
    {
        if (g_ISP_Camera[Idx].pCamera->sensor->GetSnapResolution(res) != IMG_SUCCESS)
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

int IAPI_ISP_SetHue(IMG_UINT32 Idx, int hue)
{
    int ret = 0;
    ISPC::ModuleR2Y *pR2Y = NULL;
    double fHue = 0.0;

    ret = IAPI_ISP_CheckHSBCInputParam(hue);
    if (ret)
        return ret;
    pR2Y = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
    if (NULL == pR2Y)
    {
        LOGE("(%s,L%d) pR2Y is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }
    ret = IAPI_ISP_CalcHSBCToISP(pR2Y->R2Y_HUE.max, pR2Y->R2Y_HUE.min, hue, fHue);
    if (ret)
        return ret;

    pR2Y->fHue = fHue;
    pR2Y->requestUpdate();
    return IMG_SUCCESS;
}

int IAPI_ISP_GetHue(IMG_UINT32 Idx, int &out_hue)
{
    int ret = 0;
    ISPC::ModuleR2Y *pR2Y = NULL;
    double fHue = 0.0;


    pR2Y = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleR2Y>();
    if (NULL == pR2Y) {
        LOGE("(%s,L%d) pR2Y is NULL error!\n", __FUNCTION__, __LINE__);
        return -1;
    }
    fHue = pR2Y->fHue;
    ret = IAPI_ISP_CalcHSBCToOut(pR2Y->R2Y_HUE.max, pR2Y->R2Y_HUE.min, fHue, out_hue);
    if (ret)
        return ret;

    LOGD("(%s,L%d) out_hue=%d\n", __FUNCTION__, __LINE__, out_hue);
    return IMG_SUCCESS;
}

bool IAPI_ISP_UseCustomGam(IMG_UINT32 Idx)
{
    return g_ISP_Camera[Idx].pCamera->getModule<ISPC::ModuleGMA>()->useCustomGam;
}


int IAPI_ISP_SetGMA(IMG_UINT32 Idx,cam_gamcurve_t *pGmaCurve)
{
    ISPC::ModuleGMA *pGMA = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleGMA>();
    CI_MODULE_GMA_LUT glut;
    CI_CONNECTION *conn  = g_ISP_Camera[Idx].pCamera->getConnection();

    if (pGMA == NULL) {
        return -1;
    }

    if (pGmaCurve != NULL) {
        pGMA->useCustomGam = 1;
        memcpy((void *)pGMA->customGamCurve, (void *)pGmaCurve->curve, sizeof(pGMA->customGamCurve));
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

int IAPI_ISP_SetCustomGMA(IMG_UINT32 Idx)
{
    ISPC::ModuleGMA *pGMA;
    pGMA = g_ISP_Camera[Idx].pCamera->getModule<ISPC::ModuleGMA>();
    if (pGMA != NULL)
    {
        if (pGMA->useCustomGam)
        {
            if (IAPI_ISP_SetGMA(Idx, NULL) != IMG_SUCCESS)
            {
                LOGE("failed to set gam curve %d\n", __LINE__);
                throw VBERR_RUNTIME;
            }
        }
    }

    return 0;
}


int IAPI_ISP_SetSensorExposure(IMG_UINT32 Idx, int usec)
{
    int ret = -1;

    ret = IAPI_ISP_IsAEEnabled(Idx) ? 0:1;
    if (ret)
        return ret;

    ret = IAPI_ISP_CheckSensorClass(Idx);
    if (ret)
        return ret;

    LOGD("(%s,L%d) usec=%d \n", __func__, __LINE__,
            usec);

    ret = g_ISP_Camera[Idx].pCamera->sensor->setExposure(usec);

    return ret;
}

int IAPI_ISP_GetSensorExposure(IMG_UINT32 Idx, int &usec)
{
    int ret = 0;

    ret = IAPI_ISP_CheckSensorClass(Idx);
    if (ret)
        return ret;

    usec = g_ISP_Camera[Idx].pCamera->sensor->getExposure();
    return IMG_SUCCESS;
}

int IAPI_ISP_SetDpfAttr(IMG_UINT32 Idx, const cam_dpf_attr_t *attr)
{
    int ret = -1;
    ISPC::ModuleDPF *pdpf = NULL;
    cam_common_t *pcmn = NULL;
    bool detect_en = false;

    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(g_ISP_Camera[Idx].pCamera);
    if (ret)
        return ret;

    pcmn = (cam_common_t*)&attr->cmn;
    pdpf = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleDPF>();

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

int IAPI_ISP_GetDpfAtrr(IMG_UINT32 Idx, cam_dpf_attr_t *attr)
{
    int ret = -1;
    ISPC::ModuleDPF *pdpf = NULL;
    cam_common_t *pcmn = NULL;


    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(g_ISP_Camera[Idx].pCamera);
    if (ret)
        return ret;

    pcmn = &attr->cmn;
    pdpf = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleDPF>();

    pcmn->mdl_en = pdpf->bDetect;
    attr->threshold = pdpf->ui32Threshold;
    attr->weight = pdpf->fWeight;

    return ret;
}

int IAPI_ISP_SetShaDenoiseAttr(IMG_UINT32 Idx, const cam_sha_dns_attr_t *attr)
{
    int ret = -1;
    ISPC::ModuleSHA *psha = NULL;
    cam_common_t *pcmn = NULL;
    bool bypass = 0;
    bool is_req = true;


    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(g_ISP_Camera[Idx].pCamera);
    if (ret)
        return ret;

    pcmn = (cam_common_t*)&attr->cmn;
    psha = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleSHA>();

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

int IAPI_ISP_GetShaDenoiseAtrr(IMG_UINT32 Idx, cam_sha_dns_attr_t *attr)
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
    ret = CheckParam(g_ISP_Camera[Idx].pCamera);
    if (ret)
        return ret;

    pcmn = &attr->cmn;
    psha = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleSHA>();

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

int IAPI_ISP_SetDnsAttr(IMG_UINT32 Idx, const cam_dns_attr_t *attr)
{
    int ret = -1;
    ISPC::ModuleDNS *pdns = NULL;
    cam_common_t *pcmn = NULL;
    bool autoctrl_en = 0;
    bool is_req = false;

    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(g_ISP_Camera[Idx].pCamera);
    if (ret)
        return ret;

    pdns = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleDNS>();
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
    if (g_ISP_Camera[Idx].pDNS)
    {
        g_ISP_Camera[Idx].pDNS->enableControl(autoctrl_en);
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

int IAPI_ISP_GetDnsAtrr(IMG_UINT32 Idx, cam_dns_attr_t *attr)
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
    ret = CheckParam(g_ISP_Camera[Idx].pCamera);
    if (ret)
        return ret;

    pdns = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleDNS>();
    pcmn = &attr->cmn;


    if (pdns->fStrength >= -EPSINON && pdns->fStrength <= EPSINON)
    {
        mdl_en = false;
    }
    else
    {
        mdl_en = true;
        if (g_ISP_Camera[Idx].pDNS)
        {
            autoctrl_en = g_ISP_Camera[Idx].pDNS->isEnabled();
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

int IAPI_ISP_SetWbAttr(IMG_UINT32 Idx, const cam_wb_attr_t *attr)
{
    int ret = -1;
    cam_common_t *pcmn = NULL;
    bool autoctrl_en = 0;
    bool is_req = false;

    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(g_ISP_Camera[Idx].pCamera);
    if (ret)
        return ret;

    pcmn = (cam_common_t*)&attr->cmn;
#ifdef COMPILE_ISP_V2500
    ISPC::ModuleWBC *pwbc = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleWBC>();
#else //COMPILE_ISP_V2505
    ISPC::ModuleWBC2_6 *pwbc = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleWBC2_6>();
#endif
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


    if (g_ISP_Camera[Idx].pAWB)
        g_ISP_Camera[Idx].pAWB->enableControl(autoctrl_en);
    if (is_req)
        pwbc->requestUpdate();


    return ret;
}

int IAPI_ISP_GetWbAtrr(IMG_UINT32 Idx, cam_wb_attr_t *attr)
{
    int ret = -1;
    cam_common_t *pcmn = NULL;
    bool mdl_en = 1;
    bool autoctrl_en = 0;
    cam_common_mode_e mode = CAM_CMN_AUTO;


    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(g_ISP_Camera[Idx].pCamera);
    if (ret)
        return ret;

    pcmn = &attr->cmn;
#ifdef COMPILE_ISP_V2500
    ISPC::ModuleWBC *pwbc = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleWBC>();
#else //COMPILE_ISP_V2505
    ISPC::ModuleWBC2_6 *pwbc = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleWBC2_6>();
#endif
    if (g_ISP_Camera[Idx].pAWB)
    {
        autoctrl_en = g_ISP_Camera[Idx].pAWB->isEnabled();
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

int IAPI_ISP_SetYuvGammaAttr(IMG_UINT32 Idx, const cam_yuv_gamma_attr_t *attr)
{
    int ret = -1;
    cam_common_t *pcmn = NULL;
    ISPC::ModuleTNM *ptnm = NULL;
    bool bypass = false;
    bool autoctrl_en = false;


    ret = CheckParam(attr);
    if (ret)
        return ret;
    ret = CheckParam(g_ISP_Camera[Idx].pCamera);
    if (ret)
        return ret;

    ptnm = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleTNM>();
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
            if (g_ISP_Camera[Idx].pTNM)
                g_ISP_Camera[Idx].pTNM->enableControl(autoctrl_en);
            ptnm->requestUpdate();
        }
        else
        {
            autoctrl_en = true;
            ptnm->bBypass = bypass;

            ptnm->requestUpdate();
            if (g_ISP_Camera[Idx].pTNM)
                g_ISP_Camera[Idx].pTNM->enableControl(autoctrl_en);
        }
    }
    else
    {
        bypass = true;
        autoctrl_en = false;
        ptnm->bBypass = bypass;

        if (g_ISP_Camera[Idx].pTNM)
            g_ISP_Camera[Idx].pTNM->enableControl(autoctrl_en);
        ptnm->requestUpdate();
    }

    return ret;
}

int IAPI_ISP_GetYuvGammaAtrr(IMG_UINT32 Idx, cam_yuv_gamma_attr_t *attr)
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
    ret = CheckParam(g_ISP_Camera[Idx].pCamera);
    if (ret)
        return ret;

    ptnm = g_ISP_Camera[Idx].pCamera->pipeline->getModule<ISPC::ModuleTNM>();
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
    if (g_ISP_Camera[Idx].pTNM)
        autoctrl_en = g_ISP_Camera[Idx].pTNM->isEnabled();
    if (autoctrl_en)
        mode = CAM_CMN_AUTO;
    else
        mode = CAM_CMN_MANUAL;
    pcmn->mdl_en = mdl_en;
    pcmn->mode = mode;

    return ret;
}

int IAPI_ISP_SetFpsRange(IMG_UINT32 Idx,const cam_fps_range_t *fps_range)
{
    int ret = -1;

    ret = CheckParam(fps_range);
    if (ret)
        return ret;

    ret = CheckParam(g_ISP_Camera[Idx].pCamera);
    if (ret)
        return ret;
    if (fps_range->min_fps > fps_range->max_fps)
    {
        LOGE("(%s,L%d) Error min %f is bigger than max %f \n",
                __func__, __LINE__, fps_range->min_fps, fps_range->max_fps);
        return -1;
    }

    if (fps_range->max_fps == fps_range->min_fps) {
        if (IAPI_ISP_IsAEEnabled(Idx) && g_ISP_Camera[Idx].pAE) {
            g_ISP_Camera[Idx].pAE->setAeExposureMethod(ISPC::ControlAE::AEMHD_EXP_DEFAULT);
        }

        g_ISP_Camera[Idx].pCamera->sensor->SetFPS((double)fps_range->max_fps);
        //clear highest bit of this byte means camera in default fps mode, assumes fps is not higher than 128
        return 1;
        LOGE("fps: %d, rangemode: 0\n", (int)fps_range->max_fps);
    } else {
        if (IAPI_ISP_IsAEEnabled(Idx) && g_ISP_Camera[Idx].pAE) {
            g_ISP_Camera[Idx].pAE->setFpsRange(fps_range->max_fps, fps_range->min_fps);
            g_ISP_Camera[Idx].pAE->setAeExposureMethod(ISPC::ControlAE::AEMHD_EXP_LOWLUX);
            LOGE("fps: %d, rangemode: 1\n", (int)fps_range->max_fps);
            return 2;
        }
    }

    return ret;
}

int IAPI_ISP_SkipFrame(IMG_UINT32 Idx, int skipNb)
{
    int ret = 0;
    int skipCnt = 0;
    ISPC::Shot frame;
    IMG_UINT32 CRCStatus = 0;
    int i = 0;


    while (skipCnt++ < skipNb)
    {
        g_ISP_Camera[Idx].pCamera->enqueueShot();
retry_1:
        ret = g_ISP_Camera[Idx].pCamera->acquireShot(frame, true);
        if (IMG_SUCCESS != ret)
        {
            IAPI_ISP_GetPhyStatus(Idx, &CRCStatus);
            if (CRCStatus != 0) {
                for (i = 0; i < 5; i++) {
                g_ISP_Camera[Idx].pCamera->sensor->reset();
                usleep(200000);
                goto retry_1;
                }
                i = 0;
            }
            LOGE("(%s:L%d) failed to get shot\n", __func__, __LINE__);
            return ret;
        }

        LOGD("(%s:L%d) skipcnt=%d phyAddr=0x%X \n", __func__, __LINE__, skipCnt, frame.YUV.phyAddr);
        g_ISP_Camera[Idx].pCamera->releaseShot(frame);
    }

    return ret;
}

int IAPI_ISP_GetDayMode(IMG_UINT32 Idx, enum cam_day_mode &day_mode)
{
    int ret = -1;

    ret = CheckParam(g_ISP_Camera[Idx].pCamera);
    if (ret)
        return ret;

    ret = g_ISP_Camera[Idx].pCamera->DayModeDetect((int &)day_mode);
    if (ret)
        return -1;

    return 0;
}

int IAPI_ISP_GetSensorsName(IMG_UINT32 Idx,cam_list_t *pName)
{
    if (NULL == pName)
    {
        LOGE("pName is invalid pointer\n");
        return -1;
    }

    std::list<std::pair<std::string, int> > sensors;
    std::list<std::pair<std::string, int> >::const_iterator it;
    int i = 0;
    g_ISP_Camera[Idx].pCamera->getSensor()->GetSensorNames(sensors);

    for(it = sensors.begin(); it != sensors.end(); it++)
    {
        strncpy(pName->name[i], it->first.c_str(), it->first.length());
        i++;
    }

    return IMG_SUCCESS;
}

int IAPI_ISP_GetPhyStatus(IMG_UINT32 Idx,IMG_UINT32 *status)
{
    IMG_UINT32 Gasket;
    CI_GASKET_INFO pGasketInfo;
    int ret = 0;

    memset(&pGasketInfo, 0, sizeof(CI_GASKET_INFO));
    CI_CONNECTION *conn  = g_ISP_Camera[Idx].pCamera->getConnection();
    g_ISP_Camera[Idx].pCamera->sensor->GetGasketNum(&Gasket);

    ret = CI_GasketGetInfo(&pGasketInfo, Gasket, conn);
    if (IMG_SUCCESS == ret) {
        *status = pGasketInfo.ui8MipiEccError || pGasketInfo.ui8MipiCrcError;
        LOGD("*****pGasketInfo = 0x%x***0x%x*** \n", pGasketInfo.ui8MipiEccError,pGasketInfo.ui8MipiCrcError);
        return ret;
    }

    return -1;
}

int IAPI_ISP_Destroy()
{
    IMG_UINT32 Idx = 0;

    for (Idx = 0; Idx < MAX_CAM_NB; Idx++)
    {
        if (g_ISP_Camera[Idx].pAE)
        {
            delete g_ISP_Camera[Idx].pAE;
            g_ISP_Camera[Idx].pAE = NULL;
        }
        if (g_ISP_Camera[Idx].pAWB)
        {
            delete g_ISP_Camera[Idx].pAWB;
            g_ISP_Camera[Idx].pAWB = NULL;
        }
        if (g_ISP_Camera[Idx].pTNM)
        {
            delete g_ISP_Camera[Idx].pTNM;
            g_ISP_Camera[Idx].pTNM = NULL;
        }
        if (g_ISP_Camera[Idx].pDNS)
        {
            delete g_ISP_Camera[Idx].pDNS;
            g_ISP_Camera[Idx].pDNS = NULL;
        }
        if (g_ISP_Camera[Idx].pCMC)
        {
            delete g_ISP_Camera[Idx].pCMC;
            g_ISP_Camera[Idx].pCMC = NULL;
        }
#ifdef COMPILE_ISP_V2505
        if (g_ISP_Camera[Idx].pLSH)
        {
            delete g_ISP_Camera[Idx].pLSH;
            g_ISP_Camera[Idx].pLSH = NULL;
        }
#endif
        if (g_ISP_Camera[Idx].pstShotFrame)
        {
            free(g_ISP_Camera[Idx].pstShotFrame);
            g_ISP_Camera[Idx].pstShotFrame = NULL;
        }
        if (g_ISP_Camera[Idx].pCamera)
        {
            delete g_ISP_Camera[Idx].pCamera;
            g_ISP_Camera[Idx].pCamera = NULL;
        }
    }

    return 0;
}

int IAPI_ISP_SaveFrameFile(const char *fileName, cam_data_format_e dataFmt, int idx, ISPC::Shot &shotFrame)
{
    int ret = -1;
    ISPC::Save saveFile;

    if (CAM_DATA_FMT_BAYER_FLX == dataFmt)
    {
         ret = saveFile.open(ISPC::Save::Bayer, *(g_ISP_Camera[idx].pCamera->getPipeline()),
            fileName);
    }
    else if (CAM_DATA_FMT_YUV == dataFmt)
    {
        ret = saveFile.open(ISPC::Save::YUV, *(g_ISP_Camera[idx].pCamera->getPipeline()),
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

int IAPI_ISP_SaveBayerRaw(const char *fileName, const ISPC::Buffer &bayerBuffer)
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


int IAPI_ISP_SaveFrame(int idx, ISPC::Shot &shotFrame,cam_save_data_t* psSaveDataParam)
{
    int ret = 0;
    cam_data_format_e dataFmt = CAM_DATA_FMT_NONE;
    struct timeval stTv;
    struct tm* stTm;
    ISPC::Save saveFile;
    char fileName[260];
    char preName[100];
    char fmtName[40];

    ret = CheckParam(psSaveDataParam);
    if (ret)
    {
        return -1;
    }
    gettimeofday(&stTv,NULL);
    stTm = localtime(&stTv.tv_sec);
    if (strlen(psSaveDataParam->file_name) > 0)
    {
        strncpy(preName, psSaveDataParam->file_name, sizeof(preName));
    }
    else
    {
        snprintf(preName,sizeof(preName), "%02d%02d-%02d%02d%02d-%03d",
                stTm->tm_mon + 1, stTm->tm_mday,
                stTm->tm_hour, stTm->tm_min, stTm->tm_sec,
                (int)(stTv.tv_usec / 1000));
    }

    if (psSaveDataParam->fmt & CAM_DATA_FMT_YUV)
    {
        dataFmt = CAM_DATA_FMT_YUV;
        snprintf(psSaveDataParam->file_name, sizeof(psSaveDataParam->file_name), "%s_%dx%d_%s.yuv",
                        preName, shotFrame.YUV.width, shotFrame.YUV.height,
                        FormatString(shotFrame.YUV.pxlFormat));
        snprintf(fileName, sizeof(fileName), "%s%s", psSaveDataParam->file_path, psSaveDataParam->file_name);
        ret = IAPI_ISP_SaveFrameFile(fileName, dataFmt, idx, shotFrame);
    }

    if (psSaveDataParam->fmt & CAM_DATA_FMT_BAYER_RAW)
    {
        dataFmt = CAM_DATA_FMT_BAYER_RAW;
        snprintf(fmtName, sizeof(fmtName),"%s%d", strchr(MosaicString(g_ISP_Camera[idx].pCamera->sensor->eBayerFormat), '_'),
                        g_ISP_Camera[idx].pCamera->sensor->uiBitDepth);
        snprintf(psSaveDataParam->file_name, sizeof(psSaveDataParam->file_name), "%s_%dx%d%s.raw",
                        preName, shotFrame.BAYER.width, shotFrame.BAYER.height,
                        fmtName);
        snprintf(fileName, sizeof(fileName), "%s%s", psSaveDataParam->file_path, psSaveDataParam->file_name);
        ret = IAPI_ISP_SaveBayerRaw(fileName, shotFrame.BAYER);
    }

    if (psSaveDataParam->fmt & CAM_DATA_FMT_BAYER_FLX)
    {
        dataFmt = CAM_DATA_FMT_BAYER_FLX;
        snprintf(psSaveDataParam->file_name, sizeof(psSaveDataParam->file_name), "%s_%dx%d_%s.flx",
                        preName, shotFrame.BAYER.width, shotFrame.BAYER.height,
                        FormatString(shotFrame.BAYER.pxlFormat));
        snprintf(fileName, sizeof(fileName), "%s%s", psSaveDataParam->file_path, psSaveDataParam->file_name);
        ret = IAPI_ISP_SaveFrameFile(fileName, dataFmt, idx, shotFrame);
    }

    return ret;
}

#ifdef COMPILE_ISP_V2505
int IAPI_ISP_GetCurrentFps(IMG_UINT32 Idx, int &out_fps)
{
    int ret = 0;
    SENSOR_STATUS status;
    ret = g_ISP_Camera[Idx].pCamera->sensor->GetFlipMirror(status);
    out_fps = status.fCurrentFps;
    if (ret)
        return ret;

    LOGD("(%s,L%d) out_hue=%d\n", __FUNCTION__, __LINE__, out_fps);
    return IMG_SUCCESS;
}

int IAPI_ISP_LoadControlParameters(IMG_UINT32 Idx)
{
    ISPC::ParameterList parameters;
    ISPC::ParameterFileParser fileParser;

    parameters = fileParser.parseFile(g_ISP_Camera[Idx].szSetupFile);
    if (!parameters.validFlag)
    {
        LOGE("failed to get parameters from setup file.\n");
        throw VBERR_BADPARAM;
    }

    if (g_ISP_Camera[Idx].pCamera->loadControlParameters(parameters) != IMG_SUCCESS)
    {
        LOGE("Failed to load control module parameters\n");
        throw VBERR_RUNTIME;
    }

    return 0;
}


int IAPI_ISP_EnableLSH(IMG_UINT32 Idx)
{
    int ret = 0;

    if (g_ISP_Camera[Idx].pLSH)
    {
        ISPC::ModuleLSH *pLSHmod = g_ISP_Camera[Idx].pCamera->getModule<ISPC::ModuleLSH>();

        if (pLSHmod)
        {
            g_ISP_Camera[Idx].pLSH->enableControl(pLSHmod->bEnableMatrix);
            if (pLSHmod->bEnableMatrix)
            {
                // load a default matrix before start
                g_ISP_Camera[Idx].pLSH->configureDefaultMatrix();
            }
        }
    }

    return ret;
}
#endif

int IAPI_ISP_SetModuleOUTRawFlag(IMG_UINT32 Idx, bool enable, CI_INOUT_POINTS DataExtractionPoint)
{
    int ret = 0;
    ISPC::ModuleOUT *out = g_ISP_Camera[Idx].pCamera->getModule<ISPC::ModuleOUT>();

    if (NULL == out)
    {
        LOGE("(%s:L%d) module out is NULL error !!!\n", __func__, __LINE__);
        return -1;
    }
    if (enable)
    {
        switch (g_ISP_Camera[Idx].pCamera->sensor->uiBitDepth)
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
    if (g_ISP_Camera[Idx].pCamera->setupModule(ISPC::STP_OUT) != IMG_SUCCESS)
    {
        LOGE("(%s:L%d) failed to setup OUT module\n", __func__, __LINE__);
        ret = -1;
    }

    return ret;
}

int IAPI_ISP_GetDebugInfo(void *info)
{
   int i = 0;
   ST_ISP_DBG_INFO *pISPInfo = NULL;

    pISPInfo = (ST_ISP_DBG_INFO *)info;

    for (i = 0; i < MAX_CAM_NB; i++)
    {
        if (!g_ISP_Camera[i].pCamera)
            continue;

        pISPInfo->s32CtxN = i+1;
        ISPC::ModuleTNM *pIspTNM = \
            g_ISP_Camera[i].pCamera->pipeline->getModule<ISPC::ModuleTNM>();

        pISPInfo->s32AEEn[i] = IAPI_ISP_IsAEEnabled(i);
        pISPInfo->s32AWBEn[i] = IAPI_ISP_IsAWBEnabled(i);

    #ifdef INFOTM_HW_AWB_METHOD
        pISPInfo->s32HWAWBEn[i] = g_ISP_Camera[i].pAWB->bHwAwbEnable;
    #else
        pISPInfo->s32HWAWBEn[i] = -1;
    #endif

        IAPI_ISP_GetDayMode(i, (enum cam_day_mode&)pISPInfo->s32DayMode[i]);
        IAPI_ISP_GetMirror(i, (enum cam_mirror_state&)pISPInfo->s32MirrorMode[i]);

        pISPInfo->s32TNMStaticCurve[i] = pIspTNM->bStaticCurve;

        pISPInfo->u32Exp[i] = g_ISP_Camera[i].pCamera->sensor->getExposure();
        pISPInfo->u32MinExp[i] = g_ISP_Camera[i].pCamera->sensor->getMinExposure();
        pISPInfo->u32MaxExp[i] = g_ISP_Camera[i].pCamera->sensor->getMaxExposure();
        LOGD("%d exp=%u min=%u max=%u\n",
            i, pISPInfo->u32Exp[i] , pISPInfo->u32MinExp[i], pISPInfo->u32MaxExp[i]);

        pISPInfo->f64Gain[i] = g_ISP_Camera[i].pCamera->sensor->getGain();
        pISPInfo->f64MinGain[i] = g_ISP_Camera[i].pCamera->sensor->getMinGain();
        pISPInfo->f64MaxGain[i] = g_ISP_Camera[i].pCamera->sensor->getMaxGain();
        LOGD("%d gain=%f min=%f max=%f\n",
            i, pISPInfo->f64Gain[i] , pISPInfo->f64MinGain[i], pISPInfo->f64MaxGain[i]);
    }

    return IMG_SUCCESS;
}

