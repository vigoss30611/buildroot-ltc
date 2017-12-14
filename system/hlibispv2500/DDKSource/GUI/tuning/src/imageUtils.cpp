/**
******************************************************************************
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

#include <stdexcept> // runtime_error (inherit exception and has a constructor with access to the what message)
#include <opencv2/highgui/highgui.hpp>

using namespace std;

//Plot a line followed by a tag
cv::Mat &plotHorizontalTag(cv::Mat &baseImage, cv::Scalar color, int row, int col, int width, string text, int thickness, float size)
{
    cv::line(baseImage, cv::Point(col, baseImage.rows-row),cv::Point(col+width, baseImage.rows-row), color,thickness);
    cv::putText(baseImage,text,cv::Point(col+width+5, baseImage.rows-row), cv::FONT_ITALIC, size,color,thickness);
    return baseImage;
}


//Plot a curve normalized to fit a particular region of the image
cv::Mat &plotCurveRegion(cv::Mat &baseImage, cv::Mat curve, cv::Scalar color, int row, int col, int width, int height, int thickness)
{

    if(curve.rows==1)
    {
        curve = curve.clone();
        curve = curve.t();
    }

    double widthScale = static_cast<double>(width)/static_cast<double>(curve.rows);
    double minVal, maxVal;
    cv::minMaxLoc(curve,&minVal,&maxVal);

    for(int i=1;i<curve.rows;i++)
    {
        cv::line(baseImage, cv::Point(col+int((i-1)*widthScale),baseImage.rows-row-int(cvRound((curve.at<float>(i-1)-minVal)/(maxVal-minVal)*height))),
                 cv::Point(col+int((i  )*widthScale),baseImage.rows-row-int(cvRound((curve.at<float>(i  )-minVal)/(maxVal-minVal)*height))), color,thickness);
    }

    return baseImage;
}

//Plot a curve normalized to fit a particular region of the image
cv::Mat &plotCurveScaled(cv::Mat &baseImage, cv::Mat curve, cv::Scalar color, int row, int col, int width, double heightScale, int thickness)
{

    if(curve.rows==1)
    {
        curve = curve.clone();
        curve = curve.t();
    }

    double widthScale = static_cast<double>(width)/static_cast<double>(curve.rows);
    double minVal, maxVal;
    cv::minMaxLoc(curve,&minVal,&maxVal);

    for(int i=1;i<curve.rows;i++)
    {
        cv::line(baseImage, cv::Point(col+int((i-1)*widthScale),baseImage.rows -row-int(cvRound((curve.at<float>(i-1))*heightScale))),
                 cv::Point(col+int((i  )*widthScale),baseImage.rows -row-int(cvRound((curve.at<float>(i  ))*heightScale))), color,thickness);
    }

    return baseImage;
}

//Plot an histogram or curve in a Mat image. The scalign factor determines the scale of the plot
//in the Y coordinate
cv::Mat &plotCurve(cv::Mat &baseImage, cv::Mat curve, cv::Scalar color, double scaling)
{

    double minVal, maxVal;
    double widthScale = static_cast<double>(baseImage.cols)/static_cast<double>(curve.rows);
    cv::minMaxLoc(curve,&minVal,&maxVal);

    for(int i=1;i<curve.rows;i++)
    {
        cv::line(baseImage, cv::Point(int((i-1)*widthScale),int(baseImage.rows-cvRound(scaling*curve.at<float>(i-1)/maxVal*baseImage.rows))),
                 cv::Point(int(i*widthScale),int(baseImage.rows-cvRound(scaling*curve.at<float>(i)/maxVal*baseImage.rows))), color,2);
    }

    return baseImage;
}


void fillImage(cv::Mat &image, cv::Scalar color)
{
    for(int row=0; row<image.rows; row++)
    {
        for(int col=0; col<image.cols; col++)
        {
            image.at<cv::Vec3b>(row,col)[0] = (unsigned char) color(0);
            image.at<cv::Vec3b>(row,col)[1] = (unsigned char) color(1);
            image.at<cv::Vec3b>(row,col)[2] = (unsigned char) color(2);
        }
    }
}

//simplistic tone mapping curve application (no interpolation)
cv::Mat applyMappingCurve(const cv::Mat &image, const cv::Mat &curve)
{

    cv::Mat mappedImage = cv::Mat(image.rows,image.cols, image.type());

    for(int row=0; row< image.rows; row++)
    {
        for(int col = 0; col<image.cols; col++)
        {
            float pixelValue = image.at<float>(row,col);
            //curve interpolation
            double position = pixelValue*(curve.rows-1);
            int index = (int)floor(position);
            index  = M_CLIP(index,0,curve.rows-2);
            double wIndex = 1.0 -(position-index);

            mappedImage.at<float>(row,col) = float(curve.at<float>(index)*wIndex + curve.at<float>(index+1)*(1-wIndex));
        }
    }

    return mappedImage;
}

//Generate the acumulated curve from an histogram
cv::Mat accumulateHistogram(const cv::Mat &histogram)
{
    cv::Mat accumulatedHistogram = cv::Mat(histogram.rows+1,1, histogram.type());

    accumulatedHistogram.at<float>(0) = 0;
    for(int i = 1; i< histogram.rows; i++)
    {
        accumulatedHistogram.at<float>(i) = (float) M_MIN(1.0,accumulatedHistogram.at<float>(i-1)+histogram.at<float>(i-1));
    }
    accumulatedHistogram.at<float>(histogram.rows) = 1;

    return accumulatedHistogram;
}

//Receives a normalized histogram and calculated the threshold in which we reach a certain
// amount of image data
double getHistogramThreshold(cv::Mat histogram, double threshold, bool reverse)
{
    double acumulated = 0;

    for(int i=0;i<histogram.rows;i++)
    {
        if(!reverse)
        {
            acumulated += histogram.at<float>(i);
        }
        else
        {
            acumulated += histogram.at<float>(histogram.rows-i-1);
        }

        if(acumulated>=threshold)
        {
            if(!reverse)
            {
                return double(i)/histogram.rows;
            }
            else
            {
                return double(histogram.rows-i-1)/histogram.rows;
            }

        }
    }

    return 1.0;
}

double roundDouble(double value, double factor)
{
    return floor(value*factor+0.5)/factor;
}


//Conversion of a CImageFlx to a opencv Mat image
cv::Mat FLXYUV2LumaMat(CImageFlx &flxImageYUV)
{
    //create a float luma plane with the flx image dimensions
    cv::Mat lumaPlane = cv::Mat(flxImageYUV.height, flxImageYUV.width, CV_32FC1);

    for(int row=0;row<flxImageYUV.height;row++)
    {
        for(int col=0;col<flxImageYUV.width;col++)
        {
            lumaPlane.at<float>(row,col) = static_cast<float>(*(flxImageYUV.chnl[0].data+row*flxImageYUV.chnl[0].chnlWidth+col))/(1<<flxImageYUV.chnl[0].bitDepth);
        }
    }

    return lumaPlane;
}


//Function for loading a CImageFlx with YUV format
cv::Mat loadYUVFlxFile(const std::string &filename) throw(std::exception)
{
    CImageFlx flxImage;
    const char *status = flxImage.LoadFileHeader(filename.c_str(),NULL);        //header must be loaded first
    if(status)
        throw std::runtime_error(status);

    //after reading the header we can load the file data. We will read the first frame only.
    status = flxImage.LoadFileData(0);
    if(status)
        throw std::runtime_error(status);

    //check that we have received an YUV Image
    if(flxImage.colorModel != CImageBase::CM_YUV)
    {
        flxImage.Unload();
        throw std::runtime_error("Invalid Flx file: must be YUV format");
    }

    cv::Mat loadedImage = FLXYUV2LumaMat(flxImage)+0.5; //we add 0.5 to make the range positive between 0.0 and 1.0 as the image is signed luma in the -0.5 to 0.5 range
    flxImage.Unload();  //free the flx image

    return loadedImage;
}


//Load a FLX BAYER file into a 4 channel cv::Mat image
//Image will be scaled to 10bit per channel
cv::Mat loadBAYERFlxFile(const std::string &filename) throw(std::exception)
{
    CImageFlx flxImage;
    const char *status = flxImage.LoadFileHeader(filename.c_str(),NULL);        //header must be loaded first
    if(status)
        throw std::runtime_error(status);

    //after reading the header we can load the file data. We will read the first frame only.
    status = flxImage.LoadFileData(0);
    if(status)
        throw std::runtime_error(status);

    //check that we have received an YUV Image
    if(flxImage.colorModel != CImageBase::CM_RGGB)
    {
        flxImage.Unload();
        throw std::runtime_error("Invalid Flx file: must be RGGB format");
    }

    cv::Mat loadedImage = FLXBAYER2MatMosaic(flxImage);
    flxImage.Unload();  //free the flx image

    return loadedImage;
}



//Display an image with opencv utils
void displayImage(const cv::Mat &image, const std::string &title, int posX, int posY, int  width,int height)
{
    cv::namedWindow(title, cv::WINDOW_NORMAL); // cv::WINDOW_NORMAL is not defined on linux opencv2.3
    cv::imshow(title, image);
    cv::moveWindow(title, posX, posY); // available in opencv 2.4 - but linux uses opencv2.3
    cv::resizeWindow(title, width,height);
}

//Copy an FLX raw image to a single Bayer plane opencv Mat type
// cannot use const CImageFlx & because GetSubsampling() is not const
template <typename T>
void FLXRAW2Mat(CImageFlx &flxImage, cv::Mat &openCVImage)
{
    int chnlConvRGGB[] = {0,1,2,3};
    int chnlConvGRBG[] = {1,0,3,2};
    int *chnlConv = chnlConvRGGB; //default

    //We have to reconstruct the original BAYER pattern pixel layout (data in FLX is loaded in RGGB order)
    switch(flxImage.GetSubsampling())
    {
    case CImageBase::SUBS_RGGB:
        chnlConv = chnlConvRGGB;
        break;
    case CImageBase::SUBS_GRBG:
        chnlConv = chnlConvGRBG;
        break;
    case CImageBase::SUBS_GBRG:
        std::cout <<"WARNING: SUBS_GBRG FLX conversion not supported properly!"<<std::endl;
        chnlConv = chnlConvRGGB; //default conversion
        break;
    case CImageBase::SUBS_BGGR:
        std::cout <<"WARNING: SUBS_BGGR FLX conversion not supported properly!"<<std::endl;
        chnlConv = chnlConvRGGB; //default conversion
        break;
    default:
        chnlConv = chnlConvRGGB; //default conversion
        break;
    }

    for(int row=0;row<flxImage.height/2;row++)
    {
        for(int col=0;col<flxImage.width/2;col++)
        {
            openCVImage.at<T>(row*2,col*2)              = static_cast<T>(*(flxImage.chnl[chnlConv[0]].data+(row)*flxImage.chnl[chnlConv[0]].chnlWidth+col));
            openCVImage.at<T>(row*2,col*2+1)    = static_cast<T>(*(flxImage.chnl[chnlConv[1]].data+(row)*flxImage.chnl[chnlConv[1]].chnlWidth+col));
            openCVImage.at<T>(row*2+1,col*2)    = static_cast<T>(*(flxImage.chnl[chnlConv[2]].data+(row)*flxImage.chnl[chnlConv[2]].chnlWidth+col));
            openCVImage.at<T>(row*2+1,col*2+1)  = static_cast<T>(*(flxImage.chnl[chnlConv[3]].data+(row)*flxImage.chnl[chnlConv[3]].chnlWidth+col));
        }
    }
}

//Copy an FLX raw image to a 4 planes (RGGB) Mat image
template <typename T>
void FLXRAW2MatChannels(const CImageFlx &flxImage, cv::Mat &openCVImage)
{
    for(int row=0;row<flxImage.height/2;row++)
    {
        for(int col=0;col<flxImage.width/2;col++)
        {
            openCVImage.at<cv::Vec4w>(row,col).val[0]= static_cast<T>(*(flxImage.chnl[0].data+(row)*flxImage.chnl[0].chnlWidth+col));
            openCVImage.at<cv::Vec4w>(row,col).val[1]= static_cast<T>(*(flxImage.chnl[1].data+(row)*flxImage.chnl[1].chnlWidth+col));
            openCVImage.at<cv::Vec4w>(row,col).val[2]= static_cast<T>(*(flxImage.chnl[2].data+(row)*flxImage.chnl[2].chnlWidth+col));
            openCVImage.at<cv::Vec4w>(row,col).val[3]= static_cast<T>(*(flxImage.chnl[3].data+(row)*flxImage.chnl[3].chnlWidth+col));
        }
    }
}

template <typename T>
void FLX2MatChannels(const CImageFlx &flxImage, cv::Mat &openCVImage)
{
    /*for (int c = 0 ; c < 3 ; c++ )
      {
      for ( int row = 0 ; row < flxImage.chnl[c].chnlHeight ; row++ )
      {
      for ( int col  = 0 ; col < flxImage.chnl[c].chnlWidth ; col++ )
      {
      openCVImage.at<cv::Vec3w>(row,col).val[c]= static_cast<T>(*(flxImage.chnl[c].data+(row)*flxImage.chnl[c].chnlWidth+col));
      }
      }
      }*/
    for(int row=0;row<flxImage.height;row++)
    {
        for(int col=0;col<flxImage.width;col++)
        {
            openCVImage.at<cv::Vec3w>(row,col).val[0]= static_cast<T>(*(flxImage.chnl[0].data+(row)*flxImage.chnl[0].chnlWidth+col));
            openCVImage.at<cv::Vec3w>(row,col).val[1]= static_cast<T>(*(flxImage.chnl[1].data+(row)*flxImage.chnl[1].chnlWidth+col));
            openCVImage.at<cv::Vec3w>(row,col).val[2]= static_cast<T>(*(flxImage.chnl[2].data+(row)*flxImage.chnl[2].chnlWidth+col));
            //openCVImage.at<cv::Vec4w>(row,col).val[3]= static_cast<T>(*(flxImage.chnl[3].data+(row)*flxImage.chnl[3].chnlWidth+col));
        }
    }
}

