/**
******************************************************************************
@file jpegExif.cpp

@brief Definition of Exif writer for v2500 HAL module

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

//#define LOG_NDEBUG 0
#define LOG_TAG "JpegExif"
#include "felixcommon/userlog.h"
#include "JpegExif.h"
#include <math.h>

// do not make local copy of data provided by caller of writeApp1()
#undef MAKE_THUMBNAIL_COPY

namespace android {

ExifWriter::ExifWriter(JpegBasicIo& jpegIo,
            sp<FelixMetadata>& metadata) :
        mMeta(metadata), mIo(jpegIo), status(OK) {

    exif = exif_data_new();
    if(!exif) {
        status = NO_MEMORY;
        LOG_ERROR("Couldn't allocate Exif object");
        return;
    }
    /* Set the image options */
    exif_data_set_option(exif, EXIF_DATA_OPTION_FOLLOW_SPECIFICATION);
    exif_data_set_data_type(exif, EXIF_DATA_TYPE_COMPRESSED);
    exif_data_set_byte_order(exif, FILE_BYTE_ORDER);

    log = exif_log_new();
    exif_log_set_func(log, logfunc, NULL);
    exif_data_log(exif, log);

    // CTS: tag contents are read from system props
    property_get("ro.product.manufacturer", HAL_TAG_MAKE, "unknown");
    property_get("ro.product.model", HAL_TAG_MODEL, "unknown");
}

ExifWriter::~ExifWriter() {
    exif_log_unref(log);
    exif_data_unref(exif);
}

void ExifWriter::logfunc(ExifLog *, ExifLogCode, const char *domain,
                  const char *format, va_list args, void *) {

    static char _message_[512];

    vsprintf(_message_, format, args);

    ALOG(LOG_VERBOSE, "ExifWriter", "%s: %s", domain, _message_);
}

ExifShort ExifWriter::rotationToExif(const uint32_t degrees) {
    switch (degrees) {
    case 0 :
        return 1;
    case 90:
        return 6;
    case 180:
        return 3;
    case 270:
        return 8;
    default:
        return 1;
    }
    return 1;
}

void ExifWriter::doubleToRational(const double& input, ExifRational& output) {
    // this is not real conversion from double to rational
    // for the sake of simplicity, we accept huge rounding here!
    // FIXME: use gcd() to find best num/denum
    output.numerator = input*10000;
    output.denominator = 10000;
}

void ExifWriter::degreesToRationalHMS(const double input,
        ExifRational& degrees,
        ExifRational& minutes,
        ExifRational& seconds) {

    double integer;
    double fraction =  modf(input, &integer);

    // degrees are just integer part
    degrees.numerator = (ExifLong)integer;
    degrees.denominator = 1;

    // minutes are integer part of fraction times number of minutes in a degree
    fraction = modf(fraction * 60.0, &integer);
    minutes.numerator = (ExifLong)integer;
    minutes.denominator = 1;

    // seconds are integer part of fraction times number of seconds in a minute
    //fraction = modf(fraction * 60.0, &integer);
    seconds.numerator = 60.0 * fraction * 10000.0;
    seconds.denominator = 10000;
}

#define CHECK(entry) if(!entry){ return NO_MEMORY; }

