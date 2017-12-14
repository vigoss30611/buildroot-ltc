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
--  Abstract :
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: mp4dechwd_vlc.c,v $
--  $Date: 2007/12/19 09:49:17 $
--  $Revision: 1.2 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of context

     1. Include headers
     2. External identifiers
     3. Module defines
     4. Module identifiers
     5. Fuctions

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "mp4dechwd_vlc.h"
#include "mp4dechwd_utils.h"
#include "mp4debug.h"
#ifdef ASIC_TRACE_SUPPORT
#include "trace.h"
#endif

/*------------------------------------------------------------------------------
    2. External identifiers
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/* local structures */

typedef struct
{
    u16 len;
    u8 mbType;
    u8 cbpc;
} mcbpc_t;

enum
{
    NON_VALID_MV = 0x7FFF
};
typedef struct
{
    u32 len;
    i32 val;
} MvTable_t;

typedef struct
{
    u16 len;
    u16 lrl;
} vlcTable_t;

enum
{
    ZERO_LUM_MOTION_VECTOR_VALUE = 993,
    ZERO_CHR_MOTION_VECTOR_VALUE = 497
};


/*------------------------------------------------------------------------------
    4. Module indentifiers
------------------------------------------------------------------------------*/

static const u32 TableMcbpcInter[24] = { 0x0,
    0x090500, 0x090403, 0x090402, 0x090401, 0x090103,
    0x080302, 0x080302, 0x080301, 0x080301, 0x080203,
    0x080203, 0x070303, 0x070303, 0x070303, 0x070303,
    0x070202, 0x070202, 0x070202, 0x070202, 0x070201,
    0x070201, 0x070201, 0x070201
};

/* vlc tables */
static const vlcTable_t vlcIntraTable1[112] = {
    {7, 34817}, {7, 34305}, {7, 3073}, {7, 35329}, {7, 3585}, {7, 1026},
    {7, 515}, {7, 9}, {6, 32770}, {6, 32770}, {6, 2561}, {6, 2561},
    {6, 33793}, {6, 33793}, {6, 33281}, {6, 33281}, {6, 2049}, {6, 2049},
    {6, 1537}, {6, 1537}, {6, 8}, {6, 8}, {6, 7}, {6, 7},
    {6, 514}, {6, 514}, {6, 6}, {6, 6}, {5, 1025}, {5, 1025},
    {5, 1025}, {5, 1025}, {5, 5}, {5, 5}, {5, 5}, {5, 5},
    {5, 4}, {5, 4}, {5, 4}, {5, 4}, {4, 32769}, {4, 32769},
    {4, 32769}, {4, 32769}, {4, 32769}, {4, 32769}, {4, 32769}, {4, 32769},
    {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1},
    {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1},
    {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1},
    {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1},
    {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1},
    {2, 1}, {2, 1}, {3, 2}, {3, 2}, {3, 2}, {3, 2},
    {3, 2}, {3, 2}, {3, 2}, {3, 2}, {3, 2}, {3, 2},
    {3, 2}, {3, 2}, {3, 2}, {3, 2}, {3, 2}, {3, 2},
    {4, 513}, {4, 513}, {4, 513}, {4, 513}, {4, 513}, {4, 513},
    {4, 513}, {4, 513}, {4, 3}, {4, 3}, {4, 3}, {4, 3},
    {4, 3}, {4, 3}, {4, 3}, {4, 3}
};

static const vlcTable_t vlcIntraTable2[96] = {
    {10, 18}, {10, 17}, {9, 39937}, {9, 39937}, {9, 39425}, {9, 39425},
    {9, 38913}, {9, 38913}, {9, 38401}, {9, 38401}, {9, 37889}, {9, 37889},
    {9, 33282}, {9, 33282}, {9, 32772}, {9, 32772}, {9, 6145}, {9, 6145},
    {9, 5633}, {9, 5633}, {9, 3586}, {9, 3586}, {9, 3074}, {9, 3074},
    {9, 2562}, {9, 2562}, {9, 1539}, {9, 1539}, {9, 1027}, {9, 1027},
    {9, 518}, {9, 518}, {9, 517}, {9, 517}, {9, 16}, {9, 16},
    {9, 2050}, {9, 2050}, {9, 15}, {9, 15}, {9, 14}, {9, 14},
    {9, 13}, {9, 13}, {8, 36865}, {8, 36865}, {8, 36865}, {8, 36865},
    {8, 36353}, {8, 36353}, {8, 36353}, {8, 36353}, {8, 35841}, {8, 35841},
    {8, 35841}, {8, 35841}, {8, 32771}, {8, 32771}, {8, 32771}, {8, 32771},
    {8, 5121}, {8, 5121}, {8, 5121}, {8, 5121}, {8, 4609}, {8, 4609},
    {8, 4609}, {8, 4609}, {8, 4097}, {8, 4097}, {8, 4097}, {8, 4097},
    {8, 37377}, {8, 37377}, {8, 37377}, {8, 37377}, {8, 1538}, {8, 1538},
    {8, 1538}, {8, 1538}, {8, 516}, {8, 516}, {8, 516}, {8, 516},
    {8, 12}, {8, 12}, {8, 12}, {8, 12}, {8, 11}, {8, 11},
    {8, 11}, {8, 11}, {8, 10}, {8, 10}, {8, 10}, {8, 10}
};

