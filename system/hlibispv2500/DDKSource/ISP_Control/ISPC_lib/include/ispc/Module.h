/**
*******************************************************************************
 @file Module.h

 @brief Declaration of SetupModule and ControlModule base classes

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
#ifndef ISPC_MODULE_H_
#define ISPC_MODULE_H_

#include <string>
#include <list>

#include "ispc/ParameterList.h"
#include "ispc/Shot.h"  // metadata

namespace ISPC {

#ifdef INFOTM_ISP
class Camera;
#endif //INFOTM_ISP
class Sensor;  // ispc/Sensor.h

/**
 * @defgroup ISPC2_SETUPMODULES Setup Modules
 * @ingroup ISPC2
 * @brief HW module configurations that are not producing statistics outputs.
 *
 * These are the classes that handle the convertion from high level parameters
 * to low-level pre-register values (to an MC_PIPELINE).
 * The @ref ISPC2_STATSMODULES group contains additional modules that generate
 * statistics from the processed image.
 *
 * All classes should derive from SetupModule to be storable in a Pipeline 
 * object.
 *
 * The CameraFactor class should be used to list all needed modules per HW
 * version.
 *
 */

/**
 * @defgroup ISPC2_STATSMODULES Setup Modules (statistics)
 * @ingroup ISPC2
 * @brief HW module configurations that produce statistics outputs.
 *
 * These are the classes that handle the convertion from high level parameters
 * to low-level pre-register values (to an MC_PIPELINE).
 * The @ref ISPC2_SETUPMODULES group contains additional modules.
 *
 * All classes should derive from SetupModule to be storable in a Pipeline
 * object.
 *
 * The CameraFactory class should be used to list all needed modules per HW
 * version.
 *
 * Some control modules use these statistics to gather information.
 * Refer to each control module to know which statistics they are using.
 */

/**
 * @defgroup ISPC2_CONTROLMODULES Control Modules
 * @ingroup ISPC2
 * @brief Modules that use statistics and information to re-configure some of
 * the SetupModule.
 *
 * All classes should derive from ControlModule to be storable in a Control
 * object.
 *
 * The CameraFactory class should provide a list of desired controls per HW
 * version.
 */

/* "ispc/Pipeline.h" - forward declared because Pipeline uses SetupModule */
class Pipeline;

/** priority for module setup */
enum modulePriority {HIGH = 0, NORMAL = 1, LOW = 2};
/** update modes: every frame vs. under request */
enum moduleUpdate   {FRAME, REQUEST};

/** @brief Pipeline Setup Modules unique identifiers */
enum SetupID {
    STP_IIF = 0, /**< @brief Imager interface */
    STP_EXS, /**< @brief Encoder statistics */
    STP_BLC, /**< @brief Black level correction */
    STP_RLT, /**< @brief Raw Look Up table correction (HW v2 only) */
    STP_LSH, /**< @brief Lens shading */
    STP_WBC, /**< @brief White balance correction (part of LSH block in HW) */
    STP_FOS, /**< @brief Focus statistics */
    STP_DNS, /**< @brief Denoiser */
    STP_DPF, /**< @brief Defective Pixel */
    STP_ENS, /**< @brief Encoder Statistics */
    STP_LCA, /**< @brief Lateral Chromatic Aberration */
    STP_CCM, /**< @brief Color Correction */
    STP_MGM, /**< @brief Main Gammut Mapper */
    STP_GMA, /**< @brief Gamma Correction */
    STP_WBS, /**< @brief White Balance Statistics */
    STP_HIS, /**< @brief Histogram Statistics */
    STP_R2Y, /**< @brief RGB to YUV conversion */
    STP_MIE, /**< @brief Main Image Enhancer (Memory Colour part) */
    STP_VIB, /**< @brief Vibrancy (part of MIE block in HW) */
    STP_TNM, /**< @brief Tone Mapper */
    STP_FLD, /**< @brief Flicker Detection */
    STP_SHA, /**< @brief Sharpening */
    STP_ESC, /**< @brief Encoder Scaler */
    STP_DSC, /**< @brief Display Scaler */
    STP_Y2R, /**< @brief YUV to RGB conversions */
    STP_DGM, /**< @brief Display Gammut Maper */

    /**
     * @brief Output format setup (not a HW module but output pipeline setup
     * parameters)
     */
    STP_OUT,
};

/**
 * @brief Represent the policy for update of the control module
 */