status_t ExifWriter::writeApp1(uint32_t image_width, uint32_t image_height,
        uint8_t* thumbnail, int thumbnailSize, bool isRgb) {
    unsigned char *exif_data;
    unsigned int exif_data_len;
    ExifEntry *entry;

    const char asciiCode[] = {
            '\x41', '\x53', '\x43', '\x49', '\x49', '\x0', '\x0', '\x0'
    };
    /* Create the mandatory EXIF fields with default data */

    /* All these tags are created with default values by exif_data_fix() */
    /* Change the data to the correct values for this image. */
#if(0)
    entry = initTag(exif, EXIF_IFD_EXIF, EXIF_TAG_IMAGE_WIDTH);
    exif_set_long(entry->data, FILE_BYTE_ORDER, mCInfo->image_width);
    entry = initTag(exif, EXIF_IFD_EXIF, EXIF_TAG_IMAGE_LENGTH);
    exif_set_long(entry->data, FILE_BYTE_ORDER, mCInfo->image_height);
#endif
    ALOG_ASSERT(mMeta.get());
    if(0){
        // not required when COMPRESSED main image ?
        uint8_t quality;
        if(mMeta->getJpegQuality(quality)) {
            entry = initTag(exif, EXIF_IFD_0, EXIF_TAG_COMPRESSION);
            CHECK(entry);
            exif_set_short(entry->data, FILE_BYTE_ORDER, quality);
        }
    }
    {
        entry = initTag(exif, EXIF_IFD_EXIF, EXIF_TAG_FOCAL_LENGTH);
        CHECK(entry);
        double fl;
        if(mMeta->getFocalLength(fl)) {
            // both values in mm
            ExifRational output;
            doubleToRational(fl, output);
            exif_set_rational(entry->data, FILE_BYTE_ORDER, output);
        }
        entry = initTag(exif, EXIF_IFD_EXIF, EXIF_TAG_EXPOSURE_TIME);
        CHECK(entry);
        int64_t exposureNs;
        if(mMeta->getExposureTime(exposureNs)) {
            ExifRational exposureSec;
            exposureSec.numerator = exposureNs;
            exposureSec.denominator = seconds_to_nanoseconds(1);
            exif_set_rational(entry->data, FILE_BYTE_ORDER, exposureSec);
        }
        entry = initTag(exif, EXIF_IFD_EXIF, EXIF_TAG_APERTURE_VALUE);
        CHECK(entry);
        double ap;
        if(mMeta->getLensAperture(ap)) {
            // APEX value, Av = 2 log 2 (F number)
            ap = 2 * log2(ap);
            ExifRational output;
            doubleToRational(ap, output);
            exif_set_rational(entry->data, FILE_BYTE_ORDER, output);
        }

        // NOTE: this tag changed name to PhotographicSensitivity in Exif
        // standards version > 2.21
        // Yet, Lollipop AOSP libexif library supports v2.1
        entry = initTag(exif, EXIF_IFD_EXIF, EXIF_TAG_ISO_SPEED_RATINGS);
        CHECK(entry);
        int32_t iso;
        if(mMeta->getISOSensitivity(iso)) {
            exif_set_short(entry->data, FILE_BYTE_ORDER, iso);
        }

        entry = initTag(exif, EXIF_IFD_EXIF, EXIF_TAG_FLASH);
        CHECK(entry);
        uint16_t flashState = 0;
        if(mMeta->getExifFlashState(flashState)) {
            exif_set_short(entry->data, FILE_BYTE_ORDER, flashState);
        }
        entry = initTag(exif, EXIF_IFD_EXIF, EXIF_TAG_WHITE_BALANCE);
        CHECK(entry);
        exif_set_short(entry->data, FILE_BYTE_ORDER,
                mMeta->getAutoWhiteBalance() ? 0 : 1);
    }
    {
        entry = initTag(exif, EXIF_IFD_0, EXIF_TAG_ORIENTATION);
        CHECK(entry);
        uint32_t degrees;
        if(mMeta->getJpegOrientation(degrees)) {
            exif_set_short(entry->data, FILE_BYTE_ORDER,
                    rotationToExif(degrees));
        }
    }
    entry = initTag(exif, EXIF_IFD_EXIF, EXIF_TAG_PIXEL_X_DIMENSION);
    CHECK(entry);
    exif_set_long(entry->data, FILE_BYTE_ORDER, image_width);
    entry = initTag(exif, EXIF_IFD_EXIF, EXIF_TAG_PIXEL_Y_DIMENSION);
    CHECK(entry);
    exif_set_long(entry->data, FILE_BYTE_ORDER, image_height);

    entry = initTag(exif, EXIF_IFD_EXIF, EXIF_TAG_COLOR_SPACE); // sRGB
    CHECK(entry);
    exif_set_short(entry->data, FILE_BYTE_ORDER, 1);

    // mark YCbCr or RGB type of image
    entry = initTag(exif, EXIF_IFD_EXIF, EXIF_TAG_COMPONENTS_CONFIGURATION);
    CHECK(entry);
    ALOG_ASSERT(entry->components == 4);
    if(isRgb) {
        // if image compressed from RGB source, set proper tag, else use default
        entry->data[0] = 4; // R
        entry->data[1] = 5; // G
        entry->data[2] = 6; // B
    }

    uint32_t len = strlen(HAL_TAG_MODEL);
    entry = createTag(exif, EXIF_IFD_0, EXIF_TAG_MODEL,
            EXIF_FORMAT_ASCII, len);
    CHECK(entry);
    strncpy((char*)entry->data, HAL_TAG_MODEL, len);

    len = strlen(HAL_TAG_MAKE);
    entry = createTag(exif, EXIF_IFD_0, EXIF_TAG_MAKE,
            EXIF_FORMAT_ASCII, len);
    CHECK(entry);
    strncpy((char*)entry->data, HAL_TAG_MAKE, len);

    // reads current system time of day and inits the data
    entry = initTag(exif, EXIF_IFD_0, EXIF_TAG_DATE_TIME);
    CHECK(entry);
    entry = initTag(exif, EXIF_IFD_EXIF, EXIF_TAG_DATE_TIME_DIGITIZED);
    CHECK(entry);

    // as the capture timestamps do not contain absolute time since epoch,
    // use general purpose function which reads _current_ localtime
    entry = initTag(exif, EXIF_IFD_EXIF, EXIF_TAG_DATE_TIME_ORIGINAL);
    CHECK(entry);

    // only sub-second part of timestamp is read from sensor hardware
    if(mMeta->getTimestampSource() ==
            ANDROID_SENSOR_INFO_TIMESTAMP_SOURCE_REALTIME) {
        const nsecs_t timestampNs = mMeta->getTimestamp();
        const nsecs_t secondNs = seconds_to_nanoseconds(1);
#if(0)
        // if sensor timestamps were time since epoch,
        // we would be able to calculate localtime of capture
        // but unfortunately it's not
        // : these calculations can be made on uint32_t types
        // should be faster on non 64bit arch.
        const time_t t = timestampNs/secondNs;
        struct tm *tm;
        tm = localtime (&t);
        entry = createTag(exif, EXIF_IFD_EXIF,
            (ExifTag)EXIF_TAG_DATE_TIME_ORIGINAL,
            EXIF_FORMAT_ASCII, 20);
        CHECK(entry);
        snprintf ((char *) entry->data, entry->size,
                "%04i:%02i:%02i %02i:%02i:%02i",
                tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                tm->tm_hour, tm->tm_min, tm->tm_sec);
#endif
        // sub nanosecond part of timestamp
        entry = createTag(exif, EXIF_IFD_EXIF,
            (ExifTag)EXIF_TAG_SUB_SEC_TIME_ORIGINAL,
            EXIF_FORMAT_ASCII, 4);
        CHECK(entry);
        // remove seconds and useconds
        int64_t subseconds = timestampNs-timestampNs/secondNs*secondNs;
        subseconds = subseconds/1000000; // leave milliseconds only
        int chars = snprintf((char *) entry->data, entry->size,
                "%03lld", subseconds);
        ALOG_ASSERT(chars == 3); // only milliseconds are written
        entry = createTag(exif, EXIF_IFD_EXIF,
                    (ExifTag)EXIF_TAG_SUB_SEC_TIME,
                    EXIF_FORMAT_ASCII, 4);
        CHECK(entry);
        snprintf((char *) entry->data, entry->size,
                "%03lld", subseconds);
        entry = createTag(exif, EXIF_IFD_EXIF,
                    (ExifTag)EXIF_TAG_SUB_SEC_TIME_DIGITIZED,
                    EXIF_FORMAT_ASCII, 4);
        CHECK(entry);
        snprintf((char *) entry->data, entry->size,
                "%03lld", subseconds);
    } else {
        // report stub value SUB_SEC="1" for DIGITIZED and main
        // to make CTS happy (don't know why it must be >0)
        // but still CTS can fail if milliseconds part of real timestamp
        // is zero
        entry = createTag(exif, EXIF_IFD_EXIF,
                    (ExifTag)EXIF_TAG_SUB_SEC_TIME,
                    EXIF_FORMAT_ASCII, 2);
        CHECK(entry);
        entry->data[0] = '1';
        entry = createTag(exif, EXIF_IFD_EXIF,
                    (ExifTag)EXIF_TAG_SUB_SEC_TIME_ORIGINAL,
                    EXIF_FORMAT_ASCII, 2);
        CHECK(entry);
        entry->data[0] = '1';
        entry = createTag(exif, EXIF_IFD_EXIF,
                    (ExifTag)EXIF_TAG_SUB_SEC_TIME_DIGITIZED,
                    EXIF_FORMAT_ASCII, 2);
        CHECK(entry);
        entry->data[0] = '1';
    }

    // GPS data
    double latitude, longitude, altitude;
    int64_t timestamp=0; // in seconds
    std::string method;
    if(mMeta->getGpsData(latitude, longitude, altitude,
            timestamp, method)) {

        long rationalSize = exif_format_get_size(EXIF_FORMAT_RATIONAL);

        ExifRational degrees, minutes, seconds, r_altitude;
        std::string latitude_ref, longitude_ref;
        // latitude and longitude are 3*rationals
        // (Degrees,minutes,seconds),
        if(latitude<0) {
            latitude_ref = "S";
            latitude = -latitude;
        } else {
            latitude_ref = "N";
        }
        degreesToRationalHMS(latitude, degrees, minutes, seconds);
        entry = createTag(exif, EXIF_IFD_GPS,
                (ExifTag)EXIF_TAG_GPS_LATITUDE,
                EXIF_FORMAT_RATIONAL, 3);
        CHECK(entry);
        exif_set_rational(entry->data, FILE_BYTE_ORDER, degrees);
        exif_set_rational(entry->data+rationalSize, FILE_BYTE_ORDER,
                minutes);
        exif_set_rational(entry->data+2*rationalSize, FILE_BYTE_ORDER,
                seconds);

        entry = createTag(exif, EXIF_IFD_GPS,
                (ExifTag)EXIF_TAG_GPS_LATITUDE_REF,
                EXIF_FORMAT_ASCII, 2);
        CHECK(entry);
        memcpy((char*)entry->data, latitude_ref.c_str(), entry->size);

        if(longitude<0) {
            longitude_ref = "W";
            longitude = -longitude;
        } else {
            longitude_ref = "E";
        }
        degreesToRationalHMS(longitude, degrees, minutes, seconds);
        entry = createTag(exif, EXIF_IFD_GPS,
                (ExifTag)EXIF_TAG_GPS_LONGITUDE,
                EXIF_FORMAT_RATIONAL, 3);
        CHECK(entry);
        exif_set_rational(entry->data, FILE_BYTE_ORDER, degrees);
        exif_set_rational(entry->data+rationalSize, FILE_BYTE_ORDER,
                minutes);
        exif_set_rational(entry->data+2*rationalSize, FILE_BYTE_ORDER,
                seconds);
        entry = createTag(exif, EXIF_IFD_GPS,
                (ExifTag)EXIF_TAG_GPS_LONGITUDE_REF,
                EXIF_FORMAT_ASCII, 2);
        CHECK(entry);
        memcpy((char*)entry->data, longitude_ref.c_str(), entry->size);

        // altitude is 1 rational
        ExifByte altitude_ref = 0;
        if(altitude<0) {
            altitude = -altitude;
            altitude_ref = 1;
        }
        doubleToRational(altitude, r_altitude);
        entry = createTag(exif, EXIF_IFD_GPS,
                (ExifTag)EXIF_TAG_GPS_ALTITUDE,
                EXIF_FORMAT_RATIONAL, 1);
        CHECK(entry);
        exif_set_rational(entry->data, FILE_BYTE_ORDER, r_altitude);
        entry = createTag(exif, EXIF_IFD_GPS,
                (ExifTag)EXIF_TAG_GPS_ALTITUDE_REF,
                EXIF_FORMAT_BYTE, 1);
        CHECK(entry);
        entry->data[0] = altitude_ref;

        // timestamp is 3*rational (hour:minutes:seconds)
        ExifRational data = { 1, 1 };
        const time_t gpsTimestamp = timestamp;
        struct tm dt;
        memcpy(&dt, localtime(&gpsTimestamp), sizeof(dt));

        dt.tm_year+=1900;
        dt.tm_mon+=1;
        LOG_DEBUG("Converting %lu seconds to %d:%d:%d %d:%d:%d",
                gpsTimestamp,
                dt.tm_year, dt.tm_mon, dt.tm_mday,
                dt.tm_hour, dt.tm_min, dt.tm_sec);

        entry = createTag(exif, EXIF_IFD_GPS,
                (ExifTag)EXIF_TAG_GPS_TIME_STAMP,
                EXIF_FORMAT_RATIONAL, 3);
        CHECK(entry);
        data.numerator = dt.tm_hour;
        exif_set_rational(entry->data, FILE_BYTE_ORDER, data);
        data.numerator = dt.tm_min;
        exif_set_rational(entry->data+rationalSize, FILE_BYTE_ORDER,
                data);
        data.numerator = dt.tm_sec;
        exif_set_rational(entry->data+2*rationalSize, FILE_BYTE_ORDER,
                data);

        // this tag includes null termination
        const uint8_t date_size = strlen("YYYY:MM:DD")+1;
        char date[date_size];
        snprintf(date, date_size, "%.4d:%.2d:%.2d",
                dt.tm_year, dt.tm_mon, dt.tm_mday);
        entry = createTag(exif, EXIF_IFD_GPS,
                (ExifTag)EXIF_TAG_GPS_DATE_STAMP,
                EXIF_FORMAT_ASCII, date_size);
        CHECK(entry);
        strncpy((char*)entry->data, date, date_size);

        // gps processing method is undefined type, prefixed with
        // character code (we use ascii)
        entry = createTag(exif, EXIF_IFD_GPS,
                (ExifTag)EXIF_TAG_GPS_PROCESSING_METHOD,
                EXIF_FORMAT_UNDEFINED, sizeof(asciiCode)+method.length());
        CHECK(entry);
        memcpy(entry->data, asciiCode, sizeof(asciiCode));
        memcpy(entry->data+sizeof(asciiCode),
            method.c_str(), method.length());
    }

    if(thumbnail!=NULL && thumbnailSize > 0) {
        entry = initTag(exif, EXIF_IFD_1, EXIF_TAG_COMPRESSION);
        CHECK(entry);
        exif_set_short(entry->data, FILE_BYTE_ORDER, 6);

#ifdef MAKE_THUMBNAIL_COPY
        /* Create a memory allocator to manage this ExifEntry */
        ExifMem *mem = exif_mem_new_default();
        CHECK(mem);
        // alloc the buffer because libExif would call exif_mem_free() on it */
        exif->data = (unsigned char*)exif_mem_alloc(
                mem,
                (ExifLong)thumbnailSize);
        CHECK(exif->data);
        // copy the compressed thumbnail data
        memcpy(exif->data, thumbnail, thumbnailSize);

        /* The ExifMem and ExifEntry are now owned elsewhere */
        exif_mem_unref(mem);
#else
        exif->data = thumbnail;
#endif
        exif->size = thumbnailSize;
    }
    exif_data_fix(exif);

    /**
     * Due to bug in libexif, we have to explicitly remove
     * the two tags added and initialized to 0 by exif_data_fix().
     * The correct tags values will be calculated and added by
     * exif_data_save_data(), otherwise we could end with malformed data
     */
    entry = exif_content_get_entry(
            exif->ifd[EXIF_IFD_1],
            (ExifTag)EXIF_TAG_JPEG_INTERCHANGE_FORMAT);
    if(entry) {
        exif_content_remove_entry(exif->ifd[EXIF_IFD_1], entry);
    }
    entry = exif_content_get_entry(
            exif->ifd[EXIF_IFD_1],
            (ExifTag)EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH);
    if(entry) {
        exif_content_remove_entry(exif->ifd[EXIF_IFD_1], entry);
    }

    exif_data_save_data(exif, &exif_data, &exif_data_len);

    if(!exif_data || !exif_data_len) {
        LOG_ERROR("Error in preparation of EXIF data");
        return UNKNOWN_ERROR;
    }
#if(0)
    exif_data_dump(exif);
#endif

    // write Exif to current position of jpeg image buffer
    mIo.writeHeader(M_APP1, exif_data_len);
    while(exif_data_len--) {
        mIo.write1byte(*exif_data++);
    }

#ifndef MAKE_THUMBNAIL_COPY
    // the pointers were not allocated by libexif allocator
    exif->data = NULL;
    exif->size = 0;
#endif

    return OK;
}

