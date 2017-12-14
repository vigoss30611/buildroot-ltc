/**
*******************************************************************************
 @file ControlAF.h

 @brief Control module for autofocus. When triggered controls the sensor focus
 distance until an 'in-focus' position has been reached

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
#ifndef ISPC_CONTROL_AF_H_
#define ISPC_CONTROL_AF_H_

#include <string>

#include "ispc/Module.h"
#include "ispc/ISPCDefs.h"
#include "ispc/Parameter.h"

namespace ISPC {

class Sensor;  // defined in ispc/sensor.h

/**
 * @ingroup ISPC2_CONTROLMODULES
 * @brief ControlAF class implements the Autofocus control.
 *
 * Uses the ModuleFOS statistics to modify the Sensor focus.
 */
class ControlAF: public ControlModuleBase<CTRL_AF>
{
public:
    /** @brief State of the AF process */
    typedef enum State {
        AF_IDLE,  /**< @brief Idle, doing nothing */
        AF_SCANNING,  /**< @brief Scanning in-focus position */
        AF_FOCUSED,  /**< @brief In-focus position reached */
        /** @brief Out of focus: no reliable in-focus position found */
        AF_OUT,
    } State;

    /** @brief State within the in-focus position scanning process */
    typedef enum ScanState {
        AF_SCAN_STOP,  /**< @brief Stopped focus process */
        AF_SCAN_INIT,  /**< @brief Initialization of the focus process */
        AF_SCAN_ROUGH,  /**< @brief Rough sweep along the focus range */
        AF_SCAN_FINE,  /**< @brief Finer sweep along a reduced focus range */
        AF_SCAN_REFINE,
        /** @brief Positioning in the best focus position */
        AF_SCAN_POSITIONING,
        AF_SCAN_FINISHED,
    } ScanState;

    /** @brief Commands for the AF control module */
    typedef enum Command {
        AF_TRIGGER,  /**< @brief Trigger a focus search */
        AF_STOP,  /**< @brief Stop a focus search */
        AF_FOCUS_CLOSER,  /**< @brief Set the focus distance closer */
        AF_FOCUS_FURTHER,  /**< @brief Set the focus position further away */
        AF_NONE, /**< @brief No command */
    } Command;

    static const char* StateName(State e);
    static const char* ScanStateName(ScanState e);
    static const char* CommandName(Command e);

protected:  // attributes
    bool flagInitialized;

    /** @brief Best sharpness measure found */
    double bestSharpness;
    /** @brief Best focus distance found */
    unsigned int bestFocusDistance;

    /** @brief Minimum focus distance (lens dependant) */
    unsigned int minFocusDistance;
    /** @brief Maximum focus position (lens dependant) */
    unsigned int maxFocusDistance;
    /** @brief Focus position we are heading towards */
    unsigned int targetFocusDistance;
    /** @brief Last sharpness measure obtained */
    double lastSharpness;
    /** @brief Current and next state of the AF algorithm */
    State currentState, nextState;
    /** @brief State withn the focus position scan process */
    ScanState scanState;
    /** @brief Last programmed command */
    Command lastCommand;
    /** @brief Init positions for the scan */
    unsigned int scanInit;
    /** @brief End positions for the scan */
    unsigned int scanEnd;

    double centerWeigth;

    /**
     * @brief Grid weights with higher relevance towards the centre but
     * more homogeneously distributed
     */
    static const double WEIGHT_7X7_SPREAD[7][7];
    /** @brief Grid weights with much more focused in the centra values */
    static const double WEIGHT_7X7_CENTRAL[7][7];

public:  // methods
    /** @brief Constructor, setting up defaults */
    explicit ControlAF(const std::string &logName = "ISPC_CTRL_AF");

    virtual ~ControlAF() {}

    /** @copydoc ControlModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ControlModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ControlModule::update() */
#ifdef INFOTM_ISP
    virtual IMG_RESULT update(const Metadata &metadata, const Metadata &metadata2);
#else
    virtual IMG_RESULT update(const Metadata &metadata);
#endif //INFOTM_DUAL_SENSOR

    /**
     * @brief Sends a command to the AF control module to drive the
     * focus process
     *
     * @param command Command to be send to the AF control
     */
    void setCommand(Command command);

    /**
     * @brief Get the current state of the internal state machine
     * @return Current state of the AF state machine
     */
    State getState() const;

    ScanState getScanState() const;

    unsigned int getBestFocusDistance() const;

    double getCenterWeight() const;

protected:
    /**
     * @copydoc ControlModule::configureStatistics()
     *
     * Configure FOS statistics
     * @return IMG_ERROR_NOT_INITIALISED if sensor is NULL
     */
    virtual IMG_RESULT configureStatistics();

    /**
     * @copydoc ControlModule::programCorrection()
     *
     * Does nothing - use runAF()
     */
    virtual IMG_RESULT programCorrection();

    /**
     * @brief Run the Autofocus state machine, which keeps track of
     * the current state, focus direction, best focus position, etc.
     *
     * @param lastFocusDistance Last metered focus distance
     * @param lastSharpness Sharpness value in the last focus position
     * @param command
     */
    virtual void runAF(unsigned int lastFocusDistance, double lastSharpness,
        Command command);

public:
    /**
     * @name Auxiliary functions
     * @brief Contains the algorithms knowledge
     * @{
     */

    /**
     * @brief Get a sharpness measure from the previously captured shot
     * metadate given a more or less weight to the central grid cells
     *
     * @param shotMetadata Previously captured shot metadada
     * @param centreWeight 0.0 to 1.0 value to control how much weight the
     * central regions of the image have in the sharpness measure
     *
     * @return Measured sharpness in the image
     */
    static double sharpnessGridMetering(const Metadata &shotMetadata,
        double centreWeight);

    /**
     * @}
     */

public:  // parameters
    static const ParamDef<double> AF_CENTER_WEIGTH;

    /** @brief Get the group of parameters used by that class */
    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_CONTROL_AF_H_ */
