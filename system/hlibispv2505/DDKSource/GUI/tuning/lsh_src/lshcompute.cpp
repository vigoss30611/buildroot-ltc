/**
*******************************************************************************
@file lshcompute.cpp

@brief LSHCompute implementation

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
#include "lsh/compute.hpp"

#if WIN32
#define _USE_MATH_DEFINES
#include <math.h>  // for some reason VS13 does not define M_PI with cmath
#else
#include <cmath>
#endif
#include <fstream>
/* runtime_error (inherit exception and has a constructor with access to
 * the what message) */
#include <stdexcept>
#include <ctime>
#include <string>
#include <algorithm>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

#include "lsh/tuning.hpp"
#include "imageUtils.h"  // NO_LINT
#include "mc/module_config.h"
#include "felixcommon/lshgrid.h"

#include <ctx_reg_precisions.h>  // NO_LINT
#include <QtCore/QThread>  // NO_LINT
#include <opencv2/core/core.hpp>

// comment tu use createGridDirect
// otherwise uses createGridDirectTile (supports margins)
#define DIRECT_TILE
/* define only 1: choose which method direct tile uses,
 * if none are defined will use the tile corners values like createGridDirect
 */
// #define DIRECT_USE_AVG
// #define DIRECT_USE_MIN
// #define DIRECT_USE_MAX

// can define to compute the error - disabled because needs validation
// #define COMPUTE_ERROR

/*
 * local functions
 */

/**
 * Create a deshading coefficients grid by approximating the 2D curve in the
 * flatField plane
 */