template <typename T>
void FLX2MatChannels_10bit(const CImageFlx &flxImage, cv::Mat &openCVImage)
{
    /*for (int c = 0 ; c < 3 ; c++ )
      {
      for ( int row = 0 ; row < flxImage.chnl[c].chnlHeight ; row++ )
      {
      for ( int col  = 0 ; col < flxImage.chnl[c].chnlWidth ; col++ )
      {
      openCVImage.at<cv::Vec3w>(row,col).val[c]= static_cast<T>(*(flxImage.chnl[c].data+(row)*flxImage.chnl[c].chnlWidth+col));
      }
      }
      }*/
    for(int row=0;row<flxImage.height;row++)
    {
        for(int col=0;col<flxImage.width;col++)
        {
            openCVImage.at<cv::Vec3w>(row,col).val[0]= static_cast<T>(*(flxImage.chnl[0].data+(row)*flxImage.chnl[0].chnlWidth+col)>>2);
            openCVImage.at<cv::Vec3w>(row,col).val[1]= static_cast<T>(*(flxImage.chnl[1].data+(row)*flxImage.chnl[1].chnlWidth+col)>>2);
            openCVImage.at<cv::Vec3w>(row,col).val[2]= static_cast<T>(*(flxImage.chnl[2].data+(row)*flxImage.chnl[2].chnlWidth+col)>>2);
            //openCVImage.at<cv::Vec4w>(row,col).val[3]= static_cast<T>(*(flxImage.chnl[3].data+(row)*flxImage.chnl[3].chnlWidth+col));
        }
    }
}

