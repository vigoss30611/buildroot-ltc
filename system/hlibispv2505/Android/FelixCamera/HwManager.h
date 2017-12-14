/**
******************************************************************************
@file HwManager.h

@brief Interface of Hardware manager class providing a level of abstraction
from ISPC library in v2500 Camera HAL

@note Hides specifics of multi context support which is not directly supported
in ISPC

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

#ifndef HW_MANAGER_H
#define HW_MANAGER_H

#include <hardware/camera3.h>

#include <utils/Singleton.h>
#include <utils/Mutex.h>
#include <utils/List.h>
#include <utils/BitSet.h>
#include <utils/KeyedVector.h>
#include <utils/SortedVector.h>
#include <utils/RefBase.h>
#include <utils/Timers.h>
#include <string>

#include "gralloc_priv.h"

#include "ispc/Camera.h"
#include "ispc/Sensor.h"
#include "ispc/ISPCDefs.h"
#include "ispc/Average.h"
#include "sensorapi/sensorapi.h"

namespace android {

#define EFFECT_TBL_SIZE 10
#define SCENE_TBL_SIZE 20

class Rect;

typedef ISPC::Sensor SensorBase;
typedef ISPC::Camera CameraBase;

class sensor_t : public SensorBase, virtual public RefBase {
public:
    sensor_t(int sensorId) : ISPC::Sensor(sensorId) {}
    virtual ~sensor_t(){}
};

class cameraHw_t : public CameraBase, virtual public RefBase {
public:
    cameraHw_t(unsigned int ctx, sensor_t* sensor) :
        ISPC::Camera(ctx, sensor){}
    virtual ~cameraHw_t(){}
};

class sensorDg_t : public ISPC::DGSensor, virtual public RefBase {
public:
    sensorDg_t(const std::string &filename, IMG_UINT8 gasketNumber = 0,
            bool isInternal = true) :
                ISPC::DGSensor(filename, gasketNumber, isInternal) {}
    virtual ~sensorDg_t(){}
};

class cameraDgHw_t  : public ISPC::DGCamera, virtual public RefBase {
public:
    cameraDgHw_t(unsigned int ctx, sensor_t* sensor) :
        ISPC::DGCamera(ctx, sensor){}
    virtual ~cameraDgHw_t(){}
};

bool isDisplayUsage(const uint32_t usage);

bool isEncoderUsage(const uint32_t usage);

bool isZslUsage(const uint32_t usage);

/**
 * @brief HW timestamp handling class
 * @note synchronizes system timer with ISP hardware timer
 */
class HwTimeBase {
public:
    HwTimeBase(const uint64_t hwClockFreqHz = 40000000);
    void setHwClockFreqHz(const uint64_t hwClockFreqHz = 40000000) {
        ispClockHz = hwClockFreqHz;
    }
    inline nsecs_t hwClockToNs(const uint64_t cycles);
    inline uint64_t nsToHwClock(const nsecs_t nanoTime);
    void initHwNanoTimer(void);
    nsecs_t processHwTimestamp(const uint32_t hwTimestamp);
private:
    // class members
    uint64_t realtimeHw;
    uint32_t prevHwTimestamp;
    uint32_t ispClockHz;
#ifdef MONITOR_CLOCK_DRIFT
    ISPC::mavg<nsecs_t> clockDrift;
#endif
};

/**
 * @brief Camera hardware manager class
 * @note Handles all hardware pipelines for single android Camera instance
 *
 * Provides additional separation layer between HAL and
 * lower level ISPC and CI libraries
 */
class HwManager {
public:
    HwManager(void);
    ~HwManager() {}
    bool statusOk(void) const { return mStatus == OK; }

    status_t initSensor(const std::string& name,
            const uint8_t mode,
            const SENSOR_FLIPPING flip = SENSOR_FLIPPING::SENSOR_FLIP_NONE);
    status_t initDgSensor(const std::string& name, uint8_t mode,
            bool externalDg = false);
    sensor_t* getSensor(void) const {
        return (!mExternalDG ? mSensor.get() :
                reinterpret_cast<sensor_t*>(mSensorDg.get())); }

    /**
     * @brief create number of contexts
     * @param numOfContexts number of available contexts to be created
     */
    status_t createHwPipelines(uint8_t numOfContexts);

    /**
     * @brief destroy all pipelines used in current HwManager instance
     */
    void destroyHwPipelines(void);

    status_t disableAllOutputs(void);

    // multicontext delegates for ISPC
    status_t stopCapture(void);
    status_t startCapture(void);

    status_t program();
    status_t setupModules();
    status_t addShots(uint32_t shotCount);

    // fast pipeline pause without disabling the sensor
    status_t pauseCapture(void);
    status_t continueCapture(void);
    void     flushRequests(void) { pauseCapture(); continueCapture(); }

