/**
*******************************************************************************
 @file TempCorr_test.cpp

 @brief Test the TemperatureCorrection class

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
#include <ispc/TemperatureCorrection.h>
#include <ispc/ISPCDefs.h>
#include <gtest/gtest.h>

#undef VERBOSE

#ifndef VERBOSE
#undef printf
#define printf(...)
#endif /* VERBOSE */

using ISPC::LineSegment;

class TempCorr : public ISPC::TemperatureCorrection, public testing::Test
{
public:
    typedef struct
    {
        double x; // point x
        double y; // point y
        double Xp; // x projected on line
        double Yp; // y projected on line
        double temperature;
    } testpoint;

    double dist(double x1, double y1, double x2, double y2) {
        return std::sqrt(std::pow(x2-x1, 2.0)+std::pow(y2-y1, 2.0));
    }

    /**
     * @brief exact implementation as in
     * TemperatureCorrection::loadParameters()
     */
    void initLines(std::list<testpoint>& points)
    {
        std::list<testpoint>::iterator low = points.begin();
        std::list<testpoint>::iterator high = points.begin();

        while(true)
        {
            ++high;
            if(high == points.end())
            {
                break;
            }
            lines.push_back(LineSegment(
                    ISPC::log2(low->x), // R
                    ISPC::log2(low->y), // B
                    ISPC::log2(high->x), // R
                    ISPC::log2(high->y), // B
                    low->temperature,
                    high->temperature));
            ++low;
        }
        // set borderlines as not bounded
        lines.front().lowBound = false;
        lines.back().highBound = false;
    }

};

TEST_F(TempCorr, LineSegment_length)
{
    const double absError = 0.00000005f;

    EXPECT_DOUBLE_EQ(LineSegment(0.0f, 0.0f, 0.0f, 0.0f).getLength(), 0.0f);
    EXPECT_DOUBLE_EQ(LineSegment(1.0f, 1.0f, 1.0f, 1.0f).getLength(), 0.0f);
    EXPECT_DOUBLE_EQ(LineSegment(-1.0f, 1.0f, -1.0f, 1.0f).getLength(), 0.0f);
    EXPECT_DOUBLE_EQ(LineSegment(1.0f, -1.0f, 1.0f, -1.0f).getLength(), 0.0f);

    EXPECT_DOUBLE_EQ(LineSegment(1.0f, 0.0f, 0.0f, 0.0f).getLength(), 1.0f);
    EXPECT_DOUBLE_EQ(LineSegment(0.0f, 1.0f, 0.0f, 0.0f).getLength(), 1.0f);
    EXPECT_DOUBLE_EQ(LineSegment(0.0f, 0.0f, 1.0f, 0.0f).getLength(), 1.0f);
    EXPECT_DOUBLE_EQ(LineSegment(0.0f, 0.0f, 0.0f, 1.0f).getLength(), 1.0f);

    EXPECT_NEAR(LineSegment(0.0f, 0.0f, 1.0f, 1.0f).getLength(),
            std::sqrt(2.0f), absError);
    EXPECT_NEAR(LineSegment(0.0f, 0.0f,-1.0f,-1.0f).getLength(),
            std::sqrt(2.0f), absError);
    EXPECT_NEAR(LineSegment(0.0f, 0.0f,-1.0f, 1.0f).getLength(),
            std::sqrt(2.0f), absError);
    EXPECT_NEAR(LineSegment(0.0f, 0.0f, 1.0f,-1.0f).getLength(),
            std::sqrt(2.0f), absError);

    EXPECT_NEAR(LineSegment(1.0f, 1.0f, 0.0f, 0.0f).getLength(),
            std::sqrt(2.0f), absError);
    EXPECT_NEAR(LineSegment(-1.0f,-1.0f, 0.0f, 0.0f).getLength(),
            std::sqrt(2.0f), absError);
    EXPECT_NEAR(LineSegment(-1.0f, 1.0f, 0.0f, 0.0f).getLength(),
            std::sqrt(2.0f), absError);
    EXPECT_NEAR(LineSegment(1.0f,-1.0f, 0.0f, 0.0f).getLength(),
            std::sqrt(2.0f), absError);
}

TEST_F(TempCorr, LineSegment_horizontal)
{
    double Xp, Yp;

    LineSegment LineSegments[] = {
        LineSegment( 1.0f, 0.0f,-1.0f, 0.0f),
        LineSegment( 1.0f, 1.0f,-1.0f, 1.0f) // parallel to line1, offset 1
    };
    for(LineSegment& line: LineSegments)
    {
        for(int x=-10;x<10;++x)
        {
            for(int y=-10;y<10;++y)
            {
                line.getRbProj(double(x), double(y), Xp, Yp);
                if(x<line.x2)
                {
                    EXPECT_DOUBLE_EQ(line.x2, Xp);
                } else if(x>line.x1) {
                    EXPECT_DOUBLE_EQ(line.x1, Xp);
                } else {
                    EXPECT_DOUBLE_EQ(double(x), Xp);
                }
                EXPECT_DOUBLE_EQ(line.y1, Yp);

                EXPECT_DOUBLE_EQ(
                    line.getDistance(x, y),
                    dist(x, y, Xp, Yp));
            }
        }
    }
}


