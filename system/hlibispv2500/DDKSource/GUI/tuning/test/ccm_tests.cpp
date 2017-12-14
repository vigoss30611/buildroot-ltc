/**
******************************************************************************
@file ccm_tests.cpp

@brief Test some of the CCMCompute functions

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
#include <gtest/gtest.h>

#include "ccm/compute.hpp"

/*class CCM: public CCMCompute, public ::testing::Test
{
};*/

#define DOUBLE_CHK_PREC 0.01

TEST(CCM, RGBtoXYZ)
{
    //double inputs[3], outputs[3], expected[3];
    cv::Scalar inputs, outputs, expected;
    int c;
    double norm = 256.0;
    double prec = DOUBLE_CHK_PREC;

    //
    inputs[0] = 0;
    inputs[1] = 0;
    inputs[2] = 0;

    expected[0] = 0;
    expected[1] = 0;
    expected[2] = 0;

    outputs = CCMCompute::RGBtoXYZ(inputs, norm);

    for ( c = 0 ; c < 3 ; c++ )
    {
        ASSERT_NEAR(expected[c], outputs[c], prec) 
            << "RGBtoXYZ(" << inputs[0] << " " << inputs[1] << " " << inputs[2] <<")[" << c << "]"
            << "==(" << expected[0] << " " << expected[1] << " " << expected[2] << ")"
            << " but is (" << outputs[0] << " " << outputs[1] << " " << outputs[2] << ") norm=" << norm;
    }

    //
    inputs[0] = 0;
    inputs[1] = 0;
    inputs[2] = 100;

    expected[0] = 0.0705;
    expected[1] = 0.0282;
    expected[2] = 0.3712;

    outputs = CCMCompute::RGBtoXYZ(inputs, norm);

    for ( c = 0 ; c < 3 ; c++ )
    {
        ASSERT_NEAR(expected[c], outputs[c], prec) 
            << "RGBtoXYZ(" << inputs[0] << " " << inputs[1] << " " << inputs[2] <<")[" << c << "]"
            << "==(" << expected[0] << " " << expected[1] << " " << expected[2] << ")"
            << " but is (" << outputs[0] << " " << outputs[1] << " " << outputs[2] << ") norm=" << norm;
    }

    //
    inputs[0] = 0;
    inputs[1] = 100;
    inputs[2] = 0;

    expected[0] = 0.1397;
    expected[1] = 0.2794;
    expected[2] = 0.0466;

    outputs = CCMCompute::RGBtoXYZ(inputs, norm);

    for ( c = 0 ; c < 3 ; c++ )
    {
        ASSERT_NEAR(expected[c], outputs[c], prec) 
            << "RGBtoXYZ(" << inputs[0] << " " << inputs[1] << " " << inputs[2] <<")[" << c << "]"
            << "==(" << expected[0] << " " << expected[1] << " " << expected[2] << ")"
            << " but is (" << outputs[0] << " " << outputs[1] << " " << outputs[2] << ") norm=" << norm;
    }

    //
    inputs[0] = 100;
    inputs[1] = 0;
    inputs[2] = 0;

    expected[0] = 0.1611;
    expected[1] = 0.0831;
    expected[2] = 0.0075;

    outputs = CCMCompute::RGBtoXYZ(inputs, norm);

    for ( c = 0 ; c < 3 ; c++ )
    {
        ASSERT_NEAR(expected[c], outputs[c], prec) 
            << "RGBtoXYZ(" << inputs[0] << " " << inputs[1] << " " << inputs[2] <<")[" << c << "]"
            << "==(" << expected[0] << " " << expected[1] << " " << expected[2] << ")"
            << " but is (" << outputs[0] << " " << outputs[1] << " " << outputs[2] << ") norm=" << norm;
    }

    //
    inputs[0] = 100;
    inputs[1] = 100;
    inputs[2] = 0;

    expected[0] = 0.3008;
    expected[1] = 0.3625;
    expected[2] = 0.0541;

    outputs = CCMCompute::RGBtoXYZ(inputs, norm);

    for ( c = 0 ; c < 3 ; c++ )
    {
        ASSERT_NEAR(expected[c], outputs[c], prec) 
            << "RGBtoXYZ(" << inputs[0] << " " << inputs[1] << " " << inputs[2] <<")[" << c << "]"
            << "==(" << expected[0] << " " << expected[1] << " " << expected[2] << ")"
            << " but is (" << outputs[0] << " " << outputs[1] << " " << outputs[2] << ") norm=" << norm;
    }

    //
    inputs[0] = 100;
    inputs[1] = 100;
    inputs[2] = 200;

    expected[0] = 0.4418;
    expected[1] = 0.4189;
    expected[2] = 0.7965;

    outputs = CCMCompute::RGBtoXYZ(inputs, norm);

    for ( c = 0 ; c < 3 ; c++ )
    {
        ASSERT_NEAR(expected[c], outputs[c], prec) 
            << "RGBtoXYZ(" << inputs[0] << " " << inputs[1] << " " << inputs[2] <<")[" << c << "]"
            << "==(" << expected[0] << " " << expected[1] << " " << expected[2] << ")"
            << " but is (" << outputs[0] << " " << outputs[1] << " " << outputs[2] << ") norm=" << norm;
    }

    //
    inputs[0] = 255;
    inputs[1] = 255;
    inputs[2] = 255;

    expected[0] = 0.9468;
    expected[1] = 0.9962;
    expected[2] = 1.0846;

    outputs = CCMCompute::RGBtoXYZ(inputs, norm);

    for ( c = 0 ; c < 3 ; c++ )
    {
        ASSERT_NEAR(expected[c], outputs[c], prec) 
            << "RGBtoXYZ(" << inputs[0] << " " << inputs[1] << " " << inputs[2] <<")[" << c << "]"
            << "==(" << expected[0] << " " << expected[1] << " " << expected[2] << ")"
            << " but is (" << outputs[0] << " " << outputs[1] << " " << outputs[2] << ") norm=" << norm;
    }

    // from real values
    inputs[0] = 233.7620;
    inputs[1] = 233.3998;
    inputs[2] = 222.7754;

    expected[0] = 0.8597;
    expected[1] = 0.9091;
    expected[2] = 0.9533;

    outputs = CCMCompute::RGBtoXYZ(inputs, norm);

    for ( c = 0 ; c < 3 ; c++ )
    {
        ASSERT_NEAR(expected[c], outputs[c], prec) 
            << "RGBtoXYZ(" << inputs[0] << " " << inputs[1] << " " << inputs[2] <<")[" << c << "]"
            << "==(" << expected[0] << " " << expected[1] << " " << expected[2] << ")"
            << " but is (" << outputs[0] << " " << outputs[1] << " " << outputs[2] << ") norm=" << norm;
    }
}

