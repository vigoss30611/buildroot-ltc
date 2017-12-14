/**
*******************************************************************************
 @file Pipeline.h

 @brief Declaration of ISPC::Pipeline

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
#ifndef ISPC_PIPELINE_H_
#define ISPC_PIPELINE_H_

#include <img_types.h>
#include <ci/ci_api_structs.h>

#include <map>
#include <iostream>
#include <string>

#include "ispc/Module.h"
#include "ispc/ParameterList.h"
#include "ispc/Shot.h"
#include "ispc/ISPCDefs.h"
#include "ispc/Sensor.h"

namespace ISPC {

/** @brief Number of Modules considered as globals */
#define PIPELINE_N_GLOBALS 2

/**
 * @brief The Pipeline class serves as an interface with the HW pipeline and
 * connects with the lower layers in the driver stack.
 *
 * It provides functionality to register different setup functions for the
 * different modules in the pipeline and provides different mechanisms for
 * updating them.
 * Apart from the modules configuration this class stores ifnormation about
 * the pipeline gloabl configuration and gives access to the MC and CI
 * driver layers if required.
 */
class Pipeline
{
protected:
    /**
     * @brief The module registry: stores the set of Module objects (1 per
     * actual HW module) used to setup the pipeline
     */
    std::map<SetupID, SetupModule *> moduleRegistry;

    /** @brief pointer to the camera sensor object */
    Sensor *sensor;

    /** @brief Connection to the CI layer */
    CI_CONNECTION *pCIConnection;
    /** @brief pointer to a pipeline configuration in CI level */
    CI_PIPELINE *pCIPipeline;
    /** @brief pointer to a pipeline configuration through the MC level */
    MC_PIPELINE *pMCPipeline;

    /**
     * @brief Maximum output size of the display scaler in pixels.
     *
     * Related to MC_PIPELINE::ui16MaxDispOutWidth
     *
     * See @ref programPipeline() @ref setDisplayDimensions()
     */
    IMG_UINT32 ui32MaxDisplayWidth;
    /**
     * @brief Maximum output size of the display scaler in pixels.
     *
     * Related to MC_PIPELINE::ui16MaxDispOutHeight
     *
     * See @ref programPipeline() @ref setDisplayDimensions()
     */
    IMG_UINT32 ui32MaxDisplayHeight;
    /**
     * @brief Maximum output size of the display scaler in pixels.
     *
     * Related to MC_PIPELINE::ui16MaxEncOutWidth
     *
     * See @ref programPipeline() @ref setEncoderDimensions()
     */
    IMG_UINT32 ui32MaxEncoderWidth;
    /**
     * @brief Maximum output size of the display scaler in pixels.
     *
     * Related to MC_PIPELINE::ui16MaxEncOutHeight
     *
     * See @ref programPipeline() @ref setEncoderDimensions()
     */
    IMG_UINT32 ui32MaxEncoderHeight;

    /** @brief The Pipeline can be configured for tiled buffers or not */
    bool bEnableTiling;

public:
    typedef std::map<SetupID, SetupModule *>::const_iterator const_iterator;

    /** @brief HW context to use */
    IMG_INT8 ui8ContextNumber;
    /**
     * @brief status of the context (initialized, not initialized, ready,
     * etc.)
     *
     * @ define the states and have the object behave better (and a way
     * to clean erroneous state!)
     */
    CtxStatus ctxStatus;

public:  // methods
    /**
     * @brief modules that are considered global
     *
     * These modules are expected to be updated less often as they affect
     * the setup of other modules. They usually configure the output formats
     * and maximum sizes of the capture.
     */
    static SetupID globalModulesID[PIPELINE_N_GLOBALS];

    /**
     * @brief Initializes the pipeline ans allocates space for CI and MC
     * structs used for configure the HW.
     *
     * This constructor is protected to limit the instantiation of Pipeline
     * objects.
     *
     * @param pCIConnection Connection to the CI driver
     * @param ctxNumber Number of the context we want to connect with
     * @param sensor Pointer to a Sensor object used to setup the actual
     * camera sensor
     */
    Pipeline(CI_CONNECTION *pCIConnection, unsigned int ctxNumber,
        Sensor *sensor);

    /**
     * @brief Destructor. Releases the allocated MC and CI structs used to
     * configure the HW
     */
    ~Pipeline();

    /**
     * @brief Access to CI Connection (const) to retrieve information about
     * HW and driver connection
     */
    const CI_CONNECTION* getConnection() const;