/**
 * @brief test the projected point (Xp, Yp) and the distance from (x,y)
 *
 * @note line segments bounded from both sides
 */
TEST_F(TempCorr, LineSegment_bound)
{
    double Xp, Yp;

    {
        // line 45degrees, no offset
        LineSegment LineSegments[] = {
            LineSegment( 1.0f, 1.0f,-1.0f,-1.0f),
            LineSegment(-1.0f,-1.0f, 1.0f, 1.0f)
        };
        testpoint lineVectors[] = {
            {-2.0f,-2.0f,-1.0f,-1.0f}, // bound
            {-1.0f,-1.0f,-1.0f,-1.0f},
            { 0.0f, 0.0f, 0.0f, 0.0f},
            { 1.0f, 1.0f, 1.0f, 1.0f},
            { 2.0f, 2.0f, 1.0f, 1.0f}, // bound

            { 1.0f, 3.0f, 1.0f, 1.0f}, // bound
            { 0.0f, 2.0f, 1.0f, 1.0f},
            {-1.0f, 1.0f, 0.0f, 0.0f},
            {-2.0f, 0.0f,-1.0f,-1.0f},
            {-3.0f,-1.0f,-1.0f,-1.0f}, // bound

            { 3.0f, 1.0f, 1.0f, 1.0f}, // bound
            { 2.0f, 0.0f, 1.0f, 1.0f},
            {-1.0f,-1.0f,-1.0f,-1.0f},
            { 0.0f,-2.0f,-1.0f,-1.0f},
            {-1.0f,-3.0f,-1.0f,-1.0f}, // bound
        };
        for(LineSegment& line: LineSegments)
        {
            for(testpoint point: lineVectors)
            {
                line.getRbProj(point.x, point.y, Xp, Yp);
                EXPECT_DOUBLE_EQ(point.Xp, Xp);
                EXPECT_DOUBLE_EQ(point.Yp, Yp);
                EXPECT_DOUBLE_EQ(
                    line.getDistance(point.x, point.y),
                    dist(point.x, point.y, point.Xp, point.Yp));
            }
        }
    }
    {
        // line -45degrees, no offset
        LineSegment LineSegments[] = {
            LineSegment(-1.0f, 1.0f, 1.0f,-1.0f),
            LineSegment( 1.0f,-1.0f,-1.0f, 1.0f)
        };
        testpoint lineVectors[] = {
            {-2.0f, 2.0f,-1.0f, 1.0f}, // bound
            {-1.0f, 1.0f,-1.0f, 1.0f},
            { 0.0f, 0.0f, 0.0f, 0.0f},
            { 1.0f,-1.0f, 1.0f,-1.0f},
            { 2.0f,-2.0f, 1.0f,-1.0f}, // bound

            {-1.0f, 3.0f,-1.0f, 1.0f}, // bound
            { 0.0f, 2.0f,-1.0f, 1.0f},
            { 1.0f, 1.0f, 0.0f, 0.0f},
            { 2.0f, 0.0f, 1.0f,-1.0f},
            { 3.0f,-1.0f, 1.0f,-1.0f}, // bound

            {-3.0f, 1.0f,-1.0f, 1.0f}, // bound
            {-2.0f, 0.0f,-1.0f, 1.0f},
            {-1.0f,-1.0f, 0.0f, 0.0f},
            { 0.0f,-2.0f, 1.0f,-1.0f},
            {1.0f, -3.0f, 1.0f,-1.0f}, // bound
        };
        for(LineSegment& line: LineSegments)
        {
            for(testpoint& point: lineVectors)
            {
                line.getRbProj(point.x, point.y, Xp, Yp);
                EXPECT_DOUBLE_EQ(point.Xp, Xp);
                EXPECT_DOUBLE_EQ(point.Yp, Yp);

                EXPECT_DOUBLE_EQ(
                    line.getDistance(point.x, point.y),
                    dist(point.x, point.y, point.Xp, point.Yp));
            }
        }
    }
}


/**
 * @brief test the projected point (Xp, Yp) and the distance from (x,y)
 *
 * @note line segments unbounded from right side only
 */
