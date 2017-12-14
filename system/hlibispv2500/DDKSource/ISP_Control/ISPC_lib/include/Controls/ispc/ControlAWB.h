/**
*******************************************************************************
 @file ControlAWB.h

 @brief Automatic White Balance control class. Deriuver from ControlModule
 class to implement generic control modules

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
#ifndef ISPC_CONTROL_AWB_H_
#define ISPC_CONTROL_AWB_H_

#include <string>

#include "ispc/Module.h"
#include "ispc/TemperatureCorrection.h"
#include "ispc/Parameter.h"

namespace ISPC {

/**
 * @brief This value is used for normalizing the average pixel R,G,B colors
 * prior to 'ungamma' correction so it has to correspond with the image
 * white level
 *
 * @ Set this value automatically according to the pipeline bitdepth would
 * be better
 */
#define PIPELINE_WHITE_LEVEL 512

/**
 * @brief Max temperature in Kelvin for the AWB algorithm
 * - it is an arbitrary choice
 */
#define AWB_MAX_TEMP 100000.0f

/**
 * @ingroup ISPC2_CONTROLMODULES
 * @brief This class implements the automatic white balance control module
 *
 * Uses the ModuleHIS statistics to update the ModuleCCM and ModuleWBC.
 */
class ControlAWB: public ControlModuleBase<CTRL_AWB>
{
public:
    /**
     * @brief Represents a state of the algorithm for white balance
     * statistics threshold calculation
     */
    enum TS_State {
        TS_RESET,
        TS_SCANNING,
        TS_FOUND
    };

    /**
     * @brief Structure to store the status of one of the WB threshold
     * controllers
     */
    struct Threshold
    {
        /**
         * @brief Current state in the search for a proper WB statistics
         * threshold
         */
        TS_State currentState;
        /** @brief Accumulated error with the previously applied thresholds */
        double accumulatedError;
        /** @brief Whats the threshold to be applied in the next capture */
        double nextThreshold;
        /** @brief Tolerance for the calculation of the threshold */
        double errorMargin;
        /** @brief How many frames we have been scanning for */
        double scanningFrames;
    };

    /** @brief Different types of WB correction modes */
    enum  Correction_Types {
        WB_NONE = 0,
        WB_AC,  /**< @brief Average colour */
        WB_WP,  /**< @brief White Patch */
        WB_HLW,  /**< @brief High Luminance White */
        WB_COMBINED, /**< @brief Combined AC + WP + HLW */
    };

protected:  // attributes
    /**
     * @brief Total amount of pixels in the image used for calculate
     * some statistics
     */
    unsigned int imageTotalPixels;
    /**
     * @brief Auxiliary class to get correlated colour temperatures and
     * interpolated colour correction matrices for given color temperatures
     */
    TemperatureCorrection colorTempCorrection;
    /** @brief The color correction matrix applied currently. */
    ColorCorrection currentCCM;
    /**
     * @brief Inverse of the color correction matrix being applied currently.
     */
    ColorCorrection inverseCCM;
    /** @brief Luma threshold for the HLW and WP statistics */
    double HLWThreshold;
    /** @brief Red threshold for the HLW and WP statistics */
    double WPThresholdR;
    /** @brief Green threshold for the HLW and WP statistics */
    double WPThresholdG;
    /** @brief Blue threshold for the HLW and WP statistics */
    double WPThresholdB;
    /**
     * @brief Keep the state of the dynamic threshold calculation for
     * statistics
     */
    Threshold thresholdInfoR;
    /** @copydoc thresholdInfoR */
    Threshold thresholdInfoG;
    /** @copydoc thresholdInfoR */
    Threshold thresholdInfoB;
    /** @copydoc thresholdInfoR */
    Threshold thresholdInfoY;
    /** @brief Scaling factor for the temperature estimation */
    double estimationScale;
    /** @brief Offset for the temperature estimation */
    double estimationOffset;

    Correction_Types correctionMode;
    double targetTemperature;
    double targetPixelRatio;

    /**
     * @brief Flag to indicate if we have already programmed the statistics
     * regions
     */
    bool statisticsRegionProgrammed;

    double measuredTemperature;
    double correctionTemperature;
    bool doAwb;
    bool configured;

public:  // methods
    static const char * CorrectionName(Correction_Types e);

    /**
     * @brief Initializes colour correction to 6500K temperature and
     * statistics thresholds to default values.
     */
    explicit ControlAWB(const std::string &logtag = "ISPC_CTRL_AWB");

    /**
    * @brief Virtual destructor
    */
    virtual ~ControlAWB() {}

    /**
     * @brief Retreive the additional information about the image size
     * from the sensor object
     */
    virtual void setPipelineOwner(Pipeline *pipeline);

    /**
     * see @ref ControlModule::load()
     */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /**
     * See @ref ControlModule::save()
     */
    virtual IMG_RESULT save(ParameterList &parameters,
        ModuleBase::SaveType t) const;

