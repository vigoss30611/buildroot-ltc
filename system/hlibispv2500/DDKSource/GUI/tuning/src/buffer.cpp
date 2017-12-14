/**
******************************************************************************
@file buffer.cpp

@brief Implementation of CVBuffer

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
#include "buffer.hpp"

#include "image.h"
#include <QtExtra/exception.hpp>
#include <QImage>
#include <felixcommon/pixel_format.h>

#include <cmath>
#include <cstdio>
#include <img_errors.h>

#include "imageUtils.h"

/*
 * CVBuffer
 */

void CVBuffer::mosaicInfo(enum MOSAICType mosaic, int &m_r, int &m_g1, int &m_g2, int &m_b)
{
    switch(mosaic)
    {
    case MOSAIC_GRBG:
        m_r=1; m_g1=0; m_g2=3; m_b=2;
        break;
    case MOSAIC_GBRG:
        m_r=2; m_g1=3; m_g2=0; m_b=1;
        break;
    case MOSAIC_BGGR:
        m_r=3; m_g1=2; m_g2=1; m_b=0;
        break;

    case MOSAIC_RGGB:
    default:
        m_r=0; m_g1=1; m_g2=2; m_b=3;
        break;
    }
}

CVBuffer::CVBuffer()
{
    mosaic = MOSAIC_NONE;
    pxlFormat = PXL_NONE;
}

CVBuffer CVBuffer::loadFile(const char *filename) throw(QtExtra::Exception)
{
    CVBuffer sOutput;
    CMetaData *pMetaData;
    const char *err;
    CImageFlx flx;
    int i, nFrames;

    if ( (err=flx.LoadFileHeader(filename, NULL)) != NULL )
    {
        throw(QtExtra::Exception(QObject::tr("Failed to load FLX header"), err));
    }

    nFrames = flx.GetNFrames(-1);

    i = 0;
    if ( nFrames > 0 ) //for ( i = 0 ; i < nFrames ; i++ )
    {
        int b;

        if ( (err=flx.LoadFileData(i)) != NULL )
        {
            throw(QtExtra::Exception(QObject::tr("Failed to load data for frame %n", "", i)));
        }

        if ( (pMetaData = flx.GetMetaForFrame(i)) == NULL )
        {
            throw(QtExtra::Exception(QObject::tr("Failed to load meta data for frame %n", "", i)));
        }

        int chnBit[4] = {10, 10, 10, 10};
        const char *meta = pMetaData->GetMetaStr(FLXMETA_BITDEPTH);
        b = sscanf(meta, "%d %d %d %d", &chnBit[0], &chnBit[1], &chnBit[2], &chnBit[3]);

        sOutput.pxlFormat = PXL_NONE;

        if ( flx.colorModel == CImageBase::CM_RGGB )
        {
            switch(flx.subsMode)
            {
            case CImageBase::SUBS_GRBG:
                sOutput.mosaic = MOSAIC_GRBG;
                break;
            case CImageBase::SUBS_GBRG:
                sOutput.mosaic = MOSAIC_GBRG;
                break;
            case CImageBase::SUBS_BGGR:
                sOutput.mosaic = MOSAIC_BGGR;
                break;
            case CImageBase::SUBS_RGGB:
            default:
                sOutput.mosaic = MOSAIC_RGGB;
                break;
            }

            switch(chnBit[0]) // assumes all channel have same bitdepth
            {
            case 8:
                sOutput.pxlFormat = BAYER_RGGB_8;
                break;
            case 10:
                sOutput.pxlFormat = BAYER_RGGB_10;
                break;
            case 12:
                sOutput.pxlFormat = BAYER_RGGB_12; // which is not really possible because data is scaled to 10b
                break;
            }

            try
            {
                sOutput.data = FLXBAYER2MatMosaic(flx);
            }
            catch(std::exception e)
            {
                throw(QtExtra::Exception(QObject::tr("Failed to load a Bayer image"), e.what()));
            }
        }
        else
        {
            throw(QtExtra::Exception(QObject::tr("Unsupported FLX format found for frame %n", "", i), pMetaData->GetMetaStr("COLOUR_FORMAT", "format not found")));
        }
    }

    return sOutput;
}

cv::Scalar CVBuffer::computeAverages(const cv::Rect &roi) const
{
    cv::Scalar avg;

    cv::Mat patch(this->data, roi);
    avg = cv::mean(patch);

    return avg;
}

