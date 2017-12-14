/**
******************************************************************************
@file FelixMetadata.cpp

@brief Definition of FelixMetadata container class in v2500 Camera HAL

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

#define LOG_TAG "FelixMetadata"

#include <string.h>
#include <ui/Rect.h>

#include "FelixCamera.h"
#include "FelixMetadata.h"
#include "felixcommon/userlog.h"

#include "ispc/ModuleLSH.h"

/* for debugging purposes only */
#undef SIMULATE_GPS_DATA

namespace android {

template status_t FelixMetadata::updateMetadataWithRegions<class FelixAF>(
        camera_metadata_tag_t tag,
        FelixAF::regions_t& regions);

template status_t FelixMetadata::updateMetadataWithRegions<class FelixAE>(
        camera_metadata_tag_t tag,
        FelixAE::regions_t& regions);

template status_t FelixMetadata::updateMetadataWithRegions<class FelixAWB>(
        camera_metadata_tag_t tag,
        FelixAWB::regions_t& regions);

FelixMetadata::FelixMetadata(FelixCamera& camera) :
                lastEffectMode((uint8_t)ANDROID_CONTROL_EFFECT_MODE_OFF),
                lastCropRegion(),
                mParent(camera)
{
    LOG_DEBUG("E: %p", this);
}

FelixMetadata::FelixMetadata(camera_metadata_t * rawMetadata,
    FelixCamera& camera) :
        CameraMetadata(rawMetadata) ,
        lastEffectMode((uint8_t)ANDROID_CONTROL_EFFECT_MODE_OFF),
        lastCropRegion() ,
        mParent(camera)
{ }

FelixMetadata::~FelixMetadata(void) {
    LOG_DEBUG("E: %p", this);
}

FelixMetadata& FelixMetadata::operator=(const camera_metadata_t *buffer) {
    // assign buffer data to our base container
    static_cast<CameraMetadata*>(this)->operator=(buffer);
    return *this;
}

FelixMetadata& FelixMetadata::operator=(const FelixMetadata& buffer) {
        // assign buffer data to our base container
        static_cast<CameraMetadata*>(this)->operator=(buffer);
        *mHistogramData = *buffer.mHistogramData;
        mAeRegions = buffer.mAeRegions;
        mAfRegions = buffer.mAfRegions;
        mAwbRegions = buffer.mAwbRegions;
        lastEffectMode = buffer.lastEffectMode;
        memcpy(&lastCropRegion, &buffer.lastCropRegion,
                        sizeof(lastCropRegion));
        return *this;
    }

void FelixMetadata::processFlickering(ISPC::Metadata &ispcSettings)
{
    uint8_t flicker = ANDROID_STATISTICS_SCENE_FLICKER_NONE;

    switch (ispcSettings.flickerStats.flickerStatus)
    {
    case FELIX_SAVE_FLICKER_WAITING:
    case FELIX_SAVE_FLICKER_CANNOT_DETERMINE:
    case FELIX_SAVE_FLICKER_NO_FLICKER:
        // no flickering detected yet
        break;
    case FELIX_SAVE_FLICKER_LIGHT_50HZ:
        flicker = ANDROID_STATISTICS_SCENE_FLICKER_50HZ;
        break;
    case FELIX_SAVE_FLICKER_LIGHT_60HZ:
        flicker = ANDROID_STATISTICS_SCENE_FLICKER_60HZ;
        break;
    default:
        LOG_ERROR("invalid flicker value %d",
                ispcSettings.flickerStats.flickerStatus);
        break;
    }

    update(ANDROID_STATISTICS_SCENE_FLICKER, &flicker, 1);
}

void FelixMetadata::processHistogram(ISPC::Metadata &ispcSettings)
{
    double totalHistogram = 1; // avoid division by zero if flat
    uint32_t *sourceH = ispcSettings.histogramStats.globalHistogram;
    uint8_t* hm;
    if(!(hm = find(ANDROID_STATISTICS_HISTOGRAM_MODE).data.u8) ||
       (*hm == ANDROID_STATISTICS_HISTOGRAM_MODE_OFF))
    {
        // no histogram requested
        return;
    }
    LOG_INFO("Histogram requested");
    if(!sourceH)
    {
        LOG_ERROR("no global histogram in shot");
        return;
    }

    // calculate data
    for(int i=0;i<FelixCamera::kHistogramBucketCount;i++)
    {
        totalHistogram +=sourceH[i];
    }
    // write normalized histogram -- : review the functionality!
    for(int i=0;i<FelixCamera::kHistogramBucketCount;i++)
    {
        mHistogramData[i*3] =
        mHistogramData[i*3+1] =
        mHistogramData[i*3+2] =
            (int32_t)((double)sourceH[i]*FelixCamera::kMaxHistogramCount/totalHistogram);
        //ALOGD("HIST[%d]=%d", i, mHistogramData[i*3]);
    }
    update(ANDROID_STATISTICS_HISTOGRAM, (int32_t*)&mHistogramData,
            sizeof(mHistogramData)/sizeof(mHistogramData[0]));
}

void FelixMetadata::processHotPixelMap(void) {
#if CAMERA_DEVICE_API_VERSION_CURRENT >= HARDWARE_DEVICE_API_VERSION(3, 2)
    // 
    // static OFF value
    const uint8_t hotPixelMapMode =
            ANDROID_STATISTICS_HOT_PIXEL_MAP_MODE_OFF;
    update(ANDROID_STATISTICS_HOT_PIXEL_MAP_MODE,
            &hotPixelMapMode, 1);
#endif /* CAMERA_DEVICE_API_VERSION_CURRENT */
}

void FelixMetadata::processLensShadingMap(void)
{
    Vector<float> lensShadingMap;

    HwManager& mCameraHw = mParent.getHwInstance();
    ALOG_ASSERT(mCameraHw.getMainPipeline());

    const ISPC::ModuleLSH* lsh;
    if(!(lsh = mCameraHw.getMainPipeline()->getModule<ISPC::ModuleLSH>()) ||
        !lsh->hasGrid()) {
        // return dummy identity map if no LSH module loaded or
        // no grid has been configured
        lensShadingMap.insertAt(1.0f, 0, 4);
    } else {
        int32_t width=1, height=1;
        mCameraHw.getLSHMapSize(width, height);
        // make big realloc instead of several smaller ones
        lensShadingMap.reserve(width*height);
        uint8_t* hm;
        //if(!(hm = find(ANDROID_STATISTICS_LENS_SHADING_MAP_MODE).data.u8) ||
        //   (*hm == ANDROID_STATISTICS_LENS_SHADING_MAP_MODE_OFF))
        //{
            // lens shading disabled - return identity map
            lensShadingMap.insertAt(1.0f, 0, width*height*4);
        //} else {
        //    LOG_INFO("Lens shading map requested");

        //    // calculate data
        //    for(int j=0;j<width*height;++j) {
        //        // : check the exact meaning of the 0-3 fields
        //        lensShadingMap.push(lsh->sGrid.apMatrix[0][j]);
        //        lensShadingMap.push(lsh->sGrid.apMatrix[1][j]);
        //        lensShadingMap.push(lsh->sGrid.apMatrix[2][j]);
        //        lensShadingMap.push(lsh->sGrid.apMatrix[3][j]);
        //    }
        //}
    }
    update(ANDROID_STATISTICS_LENS_SHADING_MAP,
            lensShadingMap.array(),
            lensShadingMap.size());
}

void FelixMetadata::preprocessEffectMode(void) {
        HwManager& mCameraHw = mParent.getHwInstance();

    uint8_t* ef = find(ANDROID_CONTROL_EFFECT_MODE).data.u8;

    uint8_t isMode = !ef ?
            (uint8_t)ANDROID_CONTROL_EFFECT_MODE_OFF :
            *ef;

    if(isMode == lastEffectMode) {
        //  no change in effect comparing to last request
        return;
    }
    LOG_DEBUG("Applying effect %d", isMode);
    FelixAWB* awb = mCameraHw.getMainPipeline()->getControlModule<
        FelixAWB>();
    ALOG_ASSERT(awb);
    // enable AWB only if effect is OFF otherwise AWB would change the settings
    awb->enableControl(isMode==(uint8_t)ANDROID_CONTROL_EFFECT_MODE_OFF);

    mCameraHw.setEffect(
        (camera_metadata_enum_android_control_effect_mode_t)isMode);
    lastEffectMode = isMode;
}

bool FelixMetadata::hasCropRegionChanged(void)
{
    bool res = false;
    int32_t* cropRegion = NULL;

    cropRegion = find(ANDROID_SCALER_CROP_REGION).data.i32;
    if(!cropRegion) {
        // signaling true would cause processing of the region which does not
        // exist
        return false;
    }
    if(lastCropRegion[0] != cropRegion[0] ||
        lastCropRegion[1] != cropRegion[1] ||
        lastCropRegion[2] != cropRegion[2] ||
        lastCropRegion[3] != cropRegion[3]) {
        // crop region has changed
        res = true;
        LOG_DEBUG("New crop region=(%d, %d, %d x %d)",
                        cropRegion[0], cropRegion[1], cropRegion[2], cropRegion[3]);
        lastCropRegion[0] = cropRegion[0];
                lastCropRegion[1] = cropRegion[1];
                lastCropRegion[2] = cropRegion[2];
                lastCropRegion[3] = cropRegion[3];
    }

    return res;
}

void FelixMetadata::updateFrameDuration(const nsecs_t durationNs) {

    update(ANDROID_SENSOR_FRAME_DURATION, &durationNs, 1);
    LOG_DEBUG("Updating metadata with duration %lld", durationNs);
}

void FelixMetadata::updateRollingShutterSkew(
        __maybe_unused uint32_t firstRowExposureNs,
        __maybe_unused uint32_t lastRowExposureNs) {

#if CAMERA_DEVICE_API_VERSION_CURRENT >= HARDWARE_DEVICE_API_VERSION(3, 2)
    int64_t rollingShutterSkew;
    if(lastRowExposureNs >= firstRowExposureNs) {
        rollingShutterSkew = lastRowExposureNs - firstRowExposureNs;
    } else {
        rollingShutterSkew = UINT_MAX - firstRowExposureNs + lastRowExposureNs;
    }
    update(ANDROID_SENSOR_ROLLING_SHUTTER_SKEW, &rollingShutterSkew, 1);
#endif
}

nsecs_t FelixMetadata::getFrameDuration() const {

    camera_metadata_ro_entry e = find(ANDROID_SENSOR_FRAME_DURATION);
    if(e.count && e.data.i64) {
        return *e.data.i64;
    }
    return 0L;
}

nsecs_t FelixMetadata::getTimestamp() const {

    camera_metadata_ro_entry e = find(ANDROID_SENSOR_TIMESTAMP);
    if(e.count && e.data.i64) {
        return *e.data.i64;
    }
    LOG_WARNING("No timestamp provided!");
    return 0L;
}

camera_metadata_enum_android_sensor_info_timestamp_source_t
FelixMetadata::getTimestampSource() const {
    // we use realtime timestamps in v2500 HAL since beginning
    camera_metadata_enum_android_sensor_info_timestamp_source_t source =
        ANDROID_SENSOR_INFO_TIMESTAMP_SOURCE_REALTIME;

#if CAMERA_DEVICE_API_VERSION_CURRENT >= HARDWARE_DEVICE_API_VERSION(3, 2)
    camera_metadata_ro_entry e = find(ANDROID_SENSOR_INFO_TIMESTAMP_SOURCE);
    if(e.count && e.data.u8) {
        source = (camera_metadata_enum_android_sensor_info_timestamp_source_t)
            *e.data.u8;
    }
#endif
    return source;
}

bool FelixMetadata::getFocalLength(double& fl) const {

    camera_metadata_ro_entry e = find(ANDROID_LENS_FOCAL_LENGTH);
    if(e.count && e.data.f) {
        fl = (double)*e.data.f;
        return true;
    }
    return false;
}

bool FelixMetadata::getExposureTime(int64_t& exposureNs) const {

    camera_metadata_ro_entry e = find(ANDROID_SENSOR_EXPOSURE_TIME);
    if(e.count && e.data.i64) {
        exposureNs = *e.data.i64;
        return true;
    }
    return false;
}

bool FelixMetadata::getISOSensitivity(int32_t& iso) const {
    camera_metadata_ro_entry e = find(ANDROID_SENSOR_SENSITIVITY);
    if(e.count && e.data.i32) {
        iso = *e.data.i32;
        return true;
    }
    return false;
}

bool FelixMetadata::getLensAperture(double& aperture) const {

    camera_metadata_ro_entry e = find(ANDROID_LENS_APERTURE);
    if(e.count && e.data.f) {
        aperture = (double)*e.data.f;
        return true;
    }
    return false;
}

bool FelixMetadata::getExifFlashState(uint16_t& flashState) const {
    // set defaults
    uint8_t flState = ANDROID_FLASH_STATE_UNAVAILABLE;
    uint8_t flMode = ANDROID_FLASH_MODE_OFF;
    uint8_t aeMode = ANDROID_CONTROL_AE_MODE_OFF;

    /*
     According to "Exchangeable image file format for digital still cameras:
     Exif Version 2.3 ", p. 56

     Coding of FLASH exif field
     Values for bit 0 indicating whether the flash fired.
    0b = Flash did not fire.
    1b = Flash fired.
    Values for bits 1 and 2 indicating the status of returned light.
    00b = No strobe return detection function
    01b = reserved
    10b = Strobe return light not detected.
    11b = Strobe return light detected.
    Values for bits 3 and 4 indicating the camera's flash mode.
    00b = unknown
    01b = Compulsory flash firing
    10b = Compulsory flash suppression
    11b = Auto mode
    Values for bit 5 indicating the presence of a flash function.
    0b = Flash function present
    1b = No flash function
    Values for bit 6 indicating the camera's red-eye mode.
    0b = No red-eye reduction mode or unknown
    1b = Red-eye reduction supported
    */
    enum {
        FLASH_FIRED                      = 0x01,
        STROBE_RETURN_LIGHT_NOT_DETECTED = 0x04,
        STROBE_RETURN_LIGHT_DETECTED     = 0x05,
        COMPULSORY_FLASH_FIRING          = 0x08,
        COMPULSORY_FLASH_SUPPRESSION     = 0x10,
        AUTO_FLASH                       = 0x18,
        FLASH_NOT_SUPPORTED              = 0x20,
        REDEYE_REDUCTION_SUPPORTED       = 0x40
    };

    camera_metadata_ro_entry e = find(ANDROID_FLASH_STATE);
    if(e.count && e.data.u8) {
        flState = *e.data.u8;
    }
    e = find(ANDROID_FLASH_MODE);
    if(e.count && e.data.u8) {
        flMode = *e.data.u8;
    }

    flashState = 0;
    if(flState == ANDROID_FLASH_STATE_UNAVAILABLE) {
        flashState &= FLASH_NOT_SUPPORTED;
        return true;
    }
    e = find(ANDROID_CONTROL_AE_MODE);
    if(e.count && e.data.u8) {
        aeMode = *e.data.u8;
    }
    switch(aeMode) {
    case ANDROID_CONTROL_AE_MODE_ON:
    case ANDROID_CONTROL_AE_MODE_OFF:
        // in AE mode OFF or ON, we can control flash directly
        if(flMode == ANDROID_FLASH_MODE_OFF) {
            flashState &= COMPULSORY_FLASH_SUPPRESSION;
        }
        break;

        // else AE FLASH mode defines whether it's auto or always fired
    case ANDROID_CONTROL_AE_MODE_ON_AUTO_FLASH:
        flashState &= AUTO_FLASH;
        break;

    case ANDROID_CONTROL_AE_MODE_ON_ALWAYS_FLASH:
        flashState &= COMPULSORY_FLASH_FIRING;
        break;

    default:
        break;
    }

    if(flState == ANDROID_FLASH_STATE_FIRED) {
        ALOG_ASSERT(flMode != ANDROID_FLASH_MODE_OFF);
        flashState &= FLASH_FIRED;
    }
    // : add support for redeye and strobe return light
    return true;
}

bool FelixMetadata::getAutoWhiteBalance() const {
    camera_metadata_ro_entry e = find(ANDROID_CONTROL_AWB_MODE);
    if(e.count && e.data.u8) {
        if(*e.data.u8 == ANDROID_CONTROL_AWB_MODE_AUTO) {
            return true;
        }
    }

    return false;
}

bool FelixMetadata::getGpsData(double& latitude,
        double& longitude, double& altitude,
        int64_t& timestamp, std::string& gpsMethod) const {

    bool ret = false;
#ifdef SIMULATE_GPS_DATA
    latitude = 37.736071;
    longitude = -122.441983;
    altitude = 21;
    timestamp = 1199145600L;
    gpsMethod = "GPS NETWORK HYBRID ARE ALL FINE.";
    ret = true;
#else
    camera_metadata_ro_entry e = find(ANDROID_JPEG_GPS_COORDINATES);
    if(e.count == 3 && e.data.d) {
        latitude = e.data.d[0];
        longitude = e.data.d[1];
        altitude = e.data.d[2];

        if(latitude < -90.0 || latitude > 90.0 ||
           longitude <= -180.0 || longitude > 180.0) {
            LOG_WARNING("Invalid GPS coordinates provided");
            return false;
        }
        e = find(ANDROID_JPEG_GPS_TIMESTAMP);
        if(e.count && e.data.i64) {
            timestamp = *e.data.i64;
        }
        e = find(ANDROID_JPEG_GPS_PROCESSING_METHOD);
        if (e.count) {
            // happens to be not null terminated in 4.4
            gpsMethod.append((const char*)e.data.u8, e.count);
        }
        ret = true;
    }
#endif
    return ret;
}

bool FelixMetadata::getJpegOrientation(uint32_t& orientation) const {

    camera_metadata_ro_entry e = find(ANDROID_JPEG_ORIENTATION);
    if(e.count && e.data.i32) {
        orientation = e.data.i32[0];
        return true;
    }
    return false;
}

bool FelixMetadata::getJpegThumbnailSize(
        uint32_t& width, uint32_t& height) const {

    camera_metadata_ro_entry e = find(ANDROID_JPEG_THUMBNAIL_SIZE);
    if(e.count == 2) {
        width = e.data.i32[0];
        height = e.data.i32[1];
        return true;
    }
    return false;
}

bool FelixMetadata::getJpegThumbnailQuality(uint8_t& quality) const {

    camera_metadata_ro_entry e = find(ANDROID_JPEG_THUMBNAIL_QUALITY);
    if(e.count) {
        quality = e.data.u8[0];
        if (quality < 1 || quality > 100) {
            LOG_WARNING("Thumb quality = %d out of range (1-100)!", quality);
            return false;
        }
        return true;
    }
    return false;
}

bool FelixMetadata::getJpegQuality(uint8_t& quality) const {

    camera_metadata_ro_entry e = find(ANDROID_JPEG_QUALITY);
    if(e.count) {
        quality = e.data.u8[0];
        if (quality < 1 || quality > 100) {
            LOG_WARNING("Thumb quality = %d out of range (1-100)!", quality);
            return false;
        }
        return true;
    }
    return false;
}

status_t FelixMetadata::calculateStreamCrop(
            const uint32_t streamWidth, const uint32_t streamHeight,
            Rect& rect, bool cropWholeSensorArea) {
    status_t res = ALREADY_EXISTS; // means no cropping needed
    const int32_t *cropRegion;

    int32_t sensorCropRegion[4];

    if (!(cropRegion = find(ANDROID_SCALER_CROP_REGION).data.i32)) {
        LOG_ERROR("No crop region defined");
        return BAD_VALUE;
    }
    LOG_DEBUG("Output (%d, %d) - Crop region (%d, %d, %d, %d)",
            streamWidth, streamHeight,
            cropRegion[0],
            cropRegion[1],
            cropRegion[2],
            cropRegion[3]);

    if(cropWholeSensorArea) {
        LOG_INFO("Cropping output using whole sensor area");
        //treat whole sensor area as initial crop region
        // defined in FelixCamera.cpp
        extern uint32_t sensorWidth, sensorHeight;
        sensorCropRegion[0] = 0;
        sensorCropRegion[1] = 0;
        sensorCropRegion[2] = (int32_t)sensorWidth;
        sensorCropRegion[3] = (int32_t)sensorHeight;
        cropRegion = sensorCropRegion;
    }

    int32_t cropOffsetX = cropRegion[0];
    int32_t cropOffsetY = cropRegion[1];
    uint32_t cropWidth = (uint32_t)cropRegion[2];
    uint32_t cropHeight = (uint32_t)cropRegion[3];

    if(streamWidth != cropWidth || streamHeight != cropHeight) {
        // aspect ratios
        double streamAr = (double)streamWidth/streamHeight;
        double cropRegionAr = (double)cropWidth/cropHeight;
        // process as described in camera3.h chapter S5. Cropping
        if (streamAr>cropRegionAr) {
            // crop vertically
            uint32_t origCropHeight = cropHeight;
            cropHeight = (uint32_t)floor((double)cropWidth / streamAr);
            // center new crop within original crop
            cropOffsetY += (origCropHeight - cropHeight)/2;
        } else if (streamAr<cropRegionAr) {
            // crop horizontally
            uint32_t origCropWidth = cropWidth;
            cropWidth = (uint32_t)floor((double)cropHeight * streamAr);
            // center new crop within original crop
            cropOffsetX += (origCropWidth - cropWidth)/2;
        }
        LOG_DEBUG("outputAr=%f cropRegionAr=%f crop: old(%d, %d, %d, %d) "
                "new(%d, %d, %d, %d) newAr=%f", streamAr, cropRegionAr,
                cropRegion[0], cropRegion[1], cropRegion[2], cropRegion[3],
                cropOffsetX, cropOffsetY,
                cropWidth, cropHeight,
                (double)cropWidth/cropHeight);
        res = NO_ERROR; // new cropping calculated
    }
    rect.left = cropOffsetX;
    rect.top = cropOffsetY;
    rect.right = cropOffsetX + cropWidth;
    rect.bottom = cropOffsetY + cropHeight;
    return res;
}

template <class T>
status_t FelixMetadata::read3aRegionsFromMetadata(
        camera_metadata_tag_t metadataKey,
        typename T::regions_t &output) {

    camera_metadata_entry e = find(metadataKey);
    rawRegion_t data = e.data.i32;
    if(!data || !e.count) {
        return BAD_VALUE;
    }
    uint32_t regions = e.count / T::region_t::rawEntrySize;

    output.clear();
    while(regions--) {
        const typename T::region_t region(data);
#ifdef BE_VERBOSE
        LOG_DEBUG("region (%d,%d,%d,%d, w=%f) requested",
                region.left, region.top, region.right, region.bottom,
                region.weight);
#endif
        output.push_back(region);
        data += T::region_t::rawEntrySize;
    }
    return NO_ERROR;
}

template <class T>
void FelixMetadata::crop3aRegions(const Rect& crop,
        typename T::regions_t &output) {
    typename T::regions_t::iterator i = output.begin();
    while(i!=output.end()) {
        typename T::region_t& region = *i;
        Rect cropped;
        if(region.intersect(crop, &cropped)) {
            // result valid, update
#ifdef BE_VERBOSE
            LOG_DEBUG("Region (%d, %d, %d, %d) cropped to (%d, %d, %d, %d)",
                    region.left, region.top, region.right, region.bottom,
                    cropped.left, cropped.top, cropped.right, cropped.bottom);
#endif
            region.set(cropped);
            ++i;
        } else {
            // result lies outside cropping rectangle, remove
            i = output.erase(i);
#ifdef BE_VERBOSE
            LOG_DEBUG("Removed region (%d, %d, %d, %d)",
                region.left, region.top, region.right, region.bottom);
#endif
        }
    }
}

void FelixMetadata::preprocess3aRegions(void) {
    // get 3A regions for 3A calculation
    read3aRegionsFromMetadata<FelixAE>(
            ANDROID_CONTROL_AE_REGIONS, mAeRegions);
    read3aRegionsFromMetadata<FelixAF>(
            ANDROID_CONTROL_AF_REGIONS, mAfRegions);
    read3aRegionsFromMetadata<FelixAWB>(
            ANDROID_CONTROL_AWB_REGIONS, mAwbRegions);

    int32_t* rawCropRegion = find(ANDROID_SCALER_CROP_REGION).data.i32;
    if(!rawCropRegion) {
        return;
    }
    Rect cropRegion(rawCropRegion[0], rawCropRegion[1],
            rawCropRegion[0] + rawCropRegion[2],
            rawCropRegion[1] + rawCropRegion[3]);
    if(!mAeRegions.empty()) {
        crop3aRegions<FelixAE>(cropRegion, mAeRegions);
    }
    if(!mAfRegions.empty()) {
        crop3aRegions<FelixAF>(cropRegion, mAfRegions);
    }
    if(!mAeRegions.empty()) {
        crop3aRegions<FelixAWB>(cropRegion, mAwbRegions);
    }
}

template <class T>
status_t FelixMetadata::updateMetadataWithRegions(
        camera_metadata_tag_t tag,
        typename T::regions_t& regions) {

	if (regions.size() == 0) {
		erase(tag);
		LOG_DEBUG("No regions defined");
		return NO_ERROR;
	}
    // treat with raw data
    const uint32_t rawSize = regions.size()*T::region_t::rawEntrySize;
    int32_t* cr = new int32_t[rawSize];
    if(!cr) {
        return IMG_ERROR_MALLOC_FAILED;
    }
    const int32_t* crStart = cr; // store orig. array pointer

    typename T::regions_t::iterator iter = regions.begin();
    while(iter!=regions.end()) {
        cr[0] = iter->left;
        cr[1] = iter->top;
        cr[2] = iter->right;
        cr[3] = iter->bottom;
        cr[4] = iter->weight;
#ifdef BE_VERBOSE
        LOG_DEBUG("Returning (%d, %d, %d, %d) to tag %X",
            cr[0], cr[1], cr[2], cr[3], tag);
#endif
        ++iter;
        cr+=T::region_t::rawEntrySize;
    }
    update(tag, crStart, rawSize);
    delete[] crStart;

    return NO_ERROR;
}

} /* android */
