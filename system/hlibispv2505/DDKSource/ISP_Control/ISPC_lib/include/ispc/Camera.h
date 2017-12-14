/**
*******************************************************************************
 @file Camera.h

 @brief Header of the Camera class which represents a camera and provides
 functionality for initialization, connection to the pipeline, registering
 setup modules and control algorithms, etc.

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
#ifndef ISPC_CAMERA_H_
#define ISPC_CAMERA_H_

#include <ci/ci_api_structs.h>

#include <string>
#include <list>
#include <ostream>

#include "ispc/Pipeline.h"
#include "ispc/Module.h"
#include "ispc/Control.h"
#include "ispc/ParameterList.h"
#include "ispc/Connection.h"
#include "ispc/Sensor.h"
#ifdef INFOTM_ISP
#include <Controls/ispc/ControlAE.h>
#include <Controls/ispc/ControlDNS.h>
#include <Controls/ispc/ControlTNM.h>
#include "Modules/ispc/ModuleSHA.h"
#include "Modules/ispc/ModuleR2Y.h"
#endif //INFOTM_ISP


#ifdef INFOTM_ENABLE_COLOR_MODE_CHANGE
extern "C" bool FlatModeStatusGet(void);
extern "C" void FlatModeStatusSet(bool flag);
#endif

#ifdef INFOTM_ENABLE_GAIN_LEVEL_IDX
extern "C" int GainLevelGet(void);
extern "C" void GainLevelSet(int idx);
#endif

namespace ISPC {
    class Camera;
}

std::ostream& operator<<(std::ostream &os, const ISPC::Camera &c);

/**
 * @defgroup ISPC2 ISP Control: library documentation
 * @brief Simplification of the usage of the CI and Sensor API libraries.
 *
 * Using the two lower level libraries:
 * @li @ref CI_API
 * @li @ref SENSOR_API
 *
 * Please refere to the ISP Control library document for information
 * about the usage of the library.
 *
 * Please refere to the ISPC Control Modules document for information
 * about the control algorithms.
 *
 * Main class is @ref ISPC::Camera
 */

/**
 * @brief ISP Control namespace
 * @ingroup ISPC2
 */
namespace ISPC {

typedef CI_BUFFID ISPC_BUFFID;

/**
 * @brief The Camera class implements all the required functionality for
 * connecting with the camera, initialization, registering of setup functions,
 * capture, etc.
 *
 * Internally it includes pointers to Pipeline and Sensor classes instances
 * which can be used for configuring the pipeline and sensor respectively.
 *
 * The Pipeline object includes a setup module registry to be able to implement
 * and register different setup functions.
 *
 * The Camera class also includes a Control class instance which allows to
 * register control algorithms that are executed after each capture (or on
 * demand) and allow the implementation of AE, AWB and AF algorithms (among
 * others).
 *
 * The attached Sensor can be owned (i.e. will be destroyed) or not by the
 * Camera.
 * Camera constructors that create the Sensor own them by default.
 *
 * To create a multi-pipeline configuration that share a Sensor it is possible
 * to create 2 Camera object that have the same Sensor object in common.
 * This can work by either having
 * @li neither of the Camera owning the sensor (then the application HAS TO
 * delete the sensor when needed and handle all the calls to the sensor to
 * replace the Camera) or
 * @li by having only 1 Camera object owning the sensor while the other does
 * not (easier option)
 *
 */
class Camera
{
public:
    /**
     * @brief Camera state to indicate if it is connected or not, initialized or
     * not, ready to capture, etc.
     */
    enum State {
        /**
        * @brief Camera object in error state
        */
        CAM_ERROR,
        /**
        * @brief Camera object is not connected to the driver
        */
        CAM_DISCONNECTED,
        /**
        * @brief Camera object is connected to the driver
        */
        CAM_CONNECTED,
        /**
        * @brief Pipeline modules have been registered
        */
        CAM_REGISTERED,
        /**
        * @brief Pipeline modules have been set up (modules are configured)
        */
        CAM_SET_UP,
        /**
        * @brief Pipeline has been programmed with the modules configuration
        */
        CAM_PROGRAMMED,
        /**
        * @brief Camera is ready for capturing
        */
        CAM_READY,
        /**
        * @brief Camera is capturing. HW is reserved.
        */
        CAM_CAPTURING
    };
    static const char* StateName(State e);

#ifdef INFOTM_ISP
public://for protected to public
#else
protected:
#endif //INFOTM_ISP