static double createGridFit(cv::Mat flatField, cv::Mat &gridImage,
    cv::Mat &targetGrid, int gridStep, int tileSize, const int *margins,
    int blackLevel)
{
    cv::Mat samples;
    cv::Mat targets;

    cv::Mat field = flatField.clone() - blackLevel;

    double minVal, maxVal;
    cv::minMaxLoc(field, &minVal, &maxVal);
    field = maxVal / field;
    field = cv::max(field, 0.0);
    field = cv::min(field, 32.0);


    // generate samples and targets with a grid over the flatField
    for (int row = margins[MARG_T];
        row < (flatField.rows - margins[MARG_B]);
        row += gridStep)
    {
        for (int col = margins[MARG_L];
            col < (flatField.cols - margins[MARG_R]);
            col += gridStep)
        {
            cv::Mat sample;
            float cRow = (row
                - flatField.rows / 2.0) / flatField.rows;
            float cCol = (col
                - flatField.cols / 2.0) / flatField.cols;
#ifdef USE_MATH_NEON
            float cosRow = static_cast<float>(cosf_neon((float)cRow*M_PI));
            float cosCol = static_cast<float>(cosf_neon((float)cCol*M_PI));
            float sinRow = static_cast<float>(sinf_neon((float)cRow*M_PI));
            float sinCol = static_cast<float>(sinf_neon((float)cCol*M_PI));
#else
            float cosRow = static_cast<float>(cos(cRow*M_PI));
            float cosCol = static_cast<float>(cos(cCol*M_PI));
            float sinRow = static_cast<float>(sin(cRow*M_PI));
            float sinCol = static_cast<float>(sin(cCol*M_PI));
#endif

            sample.push_back<float>(1);
            sample.push_back<float>(cRow);
            sample.push_back<float>(cCol);

            sample.push_back<float>(cRow*cRow);
            sample.push_back<float>(cRow*cCol);
            sample.push_back<float>(cCol*cCol);

            sample.push_back<float>(cRow*cRow*cRow);
            sample.push_back<float>(cRow*cRow*cCol);
            sample.push_back<float>(cRow*cCol*cCol);
            sample.push_back<float>(cCol*cCol*cCol);

            sample.push_back<float>(cCol*cCol*cCol*cCol);
            sample.push_back<float>(cCol*cCol*cCol*cRow);
            sample.push_back<float>(cCol*cCol*cRow*cRow);
            sample.push_back<float>(cCol*cRow*cRow*cRow);
            sample.push_back<float>(cRow*cRow*cRow*cRow);

            sample.push_back<float>(cCol*cCol*cCol*cCol*cCol);
            sample.push_back<float>(cCol*cCol*cCol*cCol*cRow);
            sample.push_back<float>(cCol*cCol*cCol*cRow*cRow);
            sample.push_back<float>(cCol*cCol*cRow*cRow*cRow);
            sample.push_back<float>(cCol*cRow*cRow*cRow*cRow);
            sample.push_back<float>(cRow*cRow*cRow*cRow*cRow);

            sample.push_back<float>(cosRow);
            sample.push_back<float>(cosCol);

            sample.push_back<float>(cosRow*cosRow);
            sample.push_back<float>(cosCol*cosCol);
            sample.push_back<float>(cosCol*cosRow);

            sample = sample.t();
            samples.push_back(sample);
            targets.push_back(M_MAX(0, field.at<float>(row, col)));
        }
    }

    cv::Mat solution;
    // compute solution
    cv::solve(samples, targets, solution, cv::DECOMP_NORMAL);

    cv::Mat output = samples*solution;

    cv::Mat squaredErrors;
    cv::pow(output - targets, 2.0, squaredErrors);
    double averageError = cv::mean(squaredErrors)(0);

    /* generate samples and targets with a grid over the flatField (and
    * with the needed outside positions) */
    int columnTiles = static_cast<int>(
        ceil(static_cast<float>(flatField.cols) / tileSize)) + 1;
    int rowTiles = static_cast<int>(
        ceil(static_cast<float>(flatField.rows) / tileSize)) + 1;

    // cv::Mat gridImage(rowTiles,columnTiles, CV_32F);
    gridImage = cv::Mat(rowTiles, columnTiles, CV_32F);
    targetGrid = cv::Mat(rowTiles - 1, columnTiles - 1, CV_32F);
    for (int rowIdx = 0; rowIdx < rowTiles; rowIdx++)
    {
        for (int colIdx = 0; colIdx < columnTiles; colIdx++)
        {
            cv::Mat sample;

            float cRow = (static_cast<float>(rowIdx*tileSize)
                - static_cast<float>(flatField.rows) / 2.0)
                / static_cast<float>(flatField.rows);
            float cCol = (static_cast<float>(colIdx*tileSize)
                - static_cast<float>(flatField.cols) / 2.0)
                / static_cast<float>(flatField.cols);
#ifdef USE_MATH_NEON
            float cosRow = static_cast<float>(cosf_neon((float)cRow*M_PI));
            float cosCol = static_cast<float>(cosf_neon((float)cCol*M_PI));
            float sinRow = static_cast<float>(sinf_neon((float)cRow*M_PI));
            float sinCol = static_cast<float>(sinf_neon((float)cCol*M_PI));
#else
            float cosRow = static_cast<float>(cos(cRow*M_PI));
            float cosCol = static_cast<float>(cos(cCol*M_PI));
            float sinRow = static_cast<float>(sin(cRow*M_PI));
            float sinCol = static_cast<float>(sin(cCol*M_PI));
#endif

            sample.push_back<float>(1);
            sample.push_back<float>(cRow);
            sample.push_back<float>(cCol);

            sample.push_back<float>(cRow*cRow);
            sample.push_back<float>(cRow*cCol);
            sample.push_back<float>(cCol*cCol);

            sample.push_back<float>(cRow*cRow*cRow);
            sample.push_back<float>(cRow*cRow*cCol);
            sample.push_back<float>(cRow*cCol*cCol);
            sample.push_back<float>(cCol*cCol*cCol);

            sample.push_back<float>(cCol*cCol*cCol*cCol);
            sample.push_back<float>(cCol*cCol*cCol*cRow);
            sample.push_back<float>(cCol*cCol*cRow*cRow);
            sample.push_back<float>(cCol*cRow*cRow*cRow);
            sample.push_back<float>(cRow*cRow*cRow*cRow);

            sample.push_back<float>(cCol*cCol*cCol*cCol*cCol);
            sample.push_back<float>(cCol*cCol*cCol*cCol*cRow);
            sample.push_back<float>(cCol*cCol*cCol*cRow*cRow);
            sample.push_back<float>(cCol*cCol*cRow*cRow*cRow);
            sample.push_back<float>(cCol*cRow*cRow*cRow*cRow);
            sample.push_back<float>(cRow*cRow*cRow*cRow*cRow);

            sample.push_back<float>(cosRow);
            sample.push_back<float>(cosCol);

            sample.push_back<float>(cosRow*cosRow);
            sample.push_back<float>(cosCol*cosCol);
            sample.push_back<float>(cosCol*cosRow);

            cv::Mat generated = sample.t()*solution;
            gridImage.at<float>(rowIdx, colIdx) = generated.at<float>(0, 0);
            if (rowIdx < rowTiles - 1 && colIdx < columnTiles - 1)
            {
                targetGrid.at<float>(rowIdx, colIdx) =
                    field.at<float>(rowIdx*tileSize, colIdx*tileSize);
            }
        }
    }


    cv::minMaxLoc(gridImage, &minVal, &maxVal);
    gridImage = gridImage / minVal;
    targetGrid = targetGrid / minVal;


    gridImage = cv::max(gridImage, 0.0);
    gridImage = cv::min(gridImage, 8.0);


    return averageError;
}

/**
 * Create a deshading gains grid from an flatfield image plane using the
 * points at the tiles corners
 */
static cv::Mat createGridDirect(const cv::Mat flatField, int gridStep,
    int tileSize, int blackLevel = 0)
{
    cv::Mat samples;
    cv::Mat targets;

    int columnTiles = static_cast<int>(
        ceil(static_cast<float>(flatField.cols) / tileSize)) + 1;
    int rowTiles = static_cast<int>(
        ceil(static_cast<float>(flatField.rows) / tileSize)) + 1;

    cv::Mat gridImage(rowTiles, columnTiles, CV_32F);
    for (int rowIdx = 0; rowIdx < rowTiles; rowIdx++)
    {
        for (int colIdx = 0; colIdx < columnTiles; colIdx++)
        {
            double cRow = rowIdx*tileSize;
            double cCol = colIdx*tileSize;
            gridImage.at<float>(rowIdx, colIdx) = flatField.at<float>(
                std::min(static_cast<int>(cRow), flatField.rows - 1),
                std::min(static_cast<int>(cCol), flatField.cols - 1))
                - blackLevel;
        }
    }

    double minVal, maxVal;
    cv::minMaxLoc(gridImage, &minVal, &maxVal);

    gridImage = maxVal / gridImage;
    gridImage = cv::max(gridImage, 0.0);
    // gridImage = cv::min(gridImage, 10.0);

    return gridImage;
}

