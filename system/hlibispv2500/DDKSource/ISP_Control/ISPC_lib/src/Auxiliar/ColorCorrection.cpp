/**
*******************************************************************************
 @file ColorCorrection.cpp

 @brief ISPC::ColorCorrection implementation

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
#include "ispc/ColorCorrection.h"

std::ostream& operator<< (std::ostream &oStream,
    const ISPC::ColorCorrection &correction)
{
    oStream << "Temp:   \t" << correction.temperature << std::endl;
    oStream << "Coeffs: \t" << correction.coefficients[0][0] << "\t"
        << correction.coefficients[0][1] << "\t"
        << correction.coefficients[0][2] << std::endl;
    oStream << "        \t" << correction.coefficients[1][0] << "\t"
        << correction.coefficients[1][1] << "\t"
        << correction.coefficients[1][2] << std::endl;
    oStream << "        \t" << correction.coefficients[2][0] << "\t"
        << correction.coefficients[2][1] << "\t"
        << correction.coefficients[2][2] << std::endl;

    oStream << "Offsets:\t" << correction.offsets[0][0] << "\t"
        << correction.offsets[0][1] << "\t"
        << correction.offsets[0][2] << std::endl;
    oStream << "Gains:  \t" << correction.gains[0][0] << "\t"
        << correction.gains[0][1] << "\t"
        << correction.gains[0][2] << "\t"
        << correction.gains[0][3] << std::endl;

    return oStream;
}

ISPC::ColorCorrection::ColorCorrection()
    : coefficients(3, 3), offsets(1, 3), gains(1, 4), temperature(6500.0)
{
}

ISPC::ColorCorrection::ColorCorrection(const double *pCoeffs,
    const double *pOffsets, const double *pGains, double temperature)
    : coefficients(3, 3), offsets(1, 3), gains(1, 4)
{
    coefficients = Matrix(3, 3, pCoeffs);
    offsets = Matrix(1, 3, pOffsets);
    gains = Matrix(1, 4, pGains);
    this->temperature = temperature;
}

ISPC::ColorCorrection ISPC::ColorCorrection::operator+(
    ColorCorrection otherCorrection) const
{
    ColorCorrection resultCorrection;
    resultCorrection.coefficients = coefficients
        + otherCorrection.coefficients;
    resultCorrection.offsets = offsets + otherCorrection.offsets;
    resultCorrection.gains = gains+ otherCorrection.gains;
    resultCorrection.temperature = temperature + otherCorrection.temperature;

    return resultCorrection;
}

ISPC::ColorCorrection ISPC::ColorCorrection::operator*(double value) const
{
    ColorCorrection resultCorrection;
    resultCorrection.coefficients = coefficients*value;
    resultCorrection.offsets = offsets*value;
    resultCorrection.gains = gains*value;
    resultCorrection.temperature = temperature*value;

    return resultCorrection;
}

bool ISPC::ColorCorrection::operator>(
    const ISPC::ColorCorrection &otherCorrection) const
{
    return (temperature > otherCorrection.temperature);
}

bool ISPC::ColorCorrection::operator<(
    const ColorCorrection &otherCorrection) const
{
    return (temperature < otherCorrection.temperature);
}

ISPC::ColorCorrection ISPC::ColorCorrection::blend(
    ISPC::ColorCorrection &otherCorrection, double blending) const
{
    ColorCorrection resultCorrection;

    resultCorrection.coefficients = (coefficients*(1.0-blending))
        + (otherCorrection.coefficients*blending);
    resultCorrection.offsets = (offsets*(1.0-blending))
        + (otherCorrection.offsets*blending);
    resultCorrection.gains = (gains*(1.0-blending))
        + (otherCorrection.gains*blending);
    resultCorrection.temperature = (temperature*(1.0-blending))
        + (otherCorrection.temperature*blending);

    return resultCorrection;
}

void ISPC::ColorCorrection::inv()
{
    coefficients = coefficients.inv();
    offsets = offsets*(-1.0);
    gains[0][0] = 1.0/gains[0][0];
    gains[0][1] = 1.0/gains[0][1];
    gains[0][2] = 1.0/gains[0][2];
    gains[0][3] = 1.0/gains[0][3];
}
