/**
*******************************************************************************
 @file ControlAWB_Planckian.h

 @brief Automatic White Balance control class using Planckian Locus method.
 Applicable since HW 2.6, where AWS statristics block is available
 Derives from ControlModule class to implement generic control modules

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
#ifndef ISPC_CONTROL_AWB_PLANCKIAN_H_
#define ISPC_CONTROL_AWB_PLANCKIAN_H_

#include <string>
#include <deque>

#include "ispc/Module.h"
#include "ispc/TemperatureCorrection.h"
#include "ispc/Parameter.h"
#include "ispc/ControlAWB.h"

#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

namespace ISPC
{

typedef struct rbNode_tag {
    double rg;
    double bg;
} rbNode_t;

typedef struct _HW_AWB_STAT
{
    double fGR;
    double fGB;
    unsigned int uiQualCnt;
} HW_AWB_STAT, *PHW_AWB_STAT;


/**
 * @ingroup ISPC2_CONTROLMODULES
 * @brief This class implements the automatic white balance control module
 *
 * Uses the ModuleHIS statistics to update the ModuleCCM and ModuleWBC.
 */
class ControlAWB_Planckian: public ControlAWB
{
    // WEIGHT_BASE_xxx cannot be 1 as this causes division by zero.
#define WEIGHT_BASE_MIN (1.001f)
#define WEIGHT_BASE_MAX (10.00f)
public:
    static const int DEFAULT_WEIGHT_BASE;
    static const int DEFAULT_TEMPORAL_STRETCH;
    static const int TEMPORAL_STRETCH_MIN;
    static const int TEMPORAL_STRETCH_MAX;

//    /** @brief Different WB temporal smoothing modes */
//    enum Smoothing_Modes {
//        WBTS_NONE = 0,
//        /**< @brief Moving Average */
//        WBTS_AVG  = 1,
//        /**< @brief Moving Average Weighted.
//         * Promoting Newest with increasing "base" parameter */
//        WBTS_AWE  = 2,
//        WBTS_MAX  = WBTS_AWE
//        /**< @brief Previous to current.
//         * No historical values. Only previous. */
////        WBTS_PTC  = 3,
//    };
//
    /** @brief Additional WB temporal smoothing features */
    enum  Smoothing_Features {
        WBTS_FEATURES_NONE  = 0,
        WBTS_FEATURES_FF    = 0x01,   /**< @brief Flash Filtering */
        // note: if adding new feature modify set/get functions too.
//        WBTS_FEATURES_SNAP  = 0x02,   /**< @brief snap if close? tbd */
    };

protected:  // attributes
    struct AwbTile : public MC_TILE_STATS_AWS
    {
        static const double RATIO_DEF;
        double ratioR;     /** @brief cached storage of ratio R */
        double ratioB;     /** @brief cached storage of ratio B */
        bool ratioRValid;  /** @brief true if cached ratioR valid */
        bool ratioBValid;  /** @brief true if cached ratioB valid */

        /**
         * @brief log2(quantum efficiency red)
         */
        double log2_QEr;

        /**
         * @brief log2(quantum efficiency blue)
         */
        double log2_QEb;

        AwbTile() : MC_TILE_STATS_AWS(),
                ratioR(RATIO_DEF),
                ratioB(RATIO_DEF),
                ratioRValid(false),
                ratioBValid(false),
                log2_QEr(RATIO_DEF),
                log2_QEb(RATIO_DEF)
        {}

        explicit AwbTile(const MC_TILE_STATS_AWS& aws) :
                MC_TILE_STATS_AWS(aws),
                ratioR(RATIO_DEF),
                ratioB(RATIO_DEF),
                ratioRValid(false),
                ratioBValid(false),
                log2_QEr(RATIO_DEF),
                log2_QEb(RATIO_DEF)
        {}

        /**
         * @brief operator used to copy straight from MC stats structure
         */
        AwbTile& operator=(const MC_TILE_STATS_AWS& rhs)
        {
            if (this == &rhs) {  // check for self assignment
                return *this;
            }
            fCollectedBlue = rhs.fCollectedBlue;
            fCollectedGreen = rhs.fCollectedGreen;
            fCollectedRed = rhs.fCollectedRed;
            numCFAs = rhs.numCFAs;
            ratioRValid = false;
            ratioBValid = false;
            return *this;
        }