/**
 * create a grid by taking the maximum value, applying black level
 * does the average of 1 tile to compute the needed coeff
 */
static cv::Mat createGridDirectTile(const cv::Mat flatField, int gridStep,
    int tileSize, int blackLevel, const int *margins, std::ostream &os)
{
    int columnTiles = static_cast<int>(
        ceil(static_cast<float>(flatField.cols) / tileSize)) + 1;
    int rowTiles = static_cast<int>(
        ceil(static_cast<float>(flatField.rows) / tileSize)) + 1;
    int tX, tY;
    int y;
    int x;
    double min_v, max_v;

    cv::Mat gridImage(rowTiles, columnTiles, CV_32F);
    cv::Mat field = flatField.clone() - blackLevel;

    cv::minMaxLoc(field, &min_v, &max_v);

#ifndef N_DEBUG
    os << "after black level of " << blackLevel
        << " min=" << min_v << " max=" << max_v << std::endl;
#endif

    if (min_v < 0)
    {
        os << "black-level " << blackLevel << " too big!";
        os << " Changed black level to " << ceil(blackLevel - min_v)
            << std::endl;

        field = flatField.clone() - (blackLevel - min_v);
    }

#if defined(DIRECT_USE_MIN)
    gridImage = 2 << 16;  // make a fake max of 16b
#else
    gridImage = 0.0;
#endif
#if !defined(DIRECT_USE_MIN) && !defined(DIRECT_USE_MAX) \
    && !defined(DIRECT_USE_AVG)
    const int y_p = tileSize;
#else
    const int y_p = 1;
#endif

    for (y = margins[1]; y < field.rows - margins[3]; y += y_p)
    {
        tY = y / tileSize;
#if defined(DIRECT_USE_AVG) || defined(DIRECT_USE_MIN) \
    || defined(DIRECT_USE_MAX)
        for (x = margins[0]; x < field.cols - margins[2]; x++)
        {
            float v = field.at<float>(y, x);

            tX = x / tileSize;

#if defined(DIRECT_USE_AVG)
            gridImage.at<float>(tY, tX) += v;
#elif defined(DIRECT_USE_MIN)
            gridImage.at<float>(tY, tX) =
                std::min(mav_v / v, gridImage.at<float>(tY, tX));
#elif defined(DIRECT_USE_MAX)
            gridImage.at<float>(tY, tX) =
                std::max(max_v / v, gridImage.at<float>(tY, tX));
#else
#error need an operation
#endif
        }
#else
        for (tX = margins[0] / tileSize; tX < flatField.cols / tileSize; tX++)
        {
            float v = field.at<float>(tY * tileSize, tX * tileSize);

            gridImage.at<float>(tY, tX) = max_v / (v);
        }
#endif
        // linear interpolation for right values
        // but interpolates the gain not the surface
        for (x = 0; tX + x < gridImage.cols; x++)
        {
            int x0 = tX + x - 2,
                x1 = tX + x - 1;
            double f0 = gridImage.at<float>(tY, x0),
                f1 = gridImage.at<float>(tY, x1);

            // linear intepolation from last gain values
            // /fmax to have it as value as if not interpolated
            gridImage.at<float>(tY, tX + x) =
                (f0 + ((f1 - f0) / (x1 - x0)) * ((tX + x) - x0));
        }
        // linear interpolation for left values
        for (tX = margins[0] / tileSize; tX > 0; tX--)
        {
            // means tX-1 needs interpolation
            int x0 = tX + 1,
                x1 = tX;
            double f0 = gridImage.at<float>(tY, x0),
                f1 = gridImage.at<float>(tY, x1);

            // linear intepolation from last values
            gridImage.at<float>(tY, tX - 1) =
                (f0 + ((f1 - f0) / (x1 - x0)) * ((tX - 1) - x0));
        }
    }
    // linear interpolation for bottom values would be better
    for (y = 0; tY + y < gridImage.rows; y++)
    {
        for (x = 0; x < gridImage.cols; x++)
        {
            gridImage.at<float>(tY + y, x) = gridImage.at<float>(tY, x);
        }
    }
    // linear interpolation for top values would be better
    for (tY = margins[1] / tileSize; tY > 0; tY--)
    {
        // means tY-1 needs interpolation
        for (x = 0; x < gridImage.cols; x++)
        {
            gridImage.at<float>(tY - 1, x) = gridImage.at<float>(tY, x);
        }
    }

#if defined(DIRECT_USE_AVG)
    // average of each tile
    gridImage /= tileSize*tileSize;
    // we know max of the channel, so we aim for (max-black)
    gridImage = max_v / gridImage;
#endif

    return gridImage;
}

/*
 * public
 */

LSHCompute::LSHCompute(QObject *parent)
    : ComputeBase("LSH: ", parent), inputs(HW_UNKNOWN)
{
}

LSHCompute::~LSHCompute()
{
    clearOutput();
}

