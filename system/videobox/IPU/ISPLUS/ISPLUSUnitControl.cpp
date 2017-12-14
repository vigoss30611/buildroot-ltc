// Copyright (C) 2016 InfoTM, sam.zhou@infotm.com
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
#include <sys/shm.h>
#include "ISPLUS.h"

int IPU_ISPLUS::UnitControl(std::string func, void *arg) {

    LOGD("%s: %s\n", __func__, func.c_str());

    UCSet(func);

    UCFunction(camera_load_parameters) {
//        return LoadParameters((const char *)arg);
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_get_mirror) {
        enum cam_mirror_state &st = *(enum cam_mirror_state *)arg;

        return GetMirror(st);
    }

    UCFunction(camera_set_mirror) {
        return SetMirror(*(enum cam_mirror_state *)arg);
    }

    UCFunction(camera_get_fps) {
        int &val = *(int *)arg;

        return GetFPS(val);
    }

    UCFunction(camera_set_fps) {
        return SetFPS(*(int *)arg);
    }


    UCFunction(camera_get_brightness) {
        int &val = *(int *)arg;

        return GetYUVBrightness(val);
    }

    UCFunction(camera_set_brightness) {
        return SetYUVBrightness(*(int *)arg);
    }


    UCFunction(camera_get_contrast) {
        int &val = *(int *)arg;

        return GetContrast(val);
    }

    UCFunction(camera_set_contrast) {
        return SetContrast(*(int *)arg);
    }

    UCFunction(camera_get_saturation) {
        int &val = *(int *)arg;

        return GetSaturation(val);
    }
    UCFunction(camera_set_saturation) {
        return SetSaturation(*(int *)arg);
    }

    UCFunction(camera_get_sharpness) {
        int &val = *(int *)arg;

        return GetSharpness(val);
    }

    UCFunction(camera_set_sharpness) {
        return SetSharpness(*(int *)arg);
    }

    UCFunction(camera_get_antifog) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_set_antifog) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_get_antiflicker) {
        int &freq = *(int *)arg;

        return GetAntiFlicker(freq);
    }

    UCFunction(camera_set_antiflicker) {
        return SetAntiFlicker(*(int *)arg);
    }

    UCFunction(camera_get_wb) {
        int mode;
        return GetWB(mode);
    }

    UCFunction(camera_set_wb) {
        return SetWB(*(int *)arg);
    }

    UCFunction(camera_get_scenario) {
        enum cam_scenario scen;
        return GetScenario(scen);
    }

    UCFunction(camera_set_scenario) {
        return SetScenario(*(int *)arg);
    }

    UCFunction(camera_monochrome_is_enabled) {
        return IsMonchromeEnabled();
    }

    UCFunction(camera_monochrome_enable) {
        return EnableMonochrome(*(int *)arg);
    }

    UCFunction(camera_awb_is_enabled) {
        return IsAWBEnabled();
    }

    UCFunction(camera_awb_enable) {
        return EnableAWB(*(int *)arg);
    }

    UCFunction(camera_ae_is_enabled) {
        return IsAEEnabled();
    }

    UCFunction(camera_ae_enable) {
        return EnableAE(*(int *)arg);
    }

    UCFunction(camera_wdr_is_enabled) {
        return IsWDREnabled();
    }

    UCFunction(camera_wdr_enable) {
        return EnableWDR(*(int *)arg);
    }

    UCFunction(camera_get_snap_res) {
        return GetSnapResolution((reslist_t *)arg);
    }

    UCFunction(camera_set_snap_res) {
        return SetSnapResolution(*(res_t*)arg);
    }

    UCFunction(camera_snap_one_shot) {
        LOGD("in V2505 notify the snap one shot");
        return SnapOneShot();
    }

    UCFunction(camera_snap_exit) {
        return SnapExit();
    }

    UCFunction(camera_af_is_enabled) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_af_enable) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_reset_lens) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_focus_is_locked) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_focus_lock) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_get_focus) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_set_focus) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_get_zoom) {
        LOGE("%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }

    UCFunction(camera_get_hue) {
        int &val = *(int *)arg;

        return GetHue(val);
    }

    UCFunction(camera_set_hue) {
        return SetHue(*(int *)arg);
    }

    UCFunction(camera_get_wb) {
        int &val = *(int *)arg;

        return GetWB(val);
    }

    UCFunction(camera_set_wb) {
        return SetWB(*(int *)arg);
    }

    UCFunction(camera_get_brightnessvalue) {
        int &val = *(int *)arg;

        return GetEnvBrightnessValue(val);
    }

    UCFunction(camera_set_scenario) {
        return SetScenario(*(int *)arg);
    }

    UCFunction(camera_get_scenario) {
        enum cam_scenario &val = *(enum cam_scenario *)arg;

        return GetScenario(val);
    }

    UCFunction(camera_set_ISOLimit) {
        return SetSensorISO(*(int *)arg);
    }

    UCFunction(camera_get_ISOLimit) {
        int &val = *(int *)arg;

        return GetSensorISO(val);
    }

    UCFunction(camera_set_exposure) {
        return SetExposureLevel(*(int *)arg);
    }

    UCFunction(camera_get_exposure) {
        int &val = *(int *)arg;

        return GetExposureLevel(val);
    }


    UCFunction(camera_set_sensor_exposure) {
        return SetSensorExposure(*(int *)arg);
    }

    UCFunction(camera_get_sensor_exposure) {
        int &val = *(int *)arg;

        return GetSensorExposure(val);
    }

    UCFunction(camera_set_dpf_attr) {
        return SetDpfAttr((cam_dpf_attr_t*)arg);
    }

    UCFunction(camera_get_dpf_attr) {
        return GetDpfAttr((cam_dpf_attr_t*)arg);
    }

    UCFunction(camera_set_dns_attr) {
        return SetDnsAttr((cam_dns_attr_t*)arg);
    }

    UCFunction(camera_get_dns_attr) {
        return GetDnsAttr((cam_dns_attr_t*)arg);
    }

    UCFunction(camera_set_sha_dns_attr) {
        return SetShaDenoiseAttr((cam_sha_dns_attr_t*)arg);
    }

    UCFunction(camera_get_sha_dns_attr) {
        return GetShaDenoiseAttr((cam_sha_dns_attr_t*)arg);
    }

    UCFunction(camera_set_wb_attr) {
        return SetWbAttr((cam_wb_attr_t*)arg);
    }

    UCFunction(camera_get_wb_attr) {
        return GetWbAttr((cam_wb_attr_t*)arg);
    }

    UCFunction(camera_set_3d_dns_attr) {
        return Set3dDnsAttr((cam_3d_dns_attr_t*)arg);
    }

    UCFunction(camera_get_3d_dns_attr) {
        return Get3dDnsAttr((cam_3d_dns_attr_t*)arg);
    }

    UCFunction(camera_set_yuv_gamma_attr) {
        return SetYuvGammaAttr((cam_yuv_gamma_attr_t*)arg);
    }

    UCFunction(camera_get_yuv_gamma_attr) {
        return GetYuvGammaAttr((cam_yuv_gamma_attr_t*)arg);
    }

    UCFunction(camera_set_fps_range) {
        return SetFpsRange((cam_fps_range_t*)arg);
    }

    UCFunction(camera_get_fps_range) {
        return GetFpsRange((cam_fps_range_t*)arg);
    }

    UCFunction(camera_get_day_mode) {
        enum cam_day_mode &val = *(enum cam_day_mode *)arg;

        return GetDayMode(val);
    }

    UCFunction(camera_save_data) {
        return SaveData((cam_save_data_t*)arg);
    }

    UCFunction(videobox_control) {
        vbctrl_t *c = (vbctrl_t *)arg;
        switch (c->code) {
            case VBCTRL_INDEX:
                if(c->para < 0) return IspostGetLcGridTargetIndex();
                else return IspostSetLcGridTargetIndex(c->para);
            case VBCTRL_GET:
                return IspostGetLcGridTargetCount();
        }
        return 0;
    }

    UCFunction(video_set_pip_info) {
        IspostSetPipInfo((struct v_pip_info *)arg);
        return 0;
    }

    UCFunction(video_get_pip_info) {
        IspostGetPipInfo((struct v_pip_info *)arg);
        return 0;
    }
#ifdef MACRO_IPU_ISPLUS_FCE
    UCFunction(camera_create_and_fire_fcedata) {
        return IspostCreateAndFireFceData((cam_fcedata_param_t *)arg);
    }
#endif
    UCFunction(camera_load_and_fire_fcedata) {
        int ret = 0;
        key_t key = *(key_t*)arg;
        int id = 0;
        char *pMap = NULL;


        id = shmget(key, 0, 0666);
        if (id == -1) {
            LOGE("(%s:L%d) shmget is error (key=%d) !!!\n", __func__, __LINE__, key) ;
            goto toFail;
        }
        pMap = (char*)shmat(id, NULL, 0);
        if (NULL == pMap) {
            LOGE("(%s:L%d) p_map is NULL (shm_id=%d) !!!\n", __func__, __LINE__, id);
            goto toFail;
        }
        ret = IspostLoadAndFireFceData((cam_fcefile_param_t *)pMap);
        if (shmdt(pMap) == -1)
            LOGE("(%s:L%d) shmdt is error (pMap=%p) !!!\n", __func__, __LINE__, pMap);
        return ret;
toFail:
        return -1;
    }
    UCFunction(camera_set_fce_mode) {
        return IspostSetFceMode(*(int *)arg);
    }
    UCFunction(camera_get_fce_mode) {
        return IspostGetFceMode((int *)arg);
    }
    UCFunction(camera_get_fce_totalmodes) {
        return IspostGetFceTotalModes((int *)arg);
    }
    UCFunction(camera_save_fcedata) {
        return IspostSaveFceData((cam_save_fcedata_param_t *)arg);
    }
    LOGE("%s is not support\n", func.c_str());
    return VBENOFUNC;
}
#if 1

int IPU_ISPLUS::SetResolution(int imgW, int imgH)
{
    this->ImgH = imgH;
    this->ImgW = imgW;
    this->SetEncScaler(this->ImgW, this->ImgH);
    return IMG_SUCCESS;
}

//1 Sepia color
int IPU_ISPLUS::SetSepiaMode(int mode)
{
    if(this->pCamera)
    {
        this->pCamera->setSepiaMode(mode);
    }

    return IMG_SUCCESS;
}

int IPU_ISPLUS::SetScenario(int sn)
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

int  IPU_ISPLUS::GetAntiFlicker(int &freq)
{
    if (this->pAE)
    {
        freq = this->pAE->getFlickerRejectionFrequency();
    }

    return IMG_SUCCESS;
}

//set exposure level
int IPU_ISPLUS::SetExposureLevel(int level)
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

int IPU_ISPLUS::GetExposureLevel(int &level)
{
    level = this->exslevel;
    return IMG_SUCCESS;
}

int  IPU_ISPLUS::SetAntiFlicker(int freq)
{
    if (this->pAE)
    {
        this->pAE->enableFlickerRejection(true, freq);
    }

    return IMG_SUCCESS;
}

int IPU_ISPLUS::SetEncScaler(int imgW, int imgH)
{
    ISPC::ModuleESC *pesc= NULL;
    MC_PIPELINE *pMCPipeline = NULL;
    IMG_UINT32 owidth, oheight;
    double esc_pitch[2];
    ISPC::ScalerRectType format;
    IMG_INT32 rect[4];
    this->ImgH = imgH;
    this->ImgW = imgW;
    //set isp output encoder scaler

    if(this->pCamera)
    {
        pesc = this->pCamera->pipeline->getModule<ISPC::ModuleESC>();
        pMCPipeline = this->pCamera->pipeline->getMCPipeline();
        if( NULL != pesc && NULL != pMCPipeline)
        {
            owidth = (pMCPipeline->sIIF.ui16ImagerSize[0]*CI_CFA_WIDTH);
            oheight = (pMCPipeline->sIIF.ui16ImagerSize[1]*CI_CFA_HEIGHT);
            //LOGE("cfa width = %d, cfa height= %d\n", pMCPipeline->sIIF.ui16ImagerSize[0], pMCPipeline->sIIF.ui16ImagerSize[1]);
            //LOGE("owidth = %d, oheight = %d\n", owidth, oheight);
            esc_pitch[0] = (float)imgW/owidth;
            esc_pitch[1] = (float)imgH/oheight;
            LOGD("esc_pitch[0] =%f, esc_pitch[1] = %f\n", esc_pitch[0], esc_pitch[1]);
            if (esc_pitch[0] <= 1.0 && esc_pitch[1] <= 1.0)
            {
                pesc->aPitch[0] = esc_pitch[0];
                pesc->aPitch[1] = esc_pitch[1];
                pesc->aRect[0] = rect[0] = 0; //offset x
                pesc->aRect[1] = rect[1] = 0; //offset y
                pesc->aRect[2] = rect[2] = this->ImgW;
                pesc->aRect[3] = rect[3] = this->ImgH;
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


int IPU_ISPLUS::SetIIFCrop(int pipH, int pipW, int pipX, int pipY)
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


int IPU_ISPLUS::SetWB(int mode)
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

int IPU_ISPLUS::GetWB(int &mode)
{
    mode = this->wbMode;

    return 0;
}

int IPU_ISPLUS::GetYUVBrightness(int &out_brightness)
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

int IPU_ISPLUS::SetYUVBrightness(int brightness)
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

        //first disable ae, lbc
        if(this->pLBC)
        {
            this->EnableLBC(false);
        }

        pr2y->fBrightness = light;
        pr2y->requestUpdate();
    }

    return IMG_SUCCESS;
}

int IPU_ISPLUS::GetContrast(int &contrast)
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

int IPU_ISPLUS::SetContrast(int contrast)
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

        //first disable ae, lbc
        if(this->pLBC)
        {
            this->EnableLBC(false);
        }
        pr2y->fContrast = cst;
        pr2y->requestUpdate();
    }

    return IMG_SUCCESS;
}

int IPU_ISPLUS::GetSaturation(int &saturation)
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

int IPU_ISPLUS::SetSaturation(int saturation)
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
    //first disable lbc
    if(this->pLBC)
    {
        this->EnableLBC(false);
    }
    pr2y->fSaturation = sat;
    pr2y->requestUpdate();
#else
    pCMC->fR2YSaturationExpect = sat;
#endif

    return IMG_SUCCESS;
}

