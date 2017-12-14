/**
******************************************************************************
 @file unit_tests.h

 @brief Unit tests helper structures and globals

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
#ifndef __UNIT_TESTS__
#define __UNIT_TESTS__

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#ifdef __cplusplus

#include <ostream>

// default videosource path (when runtime arg is not defined)
#ifdef WIN32
#define SAMBA_VIDEOSOURCE "//klsamba02/videosource"
#else
#define SAMBA_VIDEOSOURCE "/mnt/videosource"
#endif

extern char *samba_videosource; // path to the videosource - configurable at runtime with --videosource <path>

typedef struct Test_IMG_RESULT
{
	IMG_RESULT ret;

	Test_IMG_RESULT(){ ret = IMG_SUCCESS; }
	Test_IMG_RESULT(IMG_RESULT r): ret(r) {}


}Test_IMG_RESULT;
std::ostream& operator<< (std::ostream& os, const Test_IMG_RESULT& e);

extern "C" {
#endif // __cplusplus

extern void (*gpfnInitTal)(void); // this is used to setup the TAL default values when using the NULL interface
extern IMG_BOOL8 gbUseTalNULL; // this is used to setup the driver's tal NULL usage

#ifdef __cplusplus
}
#endif

#endif // __UNIT_TESTS__
