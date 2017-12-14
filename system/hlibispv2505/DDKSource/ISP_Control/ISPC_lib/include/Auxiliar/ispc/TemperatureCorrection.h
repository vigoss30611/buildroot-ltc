/**
*******************************************************************************
 @file TemperatureCorrection.h

 @brief ISPC::TemperatureCorrection declaration

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
#ifndef ISPC_AUX_TEMPERATURE_CORRECTION_H_
#define ISPC_AUX_TEMPERATURE_CORRECTION_H_

#ifdef WIN32
#define NOMINMAX
#endif
#include <cmath>
#include <limits>
#include <vector>
#include <algorithm>

#include "img_types.h"
#include "ispc/Matrix.h"
#include "ispc/ColorCorrection.h"
#include "ispc/ParameterList.h"
#include "ispc/Parameter.h"
#include "ispc/Module.h"
#include "ispc/ModuleAWS.h"
#include "ispc/LineSegments.h"

namespace ISPC {

/**
 * @brief Provides functionality to calculate correlated colour temperatures
 * and provided interpolated colour correction transforms corresponding to a
 * requested temperature.
 *
 * The collection of possible corrections can be defined by defaults or loaded
 * from a ParameterList.
 * There is functionality for automatic interpolation of corrections if a
 * requested temperature doesn't exactly match one of the stored transforms.
 * @see ColorCorrection
 */
class TemperatureCorrection
{
protected:
    /** @brief store the loaded corrections */
    std::vector<ColorCorrection> temperatureCorrections;

    /**
     * @brief the line segments generated from calibration points
     */
    LineSegments lines;

public:
    TemperatureCorrection();

    /**
     * @brief Add a new color correction to the collection
     * @param newCorrection New colour correction to be added to the existing
     * collection
     */
    void addCorrection(const ColorCorrection &newCorrection);

    /** @brief Access to the number of loaded temperature corrections */
    int size() const;

    /** @brief Clear the stored collection of colour corrections */
    void clearCorrections();

    /**
     * @brief Add a set of colour correction from a ParameterList object
     * @warning does not remove previously loaded corrections
     */
    IMG_RESULT loadParameters(const ParameterList &parameters);
#ifdef INFOTM_AUTO_CHANGE_CCM
	IMG_RESULT updateCCM(double ccm_factor, double gain_factor);
#endif

    /** @brief Save the current correction list to a ParameterList */
    IMG_RESULT saveParameters(ParameterList &parameters,
        ModuleBase::SaveType t) const;

    /**
     * @brief Returns a correction corresponding to the requested temperature
     * If no exact match is available the returned correction is an
     * interpolation of 2 existing corrections.
     *
     * @param temperature Illuminant temperature for which we are requesting
     * a colour correction
     */
    ColorCorrection getColorCorrection(double temperature) const;

    /**
     * @brief Returns the index of the corresponding correction if an exact
     * match is available in the loaded values.
     *
     * @return index if exact match for temperature is found (can be used to
     * retrieve value with getTempCorrection())
     *
     * @return negative value otherwise
     */
    int getCorrectionIndex(double temperature) const;

    /**
     * @brief Access a copy of the loaded temperature correction
     *
     * @return new ColorCorrection() if i > the loaded list
     */
    ColorCorrection getCorrection(unsigned int i) const;

#if defined(INFOTM_ISP)
    /**
     * @brief Access a copy of the loaded temperature correction
     *
     * @return new ColorCorrection() if i > the loaded list
     */
    ColorCorrection getCorrection(unsigned int i);

    /**
     * @brief Updated the loaded temperature correction
     */
    IMG_RESULT setCorrection(unsigned int i, ColorCorrection cc);

#endif //#if defined(INFOTM_ISP)
    /**
     * @brief Returns the correlated colour temperature of a RGB value given in
     * sRGB color space
     *
     * @param r Red value
     * @param g Gree value
     * @param b Blue value
     *
     * @return temperature Correlated colour temperature of the received RGB
     */
    double getCorrelatedTemperature_sRGB(double r, double g, double b) const;

    /**
     * @brief Returns the correlated colour temperature of a RGB value
     * in sensor calibrated color space
     *
     * @param r Red value
     * @param g Gree value
     * @param b Blue value
     *
     * @return temperature Correlated colour temperature of the received RGB
     */
    double getCorrelatedTemperature_Calibrated(double r, double g, double b) const;

    /**
     * @brief Returns the correlated colour temperature of a RGB value
     * using McCamy approximation
     *
     * @param r Red value
     * @param g Gree value
     * @param b Blue value
     * @param gammaCorrection if true then gamma correction is applied
     * prior to temperature estimation, default is false
     *
     * @return temperature Correlated colour temperature of the received RGB
     */
    double getCorrelatedTemperature_McCamy(double r, double g, double b,
            bool gammaCorrection = false) const;

    /**
     * @brief Returns the correlated colour temperature of a RGB value
     *
     * @param r Red value
     * @param g Gree value
     * @param b Blue value
     *
     * @return temperature Correlated colour temperature of the received RGB
     */
    double getCorrelatedTemperature(double r, double g, double b) const;

    const LineSegments& getLines(void) const { return lines; }
	
#if defined(INFOTM_ISP) && defined(INFOTM_AWB_PATCH_GET_CCT)
		double getCCT(double R, double G, double B) const;
#endif

public:  // parameters
#ifdef INFOTM_SUPPORT_CCM_FACTOR
    double origial_coefficients[6][3][3];
    double origial_gains[6][4];
    double origial_offset[6][3];
#endif	
    static const ParamDef<int> WB_CORRECTIONS;
    static const ParamDef<double> WB_TEMPERATURE_S;  // 1 per correction
    static const ParamDefArray<double> WB_CCM_S;  // 1 per correction
    static const ParamDefArray<double> WB_OFFSETS_S;  // 1 per correction
    static const ParamDefArray<double> WB_GAINS_S;  // 1 per correction

    /** @brief Get the group of parameters used by that class */
    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_AUX_TEMPERATURE_CORRECTION_H_ */