int IPU_ISPLUS::GetSharpness(int &sharpness)
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

int IPU_ISPLUS::SetSharpness(int sharpness)
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

    //first disable ae, lbc
    if(this->pLBC)
    {
        this->EnableLBC(false);
    }
#if 0
    psha->fStrength = shap;
    psha->requestUpdate();
#else
    pCMC->SetShaStrengthExpect(shap);
    pCMC->CalcShaStrength();
#endif

    return IMG_SUCCESS;
}

int IPU_ISPLUS::SetFPS(int fps)
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

        this->GetOwner()->SyncPathFps(0); //set 0 means camera in default fps mode
        this->pCamera->sensor->SetFPS(fps);
        this->GetOwner()->SyncPathFps(fps);
    }

    return 0;
}

int IPU_ISPLUS::GetFPS(int &fps)
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

int IPU_ISPLUS::GetScenario(enum cam_scenario &scen)
{
    scen = (enum cam_scenario)this->scenario;

    return 0;
}

int IPU_ISPLUS::GetResolution(int &imgW, int &imgH)
{
    imgH = this->ImgH;
    imgW = this->ImgW;

    return IMG_SUCCESS;
}

//get current frame statistics lumination value in AE
// enviroment brightness value
int IPU_ISPLUS::GetEnvBrightnessValue(int &out_bv)
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
int IPU_ISPLUS::EnableAE(int enable)
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

int IPU_ISPLUS::EnableAWB(int enable)
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

int IPU_ISPLUS::IsAWBEnabled()
{
    if(this->pAWB)
    {
        //return this->pAWB->IsdoAWBenabled(); //toedit
        return 1;
    }
    else
    {
        return 0;
    }
}

int IPU_ISPLUS::IsAEEnabled()
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

int IPU_ISPLUS::IsMonchromeEnabled()
{
    if(this->pLBC)
    {
        return this->pLBC->monoChrome;
    }
    else
    {
        return 0;
    }
}

int IPU_ISPLUS::SetSensorISO(int iso)
{
    double fiso = 0.0;
    int ret = -1;
    double max_gain = 0.0;
    double min_gain = 0.0;


    ret = CheckAEDisabled();
    if (ret)
        return ret;

    ret = CheckSensorClass();
    if (ret)
        return ret;

    ret = CheckHSBCInputParam(iso);
    if (ret)
        return ret;

    max_gain = this->pCamera->sensor->getMaxGain();
    min_gain = this->pCamera->sensor->getMinGain();

    LOGD("%s maxgain=%f, mingain=%f \n", __func__, max_gain, min_gain);
    ret = CalcHSBCToISP(max_gain, min_gain, iso, fiso);
    if (ret)
        return ret;

    this->pCamera->sensor->setGain(fiso);

    return IMG_SUCCESS;
}

int IPU_ISPLUS::GetSensorISO(int &iso)
{
    double fiso = 0.0;
    int ret = 0;
    double max_gain = 0.0;
    double min_gain = 0.0;


    ret = CheckSensorClass();
    if (ret)
        return ret;

    fiso = this->pCamera->sensor->getGain();
    max_gain = this->pCamera->sensor->getMaxGain();
    min_gain = this->pCamera->sensor->getMinGain();

    LOGD("%s maxgain=%f, mingain=%f \n", __func__, max_gain, min_gain);
    ret = CalcHSBCToOut(max_gain, min_gain, fiso, iso);
    if (ret)
        return ret;

    return IMG_SUCCESS;
}