void LSHCompute::clearOutput()
{
    std::map<LSH_TEMP, lsh_output*>::const_iterator it;
    for (it = output.begin(); it != output.end(); it++)
    {
        if (it->second)
        {
            delete it->second;
            output[it->first] = NULL;
        }
    }
    output.clear();
}

/*
 * public class methods
 */

void LSHCompute::writeLSHMatrix(const cv::Mat *matrices, const int tileSize,
    const std::string &filename, const bool saveAsTxt) throw(std::exception)
{
    LSH_GRID grid = LSH_GRID();
    IMG_RESULT ret;
    int channel;

#ifndef NDEBUG
    for (channel = 1; channel < 4; channel++)
    {
        if (matrices[channel].rows != matrices[0].rows
            || matrices[channel].cols != matrices[0].cols)
        {
            throw std::runtime_error(
                "input matrices should have the same size");
        }
    }
#endif

    ret = LSH_AllocateMatrix(&grid, matrices[0].cols, matrices[0].rows,
        tileSize);

    if (IMG_SUCCESS != ret)
    {
        throw std::runtime_error("failed to allocate matrix");
    }

    if (matrices[0].cols != grid.ui16Width
        || matrices[0].rows != grid.ui16Height)
    {
        LSH_Free(&grid);
        throw std::runtime_error("Configured matrix does not have correct sizes");
    }

    for (channel = 0; channel < 4; channel++)
    {
        for (int row = 0; row < grid.ui16Height; row++)
        {
            for (int col = 0; col < grid.ui16Width; col++)
            {
                float floatValue = matrices[channel].at<float>(row, col);

                grid.apMatrix[channel][row * grid.ui16Width + col] =
                    static_cast<LSH_FLOAT>(floatValue);
            }
        }
    }

    if (saveAsTxt)
    {
        ret = LSH_Save_txt(&grid, filename.c_str());
    }
    else
    {
        ret = LSH_Save_bin(&grid, filename.c_str());
    }
    LSH_Free(&grid);
    if (IMG_SUCCESS != ret)
    {
        throw std::runtime_error(std::string("failed to save matrix to '")
            + filename + "'");
    }
}

void LSHCompute::smoothMatrix(const cv::Mat &input, cv::Mat &output,
    const double smoothing, const int smoothsize)
{
    cv::Size smoothSize(smoothsize, smoothsize);

    cv::GaussianBlur(input, output, smoothSize, smoothing,
        smoothing, cv::BORDER_REPLICATE);
}

/*
 * protected
 */

void LSHCompute::calculateLSHFit(const lsh_input::input_info &info,
    lsh_output *pOutput)
{
    std::ostringstream os;

    cv::Mat solution;

    os.str("");
    os << prefix << "Calculating fitted curve LSH coefficients "
        << "with tile size " << inputs.tileSize << " for temperature "
        << pOutput->temperature << std::endl;
    emit logMessage(QString::fromStdString(os.str()));

    for (int c = 0; c < 4; c++)  // for each channel
    {
        smoothMatrix(pOutput->inputImage[c], pOutput->smoothedImage[c],
            info.smoothing, 31);
        pOutput->fittingMSE[c] = createGridFit(pOutput->smoothedImage[c],
            pOutput->channels[c],
            pOutput->targetLSH[c],
            info.granularity, inputs.tileSize, info.margins,
            inputs.blackLevel[c] << 1);

        os.str("");
        os << prefix << "input for temp " << pOutput->temperature
            << " channel " << c << " completed" << std::endl;
        emit logMessage(QString::fromStdString(os.str()));
        emit currentStep(++this->step);  // max is LSH_N_STEPS
    }
}

void LSHCompute::calculateLSHDirect(const lsh_input::input_info &info,
    lsh_output *pOutput)
{
    cv::Mat solution;
    std::ostringstream os;

    os.str("");
    os << prefix << "Calculating direct flat field LSH coefficients "
        << "with tile size " << inputs.tileSize << " for temperature "
        << pOutput->temperature << std::endl;
    emit logMessage(QString::fromStdString(os.str()));

    for (int c = 0; c < 4; c++)
    {
        smoothMatrix(pOutput->inputImage[c], pOutput->smoothedImage[c],
            info.smoothing, 31);

        os.str("");
#if defined(DIRECT_TILE)
        pOutput->channels[c] = createGridDirectTile(pOutput->smoothedImage[c],
            info.granularity,
            inputs.tileSize, inputs.blackLevel[c] << 1, info.margins, os);
#else
        pOutput->channels[c] = createGridDirect(smooth, info.granularity,
            inputs.tileSize, inputs.blackLevel[c] << 1);
#endif
        if (!os.str().empty())
        {
            std::ostringstream val;
            val << prefix << "channel[" << c << "] " << os.str();
            emit logMessage(QString::fromStdString(val.str()));
        }

        os.str("");
        os << prefix << "channel " << c << " completed" << std::endl;
        emit logMessage(QString::fromStdString(os.str()));
        emit currentStep(++this->step);  // max is LSH_N_STEPS
    }
}

