/**
******************************************************************************
@file perf_controls.cpp

@brief Performance of convergence in Controls. Implementation.

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
#include <string.h>
#include "ispctest/perf_controls.h"

#define LOG_TAG "PERF_controls"

#include "felixcommon/userlog.h"

ConvergenceSpeed::ConvergenceSpeed(ISPC::ControlModule const * const module):
            mState(PERF_IDLE)
            ,mFrameCount(0)
{
    mModule = module;
}

ConvergenceSpeed::~ConvergenceSpeed() {

}

void ConvergenceSpeed::reset()    {
    mState = PERF_IDLE;
    mFrameCount = 0;
}

void ConvergenceSpeed::proceed()  {   mState = PERF_COUNTING_FRAMES; /* LOG_PERF("%s proceed() ", name.c_str()); */ }
void ConvergenceSpeed::finish()   {   mState = PERF_COMPLETED;   /*LOG_PERF("%s finish() ", name.c_str()); */ }
void ConvergenceSpeed::count()    {   mFrameCount++; /*if(!(frameCount & 0xF)) LOG_PERF("%s frameCount = %d ", name.c_str(), frameCount);*/ }

void ConvergenceSpeed::logOk()        {   LOG_PERF("%s converged in %d frames\n", mModule->getLoggingName(), mFrameCount); }
void ConvergenceSpeed::logRestart()   {   LOG_PERF("%s converged in %d frames, immediate restart\n", mModule->getLoggingName(), mFrameCount); }

void ConvergenceSpeed::enable()   {   reset();     }
void ConvergenceSpeed::disable()  {   reset();     }

void ConvergenceSpeed::update() {
    update(mModule->hasConverged());
}

void ConvergenceSpeed::update(bool converged) {

    switch(mState)
    {
    case PERF_IDLE:
        if(!converged) {    proceed();  }
        break;
    case PERF_COUNTING_FRAMES:
        count();
        if(converged)   {   finish();   }
        break;
    case PERF_COMPLETED:
        if(converged)   {   logOk(); reset();  }
            else        {   logRestart(); reset();  proceed(); }
        break;
    default:
        LOG_PERF("PERF: default");
        break;
    }
}

//********************************************************************
PerfControls::PerfControls() {
}

PerfControls::~PerfControls() {
    perfConv.clear();
}

void PerfControls::registerControlForConvergenceMonitoring(ISPC::ControlModule const * const module) {
    ConvergenceSpeed cSpeed(module);
    perfConv.push_back(cSpeed);
}

void PerfControls::updateConv() {
    // traverse the list
    for(std::list<ConvergenceSpeed>::iterator iterator=perfConv.begin(),
            end = perfConv.end(); iterator != end; ++iterator) {
        iterator->update();
    }
}
