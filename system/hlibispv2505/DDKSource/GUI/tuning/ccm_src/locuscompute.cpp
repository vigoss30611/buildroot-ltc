/**
******************************************************************************
@file ccmcompute.cpp

@brief CCMCompute class implementation

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
#include "ccm/locuscompute.hpp"
#include <vector>
#include <sstream>
#include <algorithm>

#include <QMessageBox>

#include "ispc/ISPCDefs.h"
#include "ispc/ParameterList.h"
#include "ispc/ModuleAWS.h"
#include "ispc/TemperatureCorrection.h"

LocusCompute::LocusCompute(QObject *parent):
    ComputeBase("PLocus: ", parent),
    fail(false)
{}

LocusCompute::~LocusCompute(){}

double LocusCompute::least_square(
    const std::vector<point> &points,
    int start_point, int end_point, double& alpha, double& beta)
{
    int N = end_point - start_point + 1;
    double sigma_x_i_square = 0;
    double sigma_x_i_y_i = 0;
    double n_x_mean = 0;
    double n_y_mean = 0;

    for (int i = start_point; i <= end_point; i++)
    {
        const point& p = points[i];
        n_x_mean += p.x;
        n_y_mean += p.y;
        sigma_x_i_square += p.x * p.x;
        sigma_x_i_y_i += p.x * p.y;
    }


    alpha = (N *sigma_x_i_y_i - n_x_mean * n_y_mean) / (N*sigma_x_i_square - n_x_mean * n_x_mean);
    beta = (n_y_mean - n_x_mean * alpha) / N;

    double cost = 0;
    double distance = 0;

    for (int i = start_point; i <= end_point; i++)
    {
        const point& p = points[i];
        distance = alpha * p.x - p.y + beta;
        cost += distance * distance;
    }
    return cost;
}

void LocusCompute::greedy_search_4_lines(
    const std::vector<point> &points,
    std::vector<line> &lines)
{
    int N = (int)points.size();

    for (int i = 0; i < N - 1; i++)
    {
        if (points[i + 1].x < points[i].x || points[i + 1].y> points[i].y)
        {
            statusStr << "ERROR: Calibration points provided are not "
                "genuine descending." << std::endl;
            return;
        }
    }

    double alpha = 0;
    double beta = 0;
    double cost = 0;
    int start_index = 0;
    int end_index = 0;

    line line1, line2, line3, line4;

    if (N == 4)
    {
        double least_cost = std::numeric_limits<double>::max();;
        int optimal_i = 0;
        int optimal_j = 0;

        //Find optimal i, j to define sets of points [0,i][i,j][j,N-1] that lines approximate better
        for (int i = 1; i < N - 2; i++)
        {
            for (int j = i + 1; j < N - 1; j++)
            {
                cost = least_square(points, 0, i, alpha, beta);
                cost += least_square(points, i, j, alpha, beta);
                cost += least_square(points, j, N - 1, alpha, beta);

                if (cost < least_cost)
                {
                    least_cost = cost;
                    optimal_i = i;
                    optimal_j = j;
                }
            }
        }

        //Fill in the lines
        least_square(points, 0, optimal_i, alpha, beta);
        line1.fill(points[0],            points[optimal_i],    alpha, beta);

        least_square(points, optimal_i, optimal_j, alpha, beta);
        line2.fill(points[optimal_i],    points[optimal_j],    alpha, beta);

        least_square(points, optimal_j, N - 1, alpha, beta);
        line3.fill(points[optimal_j],    points[N - 1],        alpha, beta);

        lines.insert(lines.end(), line1);
        lines.insert(lines.end(), line2);
        lines.insert(lines.end(), line3);
    }
    else if (N > 4)
    {
        double least_cost = std::numeric_limits<double>::max();;
        int optimal_i = 0;
        int optimal_j = 0;
        int optimal_k = 0;

        //Find optimal i, j to define sets of points [0,i][i,j][j,N-1] that lines approximate better
        for (int i = 1; i < N - 3; i++)
        {
            for (int j = i + 1; j < N - 2; j++)
            {
                for (int k = j + 1; k < N - 1; k++)
                {
                    cost = least_square(points, 0, i, alpha, beta);
                    cost += least_square(points, i, j, alpha, beta);
                    cost += least_square(points, j, k, alpha, beta);
                    cost += least_square(points, k, N - 1, alpha, beta);

                    if (cost < least_cost)
                    {
                        least_cost = cost;
                        optimal_i = i;
                        optimal_j = j;
                        optimal_k = k;
                    }
                }
            }
        }

        //Fill in the lines
        least_square(points, 0, optimal_i, alpha, beta);
        line1.fill(points[0], points[optimal_i], alpha, beta);

        least_square(points, optimal_i, optimal_j, alpha, beta);
        line2.fill(points[optimal_i], points[optimal_j], alpha, beta);

        least_square(points, optimal_j, optimal_k, alpha, beta);
        line3.fill(points[optimal_j], points[optimal_k], alpha, beta);

        least_square(points, optimal_k, N - 1, alpha, beta);
        line4.fill(points[optimal_k], points[N - 1], alpha, beta);

        lines.insert(lines.end(), line1);
        lines.insert(lines.end(), line2);
        lines.insert(lines.end(), line3);
        lines.insert(lines.end(), line4);
    }
    else
    {
        statusStr << "Error: At least 4 points are needed." << std::endl;
        return;
    }
}

void LocusCompute::balance_line_parameters(double& A, double &B, double&C)
{
    double distnace_balance = 1.0f / sqrt(A * A + B * B);
    A *= distnace_balance;
    B *= distnace_balance;
    C *= distnace_balance;
}

bool LocusCompute::calibrate_sensor_params(
    const std::vector<point> &points,
    ISPC::ParameterList& params)
{
    double A, B, C;
    std::vector<line> lines;

    greedy_search_4_lines(points, lines);
    if(lines.size() < 3)
    {
        return false;
    }
    const double bbDistance = params.getParameter(
        ISPC::ModuleAWS::AWS_BB_DIST);

    //Unless we have measurement that defines max log(B/G) gain,
    // it assumed to be constant.
    double boundary = 0.25;

    ISPC::Parameter boundaries(ISPC::ModuleAWS::AWS_CURVE_BOUNDARIES.name);
    ISPC::Parameter xCoeffs(ISPC::ModuleAWS::AWS_CURVE_X_COEFFS.name);
    ISPC::Parameter yCoeffs(ISPC::ModuleAWS::AWS_CURVE_Y_COEFFS.name);
    ISPC::Parameter offsets(ISPC::ModuleAWS::AWS_CURVE_OFFSETS.name);

    // fill the output with calculated coefficients
    for(std::vector<line>::iterator curline = lines.begin();
        curline!=lines.end();
        ++curline)
    {
        A = -1.0 * curline->alpha;
        B = 1.0;
        C = -1.0 * curline->beta;
        balance_line_parameters(A, B, C);

        boundaries.addValue(ISPC::toString(boundary));
        xCoeffs.addValue(ISPC::toString(A));
        yCoeffs.addValue(ISPC::toString(B));
        offsets.addValue(ISPC::toString(C));
        boundary = curline->end.y;
    }

    // pad the outstanding lines with such setting that
    // the lines behave as a HW boundary
    for(int i=lines.size();i<5;++i)
    {
        boundaries.addValue(ISPC::toString(points.back().y - bbDistance));
        xCoeffs.addValue(ISPC::toString(0));
        yCoeffs.addValue(ISPC::toString(1));
        offsets.addValue(ISPC::toString(-1.0 * bbDistance));
    }

    params.addParameter(boundaries);
    params.addParameter(xCoeffs);
    params.addParameter(yCoeffs);
    params.addParameter(offsets);
    // always output 5 parameters
    params.addParameter(ISPC::ModuleAWS::AWS_CURVES_NUM, 5, true);
    return true;
}

namespace {

/**
 * @brief compare the points for sorting purpose
 */