enum CtrlUpdateMode {
    UPDATE_SHOT, /**< @brief Update every time a new shot is retrieved */
    UPDATE_REQUEST /**< @brief Update under request */
    // We might want to add a time based update mode
};

/**
 * @brief Identifiers for different types of control modules. There's only one
 * control module allowed to be registered per id.
 */
enum ControlID {
    CTRL_AWB, /**< @brief ID for automatic white balance control */
    CTRL_AE, /**< @brief ID for automatic exposure */
    CTRL_AF, /**< @brief ID for autofocus control */
    CTRL_TNM, /**< @brief ID for tone mapping control */
    CTRL_DNS, /**< @brief ID for denoising control */
    CTRL_LBC, /**< @brief ID for light based control */
#ifdef INFOTM_MOTION	
    CTRL_MOTION, /**< @by dirac  ID for motion detect */
#endif //INFOTM_MOTION	
#if defined(INFOTM_ENABLE_COLOR_MODE_CHANGE) || defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)    
    CTRL_CMC, /**< @brief ID for color mode change parameters */
#endif
    /** 
     * @brief ID for auxiliary control #1 (for usage of additional modules 
     * without having to modify ISPC library)
     */
    CTRL_AUX1,
    CTRL_AUX2 /**< @brief ID for auxiliary control #2 (similar to CTRL_AUX1) */
};


/**
 * @brief The root of all modules that can load and save parameters from a
 * ParameterList
 */
class ModuleBase
{
protected:
    /** @brief Name used when using the MOD_LOG macros in children classes */
    const std::string logName;

public:
    /** @brief Enum to be used with save() function */
    enum SaveType {
        SAVE_VAL, /**< @brief Save current value */
        SAVE_MIN, /**< @brief Save minimum value */
        SAVE_MAX, /**< @brief Save maximum value */
        SAVE_DEF, /**< @brief Save default value */
    };

    explicit ModuleBase(const std::string &loggingName): logName(loggingName)
    {}

    virtual ~ModuleBase() {}

    const char* getLoggingName() const;

    /**
     * @brief Load the module values from a parameter list
     * @param parameters The list of parameters to configure the module from
     * @return IMG_SUCCESS or IMG_ERROR_xxx code defined in overloaded 
     * implementation
     * @see ParameterList
     */
    virtual IMG_RESULT load(const ParameterList &parameters) = 0;

    /**
     * @brief Save the current values into a parameter list
     * @param parameters
     * @param t what to save (Value, Minimum, Maximum, Default)
     * @return IMG_SUCCESS or IMG_ERROR_xxx code defined in overloaded 
     * implementation
     * @see ParameterList
     */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const = 0;

    /**
     * @brief Simpler call to save(ParameterList, SaveType) to save values
     */
    virtual IMG_RESULT save(ParameterList &parameters) const;

    static std::string setupIDName(SetupID id);

    static std::string controlIDName(ControlID id);
};

/**
 * @brief Used for storage of Setup modules
 *
 * The implementation of new modules for the pipeline must inherit from
 * SetupModuleBase (which is template version of that class).
 *
 * The SetupModule class represent one of the modules in the pipeline and 
 * provide the means for its configuration and settings update.
 */
class SetupModule : public ModuleBase
{
protected:
    /**
     * @brief Pointer to the Pipeline instance this module belongs to - not
     * owned by the module
     */
    Pipeline *pipeline;
    /**
     * @brief Priority for the module setup (in case we have modules with
     * different priorities)
     */
    modulePriority priority;
    /**
     * @brief Update policy (every frame, on demand...)
     */
    moduleUpdate updateBasis;

    /** @brief Flag set if the module setup has been completed */
    bool setupFlag;
    /** @brief Flag to indicate that this module setup has been requested */
    bool updateRequested;

public:
    virtual SetupID getModuleID() const = 0;

    /**
     * @brief Constructor with priority level and update mode specification
     *
     * @param loggingName name used when logging macros are used
     * @param priorityLevel Level of priority for the execution of this module 
     * in case we want to implement a multi-priority approach
     * @param updateMode Every frame vs. under request update of the module
     *
     * @see modulePriority
     * @see moduleUpdate
     */
    SetupModule(const std::string &loggingName,
        modulePriority priorityLevel = NORMAL,
        moduleUpdate updateMode = REQUEST)
        : ModuleBase(loggingName), pipeline(0), priority(priorityLevel),
        updateBasis(updateMode), setupFlag(false), updateRequested(false)
    {}

