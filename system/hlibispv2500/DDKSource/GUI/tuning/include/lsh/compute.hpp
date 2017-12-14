/**
*******************************************************************************
@file lsh/compute.hpp

@brief Compute lens shading grid on a separate QThread

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
#ifndef LSHCOMPUTE_HPP
#define LSHCOMPUTE_HPP

#include <opencv2/core/core.hpp>

#include <QtCore/QObject>
class QThread;  // <QtCore/QThread>

#include <string>

#include "lsh/tuning.hpp"
#include "computebase.hpp"

// #include "buffer.hpp"

/**
 * @brief Number of steps the compute process sends
 *
 * @li load
 * @li 1 per channel to compute grid
 * @li 1 per channel to computer max difference
 * @li 1 per channel to copy results
 * @li 1 per channel for error computation
 */
#define LSH_N_STEPS (1+4+4+4+4)

struct lsh_output
{
    /** @brief matrix channels */
    cv::Mat channels[4];
    // LSH parameter in FelixSetupArgs are not used beside the matrix

    // output information used for displaying matrix
    /** @brief input image loaded values */
    cv::Mat inputImage[4];
    /** @brief result of the smoothing */
    cv::Mat smoothedImage[4];
    /** @brief computed from input image and channels */
    cv::Mat blackLevel[4];
    /** @brief target matrix when using fitting algorithm */
    cv::Mat targetLSH[4];
    /** @brief mean square error used when fitting algorithm enabled */
    double fittingMSE[4];
    /** @brief normalised values of channels to have better display */
    cv::Mat normalised[4];
    /** @brief output MSE */
    cv::Mat outputMSE[4];

    /** @brief expected output matrix bits per diff */
    unsigned int bitsPerDiff;
    /**
     * @brief expected output matrix line size in Bytes when encoded with HW
     * differential format
     */
    unsigned int lineSize;

    /**
     * @brief if different than 1.0 the grid has been rescaled by that value
     */
    double rescaled;
};

class LSHCompute: public ComputeBase
{
    Q_OBJECT

protected:
    lsh_input inputs;
    /** @brief current computation step value */
    int step;

    /**
     * Receives a 4 channel RGGB cv::Mat image and returns a 4 element array
     * with a LSH coefficients grid per element
     *
     * LSH coefficients are calcualted using a curve fitting approach
     */
    void calculateLSHFit(const cv::Mat &flatFieldRGGB,
        cv::Mat(&lshGrids)[4], cv::Mat lshTargets[4], double mse[4],
        cv::Mat(&inputSmoothed)[4]);

    /**
     * Receives a 4 channel RGGB cv::Mat image and returns a 4 element array
     * with a LSH coefficients grid per element
     *
     * LSH coefficients are calcualted directly from the smoothed flat field
     */
    void calculateLSHDirect(const cv::Mat &flatFieldRGGB,
        cv::Mat (&lshGrids)[4], cv::Mat (&inputSmoothed)[4]);

    /**
     * @brief rescales a 4 channel matrix (creates new matrices as output)
     *
     * @param[in] input 4 channels as input
     * @param[out] output 4 channels (created)
     * @param rescale multiplier applied to all entries
     * @param[in] min if present clip to min
     * @param[in] min if present clip to max (min applied first
     */
    static void rescaleMatrix(const cv::Mat *input, cv::Mat *output,
        double rescale, double *min_v = 0, double *max_v = 0);

    // cannot be const because modified step
    void computeError(lsh_output *pOutput);

    /*
     * copy the result matrix, involving rescaling of values or not
     *
     * computes the expect min bit diff and the expected size per line
     */
    void copyResultMatrix(const lsh_input *pInputs,
        const cv::Mat *channelLSH, lsh_output *pOutput);

public:
    explicit LSHCompute(QObject *parent = 0);

    // virtual ~LSHCompute();

    static void writeLSHMatrix(cv::Mat (&matrix)[4], int tileSize,
        const std::string &filename) throw(std::exception);

public slots:
    int compute(const lsh_input *inputs);

signals:
    void finished(LSHCompute *toStop, lsh_output *pOutput);
    void currentStep(int s);
};

#endif /* LSHCOMPUTE_HPP */