//Create a four channel opencv Mat image from a FLX RGGB bayer image
cv::Mat FLXBAYER2MatMosaic(const CImageFlx &flxImageBayer)
{
    //Create a openCV Mat 16 bit unsigned to contain the bayer image
    cv::Mat opencvMatBayer(flxImageBayer.height/2, flxImageBayer.width/2, CV_16UC4);

    //Fill in the openCV Mat with the information in the FLX RAW image
    FLXRAW2MatChannels<unsigned short int>(flxImageBayer, opencvMatBayer); //fill it as unsigned short int (16 bits unsigned)

    opencvMatBayer.convertTo(opencvMatBayer,CV_32F, 1.0/(1<<(flxImageBayer.chnl[0].bitDepth-10)));//We assume that all the channels have the same bitdepth as the first channel
    //opencvMatBayer.convertTo(opencvMatBayer,CV_32F, 1.0/(1<<(flxImageBayer.chnl[0].bitDepth)));//We assume that all the channels have the same bitdepth as the first channel

    return opencvMatBayer;
}

cv::Mat FLXRGB2Mat(const CImageFlx &flxImageBayer)
{
    //Create a openCV Mat 16 bit unsigned to contain the bayer image
    cv::Mat opencvMatRgb(flxImageBayer.height, flxImageBayer.width, CV_16UC3);

    //Fill in the openCV Mat with the information in the FLX RAW image
    FLX2MatChannels<unsigned short int>(flxImageBayer, opencvMatRgb); //fill it as unsigned short int (16 bits unsigned)

    opencvMatRgb.convertTo(opencvMatRgb,CV_8U);

    return opencvMatRgb;
}

