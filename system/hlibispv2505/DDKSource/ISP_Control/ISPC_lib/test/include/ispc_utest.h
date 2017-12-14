/**
*******************************************************************************
 @file ispc_utest.h

 @brief Base class to help unit tests of ISPC modules

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
#ifndef ISPC_UTEST_H_
#define ISPC_UTEST_H_

#include <img_types.h>
#include <felixcommon/lshgrid.h>
#include <list>
#include "felix.h"

namespace ISPC {
class DGCamera;  // include "ispc/Camera.h"
class DGSensor;
}

struct ISPCUTest
{
    Felix driver;
    ISPC::DGCamera *pCamera;
    ISPC::DGSensor *pSensor;  // do not delete - owned by pCamera
    std::list<IMG_UINT32> allocBufferId;

    ISPCUTest();
    virtual ~ISPCUTest();

    virtual void addControlModules();
    
    virtual IMG_RESULT configure(int ctx = 0, int gasket = 0);

    virtual void finalise();

    IMG_RESULT startCapture(int n = 2, bool enqueue = false);
    IMG_RESULT stopCapture();

    IMG_RESULT newLSHGrid(LSH_GRID &grid, int tile,
        const float *corners = NULL);

    static std::list<IMG_UINT32> getLSHTileSizes();
};

#endif /* ISPC_UTEST_H_ */