#include <System.h>
#include "Configuration.h"
#include <dyncmd/commandline.h>
#include <qsdk/videobox.h>


Configuration::Configuration()
{
    this->enableAE = false;
    this->enableAWB = false;
    this->enableDNS = false;
    this->enableTNM = false;
    this->enableLSH = false;
    this->enableLBC = false;
    this->enflickerReject = false;
    this->enableMonoChrome = false;
    this->pszSensor = NULL;
    this->getBrightness = false;
    this->enableWDR = false;
    this->mirror = 0;
    this->fps = 0;
    //127 as 0.0
    this->light = 127;
    this->targetBrightness = 127;
    this->saturation = 127;
    this->contrast = 127;
    this->sharpness = 127;
    this->iso = 127;
    this->wb = 0;
    this->senario = 0;

}

int Configuration::getParameters(int argc, char *argv[], IPU_V2500 &ipu_v2500)
{
    IMG_RESULT ret;
    enum cam_mirror_state dir;
    ret = DYNCMD_AddCommandLine(argc, argv, "");
    if (IMG_SUCCESS != ret)
    {
        LOGE("Add command line failed\n");
        return -1;
    }

    ret = DYNCMD_RegisterParameter("-mir", DYNCMDTYPE_UINT, "set picture mirror direction.", &this->mirror);
    if (IMG_SUCCESS != ret)
    {
        LOGE("parsing mirror failed\n");
        //return -1;
    }
    else
    {
        ipu_v2500.SetMirror((enum cam_mirror_state)this->mirror);
        ipu_v2500.GetMirror(dir);
        LOGE("Get current mirror status:%d\n", dir);
    }

    ret = DYNCMD_RegisterParameter("-iso", DYNCMDTYPE_UINT, "set camera iso.", &this->iso);
    if (IMG_SUCCESS != ret)
    {
        LOGE("parsing iso failed\n");
        //return -1;
    }
    else
    {
        ipu_v2500.SetCameraISO(this->iso);
    }

    ret = DYNCMD_RegisterParameter("-sn", DYNCMDTYPE_UINT, "set camera senario.", &this->senario);
    if (IMG_SUCCESS != ret)
    {
        LOGE("parsing senario failed\n");
        //return -1;
    }
    else
    {
        ipu_v2500.SetScenario(this->senario);
    }

    ret = DYNCMD_RegisterParameter("-tbr", DYNCMDTYPE_UINT, "set AE target brightness.", &this->targetBrightness);
    if (IMG_SUCCESS != ret)
    {
        LOGE("parsing AE target brightness failed\n");
        //return -1;
    }
    else
    {
        ipu_v2500.SetExposureLevel(this->targetBrightness);
    }

    ret = DYNCMD_RegisterParameter("-sbr", DYNCMDTYPE_UINT, "set LBC brightness.", &this->light);
    if (IMG_SUCCESS != ret)
    {
        LOGE("parsing LBC  brightness failed\n");
        //return -1;
    }
    else
    {
        ipu_v2500.SetYUVBrightness(this->light);
    }

    ret = DYNCMD_RegisterParameter("-fps", DYNCMDTYPE_FLOAT, "set FPS down ration.", &this->fps);
    if (IMG_SUCCESS != ret)
    {
        LOGE("parsing FPS down failed\n");
        //return -1;
    }
    else
    {
        int fps = 0;
        LOGE("fps down ration :%f\n", this->fps);
        ipu_v2500.SetFPS(this->fps);
        ipu_v2500.GetFPS(fps);
        LOGE("current fps =%d=========\n", fps);
    }

    ret = DYNCMD_RegisterParameter("-sctr", DYNCMDTYPE_UINT, "set LBC contrast.", &this->contrast);
    if (IMG_SUCCESS != ret)
    {
        LOGE("parsing LBC  brightness failed\n");
        //return -1;
    }
    else
    {
        ipu_v2500.SetContrast(this->contrast);
    }

    ret = DYNCMD_RegisterParameter("-sat", DYNCMDTYPE_UINT, "set LBC saturation.", &this->saturation);
    if (IMG_SUCCESS != ret)
    {
        LOGE("parsing LBC  saturation failed\n");
        //return -1;
    }
    else
    {
        ipu_v2500.SetSaturation(this->saturation);
    }

    ret = DYNCMD_RegisterParameter("-wb", DYNCMDTYPE_UINT, "set wb mode.", &this->wb);
    if (IMG_SUCCESS != ret)
    {
        LOGE("parsing wb mode failed\n");
        //return -1;
    }
    else
    {
        ipu_v2500.SetWB(this->wb);
    }

    ret = DYNCMD_RegisterParameter("-shap", DYNCMDTYPE_UINT, "set LBC sharpness.", &this->sharpness);
    if (IMG_SUCCESS != ret)
    {
        LOGE("parsing LBC  sharpness failed\n");
        //return -1;
    }
    else
    {
        ipu_v2500.SetSharpness(this->sharpness);
    }

    ret = DYNCMD_RegisterParameter("-enAE", DYNCMDTYPE_BOOL8, "enable AE.", &this->enableAE);
    if (IMG_SUCCESS != ret)
    {
        LOGE("enable AE failed\n");
        //return -1;
    }
    else
    {
        LOGE("configration: enableAE:%d\n", this->enableAE);
        ipu_v2500.EnableAE(this->enableAE);
    }

    ret = DYNCMD_RegisterParameter("-enMono", DYNCMDTYPE_BOOL8, "enable monochrome.", &this->enableMonoChrome);
    if (IMG_SUCCESS != ret)
    {
        LOGE("enable monochrome failed\n");
        //return -1;
    }
    else
    {
        LOGE("configration: enablemonochrome:%d\n", this->enableMonoChrome);
        ipu_v2500.EnableMonochrome(this->enableMonoChrome);
    }

    ret = DYNCMD_RegisterParameter("-enWDR", DYNCMDTYPE_BOOL8, "enable wdr.", &this->enableWDR);
    if (IMG_SUCCESS != ret)
    {
        LOGE("enable WDR failed\n");
        //return -1;
    }
    else
    {
        LOGE("configration: enableWDR:%d\n", this->enableWDR);
        ipu_v2500.EnableWDR(this->enableWDR);
    }

    ret = DYNCMD_RegisterParameter("-gb", DYNCMDTYPE_COMMAND, "git brightness", NULL);
    if (RET_FOUND== ret)
    {
        int brt;
        this->getBrightness = true;
        //return -1;
        if (this->getBrightness)
        {
            ipu_v2500.GetYUVBrightness(brt);
            LOGE("Get the brightness: %d\n", brt);
        }
    }

    ret = DYNCMD_RegisterParameter("-enCap", DYNCMDTYPE_BOOL8, "enable wdr.", &this->enCapture);
    if (IMG_SUCCESS != ret)
    {
        LOGE("enable capture failed\n");
        //return -1;
    }
    else
    {
        LOGE("configration: enableWDR:%d\n", this->enCapture);
        ipu_v2500.runSt = this->enCapture;
    }

    return IMG_SUCCESS;
}

int Configuration::parseParameter(IPU_V2500 &ipu_v2500)
{
    LOGE("configration: enableAE:%d\n", this->enableAE);
    ipu_v2500.EnableAE(this->enableAE);
    if (this->enableAE)
    {
        ipu_v2500.SetExposureLevel(this->targetBrightness);
    }

    if (this->getBrightness)
    {
        int brt;
        ipu_v2500.GetYUVBrightness(brt);
        LOGE("Get the brightness: %d\n", brt);
    }

    if (!this->enableAE)
    ipu_v2500.SetYUVBrightness(this->light);
    return IMG_SUCCESS;
}

void Configuration::releaseParameter()
{
    DYNCMD_ReleaseParameters();
}

Configuration:: ~Configuration()
{
    //TODO
}