static const vlcTable_t vlcIntraTable3[121] = {
    {11, 32775}, {11, 32775}, {11, 32774}, {11, 32774}, {11, 22}, {11, 22},
    {11, 21}, {11, 21}, {10, 33794}, {10, 33794}, {10, 33794}, {10, 33794},
    {10, 33283}, {10, 33283}, {10, 33283}, {10, 33283}, {10, 32773}, {10,
                                                                      32773},
    {10, 32773}, {10, 32773}, {10, 6657}, {10, 6657}, {10, 6657}, {10, 6657},
    {10, 2563}, {10, 2563}, {10, 2563}, {10, 2563}, {10, 4098}, {10, 4098},
    {10, 4098}, {10, 4098}, {10, 2051}, {10, 2051}, {10, 2051}, {10, 2051},
    {10, 1540}, {10, 1540}, {10, 1540}, {10, 1540}, {10, 1028}, {10, 1028},
    {10, 1028}, {10, 1028}, {10, 519}, {10, 519}, {10, 519}, {10, 519},
    {10, 20}, {10, 20}, {10, 20}, {10, 20}, {10, 19}, {10, 19},
    {10, 19}, {10, 19}, {11, 23}, {11, 23}, {11, 24}, {11, 24},
    {11, 520}, {11, 520}, {11, 4610}, {11, 4610}, {11, 34306}, {11, 34306},
    {11, 34818}, {11, 34818}, {11, 40449}, {11, 40449}, {11, 40961}, {11,
                                                                      40961},
    {12, 25}, {12, 26}, {12, 27}, {12, 521}, {12, 3075}, {12, 522},
    {12, 1029}, {12, 3587}, {12, 7169}, {12, 32776}, {12, 35330}, {12, 35842},
    {12, 41473}, {12, 41985}, {12, 42497}, {12, 43009}, {6, 65535}, {6, 65535},
    {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535},
    {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535},
    {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535},
    {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535},
    {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535},
    {6, 65535}
};

static const vlcTable_t vlcInterTable1[112] = {
    {7, 36865}, {7, 36353}, {7, 35841}, {7, 35329}, {7, 6145}, {7, 5633},
    {7, 5121}, {7, 4}, {6, 34817}, {6, 34817}, {6, 34305}, {6, 34305},
    {6, 33793}, {6, 33793}, {6, 33281}, {6, 33281}, {6, 4609}, {6, 4609},
    {6, 4097}, {6, 4097}, {6, 3585}, {6, 3585}, {6, 3073}, {6, 3073},
    {6, 514}, {6, 514}, {6, 3}, {6, 3}, {5, 2561}, {5, 2561},
    {5, 2561}, {5, 2561}, {5, 2049}, {5, 2049}, {5, 2049}, {5, 2049},
    {5, 1537}, {5, 1537}, {5, 1537}, {5, 1537}, {4, 32769}, {4, 32769},
    {4, 32769}, {4, 32769}, {4, 32769}, {4, 32769}, {4, 32769}, {4, 32769},
    {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1},
    {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1},
    {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1},
    {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1},
    {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1}, {2, 1},
    {2, 1}, {2, 1}, {3, 513}, {3, 513}, {3, 513}, {3, 513},
    {3, 513}, {3, 513}, {3, 513}, {3, 513}, {3, 513}, {3, 513},
    {3, 513}, {3, 513}, {3, 513}, {3, 513}, {3, 513}, {3, 513},
    {4, 1025}, {4, 1025}, {4, 1025}, {4, 1025}, {4, 1025}, {4, 1025},
    {4, 1025}, {4, 1025}, {4, 2}, {4, 2}, {4, 2}, {4, 2},
    {4, 2}, {4, 2}, {4, 2}, {4, 2}
};

static const vlcTable_t vlcInterTable2[96] = {
    {10, 9}, {10, 8}, {9, 45057}, {9, 45057}, {9, 44545}, {9, 44545},
    {9, 44033}, {9, 44033}, {9, 43521}, {9, 43521}, {9, 43009}, {9, 43009},
    {9, 42497}, {9, 42497}, {9, 41985}, {9, 41985}, {9, 41473}, {9, 41473},
    {9, 32770}, {9, 32770}, {9, 11265}, {9, 11265}, {9, 10753}, {9, 10753},
    {9, 10241}, {9, 10241}, {9, 9729}, {9, 9729}, {9, 9217}, {9, 9217},
    {9, 8705}, {9, 8705}, {9, 8193}, {9, 8193}, {9, 7681}, {9, 7681},
    {9, 2050}, {9, 2050}, {9, 1538}, {9, 1538}, {9, 7}, {9, 7},
    {9, 6}, {9, 6}, {8, 40961}, {8, 40961}, {8, 40961}, {8, 40961},
    {8, 40449}, {8, 40449}, {8, 40449}, {8, 40449}, {8, 39937}, {8, 39937},
    {8, 39937}, {8, 39937}, {8, 39425}, {8, 39425}, {8, 39425}, {8, 39425},
    {8, 38913}, {8, 38913}, {8, 38913}, {8, 38913}, {8, 38401}, {8, 38401},
    {8, 38401}, {8, 38401}, {8, 37889}, {8, 37889}, {8, 37889}, {8, 37889},
    {8, 37377}, {8, 37377}, {8, 37377}, {8, 37377}, {8, 7169}, {8, 7169},
    {8, 7169}, {8, 7169}, {8, 6657}, {8, 6657}, {8, 6657}, {8, 6657},
    {8, 1026}, {8, 1026}, {8, 1026}, {8, 1026}, {8, 515}, {8, 515},
    {8, 515}, {8, 515}, {8, 5}, {8, 5}, {8, 5}, {8, 5}
};

static const vlcTable_t vlcInterTable3[121] = {
    {11, 33282}, {11, 33282}, {11, 32771}, {11, 32771}, {11, 11}, {11, 11},
    {11, 10}, {11, 10}, {10, 47105}, {10, 47105}, {10, 47105}, {10, 47105},
    {10, 46593}, {10, 46593}, {10, 46593}, {10, 46593}, {10, 46081}, {10,
                                                                      46081},
    {10, 46081}, {10, 46081}, {10, 45569}, {10, 45569}, {10, 45569}, {10,
                                                                      45569},
    {10, 4610}, {10, 4610}, {10, 4610}, {10, 4610}, {10, 4098}, {10, 4098},
    {10, 4098}, {10, 4098}, {10, 3586}, {10, 3586}, {10, 3586}, {10, 3586},
    {10, 3074}, {10, 3074}, {10, 3074}, {10, 3074}, {10, 2562}, {10, 2562},
    {10, 2562}, {10, 2562}, {10, 1539}, {10, 1539}, {10, 1539}, {10, 1539},
    {10, 1027}, {10, 1027}, {10, 1027}, {10, 1027}, {10, 516}, {10, 516},
    {10, 516}, {10, 516}, {11, 12}, {11, 12}, {11, 517}, {11, 517},
    {11, 11777}, {11, 11777}, {11, 12289}, {11, 12289}, {11, 47617}, {11,
                                                                      47617},
    {11, 48129}, {11, 48129}, {11, 48641}, {11, 48641}, {11, 49153}, {11,
                                                                      49153},
    {12, 518}, {12, 1028}, {12, 2051}, {12, 2563}, {12, 3075}, {12, 5122},
    {12, 12801}, {12, 13313}, {12, 49665}, {12, 50177}, {12, 50689}, {12,
                                                                      51201},
    {12, 51713}, {12, 52225}, {12, 52737}, {12, 53249}, {6, 65535}, {6, 65535},
    {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535},
    {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535},
    {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535},
    {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535},
    {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535}, {6, 65535},
    {6, 65535}
};