//control monochrome
int IPU_ISPLUS::EnableMonochrome(int enable)
{
#if 0
    if(this->pLBC)
    {
        this->pLBC->monoChrome = enable;
        return IMG_SUCCESS;
    }
    else
    {
        return -1;
    }
#else
    this->pCamera->updateTNMSaturation(enable?0:1);
    return 0;
#endif
}

int IPU_ISPLUS::EnableWDR(int enable)
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

int IPU_ISPLUS::IsWDREnabled()
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

//control LBC enable
int IPU_ISPLUS::EnableLBC(int enable)
{
    if(this->pLBC)
    {
        this->pLBC->doLBC = enable;
        return IMG_SUCCESS;
    }
    else
    {
        return -1;
    }
}

int IPU_ISPLUS::GetMirror(enum cam_mirror_state &dir)
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

int IPU_ISPLUS::SetMirror(enum cam_mirror_state dir)
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

int IPU_ISPLUS::SetSensorResolution(const res_t res)
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

int IPU_ISPLUS::SetSnapResolution(const res_t res)
{
    return SetResolution(res.W, res.H);
}

int IPU_ISPLUS::SnapOneShot()
{
    //this->runSt = 1;
    pthread_mutex_lock(&mutex_x);
    runSt = CAM_RUN_ST_CAPTURING;
    pthread_mutex_unlock(&mutex_x);
    return IMG_SUCCESS;
}

int IPU_ISPLUS::SnapExit()
{
    //pthread_mutex_unlock(&capture_done_lock);
    //this->runSt = 2;
    pthread_mutex_lock(&mutex_x);
    runSt = CAM_RUN_ST_PREVIEWING;
    pthread_mutex_unlock(&mutex_x);
    return IMG_SUCCESS;
}

int IPU_ISPLUS::GetSnapResolution(reslist_t *presl)
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


IPU_ISPLUS::~IPU_ISPLUS()
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

    if (this->pLBC)
    {
        delete pLBC;
        pLBC = NULL;
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
    //release isplus port
    this->DestroyPort("out");
    this->DestroyPort("hist");
}

int IPU_ISPLUS::CheckHSBCInputParam(int param)
{
    if (param < IPU_HSBC_MIN || param > IPU_HSBC_MAX)
    {
        LOGE("(%s,L%d) param %d is error!\n", __FUNCTION__, __LINE__, param);
        return -1;
    }
    return 0;
}

int IPU_ISPLUS::CalcHSBCToISP(double max, double min, int param, double &out_val)
{
    if (max <= min)
    {
        LOGE("(%s,L%d) max %f or min %f is error!\n", __FUNCTION__, __LINE__, max, min);
        return -1;
    }

    out_val = ((double)param) * (max - min) / (IPU_HSBC_MAX - IPU_HSBC_MIN) + min;
    return 0;
}

int IPU_ISPLUS::CalcHSBCToOut(double max, double min, double param, int &out_val)
{
    if (max <= min)
    {
        LOGE("(%s,L%d) max %f or min %f is error!\n", __FUNCTION__, __LINE__, max, min);
        return -1;
    }
    out_val = (int)floor((param - min) * (IPU_HSBC_MAX - IPU_HSBC_MIN) / (max - min) + 0.5);

    return 0;
}

int IPU_ISPLUS::GetHue(int &out_hue)
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

int IPU_ISPLUS::SetHue(int hue)
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

    //first disable ae, lbc
    if(this->pLBC)
    {
        this->EnableLBC(false);
    }
    pr2y->fHue = fHue;
    pr2y->requestUpdate();
    return IMG_SUCCESS;
}

int IPU_ISPLUS::CheckAEDisabled()
{
    int ret = -1;


    if (IsAEEnabled())
    {
        LOGE("(%s,L%d) Error: Not disable AE !!!\n", __func__, __LINE__);
        return ret;
    }
    return 0;
}

int IPU_ISPLUS::SetGAMCurve(cam_gamcurve_t *GamCurve)
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

int IPU_ISPLUS::CheckSensorClass()
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

int IPU_ISPLUS::SetSensorExposure(int usec)
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

int IPU_ISPLUS::GetSensorExposure(int &usec)
{
    int ret = 0;


    ret = CheckSensorClass();
    if (ret)
        return ret;

    usec = this->pCamera->sensor->getExposure();
    return IMG_SUCCESS;
}

int IPU_ISPLUS::SetDpfAttr(const cam_dpf_attr_t *attr)
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


int IPU_ISPLUS::GetDpfAttr(cam_dpf_attr_t *attr)
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

int IPU_ISPLUS::SetShaDenoiseAttr(const cam_sha_dns_attr_t *attr)
{
    int ret = -1;
    ISPC::ModuleSHA *psha = NULL;
    cam_common_t *pcmn = NULL;
    bool bypass = 0;
    bool autoctrl_en = 0;
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
            autoctrl_en = false;
            psha->fDenoiseSigma = attr->strength;
            psha->fDenoiseTau = attr->smooth;
        }
        else
        {
            autoctrl_en = true;
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
    if(this->pLBC)
    {
        pLBC->enableControl(autoctrl_en);
    }
    if(is_req)
        psha->requestUpdate();

    LOGD("(%s,L%d) mdl_en=%d mode=%d sigma=%f tau=%f \n",
        __func__, __LINE__,
        attr->cmn.mdl_en, attr->cmn.mode,
        attr->strength, attr->smooth
    );


    return ret;
}


int IPU_ISPLUS::GetShaDenoiseAttr(cam_sha_dns_attr_t *attr)
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

    if(this->pLBC)
    {
        autoctrl_en = pLBC->isEnabled();
    }
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

int IPU_ISPLUS::SetDnsAttr(const cam_dns_attr_t *attr)
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


int IPU_ISPLUS::GetDnsAttr(cam_dns_attr_t *attr)
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

int IPU_ISPLUS::EnqueueReturnShot(void)
{
    struct ipu_v2500_private_data *returnPrivData = NULL;
    IMG_UINT32 i;

    for (i = 0; i < nBuffers; i++)
    {
        pthread_mutex_lock(&mutex_x);
        if (!m_postShots.empty()) {
            struct ipu_v2500_private_data *privData = (struct ipu_v2500_private_data *)(m_postShots.front());
            //NOTE: only SHOT_RETURNED will be releae
            //TODO: release all shot before SHOT_RETURNED
            if (privData->state == SHOT_RETURNED) {
                privData->state = SHOT_INITED;
                returnPrivData = privData;
             //   m_iBufUseCnt--;
                m_postShots.pop_front();
            }
        }
        pthread_mutex_unlock(&mutex_x);

        if (returnPrivData != NULL) {
            int bufID = returnPrivData->iBufID;
            pCamera->releaseShot(m_pstShotFrame[bufID]);
            pCamera->enqueueShot();
            m_pstShotFrame[bufID].YUV.phyAddr = 0;
            returnPrivData = NULL;
        }
        else {
            // no returned buffer
            break;
        }
    }
    return 0;
}

int IPU_ISPLUS::ClearAllShot()
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

int IPU_ISPLUS::SetWbAttr(const cam_wb_attr_t *attr)
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

int IPU_ISPLUS::GetWbAttr(cam_wb_attr_t *attr)
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

int IPU_ISPLUS::Set3dDnsAttr(const cam_3d_dns_attr_t *attr)
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

int IPU_ISPLUS::Get3dDnsAttr(cam_3d_dns_attr_t *attr)
{
    int ret = -1;


    ret = CheckParam(attr);
    if (ret)
        return ret;
    this->dns_3d_attr.cmn.mdl_en = 1;
    *attr = this->dns_3d_attr;

    return ret;
}

int IPU_ISPLUS::SetYuvGammaAttr(const cam_yuv_gamma_attr_t *attr)
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

int IPU_ISPLUS::GetYuvGammaAttr(cam_yuv_gamma_attr_t *attr)
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

int IPU_ISPLUS::SetFpsRange(const cam_fps_range_t *fps_range)
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
        this->GetOwner()->SyncPathFps(0); //set 0 means camera in default fps mode
        this->GetOwner()->SyncPathFps(fps_range->max_fps);
    } else {
        if (IsAEEnabled() && pAE) {
            pAE->setFpsRange(fps_range->max_fps, fps_range->min_fps);
            this->pAE->setAeExposureMethod(ISPC::ControlAE::AEMHD_EXP_LOWLUX);
            this->GetOwner()->SyncPathFps(255); //set 255 means camera in range fps mode
        }
    }

    sFpsRange = *fps_range;

    return ret;
}

int IPU_ISPLUS::GetFpsRange(cam_fps_range_t *fps_range)
{
    int ret = -1;

    ret = CheckParam(fps_range);
    if (ret)
        return ret;

    *fps_range = sFpsRange;

    return ret;
}

