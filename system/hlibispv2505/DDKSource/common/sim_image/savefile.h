/**
*******************************************************************************
@file savefile.h

@brief saving sim_image to file

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
#ifndef SAVEFILE
#define SAVEFILE

#include <img_types.h>
#include <stdio.h>

#ifdef SAVE_LOCKS
    #include <pthread.h> // for locks
#endif
#include "sim_image.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SaveFile 
{
    FILE* saveTo;
    sSimImageOut *pSimImage;

    IMG_UINT32 ui32TimeWritten;
    
#ifdef SAVE_LOCKS
    /** just to insure that it is not written to and closed at the same time */
    pthread_mutex_t fileLock;
    /** to know if the lock has been initialised otherwise it may segfault
     * when accessing it (if only memset to 0) */
    IMG_BOOL8 bInitiliasedLock;
#endif

} SaveFile;

IMG_BOOL SaveFile_useSaveLocks();

IMG_RESULT SaveFile_init(SaveFile *pFile);

IMG_RESULT SaveFile_open(SaveFile *pFile, const char* filename);

// need metadata as well
IMG_RESULT SaveFile_openFLX(SaveFile *pFile, const char *filename,
    const struct SimImageInfo *info);

IMG_RESULT SaveFile_close(SaveFile *pFile);

IMG_RESULT SaveFile_destroy(SaveFile *pFile);

IMG_RESULT SaveFile_write(SaveFile *pFile, const void* ptr, size_t size);

IMG_RESULT SaveFile_writeFrame(SaveFile *pFile, const void *ptr,
    size_t stride, size_t size, IMG_UINT32 lines);

IMG_RESULT convertToPlanarBayer(IMG_UINT16 imageSize[2], const void *pInput,
    IMG_SIZE stride, IMG_UINT8 uiBayerDepth, void **pOutput,
    IMG_SIZE *pOutsize);

IMG_RESULT convertToPlanarBayerTiff(IMG_UINT16 imageSize[2],
    const void *pInput, IMG_SIZE stride, IMG_UINT8 uiBayerDepth,
    void **pOutput, IMG_SIZE *pOutsize);

#ifdef __cplusplus
}
#endif

#endif // SAVEFILE