    /**
     * @brief Connection to the CI library (object)
     */
    CI_Connection ciConnection;
    /**
     * @brief HW Context number to use
     */
    unsigned int ctxNumber;

    /**
     * @brief A pointer to a Pipeline object which serves as interface to
     * configure the pipeline and manage setup functions.
     */
    Pipeline *pipeline;
    /**
     * @brief A pointer to a Sensor object which serves as interface to
     * configure the physical sensor.
     *
     * Owned by the Camera if bOwnSensor is true
     */
    Sensor *sensor;
    /**
     * @brief To know if the sensor is owned by the camera (i.e. should the
     * Camera delete the sensor when deleted)
     */
    bool bOwnSensor;

    /**
     * @brief The Control class is used to manage the control algorithms
     * executed regularly.
     */
    Control control;

public:
    /**
     * @brief A copy of the CI_HWINFO struct that CI provides with information
     * about the HW.
     */
    CI_HWINFO hwInfo;
    /**
     * @brief Camera state.
     */
    State state;

    /**
     * @brief Enable the usage of update or update ASAP in CI layer when
     * calling @ref Pipeline::programPipeline() function.
     *
     * @note Update ASAP will try to update all shots already in the queue
     * but the CI or ISP layer have no way of knowing if the update was done
     * before the HW already triggered the shot. Therefore it is useful to
     * do update ASAP only when using an unusually big number of buffers (to
     * avoid latency of parameters being applied) and when knowing when
     * parameters were applied is not crutial.
     */
    bool bUpdateASAP;

    double AvgGain;

    bool capture_mode_flag;

#ifdef INFOTM_ISP
    IMG_CHAR* isp_marker_debug_string;
#endif

protected:
    /**
     * @brief enable the sensor and create the pipeline object
     *
     * Shared between constructors.
     */
    void init(int sensorMode = 0, int sensorFlipping = SENSOR_FLIP_NONE);

    /**
     * @brief Constructor for specialised classes
     *
     * Unlike the other constructor init() is not called and has to be called
     * by the specialised constructor if needed
     */
    explicit Camera(unsigned int contextNumber);

public:
    /**
     * @brief Camera constructor. Connects to CI, instantiates and initializes
     * Pipeline and Sensor instances. Also registers all the default modules in
     * the pipeline
     *
     * @param contextNumber The HW context number to be used
     * @param sensorId sensor to initialize. Has to be specified
     * @param sensorMode sensor mode to initialize the sensor with.
     * @param sensorFlipping sensor flipping (see @ref SENSOR_FLIPPING)
     *
     * @see Pipeline Sensor
     * @see Camera(unsigned int, Sensor*)
     * @see Delegates to init() to initialise the Sensor and create the
     * Pipeline object
     */
    Camera(unsigned int contextNumber, int sensorId, int sensorMode = 0,
        int sensorFlipping = SENSOR_FLIP_NONE);
#ifdef INFOTM_ISP
    //for dual sensors support
    Camera(unsigned int contextNumber, int sensorId, int sensorMode = 0,
           int sensorFlipping = SENSOR_FLIP_NONE, int index = 0);
#endif
    /**
     * @brief Camera constructor that takes ownership of the given sensor
     *
     * @see Delegates to init() to initialise the Sensor and create the
     * Pipeline object
     */
    Camera(unsigned int contextNumber, Sensor *pSensor);

    /**
     * @brief Destructor, deletes the Pipeline and Sensor instances and
     * disconnects from CI.
     *
     * @see ~Pipeline() ~Sensor()
     */
    virtual ~Camera();

    /** @brief Returns context number */
    unsigned int getContextNumber() const;

    /**
     * @brief Returns a pointer to the Pipeline instantiated in this Camera
     */
    Pipeline* getPipeline();

