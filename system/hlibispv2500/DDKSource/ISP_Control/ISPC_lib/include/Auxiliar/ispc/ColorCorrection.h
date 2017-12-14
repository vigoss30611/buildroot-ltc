/**
*******************************************************************************
 @file ColorCorrection.h

 @brief ISPC::ColorCorrection declaration

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
#ifndef ISPC_AUX_COLOR_CORRECTION_H_
#define ISPC_AUX_COLOR_CORRECTION_H_

#include <iostream>

#include "ispc/Matrix.h"

namespace ISPC {
class ColorCorrection;  // forward declare for operator<<
}

std::ostream& operator<<(std::ostream &oStream,
    const ISPC::ColorCorrection &correction);

namespace ISPC {

/**
 * @brief Encapsulates a pipeline color correction information and provides
 * additional functions and operators for easy colour correction
 * interpolation.
 *
 * That is the color correction matrix coefficients, offsets and channel
 * gains. 
 * It overrides the + and * operators and provides blending functions to be
 * able to operate and interpolate couples of ColorCorrection instances.
 */
class ColorCorrection
{
public:
    /** @brief Color correction matrix coefficients */
    Matrix coefficients;
    /** @brief Color correction offsets */
    Matrix offsets;
    /** @brief Channel gains for WB correction */
    Matrix gains;
    /** @brief Illuminant temperature this correction correspond to */
    double temperature;

    ColorCorrection();

    /**
     * @brief Create a ColorCorrection object from three double arrays of
     * matrix coefficients, offsets and gains, as well as the corresponding
     * illuminant temperature
     * @param pCoeffs color correction matrix coefficients
     * @param pOffsets color correction offsets
     * @param pGains WB gains
     * @param temperature correlated temperature corresponding to the
     * created color correction
     */
    ColorCorrection(const double *pCoeffs, const double *pOffsets,
        const double *pGains, double temperature);

    /**
     * @brief Addition operation to be able to add up two color corrections
     * @param otherCorrection The second ColorCorrection operand
     */
    ColorCorrection operator+(ColorCorrection otherCorrection) const;

    /**
     * @brief Multiplication by a scalar
     * @param value scalar value to multiply the color correction
     */
    ColorCorrection operator*(double value) const;

    /**
     * @brief Comparison operator to enable sorting algorithms to operate
     * with ColorCorrection objects
     * @param otherCorrection ColorCorrection to compare with
     */
    bool operator>(const ColorCorrection &otherCorrection) const;

    /**
     * @brief Comparison operator to enable sorting algorithms to operate
     * with ColorCorrection objects
     * @param otherCorrection ColorCorrection to compare with
     */
    bool operator<(const ColorCorrection &otherCorrection) const;

    /**
     * @brief Method for blending two color corrections with a given
     * weight (blending)
     * @param otherCorrection ColorCorrection object to blend with
     * @param blending blending factor, between 0 and 1 to control
     * the mixture/blending of the color corrections
     */
    ColorCorrection blend(ColorCorrection &otherCorrection,
        double blending) const;

    /**
     * @brief Convert the color correction to its inverse
     */
    void inv();
};

} /* namespace ISPC */

#endif /* ISPC_AUX_COLOR_CORRECTION_H_ */