TEST_F(TempCorr, LineSegment_unbound_right)
{
    double Xp, Yp;

    {
        // line 45degrees, no offset, unbound at (1,1)
        LineSegment LineSegments[] = {
            LineSegment( 1.0f, 1.0f,-1.0f,-1.0f),
            LineSegment(-1.0f,-1.0f, 1.0f, 1.0f)
        };
        LineSegments[0].lowBound = false;
        LineSegments[1].highBound = false;
        testpoint lineVectors[] = {
            {-2.0f,-2.0f,-1.0f,-1.0f}, // bound
            {-1.0f,-1.0f,-1.0f,-1.0f},
            { 0.0f, 0.0f, 0.0f, 0.0f},
            { 1.0f, 1.0f, 1.0f, 1.0f},
            { 2.0f, 2.0f, 2.0f, 2.0f},

            { 1.0f, 3.0f, 2.0f, 2.0f},
            { 0.0f, 2.0f, 1.0f, 1.0f},
            {-1.0f, 1.0f, 0.0f, 0.0f},
            {-2.0f, 0.0f,-1.0f,-1.0f},
            {-3.0f,-1.0f,-1.0f,-1.0f}, // bound

            { 3.0f, 1.0f, 2.0f, 2.0f},
            { 2.0f, 0.0f, 1.0f, 1.0f},
            {-1.0f,-1.0f,-1.0f,-1.0f},
            { 0.0f,-2.0f,-1.0f,-1.0f},
            {-1.0f,-3.0f,-1.0f,-1.0f}, // bound
        };
        for(LineSegment& line: LineSegments)
        {
            for(testpoint point: lineVectors)
            {
                line.getRbProj(point.x, point.y, Xp, Yp);
                EXPECT_DOUBLE_EQ(point.Xp, Xp);
                EXPECT_DOUBLE_EQ(point.Yp, Yp);

                EXPECT_DOUBLE_EQ(
                    line.getDistance(point.x, point.y),
                    dist(point.x, point.y, point.Xp, point.Yp));
            }
        }
    }
    {
        // line -45degrees, no offset, unbound at (1, -1)
        LineSegment LineSegments[] = {
            LineSegment(-1.0f, 1.0f, 1.0f,-1.0f),
            LineSegment( 1.0f,-1.0f,-1.0f, 1.0f)
        };
        LineSegments[0].highBound = false;
        LineSegments[1].lowBound = false;
        testpoint lineVectors[] = {
            {-2.0f, 2.0f,-1.0f, 1.0f}, // bound
            {-1.0f, 1.0f,-1.0f, 1.0f},
            { 0.0f, 0.0f, 0.0f, 0.0f},
            { 1.0f,-1.0f, 1.0f,-1.0f},
            { 2.0f,-2.0f, 2.0f,-2.0f},

            {-1.0f, 3.0f,-1.0f, 1.0f}, // bound
            { 0.0f, 2.0f,-1.0f, 1.0f},
            { 1.0f, 1.0f, 0.0f, 0.0f},
            { 2.0f, 0.0f, 1.0f,-1.0f},
            { 3.0f,-1.0f, 2.0f,-2.0f},

            {-3.0f, 1.0f,-1.0f, 1.0f}, // bound
            {-2.0f, 0.0f,-1.0f, 1.0f},
            {-1.0f,-1.0f, 0.0f, 0.0f},
            { 0.0f,-2.0f, 1.0f,-1.0f},
            {1.0f, -3.0f, 2.0f,-2.0f},
        };
        for(LineSegment& line: LineSegments)
        {
            for(testpoint& point: lineVectors)
            {
                line.getRbProj(point.x, point.y, Xp, Yp);
                EXPECT_DOUBLE_EQ(point.Xp, Xp);
                EXPECT_DOUBLE_EQ(point.Yp, Yp);

                EXPECT_DOUBLE_EQ(
                    line.getDistance(point.x, point.y),
                    dist(point.x, point.y, point.Xp, point.Yp));
            }
        }
    }
}

/**
 * @brief test the projected point (Xp, Yp) and the distance from (x,y)
 *
 * @note line segments unbounded from left side only
 */