    virtual ~SetupModule(){} /*pipeline not owned so not deleted*/

    /**
     * @brief Set the pipeline this module is associated with
     * @param pipeline Pointer to the object we are going to interact with
     * @see Pipeline
     * @see getPipeline
     */
    void setPipeline(Pipeline *pipeline);

    /**
     * @brief Get a pointer to the pipeline this module is associated with
     * @return Pointer to the pipeline this module is interacting with
     * @see Pipeline
     * @see setPipeline
     */
    Pipeline* getPipeline();

    /**
     * @copydoc getPipeline()
     */
    const Pipeline* getPipeline() const;

    /**
     * @brief Get the priority level to update this module
     * @return The priority level this module has assigned
     * @see modulePriority
     */
    modulePriority getPriority() const;

    /**
     * @brief Set the priority level to update this module
     * @param newPriority The priority level to be setPipeline
     * @see modulePriority
     */
    void setPriority(modulePriority newPriority);

    /**
     * @brief Query if this module has been set up or not
     * @return true if the module has been set up
     * @return false otherwise
     */
    bool isSetup() const;

    /**
     * @brief Query if this module has been requested to be update
     * @return true if this module has been requested to be updated.
     * @return false otherwise
     * @see requestUpdate()
     */
    bool isUpdateRequested() const;

    /**
     * @brief Request this module to be updated (configuration of the module
     * will be updated in the pipeline
     * @see isUpdateRequested()
     */
    void requestUpdate();

    /**
     * @brief clear the setup and update request flags
     */
    virtual void clearFlags();

    /**
     * @brief Setup the module: convert the setup parameters into register
     * values to be programmed in the pipeline
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if needed elements are not
     * initialised
     */
    virtual IMG_RESULT setup() = 0;
};

/**
 * @brief Template implementation to allow easy casting from the Pipeline list
 */
template <SetupID mid>
class SetupModuleBase : public SetupModule
{
public:
    static const SetupID id = mid;
    virtual SetupID getModuleID() const { return id; }

    SetupModuleBase(const std::string &loggingName,
        modulePriority priorityLevel = NORMAL,
        moduleUpdate updateMode = REQUEST)
        : SetupModule(loggingName, priorityLevel, updateMode) {}

    virtual ~SetupModuleBase(){}
};

/**
 * @brief A Control Module is intended to encapsulate functionality that has
 * to be run in a regular basis.
 * For example control loop algorithms such as auto exposure.
 *
 * A control module has access to the Pipeline objects to be able to
 * read/write information from its SetupModule.
 */
class ControlModule: public ModuleBase
{
protected:
    std::list<Pipeline*> pipelineList;
    /**
     * @brief one of the pipeline of the list is responsible for providing 
     * meta-data and should be configured for that
     */
    std::list<Pipeline*>::iterator pipelineOwner;

    bool enabled;

    std::list<Pipeline*>::iterator findPipeline(const Pipeline *pipe);

    std::list<Pipeline*>::const_iterator findPipeline(
        const Pipeline *pipe) const;

    /**
     * @brief Programs the relevant Pipeline modules for the module to perform
     * its goal
     *
     * Programs the correction on all Pipeline objects in the list if
     * applicable.
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if needed modules were not found in 
     * Pipeline
     * @return IMG_ERROR_NOT_SUPPORTED if module should not be programmed
     * using that function
     */
    virtual IMG_RESULT programCorrection() = 0;

public:
    CtrlUpdateMode updateMode; /**< @brief Update modality */
#ifdef INFOTM_ISP
    Camera *pCamera;
#endif //INFOTM_ISP

public:
    /** @brief Constructor with no parameters. Setting defaults */
    explicit  ControlModule(const std::string &loggingName)
#ifdef INFOTM_ISP
        : ModuleBase(loggingName), enabled(true), updateMode(UPDATE_SHOT), pCamera(NULL) {}
#else
        : ModuleBase(loggingName), enabled(true), updateMode(UPDATE_SHOT) {}
#endif //INFOTM_ISP

    virtual ~ControlModule() {}

    const std::list<Pipeline*>& getPipelines() const;

    /**
     * @brief Removes all Pipeline from the list
     *
     * The module becomes useless until new Pipelines are added with
     * addPipeline().
     * Module also looses its owner and a new one has to be set!
     */
    void clearPipelines();