    bool detachStream(const camera3_stream_t* const stream);
    cameraHw_t* attachStream(const camera3_stream_t* const stream,
                    CI_BUFFTYPE& ciBuffType);

    bool getFirstHwOutputAvailable(
            __maybe_unused uint32_t width, __maybe_unused uint32_t height,
            uint32_t& format);

    cameraHw_t* getMainPipeline(void) const {
        return mMainPipeline.promote().get(); }

    cameraHw_t* getPipeline(const camera3_stream_t * const stream) {
        return buffersMap.indexOfKey(stream)>=0 ?
                buffersMap.valueFor(stream).promote().get() : NULL;
    }
    bool getOutputParams(const camera3_stream_t * const stream,
            CI_BUFFTYPE& type, ePxlFormat& format);

    bool isEncoderFormat(const ePxlFormat format) {
    return (format == YVU_420_PL12_8 ||
            format == YUV_420_PL12_8 ||
            format == YVU_422_PL12_8 ||
            format == YUV_422_PL12_8 ||
            format == YVU_420_PL12_10 ||
            format == YUV_420_PL12_10 ||
            format == YVU_422_PL12_10 ||
            format == YUV_422_PL12_10);
    }

    bool isDisplayFormat(const ePxlFormat format) {
        return (format == RGB_888_24 ||
                format == RGB_888_32 ||
                format == BGR_888_24 ||
                format == BGR_888_32 ||
                format == RGB_101010_32 ||
                format == BGR_101010_32);
    }

    bool isHDRFormat(const ePxlFormat format) {
        return (format == RGB_101010_32 ||
                format == BGR_101010_32 ||
                format == YUV_420_PL12_10 ||
                format == YUV_422_PL12_10);
    }

    bool isBayerFormat(const ePxlFormat format) {
        return (format == BAYER_RGGB_8 ||
                format == BAYER_RGGB_10 ||
                format == BAYER_RGGB_12);
    }

    /**
     * @brief return exact HAL format for implementation defined format and
     * given usage mask
     */
    static int translateImplementationDefinedFormat(unsigned usage);

    /**
     * @brief convert android framework buffer format and usage to ISP
     * internal format
     */
    ePxlFormat convertBufferFormat(int format, unsigned usage);

    /**
     * @brief Initializes our hardware pipelines with provided params object
     */
    status_t programParameters(
        const ISPC::ParameterList &parameters);

    status_t programParameters() {
        return programParameters(mDefaultParameters);
    }

    /**
     * @brief Allocates control modules, connects to main pipeline
     */
    status_t initControlModules(
            const ISPC::ParameterList &parameters);

    status_t initControlModules() {
        return initControlModules(mDefaultParameters);
    }

    /**
     * @brief Parse ISPC config file and initialize parameter container
     *
     * @param paramsFile filename of configuration source file
     * @param parameters parameters container, default is global
     * @ref mDefaultParameters
     */
    status_t loadParametersFromFile(const std::string& paramsFile,
            ISPC::ParameterList &parameters);

    status_t loadParametersFromFile(const std::string& paramsFile) {
        return loadParametersFromFile(paramsFile, mDefaultParameters);
    }

    /**
     * @brief Acquire shot from all active pipelines
     */
    status_t acquireShot(ISPC::Shot& acquiredShot);

    /**
     * @brief Set crop to full sensor image (no crop)
     * and downscale to output
     * window
     *
     * @param stream descriptor of output stream
     * @param rect crop rectangle
     * @return status of operation
     */
    status_t scaleAndCrop(const camera3_stream_t* stream,
        const Rect& rect);

    /**
     * @brief Set crop to full sensor image (no crop)
     * and downscale to output window
     *
     * @param stream descriptor of output stream
     * @return status of operation
     */
    status_t scaleAndNoCrop(const camera3_stream_t* stream);

    void getAvailableEffects(const uint8_t*& effectArray, uint8_t& count) {
        effectArray = EffectsAvailable.array();
        count = EffectsAvailable.size();
    }

    status_t setEffect(
        camera_metadata_enum_android_control_effect_mode_t mode);

    status_t setSceneMode(
        camera_metadata_enum_android_control_scene_mode_t);

    /**
     * @brief return max supported display width & height
     */
    void getMaxOutputResolution (uint32_t& width, uint32_t& height) const {
        width = mMaxHwSupportedWidth; height = mMaxHwSupportedHeight;
    }

    bool getSensorResolution (uint32_t& width, uint32_t& height) {
        Mutex::Autolock m(mHwLock);

        sensor_t* sensor = getSensor();
        if(mStatus != NO_ERROR || !sensor) {
            return false;
        }
        width = sensor->uiWidth; height = sensor->uiHeight;
        return true;
    }

    /**
     * @brief return hardware ISP clock value in Hz
     */
    uint32_t getHwClockHz() const {
        return sHWInfo.ui32RefClockMhz*1000000;
    }

