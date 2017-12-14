/**
*******************************************************************************
 @file sensorapi.h

 @brief Header describing the API for controlling sensors in the V2500 
 development system

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

#ifndef SENSOR_API_H
#define SENSOR_API_H
#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include <img_types.h>
#include <img_defs.h>
#include <stdio.h> // FILE*
#include <felixcommon/pixel_format.h>

/**
 * @defgroup SENSOR_API Sensor Interface: API documentation
 * @brief Documentation of the Sensor Interface user-side API.
 * 
 * The Sensor API is intended to provide an abstraction of common sensor setup
 * and control functions so that the IMG AAA functions and camera HAL can 
 * easily be re-targeted at other camera sensors. 
 *
 * To see how to add a new sensor look at the @ref SENSOR_GUIDE
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the SENSOR_API documentation module
 *---------------------------------------------------------------------------*/

/**
 * @brief Handle to the functions of the sensor
 */
typedef struct _Sensor_Functions_* SENSOR_HANDLE;


/** @brief maximum size of the string for the sensor info name string */
#define SENSOR_INFO_NAME_MAX 64
/** @brief maximum size of the string for the sensor version string */
#define SENSOR_INFO_VERSION_MAX 64

typedef enum _Sensor_Flipping_
{
    SENSOR_FLIP_NONE = 0,
    /** @brief Horizontal mirroring of the image */
    SENSOR_FLIP_HORIZONTAL = 1, 
    /** @brief Vertical mirroring of the image */
    SENSOR_FLIP_VERTICAL = (SENSOR_FLIP_HORIZONTAL)<<1, 
    /** @brief Horizontal and vertical mirroring of the image */
    SENSOR_FLIP_BOTH = (SENSOR_FLIP_HORIZONTAL|SENSOR_FLIP_VERTICAL),
} SENSOR_FLIPPING;

/**
 * @brief sensor states
 *
 * When the sensor is Uninitialised you can do nothing other than initialise 
 * it, this will return a handle that you use in subsequent calls.
 * User should never see an Uninitialised sensor as while it is in this state 
 * they do not have a handle to it.
 *
 * In Idle you destroy the sensor, you can also adjust parameters such as 
 * setting the mode, exposure, gain and focus, and change mode.
 *
 * When running you can disable the sensor or adjust flipping, focus ,exposure 
 * and gain. you cannot change mode.
 */
typedef enum _Sensor_State_ {
    /**
     * @brief Sensor has not been initialised should never see this as it 
     * should always be either STATE_IDLE or STATE_RUNNING
     */
    SENSOR_STATE_UNINITIALISED=0,
    /**
     * @brief Sensor has been initialised and is idle
     */
    SENSOR_STATE_IDLE,
    /**
     * @brief Sensor has been initialised and is running
     */
    SENSOR_STATE_RUNNING
}SENSOR_STATE;

/**
 * @brief Status of the sensor (state + selected mode + selected flipping)
 */
typedef struct _Sensor_Status_ {
    /**
     * @brief Mode number that sensor is currently running in, this is an index
     * to the list of modes return by the GetSensorMode call
     */
    IMG_UINT16 ui16CurrentMode;
    /**
     * @brief Selected flipping (composed with values from @ref SENSOR_FLIPPING)
     */
    IMG_UINT8 ui8Flipping;
#ifdef INFOTM_ISP
    double fCurrentFps;
#endif //INFOTM_ISP
    /**
     * @brief State of the sensor
     */
    SENSOR_STATE eState;
} SENSOR_STATUS;

/**
 * @brief Contains information about a sensor mode
 */
typedef struct _Sensor_Mode_
{
    /**
     * @brief Mode's bitdepth
     */
    IMG_UINT8 ui8BitDepth;
    /**
     * @brief width of output in pixels
     */
    IMG_UINT16 ui16Width;
    /**
     * @brief height of output in pixels
     */
    IMG_UINT16 ui16Height;
    /**
     * @brief frame-rate in Hz
     */
    double flFrameRate;
    /**
     * @brief pixel rate in Mpx/s
     *
     * This can usually be computed from
     * ui16HorizontalTotal*ui16VerticalTotal*flFrameRate but may vary for
     * some sensor
     */
    double flPixelRate;
    /**
     * @brief Horizontal total size (including blanking) in pixels
     *
     * Uesed by the system to compute the bit-rate
     */
    IMG_UINT16 ui16HorizontalTotal;
    /**
     * @brief Vertical total size (including blanking) in pixels
     *
     * Used by the system to allow it to pass the information to the flicker 
     * detection
     */
    IMG_UINT16 ui16VerticalTotal;
    /**
     * @brief Supports flipping when enabling
     */
    IMG_UINT8 ui8SupportFlipping;
    
    IMG_UINT32 ui32ExposureMin;
    IMG_UINT32 ui32ExposureMax;
#ifdef INFOTM_ISP
    IMG_DOUBLE dbExposureMin;
#endif //INFOTM_ISP
    
    IMG_UINT8 ui8MipiLanes;
}SENSOR_MODE;