    /**
     * @copydoc ControlModule::update()
     *
     * This function calculates the required colour correctionbased on
     * the capture white balance statistics and programs the pipeline
     * accordingly.
     *
     * In this function several actions are performed: The WB statistics
     * contained in the captured shot metadata are processed and the
     * statistics thresholds updated accordingly.
     * The gathered statistics are processed try to invert the colour
     * processing carried out in different modules in the pipeline.
     * The purpose is to try to eliminate the effect of the colour
     * correction module, which is applied before the extraction of the WB
     * statistics.
     * Once that is done the scene illuminant temperature is estimated and
     * a color correction matrix is interpolated to compensate for that
     * particular color temperature.
     * Both the WB statistics modules as well as the CCM and white
     * balance gains are reprogrammed during the call to this function.
     */
#ifdef INFOTM_ISP
    virtual IMG_RESULT update(const Metadata &metadata, const Metadata &metadata2);
#else
    virtual IMG_RESULT update(const Metadata &metadata);
#endif //INFOTM_DUAL_SENSOR


    /**
     * @brief Set the target colour temperature for the image (how we want
     * the image to look like after WB correction)
     * @param targetTemperature Target color temperature (Kelvin)
     */
    virtual void setTargetTemperature(double targetTemperature);

    /**
     * @brief Get the target colour temperature
     * @return Applied target temperature (Kelvin)
     */
    virtual double getTargetTemperature() const;

    /**
     * @brief Set correction modes
     *
     * Sets one of the white balance correction modes between the following
     * modes:
     * @li WB_NONE (no WB correction),
     * @li WB_AC (correction based on average color),
     * @li WB_WP (correction based in white patch algorithm),
     * @li WB_HLW (correction based on high luminance white algorithm),
     * @li WB_COMBINED (a combination of the previous modes)
     *
     * @param correctionMode The correction mode to apply
     *
     * See @ref Correction_Types
     */
    void setCorrectionMode(Correction_Types correctionMode);

    /**
     * @brief Get the applied correction mode
     * @see setCorrectionMode
     * @see Correction_Types
     * @return The white balance correction mode that it is being applied
     */
    Correction_Types getCorrectionMode() const;

    const TemperatureCorrection& getTemperatureCorrections() const;

#if defined(INFOTM_ISP)
    /**
     * @brief getter for internal TemperatureCorrection container
     */
    TemperatureCorrection& getTemperatureCorrections();

#endif //#if defined(INFOTM_ISP)
    virtual double getMeasuredTemperature() const;

    virtual double getCorrectionTemperature() const;

protected:
    /**
     * @copydoc ControlModule::configureStatistics()
     *
     * Program the WB statistics extraction modules of the pipeline with
     * whatever settings and threshold we have calculated
     */
    virtual IMG_RESULT configureStatistics();

    /**
     * @copydoc ControlModule::programCorrection()
     *
     * Program the desired CCM coefficients, gains and offsets in the
     * pipeline to achieve the desired WB correction
     * @see currentCCM
     */
    virtual IMG_RESULT programCorrection();

public:
    /**
     * @name Auxiliary functions
     * @brief Contains the algorithms knowledge
     * @{
     */

    /**
     * @brief Estimate the threshold required for one of the WB statistics
     * so a given ratio of pixels is counted in the statistics
     *
     * @param thresholdInfo Structure containing the state of the threshold
     * search
     * @param meteredRatio The ratio of pixels counted in the statistics with
     * the previously applied threshold.
     * @param targetRatio The desired ratio of pixels we would like to be
     * counted in the statistics.
     *
     * @return The updated threshold to be applied in the next capture
     *
     * Some of the WB statistics (WP and HLW) rely on calculating average
     * colours of certain range of pixels value. In both cases it is required
     * to target a certain fixed percentage of pixels with the brighter
     * values. In order to do so only the pixels above certain thresholds are
     * considered in the statistics. However the distribution of pixel
     * values vary according to the characteristics of the captured image and
     * therefore the thresholds must vary. This function calculates updates
     * thresholds given a target ratio of pixels to be counted, previous
     * settings and the ratio of pixels obtained for the previous thresholds.
     * It is implemented in a similar ways as a pid controller, trying
     * to maximize the speed to achieve the proper ratio but avoiding
     * oscillations
     */
    static double estimateThreshold(Threshold &thresholdInfo,
        double meteredRatio, double targetRatio);

    /**
     * @brief Processing of the White Patch (WP) obtained statistics and
     * update the thresholds applied in the WP statistics module for the
     * next capture.
     *
     * @param[in] metadata use WBS info
     * @param imagePixels total number of pixels in the image
     * @param targetPixelRatio
     * @param RInfo red threshold info
     * @param GInfo green threshold info
     * @param BInfo green threshold info
     * @param[out] RThreshold
     * @param[out] GThreshold
     * @param[out] BThreshold
     *
     * @see Delegates to estimateThreshold()
     */
    static void estimateWPThresholds(const Metadata &metadata,
        double imagePixels, double targetPixelRatio,
        Threshold &RInfo, Threshold &GInfo, Threshold &BInfo,
        double &RThreshold, double &GThreshold, double &BThreshold);