int IPU_ISPLUS::GetDayMode(enum cam_day_mode &out_day_mode)
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

int IPU_ISPLUS::SkipFrame(int skipNb)
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

int IPU_ISPLUS::SaveBayerRaw(const char *fileName, const ISPC::Buffer &bayerBuffer)
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

int IPU_ISPLUS::SaveFrameFile(const char *fileName, cam_data_format_e dataFmt, ISPC::Shot &shotFrame)
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

int IPU_ISPLUS::SaveFrame(ISPC::Shot &shotFrame)
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


int IPU_ISPLUS::SaveData(cam_save_data_t *saveData)
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
    SnapOneShot();
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
    SnapExit();
    sem_destroy(&m_semNotify);
    pthread_mutex_lock(&mutex_x);
    m_psSaveDataParam = NULL;
    pthread_mutex_unlock(&mutex_x);

    return ret;
}

int IPU_ISPLUS::GetPhyStatus(IMG_UINT32 *status)
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
#endif

#ifdef MACRO_IPU_ISPLUS_FCE

int IPU_ISPLUS::CalcGridDataSize(TFceCalcGDSizeParam *pstDataSize)
{
    int ret = 0;

    if (NULL == pstDataSize)
    {
        LOGE("(%s:L%d) param is NULL !\n", __func__, __LINE__);
        return -1;
    }
    ret = FceCalGDSize(pstDataSize);
    if (ret)
    {
        LOGE("(%s:L%d) FceCalGDSize() error!\n", __func__, __LINE__);
        goto toOut;
    }
    pstDataSize->gridBufLen = (pstDataSize->gridBufLen + GRIDDATA_ALIGN_SIZE - 1) / GRIDDATA_ALIGN_SIZE * GRIDDATA_ALIGN_SIZE;
    pstDataSize->geoBufLen = (pstDataSize->geoBufLen + GRIDDATA_ALIGN_SIZE - 1) / GRIDDATA_ALIGN_SIZE * GRIDDATA_ALIGN_SIZE;
toOut:
    return ret;
}

int IPU_ISPLUS::GenGridData(TFceGenGridDataParam *stGridParam)
{
    int ret = 0;

    if (NULL == stGridParam)
    {
        LOGE("(%s:L%d) param is NULL !\n", __func__, __LINE__);
        return -1;
    }
    ret = FceGenGridData(stGridParam);
    if (ret)
    {
        LOGE("(%s:L%d) Genrating grid data error !\n", __func__, __LINE__);
    }

    return ret;
}

void IPU_ISPLUS::ParseFceDataParam(cam_fisheye_correction_t *pstFrom, TFceGenGridDataParam *pstTo)
{
    if (NULL == pstFrom || NULL == pstTo)
        return;
    pstTo->fisheyeMode = pstFrom->fisheye_mode;

    pstTo->fisheyeStartTheta = pstFrom->fisheye_start_theta;
    pstTo->fisheyeEndTheta = pstFrom->fisheye_end_theta;
    pstTo->fisheyeStartPhi = pstFrom->fisheye_start_phi;
    pstTo->fisheyeEndPhi = pstFrom->fisheye_end_phi;

    pstTo->fisheyeRadius = pstFrom->fisheye_radius;

    pstTo->rectCenterX = pstFrom->rect_center_x;
    pstTo->rectCenterY = pstFrom->rect_center_y;

    pstTo->fisheyeFlipMirror = pstFrom->fisheye_flip_mirror;
    pstTo->scalingWidth = pstFrom->scaling_width;
    pstTo->scalingHeight = pstFrom->scaling_height;
    pstTo->fisheyeRotateAngle = pstFrom->fisheye_rotate_angle;
    pstTo->fisheyeRotateScale = pstFrom->fisheye_rotate_scale;

    pstTo->fisheyeHeadingAngle = pstFrom->fisheye_heading_angle;
    pstTo->fisheyePitchAngle = pstFrom->fisheye_pitch_angle;
    pstTo->fisheyeFovAngle = pstFrom->fisheye_fov_angle;

    pstTo->coefficientHorizontalCurve = pstFrom->coefficient_horizontal_curve;
    pstTo->coefficientVerticalCurve = pstFrom->coefficient_vertical_curve;

    pstTo->fisheyeTheta1 = pstFrom->fisheye_theta1;
    pstTo->fisheyeTheta2 = pstFrom->fisheye_theta2;
    pstTo->fisheyeTranslation1 = pstFrom->fisheye_translation1;
    pstTo->fisheyeTranslation2 = pstFrom->fisheye_translation2;
    pstTo->fisheyeTranslation3 = pstFrom->fisheye_translation3;
    pstTo->fisheyeCenterX2 = pstFrom->fisheye_center_x2;
    pstTo->fisheyeCenterY2 = pstFrom->fisheye_center_y2;
    pstTo->fisheyeRotateAngle2 = pstFrom->fisheye_rotate_angle2;

    pstTo->debugInfo = pstFrom->debug_info;
}