/**
 * @brief Sensor + status + mode info
 */
typedef struct _Sensor_Info_
{
    /**
     * @brief original CFA filter layout (before flipping)
     *
     * @ref eBayerEnabled for currently enabled bayer format
     *
     * @note SHOULD NOT be affected by mode selection
     */
    enum MOSAICType eBayerOriginal;
    
    /**
     * @brief enabled CFA filter layout (including flipping)
     *
     * This is filled up by sensor's function as sensors may have different
     * patterns if flipping is enabled or not
     *
     * Should be derived from @ref SENSOR_INFO::eBayerOriginal and
     * @ref SENSOR_STATUS::ui8Flipping
     */
    enum MOSAICType eBayerEnabled;

    /** 
     * @brief a Text name for the sensor for easy identification
     */
    char pszSensorName[SENSOR_INFO_NAME_MAX]; 
    /**
     * @brief to be used where multiple versions of the sensor exists
     */
    char pszSensorVersion[SENSOR_INFO_VERSION_MAX];

    /**
     * @brief Aperture of the attached lens
     */
    double fNumber;

    /**
     * @brief Focal length of the lens (does not take into account any 
     * magnification due to cropping)
     */
    IMG_UINT16 ui16FocalLength;

    /**
     * @brief Number of electrons the sensor wells can take before clipping
     */
    IMG_UINT32 ui32WellDepth;
    
    /**
     * @brief noise of the sensor in electrons (standard deviation)
     */
    double flReadNoise;

    /**
     * @brief V2500 gasket used by the imager
     */
    IMG_UINT8 ui8Imager;

    /**
     * @brief Is this a back or front facing camera. 
     *
     * To be used by application layer to determine horizontal/vertical flip 
     * orientations
     */
    IMG_BOOL bBackFacing;

    /**
     * @brief Current status of the sensor
     *
     * @note Filled by @ref Sensor_GetInfo() before calling sensor's function 
     * - no need to re-implement in sensor
     */
    SENSOR_STATUS sStatus;
    
    /**
     * @brief Information about current mode
     *
     * @note Filled by @ref Sensor_GetInfo() before calling sensor's function 
     * - no need to re-implement in sensor
     */
    SENSOR_MODE sMode;
#ifdef INFOTM_ISP	
    IMG_UINT32 ui32ModeCount;
#endif //INFOTM_ISP

}SENSOR_INFO;

/* Parameter handling
The main parameters that we are interested in handling are Gain, Exposure, Sensor orientation and focus.
In order to allow an extensible system that can cope with other potential parameters we shall use a mechanism similar to the OMX extension system

So calls to will fill in the address of the param name (we would expect to use common ones for common parameters e.g. GlobalGain, but this
way will allo a tweaking application to require very little specific knowledge) it will also return the max,min and current values for the configuration.
The return value wivoid Sensor_PrintAllModes(FILE *f)ll be the index for parameters that are known, and -1 when the parameter does not exist (i.e. we have reached the end of the list)
This function can only be called in the idle state or above*/
/* Using a simple naming scheme will allow an application to group related parameters together e.g.
"Invert.Horizontal" and "Invert.Vertical" could be used*/

/**
 * @brief Interface functions for the sensor device.
 *
 * This should be populated by the sensor driver to provide an interface that
 * can cope with multiple sensors implemented by different vendors whilst 
 * providing a consistent point of entry
 *
 * This structure should be populate at the creation time of a sensor.
 *
 * All function must be implemented.
 * If they are marked as optional it is allowed to always have an erroneous
 * return such as IMG_ERROR_NOT_SUPPORTED
 */