    /**
     * @brief Processing of the High Luminance White (HLW) statistics and
     * update the threshold applied in the HLW statistics module for the
     * next capture
     */
    static double estimateHLWThreshold(const Metadata &metadata,
        double imagePixels, double targetPixelRatio, Threshold &YInfo);

    /**
     * @brief Calculates R,G,B averages for the High Luminance White
     * (HLW) statistics
     *
     * The HLW estatistics are obtained for all the pixels in the image
     * with a luminance value about a predefined threshold.
     * This function calculates the averages for each channel from the
     * statistics obtained from the pipeline
     *
     * @param[in] metadata The metadata from the previously captured shot
     * @param imagePixels total number of pixels in the image
     * @param[out] redHLW Calculated average red value
     * @param[out] greenHLW Calculated average green value
     * @param[out] blueHLW Calculated average blue value
     *
     * See @ref estimateThreshold
     */
    static void getHLWAverages(const Metadata &metadata, double imagePixels,
        double &redHLW, double &greenHLW, double &blueHLW);

    /**
     * @brief Calculates R,G,B average values for the White Patch (WP)
     * statistics
     *
     * The WP estatistics are obtained from the pixels from every channel that
     * are above a predefined threshold.
     * There is a different threshold per channel.
     *
     * @param[in] metadata The metadata from the previosuly captured shot
     * @param imagePixels total number of pixels in the image
     * @param[out] redWP Calculated average red value
     * @param[out] greenWP Calculated average green value
     * @param[out] blueWP Calculated average blue value
     * @see estimateThreshold
     */
    static void getWPAverages(const Metadata &metadata, double imagePixels,
        double &redWP, double &greenWP, double &blueWP);

    /**
     * @brief Calculate average values per channel of the image pixels
     *
     * The AC average are just the averages per channel of all the pixels in
     * the image. This is used for the gray-world
     * white balance correction algorithm.
     *
     * @param[in] metadata The metadata from the previously captured shot.
     * @param imagePixels total number of pixels in the image
     * @param[out] redAvg Calculated average red value
     * @param[out] greenAvg Calculated average green value
     * @param[out] blueAvg Calculated average blue value
     * @see estimateThreshold
     */
    static void getACAverages(const Metadata &metadata, double imagePixels,
        double &redAvg, double &greenAvg, double &blueAvg);

    /**
    * @brief Applies a color correction matrix coefficients to a R,G,B value
    *
    * @param R input red value
    * @param G input green value
    * @param B input blue value
    * @param[in] correction A color correction matrix, gains and offsets.
    * In this function only the gains are used
    * @param[out] targetR The resulting red value
    * @param[out] targetG The resulting green value
    * @param[out] targetB The resulting blue value
    */
    static void colorTransform(double R, double G, double B,
        const ColorCorrection &correction,
        double &targetR, double &targetG, double &targetB);

    /**
     * @brief Applies and inverted color correction coefficients, gains
     * and offsets
     *
     * @param R input red value
     * @param G input green value
     * @param B input blue value
     * @param[in] invCCM A color correction matrix, gains and offsets.
     * Corresponds to an inverted correction so offsets and gains are
     * applied in reverse order than in the pipeline
     * @param[out] iR red value resulting of the inverted correction
     * @param[out] iG green value resulting of the inverted correction
     * @param[out] iB blue value resulting of the inverted correction
     *
     * See @ref ColorCorrection
     */
    static void invertColorCorrection(double R, double G, double B,
        const ColorCorrection &invCCM, double &iR, double &iG, double &iB);

    /**
     * @brief Corrects the estimated illuminant temperature using the set up
     * offset and scale (t = (t+off)*scale)
     *
     * For fine calibration of the WB illuminant estimation algorithm it
     * is possible to offset and scale the estimated temperature
     * for cases in which the estimation is biased (or if for whatever
     * reason we want to bias it).
     *
     * @param temperature Estimated temperature
     * @param offset offset of the temperature estimation
     * @param scale scale of the temperature estimation
     */
    static double correctTemperature(double temperature, double offset,
        double scale);

    /**
     * @brief Same as correctTemperature() but does t = (t*scale)+off
     */
    static double correctTemperature2(double temperature, double offset,
        double scale);

    /**
     * @}
     */

public:  // parameters
    static const ParamDef<float> AWB_ESTIMATION_SCALE;
    static const ParamDef<float> AWB_ESTIMATION_OFFSET;
    static const ParamDef<float> AWB_TARGET_TEMPERATURE;
    static const ParamDef<float> AWB_TARGET_PIXELRATIO;

    /** @brief Get the group of parameters used by that class */
    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_CONTROL_AWB_H_ */