static const u32 lmaxIntraTable[15] = {
    27, 10, 5, 4, 3, 3, 3, 3, 2, 2, 1, 1, 1, 1, 1 };

static const u32 lmaxInterTable[27] = {
    12, 6, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

static const u32 rmaxIntraTableLast[9] = { 20, 6, 1, 0, 0, 0, 0, 0 };

static const u32 rmaxIntraTable[28] = {
    14, 9, 7, 3, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static const u32 rmaxInterTable[13] = { 26, 10, 6, 2, 1, 1, 0, 0, 0, 0, 0, 0 };

static u32 StrmDec_DecodeMvVlc(u32 buffer, i32 * motionVector);

/* vlc tables */
static const MvTable_t MvTable1[14] = {
    {5, 3}, {5, -3}, {4, 2}, {4, 2}, {4, -2}, {4, -2},
    {3, 1}, {3, 1}, {3, 1}, {3, 1}, {3, -1}, {3, -1}, {3, -1}, {3, -1}
};

static const MvTable_t MvTable2[8] = {
    {8, 6}, {8, -6}, {8, 5}, {8, -5}, {7, 4}, {7, 4}, {7, -4}, {7, -4}
};

static const MvTable_t MvTable3[32] = {
    {11, 12}, {11, -12}, {11, 11}, {11, -11}, {10, 10}, {10, 10}, {10, -10},
    {10, -10},
    {10, 9}, {10, 9}, {10, -9}, {10, -9}, {10, 8}, {10, 8}, {10, -8}, {10, -8},
    {8, 7}, {8, 7}, {8, 7}, {8, 7}, {8, 7}, {8, 7}, {8, 7}, {8, 7},
    {8, -7}, {8, -7}, {8, -7}, {8, -7}, {8, -7}, {8, -7}, {8, -7}, {8, -7}
};

static const MvTable_t MvTable4[12] = {
    {11, 24}, {11, 23}, {11, 22}, {11, 21}, {11, 20}, {11, 19},
    {11, 18}, {11, 17}, {11, 16}, {11, 15}, {11, 14}, {11, 13}
};

static const MvTable_t MvTable5[28] = {
    {13, 32}, {13, -32}, {13, 31}, {13, -31}, {12, 30}, {12, 30}, {12, -30},
    {12, -30},
    {12, 29}, {12, 29}, {12, -29}, {12, -29}, {12, 28}, {12, 28}, {12, -28},
    {12, -28},
    {12, 27}, {12, 27}, {12, -27}, {12, -27}, {12, 26}, {12, 26}, {12, -26},
    {12, -26},
    {12, 25}, {12, 25}, {12, -25}, {12, -25}
};

/*------------------------------------------------------------------------------

   5.1  Function name:
            StrmDec_DecodeMcbpc

        Purpose:
            decodes variable length coded mb-type and cbpc from
            stream.

        Input:
            Pointer to DecContainer structure

        Output:
            u32 status OK/NOK

------------------------------------------------------------------------------*/

u32 StrmDec_DecodeMcbpc(DecContainer * pDecContainer, u32 mbNum,
                       u32 buffer, u32 * pusedBits)
{
    mcbpc_t tmpMcbpc;

    switch (pDecContainer->VopDesc.vopCodingType)
    {
        case IVOP:
            if(buffer >= 256)
            {
                tmpMcbpc.len = 1;
                tmpMcbpc.mbType = MB_INTRA;
                tmpMcbpc.cbpc = 0;
            }
            else if(buffer >= 192)
            {
                tmpMcbpc.len = 3;
                tmpMcbpc.mbType = MB_INTRA;
                tmpMcbpc.cbpc = 3;
            }
            else if(buffer >= 128)
            {
                tmpMcbpc.len = 3;
                tmpMcbpc.mbType = MB_INTRA;
                tmpMcbpc.cbpc = 2;
            }
            else if(buffer >= 64)
            {
                tmpMcbpc.len = 3;
                tmpMcbpc.mbType = MB_INTRA;
                tmpMcbpc.cbpc = 1;
            }
            else if(buffer >= 32)
            {
                tmpMcbpc.len = 4;
                tmpMcbpc.mbType = MB_INTRAQ;
                tmpMcbpc.cbpc = 0;
            }
            else if(buffer >= 24)
            {
                tmpMcbpc.len = 6;
                tmpMcbpc.mbType = MB_INTRAQ;
                tmpMcbpc.cbpc = 3;
            }
            else if(buffer >= 16)
            {
                tmpMcbpc.len = 6;
                tmpMcbpc.mbType = MB_INTRAQ;
                tmpMcbpc.cbpc = 2;
            }
            else if(buffer >= 8)
            {
                tmpMcbpc.len = 6;
                tmpMcbpc.mbType = MB_INTRAQ;
                tmpMcbpc.cbpc = 1;
            }
            else if(buffer == 1)
            {
                tmpMcbpc.len = 9;
                tmpMcbpc.mbType = MB_STUFFING;
                tmpMcbpc.cbpc = 0;
            }
            else
                return (HANTRO_NOK);
            break;

        case PVOP:
            if(buffer >= 256)
            {
                tmpMcbpc.len = 1;
                tmpMcbpc.mbType = MB_INTER;
                tmpMcbpc.cbpc = 0;
            }
            else if(buffer >= 192)
            {
                tmpMcbpc.len = 3;
                tmpMcbpc.mbType = MB_INTERQ;
                tmpMcbpc.cbpc = 0;
            }
            else if(buffer >= 128)
            {
                tmpMcbpc.len = 3;
                tmpMcbpc.mbType = MB_INTER4V;
                tmpMcbpc.cbpc = 0;
            }
            else if(buffer >= 96)
            {
                tmpMcbpc.len = 4;
                tmpMcbpc.mbType = MB_INTER;
                tmpMcbpc.cbpc = 1;
            }
            else if(buffer >= 64)
            {
                tmpMcbpc.len = 4;
                tmpMcbpc.mbType = MB_INTER;
                tmpMcbpc.cbpc = 2;
            }
            else if(buffer >= 48)
            {
                tmpMcbpc.len = 5;
                tmpMcbpc.mbType = MB_INTRA;
                tmpMcbpc.cbpc = 0;
            }
            else if(buffer >= 40)
            {
                tmpMcbpc.len = 6;
                tmpMcbpc.mbType = MB_INTER;
                tmpMcbpc.cbpc = 3;
            }
            else if(buffer >= 32)
            {
                tmpMcbpc.len = 6;
                tmpMcbpc.mbType = MB_INTRAQ;
                tmpMcbpc.cbpc = 0;
            }
            else if(buffer >= 28)
            {
                tmpMcbpc.len = 7;
                tmpMcbpc.mbType = MB_INTERQ;
                tmpMcbpc.cbpc = 1;
            }
            else if(buffer >= 24)
            {
                tmpMcbpc.len = 7;
                tmpMcbpc.mbType = MB_INTERQ;
                tmpMcbpc.cbpc = 2;
            }
            else
            {
                tmpMcbpc.len = (TableMcbpcInter[buffer] & 0xFF0000) >> 16;
                tmpMcbpc.mbType = (TableMcbpcInter[buffer] & 0xFF00) >> 8;
                tmpMcbpc.cbpc = (TableMcbpcInter[buffer] & 0xFF);
            }
            break;

        default:
            return (HANTRO_NOK);
    }
#ifdef ASIC_TRACE_SUPPORT
    if (tmpMcbpc.mbType == MB_INTER4V)
        trace_mpeg4DecodingTools.fourMv = 1;
#endif

    if(tmpMcbpc.len == 0)
        return (HANTRO_NOK);
    *pusedBits += (u32) tmpMcbpc.len;

    pDecContainer->MBDesc[mbNum].typeOfMB = tmpMcbpc.mbType;
    pDecContainer->StrmStorage.codedBits[mbNum] = tmpMcbpc.cbpc;

    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

   5.2  Function name:
            StrmDec_DecodeCbpy

        Purpose:
            decodes variable length coded cbpy from stream

        Input:
            Pointer to DecContainer structure

        Output:
            u32 status OK/NOK

------------------------------------------------------------------------------*/

u32 StrmDec_DecodeCbpy(DecContainer * pDecContainer, u32 mbNum,
                      u32 buffer, u32 * pusedBits)
{
    u32 cbpy;
    u32 len;

    if(buffer >= 48)
    {
        len = 2;
        if(MB_IS_INTRA(mbNum))
            cbpy = 15;
        else
            cbpy = 0;
    }
    else if(buffer >= 44)
    {
        len = 4;
        if(MB_IS_INTRA(mbNum))
            cbpy = 7;
        else
            cbpy = 8;
    }
    else if(buffer >= 40)
    {
        len = 4;
        if(MB_IS_INTRA(mbNum))
            cbpy = 11;
        else
            cbpy = 4;
    }
    else if(buffer >= 36)
    {
        len = 4;
        if(MB_IS_INTRA(mbNum))
            cbpy = 3;
        else
            cbpy = 12;
    }
    else if(buffer >= 32)
    {
        len = 4;
        if(MB_IS_INTRA(mbNum))
            cbpy = 13;
        else
            cbpy = 2;
    }
    else if(buffer >= 28)
    {
        len = 4;
        if(MB_IS_INTRA(mbNum))
            cbpy = 5;
        else
            cbpy = 10;
    }
    else if(buffer >= 24)
    {
        len = 4;
        if(MB_IS_INTRA(mbNum))
            cbpy = 14;
        else
            cbpy = 1;
    }
    else if(buffer >= 20)
    {
        len = 4;
        if(MB_IS_INTRA(mbNum))
            cbpy = 10;
        else
            cbpy = 5;
    }
    else if(buffer >= 16)
    {
        len = 4;
        if(MB_IS_INTRA(mbNum))
            cbpy = 12;
        else
            cbpy = 3;
    }
    else if(buffer >= 12)
    {
        len = 4;
        if(MB_IS_INTRA(mbNum))
            cbpy = 0;
        else
            cbpy = 15;
    }
    else if(buffer >= 10)
    {
        len = 5;
        if(MB_IS_INTRA(mbNum))
            cbpy = 1;
        else
            cbpy = 14;
    }
    else if(buffer >= 8)
    {
        len = 5;
        if(MB_IS_INTRA(mbNum))
            cbpy = 2;
        else
            cbpy = 13;
    }
    else if(buffer >= 6)
    {
        len = 5;
        if(MB_IS_INTRA(mbNum))
            cbpy = 4;
        else
            cbpy = 11;
    }
    else if(buffer >= 4)
    {
        len = 5;
        if(MB_IS_INTRA(mbNum))
            cbpy = 8;
        else
            cbpy = 7;
    }
    else if(buffer >= 3)
    {
        len = 6;
        if(MB_IS_INTRA(mbNum))
            cbpy = 9;
        else
            cbpy = 6;
    }
    else if(buffer >= 2)
    {
        len = 6;
        if(MB_IS_INTRA(mbNum))
            cbpy = 6;
        else
            cbpy = 9;
    }
    else
    {
        return (HANTRO_NOK);
    }

    pDecContainer->StrmStorage.codedBits[mbNum] |= (cbpy << 2);
    *pusedBits += len;
    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

   5.1  Function name: StrmDec_DecodeDcCoeff

        Purpose: Decode separately coded dc coefficient. Decodes vlc coded
        dct_dc_size and differential dc additional components.
        This function is called for each block. Note that this function does
        not write dc coefficient to asic input but stores it into location
        pointed by *dcCoeff.

        Input:
            pointer to DecContainer
            macro block number
            block number
            pointer to dc coefficient

        Output:
            OK/NOK/END_OF_STREAM

------------------------------------------------------------------------------*/

u32 StrmDec_DecodeDcCoeff(DecContainer * pDecContainer, u32 mbNum, u32 blockNum,
              i32 * dcCoeff)
{

    u32 buffer;
    u32 length;
    u32 dct_dc_size;
    u32 sign;
    u32 tmp;
    i32 value = 0;

    ASSERT(blockNum < 6);
    ASSERT(mbNum < pDecContainer->VopDesc.totalMbInVop);

    if(pDecContainer->StrmStorage.shortVideo == HANTRO_TRUE)
    {
        tmp = StrmDec_GetBits(pDecContainer, 8);
        if(tmp == END_OF_STREAM)
            return (END_OF_STREAM);
        value = (i32) tmp;

        if (value == 255)
        {
            value = 128;
        }
    }
    /* normal mpeg4 case */
    else
    {
        /* This is luminance case */
        if(blockNum < 4)
        {
            SHOWBITS32(buffer);
            buffer = buffer >> 24;

            if(buffer >= 192)   /* 11xx xxxx */
            {
                dct_dc_size = 1;
                length = 2;
            }
            else if(buffer >= 128)  /* 10xx xxxx */
            {
                dct_dc_size = 2;
                length = 2;
            }
            else if(buffer >= 96)   /* 011x xxxx */
            {
                dct_dc_size = 0;
                length = 3;
            }
            else if(buffer >= 64)   /* 010x xxxx */
            {
                dct_dc_size = 3;
                length = 3;
            }
            else if(buffer >= 32)   /* 001x xxxx */
            {
                dct_dc_size = 4;
                length = 3;
            }
            else if(buffer >= 16)   /* 0001 xxxx */
            {
                dct_dc_size = 5;
                length = 4;
            }
            else if(buffer >= 8)    /* 0000 1xxx */
            {
                dct_dc_size = 6;
                length = 5;
            }
            else if(buffer >= 4)    /* 0000 01xx */
            {
                dct_dc_size = 7;
                length = 6;
            }
            else if(buffer >= 2)    /* 0000 001x */
            {
                dct_dc_size = 8;
                length = 7;
            }
            else if(buffer == 1)    /* 0000 0001 */
            {
                dct_dc_size = 9;
                length = 8;
            }
            else
            {
                return (HANTRO_NOK);
            }

        }

        /* And this is chrominance */
        else
        {
            SHOWBITS32(buffer);
            buffer = buffer >> 23;

            if(buffer >= 384)   /* 1 1xxx xxxx */
            {
                dct_dc_size = 0;
                length = 2;
            }
            else if(buffer >= 256)  /* 1 0xxx xxxx */
            {
                dct_dc_size = 1;
                length = 2;
            }
            else if(buffer >= 128)  /* 0 1xxx xxxx */
            {
                dct_dc_size = 2;
                length = 2;
            }
            else if(buffer >= 64)   /* 0 01xx xxxx */
            {
                dct_dc_size = 3;
                length = 3;
            }
            else if(buffer >= 32)   /* 0 001x xxxx */
            {
                dct_dc_size = 4;
                length = 4;
            }
            else if(buffer >= 16)   /* 0 0001 xxxx */
            {
                dct_dc_size = 5;
                length = 5;
            }
            else if(buffer >= 8)    /* 0 0000 1xxx */
            {
                dct_dc_size = 6;
                length = 6;
            }
            else if(buffer >= 4)    /* 0 0000 01xx */
            {
                dct_dc_size = 7;
                length = 7;
            }
            else if(buffer >= 2)    /* 0 0000 001x */
            {
                dct_dc_size = 8;
                length = 8;
            }
            else if(buffer == 1)    /* 0 0000 0001 */
            {
                dct_dc_size = 9;
                length = 9;
            }
            else
            {
                return (HANTRO_NOK);
            }

        }

        FLUSHBITS(length);

        if(dct_dc_size == 0)
        {
            value = 0;
        }
        else
        {
            /* To get the additional code to count differential dc values.
             * Take marker bit as well if dct_dc_size>8 */
            if(dct_dc_size > 8)
            {
                buffer = StrmDec_GetBits(pDecContainer, dct_dc_size + 1);
                if(buffer == END_OF_STREAM)
                    return buffer;
                /* check marker bit */
                if(!(buffer & 0x1))
                    return (HANTRO_NOK);
                buffer >>= 1;
            }
            else
            {
                buffer = StrmDec_GetBits(pDecContainer, dct_dc_size);
                if(buffer == END_OF_STREAM)
                    return buffer;
            }

            /* msb indicates the sign (1 -> positive, 0 -> negative) */
            sign = !(buffer >> (dct_dc_size - 1));

            /* absolute value */
            tmp = buffer & ((1 << (dct_dc_size - 1)) - 1);

            /* negative value */
            if(sign)
            {
                if(dct_dc_size > 8)
                    value = -256;
                else
                    value = -(1 << dct_dc_size) + 1 + tmp;
            }
            /* positive value */
            else
            {
                if(dct_dc_size > 8)
                    value = 255;
                else
                    value = (1 << (dct_dc_size - 1)) + tmp;
            }
        }
    }

    *dcCoeff = value;
    if( (pDecContainer->VopDesc.moduloTimeBase >= 0xFF) &&
        !(mbNum != (0) ||
          blockNum ||
          pDecContainer->VopDesc.vopCodingType) &&
        pDecContainer->VopDesc.vopNumber )
        *dcCoeff = 255;

    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

   5.2  Function name: StrmDec_DecodeVlcBlock

        Purpose: The decode vlc encoded rlc words
                 This function is called for each block

        Input:
            pointer to DecContainer
            macro block number
            block number

        Output: OK/NOK/END_OF_STREAM

------------------------------------------------------------------------------*/

u32 StrmDec_DecodeVlcBlock(DecContainer * pDecContainer, u32 mbNum,
    u32 blockNum)
{

    const vlcTable_t *tab;
    const vlcTable_t *vlcTable1;
    const vlcTable_t *vlcTable2;
    const vlcTable_t *vlcTable3;
    i32 level;
    u32 sign;
    u32 inputBuffer;
    u32 escape_code;
    u32 escape_type;
    u32 lmax = 0;
    u32 rmax = 0;
    u32 addrCount = 0;
    u32 CodeCount = 0;
    u32 run = 0;
    u32 last = 0;
    u32 rlc = 0;
    u32 tmp = 0;
    u32 intra = 0;
    u32 usedBits = 0;
    u32 shiftt = 0;
    u32 oddRlcTmp = 0;

    if(mbNum == 0 && blockNum == 0)
    {
        pDecContainer->MbSetDesc.oddRlc = 0;
    }
    ASSERT(blockNum < 6);
    ASSERT(mbNum < pDecContainer->VopDesc.totalMbInVop);

    if((pDecContainer->MbSetDesc.
        pCtrlDataAddr[NBR_OF_WORDS_MB * mbNum] & (1<<ASICPOS_USEINTRADCVLC)) &&
       MB_IS_INTRA(mbNum))
    {
        CodeCount = 1;
    }

    /* Check that there is enough 'space' in rlc data buffer (max locations
     * needed by block is 64) */
    if((u32) ((u8 *) pDecContainer->MbSetDesc.pRlcDataCurrAddr -
        (u8 *) pDecContainer->MbSetDesc.pRlcDataAddr) <=
       ((u32) pDecContainer->MbSetDesc.rlcDataBufferSize - 64))
    {

        if(MB_IS_INTRA(mbNum))
            intra = 1;
        if(intra && (pDecContainer->StrmStorage.shortVideo == HANTRO_FALSE))
        {
            vlcTable1 = vlcIntraTable1;
            vlcTable2 = vlcIntraTable2;
            vlcTable3 = vlcIntraTable3;
        }
        else
        {
            vlcTable1 = vlcInterTable1;
            vlcTable2 = vlcInterTable2;
            vlcTable3 = vlcInterTable3;
        }
        SHOWBITS32(inputBuffer);

        do
        {
            if(inputBuffer >= 0x20000000)
            {
                tab = &vlcTable1[(inputBuffer >> 25) - 16];
            }
            else if(inputBuffer >= 0x8000000)
            {
                tab = &vlcTable2[(inputBuffer >> 22) - 32];
            }
            else if(inputBuffer >= 0x800000)
            {
                tab = &vlcTable3[(inputBuffer >> 20) - 8];
            }
            else
            {
                MP4DEC_DEBUG(("NOK1\n"));
                return (HANTRO_NOK);
            }

            usedBits = usedBits + (tab->len) + 1;
            shiftt = (tab->len) + 1;

            /* value 65535 used in tables to indicate escape code */
            if(tab->lrl == 65535)
            {

                /*-----------   ESCAPE CODING  ------------ */

                /* Type 4 escape for short video */
                if(pDecContainer->StrmStorage.shortVideo == HANTRO_TRUE)
                {
                    if(usedBits > 17)
                    {
                        /* not enough bits in inputBuffer */
                        FLUSHBITS(usedBits);
                        SHOWBITS32(inputBuffer);
                        usedBits = shiftt = 0;
                    }
                    escape_code = (inputBuffer << shiftt) >> 17;
                    usedBits += 15;
                    shiftt += 15;

                    last = (escape_code & 0x4000) >> 14;
                    run = (escape_code & 0x3F00) >> 8;
                    level = (escape_code & 0x00FF);

                    if(level == 0 || level == 128)
                    {
                        MP4DEC_DEBUG(("NOK2\n"));
                        return (HANTRO_NOK);
                    }
                    else if(level > 128)
                    {
                        level -= 256;
                    }
                    rlc = (last << 15) | (run << 9) | (level & 0x1FF);
                }
                else
                {
                    escape_type = (inputBuffer << shiftt) >> 30;
                    if(!(escape_type & 0x2))
                    {
                        shiftt += 1;
                        usedBits += 1;
                        escape_type = 1;
                    }
                    else
                    {
                        shiftt += 2;
                        usedBits += 2;
                    }

                    /* Type 1 Escape: After escape_code 0 */
                    if((escape_type == 1) || (escape_type == 2))
                    {
                        if(usedBits > 19)
                        {
                            /* not enough bits, needs more */
                            FLUSHBITS(usedBits);
                            usedBits = shiftt = 0;
                            SHOWBITS32(inputBuffer);
                        }
                        escape_code = (inputBuffer << shiftt) >> 19;

                        if(escape_code >= 1024)
                        {
                            tab = &vlcTable1[(escape_code >> 6) - 16];
                        }
                        else if(escape_code >= 256)
                        {
                            tab = &vlcTable2[(escape_code >> 3) - 32];
                        }
                        else if(escape_code >= 16)
                        {
                            tab = &vlcTable3[(escape_code >> 1) - 8];
                        }
                        else
                        {
                            MP4DEC_DEBUG(("NOK3\n"));
                            return (HANTRO_NOK);
                        }

                        /* can not be escape code at this point */
                        if(tab->lrl == 65535)
                        {
                            MP4DEC_DEBUG(("NOK4\n"));
                            return (HANTRO_NOK);
                        }
                        /* sign, rlc & last */
                        sign = escape_code & (0x1000 >> tab->len);
                        rlc = tab->lrl;
                        last = rlc >> 15;
                        run = (tab->lrl & 0x7E00) >> 9;
                        level = tab->lrl & 0x01FF;

                        if((escape_type == 1 && !intra))
                        {
                            /* inter mb and type 1 escape */
                            if(last == 1)
                            {
                                if(run == 0)
                                    lmax = 3;
                                else if(run == 1)
                                    lmax = 2;
                                else if(run < 41)
                                    lmax = 1;
                                else
                                {
                                    MP4DEC_DEBUG(("NOK5\n"));
                                    return (HANTRO_NOK);
                                }
                            }
                            else
                                lmax = lmaxInterTable[run];
                        }
                        else if(escape_type == 1 && intra)
                        {
                            if(last == 1)
                            {
                                if(run == 0)
                                    lmax = 8;
                                else if(run == 1)
                                    lmax = 3;
                                else if(run < 7)
                                    lmax = 2;
                                else if(run < 21)
                                    lmax = 1;
                                else
                                {
                                    MP4DEC_DEBUG(("NOK6\n"));
                                    return (HANTRO_NOK);
                                }
                            }
                            else
                                lmax = lmaxIntraTable[run];
                        }
                        else if(escape_type == 2 && intra)
                        {
                            if(last == 1)
                                rmax = rmaxIntraTableLast[level - 1];
                            else    /* Last == 0 */
                                rmax = rmaxIntraTable[level - 1];
                        }
                        else
                        {
                            /* inter mb and escape type 2 */
                            if(last == 1)
                            {
                                if(level == 1)
                                    rmax = 40;
                                else if(level == 2)
                                    rmax = 1;
                                else if(level == 3)
                                    rmax = 0;
                                else
                                {
                                    MP4DEC_DEBUG(("NOK7\n"));
                                    return (HANTRO_NOK);
                                }
                            }
                            else    /* Last == 0 */
                                rmax = rmaxInterTable[level - 1];
                        }

                        if(escape_type == 1)
                        {
                            level += lmax;
                            if(sign)
                                level = -level;
                        }
                        else
                        {
                            if(sign)
                                level = -level;
                            run += rmax + 1;
                            if(run > 63)
                            {
                                MP4DEC_DEBUG(("NOK8\n"));
                                return (HANTRO_NOK);
                            }
                        }
                        rlc = (last << 15) | (run << 9) | (level & 0x1FF);

                        /* Length to be flushed: sign + 1bit from escape type */
                        usedBits += (tab->len) + 1;
                        shiftt += (tab->len) + 1;
                    }
                    /* Type 3 Escape: After escape_code 11 */
                    else
                    {
                        if(usedBits > 11)
                        {
                            FLUSHBITS(usedBits);
                            usedBits = shiftt = 0;
                            SHOWBITS32(inputBuffer);
                        }
                        escape_code = (inputBuffer << shiftt) >> 11;
                        usedBits += 21;
                        shiftt += 21;

                        last = (escape_code & 0x100000) >> 20;
                        run = (escape_code & 0xFC000) >> 14;
                        level = (escape_code & 0x0FFE) >> 1;
                        sign = (escape_code & 0x1000);

                        /*Check Marker Bits */
                        if(!(escape_code & 0x01) || !(escape_code & 0x2000))
                        {
                            MP4DEC_DEBUG(("NOK9\n"));
                            return (HANTRO_NOK);
                        }
                        if(level == 0)
                        {
                            MP4DEC_DEBUG(("NOK10\n"));
                            return (HANTRO_NOK);
                        }
                        if(sign)
                            level = level - 2048;
                        if((level > 255) || (level < -256))
                            rlc = ((level << 16) | (last << 15) | (run << 9));
                        else
                            rlc = (last << 15) | (run << 9) | (level & 0x1FF);
                    }
                }
            }
            /* no escape code */
            else
            {
                /* sign */
                sign = inputBuffer & (0x80000000 >> (tab->len));
                rlc = tab->lrl;
                if(sign)
                {
                    level = rlc & 0x1FF;
                    level = -level;
                    rlc = (rlc & 0xFE00) | (level & 0x1FF);
                }
            }

            /* Saving the word to asicinputbuffer   */

            /* if half of a word was left empty last time, fill it first */
            if(pDecContainer->MbSetDesc.oddRlc)
            {
                oddRlcTmp = pDecContainer->MbSetDesc.oddRlc;
                tmp = *pDecContainer->MbSetDesc.pRlcDataCurrAddr;
            }

            /* odd address count -> start saving in 15:0 */
            /* oddRlcTmp means that a word was left half empty in the last
             * block */

            if((addrCount + oddRlcTmp) & 0x01)
            {

                /*MP4DEC_DEBUG(("Writing low, rlc %x %d \n", rlc, blockNum)); */

                pDecContainer->MbSetDesc.oddRlc = 0;
                addrCount++;
                if((rlc & 0x1FF) == 0)  /* Big level */
                {
                    addrCount++;
                    tmp |= (0xFFFF & rlc);
                    *pDecContainer->MbSetDesc.pRlcDataCurrAddr++ = tmp;

                    /* big level to the next word */
                    tmp = (rlc & 0xFFFF0000 /*<<16 */ );
                }
                else    /* normal rlc */
                {
                    tmp = tmp | (rlc & 0xFFFF);
                }

            }
            /* even address count -> start saving in 31:16 */
            else
            {
                /* If starts with big level lower part of rlc written to upper
                 * side of mem */
                addrCount++;

                if((rlc & 0x1FF) == 0)  /* big level */
                {
                    addrCount++;
                    tmp = ((rlc & 0xFFFF) << 16);
                    tmp = tmp | (rlc >> 16 /*& 0xFFFF */ );
                }
                else
                {
                    tmp = (rlc << 16);
                }

            }

            /* 32-bits used -> write to asic input buffer */
            if(((addrCount + oddRlcTmp) & 0x01) == 0)
            {
                *pDecContainer->MbSetDesc.pRlcDataCurrAddr++ = tmp;
            }

            last = rlc & 0x8000;
            CodeCount += (((rlc & 0x7E00) >> 9) + 1);   /* += run + 1 */

            if(CodeCount > 64)
            {
                MP4DEC_DEBUG(("NOK11\n"));
                return (HANTRO_NOK);
            }
            if((usedBits > 19) && !last)
            {
                /* more bits to inputBuffer */
                FLUSHBITS(usedBits);
                SHOWBITS32(inputBuffer);
                usedBits = shiftt = 0;
            }
            else
            {
                inputBuffer = inputBuffer << shiftt;
            }
            /*MP4DEC_DEBUG(("currAddr %x %x %d  \n",
             *(pDecContainer->MbSetDesc.pRlcDataCurrAddr-1),
             (pDecContainer->MbSetDesc.pRlcDataCurrAddr-1), CodeCount));*/
        }
        while(!last);

        /* lonely 16 bits stored in tmp -> write to asic input buffer */
        if(((addrCount + oddRlcTmp) & 0x01) == 1)
        {

            *pDecContainer->MbSetDesc.pRlcDataCurrAddr = tmp;
            pDecContainer->MbSetDesc.oddRlc = 1;
        }

        /* flush before returning upper level */
        if(usedBits != 0)
        {
            FLUSHBITS(usedBits);
        }
    }
    /* less than 64 addresses left in asic input buffer -> get away */
    else
    {
        MP4DEC_DEBUG(("\n\n\nOUTOFBUFFER\n\n\n"));
        return (HANTRO_NOK);
    }

    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

   5.3  Function name: StrmDec_DecodeMv

        Purpose: decode motion vector. Decodes vlc coded mv, residual motion
        vector and differential coding of mv

        Input:
            pointer to DecContainer
            macro block number

        Output:
            OK/NOK/END_OF_STREAM

------------------------------------------------------------------------------*/

u32 StrmDec_DecodeMv(DecContainer * pDecContainer, u32 mbNum)
{

    u32 i;
    u32 buffer;
    u32 numMvs;
    u32 column;
    i32 horMv, verMv;
    i32 rsize;
    u32 len;
    u32 tmp = 0;

    /* [low,high] and range determined based on vop_fcode_forward */
    rsize = pDecContainer->VopDesc.fcodeFwd - 1;

    if(pDecContainer->MBDesc[mbNum].typeOfMB == MB_INTER4V)
    {
        numMvs = 4;
    }
    else
    {
        numMvs = 1;
    }

    /*MP4DEC_DEBUG(("MB %d %-2d MV\n", mbNum, numMvs)); */
    /* determine column index */
    column = mbNum;
    while(column >= pDecContainer->VopDesc.vopWidth)
    {
        column -= pDecContainer->VopDesc.vopWidth;
    }

    SHOWBITS32(buffer);
    len = 32;

    for(i = 0; i < numMvs; i++)
    {
        if(len < 13)
        {
            if(StrmDec_FlushBits(pDecContainer, 32 - len) == END_OF_STREAM)
                return (END_OF_STREAM);
            buffer = StrmDec_ShowBits(pDecContainer, 32);
            len = 32;
        }

        /* vlc code for horizontal motion vector */
        tmp = StrmDec_DecodeMvVlc(buffer >> 19, &horMv);
        if(!tmp)
            return (HANTRO_NOK);
        len -= tmp;
        buffer <<= tmp;

        /* residual motion vector */
        if((rsize != 0) && (horMv != 0))
        {
            if((i32) len < rsize)
            {
                if(StrmDec_FlushBits(pDecContainer, 32 - len) == END_OF_STREAM)
                    return (END_OF_STREAM);
                buffer = StrmDec_ShowBits(pDecContainer, 32);
                len = 32;
            }
            tmp = buffer >> (32 - rsize);

            if(horMv > 0)
            {
                horMv = ((horMv - 1) << rsize) + tmp + 1;
            }
            else
            {
                horMv = -((-horMv - 1) << rsize) - tmp - 1;
            }
            len -= rsize;
            buffer <<= rsize;
        }
        if(len < 13)
        {
            if(StrmDec_FlushBits(pDecContainer, 32 - len) == END_OF_STREAM)
                return (END_OF_STREAM);
            buffer = StrmDec_ShowBits(pDecContainer, 32);
            len = 32;
        }

        /* vlc code for vertical motion vector */
        tmp = StrmDec_DecodeMvVlc(buffer >> 19, &verMv);
        if(!tmp)
            return (HANTRO_NOK);
        len -= tmp;
        buffer <<= tmp;

        /* residual motion vector */
        if((rsize != 0) && (verMv != 0))
        {
            if((i32) len < rsize)
            {
                if(StrmDec_FlushBits(pDecContainer, 32 - len) == END_OF_STREAM)
                    return (END_OF_STREAM);
                buffer = StrmDec_ShowBits(pDecContainer, 32);
                len = 32;
            }
            tmp = buffer >> (32 - rsize);

            if(verMv > 0)
            {
                verMv = ((verMv - 1) << rsize) + tmp + 1;
            }
            else
            {
                verMv = -((-verMv - 1) << rsize) - tmp - 1;
            }
            len -= rsize;
            buffer <<= rsize;
        }

        /* limit motion vectors to range [-992,992] before writing to asic */

        tmp = ((u32) (horMv & 0x7FFF)) << 17;
        tmp |= ((u32) verMv & 0x1FFF) << 4;

        /* write to asic mv data */

        pDecContainer->MbSetDesc.pMvDataAddr[NBR_MV_WORDS_MB * mbNum + i] = tmp;
    }
    FLUSHBITS((32 - len));

    return (HANTRO_OK);

}


/*------------------------------------------------------------------------------

   5.4  Function name: StrmDec_DecodeMvVlc

        Purpose: decode vlc coded motion vector

        Input:
            pointer to DecContainer
            pointer to motion vector

        Output:
            OK/NOK/END_OF_STREAM

------------------------------------------------------------------------------*/

u32 StrmDec_DecodeMvVlc(u32 buffer, i32 * motionVector)
{

    MvTable_t tab;

    if(buffer > 4095)
    {
        tab.val = 0;
        tab.len = 1;
    }
    else if(buffer > 511)
    {
        tab = MvTable1[(buffer >> 8) - 2];
    }
    else if(buffer > 255)
    {
        tab = MvTable2[(buffer >> 5) - 8];
    }
    else if(buffer > 127)
    {
        tab = MvTable3[(buffer >> 2) - 32];
    }
    else if(buffer > 31)
    {
        tab = MvTable4[(buffer >> 3) - 4];
        /* sign */
        if(buffer & 0x4)
            tab.val = -tab.val;
    }
    else if(buffer > 3)
    {
        tab = MvTable5[buffer - 4];
    }
    else
    {
        return (0);
    }

    *motionVector = tab.val;

    return (tab.len);

}

