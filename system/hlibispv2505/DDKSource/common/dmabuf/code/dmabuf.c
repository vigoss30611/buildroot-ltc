/**
******************************************************************************
 @file dmabuf.c

 @brief Implementation of userspace dmabuf allocator/exporter

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

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/types.h>

#include <errno.h>
#include <string.h>

#include "img_systypes.h"
#include "img_errors.h"
#include "felixcommon/userlog.h"
#include "dmabuf/dmabuf.h"
#include "dmabuf_exporter.h"
#define LOG_TAG "DMABUF"

/*!
******************************************************************************
S_DMABUFAllocator:  DMABUF dev_handle_t
 Holds the DMA device file descriptor.
******************************************************************************/
typedef struct
{
	int bufFd;
	int ui32DmadevFd;
}S_DMABUFHandle;

#if(0)
/**
 * FIXME : those ioctls shall be extracted to separate header!
 */
#define DMABUF_IOCTL_BASE	'D'
#define DMABUF_IOCTL_CREATE	_IOR(DMABUF_IOCTL_BASE, 0, unsigned long)
#define DMABUF_IOCTL_EXPORT	_IOR(DMABUF_IOCTL_BASE, 1, unsigned long)

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#endif

/*!
******************************************************************************
strDmabufDevice:
DMABUF exporter device name
******************************************************************************/
static const char strDmabufDevice[] = "/dev/dmabuf";

/*!
******************************************************************************
dmabuf_Create:
Perform a IOCTL to the DMABUF exporter device for dmabuf creation.
******************************************************************************/
static int dmabuf_Create(int DmadevFd, unsigned long size,
		unsigned long flags)
{
	return ioctl(DmadevFd, DMABUF_IOCTL_CREATE, size);
}

/*!
******************************************************************************
DMABUF_DMABUFDelete:
Perform a IOCTL to the DMABUF exporter device for dmabuf deletion.
******************************************************************************/
/*static IMG_INT32 dmabuf_DMABUFDelete(int ui32DmadevFd)
{
	return ioctl(ui32DmadevFd, DMABUF_IOCTL_DELETE, 0);
}*/

/*!
******************************************************************************
dmabuf_Export:
Perform a IOCTL to the DMABUF exporter device for dmabuf export.
******************************************************************************/
static int dmabuf_Export(int DmadevFd, unsigned long flags)
{
	return ioctl(DmadevFd, DMABUF_IOCTL_EXPORT, flags);
}

static IMG_RESULT dmabuf_Open (IMG_HANDLE* const pHandle)
{
	int devFd;
	S_DMABUFHandle *psDMABUFAllocator;

	if(!pHandle) {
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	devFd = open(strDmabufDevice, O_RDWR);
	if (devFd < 0) {
		LOG_ERROR("Could not open DMABUF Device\n");
		return IMG_ERROR_FATAL;
	}
	psDMABUFAllocator = (S_DMABUFHandle *)malloc(sizeof(S_DMABUFHandle));
	if(psDMABUFAllocator == NULL)
	{
		LOG_ERROR("Malloc failure while allocating BufferAllocator struct\n");
		close(devFd);
		return IMG_ERROR_MALLOC_FAILED;
	}

	psDMABUFAllocator->ui32DmadevFd = devFd;
	psDMABUFAllocator->bufFd = -1;
	*pHandle = (IMG_HANDLE) psDMABUFAllocator;

	return IMG_SUCCESS;
}

static IMG_RESULT dmabuf_Close (IMG_HANDLE const handle)
{
	S_DMABUFHandle * psDMABUFAllocator;

	if(!handle) {
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	psDMABUFAllocator = (S_DMABUFHandle *)handle;
	if(psDMABUFAllocator->ui32DmadevFd < 0) {
		return IMG_ERROR_NOT_INITIALISED;
	}

	close(psDMABUFAllocator->bufFd);
	close(psDMABUFAllocator->ui32DmadevFd);
	free(psDMABUFAllocator);

	return IMG_SUCCESS;
}

IMG_RESULT DMABUF_Alloc (IMG_HANDLE* pHandle, IMG_SIZE bytes)
{
	S_DMABUFHandle *psDMABUFAllocator;
	IMG_RESULT eError = IMG_ERROR_FATAL;

	if(!pHandle || *pHandle || bytes<=0 ) {
		// HANDLE must be NULL
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	eError = dmabuf_Open(pHandle);
	if(eError != IMG_SUCCESS) {
		return eError;
	}

	psDMABUFAllocator = (S_DMABUFHandle *)*pHandle;
	if(psDMABUFAllocator->ui32DmadevFd <=0)
	{
		LOG_ERROR("Not initialized.\n");
		return IMG_ERROR_NOT_INITIALISED;
	}

	eError = dmabuf_Create(psDMABUFAllocator->ui32DmadevFd,
			bytes, O_RDWR);
	if (eError != IMG_SUCCESS) {
		LOG_ERROR("Could not create a DMABUF\n");
		return IMG_ERROR_FATAL;
	}

	return IMG_SUCCESS;
}

IMG_RESULT DMABUF_Free (IMG_HANDLE handle)
{
	S_DMABUFHandle *psDMABUFAllocator;
	IMG_RESULT eError = IMG_ERROR_FATAL;

	if(!handle) {
		return IMG_ERROR_INVALID_PARAMETERS;
	}
	psDMABUFAllocator = (S_DMABUFHandle*)handle;
	if(psDMABUFAllocator->ui32DmadevFd<0) {
		return IMG_ERROR_NOT_INITIALISED;
	}

    return dmabuf_Close(handle);
}

IMG_RESULT DMABUF_GetBufferFd(IMG_HANDLE handle, int* fd)
{
	S_DMABUFHandle *psDMABUFAllocator;
	if(!handle || !fd) {
		return IMG_ERROR_INVALID_PARAMETERS;
	}
	psDMABUFAllocator = (S_DMABUFHandle*)handle;
	if(psDMABUFAllocator->ui32DmadevFd<0) {
		return IMG_ERROR_NOT_INITIALISED;
	}
	if(psDMABUFAllocator->bufFd < 0) {
		int bufFd = dmabuf_Export(psDMABUFAllocator->ui32DmadevFd, 0);
		if (bufFd < 0) {
			LOG_ERROR("Could not export a DMABUF\n");
			return IMG_ERROR_FATAL;
		}
		psDMABUFAllocator->bufFd = bufFd;
	}
	LOG_DEBUG("Obtained DMA-BUF handle: %d\n", psDMABUFAllocator->bufFd);

	*fd = psDMABUFAllocator->bufFd;

	return IMG_SUCCESS;
}