CVBuffer CVBuffer::demosaic() const
{
    CVBuffer result;

    if ( this->mosaic != MOSAIC_NONE )
    {
        cv::Mat crudeDmo[3];//, demosaiced(this->data.rows, this->data.cols, CV_16UC3);
        std::vector<cv::Mat> channels;

        result.data = cv::Mat(this->data.rows, this->data.cols, CV_8UC3); // result

        cv::split(this->data, channels);

        // RGB stored as BGR in openCV
        crudeDmo[2] = channels[0].clone();
        crudeDmo[1] = channels[1].clone()*0.5 + channels[2].clone()*0.5;
        crudeDmo[0] = channels[3].clone();

        cv::merge(crudeDmo, 3, result.data);

        //demosaiced.convertTo(result.data,CV_8U, 1/4.0);
        result.mosaic = MOSAIC_NONE;
        switch(this->pxlFormat)
        {
        case BAYER_RGGB_8:
            result.pxlFormat = RGB_888_24;
            break;
        case BAYER_RGGB_10:
            result.pxlFormat = RGB_101010_32;
            break;
        default:
            result.pxlFormat = PXL_NONE;
            break;
        }
    }
    else
    {
        std::cerr << "try to demosaic an already demosaiced image" << std::endl;
        result = this->clone();
    }

    return result;
}

CVBuffer CVBuffer::demosaic(double factor, int type) const
{
    CVBuffer result = demosaic();
    cv::Mat rescaled;//(result.data.rows, result.data.cols, type);

    result.data.convertTo(rescaled, type, factor);
    result.data = rescaled;
    return result;
}

double CVBuffer::gammaCorrect(double v)
{
    return pow(v/255.0, 1/2.2)*255.0;
}

CVBuffer CVBuffer::gammaCorrect() const
{
    CVBuffer sOutput = this->clone();

    if ( this->data.depth() == CV_8U )
    {
        std::vector<cv::Mat> channels;

        cv::split(sOutput.data, channels);
        for (unsigned int c = 0 ; c < channels.size() ; c++ )
        {
            cv::MatIterator_<unsigned char> it = channels[c].begin<unsigned char>(),
                it_end = channels[c].end<unsigned char>();

            for ( ; it != it_end ; ++it )
            {
                double v = std::max((double)*it, 0.0);

                *it = (unsigned char)std::floor(CVBuffer::gammaCorrect(v));
            }
        }

        cv::merge(channels, sOutput.data);
    }

    return sOutput;
}

QImage *CVBuffer::convertToQImage() const
{
    QImage *pImage = 0;
    cv::Mat opencvMat(this->data.rows, this->data.cols, CV_8UC3); // result
    bool bSwap = true;
    QImage::Format fmt = QImage::Format_RGB888;
    PIXELTYPE sYUVType;
    bool bYUV = false;

    if ( PixelTransformYUV(&sYUVType, this->pxlFormat) == IMG_SUCCESS )
    {
        bYUV = true;
    }

    if ( this->mosaic != MOSAIC_NONE )
    {
        // demosaiced result
        try
        {
            opencvMat = this->demosaic(1/4.0, CV_8U).data;
        }
        catch(std::exception ex)
        {
            QtExtra::Exception e(QObject::tr("Failed to demosaic image for display"), ex.what());
            e.displayQMessageBox(0);
        }
    }
    else if ( bYUV )
    {
        //unsigned char *pRGB = 0;

        //switch(settings->conversionStd())
        //{
        //case STD_BT709:
        opencvMat = YUV444ToRGBMat(this->data, false, BT709_coeff, BT709_Inputoff); // if not swapped does not work...
        bSwap = false;
        //      break;
        //case STD_BT601:
        //      pRGB = BufferTransformYUV444ToRGB(p444, IMG_TRUE, RGB_888_24, IMG_TRUE, yuv444stride, yuv444height, BT601_coeff, BT601_Inputoff);
        //      break;
        //case STD_JFIF:
        //      pRGB = BufferTransformYUV444ToRGB(p444, IMG_TRUE, RGB_888_24, IMG_TRUE, yuv444stride, yuv444height, JFIF_coeff, JFIF_Inputoff);
        //      break;
        //default:
        //      //error(tr("Unsupported conversion format selected"));
        //}

        //free(pRGB);
    }
    else // rgb
    {
        opencvMat = this->data;

        if ( opencvMat.channels() == 1 )
        {
            fmt = QImage::Format_MonoLSB;
        }

		// Swap for BGR formats
		if(pxlFormat >= BGR_888_24 && pxlFormat <= BGR_101010_32)
		{
			bSwap = false;
		}
    }

#if 0
    pImage = new QImage(opencvMat.data, opencvMat.cols, opencvMat.rows, opencvMat.step, QImage::Format_RGB888);
#else
    QImage image(opencvMat.data, opencvMat.cols, opencvMat.rows, opencvMat.step, fmt);
    if ( bSwap )
    {
        pImage = new QImage(image.rgbSwapped());
    }
    else
    {
        pImage = new QImage(image.copy());
    }
#endif

    return pImage;
}

CVBuffer CVBuffer::clone() const
{
    CVBuffer cpy = *this;
    cpy.data = this->data.clone();
    return cpy;
}
