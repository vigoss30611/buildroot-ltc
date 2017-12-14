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

#include <ctx_reg_precisions.h>  // NO_LINT
#include <QtCore/QThread>  // NO_LINT

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
    cv::Mat &targetGrid, int gridStep, int tileSize, int *margins,
    int blackLevel = 0)
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
static cv::Mat createGridDirect(cv::Mat flatField, int gridStep,
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
static cv::Mat createGridDirectTile(cv::Mat flatField, int gridStep,
    int tileSize, int blackLevel, int *margins)
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

    cv::minMaxLoc(flatField, &min_v, &max_v);

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

    for (y = margins[1]; y < flatField.rows - margins[3]; y += y_p)
    {
        tY = y / tileSize;
#if defined(DIRECT_USE_AVG) || defined(DIRECT_USE_MIN) \
    || defined(DIRECT_USE_MAX)
        for (x = margins[0]; x < flatField.cols - margins[2]; x++)
        {
            float v = flatField.at<float>(y, x) - blackLevel;

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
            float v = flatField.at<float>(tY * tileSize, tX * tileSize);

            gridImage.at<float>(tY, tX) = max_v / (v - blackLevel);
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

LSHCompute::LSHCompute(QObject *parent) : ComputeBase("LSH: ", parent)
{
}

/*
 * public class methods
 */

void LSHCompute::writeLSHMatrix(cv::Mat(&matrix)[4], int tileSize,
    const std::string &filename) throw(std::exception)
{
    std::ofstream oFile(filename.c_str(), std::ios::binary);

    if (!oFile.is_open())
    {
        throw std::runtime_error(std::string("Error opening file in ")
            + __FUNCTION__);
    }

    oFile.write("LSH\0", 4 * sizeof(char));

    int integerValue;
    integerValue = 1;
    oFile.write(reinterpret_cast<char *>(&integerValue), sizeof(int));
    integerValue = matrix[0].cols;  // write width of the grid
    oFile.write(reinterpret_cast<char *>(&integerValue), sizeof(int));
    integerValue = matrix[0].rows;   // write height of the grid
    oFile.write(reinterpret_cast<char *>(&integerValue), sizeof(int));
    integerValue = tileSize;  // write tilesize
    oFile.write(reinterpret_cast<char *>(&integerValue), sizeof(int));

    // write matrix coefficientes (32bit float)
    for (int channel = 0; channel < 4; channel++)
    {
        for (int row = 0; row < matrix[channel].rows; row++)
        {
            for (int col = 0; col < matrix[channel].cols; col++)
            {
                float floatValue = matrix[channel].at<float>(row, col);
                oFile.write(reinterpret_cast<char *>(&floatValue),
                    sizeof(float));
            }
        }
    }

    oFile.close();
}

/*
 * protected
 */

void LSHCompute::calculateLSHFit(const cv::Mat &flatFieldRGGB,
    cv::Mat(&lshGrids)[4], cv::Mat lshTargets[4], double mse[4],
    cv::Mat(&inputSmoothed)[4])
{
    cv::Mat separatedChannels[4];
    cv::split(flatFieldRGGB, separatedChannels);

    cv::Mat solution;
    cv::Mat smooth;
    int smoothSize = 31;
    std::ostringstream os;

    os.str("");
    os << prefix << "Calculating fitted curve LSH coefficients "\
        "with tile size " << inputs.tileSize
        << std::endl;
    emit logMessage(QString::fromStdString(os.str()));

    for (int c = 0; c < 4; c++)  // for each channel
    {
        cv::GaussianBlur(separatedChannels[c], smooth,
            cv::Size(smoothSize, smoothSize), inputs.smoothing,
            inputs.smoothing, cv::BORDER_REPLICATE);
        inputSmoothed[c] = smooth;
        mse[c] = createGridFit(smooth, lshGrids[c], lshTargets[c],
            inputs.granularity, inputs.tileSize, inputs.margins,
            inputs.blackLevel[c] << 1);

        os.str("");
        os << prefix << "channel " << c << " completed" << std::endl;
        emit logMessage(QString::fromStdString(os.str()));
        emit currentStep(++this->step);  // max is LSH_N_STEPS
    }
}

void LSHCompute::calculateLSHDirect(const cv::Mat &flatFieldRGGB,
    cv::Mat(&lshGrids)[4], cv::Mat(&inputSmoothed)[4])
{
    cv::Mat separatedChannels[4];
    cv::split(flatFieldRGGB, separatedChannels);

    cv::Mat solution;
    cv::Mat smooth;
    int smoothSize = 31;
    std::ostringstream os;

    os.str("");
    os << prefix << "Calculating direct flat field LSH coefficients " \
        "with tile size " << inputs.tileSize
        << std::endl;
    emit logMessage(QString::fromStdString(os.str()));

    for (int c = 0; c < 4; c++)
    {
        cv::GaussianBlur(separatedChannels[c], smooth,
            cv::Size(smoothSize, smoothSize), inputs.smoothing,
            inputs.smoothing, cv::BORDER_REPLICATE);
        inputSmoothed[c] = smooth;
#if defined(DIRECT_TILE)
        lshGrids[c] = createGridDirectTile(smooth, inputs.granularity,
            inputs.tileSize, inputs.blackLevel[c] << 1, inputs.margins);
#else
        lshGrids[c] = createGridDirect(smooth, inputs.granularity,
            inputs.tileSize, inputs.blackLevel[c] << 1);
#endif

        os.str("");
        os << prefix << "channel " << c << " completed" << std::endl;
        emit logMessage(QString::fromStdString(os.str()));
        emit currentStep(++this->step);  // max is LSH_N_STEPS
    }
}

void LSHCompute::rescaleMatrix(const cv::Mat *input, cv::Mat *output,
    double rescale, double *min_v, double *max_v)
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

void LSHCompute::copyResultMatrix(const lsh_input *pInputs,
    const cv::Mat *channelLSH, lsh_output *pOutput)
{
    std::ostringstream os;
    int c;
    bool copied = false;

    pOutput->rescaled = 1.0;

    if (pInputs->bUseMaxValue)
    {
        os.str("");
        os << prefix << "using max value "
            << pInputs->maxMatrixValue << std::endl;
        emit logMessage(QString::fromStdString(os.str()));

        double fMatMax = 0.0;

        os.str("");
        for (c = 0; c < 4; c++)
        {
            double fmin, fmax;
            cv::minMaxLoc(channelLSH[c], &fmin, &fmax);

            os.str("");
            os << "channel[" << c << "] range from " << fmin << " to "
                << fmax << std::endl;
            emit logMessage(QString::fromStdString(os.str()));

            fMatMax = std::max(fmax, fMatMax);
        }

        if (fMatMax > pInputs->maxMatrixValue)
        {
            double rescale = pInputs->maxMatrixValue / fMatMax;
            double min_v = 1.0;

            os.str("");
            os << prefix << "rescaling all channels by " << rescale
                << " to enforce max gain supported by HW" << std::endl;
            emit logMessage(QString::fromStdString(os.str()));

            rescaleMatrix(channelLSH, pOutput->channels, rescale, &min_v);
            pOutput->rescaled = pOutput->rescaled * rescale;

            copied = true;
        }
    }

    if (!copied)
    {
        os.str("");
        os << prefix << "Copy without rescaling of matrix" << std::endl;
        emit logMessage(QString::fromStdString(os.str()));

        for (c = 0; c < 4; c++)
        {
            pOutput->channels[c] = channelLSH[c];
        }
        copied = true;
    }

    IMG_INT32 maxDiff = 0;
    // in bytes, but needs to be a multiple of 16 Bytes
    unsigned int size = 0;

    while (0 == maxDiff)
    {
        for (c = 0; c < 4; c++)
        {
            for (int j = 0; j < pOutput->channels[c].rows; j++)
            {
                float current = pOutput->channels[c].at<float>(j, 0);
                IMG_INT32 prev = IMG_Fix_Clip(current, PREC(LSH_VERTEX));
                IMG_INT32 curr = 0;
                IMG_INT32 diff = 0;

                for (int i = 1; i < pOutput->channels[c].cols; i++)
                {
                    current = pOutput->channels[c].at<float>(j, i);

                    curr = IMG_Fix_Clip(current, PREC(LSH_VERTEX));
                    diff = curr - prev;

                    if (diff < 0)
                    {
                        diff = (diff*-1) - 1;
                    }

                    maxDiff = IMG_MAX_INT(diff, maxDiff);

                    prev = curr;
                }
            }

            emit currentStep(++this->step);  // max is LSH_N_STEPS
        }

        // log2()+1 for number of bits needed for difference
        pOutput->bitsPerDiff = (log(maxDiff) / log(2)) + 1;
        // diff bits + 1 for negative
        pOutput->bitsPerDiff = IMG_MAX_INT(LSH_DELTA_BITS_MIN,
            pOutput->bitsPerDiff + 1);

        os.str("");
        os << prefix << "maximum difference found is (integer value) "
            << maxDiff << " which needs "
            << pOutput->bitsPerDiff << " bits in the HW format."
            << std::endl;
        emit logMessage(QString::fromStdString(os.str()));

        if (0 == maxDiff)
        {
            // should not happen but if it does then we loop forever
            break;
        }

        if (pOutput->bitsPerDiff > inputs.maxBitDiff)
        {
            if (pInputs->bUseMaxBitsPerDiff)
            {
                cv::Mat tmpOutput[4];
                double rescale =
                    static_cast<double>((1 << (inputs.maxBitDiff - 1)) - 1)
                    / maxDiff;
                double min_v = 1.0;

                os.str("");
                os << prefix << "rescaling all channels by " << rescale
                    << " to enforce max bits per difference of "
                    << inputs.maxBitDiff << "b in HW grid format."
                    << std::endl;
                emit logMessage(QString::fromStdString(os.str()));

                rescaleMatrix(pOutput->channels, tmpOutput, rescale, &min_v);

                for (c = 0; c < 4; c++)
                {
                    pOutput->channels[c] = tmpOutput[c];
                }

                pOutput->rescaled = pOutput->rescaled * rescale;
                maxDiff = 0;  // to recompute the max grid

                this->step -= 4;
                emit currentStep(this->step);  // max is LSH_N_STEPS
            }
        }
    }

    // bits per line (rounded up)
    size = (((pOutput->channels[0].cols - 1)*pOutput->bitsPerDiff + 7) / 8);
    size = size + 2;  // add initial 16b not compressed

    // round up size to multiple of 16 Bytes
    pOutput->lineSize = ((size + 15) / 16) * 16;

    os.str("");
    os << prefix << "expected LSH grid line size is "
        << pOutput->lineSize << " Bytes" << std::endl;
    os << prefix << "expected LSH grid total size is "
        << pOutput->lineSize*pOutput->channels[0].rows << " Bytes"
        << std::endl;
    emit logMessage(QString::fromStdString(os.str()));
}

/*
 * public slots
 */

int LSHCompute::compute(const lsh_input *pInputs)
{
    cv::Mat flatField;  // input image
    std::ostringstream os;
    int c;

    time_t startTime = time(NULL);

    this->inputs = *pInputs;

    step = 0;
    os.str("");
    os << prefix << inputs;
        //<< "using file " << this->inputs.imageFilename << std::endl;
    emit logMessage(QString::fromStdString(os.str()));

    try
    {
        flatField = loadBAYERFlxFile(inputs.imageFilename);
    }
    catch (std::exception e)
    {
        std::cerr << prefix << "ERROR loading flx image ("
            << inputs.imageFilename << "):" << e.what() << std::endl;
        return -1;
    }

    emit currentStep(++this->step);  // max is LSH_N_STEPS

    lsh_output *pOutput = new lsh_output();
    cv::Mat channelLSH[4];
    cv::Mat targetLSH[4];
    double mse[4] = { 0.0, 0.0, 0.0, 0.0 };

    if (inputs.fitting)  // generate grid based on curve fitting
    {
        calculateLSHFit(flatField, channelLSH, targetLSH, mse,
            pOutput->smoothedImage);
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
        calculateLSHDirect(flatField, channelLSH, pOutput->smoothedImage);
    }

    cv::split(flatField, pOutput->inputImage);

    // copy the result matrix with or without rescaling
    copyResultMatrix(pInputs, channelLSH, pOutput);

    // copy the other parameters
    for (c = 0; c < 4; c++)
    {
        // for the display
        pOutput->blackLevel[c] =
            pOutput->inputImage[c] - (inputs.blackLevel[c] << 1);
        pOutput->blackLevel[c] = cv::max(pOutput->blackLevel[c], 0.0);

        pOutput->targetLSH[c] = targetLSH[c];
        pOutput->fittingMSE[c] = mse[c];

        double minVal, maxVal;
        cv::minMaxLoc(pOutput->channels[c], &minVal, &maxVal);
        pOutput->normalised[c] =
            (pOutput->channels[c] - minVal) / (maxVal - minVal) / 1.25 + 0.1;
        pOutput->normalised[c].convertTo(pOutput->normalised[c], CV_8U, 255);
        cv::cvtColor(pOutput->normalised[c], pOutput->normalised[c],
            CV_GRAY2RGB);

        emit currentStep(++this->step);  // max is LSH_N_STEPS
    }

    computeError(pOutput);

    startTime = time(0) - startTime;

    os.str("");
    os << prefix << "ran into " << static_cast<int>(startTime) << " sec"
        << std::endl;
    emit logMessage(QString::fromStdString(os.str()));

    emit finished(this, pOutput);

    // assert(this->steps <= LSH_N_STEPS);
    return 0;
}
