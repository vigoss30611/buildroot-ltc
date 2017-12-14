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
 * @brief Number of steps the compute process sends per input image
 *
 * @li load
 * @li 1 per channel to compute grid
 * @li 1 to compute max gain
 * @li 1 to compute max diff bits
 * @li 1 per channel for interpolation
 * @li 1 to compute normalised output
 * @li 1 per channel for error computation
 */
#define LSH_N_STEPS (4+1+1+1+4)

/**
 * @ingroup VISION_TUNING
 */
struct lsh_output
{
    int temperature;
    /**
     * @brief to know if the output matrix was interpolated or generated
     *
     * interpolated matrices do not have:
     * @li inputImage[]
     * @li smoothedImage[]
     * @li blackLevel[]
     * @li targetLSH[]
     * @li fittginMSE[]
     * @li outputMSE[]
     */
    bool interpolated;
    /** @brief proposed output filename */
    std::string filename;
    /** @brief matrix channels of <float> */
    cv::Mat channels[4];
    /**
     * @brief normalised values of channels to have better display
     */
    cv::Mat normalised[4];

    // output information used for displaying matrix
    /**
     * @brief input image loaded values
     * @warning not populated if @ref interpolated
     */
    cv::Mat inputImage[4];
    /**
     * @brief result of the smoothing
     * @warning not populated if @ref interpolated
     */
    cv::Mat smoothedImage[4];
    /**
     * @brief computed from input image and channels
     * @warning not populated if @ref interpolated
     */
    cv::Mat blackLevel[4];
    /**
     * @brief target matrix when using fitting algorithm
     * @warning not populated if @ref interpolated
     */
    cv::Mat targetLSH[4];
    /**
     * @brief mean square error used when fitting algorithm enabled
     * @warning not populated if @ref interpolated
     */
    double fittingMSE[4];
    /**
     * @brief output MSE
     * @warning not populated if @ref interpolated
     */
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

/**
 * @ingroup VISION_TUNING
 */
class LSHCompute: public ComputeBase
{
    Q_OBJECT

protected:
    /** @brief parameters used for the computation */
    lsh_input inputs;
    /**
     * @brief current computation step value. Transfered to the GUI using
     * @ref currentStep() signal
     */
    int step;

    /**
     * LSH coefficients are calcualted using a curve fitting approach
     *
     * @param[in] info contains the settings for the input image
     * @param[in,out] output will be populated according to fitting algorithm.
     * @warning Assumes that pOutput has:
     * @li correct temperature
     * @li inputImage already splitted from loaded image
     * @li blackLevel with values to use
     *
     * Not static or const because updates @ref step once per channel
     */
    void calculateLSHFit(const lsh_input::input_info &info,
        lsh_output *pOutput);

    /**
     * LSH coefficients are calcualted directly from the smoothed flat field
     *
     * @param[in] info contains the settings for the input image
     * @param[in,out] output will be populated according to direct algorithm.
     * @warning Assumes that pOutput has:
     * @li correct temperature
     * @li inputImage already splitted from loaded image
     * @li blackLevel with values to use
     *
     * Not static or const because updates @ref step once per channel
     */
    void calculateLSHDirect(const lsh_input::input_info &info,
        lsh_output *pOutput);

    /**
     * @brief rescales a 4 channel matrix (creates new matrices as output)
     *
     * @param[in] input 4 channels as input
     * @param[out] output 4 channels (created)
     * @param rescale multiplier applied to all entries
     * @param[in] min if present clip to min
     * @param[in] min if present clip to max (min applied first)
     */
    static void rescaleMatrix(const cv::Mat *input, cv::Mat *output,
        double rescale, const double *min_v = 0, const double *max_v = 0);

    /**
     * @brief compute the error in pOutput->outputMSE
     *
     * @warning available as a compilation time choice.
     *
     * Not static or const because updates @ref step once per channel
     */
    void computeError(lsh_output *pOutput);

    /**
     * @brief Computes the number of bits per difference needed when applying
     * the HW format.
     */
    static unsigned int getBitsPerDiff(const lsh_input *pInputs,
        const cv::Mat *channels, int minBits, int &maxDiff);

    /**
     * @brief computes a common rescale factor to enfore the maximum HW gain
     */
    double getGainRescale(const std::map<LSH_TEMP, lsh_output*> &output) const;

    /**
     * @brief compute a common rescale factor to enfore the maximum HW bits
     * per diff
     *
     * Delegates to getBitsPerDiff() to get the biggest bits per diff of all
     * the output and rescales all outputs according to that.
     */
    double getDiffBitRescale(const std::map<LSH_TEMP, lsh_output*> &output,
        unsigned int &maxDiffPerBit) const;


    int interpolateOutputs(std::map<LSH_TEMP, lsh_output*> &output);

    /**
     * @brief Recale the output images to fit the max HW gain and bits per
     * diff.
     *
     * Delegates to getGainRescale() and getDiffBitRescale() to compute the
     * scaling factor.
     *
     * Updates lsh_output elements with the correct factor.
     *
     * Updates @ref step 2x per output (1 for gain check, 1 for bits check)
     */
    void rescaleOutputs(std::map<LSH_TEMP, lsh_output*> &output);

    /**
     * @brief Computes the size of the matrix after the rescaling and the
     * bits per diff is chosen. Also compute the error grid.
     *
     * Updates @ref step 1x per output
     */
    void computeError(std::map<LSH_TEMP, lsh_output*> &output);

public:
    explicit LSHCompute(QObject *parent = 0);
    virtual ~LSHCompute();

    void clearOutput();

    std::map<LSH_TEMP, lsh_output*> output;

    /**
     * @brief write a matrix to a file respecting the LSH format
     *
     * @param[in] matrices pointer to 4 matrices (1 per channel)
     * @param tileSize
     * @param filename
     * @param saveAsTxt if true will save as text, otherwise save binary
     *
     * Delegates to @ref LSH_AllocateMatrix() and @ref LSH_Save_bin() or
     * @ref LSH_Save_txt()
     */
    static void writeLSHMatrix(const cv::Mat *matrices, const int tileSize,
        const std::string &filename, const bool saveAsTxt = false)
        throw(std::exception);

    /**
     * @brief gaussian smooth of a matrix
     *
     * Can be used to smooth the input after loaded with
     * @ref loadBAYERFlxFile() and then split into 4 channels with
     * cv::splot() to get 1 matrix per channel
     */
    static void smoothMatrix(const cv::Mat &input, cv::Mat &output,
        const double smoothing, const int smoothsize = 31);

public slots:
    /** @brief compute the LSH grid for all given inputs */
    int compute(const lsh_input *inputs);

signals:
    /**
     * @brief emits this signal when the computation is finished
     *
     * Results can be retrieved using @ref output
     */
    void finished(LSHCompute *toStop);
    /** @brief emit LSH_N_STEPS per input */
    void currentStep(int s);
};

#endif /* LSHCOMPUTE_HPP */
