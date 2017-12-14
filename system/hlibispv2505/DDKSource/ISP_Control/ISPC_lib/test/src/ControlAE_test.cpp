/**
*******************************************************************************
 @file ControlAE_test.cpp

 @brief Test the AE control class

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
#include <ispc/ControlAE.h>
#include <ispc/Sensor.h>
#include <ispc/Pipeline.h>
#include <gtest/gtest.h>

#include <string>
#include <list>
#include <memory>

#undef VERBOSE

#ifndef VERBOSE
#undef printf
#define printf(...)
#endif /* VERBOSE */

/* STATS-021 from Atomos.Eagle.Requirements */
static const int maxConvergeFrames = 20;

typedef ISPC::ControlAE::microseconds_t microseconds_t;

class TestSensor : public ISPC::Sensor {
public:
    static const double MIN_GAIN;
    static const double MAX_GAIN;
    static const uint32_t MIN_EXP;
    static const uint32_t MAX_EXP;

    TestSensor() : ISPC::Sensor(0)
    {
        maxExposure = MAX_EXP;
        minExposure = MIN_EXP;
        maxGain = MAX_GAIN;
        minGain = MIN_GAIN;
    }
};

class TestPipeline : public ISPC::Pipeline {
public:
    explicit TestPipeline(TestSensor* sensor) :
        ISPC::Pipeline(NULL, 0, NULL) {
        setSensor(sensor);
    }
};

const double TestSensor::MIN_GAIN = 1.0;
const double TestSensor::MAX_GAIN = 12.0;
const uint32_t TestSensor::MIN_EXP = 100;
const uint32_t TestSensor::MAX_EXP = 1234;

/**
 * @note this test fixture enables us to access
 * protected members and methods of ControlAE
 */
class ControlAE : public ISPC::ControlAE, public testing::Test
{
public:
    // backward compatible but deprecated since C++11
    std::auto_ptr<TestSensor> testSensor;
    std::auto_ptr<TestPipeline> testPipeline;

    void setupHarness(void) {
        testSensor = std::auto_ptr<TestSensor>(new TestSensor());
        testPipeline = std::auto_ptr<TestPipeline>(
                new TestPipeline(testSensor.get()));
        addPipeline(testPipeline.get());
        setPipelineOwner(testPipeline.get());
    }

    void testAeControl(
            const microseconds_t minExposure,
            const microseconds_t maxExposure,
            const double minGain,
            const double maxGain) {

        // loop variables
        microseconds_t sensorExposure = minExposure;
        double sensorGain = minGain;

        double newGain;
        microseconds_t newExposure;

        ASSERT_FALSE(isUnderexposed());

        printf("targetExposure;sensorExposure;sensorGain;underexposed\n");

        do {
            getAutoExposure(
                sensorExposure*sensorGain,
                minExposure,
                maxExposure,
                minGain,
                maxGain,
                newGain,
                newExposure);

            printf("%f;%d;%f;%d\n",
                    sensorExposure*sensorGain,
                    newExposure,
                    newGain,
                    isUnderexposed());

            ASSERT_LE(newExposure, maxExposure);
            ASSERT_GE(newExposure, minExposure);
            ASSERT_LE(newGain, maxGain);
            ASSERT_GE(newGain, minGain);
            ASSERT_GE(newGain*newExposure,
                    sensorGain*sensorExposure);
            ASSERT_GE(newExposure, sensorExposure);

            sensorExposure = newExposure;
            sensorGain = newGain;

        } while (newGain < maxGain);
    }

