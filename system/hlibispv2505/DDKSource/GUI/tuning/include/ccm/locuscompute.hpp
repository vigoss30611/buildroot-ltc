/**
******************************************************************************
@file ccm/locuscompute.hpp

@brief Compute Planckian Locus approximation based on given input in a
separate QThread

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
#ifndef LOCUSCOMPUTE_HPP
#define LOCUSCOMPUTE_HPP

#include <QtCore/QObject>

#include "computebase.hpp"
#include "ispc/ParameterList.h"


/**
 * @brief structure defining a calibration point on log2(R)-log2(B) plane
 */
struct point
{
    point(double _x = 0, double _y = 0) :x(_x), y(_y) {}
    double x;
    double y;
};

/**
 * @brief class purposed for computation of Planckian Locus linear approximation
 */
class LocusCompute: public ComputeBase
{
    Q_OBJECT
public:
    /**
     * @brief constructor, requires input parameters for processing
     */
    explicit LocusCompute(QObject *parent = 0);
    virtual ~LocusCompute();

    /**
     * @brief getter of processing status log
     */
    const std::string status() const;
    bool hasFailed() const;

    void setParams(ISPC::ParameterList& _params);
    ISPC::ParameterList& getParams();

protected:
    /**
     * @brief structure defining line segment connecting two 'struct point' points
     */
    struct line
    {
        line(void) {}

        point start;    //First point that line approximates
        point end;      //Last point that line approximates
        double alpha;    //alpha parameter of y = alpha * x + beta for the line approximation
        double beta;     //beta  parameter of y = alpha * x + beta for the line approximation

        void fill(point _start, point _end, double _alpha, double _beta)
        {
            start = _start;
            end = _end;
            alpha = _alpha;
            beta = _beta;
        }
    };

    /**
     * @brief Gets a set of points and returns alpha and beta for the line
     * y = a * x + beta
     * @return the cost of the approximation. A least square approach is used
     */
    double least_square(
        const std::vector<point> &points,
        int start_point,
        int end_point,
        double& alpha, double& beta);

    /**
     * @brief search the optimal combination of lines to minimize approximation
     * error for the given points
     *
     * @note a brute force greedy approach so it may be SLOW for large input
     * points. Part of the calibration phase so speed is irrelevant
     *
     * @note Points assumed to be ordered to get meaningful results.
     */
    void greedy_search_4_lines(
        const std::vector<point> &points,
        std::vector<line> &lines);

    /**
     * @brief Balances line parameters of line Ax+By+C = 0 so that
     * sqrt(A^2+B^2) = 1024
     */
    void balance_line_parameters(double& A, double &B, double&C);

    /**
     * @brief Generates AWS planckian locus curve approximation segments
     * from given points. fills in the params
     *
     * @note Assumes points to be ordered from min-max on x or y as sensor response seems monotonic)
     */
    bool calibrate_sensor_params(
        const std::vector<point> &points,
        ISPC::ParameterList& params);

public slots:
    int compute();

signals:
    /**
     * @brief signals computation finish, gives back updated paramaters and status code
     */
    void finished(LocusCompute* ptr);

private:
    ISPC::ParameterList params;

    /**
     * @brief collects status logs from compute() thread
     */
    std::stringstream statusStr;

    bool fail;
};

#endif //CCMCOMPUTE_HPP