        AwbTile& operator=(const  AwbTile& rhs)
        {
            if (this == &rhs)
            {  // check for self assignment
                return *this;
            }
            log2_QEr = rhs.log2_QEr;
            log2_QEb = rhs.log2_QEb;
            return operator=(static_cast<const MC_TILE_STATS_AWS& > (rhs));
        }

        /**
         * @brief operator used to accumulate stats straight from
         * MC statistics
         */
        AwbTile& operator+=(const MC_TILE_STATS_AWS& rhs)
        {
            fCollectedRed += rhs.fCollectedRed;
            fCollectedGreen += rhs.fCollectedGreen;
            fCollectedBlue += rhs.fCollectedBlue;
            numCFAs += rhs.numCFAs;
            ratioRValid = false;
            ratioBValid = false;
            return *this;
        }

        AwbTile& operator+=(const AwbTile& rhs)
        {
            return operator+=(static_cast<const MC_TILE_STATS_AWS& > (rhs));
        }

        /**
         * @brief set quantum efficiences for correct calculation of
         * statistics
         *
         * @param _log2_QEr log2() of red QEff
         *
         * @param _log2_QEb log2() of blue QEff
         */
        void setQeffs(double _log2_QEr, double _log2_QEb)
        {
            log2_QEr = _log2_QEr;
            log2_QEb = _log2_QEb;
        }

        /**
         * @brief apply lower end bounding box on the tile
         *
         * @return true if inside bounding box, false otherwise
         */
        bool applyBounds(
            const double lowLog2RatioR,
            const double lowLog2RatioB);

        /**
         * @brief returns true if tile contains meaningful data
         */
        inline bool isValid(void) const { return numCFAs > 0; }

        /**
         * @brief returns log2(R/G)
         */
        double getLog2RatioR(void) const
        {
            if (!isValid())
            {
                return 0;
            }

            return (fCollectedRed +
                     numCFAs*log2_QEr -
                     fCollectedGreen)/numCFAs;
        }

        /**
         * @brief returns log2(B/G)
         */
        double getLog2RatioB(void) const
        {
            if (!isValid())
            {
                return 0;
            }

            return (fCollectedBlue +
                     numCFAs*log2_QEb -
                     fCollectedGreen)/numCFAs;
        }

        /**
         * @brief returns ratio R
         * @note caches calculated ratio to conserve float operations
         */
        double getRatioR(void)
        {
            if (!isValid())
            {
                return RATIO_DEF;
            }
            if (!ratioRValid)
            {
                // store cached value
#ifdef USE_MATH_NEON
                ratioR = powf_neon((float)2.0, (float)getLog2RatioR());
#else
                ratioR = std::pow(2.0, getLog2RatioR());
#endif
                ratioRValid = true;
            }
            return ratioR;
        }

        /**
         * @brief returns ratio B
         * @note caches calculated ratio to conserve float operations
         */
        double getRatioB(void)
        {
            if (!isValid())
            {
                return RATIO_DEF;
            }
            if (!ratioBValid)
            {
                // store cached value
#ifdef USE_MATH_NEON
                ratioB = powf_neon((float)2.0, (float)getLog2RatioB());
#else
                ratioB = std::pow(2.0, getLog2RatioB());
#endif
                ratioBValid = true;
            }
            return ratioB;
        }

        /**
         * @brief return true if current tile is closer than radius to centroid
         */
        bool isCloseToCentroid(
            const double centroidR,
            const double centroidB,
            const double radiusPow2) const
       {
            // assume it's calculated prior to calling this method
            IMG_ASSERT(ratioBValid && ratioBValid);
            // true if inside circle of radius sqrt(radiusPow2)
            return ((ratioR-centroidR)*(ratioR-centroidR) +
                    (ratioB-centroidB)*(ratioB-centroidB)) < radiusPow2;
        }
    };

