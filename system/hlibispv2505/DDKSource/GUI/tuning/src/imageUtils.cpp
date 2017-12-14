/**
*******************************************************************************
@file imageUtils.cpp

@brief Implementation of the various utilities for OpenCV matrices

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
#include "imageUtils.h"

/* runtime_error (inherit exception and has a constructor with access to the
 * what message) */
#include <stdexcept> 
#include <opencv2/highgui/highgui.hpp>

#include <ostream>
//#define DEBUG_LOADING

#ifdef DEBUG_LOADING
/* if defined print input FLX and matrix to txt file when doing preview
 * (very slow!) */
#include <fstream>
#endif

/*
 * local functions
 */

/**
* @brief Copy an FLX raw image to a 4 planes (RGGB) Mat image
*/
template <typename T>
void FLXRAW2MatChannels(const CImageFlx &flxImage, cv::Mat &openCVImage)
{
    // flxImage.GetNColChannels() could be used to know the number of channels
    // but it may be slower than having a manually unrolled loop
    for (int row = 0; row < flxImage.height / 2; row++)
    {
        for (int col = 0; col < flxImage.width / 2; col++)
        {
            openCVImage.at<cv::Vec4w>(row, col).val[0] = static_cast<T>(*(flxImage.chnl[0].data + (row)*flxImage.chnl[0].chnlWidth + col));
            openCVImage.at<cv::Vec4w>(row, col).val[1] = static_cast<T>(*(flxImage.chnl[1].data + (row)*flxImage.chnl[1].chnlWidth + col));
            openCVImage.at<cv::Vec4w>(row, col).val[2] = static_cast<T>(*(flxImage.chnl[2].data + (row)*flxImage.chnl[2].chnlWidth + col));
            openCVImage.at<cv::Vec4w>(row, col).val[3] = static_cast<T>(*(flxImage.chnl[3].data + (row)*flxImage.chnl[3].chnlWidth + col));
        }
    }
}

template <typename T>
void FLX2MatChannels(const CImageFlx &flxImage, cv::Mat &openCVImage)
{
    for (int row = 0; row < flxImage.height; row++)
    {
        for (int col = 0; col < flxImage.width; col++)
        {
            openCVImage.at<cv::Vec3w>(row, col).val[0] = static_cast<T>(*(flxImage.chnl[0].data + (row)*flxImage.chnl[0].chnlWidth + col));
            openCVImage.at<cv::Vec3w>(row, col).val[1] = static_cast<T>(*(flxImage.chnl[1].data + (row)*flxImage.chnl[1].chnlWidth + col));
            openCVImage.at<cv::Vec3w>(row, col).val[2] = static_cast<T>(*(flxImage.chnl[2].data + (row)*flxImage.chnl[2].chnlWidth + col));
        }
    }
}

template <typename T>
void FLX2MatChannels_rshift(const CImageFlx &flxImage, cv::Mat &openCVImage, const int r_shift)
{
    for (int row = 0; row < flxImage.height; row++)
    {
        for (int col = 0; col < flxImage.width; col++)
        {
            openCVImage.at<cv::Vec3w>(row, col).val[0] = static_cast<T>(*(flxImage.chnl[0].data + (row)*flxImage.chnl[0].chnlWidth + col) >> r_shift);
            openCVImage.at<cv::Vec3w>(row, col).val[1] = static_cast<T>(*(flxImage.chnl[1].data + (row)*flxImage.chnl[1].chnlWidth + col) >> r_shift);
            openCVImage.at<cv::Vec3w>(row, col).val[2] = static_cast<T>(*(flxImage.chnl[2].data + (row)*flxImage.chnl[2].chnlWidth + col) >> r_shift);
        }
    }
}

