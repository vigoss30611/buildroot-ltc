/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2006 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Abstract  :
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of contents

    1. Include headers
    2. Module defines
    3. Data types
    4. Function prototypes

------------------------------------------------------------------------------*/
#ifndef __JPEG_ENC__H__
#define __JPEG_ENC__H__

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "common.h"
#include "JpegEncApi.h"
/*------------------------------------------------------------------------------
    2. Module defines
------------------------------------------------------------------------------*/

#define MAX_NUMBER_OF_COMPONENTS 3

/*------------------------------------------------------------------------------
    3. Data types
------------------------------------------------------------------------------*/

enum
{
    ENC_420_MODE,
    ENC_422_MODE
};

typedef enum
{
    ENC_NO_UNITS = 0,
    ENC_DOTS_PER_INCH = 1,
    ENC_DOTS_PER_CM = 2
} EncAppUnitsType;

enum
{
    ENC_SINGLE_MARKER,
    ENC_MULTI_MARKER
};

typedef struct JpegEncQuantTables_t
{
    const u8 *pQlumi;
    const u8 *pQchromi;

} JpegEncQuantTables;

typedef struct JpegEncFrameHeader_t /* SOF0 */
{
    u32 header;
    u32 Lf;
    u32 P;
    u32 Y;
    u32 X;
    u32 Nf;
    u32 Ci[MAX_NUMBER_OF_COMPONENTS];
    u32 Hi[MAX_NUMBER_OF_COMPONENTS];
    u32 Vi[MAX_NUMBER_OF_COMPONENTS];
    u32 Tqi[MAX_NUMBER_OF_COMPONENTS];

} JpegEncFrameHeader;

typedef struct JpegEncCommentHeader_t   /* COM */
{
    u32 comEnable;
    u32 Lc;
    u32 comLen;
    const u8 *pComment;

} JpegEncCommentHeader;

typedef struct JpegEncRestart_t /* DRI */
{
    u32 Lr;
    u32 Ri;

} JpegEncRestart;

typedef struct   /* APP0 */
{
    u32 Lp;
    u32 ident1;
    u32 ident2;
    u32 ident3;
    u32 version;
    u32 units;
    u32 Xdensity;
    u32 Ydensity;
    u32 thumbEnable;
    u32 App1;
    char *XmpIdentifier;
    char *XmpData;
} JpegEncAppn_t;

typedef struct
{
    JpegEncRestart restart; /* Restart Interval             */
    JpegEncFrameHeader frame;   /* Frame Header Data            */
    JpegEncCommentHeader com;   /* COM Header Data              */
    JpegEncAppn_t appn; /* APPn Header Data             */
    JpegEncQuantTables qTable;
    i32 markerType;
    i32 codingMode; /* 420 or 422 */
    i32 width;
    i32 height;
    u32 stride;
    JpegEncFrameType frameType;
    u8 qTableLuma[64];
    u8 qTableChroma[64];
    u32 qtLevel;
    u8 *streamStartAddress; /* output start address */
    JpegEncThumb thumbnail; /* thumbnail data */
} jpegData_s;


/*------------------------------------------------------------------------------
    4. Function prototypes
------------------------------------------------------------------------------*/

void EncJpegInit(jpegData_s * jpeg);

u32 EncJpegHdr(stream_s * stream, jpegData_s * data);

#endif
