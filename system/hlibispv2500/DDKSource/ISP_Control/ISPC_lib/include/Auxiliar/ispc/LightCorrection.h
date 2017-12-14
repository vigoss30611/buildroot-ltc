/**
*******************************************************************************
 @file LightCorrection.h

 @brief ISPC::LightCorrection declaration
 
 Used for to store corrections for the LBCControl object

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
#ifndef ISPC_AUX_LIGHT_CORRECTION_H_
#define ISPC_AUX_LIGHT_CORRECTION_H_

namespace ISPC {

/**
 * @brief LightCorrection is an auxiliary class employed to store a given 
 * configuration of brightness, saturation, contrast and sharpness and to 
 * provide some methods and operators to easy the interpolation between 
 * different configurations for the ControlLBC class.
 *
 * Each configuration is associated with a given light level which is an 
 * estimation of the amount of light captured by the sensor.
 */
class LightCorrection
{
public:
    /** @brief Sharpness strength value */
    double sharpness;
    /** @brief Colour saturation value */
    double saturation;
    /** @brief Image brightness value */
    double brightness;
    /** @brief Image contrast value */
    double contrast;

    /** @brief Light level corresponding to this configuration */
    double lightLevel;

public:  // methods
    /** @brief Constructor with default values */
    LightCorrection();

    /**
     * @brief Addition override to being able to add up the values of two
     * LightCorrection instances
     */
    LightCorrection operator+(const LightCorrection &otherConfig) const;

    /**
     * @brief Multiply override to being able to multiply the values of a
     * LightCorrection by a scalar
     */
    LightCorrection operator*(double value) const;

    /**
     * @brief Method to interpolate current LightCorrection with another
     * LightCorrection instance.
     *
     * @param otherConfig another instance of LightCorrection to blend with
     * @param blendRatio Value between 0.0 and 1.0 indicating the blending
     * between the current and the otherConfig LightCorrection instances.
     * blending = 1.0 will return otherConfig values
     *
     * @return Interpolated LightCorrection
     */
    LightCorrection blend(const LightCorrection &otherConfig,
        double blendRatio);

    /**
     * @brief Comparison based on the light level to allow sorting of
     * instances in collections.
     * 
     * @param otherConfig other LightCorrection instance to compare with.
     * @return true if the light level associated with the current 
     * LightCorrection instance is greater than the light level of the 
     * otherConfig instance
     */
    bool operator>(const LightCorrection &otherConfig) const;

    /**
     * @brief Comparison based on the light level to allow sorting of
     * LightCorrection instances in collections. 
     *
     * @param otherConfig other LightCorrection instance to compare with.
     * @return true if the light level associated with the current
     * LightCorrection instance is smaller than the light level of the
     * otherConfig instance
     */
    bool operator<(const LightCorrection &otherConfig) const;
};

} /* namespace ISPC */

#endif /* ISPC_AUX_LIGHT_CORRECTION_H_ */
