#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#include <thread>
#include <iostream>
#include <unistd.h>
#include <list>
//#include <linux-kernel/img_systypes.h>
#include <ispc/Pipeline.h>
#include <ispc/ParameterList.h>
#include <ispc/ParameterFileParser.h>
#include "V2505.h"

//class IPU_ISPC::IPU_V2500;
class Configuration
{
public:
    bool enableAE;
    bool enableMonoChrome;
    bool enflickerReject;
    bool enableAWB;
    bool enableTNM;
    bool enableDNS;
    bool enableLBC;
    bool enableLSH;
    bool getBrightness;
    bool enableWDR;
    bool enCapture;
    IMG_UINT8 iso;
    int targetBrightness;
    IMG_UINT8 light;
    IMG_UINT8 contrast;
    IMG_UINT8 saturation;
    IMG_UINT8 sharpness;
    IMG_UINT8 mirror;
    IMG_UINT8 wb;
    IMG_UINT8 senario;
    char *pszSensor;
    float fps = 0;
    //call function
    Configuration();
    int getParameters(int argc, char *argv[], IPU_V2500 &ipu_v2500);
    int parseParameter(IPU_V2500 &ipu_v2500);
    void releaseParameter();
    virtual ~Configuration();
};














#endif
