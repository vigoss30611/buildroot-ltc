/**
*******************************************************************************
 @file Control.h

 @brief Declaration of ISPC::Control

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
#ifndef ISPC_CONTROL_H_
#define ISPC_CONTROL_H_

#include <img_types.h>
#include <map>

#include "ispc/Module.h"
#include "ispc/ParameterList.h"

namespace ISPC {

/**
 * @brief Container for Control algorithms
 */
class Control
{
private:
    /** @brief Registry for the control modules */
    std::map<ControlID, ControlModule *> controlRegistry;

public:
    /**
     * @brief Clears all the registered control modules. The registered
     * modules are deleted (shouldn't be deleted outside this class once
     * registered)
     */
    virtual ~Control();

    /**
     * @brief Register a control module in the list. It's not possible
     * to register two modules with the same ID.
     *
     * All the registered module's update function will be invoked when
     * required.
     *
     * @note The deletion of the registered modules will be in charge of
     * the control module (so registered modules shouldn't be deleted
     * outside this class once they have been registered).
     *
     * @param module Control module to be registered
     * @param pipeline Pointer to the Pipeline object the control module is
     * going to interact with
     * @note Modules will used the pipeline's Sensor pointer to access the
     * sensor object
     *
     * @return IMG_SUCCESS
     */
    IMG_RESULT registerControlModule(ControlModule *module,
        Pipeline *pipeline);

    /**
     * @brief Get the registered control module with the requested id.
     * There is only one possible registered control module with a given ID.
     *
     * @param id Requestede control module ID.
     *
     * @return Pointer to the control module with the requested ID.
     * @Return NULL if there is not control module with such ID registered.
     */
    ControlModule *getControlModule(ControlID id);

    /**
     * @brief Templated getter for control modules. Does not need static
     * casting
     *
     * usage example: ControlAE* mod = getControlModule<ControlAE>();
     */
    template <class T>
    T* getControlModule(void)
    {
        return static_cast<T*>(getControlModule(T::id));
    }

    /**
     * @brief Run the control module with the given ID (even if module
     * is disabled)
     *
     * @param id ID of the control module to run
     * @param metadata Metadata from a previously captured shot. Update
     * functions of the control modules require this information.
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if no module was registered
     * with this ID
     * @return Delegates the to called module update function
     *
     * See @ref ControlModule::update()
     */
#ifdef INFOTM_ISP
    IMG_RESULT runControlModule(ControlID id, const Metadata &metadata, const Metadata &metadata2);
#else
    IMG_RESULT runControlModule(ControlID id, const Metadata &metadata);
#endif //INFOTM_ISP

    /**
     * @brief Run all the control modules registered (only the enabled ones!)
     *
     * @warning Runs only the ENABLED modules
     *
     * @param metadata Metadata from a previously captured shot. Update
     * functions of the control modules require this information.
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_FATAL if some of the modules failed in their
     * update function
     *
     * See @ref ControlModule::update()
     */
#ifdef INFOTM_ISP
    IMG_RESULT runControlModules(const Metadata &metadata, const Metadata &metadata2);
#else
    IMG_RESULT runControlModules(const Metadata &metadata);
#endif //INFOTM_DUAL_SENSOR
    /**
     * @brief Run all the control modules registered which update mode is
     * the one specified by parameters (only if enabled)
     *
     * @warning Runs only the ENABLED modules
     *
     * @param updateMode The update mode that the control modules have to
     * be set to in order to be executed by this call.
     * @param metadata Metadata from a previously captured shot. Update
     * functions of the control modules require this information.
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_FATAL if some of the module failed in their
     * update function
     *
     * See @ref ControlModule::update()
     */
#ifdef INFOTM_ISP
    IMG_RESULT runControlModules(CtrlUpdateMode updateMode, const Metadata &metadata, const Metadata &metadata2);
#else
    IMG_RESULT runControlModules(CtrlUpdateMode updateMode, const Metadata &metadata);
#endif //INFOTM_ISP
    /**
     * @brief Load all the control module parameters
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_FATAL if some of the module failed in their
     * load function
     *
     * See @ref ControlModule::load()
     */
    IMG_RESULT loadAll(const ParameterList &parameters);

    /**
     * @brief Load a specific control module parameters
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if no module was registered
     * with this ID
     * @return Delegates to the module load function
     *
     * See @ref ControlModule::load()
     */
    IMG_RESULT loadControlModule(ControlID id,
        const ParameterList &parameters);

    /**
     * @brief Program the statistics for all modules
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_FATAL if one of the module fails (but all of them are
     * configured before stopping).
     */
    IMG_RESULT configureStatistics();

    /**
     * @brief Save all Controls to a parameter list
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_FATAL if some of the module failed in their
     * save function
     *
     * See @ref ControlModule::save()
     */
    IMG_RESULT saveAll(ParameterList &parameters,
        ModuleBase::SaveType t = ModuleBase::SAVE_VAL) const;

    /**
     * @brief Save selected control module
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if no module was registered with
     * this ID
     * @return Delegates to the module save function
     *
     * See @ref ControlModule::save()
     */
    IMG_RESULT saveControlModule(ControlID id, ParameterList &parameters,
        ModuleBase::SaveType t = ModuleBase::SAVE_VAL) const;

    /**
     * @brief Removes and deletes all the control modules registered in
     * the class. (Each module is actually destroyed so it must not be
     * destroyed outside this function).
     *
     * @return IMG_SUCCESS
     */
    IMG_RESULT clearModules();

    /**
     * @brief Add the Pipeline object to all the registered modules
     *
     * May not want to use that function if the control module list contains
     * modules that expect to be run alone (e.g. Auto Exposure).
     *
     * Will try to add the Pipeline to all modules regardless of previous
     * errors
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_FATAL if one or more module failed to add the
     * Pipeline
     */
    IMG_RESULT addPipelineToAll(Pipeline *pipeline);

    /**
     * @brief Set the given Pipeline as the owner of all the registered
     * modules
     *
     * @warning the Pipeline needs to be in the list of ALL modules to
     * become the owner.
     * Consider using @ref addPipelineToAll() before calling that function.
     *
     * @note Owner is set to all modules when registering them
     *
     * Will try to set the Pipeline to all modules regardless of previous
     * errors
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_FATAL if one or more module failed to set the
     * Pipeline
     */
    IMG_RESULT setPipelineOwnerToAll(Pipeline *pipeline);
};

} /* namespace ISPC */

#endif /* ISPC_CONTROL_H_ */