/// @ re-enable when XYZ to LAB values have been given with the new algorithm
TEST(CCM, DISABLED_XYZtoLAB)
{
    //double inputs[3], outputs[3], expected[3];
    cv::Scalar inputs, outputs, expected;
    double prec = DOUBLE_CHK_PREC;
    int c;

    //
    inputs[0] = 0;
    inputs[1] = 0;
    inputs[2] = 0;

    expected[0] = 0;
    expected[1] = 0;
    expected[2] = 0;

    outputs = CCMCompute::XYZtoLAB(inputs);

    for ( c = 0 ; c < 3 ; c++ )
    {
        ASSERT_NEAR(expected[c], outputs[c], prec) 
            << "XYZtoLAB(" << inputs[0] << " " << inputs[1] << " " << inputs[2] <<")[" << c << "]"
            << "==(" << expected[0] << " " << expected[1] << " " << expected[2] << ")"
            << " but is (" << outputs[0] << " " << outputs[1] << " " << outputs[2] << ")";
    }

    //
    inputs[0] = 0;
    inputs[1] = 0;
    inputs[2] = 0.1;

    expected[0] = 0;
    expected[1] = 0;
    expected[2] = -65.2456;

    outputs = CCMCompute::XYZtoLAB(inputs);

    for ( c = 0 ; c < 3 ; c++ )
    {
        ASSERT_NEAR(expected[c], outputs[c], prec) 
            << "XYZtoLAB(" << inputs[0] << " " << inputs[1] << " " << inputs[2] <<")[" << c << "]"
            << "==(" << expected[0] << " " << expected[1] << " " << expected[2] << ")"
            << " but is (" << outputs[0] << " " << outputs[1] << " " << outputs[2] << ")";
    }

    //
    inputs[0] = 0;
    inputs[1] = 0.5;
    inputs[2] = 0.1;

    expected[0] = 76.0693;
    expected[1] = -327.8847;
    expected[2] = 65.9083;

    outputs = CCMCompute::XYZtoLAB(inputs);

    for ( c = 0 ; c < 3 ; c++ )
    {
        ASSERT_NEAR(expected[c], outputs[c], prec) 
            << "XYZtoLAB(" << inputs[0] << " " << inputs[1] << " " << inputs[2] <<")[" << c << "]"
            << "==(" << expected[0] << " " << expected[1] << " " << expected[2] << ")"
            << " but is (" << outputs[0] << " " << outputs[1] << " " << outputs[2] << ")";
    }

    //
    inputs[0] = 1;
    inputs[1] = 0.5;
    inputs[2] = 0.1;

    expected[0] = 76.0693;
    expected[1] = 103.1497;
    expected[2] = 65.9083;

    outputs = CCMCompute::XYZtoLAB(inputs);

    for ( c = 0 ; c < 3 ; c++ )
    {
        ASSERT_NEAR(expected[c], outputs[c], prec) 
            << "XYZtoLAB(" << inputs[0] << " " << inputs[1] << " " << inputs[2] <<")[" << c << "]"
            << "==(" << expected[0] << " " << expected[1] << " " << expected[2] << ")"
            << " but is (" << outputs[0] << " " << outputs[1] << " " << outputs[2] << ")";
    }

    // from real values
    inputs[0] = 0.8597;
    inputs[1] = 0.9091;
    inputs[2] = 0.9533;

    expected[0] = 96.3735;
    expected[1] = -8.9425;
    expected[2] = -3.0871;

    outputs = CCMCompute::XYZtoLAB(inputs);

    for ( c = 0 ; c < 3 ; c++ )
    {
        ASSERT_NEAR(expected[c], outputs[c], prec) 
            << "XYZtoLAB(" << inputs[0] << " " << inputs[1] << " " << inputs[2] <<")[" << c << "]"
            << "==(" << expected[0] << " " << expected[1] << " " << expected[2] << ")"
            << " but is (" << outputs[0] << " " << outputs[1] << " " << outputs[2] << ")";
    }
}