cv::Mat FLXRGB2Mat_10bit(const CImageFlx &flxImageBayer)
{
    //Create a openCV Mat 16 bit unsigned to contain the bayer image
    cv::Mat opencvMatRgb(flxImageBayer.height, flxImageBayer.width, CV_16UC3);

    //Fill in the openCV Mat with the information in the FLX RAW image
    FLX2MatChannels_10bit<unsigned short int>(flxImageBayer, opencvMatRgb); //fill it as unsigned short int (16 bits unsigned)

    opencvMatRgb.convertTo(opencvMatRgb,CV_8U);

    return opencvMatRgb;
}

//Create a demosaiced openCV RGB mat type from a FLX RAW BAYER image
// cannot use const CImageFlx & because GetSubsampling() is not const
cv::Mat FLXBAYER2Mat(CImageFlx &flxImageBayer)
{
    int conversionFormat=CV_BayerRG2RGB; //use of _VNG is optional
    switch(flxImageBayer.GetSubsampling())
    {
    case CImageBase::SUBS_RGGB:
        conversionFormat = CV_BayerRG2RGB;
        break;
    case CImageBase::SUBS_GRBG:
        conversionFormat = CV_BayerGR2RGB;
        break;
    case CImageBase::SUBS_GBRG:
        std::cout <<"WARNING: SUBS_GBRG FLX conversion not supported properly!"<<std::endl;
        conversionFormat = CV_BayerRG2RGB;
        break;
    case CImageBase::SUBS_BGGR:
        std::cout <<"WARNING: SUBS_BGGR FLX conversion not supported properly!"<<std::endl;
        conversionFormat = CV_BayerRG2RGB;
        break;

    default:
        conversionFormat = CV_BayerRG2RGB;
        break;
    }

    //Create a openCV Mat 16 bit unsigned to contain the bayer image
    cv::Mat opencvMatBayer(flxImageBayer.height, flxImageBayer.width, CV_16UC1);

    //Fill in the openCV Mat with the information in the FLX RAW image
    FLXRAW2Mat<unsigned short int>(flxImageBayer, opencvMatBayer); //fill it as unsigned short int (16 bits unsigned)

    //Create the target RGB image
    cv::Mat opencvMat(flxImageBayer.height, flxImageBayer.width, /*CV_8UC3*//*CV_16SC3*/ /*CV_32FC3*/CV_16UC3);


    //Demosaic process with cvtColor. There is also a _VNG demosaicing algorithm but only supports U8
    cv::cvtColor(opencvMatBayer,opencvMat,conversionFormat,3);

    //Convert the result to normalized 32 bit float Mat
    opencvMat.convertTo(opencvMat,CV_32F, 1.0/(1<<flxImageBayer.chnl[0].bitDepth));//We assume that all the channels have the same bitdepth as the first channel

    return opencvMat;
}