void LSHCompute::rescaleMatrix(const cv::Mat *input, cv::Mat *output,
    double rescale, const double *min_v, const double *max_v)
{
    for (int c = 0; c < 4; c++)
    {
        output[c] = cv::Mat(input[c].rows, input[c].cols, CV_32F);

        cv::MatConstIterator_<float> it = input[c].begin<float>();
        cv::MatIterator_<float> ot = output[c].begin<float>();

        while (it != input[c].end<float>())
        {
            *ot = (*it)*rescale;
            if (min_v)
            {
                *ot = std::max(static_cast<float>(*min_v), *ot);
            }
            if (max_v)
            {
                *ot = std::min(static_cast<float>(*max_v), *ot);
            }
            it++;
            ot++;
        }
    }
}

void LSHCompute::computeError(lsh_output *pOutput)
{
#ifdef COMPUTE_ERROR
    std::ostringstream os;
    int c;

    os.str("");
    os << prefix << "compute error with smoothed input "
        << pOutput->smoothedImage[0].rows
        << "x" << pOutput->smoothedImage[0].cols
        << " and channels " << pOutput->channels[0].rows
        << "x" << pOutput->channels[0].cols
        << " (tile " << inputs.tileSize << ")"
        << std::endl;
    emit logMessage(QString::fromStdString(os.str()));

    for (c = 0; c < 4; c++)
    {
        double fmin, fmax;
        double diff;
        double gain;
        int row, col;
        cv::minMaxLoc(pOutput->smoothedImage[c], &fmin, &fmax);

        pOutput->outputMSE[c] = cv::Mat(pOutput->smoothedImage[c].rows,
            pOutput->smoothedImage[c].cols, CV_32F);

        os.str("");
        os << prefix << "smoothed input channel " << c
            << " range [" << fmin << "; " << fmax << "] (BLC "
            << inputs.blackLevel[0] << ", "
            << inputs.blackLevel[1] << ", "
            << inputs.blackLevel[2] << ", "
            << inputs.blackLevel[3] << ")"
            << std::endl;
        emit logMessage(QString::fromStdString(os.str()));

        // set whole targetMSE to 0
        pOutput->outputMSE[c] = 0.0;

        for (row = 0; row < pOutput->outputMSE[c].rows; row++)
        {
            int tY = row / inputs.tileSize;
            int y1 = tY*inputs.tileSize;
            int y2 = (tY + 1)*inputs.tileSize;
            for (col = 0; col < pOutput->outputMSE[c].cols; col++)
            {
                int tX = col / inputs.tileSize;
                int x1 = tX*inputs.tileSize;
                int x2 = (tX + 1)*inputs.tileSize;

                {
                    // bilinear interpolation between
                    double f_y1 =
                        static_cast<double>(x2 - col) / inputs.tileSize
                        * pOutput->channels[c].at<float>(tY + 1, tX)
                        + static_cast<double>(col - x1) / inputs.tileSize
                        * pOutput->channels[c].at<float>(tY + 1, tX + 1);
                    double f_y2 =
                        static_cast<double>(x2 - col) / inputs.tileSize
                        * pOutput->channels[c].at<float>(tY, tX)
                        + static_cast<double>(col - x1) / inputs.tileSize
                        * pOutput->channels[c].at<float>(tY, tX + 1);

                    gain = static_cast<double>(y2 - row) / inputs.tileSize
                        * f_y1
                        + static_cast<double>(row - y1) / inputs.tileSize
                        * f_y2;
                }

                diff = (fmax - inputs.blackLevel[c]) - (gain
                    *pOutput->smoothedImage[c].at<float>(row, col));

                diff *= diff;

                pOutput->outputMSE[c].at<float>(row, col) = diff;
            }  // for col
        }  // for row

        emit currentStep(++this->step);  // max is LSH_N_STEPS
    }  // for chanel
#else
    this->step += 4;  // 1 per channel that would normally be computed
    emit currentStep(this->step);  // max is LSH_N_STEPS
#endif
}

double LSHCompute::getGainRescale(
    const std::map<LSH_TEMP, lsh_output*> &output) const
{
    std::map<LSH_TEMP, lsh_output*>::const_iterator i;
    int c;
    double res = 1.0;
    std::ostringstream os;

    for (i = output.begin(); i != output.end(); i++)
    {
        double fMatMax = 0.0;
        double factor;

        os.str("");
        for (c = 0; c < 4; c++)
        {
            double fmin, fmax;
            cv::minMaxLoc(i->second->channels[c], &fmin, &fmax);

            os.str("");
            os << prefix << "output for temp " << i->first
                << " channel[" << c << "] range from " << fmin << " to "
                << fmax << std::endl;
            emit logMessage(QString::fromStdString(os.str()));

            fMatMax = std::max(fmax, fMatMax);
        }

        if (inputs.bUseMaxValue
            && inputs.maxMatrixValue < fMatMax)
        {
            factor = inputs.maxMatrixValue / fMatMax;
        }
        else
        {
            factor = 1.0;
        }
        res = std::min(res, factor);
    }
    return res;
}