    void testAeControl_fixed_gain(
            const double gain,

            const microseconds_t minExposure,
            const microseconds_t maxExposure,
            const double minGain,
            const double maxGain) {

        const double maxBrightness = 1.0;
        const double minBrightness = -1.0;

        const microseconds_t expStep = (minExposure+maxExposure)/100;

        currentBrightness = minBrightness;

        printf("currentBrightness;targetExposure;sensorExposure;sensorGain;underexposed\n");

        enableFixedAeExposure(false);
        enableFixedAeGain(true);
        setFixedAeGain(gain);

        const double maxTargetExposure = maxGain*maxExposure;

        do {
            double prevTargetExposure = minGain*minExposure;
            // start from min exposure
            double sensorExposure=minExposure;
            microseconds_t newExposure;
            double sensorGain=0.0, newGain;

            while (prevTargetExposure<maxTargetExposure)
            {
                getAutoExposure(
                    prevTargetExposure,
                    minExposure,
                    maxExposure,
                    minGain,
                    maxGain,
                    newGain,
                    newExposure);

                printf("%f;%f;%f;%d;%f;%d\n",
                        currentBrightness,
                        prevTargetExposure,
                        newExposure*newGain,
                        newExposure,
                        newGain,
                        isUnderexposed());

                ASSERT_LE(newExposure, maxExposure);
                ASSERT_GE(newExposure, minExposure);
                // we move from minExposure up to maxExposure
                ASSERT_GE(newExposure, sensorExposure);
                ASSERT_LE(newGain, getMaxAeGain());
                ASSERT_GE(newGain, minGain);
                if (gain > minGain && gain < getMaxAeGain()) {
                    ASSERT_EQ(newGain, gain);
                }

                if (prevTargetExposure<=gain*maxExposure) {
                    // target exp is topped to  gain*maxExposure
                    if (currentBrightness<0.0) {
                        ASSERT_GE(newGain*newExposure, prevTargetExposure);
                    } else {
                        ASSERT_LE(newGain*newExposure, prevTargetExposure);
                    }
                }
                // prepare to next step
                sensorExposure = newExposure;
                sensorGain = newGain;

                prevTargetExposure += expStep;
            }

            currentBrightness += 0.1;
        } while (currentBrightness < 1.0);
    }

    void testAeControl_fixed(
            const double gain,
            const microseconds_t exposure,

            const microseconds_t minExposure,
            const microseconds_t maxExposure,
            const double minGain,
            const double maxGain) {

        const double maxBrightness = 1.0;
        const double minBrightness = -1.0;

        currentBrightness = minBrightness;

        printf("currentBrightness;targetExposure;sensorExposure;sensorGain;underexposed\n");

        enableFixedAeExposure(true);
        enableFixedAeGain(true);

        setFixedAeExposure(exposure);
        setFixedAeGain(gain);

        double sensorExposure=0.0;
        microseconds_t newExposure;
        double sensorGain=0.0, newGain;

        do {
            getAutoExposure(
                sensorExposure*sensorGain,
                minExposure,
                maxExposure,
                minGain,
                maxGain,
                newGain,
                newExposure);

            printf("%f;%f;%d;%f;%d\n",
                    currentBrightness,
                    sensorExposure*sensorGain,
                    newExposure,
                    newGain,
                    isUnderexposed());

            ASSERT_LE(newExposure, maxExposure);
            ASSERT_GE(newExposure, minExposure);
            ASSERT_LE(newGain, maxGain);
            ASSERT_GE(newGain, minGain);
            ASSERT_EQ(newGain*newExposure,
                    gain*exposure);

            sensorExposure = newExposure;
            sensorGain = newGain;

            currentBrightness += 0.05;

        } while (currentBrightness < 1.0);
    }

};

/**
 * @brief test working of  setMaxAeGain()/getMaxAeGain
 */
