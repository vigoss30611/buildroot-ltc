/**
*******************************************************************************
 @file Sensor.h

 @brief Declaration of ISPC::Sensor

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
#ifndef ISPC_SENSOR_H_
#define ISPC_SENSOR_H_

#include <felixcommon/pixel_format.h>
#include <sensorapi/sensorapi.h>
#include <img_errors.h>

#include <iostream>
#include <list>
#include <utility>  // pair
#include <string>

#include "ispc/Connection.h"
#include "ispc/Parameter.h"
#include "ispc/ParameterList.h"
#include "ispc/Module.h"  // ModuleBase::SaveType

namespace ISPC {

/**
 * @brief Sensor class encapsulates the sensor api and provide the basic
 * functions for initializing the sensor and setting exposure time and gains.
 */
class Sensor
{
public:
    /** @brief Sensor running state */
    enum State
    {
        SENSOR_ERROR, /**< @brief Sensor is in error estate */
        SENSOR_INITIALIZED, /**< @brief Sensor initialization is complete */
        SENSOR_ENABLED, /**< @brief Sensor is enabled for capture */
        SENSOR_CONFIGURED, /**< @brief Sensor is configured */
    };

    // sensor info
    /** @brief exposure in ms */
    static const ParamDef<double> SENSOR_EXPOSURE;
    /** @brief gain multiplier */
    static const ParamDef<double> SENSOR_GAIN;
    /**
     * @note save only as information - loaded from sensorAPI even if
     * using a data generator
     */
    static const ParamDef<unsigned int> SENSOR_BITDEPTH;
    static const ParamDef<unsigned int> SENSOR_WELLDEPTH;
    static const ParamDef<double> SENSOR_READNOISE;
    static const ParamDef<double> SENSOR_FRAMERATE;
    static const ParamDefArray<unsigned int> SENSOR_SIZE;
    /**
     * @note save only as information - loaded from sensorAPI even if
     * using a data generator
     */
    static const ParamDef<unsigned int> SENSOR_VTOT;
#ifdef INFOTM_ISP
    State state;
#endif

    static ParameterGroup GetGroup();

    /**
    * @brief Get sensor state name
    *
    * @param s Sensor state
    */
    static const char* StateName(State s);

    /**
    * @brief Populate a list with the available sensors from Sensor API
    *
    * @param [out] sensorNames appends name to the list
    */
    static void GetSensorNames(std::list<std::pair<std::string, int> >
        &sensorNames);

    /**
    * @brief Get sensor id from a name
    *
    * @param name of the sensor in Sensor API
    *
    * @return positive identifier or negative number if sensor not found
    */
    static int GetSensorId(const std::string &name);

protected:
    /** @brief Sensor handle that controls the camera */
    SENSOR_HANDLE hSensorHandle;

    /** @brief Exposure time programmed in the sensor (microseconds) */
    IMG_UINT32 programmedExposure;
    /** @brief Gain programmed in the sensor */
    double programmedGain;
    /** @brief Focus distance set in the sensor */
    IMG_UINT16 programmedFocus;

    /** @brief Minumum exposure times (microseconds) */
    IMG_UINT32 minExposure;
    /** @brief Maximum exposure times (microseconds) */
    IMG_UINT32 maxExposure;
    /** @brief Minimum focus distance (milimetre) */
    IMG_UINT16 minFocus;
    /** @brief Maximum focus distance (milimetre) */
    IMG_UINT16 maxFocus;
    /** @brief Minimum exposure gain */
    double minGain;
    /** @brief Maximum exposure gain */
    double maxGain;
#ifndef INFOTM_ISP
    State state;
#endif
    /** @brief variable focus support status */
    bool focusSupported;

    /**
     * @brief Protected constructor so that child classes can choose the
     * sensor identifier after calling the parent class constructor
     */
    Sensor();

    /**
     * @brief to centralise the init of the constructor so that children
     * classes can initialise the object as if constructor has been called
     * with a sensor id
     */

#ifdef INFOTM_ISP
    void init(int sensorId, int index = 0);
#else
    void init(int sensorId);
#endif

public:
    /**
     * @name Sensor mode configuration/parameters
     * @{
     */
    /** @brief in pixels */
    unsigned int uiWidth;
    /** @brief in lines */
    unsigned int uiHeight;
    /** imager id */
    unsigned int uiImager;
    /** @brief imager number of lines including blanking */
    unsigned int nVTot;
    /** @brief Mosaic of the sensor */
    enum MOSAICType eBayerFormat;
    unsigned char ui8SensorContexts;
    /** @brief Data size of the sensor */
    unsigned int uiBitDepth;

    /** @brief imager frame rate*/
    double flFrameRate;
#ifdef INFOTM_ISP
    double flCurrentFPS;
#endif
    unsigned int uiWellDepth;
    double flReadNoise;
    double flAperture;
    unsigned int uiFocalLength;
    /**
     * @}
     */

public:  // methods
    /**
     * @brief Create a sensor using given identifier.
     *
     * Sensor is initialised in this function.
     * Mode should be selected with configure() and sensor can then be started
     * using enabled()
     *
     * @param sensorId
     */
    explicit Sensor(int sensorId);
#ifdef INFOTM_ISP
    Sensor(int sensorID, int index);
#endif
    virtual ~Sensor();

    /**
     * @name Sensor information
     * @{
     */

    /**
     * @brief Get maximum exposure time value programmable in the sensor
     * (microseconds).
     */
    unsigned long getMaxExposure() const;

    /**
     * @brief Get minimum exposure time value programmable in the sensor
     * (microseconds).
     */
    unsigned long getMinExposure() const;