TEST_F(TempCorr, LineSegment_unbound_left)
{
    double Xp, Yp;

    {
        // line 45degrees, no offset, unbound at (-1, -1)
        LineSegment LineSegments[] = {
            LineSegment( 1.0f, 1.0f,-1.0f,-1.0f),
            LineSegment(-1.0f,-1.0f, 1.0f, 1.0f)
        };
        LineSegments[0].highBound = false;
        LineSegments[1].lowBound = false;
        testpoint lineVectors[] = {
            {-2.0f,-2.0f,-2.0f,-2.0f},
            {-1.0f,-1.0f,-1.0f,-1.0f},
            { 0.0f, 0.0f, 0.0f, 0.0f},
            { 1.0f, 1.0f, 1.0f, 1.0f},
            { 2.0f, 2.0f, 1.0f, 1.0f}, // bound

            { 1.0f, 3.0f, 1.0f, 1.0f}, // bound
            { 0.0f, 2.0f, 1.0f, 1.0f},
            {-1.0f, 1.0f, 0.0f, 0.0f},
            {-2.0f, 0.0f,-1.0f,-1.0f},
            {-3.0f,-1.0f,-2.0f,-2.0f},

            { 3.0f, 1.0f, 1.0f, 1.0f}, // bound
            { 2.0f, 0.0f, 1.0f, 1.0f},
            {-1.0f,-1.0f,-1.0f,-1.0f},
            { 0.0f,-2.0f,-1.0f,-1.0f},
            {-1.0f,-3.0f,-2.0f,-2.0f},
        };
        for(LineSegment& line: LineSegments)
        {
            for(testpoint point: lineVectors)
            {
                line.getRbProj(point.x, point.y, Xp, Yp);
                EXPECT_DOUBLE_EQ(point.Xp, Xp);
                EXPECT_DOUBLE_EQ(point.Yp, Yp);

                EXPECT_DOUBLE_EQ(
                    line.getDistance(point.x, point.y),
                    dist(point.x, point.y, point.Xp, point.Yp));
            }
        }
    }
    {
        // line -45degrees, no offset, unbound at (-1, 1)
        LineSegment LineSegments[] = {
            LineSegment(-1.0f, 1.0f, 1.0f,-1.0f),
            LineSegment( 1.0f,-1.0f,-1.0f, 1.0f)
        };
        LineSegments[0].lowBound = false;
        LineSegments[1].highBound = false;
        testpoint lineVectors[] = {
            {-2.0f, 2.0f,-2.0f, 2.0f},
            {-1.0f, 1.0f,-1.0f, 1.0f},
            { 0.0f, 0.0f, 0.0f, 0.0f},
            { 1.0f,-1.0f, 1.0f,-1.0f},
            { 2.0f,-2.0f, 1.0f,-1.0f}, // bound

            {-1.0f, 3.0f,-2.0f, 2.0f},
            { 0.0f, 2.0f,-1.0f, 1.0f},
            { 1.0f, 1.0f, 0.0f, 0.0f},
            { 2.0f, 0.0f, 1.0f,-1.0f},
            { 3.0f,-1.0f, 1.0f,-1.0f}, // bound

            {-3.0f, 1.0f,-2.0f, 2.0f},
            {-2.0f, 0.0f,-1.0f, 1.0f},
            {-1.0f,-1.0f, 0.0f, 0.0f},
            { 0.0f,-2.0f, 1.0f,-1.0f},
            {1.0f, -3.0f, 1.0f,-1.0f}, // bound
        };
        for(LineSegment& line: LineSegments)
        {
            for(testpoint& point: lineVectors)
            {
                line.getRbProj(point.x, point.y, Xp, Yp);
                EXPECT_DOUBLE_EQ(point.Xp, Xp);
                EXPECT_DOUBLE_EQ(point.Yp, Yp);

                EXPECT_DOUBLE_EQ(
                    line.getDistance(point.x, point.y),
                    dist(point.x, point.y, point.Xp, point.Yp));
            }
        }
    }
}

/**
 * @brief test the projected point (Xp, Yp) and the distance from (x,y)
 *
 * @note line segments unbounded from both sides
 */