typedef struct _Sensor_Functions_
{
    /**
     * @name Controls and information
     * @{
     */
    /**
     * @brief Fill in a structure detailing the modes the sensor supports
     *
     * @param hHandle the sensor
     * @param nIndex the index for the mode to check.
     * @param[out] psModes pointer to a Sensor_Mode structure listing the mode
     * corresponding to the index.
     *
     * @return IMG_SUCCESS if a mode with the corresponding index was found
     * @return IMG_ERROR_VALUE_OUT_OF_RANGE if the mode index does not
     * exist
     */
    IMG_RESULT (*GetMode)(SENSOR_HANDLE hHandle, IMG_UINT16 nIndex,
        SENSOR_MODE *psModes);
    /**
     * @brief Get the current state of the sensor
     *
     * @param hHandle the sensor
     * @param[out] psStatus pointer to a SENSOR_STATUS structure to be filled 
     * in by the call
     *
     * @return IMG_SUCCESS
     */
    IMG_RESULT (*GetState)(SENSOR_HANDLE hHandle, 
        SENSOR_STATUS *psStatus);
    /**
     * @brief Set the mode of the sensor
     *
     * A sensor mode should be a combination of size and frame-rate
     *
     * @param hHandle the sensor
     * @param nMode index into the sensor mode list returned by the 
     * GetSensorModes function
     * @param ui8Flipping Flip the sensor image horizontal readout
     * direction (mirror mode) or in vertical readout direction (use 
     * combinaison of @ref SENSOR_FLIPPING)
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_VALUE_OUT_OF_RANGE if nMode is not a valid mode
     * @return IMG_ERROR_NOT_SUPPORTED if selected flipping isn't available
     * @return IMG_ERROR_UNEXPECTED_STATE if sensor already enabled
     */
    IMG_RESULT (*SetMode)(SENSOR_HANDLE hHandle, IMG_UINT16 nMode,
        IMG_UINT8 ui8Flipping);
    /**
     * @brief Enable the sensor
     *
     * The difference between configured (done with @ref SENSOR_FUNCS::SetMode)
     * and enabled is that a configured sensor may not be sending data yet 
     * while an enabled one should be sending data.
     *
     * @param hHandle the sensor
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the device cannot be enabled
     * (either it is already enabled or not in correct state to enable)
     */
    IMG_RESULT (*Enable)(SENSOR_HANDLE hHandle);
    /**
     * @brief Disable the sensor
     *
     * The sensor should stop sending data but not be destroyed (i.e. 
     * @ref SENSOR_FUNCS::Enable should make it start sending data again)
     *
     * @param hHandle the sensor
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the device cannot be disabled
     * (either it is already disabled or not in correct state to disable)
     */
    IMG_RESULT (*Disable)(SENSOR_HANDLE hHandle);
    /**
     * @brief Release and de-initialise sensor
     *
     * @param hHandle the sensor
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the device cannot be destroyed
     * (it should be IDLE to be destroyable)
     */
    IMG_RESULT (*Destroy)(SENSOR_HANDLE hHandle);
    
    /**
     * @brief Get information about the sensor - dependent on what was chosen 
     * with @ref SENSOR_FUNCS::SetMode
     * 
     * See @ref SENSOR_INFO members description to know what this function
     * should populate.
     *
     * @param hHandle the sensor
     * @param[out] return info structure
     *
     * @return IMG_SUCCESS
     */
    IMG_RESULT (*GetInfo)(SENSOR_HANDLE hHandle, SENSOR_INFO *psInfo);

    /**
     * @}
     * @name Gains and exposure
     * @{
     */

    /**
     * @brief Get the range of the gain settings the sensor can support
     *
     * @param hHandle the sensor
     * @param[out] pflMin pointer to minimum gain setting
     * @param[out] pflMax pointer to maximum gain setting
     * @param[out] pui8Contexts the number of separate gains
     *
     * @note This range can include both analogue and digital gain. 
     * The adjustment should apply analogue first and if the required setting
     * needs some extra then apply digital gain.
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if setMode() has not been called
     */
    IMG_RESULT (*GetGainRange)(SENSOR_HANDLE hHandle, double *pflMin, 
        double *pflMax, IMG_UINT8 *puiContexts);
    /**
     * @brief Get the current gain setting for the sensor
     *
     * @param hHandle the sensor
     * @param[out] pflCurrent Pointer to receive the current gain setting
     * @param ui8Context the gain context that this setting refers to
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_VALUE_OUT_OF_RANGE if ui8Context is not a valid 
     * context
     * @return IMG_ERROR_UNEXPECTED_STATE if the state is not valid to get
     * gains
     */
    IMG_RESULT (*GetCurrentGain)(SENSOR_HANDLE hHandle, double *pflCurrent,
        IMG_UINT8 ui8Context);
    /**
     * @brief Set the current gain setting for the sensor
     *
     * @param hHandle the sensor
     * @param flGain The new setting to use
     * @param ui8Context the gain context that this setting refers to
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_VALUE_OUT_OF_RANGE if ui8Context is not a valid 
     * context
     * @return IMG_ERROR_UNEXPECTED_STATE if the state is not valid to change
     * gains
     */
    IMG_RESULT (*SetGain)(SENSOR_HANDLE hHandle, double flGain,
        IMG_UINT8 ui8Context);
    /**
     * @brief Get the range of exposures the sensor can use
     *
     * @param hHandle the sensor
     * @param[out] pui32Min the shortest exposure (measured in microseconds)
     * @param[out] pui32Max the longest exposure the sensor will allow
     * @param[out] pui8Contexts the number of separate exposures
     *
     * @note If an exposure is longer than the frame period this can cause the 
     * frame-rate to slow down
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the state is not valid to get
     * exposure ranges
     */
    IMG_RESULT (*GetExposureRange)(SENSOR_HANDLE hHandle, IMG_UINT32 *pui32Min,
        IMG_UINT32 *pui32Max, IMG_UINT8 *pui8Contexts);
    /**
     * @brief Query the sensor for the current exposure period
     *
     * @param hHandle the sensor
     * @param[out] pui32Exposure pointer to hold the queried exposure value
     * @param ui8Context the exposure context that this setting refers to
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_VALUE_OUT_OF_RANGE if ui8Context is not a valid 
     * context
     */
    IMG_RESULT (*GetExposure)(SENSOR_HANDLE hHandle, IMG_UINT32 *pui32Exposure,
        IMG_UINT8 ui8Context);
    /**
     * @brief Set the sensor to the requested exposure period
     *
     * @param hHandle the sensor
     * @param ui32Exposure target exposure period in microseconds.
     * @param ui8Context the exposure context that this setting change will 
     * apply to
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_VALUE_OUT_OF_RANGE if ui8Context is not a valid  
     * context
     */
    IMG_RESULT (*SetExposure)(SENSOR_HANDLE hHandle, IMG_UINT32 ui32Exposure,
        IMG_UINT8 ui8Context);
    
    /**
     * @name Focus control
     * @{
     */

    /**
     * @brief [optional] Get the range of allowed focus positions
     *
     * @param hHandle the sensor
     * @param[out] pui16Min the minimum focus position in mm
     * @param[out] pui16Max the maximum focus position in mm
     *
     * @return IMG_SUCCESS
     */
    IMG_RESULT (*GetFocusRange)(SENSOR_HANDLE hHandle, IMG_UINT16 *pui16Min,
        IMG_UINT16 *pui16Max);
    /**
     * @brief [optional] Query the current focus position
     *
     * @param hHandle the sensor
     * @param[out] pui16Current the current focus position in mm
     *
     * @return IMG_SUCCESS
     */
    IMG_RESULT (*GetCurrentFocus)(SENSOR_HANDLE hHandle, 
        IMG_UINT16 *pui16Current);
    /**
     * @brief [optional] Set the sensor to focus at the selected position
     *
     * @param hHandle the sensor
     * @param ui16Focus the target focus position that the sensor should set in
     * mm
     *
     * @return IMG_SUCCESS
     */
    IMG_RESULT (*SetFocus)(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Focus);

    /**
     * @}
     * @name Miscellaneous controls
     * @{
     */
    
    /**
     * @brief [optional] Set the sensor to the requested exposure period
     *
     * @param hHandle the sensor
     * @param bAlwaysOn If set, the flash is always enabled, if false then the
     * flash will be enabled for i16Frames, after a delay of i16FrameDelay 
     * frames
     * @param i16FrameDelay delay between this call and the enabling of the 
     * flash
     * @param i16Frames number of frames for flash to be active
     * @param ui16FlashPulseWidth Pulse width of flash pulse for Xenon flashes,
     * measured in pixel clock cycles.
     *
     * @return IMG_SUCCESS
     */
    IMG_RESULT (*ConfigureFlash)(SENSOR_HANDLE hHandle, IMG_BOOL bAlwaysOn, 
        IMG_INT16 i16FrameDelay, IMG_INT16 i16Frames,
        IMG_UINT16 ui16FlashPulseWidth);
    
    /**
     * @brief [optional] Used to control insertion point of the sensor.
     *
     * When call a frame should be inserted into the sensor.
     * The mechanism of setting the source of the data has to be handled using
     * extended parameters and is specific per sensor.
     *
     * Optionally after an insert a sensor can require
     * @ref SENSOR_FUNCS::WaitProcessed to be called to wait for the frame
     * to have been fully processed.
     *
     * @param hHandle the sensor
     *
     * @return IMG_SUCCESS
     */
    IMG_RESULT (*Insert)(SENSOR_HANDLE hHandle);

    /**
     * @brief [optional] Used to wait for an inserted frame to have been
     * fully processed and sent to the ISP.
     *
     * @param hHandle the sensor
     *
     * @return IMG_SUCCESS
     */
    IMG_RESULT (*WaitProcessed)(SENSOR_HANDLE hHandle);
#ifdef INFOTM_ISP	
	IMG_RESULT (*SetFlipMirror)(SENSOR_HANDLE hHandle, IMG_UINT8 flag);
    IMG_RESULT (*SetFPS)(SENSOR_HANDLE hHandle, double fps);
    IMG_RESULT (*SetGainAndExposure)(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context);
    IMG_RESULT (*GetFixedFPS)(SENSOR_HANDLE hHandle, int *pfixed);
    IMG_RESULT (*SetResolution)(SENSOR_HANDLE hHandle, int imgW, int imgH);
    IMG_RESULT (*GetSnapRes)(SENSOR_HANDLE hHandle, reslist_t *preslist);
    IMG_RESULT (*Reset)(SENSOR_HANDLE hHandle);
    IMG_UINT8* (*ReadSensorCalibrationData)(SENSOR_HANDLE hHandle, int infotm_method, int sensor_index/*0 or 1*/, IMG_FLOAT awb_convert_gain, IMG_UINT16* otp_calibration_version);
    IMG_UINT8* (*ReadSensorCalibrationVersion)(SENSOR_HANDLE hHandle, int infotm_method, int sensor_index/*0 or 1*/, IMG_UINT16* otp_calibration_version);
    IMG_RESULT (*UpdateSensorWBGain)(SENSOR_HANDLE hHandle, int infotm_method, IMG_FLOAT awb_convert_gain);
	IMG_RESULT (*ExchangeCalibDirection)(SENSOR_HANDLE hHandle, int flag);
#endif //INFOTM_ISP
    /**
     * @}
     */
}SENSOR_FUNCS;