TEST_F(ControlAE, getset_maxAeGain)
{
    setupHarness();
    ASSERT_EQ(getSensor()->getMinGain(), TestSensor::MIN_GAIN);
    ASSERT_EQ(getSensor()->getMaxGain(), TestSensor::MAX_GAIN);

    for (double gain: {
        TestSensor::MIN_GAIN-1.0,
        TestSensor::MIN_GAIN,
        (TestSensor::MIN_GAIN+TestSensor::MAX_GAIN)*0.5,
        TestSensor::MAX_GAIN,
        TestSensor::MAX_GAIN+1.0 }) {

        setMaxAeGain(gain);
        // check clipping
        ASSERT_GE(getMaxAeGain(), getSensor()->getMinGain());
        ASSERT_LE(getMaxAeGain(), getSensor()->getMaxGain());
        if (gain >=getSensor()->getMinGain() &&
            gain <=getSensor()->getMaxGain()) {
            ASSERT_EQ(getMaxAeGain(), gain);
        }
    }
}

/**
 * @brief test working of  setTargetAeGain()/getTargetAeGain
 */
TEST_F(ControlAE, getset_targetAeGain)
{
    setupHarness();
    ASSERT_EQ(getSensor()->getMinGain(), TestSensor::MIN_GAIN);
    ASSERT_EQ(getSensor()->getMaxGain(), TestSensor::MAX_GAIN);

    for (double gain: {
        TestSensor::MIN_GAIN-1.0,
        TestSensor::MIN_GAIN,
        (TestSensor::MIN_GAIN+TestSensor::MAX_GAIN)*0.5,
        TestSensor::MAX_GAIN,
        TestSensor::MAX_GAIN+1.0 }) {

        setTargetAeGain(gain);
        // check clipping
        ASSERT_GE(getTargetAeGain(), getSensor()->getMinGain());
        ASSERT_LE(getTargetAeGain(), getSensor()->getMaxGain());
        if (gain >=getSensor()->getMinGain() &&
            gain <=getSensor()->getMaxGain()) {
            ASSERT_EQ(getTargetAeGain(), gain);
        }
    }
}

/**
 * @brief test working of  setTargetAeGain()/getTargetAeGain
 */
TEST_F(ControlAE, getset_fixedAeGain)
{
    setupHarness();
    ASSERT_EQ(getSensor()->getMinGain(), TestSensor::MIN_GAIN);
    ASSERT_EQ(getSensor()->getMaxGain(), TestSensor::MAX_GAIN);

    for (double gain: {
        TestSensor::MIN_GAIN-1.0,
        TestSensor::MIN_GAIN,
        (TestSensor::MIN_GAIN+TestSensor::MAX_GAIN)*0.5,
        TestSensor::MAX_GAIN,
        TestSensor::MAX_GAIN+1.0 }) {

        setFixedAeGain(gain);
        // check clipping
        ASSERT_GE(getFixedAeGain(), getSensor()->getMinGain());
        ASSERT_LE(getFixedAeGain(), getSensor()->getMaxGain());
        if (gain >=getSensor()->getMinGain() &&
            gain <=getSensor()->getMaxGain()) {
            ASSERT_EQ(getFixedAeGain(), gain);
        }
    }
}

/**
 * @brief test working of  setMaxAeExposure()/getMaxAeExposure
 */
TEST_F(ControlAE, getset_maxAeExposure)
{
    setupHarness();
    ASSERT_EQ(getSensor()->getMinExposure(), TestSensor::MIN_EXP);
    ASSERT_EQ(getSensor()->getMaxExposure(), TestSensor::MAX_EXP);

    for (microseconds_t exp: {
            (microseconds_t)0,
            (microseconds_t)TestSensor::MIN_EXP,
            (microseconds_t)(TestSensor::MIN_EXP+TestSensor::MAX_EXP)/2,
            (microseconds_t)TestSensor::MAX_EXP,
            (microseconds_t)TestSensor::MAX_EXP+100 }) {

        setMaxAeExposure(exp);
        // check clipping
        ASSERT_GE(getMaxAeExposure(), getSensor()->getMinExposure());
        ASSERT_LE(getMaxAeExposure(), getSensor()->getMaxExposure());
        if (exp >=getSensor()->getMinExposure() &&
            exp <=getSensor()->getMaxExposure()) {
            ASSERT_EQ(getMaxAeExposure(), exp);
        }
    }
}