int IPU_ISPLUS::IspostCreateAndFireFceData(cam_fcedata_param_t *pstParam)
{
    int ret = -1;
    TFceCalcGDSizeParam stDataSize;
    TFceGenGridDataParam stGridParam;
    int gridSize = 0;
    int inWidth = 0;
    int inHeight = 0;
    ST_GRIDDATA_BUF_INFO stBufInfo;
    int fisheyeCenterX = 0;
    int fisheyeCenterY = 0;
    int listSize = 0;
    char szFrBufName[50];
    bool hasNewBuf = false;
    ST_FCE_MODE_GROUP stGroup;
    std::vector<ST_FCE_MODE_GROUP>::iterator idleIt;


    pthread_mutex_lock(&m_FceLock);
    if (m_HasFceFile)
    {
        LOGE("(%s:L%d) FCE had existed !\n", __func__, __LINE__);
        goto toOut;
    }
    inWidth = m_pPortIn->GetWidth();
    inHeight = m_pPortIn->GetHeight();
    switch (pstParam->common.grid_size)
    {
    case CAM_GRID_SIZE_32:
        gridSize = 32;
        break;
    case CAM_GRID_SIZE_16:
        gridSize = 16;
        break;
    case CAM_GRID_SIZE_8:
        gridSize = 8;
        break;
    default:
        gridSize = 32;
        break;
    }
    if (0 == pstParam->common.fisheye_center_x
        && 0 == pstParam->common.fisheye_center_y)
    {
        fisheyeCenterX = inWidth / 2;
        fisheyeCenterY = inHeight / 2;
    }
    else
    {
        fisheyeCenterX = pstParam->common.fisheye_center_x;
        fisheyeCenterY = pstParam->common.fisheye_center_y;
    }

    stDataSize.gridSize = gridSize;
    stDataSize.inWidth = inWidth;
    stDataSize.inHeight = inHeight;
    stDataSize.outWidth = pstParam->common.out_width;
    stDataSize.outHeight = pstParam->common.out_height;
    ret = CalcGridDataSize(&stDataSize);
    if (ret)
        goto calcSizeFail;

    memset(&stBufInfo, 0, sizeof(stBufInfo));
    m_stFce.maxModeGroupNb = CREATEFCEDATA_MAX;
    listSize = m_stFce.stModeGroupAllList.size();
    if (listSize < m_stFce.maxModeGroupNb)
    {
        sprintf(szFrBufName, "CreateGDGrid%d", listSize);
        stBufInfo.stBufGrid.virt_addr = fr_alloc_single(szFrBufName, stDataSize.gridBufLen, &stBufInfo.stBufGrid.phys_addr);
        if (NULL == stBufInfo.stBufGrid.virt_addr)
        {
            LOGE("(%s:L%d) failed to alloc grid!\n", __func__, __LINE__);
            goto allocFail;
        }
        stBufInfo.stBufGrid.size = stDataSize.gridBufLen;
        stBufInfo.stBufGrid.map_size = stDataSize.gridBufLen;
#if 0
        sprintf(szFrBufName, "CreateGDGeo%d", listSize);
        stBufInfo.stBufGeo.virt_addr = fr_alloc_single(szFrBufName, stDataSize.geoBufLen, &stBufInfo.stBufGeo.phys_addr);
        if (NULL == stBufInfo.stBufGeo.virt_addr)
        {
            LOGE("(%s:L%d) failed to alloc geo!\n", __func__, __LINE__);
            goto allocFail;
        }
        stBufInfo.stBufGeo.size = stDataSize.geoBufLen;
        stBufInfo.stBufGeo.map_size = stDataSize.geoBufLen;
#endif
        hasNewBuf = true;
        ret = 0;
    }
    else
    {
        if (!m_stFce.modeGroupIdleList.empty())
        {
            int idleIndex = 0;
            int foundGroup = false;

            idleIndex = m_stFce.modeGroupIdleList.front();
            m_stFce.modeGroupIdleList.pop_front();

            idleIt = m_stFce.stModeGroupAllList.begin();
            while (idleIt != m_stFce.stModeGroupAllList.end())
            {
                if (idleIt->groupID == idleIndex)
                {
                    foundGroup = true;
                    break;
                }
                ++idleIt;
            }
            if (foundGroup)
            {
                stBufInfo = idleIt->stModeAllList.at(0).stBufList.at(0);
            }
            if (NULL == stBufInfo.stBufGrid.virt_addr)
            {
                LOGE("(%s:L%d) grid buf is NULL !\n", __func__, __LINE__);
                goto toOut;
            }
            if (stDataSize.gridBufLen > stBufInfo.stBufGrid.size)
            {
                LOGE("(%s:%d) Error new grid size %d > old size %d \n", __func__, __LINE__,
                        stDataSize.gridBufLen, stBufInfo.stBufGrid.size);
                goto toOut;
            }
            if (stBufInfo.stBufGeo.virt_addr)
            {
                if (stDataSize.geoBufLen > stBufInfo.stBufGeo.size)
                {
                    LOGE("(%s:%d) Error new geo size %d > old size %d \n", __func__, __LINE__,
                            stDataSize.geoBufLen, stBufInfo.stBufGeo.size);
                    goto toOut;
                }
            }
            ret = 0;
        }
        else
        {
            LOGE("(%s:%d) Warning NO idle buf \n", __func__, __LINE__);
            goto toOut;
        }
    }
    ParseFceDataParam(&pstParam->fec, &stGridParam);
    stGridParam.fisheyeGridSize = stDataSize.gridSize;
    stGridParam.fisheyeImageWidth = stDataSize.inWidth;
    stGridParam.fisheyeImageHeight = stDataSize.inHeight;;
    stGridParam.fisheyeCenterX = fisheyeCenterX;
    stGridParam.fisheyeCenterY = fisheyeCenterY;
    stGridParam.rectImageWidth = stDataSize.outWidth;
    stGridParam.rectImageHeight = stDataSize.outHeight;
    stGridParam.gridBufLen = stDataSize.gridBufLen;
    stGridParam.geoBufLen = stDataSize.geoBufLen;
    stGridParam.fisheyeGridbuf = (unsigned char*)stBufInfo.stBufGrid.virt_addr;
    stGridParam.fisheyeGeobuf = (unsigned char*)stBufInfo.stBufGeo.virt_addr;
    ret = GenGridData(&stGridParam);
    if (ret)
        goto genFail;

    if (hasNewBuf)
    {
        ST_FCE_MODE stMode;

        stGroup.groupID = listSize;
        stGroup.stModeAllList.clear();
        stGroup.modePendingList.clear();
        stGroup.modeActiveList.clear();

        stMode.modeID = 0;
        stMode.stBufList.clear();
        stMode.stPosDnList.clear();
        stMode.stPosUoList.clear();
        stMode.stPosSs0List.clear();
        stMode.stPosSs1List.clear();

        stMode.stBufList.push_back(stBufInfo);
        if (m_pPortUo && m_pPortUo->IsEnabled())
        {
            ret = SetFceDefaultPos(m_pPortUo, 1, &stMode.stPosUoList);
            if (ret)
            {
                LOGE("(%s:L%d) failed to set uo default pos !\n", __func__, __LINE__);
            }
        }
        if (m_pPortDn && m_pPortDn->IsEnabled())
        {
            ret = SetFceDefaultPos(m_pPortDn, 1, &stMode.stPosDnList);
            if (ret)
            {
                LOGE("(%s:L%d) failed to set dn default pos !\n", __func__, __LINE__);
            }
        }
        if (m_pPortSs0 && m_pPortSs0->IsEnabled())
        {
            ret = SetFceDefaultPos(m_pPortSs0, 1, &stMode.stPosSs0List);
            if (ret)
            {
                LOGE("(%s:L%d) failed to set ss0 default pos !\n", __func__, __LINE__);
            }
        }
        if (m_pPortSs1 && m_pPortSs1->IsEnabled())
        {
            ret = SetFceDefaultPos(m_pPortSs1, 1, &stMode.stPosSs1List);
            if (ret)
            {
                LOGE("(%s:L%d) failed to set ss1 default pos !\n", __func__, __LINE__);
            }
        }
        stGroup.stModeAllList.push_back(stMode);
        stGroup.modeActiveList.push_back(0);

        m_stFce.stModeGroupAllList.push_back(stGroup);
        m_stFce.modeGroupActiveList.push_back(stGroup.groupID);
        m_stFce.hasFce = true;
        m_HasFceData = true;
    }
    else
    {
        idleIt->modeActiveList.push_back(0);
        idleIt->modePendingList.clear();
        m_stFce.modeGroupActiveList.push_back(idleIt->groupID);
    }
    pthread_mutex_unlock(&m_FceLock);
    return 0;

allocFail:
genFail:
    if (hasNewBuf)
    {
        if (stBufInfo.stBufGrid.virt_addr)
            fr_free_single(stBufInfo.stBufGrid.virt_addr, stBufInfo.stBufGrid.phys_addr);
        if (stBufInfo.stBufGeo.virt_addr)
            fr_free_single(stBufInfo.stBufGeo.virt_addr, stBufInfo.stBufGeo.phys_addr);
    }
calcSizeFail:
toOut:
    pthread_mutex_unlock(&m_FceLock);

    LOGE("(%s:%d) ret=%d\n", __func__, __LINE__, ret);
    return ret;
}

#endif //MACRO_IPU_ISPLUS_FCE

int IPU_ISPLUS::IspostLoadAndFireFceData(cam_fcefile_param_t *pstParam)
{
    int ret = 0;
    int modeGroupSize = 0;


    pthread_mutex_lock(&m_FceLock);
    if (m_HasFceData)
    {
        ret = -1;
        LOGE("(%s:L%d) FCE had existed !\n", __func__, __LINE__);
        goto toOut;
    }
    m_stFce.maxModeGroupNb = LOADFCEDATA_MAX;
    modeGroupSize = m_stFce.stModeGroupAllList.size();
    if (modeGroupSize < m_stFce.maxModeGroupNb)
    {
        ST_FCE_MODE_GROUP stGroup;

        stGroup.groupID = modeGroupSize;
        stGroup.stModeAllList.clear();
        stGroup.modePendingList.clear();
        stGroup.modeActiveList.clear();

        ret = ParseFceFileParam(&stGroup, stGroup.groupID, pstParam);
        if (ret)
            goto toOut;
        m_stFce.stModeGroupAllList.push_back(stGroup);
        m_stFce.modeGroupActiveList.push_back(modeGroupSize);
        m_stFce.hasFce = true;
        m_HasFceFile = true;
    }
    else
    {
        if (!m_stFce.modeGroupIdleList.empty())
        {
            int idleIndex = 0;
            ST_FCE_MODE_GROUP stIdleGroup;
            std::vector<ST_FCE_MODE_GROUP>::iterator idleIt;
            int foundGroup = false;

            idleIndex = m_stFce.modeGroupIdleList.front();
            m_stFce.modeGroupIdleList.pop_front();
            stIdleGroup = m_stFce.stModeGroupAllList.at(idleIndex);

            idleIt = m_stFce.stModeGroupAllList.begin();
            while (idleIt != m_stFce.stModeGroupAllList.end())
            {
                if (idleIt->groupID == idleIndex)
                {
                    foundGroup = true;
                    break;
                }
                ++idleIt;
            }
            if (foundGroup)
            {
                ret = ClearFceModeGroup(&(*idleIt));
                if (ret)
                    goto toOut;
            }
            stIdleGroup.modePendingList.clear();
            stIdleGroup.modeActiveList.clear();
            stIdleGroup.stModeAllList.clear();
            ret = ParseFceFileParam(&stIdleGroup, stIdleGroup.groupID, pstParam);
            if (ret)
                goto toOut;
            ret = UpdateFceModeGroup(&m_stFce, &stIdleGroup);
            if (ret)
                goto toOut;
            m_stFce.modeGroupActiveList.push_back(stIdleGroup.groupID);
        }
        else
        {
            LOGE("%s:%d Warning NO idle buf !!!\n", __func__, __LINE__);
            ret = -1;
            goto toOut;
        }
    }
    pthread_mutex_unlock(&m_FceLock);
    return 0;

toOut:
    pthread_mutex_unlock(&m_FceLock);
    LOGE("(%s:%d) ret=%d \n", __func__, __LINE__, ret);

    return ret;
}