    /** @copydoc getPipeline() */
    const Pipeline* getPipeline() const;

    /**
     * @brief Returns a pointer to the Sensor used by this Camera
     */
    Sensor* getSensor();

    /** @copydoc getSensor() */
    const Sensor* getSensor() const;

    /** @brief Is the attached sensor owned by the Camera? */
    bool ownsSensor() const;

    /**
     * @brief Changes the fact that the sensor is owned by the Camera or not
     *
     * Do not change the ownership of sensors lightly.
     * In case of multi-contexts ensure that at maximum of 1 Camera owns a
     * sensor otherwise the sensor may be double-deleted.
     */
    void setOwnSensor(bool b);

    /**
     * @brief Access to the CI connection
     *
     * Should be used only by advanced users who needs special CI interaction
     */
    CI_CONNECTION* getConnection();

    /**
     * @copydoc getConnection()
     */
    const CI_CONNECTION* getConnection() const;

    /**
     * @brief Configure the maximum dimensions of the display output.
     *
     * This affects the buffers to be imported later, so they would match
     * with display pipeline output size (@ref Pipeline::programPipeline()).
     *
     * The maximum sizes cannot change if the capture is running.
     *
     * @param width width in pixels
     * @param height height in pixels
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if camera is CAM_CAPTURING
     * @return IMG_ERROR_FATAL if there is no assocaited Pipeline
     * Delegates to @ref Pipeline::setDisplayDimensions()
     */
    IMG_RESULT setDisplayDimensions(unsigned int width, unsigned int height);

    /**
     * @brief Configure the maximum dimensions of the encoder output.
     *
     * This affects the buffers to be imported later, so they would match
     * with display pipeline output size (@ref Pipeline::programPipeline())
     *
     * @param width width in pixels
     * @param height height in pixels
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if camera is not CAM_CAPTURING
     * @return IMG_ERROR_FATAL if there is no assocaited Pipeline
     * Delegates to @ref Pipeline::setEncoderDimensions()
     */
    IMG_RESULT setEncoderDimensions(unsigned int width, unsigned int height);

    /**
     * @name Module functions
     * @brief Register, load, save and setup ISP modules
     * @{
     */

    /**
     * @brief Updates the configuration of all the modules registered in the
     * pipeline by passing the received parameters to all of them.
     * Control modules are NOT reloaded (see loadControlParameters())
     *
     * After reloading the parameters the setup function of each module is
     * invoked
     *
     * @note If the Camera owns it, the Sensor loads its parameters first
     * (in case it's a data-generator)
     *
     * @param parameters A ParameterList instance containing a set of
     * configuration parameters and corresponding values (could be parsed from
     * a setup file or received by other means).
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Camera is in a wrong state
     * @return IMG_ERROR_FATAL if the Pipeline object is not created
     * @return Delegates to @ref Pipeline::reloadAll() and
     * @ref Pipeline::setupAll()
     */
    IMG_RESULT loadParameters(const ParameterList &parameters);

    /**
     * @brief Updates the configuration for the control modules registered.
     * Modules are NOT reloaded (see loadParameters())
     *
     * Control parameters are expected to be loaded only once, at
     * initialisation time and refined over time using statistics information
     *
     * @param parameters A ParameterList instance containing a set of
     * configuration parameters and corresponding values (could be parsed from
     * a setup file or received by other means).
     *
     * @return IMG_SUCCESS
     * @return Delegates to @ref Control::loadAll()
     */
    IMG_RESULT loadControlParameters(const ParameterList &parameters);

    /**
     * @brief Save all modules, all controls and the sensor values to
     * a parameter file
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_FATAL if the Pipeline object is not created
     * @return Delegates to @ref Pipeline::saveAll() and
     * @ref Control::saveAll()
     */
    IMG_RESULT saveParameters(ParameterList &parameters,
        ModuleBase::SaveType t = ModuleBase::SAVE_VAL) const;

    /**
     * @brief Call setup function of all the modules in the pipeline
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Camera is in a wrong state
     * @return IMG_ERROR_FATAL if the Pipeline object is not created
     * @return Delegates to @ref Pipeline::setupAll()
     */
    IMG_RESULT setupModules();