    /**
     * @brief Access to MC pipeline (not const so that modules can modify it)
     *
     * @see const equivalent just to retrieve information
     */
    MC_PIPELINE* getMCPipeline();

    /**
     * @brief Access to MC pipeline (const) to retrieve setup information
     */
    const MC_PIPELINE* getMCPipeline() const;

    /**
     * @brief Access to CI Pipeline (const) to retrieve converted information
     * from MC pipeline
     */
    const CI_PIPELINE* getCIPipeline() const;

    CI_PIPELINE* getCIPipeline();

    /**
     * @brief Get Global setup information from the configured modules
     *
     * @warning This function assumes the pMCPipeline is already populated
     * correctly for IIF!!!
     *
     * @param[out] ret (optional) return code
     */
    Global_Setup getGlobalSetup(IMG_RESULT *ret = 0) const;

    Sensor* getSensor();

    const Sensor* getSensor() const;

    void setSensor(Sensor *_sensor);

    /**
     * @brief Is Pipeline configured for tiled buffer? Does not verify if HW
     * supports tiling.
     */
    bool tilingEnabled() const;

    /**
     * @brief Enables Pipeline support for tiling. User needs to check if HW
     * supports tiling first.
     *
     * Without enabling that tiled buffers cannot be used.
     * @warning But only enable it if tiling needs to be used as it creates
     * size limitations for the buffers
     *
     * @warning Tiling should be enabled before the 1st call to
     * @ref programPipeline()
     */
    void setTilingEnabled(bool b);

    /**
     * @name Module management
     * @brief Register, clear, access, load and save modules
     * @{
     */

    /**
     * @brief Register a module in the pipeline
     *
     * Once the module is registered the Pipeline instance will be in charge
     * of deleting the object.
     *
     * If a module with the same ID was already registered it is replaced
     *
     * @param module Pointer to the module to be registered
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if module is NULL
     */
    IMG_RESULT registerModule(SetupModule *module);

    /**
     * @brief Retrieve a registered module
     * @param id module identifier
     * @return A pointer to the module corresponding with the id
     * @return NULL if such module is not registered.
     */
    SetupModule* getModule(SetupID id);

    /**
     * @brief const access to modules to retrieve information
     */
    const SetupModule* getModule(SetupID id) const;

    /**
     * @brief Templated getter for control modules. Does not need static
     * casting
     *
     * usage example: ModuleDSC* mod = getModule<ModuleDSC>();
     */
    template <class T>
    T* getModule(void)
    {
        return static_cast<T*>(getModule(T::id));
    }

    template <class T>
    const T* getModule(void) const
    {
        return static_cast<const T*>(getModule(T::id));
    }

    /**
     * @brief Removes and deletes all the modules registered in the pipeline.
     *
     * @warning Each module is actually destroyed so it must not be destroyed
     * outside this function.
     */
    void clearModules();

    /**
     * @brief Reload all the registered modules configuration from the
     * received list of parameters
     *
     * @param parameters List of parameters to read the configuration from
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Pipeline is in the wrong state
     * @return Delegates to all @ref SetupModule::load() functions
     * @see ParameterList
     */
    IMG_RESULT reloadAll(const ParameterList &parameters);

    IMG_RESULT reloadAllModules(const ParameterList &parameters);

    IMG_RESULT reloadAllGlobals(const ParameterList &parameters);

    /**
     * @brief Reload the specified module configuration from the received
     * list of parameters
     *
     * @param parameters List of parameters to read the configuration from
     * @param id of the module to be configured
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if the given module was not found
     * @return Delefates to the the @ref SetupModule::load() function
     */
    IMG_RESULT reloadModule(SetupID id, const ParameterList &parameters);

    /**
     * @brief Save the current modules configuration to a list of parameters
     *
     * @param[in, out] paremeters to alterate
     * @param[in] t what to save
     *
     * @return IMG_SUCCESS
     * @return Delegates to all @ref SetupModule::save() functions
     *
     * See @ref SetupModule::SaveType
     */
    IMG_RESULT saveAll(ParameterList &paremeters,
        ModuleBase::SaveType t = ModuleBase::SAVE_VAL) const;

