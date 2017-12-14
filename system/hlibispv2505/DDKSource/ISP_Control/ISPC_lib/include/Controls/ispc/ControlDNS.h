/**
*******************************************************************************
 @file ControlDNS.h

 @brief Declaration of the ISPC::ControlDNS class

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
#ifndef ISPC_CONTROL_DNS_H_
#define ISPC_CONTROL_DNS_H_

#include <string>

#include "ispc/Module.h"

namespace ISPC {

/**
 * @ingroup ISPC2_CONTROLMODULES
 * @brief The ControlDNS class implements a simple dynamic control for the
 * denoiser (DNS) configuration module, updating the ISO_GAIN parameter of
 * the DNS according to the sensor capture settings.
 *
 * Uses the Sensor to get information to update ModuleDNS.
 */
class ControlDNS: public ControlModuleBase<CTRL_DNS>
{
protected:  // attributes
    /** @brief Current ISO value programmed in the denoiser */
    double fSensorGain;

public:  // methods
    /**
     * @brief Constructor with receiving the default ISO value to be
     * programmed in the DNS module. Dynamic ISO configuration is set
     * to false.
     *
     * @param defaultGain Default Gain value to be programmed in the denoiser
     *
     * @param logname Name to use when using MOD_LOG_X macros
     */
    ControlDNS(double defaultGain = 1.0,
        const std::string &logname = "ISPC_CTRL_DNS");

    virtual ~ControlDNS() {}

    /**
     * @copydoc ControlModule::load()
     *
     * Loads no parameters.
     */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /**
     * @copydoc ControlModule::save()
     *
     * Saves no parameters.
     */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /**
     * @brief Update configuration based on previously captured shot metadata
     *
     * @param[in] metadata Metadata retrieved from a previously captured shot
     *
     * @return IMG_SUCCESS
     */
    virtual IMG_RESULT update(const Metadata &metadata);

    /** @warning not using a convergence algorithm so is always true */
    virtual bool hasConverged() const;

protected:
    /**
     * @copydoc ControlModule::configureStatistics
     *
     * Does nothing, module does not use statistics
     */
    virtual IMG_RESULT configureStatistics();

    /**
     * @copydoc ControlModule::programCorrection()
     *
     * Program the denoiser configuration in the pipeline
     */
    virtual IMG_RESULT programCorrection();

public:  // parameters
    /** @brief Get the group of parameters used by that class */
    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_CONTROL_DNS_H_ */
