/**
*******************************************************************************
 @file LightCorrection.cpp

 @brief ISPC::LightCorrection implementation

 @copyright Imagination Technologies Ltd. All Rights Reserved. 

 @license Strictly Confidential. 
   No part of this software, either material or conceptual may be copied or 
   distributed, transmitted, transcribed, stored in a retrieval system or  
   translated into any human or computer language in any form by any means, 
   electronic, mechanical, manual or other-wise, or disclosed to third  
   parties without the express written permission of  
   Imagination Technologies Limited,  
   Unit 8, HomePark Industrial Estate, 
   King's Langley, Hertfordshire, 
   WD4 8LZ, U.K.

******************************************************************************/
#include "ispc/LightCorrection.h"

#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_LIGHTCORRECTION"

#include "ispc/ControlLBC.h"
#include "ispc/ModuleSHA.h"
#include "ispc/ModuleR2Y.h"

ISPC::LightCorrection::LightCorrection()
    :sharpness(SHA_STRENGTH_DEF),
    saturation(R2Y_SATURATION_DEF),
    brightness(R2Y_BRIGHTNESS_DEF),
    contrast(R2Y_CONTRAST_DEF),
    lightLevel(LBC_LIGHT_DEF)
{
}

ISPC::LightCorrection ISPC::LightCorrection::operator+(
    const LightCorrection &otherConfig) const
{
    LightCorrection result;

    result.sharpness = sharpness + otherConfig.sharpness;
    result.saturation = saturation + otherConfig.saturation;
    result.brightness = brightness + otherConfig.brightness;
    result.contrast = contrast + otherConfig.contrast;
    result.lightLevel = lightLevel + otherConfig.lightLevel;

    return result;
}

ISPC::LightCorrection ISPC::LightCorrection::operator*(double value) const
{
    LightCorrection result;

    result.sharpness = sharpness*value;
    result.saturation = saturation*value;
    result.brightness = brightness*value;
    result.contrast = contrast*value;
    result.lightLevel = lightLevel*value;

    return result;
}

ISPC::LightCorrection ISPC::LightCorrection::blend(
    const LightCorrection &otherConfig, double blendRatio)
{
    LightCorrection result;

    if (blendRatio < 0.0 || blendRatio > 1.0)
    {
        LOG_ERROR("Blend ratio value must be between 0.0 and 1.0 "\
            "(received %f)\n", blendRatio);
        return result;
    }

    result = (*this)*(1.0 - blendRatio) + otherConfig * blendRatio;
    return result;
}

bool ISPC::LightCorrection::operator>(const LightCorrection &otherConfig) const
{
    return lightLevel > otherConfig.lightLevel;
}

bool ISPC::LightCorrection::operator<(const LightCorrection &otherConfig) const
{
    return lightLevel < otherConfig.lightLevel;
}