    /** @brief Linear search of a Pipeline object in the list */
    bool hasPipeline(const Pipeline *pipe) const;

    /**
     * @brief Add another object to the list of Pipeline to modify by this
     * control module
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if pipe is NULL
     * @return IMG_ERROR_ALREADY_INITIALISED if pipe is already in the list
     *
     * @see verifies if pipe is already in the list with hasPipeline()
     */
    IMG_RESULT addPipeline(Pipeline *pipe);

    /**
     * @brief Set the new owner of the module (the one that provides meta-data)
     *
     * @param[in] pipe object already in the list of pipelines. If 0 is
     * given module looses its owner
     *
     */
    IMG_RESULT setPipelineOwner(Pipeline *pipe);

    /** @brief Access to the Pipeline owner of this Module */
    Pipeline* getPipelineOwner() const;

    /** @brief Access to the Sensor of the Pipeline ownning this Module */
    Sensor* getSensor() const;

    /**
     * @brief Remove a Pipeline from the list of objects to modify.
     *
     * @warning Cannot remove the Pipeline that owns the control module.
     *
     * @param[in] pipe
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_NOT_INITIALISED if given Pipeline was not found in
     * the list
     * @return IMG_ERROR_NOT_SUPPORTED if given Pipeline is the owner
     */
    IMG_RESULT removePipeline(Pipeline *pipe);

    /**
     * @brief Change enable status
     *
     * A disabled ControlModule will not be updated when new frames are
     * received
     */
    virtual void enableControl(bool enable = true);

    /** @brief To know if a module is enabled */
    virtual bool isEnabled() const;

    /**
     * @brief virtual function aimed to allow the control module to be updated 
     * in a regular basis and have access to the latest metadata.
     *
     * The update function is the function from the control module that it's 
     * going to be called regularly and it is the entry point to receive 
     * metadata from previously captured shot.
     * The update module will be called after each shot captured if the
     * control module update policy is set to UPDATE_SHOT.
     *
     * @note Caller should check if the module is enabled before calling this
     * function
     * 
     * @param metadata Metadata from a previously captured shot
     *
     * @return IMG_SUCCESS or IMG_ERROR_xxx code defined by implementation
     */
#ifdef INFOTM_ISP
    virtual IMG_RESULT update(const Metadata &metadata, const Metadata &metadata2) = 0;
#else
    virtual IMG_RESULT update(const Metadata &metadata) = 0;
#endif //INFOTM_ISP
    /**
    * @brief Configures the needed statistics for the module to perform its
    * update() function properly
    *
    * Should configure the statistics of the Pipeline object related to the
    * Camera that owns the control module (as only that one will provide
    * statistics).
    *
    * @return IMG_SUCCESS
    * @return IMG_ERROR_UNEXPECTED_STATE if needed modules were not found in
    * Pipeline
    * @return IMG_ERROR_NOT_SUPPORTED if module should not be programmed using
    * that function
    * @return IMG_ERROR_NOT_INITIALISED if module does not have owner to
    * configure or is badly configured (e.g. needs a sensor pointer)
    */
    virtual IMG_RESULT configureStatistics() = 0;

    /**
     * @brief Set the update policy for the control module (to be updated
     * after each capture or not).
     *
     * @param updateMode The update mode
     *
     * @return IMG_SUCCESS
     */
    virtual IMG_RESULT setUpdateType(CtrlUpdateMode updateMode);

    /**
     * @brief Returns the update policy being employed in this control module
     * @return The update policy being applied
     */
    virtual CtrlUpdateMode getUpdateType() const;

    /**
     * @brief Initialize module during registration.
     *
     * Can be used for code that cannot be run in constructor (like 
     * configureStatistic() call).
     * Most likely needs the Pipeline owner and sensor to be set
     */
    virtual void init(void) {}

    /** @brief generic module ID getter */
    virtual ControlID getModuleID() const = 0;
};

/**
 * @brief Template implementation of ControlModule to allow easy casting from 
 * the Control list
 */
template <ControlID mid>
class ControlModuleBase : public ControlModule
{
public:
    static const ControlID id = mid;
    virtual ControlID getModuleID() const { return id; }

    explicit ControlModuleBase(const std::string &loggingName)
        : ControlModule(loggingName) {}

    virtual ~ControlModuleBase() {}
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_H_ */