    static const int STATS_TILES_HORIZ = AWS_NUM_GRID_TILES_HORIZ;
    static const int STATS_TILES_VERT = AWS_NUM_GRID_TILES_VERT;

    typedef AwbTile statsTilesArray_t[STATS_TILES_VERT][STATS_TILES_HORIZ];

    /**
     * @brief array of AwbTile objects
     */
    statsTilesArray_t awbTiles;

    /**
     * @brief ratioR for current capture
     */
    double fRatioR;

    /**
     * @brief ratioB for current capture
     */
    double fRatioB;

    /**
     * @brief contains log2() of red quantum efficiency
     */
    double fLog2_R_Qeff;

    /**
     * @brief contains log2() of blue quantum efficiency
     */
    double fLog2_B_Qeff;

    /**
     * @brief local storage of red quantum efficiency
     *
     * @note may be loaded from sensor calibration data or overridden
     * using AWS_LOG2_R_QEFF when AWB_USE_AWS_CONFIG is 1
     */
    double f_R_Qeff;

    /**
     * @brief local storage of blue quantum efficiency
     *
     * @note may be loaded from sensor calibration data or overridden
     * using AWS_LOG2_B_QEFF when AWB_USE_AWS_CONFIG is 1
     *
     */
    double f_B_Qeff;

    /**
     * @brief low temperature bounding box R for ratios used in stat analysis
     */
    double fLog2LowTempRatioR;

    /**
     * @brief low temperature bounding box B for ratios used in stat analysis
     */
    double fLog2LowTempRatioB;

    /**
     * @brief radius around centroid above which tiles are filtered out
     * in final ratio calculations
     */
    double fMaxRatioDistance;

    /**
     * @brief cached storage of pow(2) of fMaxRatioDistance
     */
    double fMaxRatioDistancePow2;

    /**
     * @brief if true then AWS_* configuration fields are used to override
     * sensor calibration
     */
    bool useCustomAwsConfig;

    /**
     * @brief if true the frames suspected to be flashed are filtered out
     * from White Balance Temporal Smoothing
     */
    bool mFlashFilteringEnabled;

    /**
     * @brief use temporal smoothing algorithm
     */
    bool mUseTemporalSmoothing;

    /**
     * @brief temporal stretch, time to settle awb
     */
    int mTemporalStretch;

public:  // methods
    /**
     * @brief Initializes colour correction to 6500K temperature and
     * statistics thresholds to default values.
     */
    explicit ControlAWB_Planckian(
            const std::string &logtag = "ISPC_CTRL_AWB");

    /**
    * @brief Virtual destructor
    */
    virtual ~ControlAWB_Planckian() {}

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
     */
    virtual IMG_RESULT update(const Metadata &metadata);

    /**
     * @warning this algorithm does not allow to know if convergence is
     * reached
     */
    virtual bool hasConverged() const;

    /**
     * @brief Set/Get:
     *          type of White Balance Temporal Smoothing
     *          base for weights generation (1 - 10) float
     *          temporal stretch            (200 - 5000)[ms] int
     */
    virtual void setWBTSAlgorithm(bool smoothingEnabled,
            float base=DEFAULT_WEIGHT_BASE,
            int temporalStretch = DEFAULT_TEMPORAL_STRETCH);
    void getWBTSAlgorithm(
            bool &, float &, int &);

    /**
     * @brief Set/Get features of White Balance Temporal Smoothing e.g. flash filtering
     */
    virtual void setWBTSFeatures(unsigned int);
    void getWBTSFeatures(unsigned int &);

    /**
     * @brief Set/Get flash filtering
     */
    void setFlashFiltering(const bool);
    void getFlashFiltering(bool &enabled);

    /**
     * @brief get set base for weight calculations
     */
    void setWeightParam(float);
    float getWeightParam(void);
    static float getMinWeightBase(void);
    static float getMaxWeightBase(void);


    /**
     * @brief Generate weight of index element out of nOfElem total weights
     */
    float weightSpread(const int nOfElem, const int index);