/**
 * @name Sensor controls
 * @brief The main controls of the sensor
 * @{
 */

/**
 * @brief Access to a list of sensor names
 *
 * The index into the list is the sensor index
 */
const char **Sensor_ListAll(void);

/**
 * @brief Access the number of available sensors
 */
unsigned Sensor_NumberSensors(void);

void Sensor_PrintAllModes(FILE *f);

/**
 * @brief Initialise a sensor
 *
 * @note causes a sensor to transition from Uninitialised to Idle
 *
 * @param nSensor The index of the sensor to initialise
 * @param phHandle The handle of the sensor to initialise
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if phHandle is NULL or nSensor is too 
 * big an idea to be supported
 */
#ifdef INFOTM_ISP
IMG_RESULT Sensor_Initialise(IMG_UINT16 nSensor, SENSOR_HANDLE *phHandle, int index);
#else
IMG_RESULT Sensor_Initialise(IMG_UINT16 nSensor, SENSOR_HANDLE *phHandle);
#endif

/**
 * @see Delegates to SENSOR_FUNCS::GetMode
 * @copydoc SENSOR_FUNCS::GetMode
 *
 * @return IMG_ERROR_INVALID_PARAMETERS if hHandle or psModes is NULL
 * @return IMG_ERROR_FATAL if GetMode function is not available for this 
 * sensor
 */
