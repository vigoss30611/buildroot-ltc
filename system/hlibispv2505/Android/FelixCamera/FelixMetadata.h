/**
******************************************************************************
@file FelixMetadata.h

@brief Interface of FelixMetadata container class in v2500 Camera HAL

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

#ifndef HW_FELIX_METADATA_H_
#define HW_FELIX_METADATA_H_

#include <string>
#include "system/camera_metadata.h"
#include "camera/CameraMetadata.h"
#include "ispc/Shot.h"
#include "utils/Timers.h"

#include "AAA/FelixAE.h"
#include "AAA/FelixAF.h"
#include "AAA/FelixAWB.h"
#include "Region.h"
#include "HwManager.h"

namespace android {

/* metadata in Android 4.4 does not define several enums.
 * Instead of using #if CAMERA_DEVICE_API_VERSION_CURRENT in these places,
 * define dummy names which won't be used in Kitkat anyway
 */
#if CAMERA_DEVICE_API_VERSION_CURRENT < HARDWARE_DEVICE_API_VERSION(3, 2)

#define ANDROID_CONTROL_SCENE_MODE_DISABLED \
    ANDROID_CONTROL_SCENE_MODE_UNSUPPORTED

// make compiler happy and make sure this value won't be used in Kitkat
#define CAMERA3_TEMPLATE_MANUAL \
    CAMERA3_TEMPLATE_COUNT
#define ANDROID_CONTROL_CAPTURE_INTENT_MANUAL \
    ANDROID_CONTROL_CAPTURE_INTENT_CUSTOM

typedef enum camera_metadata_enum_android_flash_info_available {
    ANDROID_FLASH_INFO_AVAILABLE_FALSE,
    ANDROID_FLASH_INFO_AVAILABLE_TRUE,
} camera_metadata_enum_android_flash_info_available_t;

// ANDROID_SENSOR_INFO_TIMESTAMP_SOURCE
typedef enum camera_metadata_enum_android_sensor_info_timestamp_source {
    ANDROID_SENSOR_INFO_TIMESTAMP_SOURCE_UNKNOWN,
    ANDROID_SENSOR_INFO_TIMESTAMP_SOURCE_REALTIME,
} camera_metadata_enum_android_sensor_info_timestamp_source_t;

#endif

class Rect; // forward declaration
class FelixCamera;

class FelixMetadata : public CameraMetadata, public RefBase {

public:
    /** @brief Construct empty object */
    FelixMetadata(FelixCamera& camera);
    ~FelixMetadata(void);

    FelixMetadata(camera_metadata_t * rawMetadata, FelixCamera& camera);

    /** @brief Assignment operator
     *
     *  @param buffer The pointer to camera_metadata_t structure
     */
    FelixMetadata &operator=(const camera_metadata_t *buffer);

    /** @brief Assignment operator
     *
     *  @param buffer The reference to CameraMetadata object
     */
    FelixMetadata &operator=(const CameraMetadata& buffer) {
        // assign buffer data to our base container
        static_cast<CameraMetadata*>(this)->operator=(buffer); return *this;
    }

    /** @brief Assignment operator
     *
     *  @param buffer The reference to CameraMetadata object
     */
    FelixMetadata &operator=(const FelixMetadata& buffer);

    /**
     * @brief Fills ANDROID_STATISTICS_SCENE_FLICKER with value provided in
     * ISPC_Metadata
     *
     * @param ispcSettings the metadata structure provided with the capture
     */
    void processFlickering(ISPC::Metadata &ispcSettings);

    /**
     * @brief If requested, fills ANDROID_STATISTICS_HISTOGRAM with histogram
     * data
     * provided in ISPC_Metadata
     *
     * @param ispcSettings the metadata structure provided with the capture
     */
    void processHistogram(ISPC::Metadata &ispcSettings);

    /**
     * @brief If requested, fills ANDROID_STATISTICS_HOT_PIXEL_MAP with
     * currently used hot pixel map data
     */
    void processHotPixelMap(void);

    /**
     * @brief If requested, fills ANDROID_STATISTICS_LENS_SHADING_MAP with
     * current data from LSH module
     */
    void processLensShadingMap(void);

    /**
     * @brief process effect mode for next capture
     */
    void preprocessEffectMode(void);

    /**
     * @brief Determines if crop area has changed
     *
     * @return true if crop area has changed comparing to stored metadata
     */
    bool hasCropRegionChanged(void);

    /**
     * @brief update frame duration field
     */
    void updateFrameDuration(const nsecs_t durationNs);