    /**
     * @brief Save a specific module to a list of parameters
     *
     * @param[in, out] parameters to alterate
     * @param id of the module to save
     * @param[in] t what to save
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if the given module was not found
     * @return Delegates to the @ref SetupModule::save() function
     *
     * See @ref SetupModule::SaveType
     */
    IMG_RESULT saveModule(SetupID id, ParameterList &parameters,
        ModuleBase::SaveType t = ModuleBase::SAVE_VAL) const;

    /**
     * @name Global setup access
     * @brief Access to the Global module
     *
     * Global parameter are parameters not belonging to any particular
     * module althought they may depend on other modules configuration.
     * @{
     */

    /**
     * @name Modules setup
     * @brief Access to the registerd Modules
     * @{
     */

    /**
     * @brief Setup all the modules in the pipeline (which have not been
     * setup already) including globals
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Pipeline is in the wrong state
     * @ref SetupModule::setup() functions
     */
    IMG_RESULT setupAll();

    /**
     * @brief Setup only the global modules (see
     * @ref Pipeline::globalModulesID[])
     */
    IMG_RESULT setupAllGlobals();

    /**
     * @brief Setup all modules but the global ones (see
     * @ref Pipeline::globalModulesID[])
     */
    IMG_RESULT setupAllModules();

    /**
     * @brief Setup the requested module (if it has not been setup already).
     *
     * @param id ID of the module to setup
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Pipeline is in the wrong state
     * @return IMG_ERROR_INVALID_PARAMETERS if the given module was not found
     * @return Delegates to the registered @ref SetupModule::setup() functions
     */
    IMG_RESULT setupModule(SetupID id);

    /**
     * @brief Setup all modules with priorirty <= the specified one
     *
     * @param priority
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Pipeline is in the wrong state
     * @return Delegates to the registered @ref SetupModule::setup() functions
     */
    IMG_RESULT setupByPriority(modulePriority priority);

    /**
     * @brief Setup all the modules that have not been setup yet
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Pipeline is in the wrong state
     * @return Delegates to the registered @ref SetupModule::setup() functions
     */
    IMG_RESULT setupPending();

    /**
     * @brief Setup all the modules with the requested flag activated
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Pipeline is in the wrong state
     * @return Delegates to the registered @ref SetupModule::setup() functions
     */
    IMG_RESULT setupRequested();

    /**
     * @brief Verifies if the configuration is correct for the current HW
     * version
     */
    IMG_RESULT verifyConfiguration() const;

    /**
     * @brief Program current configuration in the pipeline.
     *
     * The setup of all the modules is maintained in an intermediate
     * structure until this function is called.
     * At such point the configuration is dumped in the HW through the lower
     * layers of the driver.
     *
     * Before updating the pipeline, we overwrite dimensions programmed in
     * MC_PipelineConvert() to align with imported buffer strides using
     * @ref ui32MaxDisplayWidth, @ref ui32MaxDisplayHeight and
     * @ref ui32MaxEncoderWidth, @ref ui32MaxEncoderHeight.
     * This functionality is used mainly in Android where framework allocates
     * buffers of sizes aligned to current dimensions of respective output.
     * @warning The dispBuf* and encBuf* fields shall reflect scaler output
     * dimensions for chosen streams.
     * It is the responsibility of camera app to provide buffers with valid
     * sizes for current streams.
     *
     * @param bUpdateASAP if pipeline is already registered chooses between
     * calling @ref CI_PipelineUpdate() and @ref CI_PipelineUpdateASAP()
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Pipeline is in the wrong state
     * @return IMG_ERROR_FATAL if a MC or CI function fails
     */
    IMG_RESULT programPipeline(bool bUpdateASAP = false);

    /**
     * @name Buffers and Shots allocation management
     * @brief Allocation or import Buffers and Shots
     * @see Camera for explanation on Buffers and Shots differences
     * @{
     */

    /**
     * @brief Adds a number of Shots to the Pipeline
     *
     * Shots must be allocated in the pipeline before being able to start and
     * run captures.
     *
     * Buffers must be allocated with @ref allocateBuffer() or imported
     * @ref importBuffer() before starting.
     *
     * The Pipeline becomes ready when at least 1 shot is available
     * (buffers are optional).
     *
     * @note Shots remains available on the whole Pipeline lifetime unless
     * deleted with @ref deleteShots().
     * Shots contains settings for the ENS output which may change if
     * Pipeline is stopped and restarted.
     * Therefore Shots HAVE to be deleted only if the ENS changes
     * (otherwise the ENS output may be too small and the low level driver
     * may disable it).
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Pipeline is in the wrong state
     * @return IMG_ERROR_INVALID_PARAMETERS if num is 0
     * @return IMG_ERROR_FATAL if CI function failed
     *
     * @see addBufferPool() importBuffer() allocateBuffer()
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
     * @return IMG_ERROR_UNEXPECTED_STATE if the Pipeline is in the wrong state
     * @return IMG_ERROR_FATAL if CI function failed
     */
    IMG_RESULT deleteShots();