IMG_RESULT Sensor_GetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nIndex,
    SENSOR_MODE *psModes);

/**
 * @see Delegates to SENSOR_FUNCS::GetState
 * @copydoc SENSOR_FUNCS::GetState
 *
 * @return IMG_ERROR_INVALID_PARAMETERS if hHandle or psStatus is NULL
 * @return IMG_ERROR_FATAL if GetState function is not available for this
 * sensor
 */
IMG_RESULT Sensor_GetState(SENSOR_HANDLE hHandle, SENSOR_STATUS *psStatus);

/**
 * @see Delegates to SENSOR_FUNCS::SetMode
 * @copydoc SENSOR_FUNCS::SetMode
 
 * @return IMG_ERROR_INVALID_PARAMETERS if hHandle is NULL
 * @return IMG_ERROR_FATAL if SetMode function is not available for this sensor
 */
IMG_RESULT Sensor_SetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nMode,
    IMG_UINT8 ui8Flipping);

/**
 * @see Delegates to SENSOR_FUNCS::Enable
 * @copydoc SENSOR_FUNCS::Enable
 *
 * @return IMG_ERROR_INVALID_PARAMETERS if hHandle is NULL
 * @return IMG_ERROR_FATAL if Enable function is not available for this sensor
 */
IMG_RESULT Sensor_Enable(SENSOR_HANDLE hHandle);

/**
 * @see Delegates to SENSOR_FUNCS::Disable
 * @copydoc SENSOR_FUNCS::Disable
 *
 * @return IMG_ERROR_INVALID_PARAMETERS if hHandle is NULL
 * @return IMG_ERROR_FATAL if Disable function is not available for this sensor
 */
IMG_RESULT Sensor_Disable(SENSOR_HANDLE hHandle);

