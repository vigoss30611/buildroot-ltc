/**
*******************************************************************************
 @file ControlAWB_test.cpp

 @brief Test the AWB control class

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
#include <ispc/ControlAWB_Planckian.h>
#include <ispc/Sensor.h>
#include <ispc/Pipeline.h>
#include <gtest/gtest.h>

#include <deque>
#include <string>
#include <list>
#include <memory>

#undef VERBOSE

#ifndef VERBOSE
#undef printf
#define printf(...)
#endif /* VERBOSE */

class TestSensor : public ISPC::Sensor {
public:
    TestSensor() : ISPC::Sensor(0)
    {
        flFrameRate = 10;
    }
};

class TestPipeline : public ISPC::Pipeline {
public:
    explicit TestPipeline(TestSensor* sensor) :
        ISPC::Pipeline(NULL, 0, NULL) {
        setSensor(sensor);
    }
};

typedef struct testInputNode_tag {
    int nOfFrames;
    double rg;
    double bg;
} testInputNode_t;

#define MAX_INPUT_NODES 10
typedef struct testCase_tag {
    int testCaseNr;
    std::string testCaseDescription;
    int nOfInputNodes;
    testInputNode_t inputNodes[MAX_INPUT_NODES];
    bool filterFlash;
} testCase_t;

/**
 * @note this test fixture enables us to access
 * protected members and methods of ControlAWB...
 */
class ControlAWB : public ISPC::ControlAWB_Planckian, public testing::Test
{
public:
    // defined here in seconds
    int getTemporalStretch_s() {
        return 1;
    }

    int getTemporalStretch_ms() {
        return getTemporalStretch_s()*1000;
    }

    virtual double getFps() {
        return 10.0f;
    }

    int getFpsInt() {
        return static_cast<int>(0.5+getFps());
    }

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

//    void temporalAWBtestFeature(ISPC::ControlAWB_Planckian::Smoothing_Types alg) {
//
//    }
//
    void temporalAWBweigthsTest(bool alg) {
        setupHarness();

        double _T_RG, _T_BG;
        double &T_RG = _T_RG;
        double &T_BG = _T_BG;
        ISPC::rbNode_t rbNode;
        int testNodeNr;
        testCase_t testCase;
        std::list<testCase_t> testCases;
        float weigths[] = {1, 2, 5, 10 };

        /* test case */
        testCase.testCaseNr = 1;
        testCase.filterFlash = false;
        testNodeNr = 0;
        testCase.nOfInputNodes = 2;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = static_cast<int>(floor(getFps()*getTemporalStretch_s()));
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 2.0;
        testCase.inputNodes[testNodeNr].bg = 2.0;
        testCase.inputNodes[testNodeNr].nOfFrames = static_cast<int>(getFps()*getTemporalStretch_s() + 3);
        testCases.push_back(testCase);
        testNodeNr++;
        ASSERT_EQ(testCase.nOfInputNodes, testNodeNr);

        for (std::list<testCase_t>::iterator iter=testCases.begin(); iter != testCases.end(); iter++)
        {
            for(int w=0; w<sizeof(weigths)/sizeof(weigths[0]); w++)
            {
                int cumulativeFrameNo=0;
                setWBTSAlgorithm(alg, weigths[w], getTemporalStretch_ms());
                printf("Test Case Nr %d weight base = %f \n", iter->testCaseNr, weigths[w]);
                printf("calculated (r/g:b/g); vs smoothed r/g:b/g; \n");
                for (testNodeNr = 0; testNodeNr < iter->nOfInputNodes; testNodeNr++)
                {
                    int fNo=1;
                    rbNode.rg = iter->inputNodes[testNodeNr].rg;
                    rbNode.bg = iter->inputNodes[testNodeNr].bg;
                    for (fNo; fNo <= iter->inputNodes[testNodeNr].nOfFrames; fNo++) {
    //                    rbNodes.push_front(rbNode);
                        setFlashFiltering(iter->filterFlash);
                        flashFiltering(rbNode.rg, rbNode.bg);
                        temporalAWBsmoothing(T_RG, T_BG);
                        if (1==fNo) {
                            printf("Nr-%02d-", fNo+cumulativeFrameNo);
                        } else {
                            printf("Nr %02d ", fNo+cumulativeFrameNo);
                        }
                        printf("(%.3f : %.3f) vs (%.3f : %.3f) \n",
                                rbNode.rg, rbNode.bg, T_RG, T_BG);
                    }
                    cumulativeFrameNo += iter->inputNodes[testNodeNr].nOfFrames;
                }
            }
            rbNodes.clear();
        }
    }