    /** @brief Get maximum gain value programmable in the sensor */
    double getMaxGain() const;

    /**
     * @brief Get minimum gain value programmable in the sensor
     */
    double getMinGain() const;

    /** @brief Get state of driver support for variable focus */
    bool getFocusSupported() const;

    /**
     * @brief Get minimum focus distance programmable in the sensor in 
     * milimiters
     */
    unsigned int getMinFocus() const;

    /**
    * @brief Get maximum focus distance programmable in the sensor in
    * milimiters
    */
    unsigned int getMaxFocus() const;

    State getState() const;

    /**
     * @}
     * @name Sensor accessors
     * @{
     */

    /**
     * @brief Set focus distance in the sensor in milimiters
     *
     * @param focusDistance Focus distance to be set in the sensor
     */
    IMG_RESULT setFocusDistance(unsigned int focusDistance);

    /**
     * @brief Get the focus distance currently programmed in the sensor in
     * milimiters
     *
     * @return Focus distance currently set in the sensor
     */
    unsigned int getFocusDistance() const;

    /**
     * @brief Get some mode information (including if flipping is supported)
     *
     * @param mode for the sensor
     * @param[out] sMode
     */
    IMG_RESULT getMode(int mode, SENSOR_MODE &sMode) const;

    /** @brief Configure the sensor with a selected mode */
    IMG_RESULT configure(int mode = 0, int flipping = SENSOR_FLIP_NONE);

    /** @brief start transmission of the data from the sensor */
    IMG_RESULT enable();

    /** @brief stop transmission of the data from the sensor */
    IMG_RESULT disable();

    /**
    * @brief Program the exposure time in the sensor (microseconds)
    *
    * @param uSeconds Exposure time (microseconds)
    */
    IMG_RESULT setExposure(unsigned long uSeconds);

    /**
     * @brief Get the exposure time programmed in the sensor (microseconds)
     */
    unsigned long getExposure() const;

    /** @brief Set the gain value in the sensor */
    IMG_RESULT setGain(double gain);

    /** @brief Get the gain value programmed in the sensor */
    double getGain() const;

    /**
     * @brief Access to sensors API handle
     *
     * Mainly to be able to call extended functions
     */
    SENSOR_HANDLE getHandle();

    /**
     * @brief Access to the Sensor API insert function (which may not be
     * available on all sensors!)
     */
    virtual IMG_RESULT insert();

    /**
     * @brief Access to the Sensor API waitProcessed function (which may not
     * be available on all sensors!)
     */
    virtual IMG_RESULT waitProcessed();

    /**
     * @brief Load information from setup file - has no effect for a real
     * sensor
     */
    virtual IMG_RESULT load(const ParameterList &parameters);
    /**
     * @brief Save information to a setup file
     */
    virtual IMG_RESULT save(ParameterList &parameters,
        ModuleBase::SaveType t) const;
#ifdef INFOTM_ISP        
    //ISO Limit
    IMG_RESULT setISOLimit(IMG_UINT8 pISOLimit);
    IMG_RESULT GetFlipMirror(SENSOR_STATUS &status);
    IMG_RESULT SetFPS(double fps);
    IMG_RESULT GetSensorState();
    IMG_RESULT GetFixedFPS(int *pfixed);
    IMG_RESULT SetResolution(int imgW, int imgH);
    IMG_RESULT GetSnapResolution(reslist_t &resList);
    IMG_RESULT reset();
    IMG_RESULT GetGasketNum(IMG_UINT32 *Gasket);

#ifdef INFOTM_SENSOR_OTP_DATA_MODULE
    IMG_UINT8* ReadSensorCalibrationData(int infotm_method, int sensor_index/*0 or 1*/, IMG_FLOAT awb_convert_gain, IMG_UINT16* otp_calibration_version);
    IMG_UINT8* ReadSensorCalibrationVersion(int infotm_method, int sensor_index/*0 or 1*/, IMG_UINT16* otp_calibration_version);
    IMG_RESULT UpdateSensorWBGain(int infotm_method, IMG_FLOAT awb_convet_gain);
    IMG_RESULT ExchangeCalibDirection(int flag);
#endif

#endif //INFOTM_ISP    

    // added by linyun.xiong @2015-07-15 for support change Max/Min Gain
#ifdef INFOTM_ISP_TUNING    
    void setMaxGain(double maxGain);
    void setMinGain(double minGain);
    //set sensor flip and mirror.
    IMG_RESULT setFlipMirror(IMG_UINT8 flag);
    IMG_RESULT SetGainAndExposure(double flGain, IMG_UINT32 ui32Exposure);
#endif //INFOTM_ISP    
};

/**
 * @brief If CI support Data Generator this class adds elements to it
 *
 * Check DGCamera::supportExtDG() to know if this class can support external
 * data generator.
 *
 * Check DGCamera::supportIntDg() to know if this class can support internal
 * data generator
 */
class DGSensor : public Sensor
{
public:
    DGSensor(const std::string &filename, IMG_UINT8 gasketNumber = 0,
        bool isInternal = true);
    virtual ~DGSensor();
    /**
     * @brief set at creation time - modifying has no effects
     */
    bool isInternal;

    /**
     * @brief Load information from setup file
     *
     * @note Unlike Sensor::load() loads the sensor information as DG do not
     * have them
     */
    virtual IMG_RESULT load(const ParameterList &parameters);

private:
    CI_Connection ciConnection;
};

} /* namespace ISPC */

#endif /* ISPC_SENSOR_H_ */