    /**
     * @brief Allocate 1 buffer for the selected output (pipeline must be
     * preconfigured)
     *
     * Buffers are not necessary (e.g. run only to acquire statistics) but no
     * output will be available once started.
     *
     * @note If tiling is used setTilingEnabled() should be set to true AND
     * user has to check if HW can support tiling.
     *
     * @param[in] eBuffer type to allocate (TYPE_RGB, TYPE_BAYER, TYPE_YUV)
     * @param[in] ui32Size in bytes - if 0 CI will compute the size of the
     * buffer
     * @param[in] isTiled allocate a tiled buffer or not
     * @param[out] pBufferId storage for created buffer ID (can be NULL
     * if not interested)
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Pipeline is in the wrong state
     * @return IMG_ERROR_FATAL if CI function failed
     *
     * @see importBuffer() addShots() deregisterBuffer() setTilingEnabled()
     */
    IMG_RESULT allocateBuffer(CI_BUFFTYPE eBuffer, IMG_UINT32 ui32Size,
        bool isTiled = false, IMG_UINT32 *pBufferId = NULL);

    /**
     * @brief This function imports a buffer
     *
     * Buffers allocated outside of ISPC should be imported.
     *
     * @note If tiling is used setTilingEnabled() should be set to true AND
     * user has to check if HW can support tiling.
     *
     * @param[in] eBuffer buffer type to import (TYPE_RGB, TYPE_BAYER,
     * TYPE_YUV)
     * @param[in] ionFd import identified for the importation mechanism
     * (when using ION it is the fd)
     * @param[in] ui32Size in bytes for the importation. Has to be >0
     * @param[in] isTiled
     * @param[out] pBufferId buffer ID to use to trigger specific shots or
     * deregister the buffer - can be NULL
     *
     * @note In practice could import a buffer of size 0 because CI would
     * compute the size but it is better than the external allocator
     * provides the size so that CI checks it fits for the configured
     * pipeline outputs.
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if eBuffer is TYPE_NONE or
     * ionFd is invalid (0)
     * @return IMG_ERROR_UNEXPECTED_STATE if the context is in the wrong state
     * @return IMG_ERROR_FATAL if CI function failed
     *
     * @see allocateBuffer() addShots() deregisterBuffer() setTilingEnabled()
     */
    IMG_RESULT importBuffer(CI_BUFFTYPE eBuffer, IMG_UINT32 ionFd,
        IMG_UINT32 ui32Size, bool isTiled = false,
        IMG_UINT32 *pBufferId = NULL);

    /**
     * @brief Deregisters an allocated or imported buffer (The pipeline must
     * be stopped)
     *
     * If the buffer was allocated it is freed, if imported it is released.
     *
     * @param[in] uiBufferID to liberate
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Pipeline is in the wrong state
     * @return IMG_ERROR_FATAL if CI function failed
     *
     * @see allocateBuffer() importBuffer()
     */
    IMG_RESULT deregisterBuffer(IMG_UINT32 uiBufferID);

    /**
     * @brief Change the display output maximum dimensions
     *
     * @warning Does not affect the selected display scaler configuration
     * but user has to ensure the configuration produces sizes below the
     * selected maximum!
     *
     * @param width in pixels
     * @param height in pixels
     *
     * @see programPipeline()
     */
    IMG_RESULT setDisplayDimensions(unsigned int width, unsigned int height);

    /**
     * @brief Change the encoder output maximum dimensions
     *
     * @warning Does not affect the selected display scaler configuration
     * but user has to ensure the configuration produces sizes below the
     * selected maximum!
     *
     * @param width in pixels
     * @param height in pixels
     *
     * @see programPipeline()
     */
    IMG_RESULT setEncoderDimensions(unsigned int width, unsigned int height);

    IMG_UINT32 getMaxDisplayWidth() const;
    IMG_UINT32 getMaxDisplayHeight() const;
    IMG_UINT32 getMaxEncoderWidth() const;
    IMG_UINT32 getMaxEncoderHeight() const;