unsigned int LSHCompute::getBitsPerDiff(const lsh_input *pInputs,
    const cv::Mat *channels, int minBits, int &maxDiff)
{
#define PRECISION(A) A->vertex_int, A->vertex_frac, A->vertex_signed, "vertex"
    int c, j;
    int bitsPerDiff = 0;

#if 0
    writeLSHMatrix(channels, pInputs->tileSize, "grids.txt", true);
#endif

    maxDiff = 0;
    for (c = 0; c < 4; c++)
    {
        for (j = 0; j < channels[c].rows; j++)
        {
            float current = channels[c].at<float>(j, 0);
            IMG_INT32 prev = IMG_Fix_Clip(current, PRECISION(pInputs));
            IMG_INT32 curr = 0;
            IMG_INT32 diff = 0;

            for (int i = 1; i < channels[c].cols; i++)
            {
                current = channels[c].at<float>(j, i);

                curr = IMG_Fix_Clip(current, PRECISION(pInputs));
                diff = curr - prev;

                if (diff < 0)
                {
                    diff = (diff*-1) - 1;
                }

                maxDiff = IMG_MAX_INT(diff, maxDiff);

                prev = curr;
            }
        }
    }

    // log2() for number of bits needed for difference
    // in MC we do log2 using while and need +1, here round up is done by ceil
    double nbits = ceil(log(maxDiff) / log(2.0));
    // diff bits + 1 because it's signed
    bitsPerDiff = static_cast<int>(nbits) + 1;
    bitsPerDiff = IMG_MAX_INT(minBits, bitsPerDiff);
    return bitsPerDiff;
#undef PRECISION
}

double LSHCompute::getDiffBitRescale(
    const std::map<LSH_TEMP, lsh_output*> &output,
    unsigned int &maxBitsPerDiff) const
{
    std::map<LSH_TEMP, lsh_output*>::const_iterator i;
    double res = 1.0;
    std::ostringstream os;
    maxBitsPerDiff = LSH_DELTA_BITS_MIN;
    int maxDiff = 0;

    for (i = output.begin(); i != output.end(); i++)
    {
        unsigned int bitsPerDiff = 0;
        int currMaxDiff = 0;

        bitsPerDiff = getBitsPerDiff(&inputs, i->second->channels,
            LSH_DELTA_BITS_MIN, currMaxDiff);

        maxBitsPerDiff = IMG_MAX_INT(maxBitsPerDiff, bitsPerDiff);
        maxDiff = IMG_MAX_INT(maxDiff, currMaxDiff);

        os.str("");
        os << prefix << "matrix for temperature " << i->first << " needs "
            << bitsPerDiff << " bits per diff to accomodate max absolute "
            << "difference of " << currMaxDiff << std::endl;

        emit logMessage(QString::fromStdString(os.str()));
    }

    if (inputs.bUseMaxBitsPerDiff
        && maxBitsPerDiff > inputs.maxBitDiff)
    {
        res = static_cast<LSH_FLOAT>((1 << (inputs.maxBitDiff - 1)) - 1)
            / maxDiff;
    }
    return res;
}

int LSHCompute::interpolateOutputs(std::map<LSH_TEMP, lsh_output*> &output)
{
    int width, height;
    std::map<LSH_TEMP, lsh_output*>::const_iterator cit;
    std::map<LSH_TEMP, lsh_output*>::iterator it;
    std::list<LSH_TEMP>::const_iterator ot;
    std::ostringstream os;
    int c;
    int toInterpolate = 0;

    if (!inputs.interpolate)
    {
        this->step += 4;

        os.str("");
        os << prefix << "interpolation step disabled" << std::endl;
        emit logMessage(QString::fromStdString(os.str()));

        emit currentStep(this->step);
        return EXIT_SUCCESS;
    }

    cit = output.begin();
    width = cit->second->channels[0].cols;
    height = cit->second->channels[0].rows;
    cit++;

    for (; cit != output.end(); cit++)
    {
        if (cit->second->channels[0].cols != width
            || cit->second->channels[0].rows != height)
        {
            // different input size - unlikely
            os.str("");
            os << prefix << "found difference sizes in input matrices "
                << "- cannnot interpolate" << std::endl;

            emit logMessage(QString::fromStdString(os.str()));
            return EXIT_FAILURE;
        }
    }

    for (ot = inputs.outputTemperatures.begin();
        ot != inputs.outputTemperatures.end(); ot++)
    {
        LSH_TEMP temp = *ot;
        cit = output.find(temp);
        if (output.end() == cit)
        {
            lsh_output *pnew = new lsh_output();
            pnew->interpolated = true;
            pnew->temperature = temp;
            pnew->rescaled = 1.0;
            pnew->filename = inputs.generateFilename(temp);
            toInterpolate++;

            os.str("");
            os << prefix << "will interpolate output for temperature "
                << temp << std::endl;
            emit logMessage(QString::fromStdString(os.str()));

            for (c = 0; c < 4; c++)
            {
                pnew->channels[c] = cv::Mat(height, width, CV_32F);
            }

            output[temp] = pnew;
        }
    }

    for (c = 0; c < 4; c++)
    {
        for (int tile = 0; tile < width*height && toInterpolate > 0; tile++)
        {
            // for each element create interpolation line
            float S = 0, Sx = 0, Sy = 0, Sxx = 0, Sxy = 0, Syy = 0;
            float Delta = 0, a = 0, b = 0;
            //            S = \sum_{i=1}^n 1 = n,
            //            S_x = \sum_{i=1}^n x_i,
            //            S_y = \sum_{i=1}^n y_i,
            //            S_{xx} = \sum_{i=1}^n x_i^2,
            //            S_{xy} = \sum_{i=1}^n x_i y_i,
            //            S_{yy} = \sum_{i=1}^n y_i^2,
            //            Delta = S * S_{xx} - (S_x)^2.

            //            a = (S * Sxy - Sx * Sy)/Delta
            //            b = (Sxx * Sy - Sx * Sxy)/Delta

            //            y = ax + b

            for (cit = output.begin(); cit != output.end(); cit++)
            {
                float t_v = cit->second->channels[c].at<float>(tile);
                if (!cit->second->interpolated)
                {
                    S += 1;
                    Sx += cit->first;
                    //Sy += *(ci->second.grid.apMatrix[ch] + tile);
                    Sy += t_v;
                    Sxx += (cit->first * cit->first);
                    //Sxy += (ci->first * (*(ci->second.grid.apMatrix[ch] + tile)));
                    Sxy += (cit->first * t_v);
                    //Syy += (*(ci->second.grid.apMatrix[ch] + tile)) * (*(ci->second.grid.apMatrix[ch] + tile));
                    Syy += t_v * t_v;
                    Delta = S * Sxx - Sx * Sx;
                }
            }

            a = (S * Sxy - Sx * Sy) / Delta;
            b = (Sxx * Sy - Sx * Sxy) / Delta;

            for (it = output.begin(); it != output.end(); it++)
            {
                if (it->second->interpolated)
                {
                    it->second->channels[c].at<float>(tile) = a * it->first + b;
                }
            }
        }  // for each tile

        emit currentStep(this->step++);
    }  // for each channel

    if (toInterpolate == 0)
    {
        os.str("");
        os << prefix << "interpolation step skipped (no new output "
            << "temperature)" << std::endl;
        emit logMessage(QString::fromStdString(os.str()));
    }

    // now we have to remove the elements from output that are NOT in the
    // list of output temperature
    // do a copy of the map...
    std::map<LSH_TEMP, lsh_output*> cleanOut = output;
    for (it = output.begin(); it != output.end(); it++)
    {
        ot = find(inputs.outputTemperatures.begin(),
            inputs.outputTemperatures.end(), it->first);
        if (inputs.outputTemperatures.end() == ot)
        {
            os.str("");
            os << prefix << "removes temperature " << it->first
                << " from the output as it was not in the list" << std::endl;
            emit logMessage(QString::fromStdString(os.str()));
            delete it->second;  // free the lsh_output as it won't be used
            cleanOut.erase(it->first);
        }
    }
    output.clear();
    output = cleanOut;
    return EXIT_SUCCESS;
}

