/**
*******************************************************************************
 @file LineSegments.cpp

 @brief ISPC::TemperatureCorrection implementation

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
#include "ispc/LineSegments.h"
#include "ispc/ISPCDefs.h"
#include "img_sysdefs.h"

ISPC::LineSegments::const_iterator
ISPC::LineSegments::getClosestLine(
        const double ratioR, const double ratioB) const
{
    const_iterator iter = this->begin();
    const_iterator line = this->end();
    double minDistance = std::numeric_limits<double>::max();
    double distance;

    // find the line which is the closest to the given point
    for(;iter!=this->end();++iter)
    {
        distance = iter->getDistancePow2(ratioR, ratioB);
        if(distance<minDistance) {
            line = iter;
            minDistance = distance;
        }
    }
    return line;
}

bool ISPC::LineSegments::getRbProj(
        const double ratioR, const double ratioB,
        double& Xp, double& Yp) const
{
    if(this->empty())
    {
        return false;
    }
    getClosestLine(ratioR, ratioB)->getRbProj(ratioR, ratioB, Xp, Yp);
    return true;
}

double ISPC::LineSegments::getTemperature(
            const double ratioR, const double ratioB) const
{
    if(this->empty())
    {
        return 6500.0;
    }
    return getClosestLine(ratioR, ratioB)->getTemperature(ratioR, ratioB);
}

void ISPC::LineSegment::getRbProj(
        const double ratioR, const double ratioB,
        double& Xp, double& Yp) const
{
    const double b=x2-x1;
    const double c=y2-y1;
    const double a=ratioR*b+ratioB*c;
    const double d=y1*b-x1*c;

    Xp=(a*b-c*d)/(b*b+c*c);
    Yp=(d+Xp*c)/b;

    // bound the projected point to the line segment
    if(lowBound)
    {
        if((x2>=x1 && Xp<x1) || (x2<=x1 && Xp>x1))
        {
            Xp=x1;
        }
        if((y2>=y1 && Yp<y1) || (y2<=y1 && Yp>y1))
        {
            Yp=y1;
        }
    }
    if(highBound)
    {
        if((x2>=x1 && Xp>x2) || (x2<=x1 && Xp<x2))
        {
            Xp=x2;
        }
        if((y2>=y1 && Yp>y2) || (y2<=y1 && Yp<y2))
        {
            Yp=y2;
        }
    }
}

ISPC::LineSegment::LineSegment(
        double _x1, double _y1,
        double _x2, double _y2,
        double _t1, double _t2) :
        x1(_x1), y1(_y1), t1(_t1),
        x2(_x2), y2(_y2),
        lowBound(true),
        highBound(true),
        length(0),
        temperatureDensity(0)
{
    lengthPow2 = getLengthPow2(x1, y1, x2, y2);
    length = std::sqrt(lengthPow2);
    temperatureDensity = calcTempDensity(_t1, _t2);
}

double ISPC::LineSegment::getLengthPow2(
            double x1, double y1, double x2, double y2)
{
    return (x2-x1)*(x2-x1)+(y2-y1)*(y2-y1);
}

double ISPC::LineSegment::getLength(
            double x1, double y1, double x2, double y2)
{
    return std::sqrt(getLengthPow2(x1, y1, x2, y2));
}

double ISPC::LineSegment::getDistancePow2(
                const double ratioR, const double ratioB) const
{
    if(x2==x1 && y2==y1) {
        return getLengthPow2(x1, y1, ratioR, ratioB);
    }
    double Xp, Yp;
    getRbProj(ratioR, ratioB, Xp, Yp);
    return getLengthPow2(ratioR, ratioB, Xp, Yp);
}

double ISPC::LineSegment::getTemperature(
        const double ratioR, const double ratioB) const
{
    double Xp, Yp;
    getRbProj(ratioR, ratioB, Xp, Yp);

    // determine the relative position of Xp to p1
    // it's important because we need to extrapolate in both directions
    const double sx = (x2-x1)*(x1-Xp);
    const double sy = (y2-y1)*(y1-Yp);
    // check they have the same sign, they should
    // as p1, p2 and Xp,Yp lie on the same line
    IMG_ASSERT(sx*sy>=0);
    const double sign = sx>=0 ? -1.0 : 1.0;
    double temp = t1+sign*getLength(x1, y1, Xp, Yp)*temperatureDensity;
    return temp;
}

