/**
*******************************************************************************
@file buffer.hpp

@brief Genric buffer class using OpenCV matrices to store data

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
#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <img_types.h>
#include <felixcommon/pixel_format.h>
#include <opencv2/core/core.hpp>

#include <QtExtra/exception.hpp>

class QImage;

#ifdef WIN32
/* disable C4290: C++ exception specification ignored except to indicate a
* function is not __declspec(nothrow) */
#pragma warning( disable : 4290 )
#endif

struct CVBuffer
{
    /** @brief  original mosaic */
    enum MOSAICType mosaic;
    /** @brief original pixel format */
    ePxlFormat pxlFormat;

    cv::Mat data;

    CVBuffer();

    static void mosaicInfo(enum MOSAICType mosaic, int &m_r, int &m_g1,
        int &m_g2, int &m_b);
    static CVBuffer loadFile(const char *filename) throw(QtExtra::Exception);
    cv::Scalar computeAverages(const cv::Rect &roi) const;

    /**
     * @brief demosaiced by averaging green channels (return a clone as BGR)
     */
    CVBuffer demosaic() const;
    CVBuffer demosaic(double rescale, int type) const;
    static double gammaCorrect(double v);
    CVBuffer gammaCorrect() const;

    QImage *convertToQImage() const;

    CVBuffer clone() const;
};

#endif // BUFFER_HPP