/**
 * @see Delegates to SENSOR_FUNCS::Destroy
 * @copydoc SENSOR_FUNCS::Destroy
 *
 * @return IMG_ERROR_INVALID_PARAMETERS if hHandle is NULL
 * @return IMG_ERROR_FATAL if Destroy function is not available for this sensor
 */
IMG_RESULT Sensor_Destroy(SENSOR_HANDLE hHandle);

/**
 * @brief Get information about the sensor
 *
 * @param hHandle the sensor
 * @param[out] psInfo info structure
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if hHandle or psInfo is NULL
 * @return IMG_ERROR_FATAL if GetInfo function is not available for this sensor
 */
IMG_RESULT Sensor_GetInfo(SENSOR_HANDLE hHandle, SENSOR_INFO *psInfo);

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the sensor controls
 *---------------------------------------------------------------------------*/ 
/**
 * @name Focus controls
 * @brief Optional control of the focus of the sensor
 * @{
 */ 
    
/**
 * @see Delegates to SENSOR_FUNCS::GetFocusRange
 * @copydoc SENSOR_FUNCS::GetFocusRange
 *
 * @return IMG_ERROR_INVALID_PARAMETERS if hHandle, pui16Min or pui16Max is
 * NULL
 * @return IMG_ERROR_NOT_SUPPORTED if GetFocusRange function is not available
 * for this sensor as it is an optional implementation
 */
IMG_RESULT Sensor_GetFocusRange(SENSOR_HANDLE hHandle, IMG_UINT16 *pui16Min,
    IMG_UINT16 *pui16Max);

/**
 * @see Delegates to SENSOR_FUNCS::GetCurrentFocus
 * @copydoc SENSOR_FUNCS::GetCurrentFocus
 *
 * @return IMG_ERROR_INVALID_PARAMETERS if hHandle or pui16Current is NULL
 * @return IMG_ERROR_NOT_SUPPORTED if GetCurrentFocus function is not available
 * for this sensor as it is an optional implementation
 */
IMG_RESULT Sensor_GetCurrentFocus(SENSOR_HANDLE hHandle, 
    IMG_UINT16 *pui16Current);

/**
 * @see Delegates to SENSOR_FUNCS::SetFocus
 * @copydoc SENSOR_FUNCS::SetFocus
 *
 * @return IMG_ERROR_INVALID_PARAMETERS if hHandle is NULL
 * @return IMG_ERROR_NOT_SUPPORTED if SetFocus function is not available for
 * this sensor as it is an optional implementation
 */
IMG_RESULT Sensor_SetFocus(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Focus);

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the focus controls
 *---------------------------------------------------------------------------*/ 
/**
 * @name Exposure and gain controls
 * @brief Exposure and gain controls for the selected mode
 * @{
 */ 

/**
 * @see Delegates to SENSOR_FUNCS::GetGainRange
 * @copydoc SENSOR_FUNCS::GetGainRange
 *
 * @return IMG_ERROR_INVALID_PARAMETERS if hHandle, pflMin, pflMax or 
 * puiContexts is NULL
 * @return IMG_ERROR_FATAL if GetGainRange function is not available for this 
 * sensor
 */
IMG_RESULT Sensor_GetGainRange(SENSOR_HANDLE hHandle, double *pflMin, 
    double *pflMax, IMG_UINT8 *pui8Contexts);

/**
 * @see Delegates to SENSOR_FUNCS::GetCurrentGain
 * @copydoc SENSOR_FUNCS::GetCurrentGain
 *
 * @return IMG_ERROR_INVALID_PARAMETERS if hHandle or pflCurrent is NULL
 * @return IMG_ERROR_FATAL if GetCurrentGain function is not available for this
 * sensor
 */
IMG_RESULT Sensor_GetCurrentGain(SENSOR_HANDLE hHandle, double *pflCurrent,
    IMG_UINT8 ui8Context);

/**
 * @see Delegates to SENSOR_FUNCS::SetGain
 * @copydoc SENSOR_FUNCS::SetGain
 *
 * @return IMG_ERROR_INVALID_PARAMETERS if hHandle is NULL
 * @return IMG_ERROR_FATAL if SetGain function is not available for this sensor
 */
IMG_RESULT Sensor_SetGain(SENSOR_HANDLE hHandle, double flGain,
    IMG_UINT8 ui8Context);

/**
 * @see Delegates to SENSOR_FUNCS::GetExposureRange
 * @copydoc SENSOR_FUNCS::GetExposureRange
 *
 * @return IMG_ERROR_INVALID_PARAMETERS if hHandle, pui32Min, pui32Max or
 * pui8Contexts is NULL
 * @return IMG_ERROR_FATAL if GetExposureRange function is not available for 
 * this sensor
 */