/**
 * @brief test working of  setTargetAeExposure()/getTargetAeExposure
 */
TEST_F(ControlAE, getset_fixedAeExposure)
{
    setupHarness();
    ASSERT_EQ(getSensor()->getMinExposure(), TestSensor::MIN_EXP);
    ASSERT_EQ(getSensor()->getMaxExposure(), TestSensor::MAX_EXP);

    for (microseconds_t exp: {
            (microseconds_t)0,
            (microseconds_t)TestSensor::MIN_EXP,
            (microseconds_t)(TestSensor::MIN_EXP+TestSensor::MAX_EXP)/2,
            (microseconds_t)TestSensor::MAX_EXP,
            (microseconds_t)TestSensor::MAX_EXP+100 }) {

        setFixedAeExposure(exp);
        // check clipping
        ASSERT_GE(getFixedAeExposure(), getSensor()->getMinExposure());
        ASSERT_LE(getFixedAeExposure(), getSensor()->getMaxExposure());
        if (exp >=getSensor()->getMinExposure() &&
            exp <=getSensor()->getMaxExposure()) {
            ASSERT_EQ(getFixedAeExposure(), exp);
        }
    }
}

/**
 * @brief test AE control without taking any constraints into account
 * @note the possible constraints are :
 * - exposure min step, flicker freq. and limits
 * - gain limits
 */
TEST_F(ControlAE, autoExposureCtrl)
{
    const microseconds_t minExposure = 10;
    const microseconds_t maxExposure = 10000;

    double targetBracket = 0.5;
    const double brightnessStep = 10*targetBracket;

    // loop variables
    int iteration;
    double targetBrightness;
    double currentBrightness;
    double targetExposure = 0.0;
    double prevExposure;

    // handy function
    auto hasConverged = [] (
        double targetBrightness,
        double currentBrightness,
        double targetBracket) {
            return fabs(targetBrightness - currentBrightness) < targetBracket;
        };

    /*
     * going from exposure min to max
     */
    targetBrightness = 0.0;
    // the problem is lack of statistics model
    currentBrightness = targetBrightness-0.5; // always less than desired
    iteration = 0;
    // used as prevExposure
    prevExposure = minExposure;

    while (targetExposure < maxExposure) {
        targetExposure = autoExposureCtrl(
                currentBrightness,
                targetBrightness,
                prevExposure);
        printf("step=%d cb=%f te=%f\n", iteration, currentBrightness, targetExposure);
        ASSERT_GE(static_cast<microseconds_t>(targetExposure),
                minExposure);
        ASSERT_LT(iteration, maxConvergeFrames);
        ASSERT_LE(prevExposure, targetExposure); // check if monotonic
        prevExposure = targetExposure;
        ++iteration;
    }

    /*
     * going from exposure max to min
     */
    targetBrightness = 0.0;
    currentBrightness = targetBrightness+0.5;
    iteration = 0;
    // used as prevExposure
    prevExposure = maxExposure;

    while (targetExposure > minExposure) {
        targetExposure = autoExposureCtrl(
                currentBrightness,
                targetBrightness,
                prevExposure);

        printf("step=%d cb=%f te=%f\n", iteration, currentBrightness, targetExposure);
        ASSERT_LE(static_cast<microseconds_t>(targetExposure),
                maxExposure);
        ASSERT_LT(iteration, maxConvergeFrames);
        ASSERT_GE(prevExposure, targetExposure); // check if monotonic
        prevExposure = targetExposure;
        ++iteration;
    }
    SUCCEED();
}

/**
 * @brief Tests flicker correction
 */
