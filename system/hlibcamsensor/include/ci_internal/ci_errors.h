/**
*******************************************************************************
 @file ci_errors.h

 @brief Converts Errno values to IMG_RESULT and vice-versa

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
#ifndef CI_ERRORS_H_
#define CI_ERRORS_H_

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#ifndef IMG_KERNEL_MODULE
#include <errno.h>

// on some versions of windows some posix errno values are not defined
// but if we use posix threads they define some values there
#include <pthread.h>

#ifndef EADDRINUSE
#define EADDRINUSE 100
#endif
#ifndef ENOTSUP
#define ENOTSUP 101
#endif
#ifndef EALREADY
#define EALREADY 103
#endif
#ifndef ETIME
#define ETIME 137
#endif
#ifndef ECANCELED
#define ECANCELED 105
#endif
  // on linux user-side ENOTSUP is defined instead of ENOTSUPP
#define ENOTSUPP ENOTSUP
#else
  // use the kernel definitions of errno
#include <linux/errno.h>
#endif /* IMG_KERNEL_MODULE */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maps some IMG_RESULT to equivalent errno values
 * @see toImgResult
 */
IMG_INLINE static int toErrno(IMG_RESULT val)
{
    int ret = 0;
    switch (val)
    {
    case IMG_SUCCESS: ret = 0; break;
    case IMG_ERROR_MALLOC_FAILED: ret = -ENOMEM; break;
    case IMG_ERROR_NOT_SUPPORTED: ret = -ENOTSUPP; break;
    case IMG_ERROR_INVALID_PARAMETERS: ret = -EINVAL; break;
    case IMG_ERROR_INTERRUPTED: ret = -EINTR; break;
    case IMG_ERROR_MEMORY_IN_USE: ret = -EADDRINUSE; break;
    case IMG_ERROR_ALREADY_INITIALISED: ret = -EEXIST; break;
    case IMG_ERROR_MINIMUM_LIMIT_NOT_MET: ret = -E2BIG; break;
    case IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE: ret = -EALREADY; break;
    case IMG_ERROR_TIMEOUT: ret = -ETIME; break;
    case IMG_ERROR_UNEXPECTED_STATE: ret = -ECANCELED; break;
    case IMG_ERROR_FATAL:  // default is FATAL
    default: ret = -EACCES;
    }
    return ret;
}

/**
 * @brief Maps some errno values to equivalent IMG_RESULT
 * @see toErrno
 */
IMG_INLINE static IMG_RESULT toImgResult(int errnov)
{
    int ret = 0;
    switch (errnov)
    {
    case 0: ret = IMG_SUCCESS; break;
    case -ENOMEM: ret = IMG_ERROR_MALLOC_FAILED; break;
    case -ENOTSUPP: ret = IMG_ERROR_NOT_SUPPORTED; break;
    case -EINTR: ret = IMG_ERROR_INTERRUPTED; break;
    case -EINVAL: ret = IMG_ERROR_INVALID_PARAMETERS; break;
    case -EADDRINUSE: ret = IMG_ERROR_MEMORY_IN_USE; break;
    case -EEXIST: ret = IMG_ERROR_ALREADY_INITIALISED; break;
    case -E2BIG: ret = IMG_ERROR_MINIMUM_LIMIT_NOT_MET; break;
    case -EALREADY: ret = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE; break;
    case -ETIME: ret = IMG_ERROR_TIMEOUT; break;
    case -ECANCELED: ret = IMG_ERROR_UNEXPECTED_STATE; break;
    case -EACCES:  // default is fatal
    default: ret = IMG_ERROR_FATAL;
    }
    return ret;
}

#ifdef __cplusplus
}
#endif

#endif /* CI_ERRORS_H_ */
