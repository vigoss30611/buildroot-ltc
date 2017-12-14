/**
******************************************************************************
 @file dmabuf.h

 @brief Userspace dma-buf allocator/exported. Uses dmabuf-exporter.ko

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

#ifndef _DMABUF_
#define _DMABUF_

#include <img_types.h>
#include <img_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

IMG_RESULT DMABUF_Alloc (IMG_HANDLE* pHandle, IMG_SIZE bytes);
IMG_RESULT DMABUF_Free (IMG_HANDLE handle);

IMG_RESULT DMABUF_GetBufferFd(IMG_HANDLE handle, int* fd);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //_DMABUF_