TEST_F(TempCorr, LineSegment_unbound_both)
{
    double Xp, Yp;

    // line 45degrees, no offset, unbound at both sides
    {
        LineSegment LineSegments[] = {
            LineSegment( 1.0f, 1.0f,-1.0f,-1.0f),
            LineSegment(-1.0f,-1.0f, 1.0f, 1.0f)
        };
        LineSegments[0].lowBound = false;
        LineSegments[0].highBound = false;
        LineSegments[1].lowBound = false;
        LineSegments[1].highBound = false;
        testpoint lineVectors[] = {
            {-2.0f,-2.0f,-2.0f,-2.0f},
            {-1.0f,-1.0f,-1.0f,-1.0f},
            { 0.0f, 0.0f, 0.0f, 0.0f},
            { 1.0f, 1.0f, 1.0f, 1.0f},
            { 2.0f, 2.0f, 2.0f, 2.0f},

            { 1.0f, 3.0f, 2.0f, 2.0f},
            { 0.0f, 2.0f, 1.0f, 1.0f},
            {-1.0f, 1.0f, 0.0f, 0.0f},
            {-2.0f, 0.0f,-1.0f,-1.0f},
            {-3.0f,-1.0f,-2.0f,-2.0f},

            { 3.0f, 1.0f, 2.0f, 2.0f}, // bound
            { 2.0f, 0.0f, 1.0f, 1.0f},
            {-1.0f,-1.0f,-1.0f,-1.0f},
            { 0.0f,-2.0f,-1.0f,-1.0f},
            {-1.0f,-3.0f,-2.0f,-2.0f},
        };
        for(LineSegment& line: LineSegments)
        {
            for(testpoint point: lineVectors)
            {
                line.getRbProj(point.x, point.y, Xp, Yp);
                EXPECT_DOUBLE_EQ(point.Xp, Xp);
                EXPECT_DOUBLE_EQ(point.Yp, Yp);

                EXPECT_DOUBLE_EQ(
                    line.getDistance(point.x, point.y),
                    dist(point.x, point.y, point.Xp, point.Yp));
            }
        }
    }
    {
        // line -45degrees, no offset, unbound at both sides
        LineSegment LineSegments[] = {
            LineSegment(-1.0f, 1.0f, 1.0f,-1.0f),
            LineSegment( 1.0f,-1.0f,-1.0f, 1.0f)
        };
        LineSegments[0].highBound = false;
        LineSegments[0].lowBound = false;
        LineSegments[1].highBound = false;
        LineSegments[1].lowBound = false;
        testpoint lineVectors[] = {
            {-2.0f, 2.0f,-2.0f, 2.0f},
            {-1.0f, 1.0f,-1.0f, 1.0f},
            { 0.0f, 0.0f, 0.0f, 0.0f},
            { 1.0f,-1.0f, 1.0f,-1.0f},
            { 2.0f,-2.0f, 2.0f,-2.0f},

            {-1.0f, 3.0f,-2.0f, 2.0f},
            { 0.0f, 2.0f,-1.0f, 1.0f},
            { 1.0f, 1.0f, 0.0f, 0.0f},
            { 2.0f, 0.0f, 1.0f,-1.0f},
            { 3.0f,-1.0f, 2.0f,-2.0f},

            {-3.0f, 1.0f,-2.0f, 2.0f},
            {-2.0f, 0.0f,-1.0f, 1.0f},
            {-1.0f,-1.0f, 0.0f, 0.0f},
            { 0.0f,-2.0f, 1.0f,-1.0f},
            {1.0f, -3.0f, 2.0f,-2.0f},
        };
        for(LineSegment& line: LineSegments)
        {
            for(testpoint& point: lineVectors)
            {
                line.getRbProj(point.x, point.y, Xp, Yp);
                EXPECT_DOUBLE_EQ(point.Xp, Xp);
                EXPECT_DOUBLE_EQ(point.Yp, Yp);

                EXPECT_DOUBLE_EQ(
                    line.getDistance(point.x, point.y),
                    dist(point.x, point.y, point.Xp, point.Yp));
            }
        }
    }
}

/**
 * @brief test if the temperature changes monotonically when interpolated
 * across whole line segment
 */