void LSHCompute::rescaleOutputs(std::map<LSH_TEMP, lsh_output*> &output)
{
    std::map<LSH_TEMP, lsh_output*>::iterator i;
    int c;
    unsigned int maxDiffPerBit = 0;
    double factor;
    std::ostringstream os;

    os.str("");
    os << prefix << "verify output grids to ensure: " << std::endl;
    os << "    max gain ";
    if (inputs.bUseMaxValue)
    {
         os << inputs.maxMatrixValue;
    }
    else
    {
        os << "none";
    }
    os << std::endl
        << "    max bits per diff ";
    if (inputs.bUseMaxBitsPerDiff)
    {
        os << inputs.maxBitDiff;
    }
    else
    {
        os << "none";
    }
    os << std::endl
        << "    min gain ";
    if (inputs.bUseMinValue)
    {
        os << inputs.minMatrixValue;
    }
    else
    {
        os << "none";
    }
    os << std::endl;

    emit logMessage(QString::fromStdString(os.str()));

    factor = getGainRescale(output);

    os.str("");
    os << prefix << "rescaling all outputs and all channels by " << factor
        << " to enforce max gain supported by HW" << std::endl;
    emit logMessage(QString::fromStdString(os.str()));

    // then apply the factor to all output
    for (i = output.begin(); i != output.end(); i++)
    {
        if (factor < 1.0)
        {
            cv::Mat out[4];

            rescaleMatrix(i->second->channels, out, factor,
                inputs.bUseMinValue ? &(inputs.minMatrixValue) : NULL);

            for (c = 0; c < 4; c++)
            {
                // replace the output
                i->second->channels[c] = out[c];
            }
        }
        i->second->rescaled *= factor;

        emit currentStep(this->step++);
    }

    factor = getDiffBitRescale(output, maxDiffPerBit);

    while (factor < 1.0)
    {
        os.str("");
        os << prefix << "rescaling all outputs and all channels by " << factor
            << " to enforce max bits per diff" << std::endl;
        emit logMessage(QString::fromStdString(os.str()));

        // then apply the factor to all output
        for (i = output.begin(); i != output.end(); i++)
        {
            cv::Mat out[4];

            rescaleMatrix(i->second->channels, out, factor);

            for (c = 0; c < 4; c++)
            {
                // replace the output
                i->second->channels[c] = out[c];
            }
            i->second->rescaled *= factor;
        }

        // recompute the new bits per diff
        factor = getDiffBitRescale(output, maxDiffPerBit);
#ifndef N_DEBUG
        if (1.0 != factor)  // unlikely
        {
            os.str("");
            os << prefix << "WARNING: rescaling did not fix the bits per diff"
                << std::endl;
            emit logMessage(QString::fromStdString(os.str()));
        }
#endif
    }

    /* separate the saving of the bits per diff because it has to be the same
     * for all matrices */
    for (i = output.begin(); i != output.end(); i++)
    {
        i->second->bitsPerDiff = maxDiffPerBit;

        emit currentStep(this->step++);
    }
}