int IPU_ISPLUS::IspostSetFceMode(int stMode)
{
    int ret = -1;
    int groupIndex = 0;
    int hasWork = 0;
    int maxMode = 0;


    LOGD("(%s:L%d) stMode=%d \n", __func__, __LINE__, stMode);
    pthread_mutex_lock(&m_FceLock);
    hasWork = !m_stFce.modeGroupPendingList.empty();
    if (hasWork)
    {
        std::vector<ST_FCE_MODE_GROUP>::iterator it;
        bool found = false;

        groupIndex = m_stFce.modeGroupPendingList.back();

        it = m_stFce.stModeGroupAllList.begin();
        while (it != m_stFce.stModeGroupAllList.end())
        {
            if (it->groupID == groupIndex)
            {
                found = true;
                break;
            }
            it++;
        }
        if (found)
        {
            maxMode = it->stModeAllList.size();
            if (stMode < maxMode)
            {
                it->modeActiveList.push_back(stMode);
                ret = 0;
            }
        }
    }
    pthread_mutex_unlock(&m_FceLock);

    return ret;
}


int IPU_ISPLUS::IspostGetFceMode(int *pMode)
{
    int ret = -1;
    int groupIndex = 0;
    int hasWork = 0;


    pthread_mutex_lock(&m_FceLock);
    hasWork = !m_stFce.modeGroupPendingList.empty();
    if (hasWork)
    {
        groupIndex = m_stFce.modeGroupPendingList.back();
        *pMode = (m_stFce.stModeGroupAllList.at(groupIndex)).modePendingList.back();
        ret = 0;
    }
    pthread_mutex_unlock(&m_FceLock);

    LOGD("(%s:L%d) ret=%d stMode=%d\n", __func__, __LINE__, ret, *pMode);

    return ret;
}

int IPU_ISPLUS::IspostGetFceTotalModes(int *pModeCount)
{
    int ret = -1;
    int groupIndex = 0;
    int hasWork = 0;


    pthread_mutex_lock(&m_FceLock);
    hasWork = !m_stFce.modeGroupPendingList.empty();
    if (hasWork)
    {
        groupIndex = m_stFce.modeGroupPendingList.back();
        *pModeCount = (m_stFce.stModeGroupAllList.at(groupIndex)).stModeAllList.size();
        ret = 0;
    }
    pthread_mutex_unlock(&m_FceLock);

    return ret;
}

int IPU_ISPLUS::IspostSaveFceData(cam_save_fcedata_param_t *pstParam)
{
    int ret = -1;
    int groupIndex = 0;
    int hasWork = 0;
    int modeIndex = 0;
    char szFileName[200];
    ST_FCE_MODE stMode;
    int nbBuf = 0;
    int i = 0;
    ST_GRIDDATA_BUF_INFO stBufInfo;
    FILE *pFd = NULL;
    ST_FCE_MODE_GROUP stModeGroup;


    pthread_mutex_lock(&m_FceLock);
    hasWork = !m_stFce.modeGroupPendingList.empty();
    if (hasWork)
    {
        groupIndex = m_stFce.modeGroupPendingList.back();
        stModeGroup = m_stFce.stModeGroupAllList.at(groupIndex);

        if (stModeGroup.modePendingList.empty())
            goto toOut;
        modeIndex = stModeGroup.modePendingList.back();

        stMode = m_stFce.stModeGroupAllList.at(groupIndex).stModeAllList.at(modeIndex);
        nbBuf = stMode.stBufList.size();
        for (i = 0; i < nbBuf; ++i)
        {
            stBufInfo = stMode.stBufList.at(i);
            if (stBufInfo.stBufGrid.size > 0)
            {
                snprintf(szFileName, sizeof(szFileName), "%s_grid%d.bin", pstParam->file_name, i);
                pFd = fopen(szFileName, "wb");
                if (pFd)
                {
                    fwrite(stBufInfo.stBufGrid.virt_addr, stBufInfo.stBufGrid.size, 1, pFd);
                    fclose(pFd);
                }
                else
                {
                    ret = -1;
                    LOGE("(%s:L%d) failed to open file %s !!!\n", __func__, __LINE__, szFileName);
                    break;
                }
            }
            if (stBufInfo.stBufGeo.size > 0)
            {
                snprintf(szFileName, sizeof(szFileName), "%s_geo%d.bin", pstParam->file_name, i);
                pFd = fopen(szFileName, "wb");
                if (pFd)
                {
                    fwrite(stBufInfo.stBufGeo.virt_addr, stBufInfo.stBufGrid.size, 1, pFd);
                    fclose(pFd);
                }
                else
                {
                    ret = -1;
                    LOGE("(%s:L%d) failed to open file %s !!!\n", __func__, __LINE__, szFileName);
                    break;
                }
            }
        }
        ret = 0;
    }
toOut:
    pthread_mutex_unlock(&m_FceLock);

    return ret;
}

int IPU_ISPLUS::UpdateFceModeGroup(ST_FCE *pstFce, ST_FCE_MODE_GROUP *pstGroup)
{
    int ret = -1;
    int found = false;
    std::vector<ST_FCE_MODE_GROUP>::iterator it;


    if (NULL == pstFce || NULL == pstGroup)
    {
        LOGE("(%s:L%d) param %p %p is NULL !!!\n", __func__, __LINE__, pstFce, pstGroup);
        return -1;
    }
    it = pstFce->stModeGroupAllList.begin();
    while (it != pstFce->stModeGroupAllList.end())
    {
        if (it->groupID == pstGroup->groupID)
        {
            found = true;
            break;
        }
        ++it;
    }
    if (found)
    {
        m_stFce.stModeGroupAllList.erase(it);
        m_stFce.stModeGroupAllList.insert(it, *pstGroup);
        ret = 0;
    }

    return ret;
}

int IPU_ISPLUS::GetFceActiveMode(ST_FCE_MODE *pstMode)
{
    int ret = -1;
    int groupIndex = 0;
    int modeIndex = 0;
    ST_FCE_MODE_GROUP stActiveGroup;
    int foundGroup = false;
    std::vector<ST_FCE_MODE_GROUP>::iterator it;


    if (!m_stFce.modeGroupActiveList.empty())
    {
        LOGD("(%s:L%d) active size=%d \n", __func__, __LINE__, m_stFce.modeGroupActiveList.size());

        groupIndex = m_stFce.modeGroupActiveList.front();
        m_stFce.modeGroupActiveList.pop_front();

        it = m_stFce.stModeGroupAllList.begin();
        while (it != m_stFce.stModeGroupAllList.end())
        {
            if(it->groupID == groupIndex)
            {
                foundGroup = true;
                break;
            }
            ++it;
        }
        if (foundGroup)
        {
            if (!it->modeActiveList.empty())
            {
                modeIndex = it->modeActiveList.front();
                it->modeActiveList.pop_front();
                *pstMode = it->stModeAllList.at(modeIndex);

                LOGD("(%s:L%d) active modeIndex=%d \n", __func__, __LINE__, modeIndex);

                it->modePendingList.push_back(modeIndex);
                m_stFce.modeGroupPendingList.push_back(groupIndex);
                ret = 0;
            }
            else
            {
                LOGE("(%s:L%d) failed to get active stMode! \n", __func__, __LINE__);
            }
        }

    }
    else
    {
        if (!m_stFce.modeGroupPendingList.empty())
        {
            groupIndex = m_stFce.modeGroupPendingList.back();

            it = m_stFce.stModeGroupAllList.begin();
            while (it != m_stFce.stModeGroupAllList.end())
            {
                if (it->groupID == groupIndex)
                {
                    foundGroup = true;
                    break;
                }
                ++it;
            }
            if (foundGroup)
            {
                if (!it->modeActiveList.empty())
                {
                    modeIndex = it->modeActiveList.front();
                    it->modeActiveList.pop_front();

                    it->modePendingList.push_back(modeIndex);
                    *pstMode = it->stModeAllList.at(modeIndex);
                    ret = 0;
                    LOGD("(%s:L%d) new active index=%d \n", __func__, __LINE__, modeIndex);
                }
                else if (!it->modePendingList.empty())
                {
                    modeIndex = it->modePendingList.back();
                    *pstMode = it->stModeAllList.at(modeIndex);
                    ret = 0;
                    LOGD("(%s:L%d) exist active index=%d \n", __func__, __LINE__, modeIndex);
                }
                else
                {
                    LOGE("(%s:L%d) failed to get active stMode !\n", __func__, __LINE__);
                }
            }
        }
        else
        {
            LOGE("(%s:L%d) Warning NO Worked stMode group \n", __func__, __LINE__);
        }
    }

    LOGD("(%s:L%d) ret=%d\n", __func__, __LINE__, ret);

    return ret;
}