TEST_F(TempCorr, LineSegment_temperature_direction)
{
    double lastTemp;
    // line: R->L
    // T: low->high
    {
        LineSegment line1 = LineSegment( 1.0f, 0.0f,-1.0f, 0.0f, 3000.0f, 4000.0f);
        EXPECT_DOUBLE_EQ(line1.temperatureDensity, (4000-3000)/line1.getLength());
        EXPECT_DOUBLE_EQ(line1.getTemperature(line1.x1, line1.y1), line1.t1);
        EXPECT_DOUBLE_EQ(line1.getTemperature(line1.x2, line1.y2), 4000.0f);

        for(double y=-10;y<10.0;y+=0.5)
        {
            // horizontal line without any offset
            lastTemp = 0.0;
            // check if it's monotonic
            for(double x=line1.x1;x>=line1.x2;x-=0.05)
            {
                double temp = line1.getTemperature(x, y);
                //printf("p(%f,%f) t=%f\n", x, y, temp);
                EXPECT_LT(lastTemp, temp);
                EXPECT_LE(temp, 4000.0f);
                EXPECT_GE(temp, 3000.0f);
                lastTemp = temp;

            }
        }
    }
    // line: R->L
    // high->low
    {
        LineSegment line1 = LineSegment( 1.0f, 0.0f,-1.0f, 0.0f, 4000.0f, 3000.0f);
        EXPECT_DOUBLE_EQ(line1.temperatureDensity, (3000-4000)/line1.getLength());
        EXPECT_DOUBLE_EQ(line1.getTemperature(line1.x1, line1.y1), line1.t1);
        EXPECT_DOUBLE_EQ(line1.getTemperature(line1.x2, line1.y2), 3000.0f);

        for(double y=-10;y<10.0;y+=0.5)
        {
            // horizontal line without any offset
            lastTemp = 4001.0;
            // check if it's monotonic
            for(double x=line1.x1;
                        x>=line1.x2;)
            {
                double temp = line1.getTemperature(x, y);
                //printf("p(%f,%f) t=%f\n", x, y, temp);
                EXPECT_GT(lastTemp, temp);
                EXPECT_LE(temp, 4000.0f);
                EXPECT_GE(temp, 3000.0f);
                lastTemp = temp;
                x-=0.05;
            }
        }
    }
    // line: L->R
    // T: low->high
    {
        LineSegment line1 = LineSegment( -1.0f, 0.0f, 1.0f, 0.0f, 3000.0f, 4000.0f);
        EXPECT_DOUBLE_EQ(line1.temperatureDensity, (4000-3000)/line1.getLength());
        EXPECT_DOUBLE_EQ(line1.getTemperature(line1.x1, line1.y1), line1.t1);
        EXPECT_DOUBLE_EQ(line1.getTemperature(line1.x2, line1.y2), 4000.0f);

        for(double y=-10;y<10.0;y+=0.5)
        {
            // horizontal line without any offset
            lastTemp = 0.0;
            // check if it's monotonic
            for(double x=line1.x1;
                        x<=line1.x2;)
            {
                double temp = line1.getTemperature(x, y);
                //printf("p(%f,%f) t=%f\n", x, y, temp);
                EXPECT_LT(lastTemp, temp);
                EXPECT_LE(temp, 4000.0f);
                EXPECT_GE(temp, 3000.0f);
                lastTemp = temp;
                x+=0.05;
            }
        }
    }
    // line: L->R
    // T: high->low
    {
        LineSegment line1 = LineSegment( -1.0f, 0.0f, 1.0f, 0.0f, 4000.0f, 3000.0f);
        EXPECT_DOUBLE_EQ(line1.temperatureDensity, (3000-4000)/line1.getLength());
        EXPECT_DOUBLE_EQ(line1.getTemperature(line1.x1, line1.y1), line1.t1);
        EXPECT_DOUBLE_EQ(line1.getTemperature(line1.x2, line1.y2), 3000.0f);

        for(double y=-10;y<10.0;y+=0.5)
        {
            // horizontal line without any offset
            lastTemp = 4001.0;
            // check if it's monotonic
            for(double x=line1.x1;
                        x<=line1.x2;)
            {
                double temp = line1.getTemperature(x, y);
                //printf("p(%f,%f) t=%f\n", x, y, temp);
                EXPECT_GT(lastTemp, temp);
                EXPECT_LE(temp, 4000.0f);
                EXPECT_GE(temp, 3000.0f);
                lastTemp = temp;
                x+=0.05;
            }
        }
    }
}

/**
 * @brief test if the unbound line segment extrapolates the temperature
 * with right sign
 *
 * @note line segment with x coefficient>0
 */

TEST_F(TempCorr, correlateTemp_extrapolation1)
{
    std::list<testpoint> points;

    // x2>x1 , y2>y1 , t2>t1
    points.push_back(testpoint({1.0f, 1.0f, 0, 0, 1000}));
    points.push_back(testpoint({2.0f, 2.0f, 0, 0, 2000}));

    initLines(points);

    EXPECT_LT(
        getCorrelatedTemperature_Calibrated(
                0.5f, 1.0f, 0.5f),
        1000.0f);
    EXPECT_GT(
        getCorrelatedTemperature_Calibrated(
                2.5f, 1.0f, 2.5f),
        2000.0f);

    double temp = getCorrelatedTemperature_Calibrated(1.5f, 1.0f, 1.5f);
    EXPECT_GT(temp, 1000.0f);
    EXPECT_LT(temp, 2000.0f);

    lines.clear();
    points.clear();
    // x2>x1 , y2>y1 , t2<t1
    points.push_back(testpoint({1.0f, 1.0f, 0, 0, 2000}));
    points.push_back(testpoint({2.0f, 2.0f, 0, 0, 1000}));

    initLines(points);

    EXPECT_GT(
        getCorrelatedTemperature_Calibrated(
                0.5f, 1.0f, 0.5f),
        2000.0f);
    EXPECT_LT(
        getCorrelatedTemperature_Calibrated(
                2.5f, 1.0f, 2.5f),
        1000.0f);

    temp = getCorrelatedTemperature_Calibrated(1.5f, 1.0f, 1.5f);
    EXPECT_GT(temp, 1000.0f);
    EXPECT_LT(temp, 2000.0f);

    lines.clear();
    points.clear();
    // x2<x1 , y2<y1 , t2>t1
    points.push_back(testpoint({2.0f, 2.0f, 0, 0, 1000}));
    points.push_back(testpoint({1.0f, 1.0f, 0, 0, 2000}));

    initLines(points);

    EXPECT_GT(
        getCorrelatedTemperature_Calibrated(
                0.5f, 1.0f, 0.5f),
        2000.0f);
    EXPECT_LT(
        getCorrelatedTemperature_Calibrated(
                2.5f, 1.0f, 2.5f),
        1000.0f);

    temp = getCorrelatedTemperature_Calibrated(1.5f, 1.0f, 1.5f);
    EXPECT_GT(temp, 1000.0f);
    EXPECT_LT(temp, 2000.0f);

    lines.clear();
    points.clear();
    // x2<x1 , y2<y1 , t2<t1
    points.push_back(testpoint({2.0f, 2.0f, 0, 0, 2000}));
    points.push_back(testpoint({1.0f, 1.0f, 0, 0, 1000}));

    initLines(points);

    EXPECT_LT(
        getCorrelatedTemperature_Calibrated(
                0.5f, 1.0f, 0.5f),
        1000.0f);
    EXPECT_GT(
        getCorrelatedTemperature_Calibrated(
                2.5f, 1.0f, 2.5f),
        2000.0f);

    temp = getCorrelatedTemperature_Calibrated(1.5f, 1.0f, 1.5f);
    EXPECT_GT(temp, 1000.0f);
    EXPECT_LT(temp, 2000.0f);

}