    /**
     * @}
     * @name Capture control
     * @brief Start, stop capture and acquire results
     * @{
     */

    /**
     * @brief Reserve the hardware for capture.
     *
     * Once the capture is started we are able to request captures from the HW.
     * Buffers must have been allocated prior to the usage of this function if
     * outputs are needed
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Pipeline is in the wrong state
     * @return IMG_ERROR_FATAL if CI function failed
     *
     * @see stopCapture()
     */
    IMG_RESULT startCapture();

    /**
     * @brief Stop the capture process releasing the HW.
     *
     * The HW is available to be reserved by other users.
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Pipeline is in the wrong state
     * @return IMG_ERROR_FATAL if CI function failed
     *
     * @see startCapture()
     */
    IMG_RESULT stopCapture();

    /**
     * @brief This function programs shot with provided buffer id.
     *
     * Once the buffers are allocated and the HW reserveed (with the call to
     * startCapture) we are able to program shots to be captured with this
     * function.
     *
     * Buffers should already be imported or allocated.
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Pipeline is in the wrong state
     * @return IMG_ERROR_FATAL if CI function failed
     *
     * @see programShot() if the used buffers do not matter
     */
    IMG_RESULT programSpecifiedShot(const CI_BUFFID &buffId);

    /**
     * @brief Program a shot to be captured in the pipeline (takes 1st
     * available Shot and 1st available Buffers)
     *
     * Once the buffers are allocated and the HW reserveed (with the call
     * to startCapture) we are able to program shots to be captured with this
     * function.
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Pipeline is in the wrong state
     * @return IMG_ERROR_FATAL if CI function failed
     *
     * @see programSpecifiedShot() if the Buffers used matter
     */
    IMG_RESULT programShot();

    /**
     * @brief Get the identifier of the 1st available buffers to run with the
     * current configuration.
     *
     * @warning Will not provide a HDR insertion ID as
     * @ref getAvailableHDRInsertion() should be used for that
     *
     * @param[out] buffId
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Pipeline is in the wrong state
     * @return IMG_ERROR_FATAL if CI function failed
     *
     * See @ref CI_PipelineFindFirstAvailable()
     */
    IMG_RESULT getFirstAvailableBuffers(CI_BUFFID &buffId);

    /**
     * @brief Get the associated HDR insertion buffer - if id is 0 then get
     * the 1st available one.
     *
     * HDR insertion buffers should be reserved with this function and
     * released when triggering a frame
     *
     * @note CI has a mechanism to release an acquired HDR buffer without
     * triggering a frame: @ref CI_PipelineReleaseHDRBuffer()
     *
     * @param[out] HDRInsertion
     * @param id if 0 then get the 1st available otherwise search for specified
     * identifier
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Pipeline is in the wrong state
     * @return IMG_ERROR_FATAL if CI function failed
     *
     * See @ref CI_PipelineAcquireHDRBuffer()
     */
    IMG_RESULT getAvailableHDRInsertion(CI_BUFFER &HDRInsertion,
        IMG_UINT32 id = 0);

    /**
     * @brief Retrieve a captured shot from the pipeline. The Shot must be
     * released later
     *
     * Will acquire a previously programmed shot from the pipeline.
     *
     * @param[out] shot
     * @param[in] block will wait for a shot to be available or will return
     * directly
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Pipeline is in the wrong state
     * @return IMG_ERROR_FATAL if CI function failed
     * @return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE if block=false and failed
     * to acquire a frame
     *
     * @see programShot() programSpecifiedShot()
     */
    IMG_RESULT acquireShot(Shot &shot, bool block = true);

    /**
     * @brief Release a previously captured and retrieved shot.
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Pipeline is in the wrong state
     * @return IMG_ERROR_FATAL if CI function failed
     *
     * @see acquireShot()
     */
    IMG_RESULT releaseShot(Shot &shot);

    const_iterator getModulesStart() const;
    const_iterator getModulesEnd() const;
    /**
     * @}
     */

private:
    /**
     * @brief Auxiliary process of a captured pipeline shot.
     *
     * Populates the metadata and the buffers points of the ISPC::Shot from
     * the information available in CI_SHOT
     */
    void processShot(Shot &shot, CI_SHOT *pCIBuffer);
};

} /* namespace ISPC */

#endif /* ISPC_PIPELINE_H_ */