TEST_F(ControlAE, getFlickerExposure)
{
    const microseconds_t minExposure = 1000;
    const microseconds_t maxExposure = 100000;
    const microseconds_t exposureStep = 123;

    double targetGain;
    microseconds_t flickerExposure;

    // check 50Hz and 60Hz
    for (double flickerHz: {50.0, 60.0}) {

        double flickerPeriod = 1000000.0/flickerHz;
        printf("flickerPeriod=%f\n", flickerPeriod);

        double requestedExposure = minExposure;
        auto prevExposure = requestedExposure;

        // loop with margins on both sides
        while (requestedExposure < 3*maxExposure) {

            getFlickerExposure(
                flickerPeriod,
                requestedExposure,
                maxExposure,
                targetGain,
                flickerExposure);

            printf("re=%f fe=%d tg=%f\n",
                    requestedExposure,
                    flickerExposure,
                    targetGain);

            ASSERT_LE(flickerExposure, maxExposure);

            // the flickerExposure contains an integer number of flicker periods
            // correctedRequested is the reference value to compare function
            // output
            microseconds_t correctedRequested =
                static_cast<microseconds_t>(std::floor(requestedExposure/flickerPeriod)*flickerPeriod);

            if (requestedExposure < flickerPeriod) {
                // Zone 0 - just checking
                // because this function does not support Zone 0
                // output exposure is zero
                ASSERT_EQ(flickerExposure, 0);
                // targetGain has no meaning
            } else {
                // Zone 1

                if (correctedRequested>maxExposure) {
                    // exposure is limited to maxExposure
                    correctedRequested = maxExposure;
                    // gain can grow higher than 2.0
                } else {
                    ASSERT_LE(targetGain, 2.0);
                }
                ASSERT_GE(targetGain, 1.0);

                // output exposure is always less than requested
                ASSERT_LE(flickerExposure, requestedExposure);
                // compare with test reference
                ASSERT_EQ(flickerExposure, correctedRequested);
                if (prevExposure != flickerExposure) {
                    // stepped to next flicker period
                    ASSERT_GT(flickerExposure, prevExposure);
                    // check output is an integer number of flicker periods
                    // must cope with accumulating errors because flickerExposure
                    // is returned as integer
                    ASSERT_DOUBLE_EQ(ceil(remainder(flickerExposure, flickerPeriod)), 0.0);
                    // check target gain is back to the value near 1.0
                    ASSERT_NEAR(targetGain, 1.0, 0.05);
                }
            }
            prevExposure = flickerExposure;
            requestedExposure += exposureStep;
        }
    }
    SUCCEED();
}

/**
 * @brief
 */
TEST_F(ControlAE, adjustExposureStep)
{
    const microseconds_t minExposure = 500;
    const microseconds_t maxExposure = 100000;

    // loop for different steps and across min and max exposure
    for (microseconds_t exposureStep: { 100, 500, 1000 }) {

        microseconds_t requestedExposure = 0;

        while (requestedExposure < maxExposure+minExposure) {
            double targetGain = 1.0;
            double correctedGain;
            microseconds_t correctedExposure;

            adjustExposureStep(
                exposureStep,
                minExposure,
                maxExposure,
                requestedExposure,
                targetGain,
                correctedExposure,
                correctedGain);
            printf("es=%d re=%d tg=%f ce=%d cg=%f\n",
                    exposureStep, requestedExposure, targetGain,
                    correctedExposure, correctedGain);
            ASSERT_GE(correctedExposure, minExposure);
            ASSERT_LE(correctedExposure, maxExposure);
            ASSERT_GE(correctedExposure, exposureStep);
            ASSERT_EQ(correctedExposure % exposureStep, 0);
            ASSERT_DOUBLE_EQ(requestedExposure * targetGain,
                      correctedExposure * correctedGain);

            requestedExposure += 250;
        }
    }
    SUCCEED();
}

/**
 * @brief auto exposure, full auto, no flicker removal
 */