	int ts_getTemporalStretch() const;
	void ts_setTemporalStretch(const int);
	float ts_getWeightBase() const;
	void ts_setWeightBase(const float);
	bool ts_getSmoothing() const;
	void ts_setSmoothing(const bool);
	bool ts_getFlashFiltering() const;

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

    /**
     * @brief processes MC stats, calculates R B ratios for each tile
     *
     * @note stores results in ControlAWB_Planckian::awbTiles
     *
     */
    void processStatistics(const MC_STATS_AWS& awsStats);

    /**
     * @brief modify centroid position if multiple strong tile 'clouds'
     * can be found on R/B plane
     *
     * @note parameters are in/out
     */
    bool postprocessCentroid(
            double& fCentroidRatioR, double& fCentroidRatioB);

public:
    /** @brief detects if current settings suggest flash condition */
    virtual bool potentialFlash(const rbNode_t);

    /** @brief clear historic data e.g. when required to apply
     * WB immediately */
    void temporalAwbClear(void);

    /** @brief apply smoothing algorithm */
    void temporalAWBsmoothing(double&, double&);

    /** @brief flash filtering algorithm */
    void flashFiltering(const double, const double);

private:
    /** @brief return time over which the wb is smoothed (in milliseconds)
     *  if calculated temperature changes from stable T1 to stable T2
     *  it is the time from first occurrence of T2 before T2 is
     *  fully applied.
     *  E.g. for FPS = 2 and stretch = 2000, the calculated sequence
     *  T1 T1 T1 T1 T2 T2 T2 T2 T2 will be applied as
     *  T1 T1 T1 T1 I1 I2 I3 I4 T2
     *  where Ix are intermediate smoothed values T2 > Ix > T1 (if T2>T1)
     *  */
    virtual int getTemporalStretch();
    virtual void setTemporalStretch(int);

    /** @brief return current frames per second */
    virtual double getFps();

    /** @brief tbd */
    //void temporalAwbSnapIfClose(void);

    /** @brief generate weights for weighted moving average algorithm */
    void generateWeights(const int);

    /** @brief check if flash filtering is to be applied */
    bool flashFilteringDisabled(void);

    /** @brief don't do WB Temporal Smoothing, pass values */
    void smoothingNone(double &, double &);

    /** @brief moving average temporal smoothing algorithm */
    //void movingAverage(double &, double &);

    /** @brief moving average temporal smoothing algorithm \
     * values are weighted with emphasis on newest */
    void movingAverageWeighted(double &, double &);

    /** @brief base for weight generation */
    float mWeightsBase;

    /** @brief weights for weighted moving average algorithm */
    std::deque<float> rbWeights;

public:  // parameters
    /** @brief collected rg and bg ratios */
    std::deque<rbNode_t> rbNodes;

    /** @brief cached ratios to be filtered out (if flash condition)
     * or put back to rbNodes (scene change condition) */
    std::deque<rbNode_t> rbNodesCache;

    static const ParamDefSingle<bool>  AWB_USE_AWS_CONFIG;
    static const ParamDef<double> AWB_MAX_RATIO_DISTANCE;

    /** @brief Get the group of parameters used by that class */
    static ParameterGroup getGroup();

    static const ParamDefSingle<bool> WBT_USE_FLASH_FILTERING;
    static const ParamDefSingle<bool> WBT_USE_SMOOTHING;
    static const ParamDef<int> WBTS_TEMPORAL_STRETCH;
    static const ParamDef<float> WBTS_WEIGHT_BASE;

#if defined(INFOTM_HW_AWB_METHOD)
    //planckian no use the parameter
    bool bHwAwbEnable;
    unsigned int ui32HwAwbMethod;
    unsigned int ui32HwAwbFirstPixel;
    HW_AWB_STAT stHwAwbStat[16][16];
#endif //INFOTM_HW_AWB_METHOD

    virtual std::ostream& printState(std::ostream &os) const;
#ifdef INFOTM_ISP
    void enableAWB(bool enable);
    bool IsAWBenabled() const;
    void enableControl(bool enable);
#endif //INFOTM_ISP
};

} /* namespace ISPC */

#endif /* ISPC_CONTROL_AWB_H_ */