TEST(CCM, RGBtoLAB)
{
    //double inputs[3], outputs[3], tempXYZ[3], expected[3];
    cv::Scalar inputs, outputs, tempXYZ, expected;
    double norm = 256.0;
    double prec = DOUBLE_CHK_PREC;
    int c;
    int tests = 8;

    //
    inputs[0] = 0;
    inputs[1] = 0;
    inputs[2] = 0;

    tempXYZ = CCMCompute::RGBtoXYZ(inputs, norm);
    expected = CCMCompute::XYZtoLAB(tempXYZ);

    outputs = CCMCompute::RGBtoLAB(inputs);

    for ( c = 0 ; c < 3 ; c++ )
    {
        ASSERT_NEAR(expected[c], outputs[c], prec) 
            << "RGBtoLAB(" << inputs[0] << " " << inputs[1] << " " << inputs[2] <<")[" << c << "]"
            << "==RGBtoXYZ+XYZtoLAB==(" << expected[0] << " " << expected[1] << " " << expected[2] << ")"
            << " but is (" << outputs[0] << " " << outputs[1] << " " << outputs[2] << ")";
    }

    for ( int t = 0 ; t < tests ; t++ )
    {
        //
        inputs[0] = CCMCompute::random(0, 255);
        inputs[1] = CCMCompute::random(0, 255);
        inputs[2] = CCMCompute::random(0, 255);

        tempXYZ = CCMCompute::RGBtoXYZ(inputs, norm);
        expected = CCMCompute::XYZtoLAB(tempXYZ);

        outputs = CCMCompute::RGBtoLAB(inputs);

        for ( c = 0 ; c < 3 ; c++ )
        {
            ASSERT_NEAR(expected[c], outputs[c], prec) 
                << "test " << t << " RGBtoLAB(" << inputs[0] << " " << inputs[1] << " " << inputs[2] <<")[" << c << "]"
                << "==RGBtoXYZ+XYZtoLAB==(" << expected[0] << " " << expected[1] << " " << expected[2] << ")"
                << " but is (" << outputs[0] << " " << outputs[1] << " " << outputs[2] << ")";
        }
    }

    //
    inputs[0] = 0;
    inputs[1] = 100;
    inputs[2] = 0;

    tempXYZ = CCMCompute::RGBtoXYZ(inputs, norm);
    expected = CCMCompute::XYZtoLAB(tempXYZ);

    outputs = CCMCompute::RGBtoLAB(inputs);

    for ( c = 0 ; c < 3 ; c++ )
    {
        ASSERT_NEAR(expected[c], outputs[c], prec) 
            << "RGBtoLAB(" << inputs[0] << " " << inputs[1] << " " << inputs[2] <<")[" << c << "]"
            << "==RGBtoXYZ+XYZtoLAB==(" << expected[0] << " " << expected[1] << " " << expected[2] << ")"
            << " but is (" << outputs[0] << " " << outputs[1] << " " << outputs[2] << ")";
    }
    
    //
    inputs[0] = 255;
    inputs[1] = 255;
    inputs[2] = 255;

    tempXYZ = CCMCompute::RGBtoXYZ(inputs, norm);
    expected = CCMCompute::XYZtoLAB(tempXYZ);

    outputs = CCMCompute::RGBtoLAB(inputs);

    for ( c = 0 ; c < 3 ; c++ )
    {
        ASSERT_NEAR(expected[c], outputs[c], prec) 
            << "RGBtoLAB(" << inputs[0] << " " << inputs[1] << " " << inputs[2] <<")[" << c << "]"
            << "==RGBtoXYZ+XYZtoLAB==(" << expected[0] << " " << expected[1] << " " << expected[2] << ")"
            << " but is (" << outputs[0] << " " << outputs[1] << " " << outputs[2] << ")";
    }

    // from real values
    inputs[0] = 233.7620;
    inputs[1] = 233.3998;
    inputs[2] = 222.7754;

    tempXYZ = CCMCompute::RGBtoXYZ(inputs, norm);
    expected = CCMCompute::XYZtoLAB(tempXYZ);

    outputs = CCMCompute::RGBtoLAB(inputs);

    for ( c = 0 ; c < 3 ; c++ )
    {
        ASSERT_NEAR(expected[c], outputs[c], prec) 
            << "RGBtoLAB(" << inputs[0] << " " << inputs[1] << " " << inputs[2] <<")[" << c << "]"
            << "==RGBtoXYZ+XYZtoLAB==(" << expected[0] << " " << expected[1] << " " << expected[2] << ")"
            << " but is (" << outputs[0] << " " << outputs[1] << " " << outputs[2] << ")";
    }
}