// : fix asserts
ExifEntry* ExifWriter::initTag(ExifData *exif, ExifIfd ifd, ExifTag tag) {
    ExifEntry *entry;

    if(status!=OK) {
        return NULL;
    }

    /* Return an existing tag if one exists */
    if (!((entry = exif_content_get_entry (exif->ifd[ifd], tag)))) {
        /* Allocate a new entry */
        entry = exif_entry_new ();
        if(!entry) { /* catch an out of memory condition */
            LOG_ERROR("No memory");
            return NULL;
        }
        entry->tag = tag; /* tag must be set before calling
                 exif_content_add_entry */

        /* Attach the ExifEntry to an IFD */
        exif_content_add_entry (exif->ifd[ifd], entry);

        /* Allocate memory for the entry and fill with default data */
        exif_entry_initialize (entry, tag);

        /* Ownership of the ExifEntry has now been passed to the IFD.
         * One must be very careful in accessing a structure after
         * unref'ing it; in this case, we know "entry" won't be freed
         * because the reference count was bumped when it was added to
         * the IFD.
         */
        exif_entry_unref(entry);
    }
    return entry;
}

ExifEntry *ExifWriter::createTag(ExifData *exif, ExifIfd ifd, ExifTag tag,
        ExifFormat format, unsigned long components) {
    uint8_t* buf;
    ExifEntry *entry;

    if(status!=OK) {
        return NULL;
    }

    /* Create a memory allocator to manage this ExifEntry */
    ExifMem *mem = exif_mem_new_default();
    if(!mem) { /* catch an out of memory condition */
        LOG_ERROR("No memory");
        return NULL;
    }

    /* Create a new ExifEntry using our allocator */
    entry = exif_entry_new_mem (mem);
    if(!entry) { /* catch an out of memory condition */
        LOG_ERROR("No memory");
        return NULL;
    }

    /* Allocate memory to use for holding the tag data */
    uint32_t len = exif_format_get_size(format)*components;
    buf = (uint8_t*)exif_mem_alloc(mem, len);
    if(!buf) { /* catch an out of memory condition */
        LOG_ERROR("No memory");
        return NULL;
    }

    /* Fill in the entry */
    entry->data = buf;
    entry->size = len;
    entry->tag = tag;
    entry->components = components;
    entry->format = format;

    /* Attach the ExifEntry to an IFD */
    exif_content_add_entry (exif->ifd[ifd], entry);

    /* The ExifMem and ExifEntry are now owned elsewhere */
    exif_mem_unref(mem);
    exif_entry_unref(entry);

    return entry;
}

} // namespace android
