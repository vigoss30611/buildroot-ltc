/**
******************************************************************************
 @file perf_controls.h

 @brief Performance of convergence in Controls

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
#ifndef ISPCT_PERF_CONTROLS_H
#define ISPCT_PERF_CONTROLS_H

#include <string>
#include <felixcommon/userlog.h>

#include "ispc/Camera.h"

enum PERFState {
    PERF_IDLE = 0,
    PERF_COUNTING_FRAMES,
    PERF_COMPLETED
};

// ***************************************************************************
class ConvergenceSpeed {
public:
    ConvergenceSpeed(ISPC::ControlModule const * const module);
    ~ConvergenceSpeed();

private:
    PERFState       mState;
    const ISPC::ControlModule  *mModule;
    int             mFrameCount;

    void reset();
    void proceed();
    void finish();
    void count();

public:
    void logOk();
    void logRestart();

    void enable();
    void disable();

    void update();
    void update(bool converged);

};

// ***************************************************************************
class PerfControls {
public:
    PerfControls();
    ~PerfControls();

private:
    std::list<ConvergenceSpeed> perfConv;

public:
    void registerControlForConvergenceMonitoring(ISPC::ControlModule const * const module);

    void updateConv();
};

#endif // ISPCT_PERF_CONTROLS_H