TEST(CCM, CIE94)
{
    //double lab1[3], lab2[3];
    cv::Scalar lab1, lab2;
    
    lab1[0] = 50;
    lab1[1] = -50;
    lab1[2] = 50;

    lab2[0] = 100;
    lab2[1] = 0;
    lab2[2] = 50;

    ASSERT_NEAR(55.7707, CCMCompute::CIE94(lab1, lab2), DOUBLE_CHK_PREC) << "CIE94(["<<lab1[0] << " " << lab1[1] << " " << lab1[2] << "],[" << lab2[0] << " " << lab2[1] << " " << lab2[2] << "])";

    lab1[0] = 50;
    lab1[1] = -50;
    lab1[2] = -50;

    lab2[0] = -10;
    lab2[1] = 20;
    lab2[2] = 50;

    ASSERT_NEAR(86.9574, CCMCompute::CIE94(lab1, lab2), DOUBLE_CHK_PREC) << "CIE94(["<<lab1[0] << " " << lab1[1] << " " << lab1[2] << "],[" << lab2[0] << " " << lab2[1] << " " << lab2[2] << "])";
    ASSERT_NEAR(86.9574, CCMCompute::CIE94(lab2, lab1), DOUBLE_CHK_PREC) << "CIE94(["<<lab2[0] << " " << lab2[1] << " " << lab2[2] << "],[" << lab1[0] << " " << lab1[1] << " " << lab1[2] << "])";

    lab1[0] = -10;
    lab1[1] = 20;
    lab1[2] = 50;

    lab2[0] = 0;
    lab2[1] = 0;
    lab2[2] = 0;

    ASSERT_NEAR(54.7723, CCMCompute::CIE94(lab1, lab2), DOUBLE_CHK_PREC) << "CIE94(["<<lab1[0] << " " << lab1[1] << " " << lab1[2] << "],[" << lab2[0] << " " << lab2[1] << " " << lab2[2] << "])";

    lab1[0] = 96.3735;
    lab1[1] = -8.9425;
    lab1[2] = -3.0871;

    lab2[0] = 94.2219;
    lab2[1] = -8.9232;
    lab2[2] = -3.2265;

    ASSERT_NEAR(2.1551, CCMCompute::CIE94(lab1, lab2), DOUBLE_CHK_PREC) << "CIE94(["<<lab1[0] << " " << lab1[1] << " " << lab1[2] << "],[" << lab2[0] << " " << lab2[1] << " " << lab2[2] << "])";
}