    /**
     * @brief Return width and height of lens shading map defined in default
     * configuration params
     * @note the data is read from @ref mDefaultParameters
     */
    bool getLSHMapSize(int32_t& width, int32_t& height);

    /**
     * @brief Synchronise the system clock with HW timestamped reference clock
     * @note A delegate of HwTimeBase
     */
    nsecs_t processHwTimestamp(const uint32_t hwTimestamp);

    void initHwNanoTimer(void);

    typedef List<sp<cameraHw_t> >     Pipelines_t;
    typedef Pipelines_t::iterator PipelinesIter_t;
    /**
     * @brief pointer to ISPC::Camera class basic method
     */
    typedef IMG_RESULT (cameraHw_t::*mPtr_t)(void);

    typedef IMG_RESULT (cameraHw_t::*mPtr2_t)(unsigned int);

    // executes provided method on all hw pipelines
    status_t forEach(mPtr_t method);
    status_t forEach(mPtr2_t method, unsigned int);

    void clearStreams() { buffersMap.clear(); }

    void dump();
    void dumpPipelines(std::ostream& output);

    Mutex& getHwLock() { return mHwLock; }

    static const uint32_t kImplementationDefinedEncoderFormat;
    static const uint32_t kImplementationDefinedDisplayFormat;

    /**
     * @brief shortcut mapping between parameter tag which value contains
     * name of parse-able text file and result of parse ISPC::ParameterList
     */
    struct Param_t {
        constexpr static const char* END="END";
        const uint8_t      modeIndex;
        const std::string  paramName;
        ISPC::ParameterList params;
        Param_t(const uint8_t index, const std::string& name) :
            modeIndex(index), paramName(name) {};
        Param_t() : modeIndex(0xff), paramName(END) {}
        /** @brief true if contains "END", marks end of table */
        bool isEnd() const { return paramName == END; }
        bool isValid() const {
            return !paramName.empty() && params.validFlag; }
    };

private:
    sp<sensor_t>      mSensor; ///<@brief single sensor instance
    sp<sensorDg_t>    mSensorDg; ///<@brief single Dg sensor instance

    uint8_t           mSensorMode;
    bool              mExternalDG; ///<@brief use external data generator

    /**
     * @brief contains all initialized pipeline instances
     */
    Pipelines_t mPipelines;
    wp<cameraHw_t>  mMainPipeline;
    typedef KeyedVector<const camera3_stream_t*, wp<cameraHw_t> > StreamsMap;

    /**
     * @brief contains all stream to HW pipeline mappings
     */
    StreamsMap buffersMap;

    // Felix hardware capabilities
    static uint32_t mMaxHwSupportedWidth; // max image width supported by hardware
    static uint32_t mMaxHwSupportedHeight; // max image height supported by hardware
    static uint8_t  mHwContexts; // number of hardware contexts supported in HW

    /**
     * @brief Bitmap of hardware contexts used at current moment
     * @note a bit set to 1 means a specific context is available
     * @note contexts are indexed from 0 to mHwContexts-1
     */
    static BitSet32  mHwContextsAvailable;

    static CI_HWINFO sHWInfo; ///<@brief hardware info struct
    HwTimeBase mHwTimeBase; ///<@brief hardware time base handler class

    // static configuration of hardware pixel format capabilities
    struct BufferFormatPair {
        int cameraHalBufferFormat;
        ePxlFormat ispcBufferFormat;
    };

    /**
     * @brief static conversion table between CI and Android
     * pixel format types
     */
    static const struct BufferFormatPair bufferFormatConvArray[];

    /**
     * @brief default ISPC parameters loaded from file
     */
    ISPC::ParameterList mDefaultParameters;

    /**
     * @brief ISPC parameters containing filename of effect configuration
     * @note indexed by camera_metadata_enum_android_control_effect_mode_t
     * enum
     */
    Param_t EffectConfigFiles[EFFECT_TBL_SIZE];

    /**
     * @brief container for android.control.availableEffects
     */
    SortedVector<uint8_t> EffectsAvailable;

    /**
     * @brief ISPC parameters containing filename of scene modes configuration
     * @note indexed by camera_metadata_enum_android_control_scene_mode_t
     * enum
     */
    Param_t SceneModeConfigFiles[SCENE_TBL_SIZE];

    /**
     * @brief container for android.control.availableSceneModes
     */
    SortedVector<uint8_t> SceneModesAvailable;

    int32_t mLshWidthCache;  ///<@brief cached value of LSH map width
    int32_t mLshHeightCache; ///<@brief cached value of LSH map height
    status_t mStatus;

    /**
     * @brief Hardware lock, shared by all HwManager instances
     */
    static Mutex mHwLock;
};

} // namespace android

#endif // HW_MANAGER_H