    /**
     * @brief Call setup function of a particular module in the pipeline
     *
     * @param id ID of the module to be updated
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Camera is in a wrong state
     * @return IMG_ERROR_FATAL if the Pipeline object is not created
     * @return Delegates to @ref Pipeline::setupModule()
     */
    IMG_RESULT setupModule(SetupID id);

    /**
     * @brief Program the pipeline with the module(s) configuration that we
     * have loaded/set up
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Camera is in a wrong state
     * @return IMG_ERROR_FATAL if the Pipeline object is not created
     * @return Delegates to @ref Pipeline::programPipeline()
     */
    IMG_RESULT program();

    /**
     * @brief Template getter for control modules. Does not need static casting
     * usage example: ModuleDSC* mod = getModule<ModuleDSC>();
     */
    template <class T>
    T* getModule(void)
    {
        if ( pipeline )
            return pipeline->getModule<T>();
        return 0;
    }

    /**
     * @copydoc getModule()
     */
    template <class T>
    const T* getModule(void) const
    {
        if ( pipeline )
            return pipeline->getModule<const T>();
        return 0;
    }

    /**
     * @}
     * @name Buffer and Shot allocation management
     * @brief Allocation or import Buffers and Shots
     *
     * Shots are internal information that tight Buffers together for a
     * capture.
     * They contain the metadata and timestamps in internal memory.
     *
     * Buffers can be imported (e.g. when using Android given by the system) or
     * allocated.
     * They contain the output images.
     * @{
     */

    /**
     * @brief Allocate a pool for future captures (both Shot and Buffer).
     *
     * @warning pipeline must be preconfigured to have correct output sizes
     *
     * Similar than calling addShots() and allocateBuffers() several times.
     *
     * @param num Number of elements to allocate
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_FATAL if Camera is in error state
     * @return IMG_ERROR_INVALID_PARAMETERS if num == 0
     * @return Delegates to @ref Camera::allocateBufferPool
     */
    IMG_RESULT allocateBufferPool(unsigned int num);

    /**
     * @brief Allocate a pool for future captures (both Shot and Buffer).
     *
     * @warning pipeline must be preconfigured to have correct output sizes
     *
     * Similar than calling addShots() and allocateBuffers() several times.
     *
     * @param num Number of elements to allocate
     * @param[out] bufferIds optional list of all the created buffer Ids
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_FATAL if Camera is in error state
     * @return IMG_ERROR_INVALID_PARAMETERS if num == 0
     */
    IMG_RESULT allocateBufferPool(unsigned int num,
        std::list<IMG_UINT32> &bufferIds);

    /**
     * @brief Adds a defined number of Shots (NO Buffers)
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_FATAL if pipline is NULL
     * @return IMG_ERROR_UNEXPECTED_STATE if the Camera is in the wrong state
     * @return IMG_ERROR_INVALID_PARAMETERS if num is 0
     * @return Delegates to @ref Pipeline::addShots()
     */
    IMG_RESULT addShots(unsigned int num);

    /**
     * @brief Delete all allocated Shots of the Pipeline - buffers are
     * unnacfected
     *
     * Pipeline must not be started and all acquired Shots should have been
     * released prior to this call.
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_FATAL if pipeline is NULL
     * @return IMG_ERROR_UNEXPECTED_STATE if Camera is in the wrong state
     * @return Delegates to @ref Pipeline::deleteShots()
     */
    IMG_RESULT deleteShots();

    /**
     * @brief Allocate 1 buffer for the selected output (pipeline must be
     * preconfigured)
     *
     * It is recommended to use @ref allocateBufferPool() with the camera
     * instead of allocating buffer one by one.
     * But it is possible to allocate buffers one by one to have a mixture
     * of allocated and imported buffers.
     *
     * See @ref Pipeline::allocateBuffer() for more details.
     *
     * @return IMG_SUCCESS
     * @return Delegates to @ref Pipeline::allocateBuffer()
     */
    IMG_RESULT allocateBuffer(CI_BUFFTYPE eBuffer, IMG_UINT32 ui32Size,
        bool isTiled = false, IMG_UINT32 *pBufferId = 0);