template <typename T>
void FLX2MatChannels_lshift(const CImageFlx &flxImage, cv::Mat &openCVImage, const int l_shift)
{
    for (int row = 0; row < flxImage.height; row++)
    {
        for (int col = 0; col < flxImage.width; col++)
        {
            openCVImage.at<cv::Vec3w>(row, col).val[0] = static_cast<T>(*(flxImage.chnl[0].data + (row)*flxImage.chnl[0].chnlWidth + col) << l_shift);
            openCVImage.at<cv::Vec3w>(row, col).val[1] = static_cast<T>(*(flxImage.chnl[1].data + (row)*flxImage.chnl[1].chnlWidth + col) << l_shift);
            openCVImage.at<cv::Vec3w>(row, col).val[2] = static_cast<T>(*(flxImage.chnl[2].data + (row)*flxImage.chnl[2].chnlWidth + col) << l_shift);
        }
    }
}

void printFLxImageTxt(std::ostream &os, const CImageFlx &flxImage)
{
    int row, col, c;
    int nchnl = flxImage.GetNColChannels();
    int flxRows = flxImage.chnl[0].chnlHeight;
    int flxCols = flxImage.chnl[0].chnlWidth;

    std::map<int, int> hist[4];
    for (c = 0; c < nchnl; c++)
    {
        for (row = 0; row < 1 << flxImage.chnl[0].bitDepth; row++)
        {
            hist[c][row] = 0;
        }
        os << "channel " << c << std::endl;
        for (row = 0; row < flxRows; row++)
        {
            os << "line " << row << std::endl;
            for (col = 0; col < flxCols; col++)
            {
                os << " " << flxImage.chnl[c].data[row * flxCols + col];
                hist[c][flxImage.chnl[c].data[row * flxCols + col]] += 1;
            }
            os << std::endl;
        }
        os << std::endl;
    }

    os << std::endl << std::endl;
    for (c = 0; c < nchnl; c++)
    {
        os << "histogram " << c << ":" << std::endl;
        for (row = 0; row < 1 << flxImage.chnl[0].bitDepth; row++)
        {
            os << " " << hist[c][row];
        }
        os << std::endl;
    }
    os << std::endl;
}

template <typename T>
void printCVBayerTxt(std::ostream &os, const cv::Mat bayer)
{
    int row, col, c;
    for (c = 0; c < 4; c++)
    {
        os << "channel " << c << std::endl;
        for (row = 0; row < bayer.rows; row++)
        {
            os << "line " << row << std::endl;
            for (col = 0; col < bayer.cols; col++)
            {
                os<< " " << bayer.at<T>(row, col).val[c];
            }
            os << std::endl;
        }
        os << std::endl;
    }
}

/*
 * Display utils
 */

void displayImage(const cv::Mat &image, const std::string &title,
    int posX, int posY, int  width, int height)
{
    // cv::WINDOW_NORMAL is not defined on linux opencv2.3
    cv::namedWindow(title, cv::WINDOW_NORMAL);
    cv::imshow(title, image);
    // available in opencv 2.4 - but linux uses opencv2.3
    cv::moveWindow(title, posX, posY);
    cv::resizeWindow(title, width, height);
}

cv::Mat &plotHorizontalTag(cv::Mat &baseImage, cv::Scalar color,
    int row, int col, int width, std::string text, int thickness, float size)
{
    cv::line(baseImage, cv::Point(col, baseImage.rows - row),
        cv::Point(col + width, baseImage.rows - row), color, thickness);
    cv::putText(baseImage, text, cv::Point(col + width + 5,
        baseImage.rows - row), cv::FONT_ITALIC, size, color, thickness);
    return baseImage;
}

cv::Mat &plotCurve(cv::Mat &baseImage, cv::Mat curve, cv::Scalar color,
    double scaling)
{
    double minVal, maxVal;
    double widthScale = static_cast<double>(baseImage.cols) / curve.rows;
    cv::minMaxLoc(curve, &minVal, &maxVal);

    for (int i = 1; i < curve.rows; i++)
    {
        cv::line(baseImage, cv::Point(int((i - 1)*widthScale),
            int(baseImage.rows - cvRound(scaling*curve.at<float>(i - 1) / maxVal*baseImage.rows))),
            cv::Point(int(i*widthScale),
            int(baseImage.rows - cvRound(scaling*curve.at<float>(i) / maxVal*baseImage.rows))),
            color, 2);
    }

    return baseImage;
}