int IPU_ISPLUS::UpdateFcePendingStatus()
{
    int ret = 0;
    int groupIndex = 0;


    if (!m_stFce.modeGroupPendingList.empty())
    {
        if (m_stFce.modeGroupPendingList.size() > 1)
        {
            groupIndex = m_stFce.modeGroupPendingList.front();
            m_stFce.modeGroupPendingList.pop_front();

            m_stFce.modeGroupIdleList.push_back(groupIndex);
            LOGD("(%s:L%d) NEW pop group index=%d \n", __func__, __LINE__, groupIndex);
        }
        else
        {
            int groupIndex = 0;
            int foundGroup = false;
            std::vector<ST_FCE_MODE_GROUP>::iterator it;


            groupIndex = m_stFce.modeGroupPendingList.back();
            it = m_stFce.stModeGroupAllList.begin();
            while (it != m_stFce.stModeGroupAllList.end())
            {
                if (it->groupID == groupIndex)
                {
                    foundGroup = true;
                    break;
                }
                ++it;
            }
            if (foundGroup)
            {
                if (it->modePendingList.size() > 1)
                {
                    LOGD("(%s:L%d) NEW pop mode=%d \n", __func__, __LINE__, it->modePendingList.front());
                    it->modePendingList.pop_front();
                }
            }
        }
    }

    return ret;
}

int IPU_ISPLUS::ClearFceModeGroup(ST_FCE_MODE_GROUP *pstModeGroup)
{
    if (NULL == pstModeGroup)
    {
        LOGE("(%s:L%d) param is NULL !!!\n", __func__, __LINE__);
        return -1;
    }
    if (!pstModeGroup->stModeAllList.empty())
    {
        int nbBuf = 0;
        int i = 0;
        std::vector<ST_FCE_MODE>::iterator modeIt;
        ST_GRIDDATA_BUF_INFO stBufInfo;


        LOGD("(%s:L%d) groupID=%d stMode size=%d \n", __func__, __LINE__,
                pstModeGroup->groupID, pstModeGroup->stModeAllList.size());
        modeIt = pstModeGroup->stModeAllList.begin();
        while (modeIt != pstModeGroup->stModeAllList.end())
        {
            nbBuf = modeIt->stBufList.size();
            LOGD("(%s:L%d) nbBuf=%d \n", __func__, __LINE__, nbBuf);
            for (i = 0; i < nbBuf; ++i)
            {
                stBufInfo = modeIt->stBufList.at(i);
                if (stBufInfo.stBufGrid.virt_addr)
                {
                    fr_free_single(stBufInfo.stBufGrid.virt_addr, stBufInfo.stBufGrid.phys_addr);
                    LOGD("(%s:L%d) free grid phys=0x%X \n", __func__, __LINE__, stBufInfo.stBufGrid.phys_addr);
                }
                if (stBufInfo.stBufGeo.virt_addr)
                {
                    fr_free_single(stBufInfo.stBufGeo.virt_addr, stBufInfo.stBufGeo.phys_addr);
                }
            }
            modeIt->stBufList.clear();
            modeIt->stPosDnList.clear();
            modeIt->stPosUoList.clear();
            modeIt->stPosSs0List.clear();
            modeIt->stPosSs1List.clear();

            pstModeGroup->stModeAllList.erase(modeIt);
        }
        pstModeGroup->modeActiveList.clear();
        pstModeGroup->modePendingList.clear();
    }

    return 0;
}

int IPU_ISPLUS::ParseFceFileParam(ST_FCE_MODE_GROUP *pstModeGroup, int groupID, cam_fcefile_param_t *pstParam)
{
    int ret = -1;
    cam_fcefile_mode_t *pstParamMode = NULL;
    cam_fcefile_file_t *pstParamFile = NULL;
    cam_fce_port_type_e portType = CAM_FCE_PORT_NONE;

    int nbMode = 0;
    int nbFile = 0;
    int nbPort = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    ST_FCE_MODE stMode;
    ST_GRIDDATA_BUF_INFO stBufInfo;
    cam_position_t stPos;
    struct stat stStatBuf;
    char szFrBufName[50];
    FILE *pFd = NULL;
    char *pszFileGrid = NULL;
    char *pszFileGeo = NULL;
    int nbNoPos = 0;


    if (NULL == pstModeGroup || NULL == pstParam)
    {
        LOGE("(%s:L%d) params %p %p are NULL !!!\n", __func__, __LINE__, pstModeGroup, pstParam);
        return -1;
    }
    pstModeGroup->groupID = groupID;
    pstModeGroup->stModeAllList.clear();
    pstModeGroup->modePendingList.clear();
    pstModeGroup->modeActiveList.clear();

    nbMode = pstParam->mode_number;
    if (0 == nbMode)
        goto toOut;
    LOGD("(%s:L%d) nbMode %d \n", __func__, __LINE__, nbMode);
    for (i = 0; i < nbMode; ++i)
    {
        nbNoPos = 0;
        stMode.modeID = i;
        stMode.stBufList.clear();
        stMode.stPosDnList.clear();
        stMode.stPosUoList.clear();
        stMode.stPosSs0List.clear();
        stMode.stPosSs1List.clear();

        pstParamMode = &pstParam->mode_list[i];
        nbFile = pstParamMode->file_number;
        LOGD("(%s:L%d) nbFile %d \n", __func__, __LINE__, nbFile);
        for (j = 0; j < nbFile; ++j)
        {
            memset(&stBufInfo, 0, sizeof(stBufInfo));
            pstParamFile = &pstParamMode->file_list[j];
            pszFileGrid = pstParamFile->file_grid;
            pFd = fopen(pszFileGrid, "rb");
            if (NULL == pFd)
            {
                LOGE("(%s:L%d) Failed to open %s !!!\n", __func__, __LINE__, pszFileGrid);
                goto openFail;
            }
            stat(pszFileGrid, &stStatBuf);
            sprintf(szFrBufName, "GDfile%dgrid%d_%d", groupID, i, j);
            stBufInfo.stBufGrid.virt_addr = fr_alloc_single(szFrBufName, stStatBuf.st_size, &stBufInfo.stBufGrid.phys_addr);
            if (!stBufInfo.stBufGrid.virt_addr)
            {
                LOGE("(%s:L%d) buf_name=%s \n", __func__, __LINE__, szFrBufName);
                goto allocFail;
            }
            stBufInfo.stBufGrid.size = stStatBuf.st_size;
            stBufInfo.stBufGrid.map_size = stStatBuf.st_size;
            fread(stBufInfo.stBufGrid.virt_addr, stStatBuf.st_size, 1, pFd);
            fclose(pFd);

            pszFileGeo = pstParamFile->file_geo;
            if (strlen(pszFileGeo) > 0)
            {
                pFd = fopen(pszFileGeo, "rb");
                if (NULL == pFd)
                {
                    LOGE("(%s:L%d) Failed to open %s !!!\n", __func__, __LINE__, pszFileGeo);
                    goto openFail;
                }
                stat(pszFileGeo, &stStatBuf);
                sprintf(szFrBufName, "GDfilegeo%d_%d_%d", groupID, i, j);
                stBufInfo.stBufGeo.virt_addr = fr_alloc_single(szFrBufName, stStatBuf.st_size, &stBufInfo.stBufGeo.phys_addr);
                if (!stBufInfo.stBufGeo.virt_addr)
                {
                    LOGE("(%s:L%d) buf_name=%s \n", __func__, __LINE__, szFrBufName);
                    goto allocFail;
                }
                stBufInfo.stBufGeo.size = stStatBuf.st_size;
                stBufInfo.stBufGeo.map_size = stStatBuf.st_size;
                fread(stBufInfo.stBufGeo.virt_addr, stStatBuf.st_size, 1, pFd);
                fclose(pFd);
            }
            nbPort = pstParamFile->outport_number;
            LOGD("(%s:L%d) nbPort=%d \n", __func__, __LINE__, nbPort);
            if (nbPort)
            {
                for (k = 0; k < nbPort; ++k)
                {
                    portType = pstParamFile->outport_list[k].type;
                    stPos = pstParamFile->outport_list[k].pos;
                    switch (portType)
                    {
                    case CAM_FCE_PORT_UO:
                        stMode.stPosUoList.push_back(stPos);
                        break;
                    case CAM_FCE_PORT_DN:
                        stMode.stPosDnList.push_back(stPos);
                        break;
                    case CAM_FCE_PORT_SS0:
                        stMode.stPosSs0List.push_back(stPos);
                        break;
                    case CAM_FCE_PORT_SS1:
                        stMode.stPosSs1List.push_back(stPos);
                        break;
                    default:
                        break;
                    }
                }
            }
            else
            {
                ++nbNoPos;
            }
            stMode.stBufList.push_back(stBufInfo);
        }
        if (nbNoPos == nbFile)
        {
            LOGD("(%s:L%d) nbNoPos %d \n", __func__, __LINE__, nbNoPos);
            if (m_pPortUo && m_pPortUo->IsEnabled())
            {
                ret = SetFceDefaultPos(m_pPortUo, nbFile, &stMode.stPosUoList);
                if (ret)
                {
                    LOGE("(%s:L%d) failed to set default pos(nbFile=%d) !\n", __func__, __LINE__, nbFile);
                    goto defaultPosFail;
                }
            }
            if (m_pPortDn && m_pPortDn->IsEnabled())
            {
                ret = SetFceDefaultPos(m_pPortDn, nbFile, &stMode.stPosDnList);
                if (ret)
                {
                    LOGE("(%s:L%d) failed to set default pos(nbFile=%d) !\n", __func__, __LINE__, nbFile);
                    goto defaultPosFail;
                }
            }
            if (m_pPortSs0 && m_pPortSs0->IsEnabled())
            {
                ret = SetFceDefaultPos(m_pPortSs0, nbFile, &stMode.stPosSs0List);
                if (ret)
                {
                    LOGE("(%s:L%d) failed to set default pos(nbFile=%d) !\n", __func__, __LINE__, nbFile);
                    goto defaultPosFail;
                }
            }
            if (m_pPortSs1 && m_pPortSs1->IsEnabled())
            {
                ret = SetFceDefaultPos(m_pPortSs1, nbFile, &stMode.stPosSs1List);
                if (ret)
                {
                    LOGE("(%s:L%d) failed to set default pos(nbFile=%d) !\n", __func__, __LINE__, nbFile);
                    goto defaultPosFail;
                }
            }
        }
        pstModeGroup->stModeAllList.push_back(stMode);
        if (i == 0)
        {
            pstModeGroup->modeActiveList.push_back(0);
        }
    }
    return 0;

defaultPosFail:
allocFail:
openFail:
    ClearFceModeGroup(pstModeGroup);
toOut:

    LOGE("(%s:L%d) ret=%d\n", __func__, __LINE__, ret);
    return ret;
}