    /**
     * @brief Similar to the other allocate buffer but defaults to ui32Size=0
     * letting CI compute the size of the allocation
     */
    IMG_RESULT allocateBuffer(CI_BUFFTYPE eBuffer, bool isTiled = false,
        IMG_UINT32 *pBufferId = NULL);

    /**
     * @brief Register buffer provided by user space (e.g Android HAL)
     *
     * @param[in] eBuffer type of buffer
     * @param[in] ionFd structure containing internal buffer IDs (0 means
     * none) - e.g. ion fd when using ion allocator
     * @param[in] ui32Size in bytes
     * @param[in] isTiled is the buffer tiled
     * @param[out] pBufferId result buffer ID (can be NULL if not interested
     * in triggering specific buffers)
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_FATAL if pPipline is NULL
     * @return IMG_ERROR_UNEXPECTED_STATE if the Camera is in the wrong state
     * @return Delegates to @ref Pipeline::importBuffer() and
     * @ref Camera::startCapture()
     */
    IMG_RESULT importBuffer(CI_BUFFTYPE eBuffer, IMG_UINT32 ionFd,
        IMG_UINT32 ui32Size, bool isTiled = false, IMG_UINT32 *pBufferId = 0);

    /**
     * @brief Liberate an allocated or imported buffer
     */
    IMG_RESULT deregisterBuffer(IMG_UINT32 uiBufferID);

    /**
     * @}
     * @name Capture control
     * @brief Start, stop capture and acquire results
     * @{
     */

    /**
     * @brief Start the capture. HW is reserved for its use, if sensor is
     * owned then it is enabled.
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Camera is in a wrong state
     * @return IMG_ERROR_NOT_INITIALISED if the Pipeline or Sensor object are
     * NULL
     * @note Sensor object is only checked if Camera owns it
     * @return Delegates to @ref Pipeline::startCapture()
     */
    IMG_RESULT startCapture(const bool enSensor=true);

    /**
     * @brief Stop the capture. HW is not reserved for its use any-more, Sensor
     * is disabled before the Pipeline if owned.
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Camera is in a wrong state
     * @return IMG_ERROR_NOT_INITIALISED if the Pipeline or Sensor object are NULL
     * @note Sensor object is only checked if Camera owns it
     * @return Delegates to @ref Pipeline::stopCapture()
     */
    IMG_RESULT stopCapture(const bool enSensor=true);

    /**
     * @brief Enqueue a new shot in the pipeline for a capture once it is
     * started
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Camera is in a wrong state
     * @return IMG_ERROR_FATAL if the Pipeline object is not created
     * @return Delegates to @ref Pipeline::setupRequested(),
     * @ref Pipeline::programPipeline() and @ref Pipeline::programShot()
     */
    virtual IMG_RESULT enqueueShot();

    /**
     * @brief Enqueue a new shot using specific buffers.
     *
     * @param buffIds structure containing internal buffer IDs (0 means none)
     * - e.g. ion fd when using ion allocator
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Camera is in a wrong state
     * @return IMG_ERROR_FATAL if the Pipeline object is not created
     * @return Delegates to @ref Pipeline::setupRequested(),
     * @ref Pipeline::programPipeline() and
     * @ref Pipeline::programSpecifiedShot()
     */
    virtual IMG_RESULT enqueueSpecifiedShot(const CI_BUFFID &buffIds);

    /**
     * @brief Retrieve a previously enqueued shot. Shots must be released
     * afterwards.
     *
     * If it is specified, it blocks or/and the loop control algorithms
     * are updated.
     *
     * @param shot Shot structure where the capture shot is returned
     * @param block If true then blocks till timeout
     * @param updateControl Specify if the control algorithms should be updated
     * or not.
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Camera is in a wrong state
     * @return IMG_ERROR_FATAL if the Pipeline object is not created
     * @return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE if block==false and \
     * no frame available
     * @return Delegates to @ref Pipeline::acquireShot()
     * and @ref Control::runControlModules()
     *
     * @see releaseShot()
     */
    virtual IMG_RESULT acquireShot(Shot &shot, bool block, bool updateControl);

