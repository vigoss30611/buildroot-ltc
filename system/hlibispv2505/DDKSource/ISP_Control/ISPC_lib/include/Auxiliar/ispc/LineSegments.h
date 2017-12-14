/**
*******************************************************************************
 @file LineSegments.h

 @brief ISPC::LineSegments declaration

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
#ifndef ISPC_AUX_LINE_SEGMENTS_H_
#define ISPC_AUX_LINE_SEGMENTS_H_

#include <list>
#ifdef WIN32
#define NOMINMAX
#endif
#include <cmath>
#include <limits>

namespace ISPC {

/**
 *
 */
class LineSegment
{
public:
    LineSegment(double _x1, double _y1,
            double _x2, double _y2,
            double t1=0.0f, double t2=0.0f);

    double x1; // effectively it's low boundary
    double y1;
    double t1; // temperature at low boundary

    double x2;
    double y2; // effectively it's high boundary

    /**
     * @brief false if line segment extends from (x1, y1) to infinity
     *
     * @note enables extrapolation of temperature below t1
     */
    bool lowBound;
    /**
     * @brief false if line segment extends from (x2, y2) to infinity
     *
     * @note enables extrapolation of temperature above t2
     */

    bool highBound;

    /**
     * @brief length of current segment
     *
     * @note initialized in constructor
     */
    double length;

    /**
     * @brief squared length of current segment
     *
     * @note initialized in constructor
     */
    double lengthPow2;

    /**
     * @brief density of temperature for current line segment
     *
     * @note used in temperature interpolation
     */
    double temperatureDensity;

    /**
     * @brief calculate temperature density
     */
    double calcTempDensity(double t1, double t2) const
    {
        return (t2-t1)/length;
    }

    /**
     * @brief return squared length of line between (x1, y1) and (x2, y2)
     */
    static double getLengthPow2(
            double x1, double y1,
            double x2, double y2);

    /**
     * @brief return length of line between (x1, y1) and (x2, y2)
     */
    static double getLength(
            double x1, double y1,
            double x2, double y2);

    /**
     * @brief return length of current line segment
     */
    double getLength() const { return length; }

    /**
     * @brief return x and y of point (ratioR,ratioB)
     * projected on Planckian Locus approximation line
     *
     * @note the projected point is clipped to segment borders
     *
     * @note more on algebra used, please refer to
     *  http://cs.nyu.edu/~yap/classes/visual/03s/hw/h2/math.pdf
     */
    void getRbProj(
            const double ratioR, const double ratioB,
            double& Xp, double& Yp) const;

    /**
     * @brief calculate squared distance between given point and current line
     * segment between (x1,y1) and (x2, y2)
     *
     * @note If the projected point lies outside the segment, the returned
     * distance is between the point and nearest line endpoint
     *
     * @note more on algebra used, please refer to
     * http://mathworld.wolfram.com/Point-LineDistance2-Dimensional.html
     */
    double getDistancePow2(
            const double ratioR, const double ratioB) const;

    /**
     * @brief returns square root of getDistancePow2()
     */
    double getDistance(
            const double ratioR, const double ratioB) const {
        return std::sqrt(getDistancePow2(ratioR, ratioB));
    }

    /**
     * @brief calculate temperature correlated for ratioR,ratioB point
     *
     * @note approximates temperature between both line borders
     * @note if lowBound or highBound is set to false, then the temperature is
     * extrapolated when the ratioR,ratioB projects outside the line segment
     */
    double getTemperature(
            const double ratioR, const double ratioB) const;

private:
    /**
     * @brief default constructor, disabled
     */
    LineSegment() :
            x1(-std::numeric_limits<double>::infinity()),
            y1(std::numeric_limits<double>::infinity()),
            t1(0.0),
            x2(-x1),
            y2(-y1),
            lowBound(true), highBound(true),
            length(0.0), lengthPow2(0.0),
            temperatureDensity(0)
    {}
};

//typedef std::list<LineSegment> curveLines;

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
class LineSegments : public std::list<LineSegment>
{
public:
    /**
     * @brief return the iterator to the line segment closest to the given
     * point
     */
    const_iterator getClosestLine(
        const double ratioR, const double ratioB) const;

    /**
     * @brief return x and y of point (ratioR,ratioB)
     * projected on the closests Planckian Locus approximation line
     */
    bool getRbProj(
            const double ratioR, const double ratioB,
            double& Xp, double& Yp) const;

    /**
     * @brief calculate temperature correlated for ratioR,ratioB point
     * @note the temperature is approximated on the line segment closest to
     * the given point
     */
    double getTemperature(
            const double ratioR, const double ratioB) const;
};

} /* namespace ISPC */

#endif /* ISPC_AUX_TEMPERATURE_CORRECTION_H_ */