void LSHCompute::computeError(std::map<LSH_TEMP, lsh_output*> &output)
{
    std::ostringstream os;
    int c;
    int size;
    std::map<LSH_TEMP, lsh_output*>::iterator i;

    for (i = output.begin(); i != output.end(); i++)
    {
        lsh_output *pOutput = i->second;
        // bits per line (rounded up)
        size = (((pOutput->channels[0].cols - 1)*pOutput->bitsPerDiff + 7) / 8);
        size = size + 2;  // add initial 16b not compressed

        // round up size to multiple of 16 Bytes
        pOutput->lineSize = ((size + 15) / 16) * 16;

        os.str("");
        os << prefix << "expected LSH grid " << i->first << " line size is "
            << pOutput->lineSize << " Bytes" << std::endl;
        os << prefix << "expected LSH grid " << i->first << " total size is "
            << pOutput->lineSize*pOutput->channels[0].rows << " Bytes"
            << std::endl;
        emit logMessage(QString::fromStdString(os.str()));

        // copy the other parameters
        for (c = 0; c < 4; c++)
        {
            double minVal, maxVal;
            cv::minMaxLoc(pOutput->channels[c], &minVal, &maxVal);
            pOutput->normalised[c] =
                (pOutput->channels[c] - minVal) / (maxVal - minVal) / 1.25 + 0.1;
            pOutput->normalised[c].convertTo(pOutput->normalised[c], CV_8U, 255);
            cv::cvtColor(pOutput->normalised[c], pOutput->normalised[c],
                CV_GRAY2RGB);
        }
        emit currentStep(++this->step);  // max is LSH_N_STEPS

        computeError(pOutput);
    }
}

/*
 * public slots
 */

int LSHCompute::compute(const lsh_input *pInputs)
{
    cv::Mat flatField;  // input image
    std::ostringstream os;
    int c;

    clearOutput();
    time_t startTime = time(NULL);

    this->inputs = *pInputs;

    // applies operator< on all elements to sort
    this->inputs.outputTemperatures.sort();

    lsh_input::const_iterator it;
    step = 0;

    os.str("");
    os << prefix << inputs;
    emit logMessage(QString::fromStdString(os.str()));

    for (it = inputs.inputs.begin(); it != inputs.inputs.end(); it++)
    {
        try
        {
            flatField = loadBAYERFlxFile(it->second.imageFilename);
            os.str("");
            os << prefix << "Loaded flx image " << it->first << " '"
                << it->second.imageFilename << std::endl;
            emit logMessage(QString::fromStdString(os.str()));
        }
        catch (std::exception e)
        {
            os.str("");
            os << prefix << "ERROR loading flx image (" << it->first << " '"
                << it->second.imageFilename << "'): " << e.what() << std::endl;
            emit logMessage(QString::fromStdString(os.str()));

            clearOutput();
            emit finished(this);
            return -1;
        }

        emit currentStep(++this->step);  // max is LSH_N_STEPS

        lsh_output *pOutput = new lsh_output();
        pOutput->temperature = it->first;
        pOutput->rescaled = 1.0;
        pOutput->interpolated = false;
        pOutput->filename = inputs.generateFilename(it->first);

        output[pOutput->temperature] = pOutput;
        cv::split(flatField, pOutput->inputImage);

        for (c = 0; c < 4; c++)
        {
            // for the display
            pOutput->blackLevel[c] =
                pOutput->inputImage[c] - (inputs.blackLevel[c] << 1);
            pOutput->blackLevel[c] = cv::max(pOutput->blackLevel[c], 0.0);
        }

        if (inputs.fitting)  // generate grid based on curve fitting
        {
            calculateLSHFit(it->second, pOutput);
        }
        else  // generate curve based on subsampling
        {
#if !defined(DIRECT_TILE)
            if (inputs.margins[0] > 0 || inputs.margins[2] > 0
                || inputs.margins[1] > 0 || inputs.margins[3] > 0)
            {
                os.str("");
                os << prefix << "WARNING Margins can't be > 0 if no fitting "
                    << "algorithm is used. Setting all margins to 0." << std::endl;
                emit logMessage(QString::fromStdString(os.str()));
                for (int c = 0; c < 4; c++)
                {
                    inputs.margins[c] = 0;
                }
            }
#endif
            calculateLSHDirect(it->second, pOutput);
        }
    }  // for each input image

    if (interpolateOutputs(output) != EXIT_SUCCESS)
    {
        os.str("");
        os << prefix << " failed to interpolate matrices!" << std::endl;
        emit logMessage(QString::fromStdString(os.str()));
    }

    rescaleOutputs(output);

    computeError(output);

    startTime = time(0) - startTime;

    os.str("");
    os << prefix << "ran into " << static_cast<int>(startTime) << " sec"
        << std::endl;
    emit logMessage(QString::fromStdString(os.str()));

    emit finished(this);

    // assert(this->steps <= LSH_N_STEPS);
    return 0;
}
