/**
******************************************************************************
 @file felix.h

 @brief Unit-test helper classes

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
#include <felixcommon/pixel_format.h>
#include "ci/ci_api.h"
#include "ci_kernel/ci_kernel.h"

#ifdef FELIX_HAS_DG
#include "dg_kernel/dg_camera.h"
#endif

#include <gtest/gtest.h>

#ifndef _FELIX_TEST_CLASS_
#define _FELIX_TEST_CLASS_

enum FelixRegInit {
	NONE = 0,
	DEFAULT_VAL,
};

// used for some tests to access registers directly
struct TAL_HANDLES
{
	struct KRN_CI_DRIVER_HANDLE driver;
	IMG_HANDLE hRegMMU;

	struct KRN_DG_DRIVER_HANDLE
	{
		IMG_HANDLE hRegFelixDG[CI_N_EXT_DATAGEN];	/**< @brief Register handle for the several data generators */
		IMG_HANDLE hMemHandle;
		IMG_HANDLE hRegMMU;
	};
	struct KRN_DG_DRIVER_HANDLE datagen;

	TAL_HANDLES();
};

class Felix
{
 protected:
	KRN_CI_DRIVER sCIDriver;
    SYS_DEVICE sDevice;
  	KRN_CI_CONNECTION *pKrnConnection;
	CI_CONNECTION *pUserConnection;

 public:
	Felix();
	virtual ~Felix();

	virtual IMG_RESULT configure(enum FelixRegInit regInit = DEFAULT_VAL, IMG_BOOL8 bMMUEnabled = IMG_FALSE);
	virtual void clean();

	KRN_CI_DRIVER* getKrnDriver();
	CI_CONNECTION* getConnection();
	KRN_CI_CONNECTION* getKrnConnection();

	static IMG_RESULT fakeInterrupt(KRN_CI_PIPELINE* pPipeline);
};

#define IMG_CI_CAPTURE_TEST_BUFFERS 10
class FullFelix: public Felix
{
public:
	CI_PIPELINE *pPipeline;

	FullFelix();
	virtual ~FullFelix();

	virtual void configure(int width=32, int height=32, ePxlFormat yuvOut=YVU_420_PL12_8, ePxlFormat rgbOut=PXL_NONE, bool genCRC=false, int nBuffers=5, IMG_BOOL8 bMMU = IMG_FALSE);
	virtual void clean();

	KRN_CI_PIPELINE* getKrnPipeline(CI_PIPELINE *pSearchPipeline = NULL);

    static void writeGasketCount(IMG_UINT32 ui32Gasket, IMG_UINT32 gasketCount);

    // difference with Felix::fakeInterrupt is that interrupt handler is called
    static IMG_RESULT fakeInterrupt(IMG_UINT32 ctx);
};

#ifdef FELIX_HAS_DG

class Datagen: public Felix
{
protected:
	KRN_DG_DRIVER sDGDriver;

public:
	virtual ~Datagen();

	IMG_RESULT configure(enum FelixRegInit regInit = DEFAULT_VAL, IMG_UINT8 uiMMU = IMG_FALSE);
};

#endif

// from simulator
#define CORE_GROUP_RESET FELIX_GROUP_ID
#define CORE_ALLOCATION_RESET FELIX_ALLOCATION_ID
#define CORE_CONFIG1_RESET 0x0
#define CORE_CONFIG2_RESET 0x0
#define CORE_CONFIG3_RESET 0x0
#define CORE_CONFIG4_RESET 0x000C0000
#define CONTEXT_MAX_WIDTH 8192 // divide by 2 for ctx1, 4 ctx2 etc
#define CONTEXT_MAX_QUEUE 16
#define CORE_BIT_DEPTH 10
#define CORE_PARALLELISM 1
#define CORE_NIMAGER 2
//#define CORE_NCONTEXT 2 // uses CI_N_CONTEXT
//#define DG_CONFIG_N_DATAGEN 2 // uses CI_N_EXT_DATAGEN
#define CORE_N_DIFF_GASKETS 2
#define CORE_GASKETTYPE_0 (CI_GASKET_MIPI)
#define CORE_GASKETTYPE_1 (CI_GASKET_PARALLEL)
#define CORE_GASKETBITDEPTH 12

#define MMU_ENABLED 1
#define MMU_BITDEPTH (40-32)

#endif //_FELIX_TEST_CLASS_