IMG_RESULT Sensor_GetExposureRange(SENSOR_HANDLE hHandle, IMG_UINT32 *pui32Min,
    IMG_UINT32 *pui32Max, IMG_UINT8 *pui8Contexts);

/**
 * @see Delegates to SENSOR_FUNCS::GetExposure
 * @copydoc SENSOR_FUNCS::GetExposure
 *
 * @return IMG_ERROR_INVALID_PARAMETERS if hHandle or pui32Exposure is NULL
 * @return IMG_ERROR_FATAL if GetExposure function is not available for this
 * sensor
 */
IMG_RESULT Sensor_GetExposure(SENSOR_HANDLE hHandle, IMG_UINT32 *pui32Exposure,
    IMG_UINT8 ui8Context);

/**
 * @see Delegates to SENSOR_FUNCS::SetExposure
 * @copydoc SENSOR_FUNCS::SetExposure
 *
 * @return IMG_ERROR_INVALID_PARAMETERS if hHandle is NULL
 * @return IMG_ERROR_FATAL if SetExposure function is not available for this
 * sensor
 */
IMG_RESULT Sensor_SetExposure(SENSOR_HANDLE hHandle, IMG_UINT32 ui32Exposure,
    IMG_UINT8 ui8Context);

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the exposure/gain controls
 *---------------------------------------------------------------------------*/ 
/**
 * @name Miscellaneous controls
 * @{
 */ 

/**
 * @see Delegates to SENSOR_FUNCS::Insert
 * @copydoc SENSOR_FUNCS::Insert
 *
 * @return IMG_ERROR_INVALID_PARAMETERS if hHandle is NULL
 * @return IMG_ERROR_NOT_SUPPORTED if Insert function is not available for this
 * sensor as it is an optional implementation
 */
IMG_RESULT Sensor_Insert(SENSOR_HANDLE hHandle);

/**
 * @see Delegates to SENSOR_FUNCS::WaitProcessed
 * @copydoc SENSOR_FUNCS::WaitProcessed
 *
 * @return IMG_ERROR_INVALID_PARAMETERS if hHandle is NULL
 * @return IMG_ERROR_NOT_SUPPORTED if WaitProcessed function is not available
 * for this sensor as it is an optional implementation
 */
IMG_RESULT Sensor_WaitProcessed(SENSOR_HANDLE hHandle);

/**
 * @see Delegates to SENSOR_FUNCS::ConfigureFlash
 * @copydoc SENSOR_FUNCS::ConfigureFlash
 *
 * @return IMG_ERROR_INVALID_PARAMETERS if hHandle is NULL
 * @return IMG_ERROR_NOT_SUPPORTED if ConfigureFlash function is not available 
 * for this sensor as it is an optional implementation
 */
IMG_RESULT Sensor_ConfigureFlash(SENSOR_HANDLE hHandle, IMG_BOOL bAlwaysOn, 
    IMG_INT16 i16FrameDelay, IMG_INT16 i16Frames,
    IMG_UINT16 ui16FlashPulseWidth);
#ifdef INFOTM_ISP
IMG_RESULT Sensor_SetFlipMirror(SENSOR_HANDLE hHandle, IMG_UINT8 flag);
IMG_RESULT Sensor_SetFPS(SENSOR_HANDLE hHandle, double fps);
IMG_RESULT Sensor_GetFixedFPS(SENSOR_HANDLE hHandle, int *pfixed);
IMG_RESULT Sensor_SetGainAndExposure(SENSOR_HANDLE hHandle, double flGain, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context);
IMG_RESULT Sensor_SetResolution(SENSOR_HANDLE hHandle, int imgW, int imgH);
IMG_RESULT Sensor_GetSnapRes(SENSOR_HANDLE hHandle, reslist_t *presList);
IMG_RESULT Sensor_Reset(SENSOR_HANDLE hHandle);
IMG_RESULT Sensor_GetGasketNum(SENSOR_HANDLE hHandle, IMG_UINT32 *GasketNum);

IMG_UINT8* Sensor_ReadSensorCalibrationData(SENSOR_HANDLE hHandle, int infotm_method, int sensor_index/*0 or 1*/, IMG_FLOAT awb_convert_gain, IMG_UINT16* otp_calibration_version);
IMG_UINT8* Sensor_ReadSensorCalibrationVersion(SENSOR_HANDLE hHandle, int infotm_method, int sensor_index/*0 or 1*/, IMG_UINT16* otp_calibration_version);
IMG_RESULT Sensor_UpdateSensorWBGain(SENSOR_HANDLE hHandle, int infotm_method, IMG_FLOAT awb_convert_gain);
IMG_RESULT Sensor_ExchangeCalibDirection(SENSOR_HANDLE hHandle, int flag);
#endif //INFOTM_ISP
/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the miscellaneous controls
 *---------------------------------------------------------------------------*/ 