void IPU_ISPLUS::FreeFce()
{
    if (!m_stFce.stModeGroupAllList.empty())
    {
        std::vector<ST_FCE_MODE_GROUP>::iterator it;

        it = m_stFce.stModeGroupAllList.begin();
        while(it != m_stFce.stModeGroupAllList.end())
        {
            ClearFceModeGroup(&(*it));
            it++;
        }
        m_stFce.stModeGroupAllList.clear();
        m_stFce.modeGroupIdleList.clear();
        m_stFce.modeGroupActiveList.clear();
        m_stFce.modeGroupPendingList.clear();
    }
}

void IPU_ISPLUS::PrintAllFce(ST_FCE *pstFce, const char *pszFileName, int line)
{
    int nbBuf = 0;
    int nbPort = 0;
    int i = 0;
    int j = 0;
    std::vector<ST_FCE_MODE_GROUP>::iterator it;
    std::vector<ST_FCE_MODE>::iterator modeIt;
    std::list<int>::iterator intIt;
    ST_GRIDDATA_BUF_INFO stBufInfo;
    int listSize = 0;
    cam_position_t stPos;


    listSize = pstFce->stModeGroupAllList.size();
    LOGE("(%s:L%d) listSize=%d all start---------\n", pszFileName, line, listSize);

    it = pstFce->stModeGroupAllList.begin();
    for (; it != pstFce->stModeGroupAllList.end(); ++it)
    {
        LOGE("groupID=%d Mode:%d all +++++++++\n", it->groupID, it->stModeAllList.size());
        modeIt = it->stModeAllList.begin();
        for(; modeIt != it->stModeAllList.end(); ++modeIt)
        {
            nbBuf = modeIt->stBufList.size();
            LOGE("modeID=%d file:%d =======\n", modeIt->modeID, nbBuf);
            for (i = 0; i < nbBuf; ++i)
            {
                stBufInfo = modeIt->stBufList.at(i);
                LOGE("grid phys=0x%X, geo phys=0x%X \n",
                                stBufInfo.stBufGrid.phys_addr, stBufInfo.stBufGeo.phys_addr);
            }
            nbPort = modeIt->stPosDnList.size();
            for (j = 0; j < nbPort; ++j)
            {
                stPos = modeIt->stPosDnList.at(j);
                LOGE("DN_%d (%d,%d,%d,%d) \n", j, stPos.x, stPos.y, stPos.width, stPos.height);
            }
            nbPort = modeIt->stPosUoList.size();
            for (j = 0; j < nbPort; ++j)
            {
                stPos = modeIt->stPosUoList.at(j);
                LOGE("UO_%d (%d,%d,%d,%d) \n", j, stPos.x, stPos.y, stPos.width, stPos.height);
            }
            nbPort = modeIt->stPosSs0List.size();
            for (j = 0; j < nbPort; ++j)
            {
                stPos = modeIt->stPosSs0List.at(j);
                LOGE("SS0_%d (%d,%d,%d,%d) \n", j, stPos.x, stPos.y, stPos.width, stPos.height);
            }
            nbPort = modeIt->stPosSs1List.size();
            for (j = 0; j < nbPort; ++j)
            {
                stPos = modeIt->stPosSs1List.at(j);
                LOGE("SS1_%d (%d,%d,%d,%d) \n", j, stPos.x, stPos.y, stPos.width, stPos.height);
            }
        }
        LOGE("group=%d modePending:%d  --------\n", it->groupID, it->modePendingList.size());
        intIt = it->modePendingList.begin();
        for(; intIt != it->modePendingList.end(); ++intIt)
        {
            LOGE("modeID=%d \n", *intIt);
        }
        LOGE("group=%d modeActive:%d  --------\n", it->groupID, it->modeActiveList.size());
        intIt = it->modeActiveList.begin();
        for(; intIt != it->modeActiveList.end(); ++intIt)
        {
            LOGE("modeID=%d \n", *intIt);
        }
    }
    LOGE("(%s:L%d) end----------\n", pszFileName, line);

    listSize = pstFce->modeGroupPendingList.size();
    LOGE("(%s:L%d) listSize=%d pending start---------\n", pszFileName, line, listSize);
    intIt = pstFce->modeGroupPendingList.begin();
    for (; intIt != pstFce->modeGroupPendingList.end(); ++intIt)
    {
        LOGE("groupID=%d \n", *intIt);
    }
    LOGE("(%s:L%d) end----------\n", pszFileName, line);

    listSize = pstFce->modeGroupIdleList.size();
    LOGE("(%s:L%d) listSize=%d idle start---------\n", pszFileName, line, listSize);
    intIt = pstFce->modeGroupIdleList.begin();
    for (; intIt != pstFce->modeGroupIdleList.end(); ++intIt)
    {
        LOGE("groupID=%d \n", *intIt);
    }
    LOGE("(%s:L%d) end----------\n", pszFileName, line);

    listSize = pstFce->modeGroupActiveList.size();
    LOGE("(%s:L%d) listSize=%d active start---------\n", pszFileName, line, listSize);
    intIt = pstFce->modeGroupActiveList.begin();
    for (; intIt != pstFce->modeGroupActiveList.end(); ++intIt)
    {
        LOGE("groupID=%d \n", *intIt);
    }
    LOGE("(%s:L%d) end----------\n", pszFileName, line);
}

int IPU_ISPLUS::SetFceDefaultPos(Port *pPort, int fileNumber, std::vector<cam_position_t> *pPosList)
{
    int ret = -1;
    cam_position_t stPos;
    int i = 0;
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;


    if (NULL == pPort || 0 == fileNumber || NULL == pPosList)
    {
        LOGE("(%s:L%d) param %p %d %p has error !\n", __func__, __LINE__, pPort, fileNumber, pPosList);
        return -1;
    }
    memset(&stPos, 0, sizeof(stPos));
    width = pPort->GetWidth();
    height = pPort->GetHeight();
    switch (fileNumber)
    {
    case 1:
        stPos.x = 0;
        stPos.y = 0;
        stPos.width = width;
        stPos.height = height;
        pPosList->push_back(stPos);
        ret = 0;
        break;
    case 2:
        height /= 2;
        stPos.width = width;
        stPos.height = height;
        for (i = 0; i < fileNumber; ++i, pPosList->push_back(stPos))
        {
            switch (i)
            {
            case 0:
                x = 0;
                y = 0;
                ret = 0;
                break;
            case 1:
                x = 0;
                y = height;
                break;
            default:
                break;
            }
            stPos.x = x;
            stPos.y = y;
        }
        ret = 0;
        break;
    case 3:
    case 4:
        width /= 2;
        height /= 2;
        stPos.width = width;
        stPos.height = height;
        for (i = 0; i < fileNumber; ++i, pPosList->push_back(stPos))
        {
            switch (i)
            {
            case 0:
                x = 0;
                y = 0;
                ret = 0;
                break;
            case 1:
                x = width;
                y = 0;
                break;
            case 2:
                x = 0;
                y = height;
                break;
            case 3:
                x = width;
                y = height;
                break;
            default:
                break;
            }
            stPos.x = x;
            stPos.y = y;
            LOGD("(%s:L%d) pos (%d,%d,%d,%d) \n", __func__, __LINE__, x, y, width, height);
        }
        ret = 0;
        break;
    default:
        ret = -1;
        LOGE("(%s:L%d) NOT support file number %d \n", __func__, __LINE__, fileNumber);
        break;
    }

    return ret;
}