cv::Mat &plotCurveRegion(cv::Mat &baseImage, cv::Mat curve, cv::Scalar color,
    int row, int col, int width, int height, int thickness)
{
    if (curve.rows == 1)
    {
        curve = curve.clone();
        curve = curve.t();
    }

    double widthScale = static_cast<double>(width) / curve.rows;
    double minVal, maxVal;
    cv::minMaxLoc(curve, &minVal, &maxVal);

    for (int i = 1; i < curve.rows; i++)
    {
        cv::line(baseImage, cv::Point(col + int((i - 1)*widthScale),
            baseImage.rows - row - int(cvRound((curve.at<float>(i - 1) - minVal) / (maxVal - minVal)*height))),
            cv::Point(col + int((i)*widthScale), baseImage.rows - row - int(cvRound((curve.at<float>(i)-minVal) / (maxVal - minVal)*height))),
            color, thickness);
    }

    return baseImage;
}

cv::Mat &plotCurveScaled(cv::Mat &baseImage, cv::Mat curve, cv::Scalar color,
    int row, int col, int width, double heightScale, int thickness)
{

    if (curve.rows == 1)
    {
        curve = curve.clone();
        curve = curve.t();
    }

    double widthScale = static_cast<double>(width) / curve.rows;
    double minVal, maxVal;
    cv::minMaxLoc(curve, &minVal, &maxVal);

    for (int i = 1; i < curve.rows; i++)
    {
        cv::line(baseImage, cv::Point(col + int((i - 1)*widthScale),
            baseImage.rows - row - int(cvRound((curve.at<float>(i - 1))*heightScale))),
            cv::Point(col + int((i)*widthScale),
            baseImage.rows - row - int(cvRound((curve.at<float>(i))*heightScale))),
            color, thickness);
    }

    return baseImage;
}

void fillImage(cv::Mat &image, cv::Scalar color)
{
    unsigned char colour[3] = {
        static_cast<unsigned char>(color(0)),
        static_cast<unsigned char>(color(1)),
        static_cast<unsigned char>(color(2))
    };
    for (int row = 0; row < image.rows; row++)
    {
        for (int col = 0; col < image.cols; col++)
        {
            image.at<cv::Vec3b>(row, col)[0] = colour[0];
            image.at<cv::Vec3b>(row, col)[1] = colour[1];
            image.at<cv::Vec3b>(row, col)[2] = colour[2];
        }
    }
}

/*
 * Processing auxiliary functions
 */

cv::Mat applyMappingCurve(const cv::Mat &image, const cv::Mat &curve)
{

    cv::Mat mappedImage = cv::Mat(image.rows, image.cols, image.type());

    for (int row = 0; row < image.rows; row++)
    {
        for (int col = 0; col < image.cols; col++)
        {
            float pixelValue = image.at<float>(row, col);
            //curve interpolation
            double position = pixelValue*(curve.rows - 1);
            int index = (int)floor(position);
            index = M_CLIP(index, 0, curve.rows - 2);
            double wIndex = 1.0 - (position - index);

            mappedImage.at<float>(row, col) = float(curve.at<float>(index)*wIndex + curve.at<float>(index + 1)*(1 - wIndex));
        }
    }

    return mappedImage;
}

cv::Mat accumulateHistogram(const cv::Mat &histogram)
{
    cv::Mat accumulatedHistogram = cv::Mat(histogram.rows + 1, 1, histogram.type());

    accumulatedHistogram.at<float>(0) = 0;
    for (int i = 1; i < histogram.rows; i++)
    {
        accumulatedHistogram.at<float>(i) = (float)M_MIN(1.0, accumulatedHistogram.at<float>(i - 1) + histogram.at<float>(i - 1));
    }
    accumulatedHistogram.at<float>(histogram.rows) = 1;

    return accumulatedHistogram;
}

double getHistogramThreshold(cv::Mat histogram, double threshold, bool reverse)
{
    double acumulated = 0;

    for (int i = 0; i < histogram.rows; i++)
    {
        if (!reverse)
        {
            acumulated += histogram.at<float>(i);
        }
        else
        {
            acumulated += histogram.at<float>(histogram.rows - i - 1);
        }

        if (acumulated >= threshold)
        {
            if (!reverse)
            {
                return double(i) / histogram.rows;
            }
            else
            {
                return double(histogram.rows - i - 1) / histogram.rows;
            }

        }
    }

    return 1.0;
}