TEST_F(ControlAE, getAutoExposure_auto_noflicker)
{
    const microseconds_t minExposure = 100;
    const microseconds_t maxExposure = 2000000;

    const double minGain = 1;
    const double maxGain = 6;

    fSensorFrameDuration = 1000000.0/15;
    currentBrightness = -0.05;
    targetBrightness = 0.0;

    enableFixedAeExposure(false);
    enableFixedAeGain(false);

    enableFlickerRejection(false);
    flickerFreq = 50.0; // just in case

    // we must bypass setters because they need Sensor object internally
    fTargetAeGain = 3.0; // above this AE starts reduction of fps

    // above uiMaxAeExposure and fMaxAeGain we have underexposed image
    uiMaxAeExposure = 130000;
    fMaxAeGain = 4.0;

    testAeControl(minExposure, maxExposure, minGain, maxGain);
}

/**
 * @brief auto exposure, full auto, 50Hz flicker removal
 */
TEST_F(ControlAE, getAutoExposure_auto_flicker_50)
{
    const microseconds_t minExposure = 100;
    const microseconds_t maxExposure = 2000000;

    const double minGain = 1;
    const double maxGain = 6;

    fSensorFrameDuration = 1000000.0/15;
    currentBrightness = -0.05;
    targetBrightness = 0.0;

    enableFixedAeExposure(false);
    enableFixedAeGain(false);

    enableFlickerRejection(true, 50.0);

    // we must bypass setters because they need Sensor object internally
    fTargetAeGain = 3; // above this AE starts reduction of fps

    // above uiMaxAeExposure and fMaxAeGain we have underexposed image
    uiMaxAeExposure = 120000;
    fMaxAeGain = 4.0;

    testAeControl(minExposure, maxExposure, minGain, maxGain);

}

/**
 * @brief auto exposure, full auto, 60Hz flicker removal
 */
TEST_F(ControlAE, getAutoExposure_auto_flicker_60)
{
    const microseconds_t minExposure = 100;
    const microseconds_t maxExposure = 2000000;

    const double minGain = 1;
    const double maxGain = 6;

    fSensorFrameDuration = 1000000.0/15;
    currentBrightness = -0.05;
    targetBrightness = 0.0;

    enableFixedAeExposure(false);
    enableFixedAeGain(false);

    enableFlickerRejection(true);
    flickerFreq = 60.0; // just in case

    // we must bypass setters because they need Sensor object internally
    fTargetAeGain = 3.0; // above this AE starts reduction of fps

    // above uiMaxAeExposure and fMaxAeGain we have underexposed image
    uiMaxAeExposure = 120000;

    // fMaxAeGain deliberately set below maxGain to show
    // if underexposed flag is working
    fMaxAeGain = 4.0;

    testAeControl(minExposure, maxExposure, minGain, maxGain);

}
#if(0)
/**
 * @brief auto exposure, full auto, 60Hz flicker removal
 */
TEST_F(ControlAE, getAutoExposure_fixed_gain)
{
    setupHarness();
    const microseconds_t minExposure = 100;
    const microseconds_t maxExposure = 2000000;

    const double minGain = 1;
    const double maxGain = 6;

    fSensorFrameDuration = 1000000.0/15;
    targetBrightness = 0.0;

    enableFixedAeExposure(false);
    enableFixedAeGain(true);

    setFixedAeGain(1.0);

    enableFlickerRejection(false);
    flickerFreq = 60.0; // just in case
    // we must bypass setters because they need Sensor object internally
    fTargetAeGain = 3.0; // above this AE starts reduction of fps
    // above uiMaxAeExposure and fMaxAeGain we have underexposed image
    uiMaxAeExposure = 120000;
    // fMaxAeGain deliberately set below maxGain to show
    // if underexposed flag is working
    fMaxAeGain = 4.0;

    for (double gain: { 1.0, 3.0, 4.0, 5.0 }) {
        testAeControl_fixed_gain(
            gain,
            minExposure,
            maxExposure,
            minGain,
            maxGain);
    }
}
#endif