/**
 * @brief test if the unbound line segment extrapolates the temperature
 * with right sign
 *
 * @note line segment with x coefficient <0
 */
TEST_F(TempCorr, correlateTemp_extrapolation2)
{
    std::list<testpoint> points;

    // x2>x1 , y2<y1 , t2>t1
    points.push_back(testpoint({1.0f, 2.0f, 0, 0, 1000}));
    points.push_back(testpoint({2.0f, 1.0f, 0, 0, 2000}));

    initLines(points);

    EXPECT_LT(
        getCorrelatedTemperature_Calibrated(
                0.5f, 1.0f, 2.5f),
        1000.0f);
    EXPECT_GT(
        getCorrelatedTemperature_Calibrated(
                2.5f, 1.0f, 0.5f),
        2000.0f);

    double temp = getCorrelatedTemperature_Calibrated(1.5f, 1.0f, 1.5f);
    EXPECT_GT(temp, 1000.0f);
    EXPECT_LT(temp, 2000.0f);

    lines.clear();
    points.clear();
    // x2>x1 , y2<y1 , t2<t1
    points.push_back(testpoint({1.0f, 2.0f, 0, 0, 2000}));
    points.push_back(testpoint({2.0f, 1.0f, 0, 0, 1000}));

    initLines(points);

    EXPECT_GT(
        getCorrelatedTemperature_Calibrated(
                0.5f, 1.0f, 2.5f),
        2000.0f);
    EXPECT_LT(
        getCorrelatedTemperature_Calibrated(
                2.5f, 1.0f, 0.5f),
        1000.0f);

    temp = getCorrelatedTemperature_Calibrated(1.5f, 1.0f, 1.5f);
    EXPECT_GT(temp, 1000.0f);
    EXPECT_LT(temp, 2000.0f);

    lines.clear();
    points.clear();
    // x2<x1 , y2>y1 , t2>t1
    points.push_back(testpoint({2.0f, 1.0f, 0, 0, 1000}));
    points.push_back(testpoint({1.0f, 2.0f, 0, 0, 2000}));

    initLines(points);

    EXPECT_GT(
        getCorrelatedTemperature_Calibrated(
                0.5f, 1.0f, 2.5f),
        2000.0f);
    EXPECT_LT(
        getCorrelatedTemperature_Calibrated(
                2.5f, 1.0f, 0.5f),
        1000.0f);

    temp = getCorrelatedTemperature_Calibrated(1.5f, 1.0f, 1.5f);
    EXPECT_GT(temp, 1000.0f);
    EXPECT_LT(temp, 2000.0f);

    lines.clear();
    points.clear();
    // x2<x1 , y2>y1 , t2<t1
    points.push_back(testpoint({2.0f, 1.0f, 0, 0, 2000}));
    points.push_back(testpoint({1.0f, 2.0f, 0, 0, 1000}));

    initLines(points);

    EXPECT_LT(
        getCorrelatedTemperature_Calibrated(
                0.5f, 1.0f, 2.5f),
        1000.0f);
    EXPECT_GT(
        getCorrelatedTemperature_Calibrated(
                2.5f, 1.0f, 0.5f),
        2000.0f);

    temp = getCorrelatedTemperature_Calibrated(1.5f, 1.0f, 1.5f);
    EXPECT_GT(temp, 1000.0f);
    EXPECT_LT(temp, 2000.0f);
}


/**
 * @brief verify the temperature in corner points
 */