double roundDouble(double value, double factor)
{
    return floor(value*factor + 0.5) / factor;
}

/*
 * convertion utils
 */

cv::Mat loadYUVFlxFile(const std::string &filename) throw(std::exception)
{
    CImageFlx flxImage;
    // header must be loaded first
    const char *status = flxImage.LoadFileHeader(filename.c_str(), NULL);
    if (status)
        throw std::runtime_error(status);

    /* after reading the header we can load the file data. We will read the
    * first frame only. */
    status = flxImage.LoadFileData(0);
    if (status)
        throw std::runtime_error(status);

    // check that we have received an YUV Image
    if (flxImage.colorModel != CImageBase::CM_YUV)
    {
        flxImage.Unload();  // free the flx image
        throw std::runtime_error("Invalid Flx file: must be YUV format");
    }

    /* we add 0.5 to make the range positive between 0.0 and 1.0 as the
    * image is signed luma in the -0.5 to 0.5 range */
    cv::Mat loadedImage = FLXYUV2LumaMat(flxImage) + 0.5;
    flxImage.Unload();  // free the flx image

    return loadedImage;
}

cv::Mat FLXYUV2LumaMat(const CImageFlx &flxImageYUV)
{
    // create a float luma plane with the flx image dimensions
    cv::Mat lumaPlane = cv::Mat(flxImageYUV.height, flxImageYUV.width,
        CV_32FC1);

    for (int row = 0; row < flxImageYUV.height; row++)
    {
        for (int col = 0; col < flxImageYUV.width; col++)
        {
            lumaPlane.at<float>(row, col) = static_cast<float>(*(flxImageYUV.chnl[0].data + row*flxImageYUV.chnl[0].chnlWidth + col)) / (1 << flxImageYUV.chnl[0].bitDepth);
        }
    }

    return lumaPlane;
}

cv::Mat loadBAYERFlxFile(const std::string &filename) throw(std::exception)
{
    CImageFlx flxImage;
    const char *status = NULL;

    if (filename.empty())
    {
        throw std::runtime_error("Empty filename!");
    }

    // header must be loaded first
    status = flxImage.LoadFileHeader(filename.c_str(), NULL);
    if (status)
    {
        std::string err = "failed to load header from '";
        err += filename;
        err += "' (";
        err += strerror(errno);
        err += " - ";
        err += status;
        err += ")";
        throw std::runtime_error(err);
    }

    /* after reading the header we can load the file data. We will read
     * the first frame only. */
    status = flxImage.LoadFileData(0);
    if (status)
    {
        std::string err = "failed to load file data for frame 0 from '";
        err += filename;
        err += "' (";
        err += strerror(errno);
        err += " - ";
        err += status;
        err += ")";
        throw std::runtime_error(err);
    }

    // check that we have a bayer image
    if (flxImage.colorModel != CImageBase::CM_RGGB)
    {
        flxImage.Unload();  // free the flx image
        throw std::runtime_error("Invalid Flx input, must be RGGB format");
    }

    cv::Mat loadedImage = FLXBAYER2MatMosaic(flxImage);
    flxImage.Unload();  // free the flx image

    return loadedImage;
}

