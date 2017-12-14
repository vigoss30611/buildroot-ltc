/**
******************************************************************************
@file JpegExif.h

@brief Interface of Exif writer for v2500 HAL module

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

*****************************************************************************/

#ifndef JPEG_EXIF_H
#define JPEG_EXIF_H

#include <utils/Errors.h>
#include <cutils/properties.h>

extern "C" {
#include <libexif/exif-data.h>
}

#include "JpegEncoder.h"
#include "FelixMetadata.h"

namespace android {

class JpegBasicIo;

/**
 * @brief Responsible of generating EXIF with thumbnail
 */
class ExifWriter {
private:
    /**
     * @brief TAG_MAKE string
     * @note read from ro.product.manufacturer
     */
    char HAL_TAG_MAKE[PROP_VALUE_MAX];
    /**
     * @brief TAG_MODEL string
     * @note read from ro.product.model
     */
    char HAL_TAG_MODEL[PROP_VALUE_MAX];

    static const uint8_t M_APP0  = 0xe0;
    static const uint8_t M_APP1  = 0xe1;
    static const uint16_t APP0_LEN = 4 + 4 + 2 + 2 + 2;
    static const ExifByteOrder FILE_BYTE_ORDER = EXIF_BYTE_ORDER_INTEL;

public:
    /**
     * @brief constructor
     *
     * @param jpegIo basic io interface of the JpegEncoder class
     * @param metadata metadata object for Exif
     */
    ExifWriter(JpegBasicIo &jpegIo, sp<FelixMetadata>& metadata);

    /**
     * @brief destructor
     */
    virtual ~ExifWriter();

    /*
     * @brief write APP1(EXIF) data block into current buffer position
     *
     * @param image_width width of the image
     * @param image_height height of the image
     * @param thumbnail pointer to raw compressed thumbnail data
     * @param thumbnailSize size of raw thumbnail buffer
     *
     * @return eror code
     */
    status_t writeApp1(uint32_t image_width, uint32_t image_height,
            uint8_t* thumbnail = NULL, int thumbnailSize = 0,
            bool isRgb = false);

private:
    /*
     * @brief Get an existing tag, or create one if it doesn't exist
     *
     * @note reused from libexif -> write-exif.c
     */
    ExifEntry* initTag(ExifData *exif, ExifIfd ifd, ExifTag tag);

    /*
     * @brief Create a brand-new tag with a data field of the given length,
     * in the given IFD. This is needed when exif_entry_initialize() isn't
     * able to create this type of tag itself, or the default data length it
     * creates isn't the correct length.
     *
     * @note reused from libexif -> write-exif.c
     */
    ExifEntry *createTag(ExifData *exif, ExifIfd ifd, ExifTag tag,
            ExifFormat format, unsigned long components);

    /**
     * @brief converts rotation [0,90,180,270] to exif enum
     */
    ExifShort rotationToExif(const uint32_t degrees);

    /**
     * @brief converts floating point to Exif rational format
     */
    void doubleToRational(const double& input, ExifRational& output);

    /**
     * @brief converts floating point degrees to degrees:MM:SS.SSSS format
     * @note output is in rational numbers
     */
    void degreesToRationalHMS(const double input,
            ExifRational& degrees,
            ExifRational& minutes,
            ExifRational& seconds);

    /**
     * @brief Android log handler for libexif
     */
    static void logfunc(ExifLog *log, ExifLogCode, const char *domain,
              const char *format, va_list args, void *data);

    ExifData* exif; ///<@brief libExif handle
    ExifLog* log;   ///<@brief libExif log handle
    sp<FelixMetadata> mMeta; ///<@brief local copy of Android metadata
    JpegBasicIo&   mIo;    ///<@brief basic io operations adapter
    status_t status;            ///<@brief internal status of ExifWriter object
};

} // namespace android

#endif /* JPEG_EXIF_H */