TEST_F(TempCorr, correlateTemp)
{
    std::list<testpoint> points;
    points.push_back(testpoint({1.49505f, 2.59104f, 0, 0, 2800}));
    points.push_back(testpoint({1.63682f, 2.41592f, 0, 0, 3200}));
    points.push_back(testpoint({2.23547f, 1.86531f, 0, 0, 5000}));
    points.push_back(testpoint({2.34968f, 1.76885f, 0, 0, 5500}));
    points.push_back(testpoint({2.52683f, 1.60813f, 0, 0, 6500}));
    points.push_back(testpoint({2.65856f, 1.51728f, 0, 0, 7500}));

    initLines(points);

    std::list<testpoint>::iterator point = points.begin();
    point = points.begin();
    while(point!=points.end())
    {
        EXPECT_DOUBLE_EQ(
            getCorrelatedTemperature_Calibrated(
                    point->x,
                    1.0,
                    point->y),
                    point->temperature);
        ++point;
    }
}

/**
 * @brief verify the temperature in corner points
 */
TEST_F(TempCorr, capturePlanckianLocus)
{
    double QEr = 2.52683f;
    double QEb = 1.60813f;
    std::list<testpoint> points;
    points.push_back(testpoint({QEr/1.49505f, QEb/2.59104f, 0, 0, 2800}));
    points.push_back(testpoint({QEr/1.63682f, QEb/2.41592f, 0, 0, 3200}));
    points.push_back(testpoint({QEr/2.23547f, QEb/1.86531f, 0, 0, 5000}));
    points.push_back(testpoint({QEr/2.34968f, QEb/1.76885f, 0, 0, 5500}));
    points.push_back(testpoint({QEr/2.52683f, QEb/1.60813f, 0, 0, 6500}));
    points.push_back(testpoint({QEr/2.65856f, QEb/1.51728f, 0, 0, 7500}));

    initLines(points);

    auto line = lines.begin();

    printf("ratioR;ratioB;calibrated;McCamy;sRGB_table\n");
    double dx, dy, x, y;
    while(line!=lines.end())
    {
        //printf("%fK@(%f,%f)->(%f,%f)\n", line->t1, line->x1, line->y1,
        //        line->x2, line->y2);
        x = line->x1;
        y = line->y1;
        dx = (line->x2-line->x1)/8;
        dy = (line->y2-line->y1)/8;
        // step on the line
        while(line->getLength(x, y, line->x1, line->y1) < line->getLength())
        {
            double ratioR = std::pow(2.0, x);
            double ratioB = std::pow(2.0, y);
            double temp_cal = getCorrelatedTemperature_Calibrated(
                    ratioR, 1.0, ratioB);
            double temp_McCamy = getCorrelatedTemperature_McCamy(
                                ratioR, 1.0, ratioB, true);
            double temp_sRGB = getCorrelatedTemperature_sRGB(
                                ratioR, 1.0, ratioB);
            printf("%f;%f;%f;%f;%f\n",
                    ratioR, ratioB, temp_cal, temp_McCamy, temp_sRGB);
            x+=dx;
            y+=dy;
        }
        ++line;
    }
}

/**
 * @brief verify the temperature in corner points
 */
TEST_F(TempCorr, captureColorSpaceTemperature)
{
    double QEr = 2.52683f;
    double QEb = 1.60813f;
    std::list<testpoint> points;
    points.push_back(testpoint({QEr/1.49505f, QEb/2.59104f, 0, 0, 2800}));
    points.push_back(testpoint({QEr/1.63682f, QEb/2.41592f, 0, 0, 3200}));
    points.push_back(testpoint({QEr/2.23547f, QEb/1.86531f, 0, 0, 5000}));
    points.push_back(testpoint({QEr/2.34968f, QEb/1.76885f, 0, 0, 5500}));
    points.push_back(testpoint({QEr/2.52683f, QEb/1.60813f, 0, 0, 6500}));
    points.push_back(testpoint({QEr/2.65856f, QEb/1.51728f, 0, 0, 7500}));

    initLines(points);

    const double xmin=0.9f;
    const double xmax=1.8f;
    const double ymin=0.4f;
    const double ymax=1.2f;

    double dx = (xmax-xmin)/40;
    double dy = (ymax-ymin)/40;
    for(double x=xmin;x<=xmax;x+=dx)
    {
        printf("%f;", x);
    }
    printf("\n");
    // step on the line
    double x, y=ymin;
    while(y<=ymax)
    {
        x = xmin;
        printf("%f;", y);
        while(x<=xmax)
        {
            printf("%f;", getCorrelatedTemperature_Calibrated(x, 1.0, y));
            x+=dx;
        }
        y+=dy;
        printf("\n");
    }
}


