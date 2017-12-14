/**
**************************************************************************
@file           img_structs.h

@brief          Defines the structures and enums other than the pixel formats

@copyright Imagination Technologies Ltd. All Rights Reserved.

@license        <Strictly Confidential.>
   No part of this software, either material or conceptual may be copied or
   distributed, transmitted, transcribed, stored in a retrieval system or
   translated into any human or computer language in any form by any means,
   electronic, mechanical, manual or other-wise, or disclosed to third
   parties without the express written permission of
   Imagination Technologies Limited,
   Unit 8, HomePark Industrial Estate,
   King's Langley, Hertfordshire,
   WD4 8LZ, U.K.
**************************************************************************/

#ifndef __IMG_STRUCTS__
#define __IMG_STRUCTS__

#include "img_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This type defines the buffer type
 */
typedef enum
{
    IMG_BUFFERTYPE_FRAME = 0,
    IMG_BUFFERTYPE_FIELD_TOP,
    IMG_BUFFERTYPE_FIELD_BOTTOM,
    IMG_BUFFERTYPE_PAIR
} IMG_eBufferType;

/**
 * @brief This type defines the image orientation.
 * @warning Rotation is in the clockwise direction
 */
typedef enum
{
    IMG_ROTATE_NEVER,
    IMG_ROTATE_0,
    IMG_ROTATE_90,
    IMG_ROTATE_180,
    IMG_ROTATE_270

} IMG_eOrientation;

#ifdef __cplusplus
}
#endif

#endif // __IMG_STRUCTS__