cv::Mat FLXBAYER2MatMosaic(const CImageFlx &flxImageBayer)
{
    // Create a openCV Mat 16 bit unsigned to contain the bayer image
    // bayer image has a size in pixels while we need a matrix of CFA
    cv::Mat opencvMatBayer(flxImageBayer.height / 2, flxImageBayer.width / 2,
        CV_16UC4);

    /* Fill in the openCV Mat with the information in the FLX RAW image
     * as unsigned short int (16 bits unsigned) */
    FLXRAW2MatChannels<IMG_UINT16>(flxImageBayer, opencvMatBayer);

#ifdef DEBUG_LOADING
    {
        std::ofstream of("input_flx.txt");
        of << "FLX image:" << std::endl;
        printFLxImageTxt(of, flxImageBayer);
        of.close();

        /*of.open("input_mati.txt");
        of << "Matrix:" << std::endl;
        printCVBayerTxt<cv::Vec4w>(of, opencvMatBayer);
        of.close();*/
    }
#endif

    /* We assume that all the channels have the same bitdepth
     * as the first channel */
    int bdiff = flxImageBayer.chnl[0].bitDepth - 10;
    double mult = 1.0;
    if (bdiff < 0)
    {
        bdiff *= -1;
#if 0
        // Make 10bit values from 8bit values
        for (int row = 0; row < opencvMatBayer.rows; row++)
        {
            for (int col = 0; col < opencvMatBayer.cols; col++)
            {
                for (int c = 0; c < opencvMatBayer.channels(); c++)
                {
                    opencvMatBayer.at<cv::Vec4w>(row, col).val[c] =
                        opencvMatBayer.at<cv::Vec4w>(row, col).val[c]  (bdiff);
                    // could copy the MSB inside the LSB too
                }
            }
        }
#else
        mult = 1 << bdiff;
#endif
    }
    else if (bdiff > 0)
    {
        mult = 1.0 / (1 << (flxImageBayer.chnl[0].bitDepth - 10));
    }
    opencvMatBayer.convertTo(opencvMatBayer, CV_32F, mult);

#ifdef DEBUG_LOADING
    /*{
        std::ofstream of("input_matf.txt");
        of << "Matrix:" << std::endl;
        printCVBayerTxt<cv::Vec4f>(of, opencvMatBayer);
        of.close();
    }*/
#endif

    return opencvMatBayer;
}

cv::Mat FLXRGB2Mat(const CImageFlx &flxImageBayer)
{
    // Create a openCV Mat 16 bit unsigned to contain the bayer image
    cv::Mat opencvMatRgb(flxImageBayer.height, flxImageBayer.width, CV_16UC3);

    /* Fill in the openCV Mat with the information in the FLX RAW image
     * as unsigned short int (16 bits unsigned) */
    FLX2MatChannels<IMG_UINT16>(flxImageBayer, opencvMatRgb); 

    opencvMatRgb.convertTo(opencvMatRgb, CV_8U);

    return opencvMatRgb;
}

cv::Mat FLXRGB2Mat_10bit(const CImageFlx &flxImageBayer)
{
    // Create a openCV Mat 16 bit unsigned to contain the bayer image
    cv::Mat opencvMatRgb(flxImageBayer.height, flxImageBayer.width, CV_16UC3);

    /* Fill in the openCV Mat with the information in the FLX RAW image
     * as unsigned short int (16 bits unsigned) */
    //  the 3rd parameter should be based on input image bitdepth
    FLX2MatChannels_rshift<IMG_UINT16>(flxImageBayer, opencvMatRgb, 2);

    opencvMatRgb.convertTo(opencvMatRgb, CV_8U);

    return opencvMatRgb;
}

cv::Mat YUV444ToRGBMat(cv::Mat yuv444, bool bBGR, const double convMatrix[9],
    const double convInputOff[3])
{
    cv::Mat rgb(yuv444.rows, yuv444.cols, CV_8UC3);

    int rgb_o[3] = { 0, 1, 2 };  // opencv reads bgr

    if (bBGR)
    {
        rgb_o[1] = 2;
        rgb_o[2] = 1;
    }

    for (int j = 0; j < yuv444.rows; j++)
    {
        for (int i = 0; i < yuv444.cols; i++)
        {
            double a, b, c;
            a = yuv444.at<cv::Vec3b>(j, i).val[0];
            b = yuv444.at<cv::Vec3b>(j, i).val[1];
            c = yuv444.at<cv::Vec3b>(j, i).val[2];

            a += convInputOff[0];
            b += convInputOff[1];
            c += convInputOff[2];

            for (int k = 0; k < 3; k++)
            {
                double v;

                v = a*convMatrix[3 * k + 0]
                    + b*convMatrix[3 * k + 1]
                    + c*convMatrix[3 * k + 2];

                rgb.at<cv::Vec3b>(j, i).val[rgb_o[k]] = (unsigned char)std::max(std::min(255.0, floor(v)), (double)0.0f);
            }
        }
    }

    return rgb;
}