// converts a 444 YUV (CV_8UC3) into RGB matrix (CV_8UC3)
//
cv::Mat YUV444ToRGBMat(cv::Mat yuv444, bool bBGR, const double convMatrix[9], const double convInputOff[3])
{
    cv::Mat rgb(yuv444.rows, yuv444.cols, CV_8UC3);

    int rgb_o[3] = { 0, 1, 2 }; // opencv reads bgr

    if ( bBGR )
    {
        rgb_o[1] = 2;
        rgb_o[2] = 1;
    }

    for ( int j = 0 ; j < yuv444.rows ; j++ )
    {
        for ( int i = 0 ; i < yuv444.cols ; i++ )
        {
            double a,b,c;
                a = yuv444.at<cv::Vec3b>(j, i).val[0];
                b = yuv444.at<cv::Vec3b>(j, i).val[1];
                c = yuv444.at<cv::Vec3b>(j, i).val[2];
                
            a += convInputOff[0];
            b += convInputOff[1];
            c += convInputOff[2];
                
            for( int k = 0 ; k < 3 ; k++ )
            {
                double v;
                
                v = a*convMatrix[3*k+0]
                    + b*convMatrix[3*k+1]
                    + c*convMatrix[3*k+2];

                rgb.at<cv::Vec3b>(j, i).val[rgb_o[k]] = (unsigned char)std::max(std::min(255.0, floor(v)), (double)0.0f);
            }
        }
    }

    return rgb;
}