bool comparePoint(
    const ::point &a,
    const ::point &b)
{
    return a.y == b.y ?
        a.x < b.x :
        a.y > b.y;
}

}; // namespace

int LocusCompute::compute()
{
    fail = false;
    statusStr.str(std::string());

    std::vector<point> inpoints;
    double QEr=1.0;
    double QEb=1.0;
    bool d65Found = false;

    // firstly remove the parameters so in case of calculation has failed,
    // no parameters are saved
    params.removeParameter(ISPC::ModuleAWS::AWS_CURVE_BOUNDARIES.name);
    params.removeParameter(ISPC::ModuleAWS::AWS_CURVE_X_COEFFS.name);
    params.removeParameter(ISPC::ModuleAWS::AWS_CURVE_Y_COEFFS.name);
    params.removeParameter(ISPC::ModuleAWS::AWS_CURVE_OFFSETS.name);
    params.removeParameter(ISPC::ModuleAWS::AWS_CURVES_NUM.name);

    int ccms =
        params.getParameter(ISPC::TemperatureCorrection::WB_CORRECTIONS);
    if(ccms < 4)
    {
        statusStr << "At least 4 CCM points "
            "shall be calibrated to correctly approximate Planckian Locus" <<
            std::endl;
        fail = true;
    } else {
        for(int i=0;i<ccms;++i)
        {
            double temperature = params.getParameter(
                    ISPC::TemperatureCorrection::WB_TEMPERATURE_S.indexed(i));

            double R = params.getParameter(
                ISPC::TemperatureCorrection::WB_GAINS_S.indexed(i), 0);
            double B = params.getParameter(
                ISPC::TemperatureCorrection::WB_GAINS_S.indexed(i), 3);

            inpoints.push_back(point(ISPC::log2(R), ISPC::log2(B)));

            if(6500.0 == temperature)
            {
                std::stringstream os;
                if(true == d65Found)
                {
                    statusStr << "Multiple D65 CCM calibration points"
                        << std::endl;
                    fail = true;
                }
                d65Found = true;
                QEr = ISPC::log2(R);
                QEb = ISPC::log2(B);
                os << "Found d65 calibration (" << QEr << ", " << QEb << ")"
                    << std::endl;
                emit logMessage(QString::fromStdString(os.str()));
            }
        }
        if(!d65Found)
        {
            statusStr << "No D65 CCM calibration point. AWB algorithm "
                "is not expected to work properly" << std::endl;
            fail = true;
        }
        for(std::vector<point>::iterator i = inpoints.begin();
            i!=inpoints.end();++i)
        {
            i->x = QEr-i->x;
            i->y = QEb-i->y;
        }
        // sort the points for calibrate_sensor_params()
        std::sort(inpoints.begin(), inpoints.end(), comparePoint);
        if(!calibrate_sensor_params(inpoints, params))
        {
            fail = true;
        }
    }

    // store detected parameters
    params.addParameter(ISPC::ModuleAWS::AWS_LOG2_R_QEFF, QEr, true);
    params.addParameter(ISPC::ModuleAWS::AWS_LOG2_B_QEFF, QEb, true);

    if(statusStr.str().length() > 0)
    {
        emit logMessage(QString::fromStdString(statusStr.str()));
    }

    emit finished(this);

    return 0;
}

ISPC::ParameterList& LocusCompute::getParams()
{
    return params;
}

void LocusCompute::setParams(ISPC::ParameterList& _params)
{
    params.clearList();
    params = _params;
}

bool LocusCompute::hasFailed() const
{
    return fail;
}

const std::string LocusCompute::status() const
{
    return statusStr.str();
}