    /**
     * @brief Retrieve a previously enqueued shot. Shots must be released
     * afterwards.
     *
     * If it is specified the loop control algorithms are updated.
     *
     * @param shot Shot structure where the capture shot is returned
     * @param updateControl Specify if the control algorithms should be updated
     * or not.
     *
     * @return IMG_SUCCESS
     * Delegates to @ref Camera::acquireShot (Shot, bool, bool)
     *
     * @see releaseShot()
     */
    IMG_RESULT acquireShot(Shot &shot, bool updateControl = true);

    /**
     * @brief Retrieve a previously enqueued shot. Shots must be released
     * afterwards.
     *
     * If it is specified the loop control algorithms are updated.
     *
     * @param shot Shot structure where the capture shot is returned
     * @param updateControl Specify if the control algorithms should be updated
     * or not.
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Camera is in a wrong state
     * @return IMG_ERROR_FATAL if the Pipeline object is not created
     * @return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE if block==false and \
     * no frame available
     * @return Delegates to @ref Pipeline::acquireShot()
     * and @ref Control::runControlModules()
     *
     * @see releaseShot()
     */
    IMG_RESULT tryAcquireShot(Shot &shot, bool updateControl = true);

    /**
     * @brief Release a previously returned shot and make the Shot (and its
     * Buffers) available for capture again
     *
     * @param shot to be released
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Camera is in a wrong state
     * @return IMG_ERROR_FATAL if the Pipeline object is not created
     * @return Delegates to @ref Pipeline::releaseShot()
     *
     * @see acquireShot()
     */
    IMG_RESULT releaseShot(Shot &shot);

    /**
     * @}
     * @name Controls
     * @brief Give some access to the attached Control object that handles the
     * control algorithms
     * @{
     */

    /**
     * @brief Register a control module. It's not possible to register two
     * modules with the same id.
     *
     * @param module Control module to be registered
     *
     * @return IMG_SUCCESS
     * @return Delegates to @ref Control::registerControlModule()
     */
    IMG_RESULT registerControlModule(ControlModule *module);

    /**
     * @brief Get the registered control module with the requested id. There is
     * only one possible registered control module with a given ID.
     *
     * @param id Requested control module ID.
     *
     * @return Delegates to @ref Control::getControlModule()
     */
    ControlModule *getControlModule(ControlID id);

    /**
     * @brief Template getter for control modules. Does not need static casting
     * usage example: ControlAE* mod = getControlModule<ControlAE>();
     */
    template <class T>
    T* getControlModule(void)
    {
        return static_cast<T*>(getControlModule(T::id));
    }

    /**
     * @}
     */

    /**
     * @brief print the state of the camera
     *
     * Includes all control modules calling @ref Control::printAllState()
     */
    std::ostream& printState(std::ostream &os) const;
#ifdef INFOTM_ISP
    //AWB

    //Saturation
    IMG_RESULT updateTNMSaturation(IMG_UINT8 pTNMSaturation);

    //ISO Limit
    IMG_RESULT updateISOLimit(IMG_UINT8 pISOLimit);

    //Sharpness
    IMG_RESULT updateSHAStrength(IMG_UINT8 pSHAStrength);

    //EV
    IMG_RESULT updateEVTarget(IMG_UINT8 pEVTarget);

    //Sensor flip and mirror
    IMG_RESULT setSensorFlipMirror(IMG_UINT8 flag);

    //Sepia mode
    IMG_RESULT setSepiaMode(IMG_UINT8 pMode);

    IMG_RESULT CalcAvgGain();

    IMG_RESULT enableLSH(IMG_BOOL Flag);
#ifdef AUTO_CHANGE_LSH
    IMG_RESULT DynamicChangeLSH();
#endif