    void temporalAWBtest(bool wbtsEnabled, bool flashFilteringEnabled=false) {
        setWBTSAlgorithm(wbtsEnabled);
        setupHarness();

        double _T_RG, _T_BG;
        double &T_RG = _T_RG;
        double &T_BG = _T_BG;
        ISPC::rbNode_t rbNode;
        int testNodeNr;
        testCase_t testCase;
        std::list<testCase_t> testCases;

        /* test case */
        testCase.testCaseNr = 1;
        testCase.filterFlash = false;
        testNodeNr = 0;
        testCase.nOfInputNodes = 2;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = static_cast<int>(floor(getFps()*getTemporalStretch_s()));
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 2.0;
        testCase.inputNodes[testNodeNr].bg = 2.0;
        testCase.inputNodes[testNodeNr].nOfFrames = static_cast<int>(getFps()*getTemporalStretch_s() + 3);
        testCases.push_back(testCase);
        testNodeNr++;
        ASSERT_EQ(testCase.nOfInputNodes, testNodeNr);

        /* test case */
        testCase.testCaseNr++;
        testCase.filterFlash = false;
        testNodeNr = 0;
        testCase.nOfInputNodes = 3;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = static_cast<int>(floor(getFps()*getTemporalStretch_s()));
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 2.0;
        testCase.inputNodes[testNodeNr].bg = 2.0;
        testCase.inputNodes[testNodeNr].nOfFrames = static_cast<int>(floor(getFps()*getTemporalStretch_s() / 2));
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = static_cast<int>(getFps()*getTemporalStretch_s());
        testCases.push_back(testCase);
        testNodeNr++;
        ASSERT_EQ(testCase.nOfInputNodes, testNodeNr);

        /* test case */
        testCase.testCaseNr++;
        testCase.filterFlash = false;
        testNodeNr = 0;
        testCase.nOfInputNodes = 10;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = static_cast<int>(floor(getFps()*getTemporalStretch_s()));
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 2.0;
        testCase.inputNodes[testNodeNr].bg = 2.0;
        testCase.inputNodes[testNodeNr].nOfFrames = 2;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = 2;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 2.0;
        testCase.inputNodes[testNodeNr].bg = 2.0;
        testCase.inputNodes[testNodeNr].nOfFrames = 2;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = 2;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 2.0;
        testCase.inputNodes[testNodeNr].bg = 2.0;
        testCase.inputNodes[testNodeNr].nOfFrames = 2;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = 2;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 2.0;
        testCase.inputNodes[testNodeNr].bg = 2.0;
        testCase.inputNodes[testNodeNr].nOfFrames = 2;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = 2;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = static_cast<int>(getFps()*getTemporalStretch_s());
        testCases.push_back(testCase);
        testNodeNr++;
        ASSERT_EQ(testCase.nOfInputNodes, testNodeNr);

        /* test case */
        testCase.testCaseNr++;
        testCase.filterFlash = false;
        testNodeNr = 0;
        testCase.nOfInputNodes = 10;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = static_cast<int>(floor(getFps()*getTemporalStretch_s()));
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 2.0;
        testCase.inputNodes[testNodeNr].bg = 2.0;
        testCase.inputNodes[testNodeNr].nOfFrames = 2;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = 3;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 2.0;
        testCase.inputNodes[testNodeNr].bg = 2.0;
        testCase.inputNodes[testNodeNr].nOfFrames = 1;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = 2;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 2.0;
        testCase.inputNodes[testNodeNr].bg = 2.0;
        testCase.inputNodes[testNodeNr].nOfFrames = 2;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = 1;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 2.0;
        testCase.inputNodes[testNodeNr].bg = 2.0;
        testCase.inputNodes[testNodeNr].nOfFrames = 3;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = 2;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = static_cast<int>(floor(getFps()*getTemporalStretch_s()));
        testCases.push_back(testCase);
        testCase.filterFlash = false;
        testNodeNr++;
        ASSERT_EQ(testCase.nOfInputNodes, testNodeNr);

        /* test case */
        testCase.testCaseNr++;
        testCase.filterFlash = false;
        testNodeNr = 0;
        testCase.nOfInputNodes = 10;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = static_cast<int>(floor(getFps()*getTemporalStretch_s()));
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 2.0;
        testCase.inputNodes[testNodeNr].bg = 2.0;
        testCase.inputNodes[testNodeNr].nOfFrames = 1;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = 5;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 2.0;
        testCase.inputNodes[testNodeNr].bg = 2.0;
        testCase.inputNodes[testNodeNr].nOfFrames = 5;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = 1;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 2.0;
        testCase.inputNodes[testNodeNr].bg = 2.0;
        testCase.inputNodes[testNodeNr].nOfFrames = 5;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = 5;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 2.0;
        testCase.inputNodes[testNodeNr].bg = 2.0;
        testCase.inputNodes[testNodeNr].nOfFrames = 3;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = 2;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = static_cast<int>(floor(getFps()*getTemporalStretch_s()));
        testCases.push_back(testCase);
        testCase.filterFlash = false;
        testNodeNr++;
        ASSERT_EQ(testCase.nOfInputNodes, testNodeNr);

        /* test case */
        testCase.testCaseNr++;
        testCase.filterFlash = true;
        testCases.push_back(testCase);

        /* test case */
        testCase.testCaseNr++;
        testCase.filterFlash = false;
        testNodeNr = 0;
        testCase.nOfInputNodes = 8;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = static_cast<int>(floor(getFps()*getTemporalStretch_s()));
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 2.0;
        testCase.inputNodes[testNodeNr].bg = 2.0;
        testCase.inputNodes[testNodeNr].nOfFrames = 1;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = 1;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 2.0;
        testCase.inputNodes[testNodeNr].bg = 2.0;
        testCase.inputNodes[testNodeNr].nOfFrames = 10;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = 1;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 2.0;
        testCase.inputNodes[testNodeNr].bg = 2.0;
        testCase.inputNodes[testNodeNr].nOfFrames = 1;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 0.2;
        testCase.inputNodes[testNodeNr].bg = 0.2;
        testCase.inputNodes[testNodeNr].nOfFrames = 1;
        testNodeNr++;
        testCase.inputNodes[testNodeNr].rg = 2.0;
        testCase.inputNodes[testNodeNr].bg = 2.0;
        testCase.inputNodes[testNodeNr].nOfFrames = static_cast<int>(floor(getFps()*getTemporalStretch_s()));
        testCases.push_back(testCase);
        testCase.filterFlash = false;
        testNodeNr++;
        ASSERT_EQ(testCase.nOfInputNodes, testNodeNr);

        /* test case */
        testCase.testCaseNr++;
        testCase.filterFlash = true;
        testCases.push_back(testCase);


        for (std::list<testCase_t>::iterator iter=testCases.begin(); iter != testCases.end(); iter++)
        {
            int cumulativeFrameNo=0;

            printf("Smoothing type %d \n", wbtsEnabled);
            printf("Test Case Nr %d \n", iter->testCaseNr);
            printf("calculated (r/g:b/g); vs smoothed r/g:b/g; \n");
            for (testNodeNr = 0; testNodeNr < iter->nOfInputNodes; testNodeNr++)
            {
                int fNo=1;
                rbNode.rg = iter->inputNodes[testNodeNr].rg;
                rbNode.bg = iter->inputNodes[testNodeNr].bg;
                for (fNo; fNo <= iter->inputNodes[testNodeNr].nOfFrames; fNo++) {
//                    rbNodes.push_front(rbNode);
                    setFlashFiltering(iter->filterFlash);
                    flashFiltering(rbNode.rg, rbNode.bg);
                    temporalAWBsmoothing(T_RG, T_BG);
                    if (1==fNo) {
                        printf("Nr-%02d-", fNo+cumulativeFrameNo);
                    } else {
                        printf("Nr %02d ", fNo+cumulativeFrameNo);
                    }
                    printf("(%.3f : %.3f) vs (%.3f : %.3f) \n",
                            rbNode.rg, rbNode.bg, T_RG, T_BG);
                }
                cumulativeFrameNo += iter->inputNodes[testNodeNr].nOfFrames;
            }
            rbNodes.clear();
        }
    }
};