    /**
     * @brief calculate rolling shutter skew from two timestamps
     * @param firstRowExposureNs start of first row exposure
     * @param lastRowExposureNs start of last row exposure
     */
    void updateRollingShutterSkew(
        uint32_t firstRowExposureNs,
        uint32_t lastRowExposureNs);

    /**
     * @brief return frame duration in nsecs
     */
    nsecs_t getFrameDuration() const;

    /**
     * @brief read sensor capture timestamp from metadata
     */
    nsecs_t getTimestamp() const;

    /**
     * @brief returns the source of capture timestamp
     */
    camera_metadata_enum_android_sensor_info_timestamp_source_t
    getTimestampSource() const;

    /**
     * @brief read focal length field
     */
    bool getFocalLength(double& fl) const;

    /**
     * @brief read exposure time in ns
     */
    bool getExposureTime(int64_t& exposureNs) const;

    /**
     * @brief read ISO sensitivity ysed for captured shot
     */
    bool getISOSensitivity(int32_t& iso) const;

    /**
     * @brief read lens aperture
     */
    bool getLensAperture(double& aperture) const;

    /**
     * @brief returns Exif formatted mask containg flash state used
     * for current capture
     */
    bool getExifFlashState(uint16_t& flashState) const;

    /**
     * @brief return AWB mode
     * @return true if auto white balance enabled, else false
     */
    bool getAutoWhiteBalance() const;

    /**
     * @brief read gps metadata
     */
    bool getGpsData(double& latitude,
            double& longtitude, double& altitude,
            int64_t& timestamp, std::string& gpsMethod) const;

    /**
     * @brief read jpeg orientation [0,90,180,270] in degrees
     */
    bool getJpegOrientation(uint32_t& orientation) const;

    /**
     * @brief get thumbnail size for jpeg
     */
    bool getJpegThumbnailSize(uint32_t& width, uint32_t& height) const;

    /**
     * @brief get wuality factor of jpeg thumbnail
     */
    bool getJpegThumbnailQuality(uint8_t& quality) const;

    /**
     * @brief get quality factor of main jpeg
     */
    bool getJpegQuality(uint8_t& quality) const;

    /**
     * @brief calculate source cropping rectangle for provided target stream
     * dimensions
     *
     * @param streamWidth width of target stream
     *
     * @param streamHeight height of target stream
     *
     * @param rect result of cropping rectangle calculation
     *
     * @param cropWholeSensorArea if false then ANDROID_SCALER_CROP_REGION
     * is used as initial cropping region of active sensor array.
     * If true then ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE array is used as
     * initial cropping region
     *
     * @return error status
     *
     * @note This method applies correction to
     * ANDROID_SCALER_CROP_REGION or ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE
     * so rect fits inside the source cropping rectangle and at the same time
     * maintains the aspect ratio of target stream (streamWidth / streamHeight)
     */
    status_t calculateStreamCrop(
                    const uint32_t streamWidth, const uint32_t streamHeight,
                    Rect& rect, bool cropWholeSensorArea = true);

    /**
     * @brief Read 3A regions from CameraMetadata, crop and store in
     * local containers for further processing in Felix 3A modules
     */
    void preprocess3aRegions(void);

    template <class T>
    status_t updateMetadataWithRegions(
            camera_metadata_tag_t tag,
            typename T::regions_t& regions);

    FelixAE::regions_t& getAeRegions(void) { return mAeRegions; }
    FelixAF::regions_t& getAfRegions(void) { return mAfRegions; }
    FelixAWB::regions_t& getAwbRegions(void) { return mAwbRegions; }

private:
    /**
     * @brief init internal region storage with data from CameraMetadata
     *
     * @note templated method because it must handle FelixXXX::region_t
     * type, which is templated as well
     */
    template <class T>
    status_t read3aRegionsFromMetadata(
            camera_metadata_tag_t metadataKey,
            typename T::regions_t &output);

    template <class T>
    void crop3aRegions(const Rect& crop,
            typename T::regions_t &output);

    /** @brief Local data buffer used in processHistogram */
    // preallocated and used in processHistogram()
    int32_t             mHistogramData[3*HIS_GLOBAL_BINS];

    /** @brief stored 3A regions, cropped */
    FelixAE::regions_t mAeRegions;
    FelixAF::regions_t mAfRegions;
    FelixAWB::regions_t mAwbRegions;

    // stored last settings - needed for comparison purposes
    uint8_t lastEffectMode;
    int32_t lastCropRegion[4];

    FelixCamera &mParent;
}; /* FelixMetadata */

} /* android */
#endif /* HW_FELIX_METADATA_H_ */