/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the SENSOR_API documentation module
 *---------------------------------------------------------------------------*/

/**
 * @page SENSOR_GUIDE Sensor Module Implementation Guide

 The Sensor API is intended to be easy to add custom sensors to. 
 The API defines a set of functions that a sensor driver should support to allow the Felix AAA algorithms and applications to interact with the sensor to produce a correctly exposed image of the required format.
 
 # Sensor API

To add a sensor add an entry providing the sensor name to the Sensors array in felix_dev_sensors.c and add a pointer to its Initialise function.


e.g.

    IMG_RESULT (*InitialiseSensors[])(SENSOR_HANDLE *phHandle) =
    {
        DummyCamCreate,
        DGCamCreate,
        AR330CamCreate,
        P401CamCreate,
        NewCamCreate
    };

    char *Sensors[]={
        "Dummy Camera",
        "Data Generator",
        "AR330 Camera",
        "P401 Camera",
        "MyNewCam"
    };

The initialise function should fill in a /a SENSOR_FUNCS   structure to point to the functions used to control the sensor, and return this as a handle that the user can use to call the sensor control functions. In order to keep some state it is recommended to place this at the start of a larger camera context structure

e.g.

 @code typedef struct _newcam_struct
    {
      SENSOR_FUNCS sFuncs;
      IMG_UINT16 ui16CurrentMode;
      IMG_BOOL nEnabled;
      IMG_UINT32 ui32ExposureMax;
      IMG_UINT32 ui32Exposure;
      IMG_UINT32 ui32ExposureMin;
      IMG_UINT8 ui8MipiLanes;
      SENSOR_I2C * psI2C;
      double flGain;
    }NEWCAM_STRUCT;

    IMG_RESULT NewCamCreate(SENSOR_HANDLE *phHandle)
    {
        MYNEWCAM_STRUCT *psCam;

        psCam=(MYNEWCAMCAM_STRUCT *)IMG_CALLOC(sizeof(MYNEWCAMCAM_STRUCT), 1);
        if(!psCam)
            return IMG_ERROR_MALLOC_FAILED;

        *phHandle=(void*)psCam;
        psCam->sFuncs.GetSensorModes=MYNEWCAM_GetSensorModes;
        psCam->sFuncs.GetSensorState=MYNEWCAM_GetSensorState;
        psCam->sFuncs.SetSensorMode=MYNEWCAM_SetSensorMode;
        psCam->sFuncs.EnableSensor=MYNEWCAM_EnableSensor;
        psCam->sFuncs.DisableSensor=MYNEWCAM_DisableSensor;
        psCam->sFuncs.DestroySensor=MYNEWCAM_DestroySensor;

        //psCam->sFuncs.GetCurrentFocus stays NULL so is Unsupported;
        //psCam->sFuncs.GetFocusRange stays NULL so is Unsupported;
        //psCam->sFuncs.SetFocus stays NULL so is Unsupported;

        psCam->sFuncs.GetCurrentGain=MYNEWCAM_GetGain;
        psCam->sFuncs.GetGainRange=MYNEWCAM_GetGainRange;
        psCam->sFuncs.SetGain=MYNEWCAM_SetGain;

        psCam->sFuncs.SetExposure=MYNEWCAM_SetExposure;
        psCam->sFuncs.GetExposureRange=MYNEWCAM_GetExposureRange;
        psCam->sFuncs.GetExposure=MYNEWCAM_GetExposure;
        
        psCam->nEnabled=0;
        psCam->ui16CurrentMode=0;

        psCam->psI2C= SensorI2CInit(0,0x20,0x21,400*1000,IMG_TRUE);

        LOG_INFO("Sensor initialised\n");
        return IMG_SUCCESS;
    }
@endcode

The Initialise function will probably need to get a handle to the I2C subsytem as shown above, by calling the SensorI2CInit() function.
It can then use this to call the SensorI2CWrite8(), SensorI2CWrite16(),SensorI2CRead8(),SensorI2CRead16() functions and also to call the DoGasket() function which must be called just before a sensor is enabled


Other issues which may need to be taken into account.
If the SOC has an external gasket then this will need to be configured. In this case the doGasket function should be modified to do whatever is required




*/
/*-------------------------------------------------------------------------
 * End of the SENSOR_GUIDE documentation module
 *------------------------------------------------------------------------*/

#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

#endif // SENSOR_API_H