    IMG_RESULT BuildGammaCurve(double* fPrecompensation, IMG_INT cnt);
    IMG_RESULT DynamicChangeRatio(double & dbSensorGain, double & dbRatio);
    IMG_RESULT DynamicChange3DDenoiseIdx(IMG_UINT32 & ui32DnTargetIdx, bool bForceCheck = false);
    IMG_RESULT DynamicChangeIspParameters(bool bForceCheck = false);
    IMG_RESULT DayModeDetect(int & nDayMode);
    IMG_RESULT SetCaptureIQFlag(IMG_BOOL Flag);
    IMG_BOOL GetCaptureIQFlag();

    //update self LBC(include 3D denoise)
    IMG_RESULT IsUpdateSelfLBC(void);
    IMG_RESULT CameraDumpReg(IMG_UINT32 eBank, IMG_UINT32 ui32Offset, IMG_UINT32 ui32Size);
    IMG_BOOL IsMonoMode(void);

    IMG_CHAR* GetISPMarkerDebugInfo(void);
#endif //INFOTM_ISP
};

class CameraFactory
{
public:
    /**
     * @brief Returns the list of Setup Modules needed to support a given HW
     * version
     *
     * Can be used to populate the Camera object manually
     */
    static std::list<SetupModule*> setupModulesFromHWVersion(
        unsigned int major, unsigned int minor);

    /**
     * @brief Returns a list of Control Modules that could be used with a given
     * HW version
     *
     * This is not expected to be used to create the modules but can give an
     * idea of what could be used.
     */
    static std::list<ControlModule*> controlModulesFromHWVersion(
        unsigned int major, unsigned int minor);

    /**
     * @brief Populates a Camera Setup Modules using HW information from the CI
     * connection
     *
     * @param camera to allocate modules for
     * @param sensor reference sensor to use - 0 if using DG
     * @param perf if >0 register modules in performance measurement
     * (if >1 registers as verbose)
     *
     * @see Get the list of modules using @ref setupModulesFromHWVersion()
     */
    static IMG_RESULT populateCameraFromHWVersion(Camera &camera,
        Sensor *sensor, int perf = 0);
};

/**
 * @brief Implementation of the Camera that uses the data generator sensor.
 *
 * This object will only work if the External Data Generator was compiled in.
 * Use supportDG() to know at run-time if it is available.
 */
class DGCamera: public Camera
{
public:
    /**
     * @brief Run-time check if the external DGCamera is available
     */
    static bool supportExtDG();

    /**
     * @brief Run-time check if the internal DGCamera is available
     */
    static bool supportIntDG();

    /**
     * @brief Creates a Sensor using the Data Generator sensor in sensor API
     *
     * @param contextNumber external DG context to use
     * @param filename to load the FLX image from
     * @param gasketNumber gasket to connect to
     * @param isInternal to know of the data generator is the internal one
     * or the external one
     */
    DGCamera(unsigned int contextNumber, const std::string &filename,
        unsigned int gasketNumber = 0, bool isInternal = false);

    /**
     * @brief Creates a DGCamera with a shared sensor
     */
    DGCamera(unsigned int contextNumber, Sensor *sensor);

    virtual ~DGCamera();

    /**
     * @brief Update the sensor information from the file info (done once at
     * loading with empty list)
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_NOT_INITIALISED is Sensor object is NULL
     */
    virtual IMG_RESULT updateSensorInfo(const ParameterList &param);

    /**
     * @return IMG_ERROR_NOT_INITIALISED if sensor object is NULL
     * @return IMG_ERROR_NOT_SUPPORTED if DG was not compiled in
     * @return Delegates to @ref Camera::enqueueShot() and then calls
     * @ref Sensor::insert()
     */
    virtual IMG_RESULT enqueueShot();
    /**
     * @return IMG_ERROR_NOT_INITIALISED if sensor object is NULL
     * @return IMG_ERROR_NOT_SUPPORTED if DG was not compiled in
     * @return Delegates to @ref Camera::enqueueSpecifiedShot() and then calls
     * @ref Sensor::insert()
     */
    virtual IMG_RESULT enqueueSpecifiedShot(CI_BUFFID &buffId);

    virtual IMG_RESULT acquireShot(Shot &shot, bool block, bool updateControl);
};

}  /* namespace ISPC */


#endif /* ISPC_CAMERA_H_ */