/**
 * @brief test White Balance Temporal Smoothing NONE.
 */
TEST_F(ControlAWB, wbts_none)
{
    temporalAWBtest(false);
}

/**
 * @brief test White Balance Temporal Smoothing moving average.

TEST_F(ControlAWB, wbts_avg)
{
    temporalAWBtest(WBTS_AVG);
}

/**
 * @brief test White Balance Temporal Smoothing moving averaging weighted.
 */
TEST_F(ControlAWB, wbts_awe)
{
    temporalAWBtest(true);
}

/**
 * @brief test White Balance Temporal Smoothing moving averaging weighted.
 */
TEST_F(ControlAWB, wbts_awe_weight_base_change)
{
    temporalAWBweigthsTest(true);
}


/**
 * @brief test filtering of flash illuminated frames
 */
TEST_F(ControlAWB, flashFiltering)
{
    setupHarness();

    int frameNumber;
    int flashFrame[2];
#define NOFSECTIONS 6
    for (int pass=0; pass<2; pass++)
    {
        temporalAwbClear();
        testInputNode_t testInput[NOFSECTIONS];
        int section;
        testInput[section=0] = {   getFpsInt()*getTemporalStretch_s(), 0.2, 0.2 };
        flashFrame[0] = testInput[section].nOfFrames;
        flashFrame[1] = testInput[section].nOfFrames;
        if (pass == 0) {
            testInput[++section] = {                              1, 4.0, 4.0 };
        } else {
            testInput[++section] = {                              2, 4.0, 4.0 };
        }
        flashFrame[1] += testInput[section].nOfFrames;
        testInput[++section] = {   getFpsInt()*getTemporalStretch_s(), 0.2, 0.2 };
        flashFrame[1] += testInput[section].nOfFrames;
        testInput[++section] = {   getFpsInt()*getTemporalStretch_s(), 1.2, 1.2 };
        flashFrame[1] += testInput[section].nOfFrames;
        if (pass == 0) {
            testInput[++section] = {                              1, 4.0, 4.0 };
        } else {
            testInput[++section] = {                              2, 4.0, 4.0 };
        }
        testInput[++section] = {   getFpsInt()*getTemporalStretch_s(), 1.2, 1.2 };
        ++section;

        ASSERT_EQ(NOFSECTIONS, section);

        frameNumber = 0;

        ISPC::rbNode_t rbNode;
        ISPC::rbNode_t rbNodePrev;
        rbNodePrev.rg = 0;
        rbNodePrev.bg = 0;
        for (int sect = 0; sect < section; sect ++) {
            for (int x=0; x<testInput[sect].nOfFrames; x++) {
                rbNode.rg = testInput[sect].rg;
                rbNode.bg = testInput[sect].bg;
                flashFiltering( rbNode.rg, rbNode.bg );
//                printf("%03d     test input (%.3f : %.3f) vs   last processed (%.3f : %.3f) \n",
//                        frameNumber, rbNode.rg, rbNode.bg,
//                        rbNodes.begin()->rg, rbNodes.begin()->bg);
                if (potentialFlash(rbNode)) {
                    printf("Potential flash frame\n");
                    ASSERT_NE(rbNode.rg, rbNodes.begin()->rg);
                    ASSERT_NE(rbNode.bg, rbNodes.begin()->bg);
                    ASSERT_EQ(rbNode.rg, rbNodesCache.begin()->rg);
                    ASSERT_EQ(rbNode.bg, rbNodesCache.begin()->bg);
                } else {
                    ASSERT_EQ(rbNode.rg, rbNodes.begin()->rg);
                    ASSERT_EQ(rbNode.bg, rbNodes.begin()->bg);
                }
                if ( flashFrame[0] == frameNumber ||
                    flashFrame[1] == frameNumber )
                {
                    rbNodePrev.rg = rbNode.rg;
                    rbNodePrev.bg = rbNode.bg;
//                    printf("%03d rbNodePrev.rg %.3f\n",frameNumber, rbNodePrev.rg);
                    ASSERT_NE(rbNode.rg, rbNodes.begin()->rg);
                    ASSERT_NE(rbNode.bg, rbNodes.begin()->bg);
                    ASSERT_EQ(rbNode.rg, rbNodesCache.begin()->rg);
                    ASSERT_EQ(rbNode.bg, rbNodesCache.begin()->bg);
                }
                if ( (flashFrame[0]+1 == frameNumber) ||
                    (flashFrame[1]+1 == frameNumber) )
                {
//                    printf("%03d rbNodePrev.rg %.3f\n",frameNumber, rbNodePrev.rg);
                    std::deque<ISPC::rbNode_t>::iterator i = rbNodes.begin();
                    ASSERT_EQ(rbNode.rg, i->rg);
                    ASSERT_EQ(rbNode.bg, i->bg);
                    i++;
                    if (pass == 0) {
                        ASSERT_NE(rbNodePrev.rg, i->rg);
                        ASSERT_NE(rbNodePrev.bg, i->bg);
                    } else {
                        ASSERT_EQ(rbNodePrev.rg, i->rg);
                        ASSERT_EQ(rbNodePrev.bg, i->bg);
                    }
                    ASSERT_EQ(rbNodesCache.size(), 0);
                }
                frameNumber++;
            }
        }
    }
}

/**
 * @brief test White Balance Temporal Smoothing moving averaging weighted.
 */
TEST_F(ControlAWB, awe_and_filtering)
{
    temporalAWBtest(true);
}

