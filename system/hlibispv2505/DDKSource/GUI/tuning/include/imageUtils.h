/**
******************************************************************************
@file imageUtils.h

@brief Various utilities to help with OpenCV matrices and tuning

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
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <image.h>
#include <string>
#include <iostream>
#include <exception>

#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

//some useful macros
#define M_MAX(a,b) (a>b?a:b)
#define M_MIN(a,b) (a<b?a:b)
#define M_CLIP(a,min,max) a= M_MAX(M_MIN(a,(max)),(min));

#ifdef WIN32
// disable the C++ exception spec ignored warning
#pragma warning(disable: 4290)
#endif

/**
 * @defgroup IMAGE_DISPLAY_UTILS cv::Mat display utils
 * @ingroup VISION_TUNING
 * @brief Functions that can be used to display CV matrices
 * @{
 */

/** @brief Display an image with opencv utils */
void displayImage(const cv::Mat &image, const std::string &title,
    int posX, int posY, int  width, int height);

/** @brief Plot a line followed by a tag */
cv::Mat &plotHorizontalTag(cv::Mat &baseImage, cv::Scalar color,
    int row, int col, int width, std::string text,
    int thickness = 2, float size = 0.75);

/**
 * @brief Plot an histogram or curve in a Mat image.
 *
 * The scalign factor determines the scale of the plot in the Y coordinate
 */
cv::Mat &plotCurve(cv::Mat &baseImage, cv::Mat curve, cv::Scalar color,
    double scaling = 1.0);

cv::Mat &plotCurveRegion(cv::Mat &baseImage, cv::Mat curve, cv::Scalar color,
    int row, int col, int width, int height, int thickness = 2);

/** @brief Plot a curve normalized to fit a particular region of the image */
cv::Mat &plotCurveScaled(cv::Mat &baseImage, cv::Mat curve, cv::Scalar color,
    int row, int col, int width, double heightScale, int thickness);

void fillImage(cv::Mat &image, cv::Scalar color);

/**
 * @}
 */
/* end of the image display utils group */

/**
 * @defgroup IMAGE_PROCESSING_UTILS cv::Mat processing utils
 * @ingroup VISION_TUNING
 * @brief Processing functions for cv::Mat
 * @{
 */

/** @brief simplistic tone mapping curve application (no interpolation) */
cv::Mat applyMappingCurve(const cv::Mat &image, const cv::Mat &curve);

/** @brief Generate the acumulated curve from an histogram */
cv::Mat accumulateHistogram(const cv::Mat &histogram);

/**
 * Receives a normalized histogram and calculated the threshold
 * in which we reach a certain amount of image data
 */
double getHistogramThreshold(cv::Mat histogram, double threshold,
    bool reverse = false);

double roundDouble(double value, double factor = 1000.0);

/**
 * @}
 */
/* end of the image processing utils group */

/**
* @defgroup IMAGE_CONVERTOR_UTILS cv::Mat convertor from FLX
* @ingroup VISION_TUNING
* @brief Convert FLX images into cv::Mat
* @{
*/

/**
 * @brief Function for loading a CImageFlx with YUV format
 *
 * Delegates to @ref FLXYUV2LumaMat()
 *
 * First frame only is loaded.
 *
 * @throws std::exception with error from the @ref CImageFLX class
 */
cv::Mat loadYUVFlxFile(const std::string &filename) throw(std::exception);

/** @brief Conversion of a CImageFlx to a opencv Mat image */
cv::Mat FLXYUV2LumaMat(const CImageFlx &flxImageYUV);

/**
 * @brief Load a FLX BAYER file into a 4 channel cv::Mat image
 *
 * Delegates to @ref FLXBAYER2MatMosaic() which will scaled the image to
 * 10bit per channel.
 *
 * First frame only is loaded.
 *
 * @throws std::exception with error from the @ref CImageFLX class
 */
cv::Mat loadBAYERFlxFile(const std::string &filename) throw(std::exception);

/**
 * @brief Create a four channel opencv Mat image from a FLX RGGB bayer image
 *
 * Delegates to @ref FLXRAW2MatChannels() to convert the @ref CImageFLX
 * to cv::Mat
 *
 * @warning Normalises the input to 10b
 */
cv::Mat FLXBAYER2MatMosaic(const CImageFlx &flxImageBayer);

/**
 * @brief Create a 3 channel opencv Mat image from a FLX RGB image
 *
 * Delegates to @ref FLX2MatChannels() to convert the @ref CImageFLX
 * to cv::Mat
 */
cv::Mat FLXRGB2Mat(const CImageFlx &flxImageBayer);

/**
 * @brief Create a 3 channel opencv Mat image from a FLX RGB image
 *
 * Delegates to @ref FLX2MatChannels_10bit() to convert the @ref CImageFLX
 * to cv::Mat
 */
cv::Mat FLXRGB2Mat_10bit(const CImageFlx &flxImageBayer);

/** @brief converts a 444 YUV (CV_8UC3) into RGB matrix (CV_8UC3) */
cv::Mat YUV444ToRGBMat(cv::Mat yuv444, bool bBGR, const double convMatrix[9],
    const double convInputOff[3]);

/**
 * @}
 */
/* end of the image convertion utils group */


#endif //IMAGE_UTILS_H
