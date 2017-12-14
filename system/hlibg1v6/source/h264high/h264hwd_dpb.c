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
--  Abstract : Decoded Picture Buffer (DPB) handling
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: h264hwd_dpb.c,v $
--  $Date: 2010/12/01 12:31:03 $
--  $Revision: 1.34 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "h264hwd_cfg.h"
#include "h264hwd_dpb.h"
#include "h264hwd_slice_header.h"
#include "h264hwd_image.h"
#include "h264hwd_util.h"
#include "basetype.h"
#include "dwl.h"

/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/* Function style implementation for IS_REFERENCE() macro to fix compiler
 * warnings */
static u32 IsReference( const dpbPicture_t a, const u32 f ) {
    switch(f) {
        case TOPFIELD: return a.status[0] && a.status[0] != EMPTY;
        case BOTFIELD: return a.status[1] && a.status[1] != EMPTY;
        default: return a.status[0] && a.status[0] != EMPTY &&
                     a.status[1] && a.status[1] != EMPTY;
    }
}

static u32 IsReferenceField( const dpbPicture_t *a)
{
    return (a->status[0] != UNUSED && a->status[0] != EMPTY) ||
        (a->status[1] != UNUSED && a->status[1] != EMPTY);
}

static u32 IsExisting(const dpbPicture_t *a, const u32 f)
{
    if(f < FRAME)
    {
        return  (a->status[f] > NON_EXISTING) &&
            (a->status[f] != EMPTY);
    }
    else
    {
        return (a->status[0] > NON_EXISTING) &&
            (a->status[0] != EMPTY) &&
            (a->status[1] > NON_EXISTING) &&
            (a->status[1] != EMPTY);
    }
}

static u32 IsShortTerm(const dpbPicture_t *a, const u32 f)
{
    if((f < FRAME))
    {
        return (a->status[f] == NON_EXISTING || a->status[f] == SHORT_TERM);
    }
    else
    {
        return (a->status[0] == NON_EXISTING || a->status[0] == SHORT_TERM) &&
            (a->status[1] == NON_EXISTING || a->status[1] == SHORT_TERM);
    }
}

static u32 IsShortTermField(const dpbPicture_t *a)
{
    return (a->status[0] == NON_EXISTING || a->status[0] == SHORT_TERM) ||
        (a->status[1] == NON_EXISTING || a->status[1] == SHORT_TERM);
}

static u32 IsLongTerm(const dpbPicture_t *a, const u32 f)
{
    if(f < FRAME)
    {
        return a->status[f] == LONG_TERM;
    }
    else
    {
        return a->status[0] == LONG_TERM && a->status[1] == LONG_TERM;
    }
}

static u32 IsLongTermField(const dpbPicture_t *a)
{
    return (a->status[0] == LONG_TERM) || (a->status[1] == LONG_TERM);
}

static u32 IsUnused(const dpbPicture_t *a, const u32 f)
{
    if(f < FRAME)
    {
        return (a->status[f] == UNUSED);
    }
    else
    {
        return (a->status[0] == UNUSED) && (a->status[1] == UNUSED);
    }
}

static void SetStatus(dpbPicture_t *pic,const dpbPictureStatus_e s,
                      const u32 f)
{
    if (f < FRAME)
    {
        pic->status[f] = s;
    }
    else
    {
        pic->status[0] = pic->status[1] = s;
    }
}

static void SetPoc(dpbPicture_t *pic, const i32 *poc, const u32 f)
{
    if (f < FRAME)
    {
        pic->picOrderCnt[f] = poc[f];
    }
    else
    {
        pic->picOrderCnt[0] = poc[0];
        pic->picOrderCnt[1] = poc[1];
    }
}

static i32 GetPoc(dpbPicture_t *pic)
{
    i32 poc0 = (pic->status[0] == EMPTY ? 0x7FFFFFFF : pic->picOrderCnt[0]);
    i32 poc1 = (pic->status[1] == EMPTY ? 0x7FFFFFFF : pic->picOrderCnt[1]);
    return MIN(poc0,poc1);
}

#define IS_REFERENCE(a,f)       IsReference(a,f)
#define IS_EXISTING(a,f)        IsExisting(&(a),f)
#define IS_REFERENCE_F(a)       IsReferenceField(&(a))
#define IS_SHORT_TERM(a,f)      IsShortTerm(&(a),f)
#define IS_SHORT_TERM_F(a)      IsShortTermField(&(a))
#define IS_LONG_TERM(a,f)       IsLongTerm(&(a),f)
#define IS_LONG_TERM_F(a)       IsLongTermField(&(a))
#define IS_UNUSED(a,f)          IsUnused(&(a),f)
#define SET_STATUS(pic,s,f)     SetStatus(&(pic),s,f)
#define SET_POC(pic,poc,f)      SetPoc(&(pic),poc,f)
#define GET_POC(pic)            GetPoc(&(pic))

#define MAX_NUM_REF_IDX_L0_ACTIVE 16

#define MEM_STAT_DPB 0x1
#define MEM_STAT_OUT 0x2
#define INVALID_MEM_IDX 0xFF

static void DpbBufFree(dpbStorage_t *dpb, u32 i);
static void OutBufFree(dpbStorage_t *dpb, u32 i);

#define DISPLAY_SMOOTHING (dpb->totBuffers > dpb->dpbSize + 1)
/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/

static i32 ComparePictures(const void *ptr1, const void *ptr2);
static i32 ComparePicturesB(const void *ptr1, const void *ptr2, i32 currPoc);

static i32 CompareFields(const void *ptr1, const void *ptr2);
static i32 CompareFieldsB(const void *ptr1, const void *ptr2, i32 currPoc);

static u32 Mmcop1(dpbStorage_t * dpb, u32 currPicNum, u32 differenceOfPicNums,
                  u32 picStruct);

static u32 Mmcop2(dpbStorage_t * dpb, u32 longTermPicNum, u32 picStruct);

static u32 Mmcop3(dpbStorage_t * dpb, u32 currPicNum, u32 differenceOfPicNums,
                  u32 longTermFrameIdx, u32 picStruct);

static u32 Mmcop4(dpbStorage_t * dpb, u32 maxLongTermFrameIdx, u32 picStruct);

static u32 Mmcop5(dpbStorage_t * dpb, u32 picStruct);

static u32 Mmcop6(dpbStorage_t * dpb, u32 frameNum, i32 * picOrderCnt,
                  u32 longTermFrameIdx, u32 picStruct);

static u32 SlidingWindowRefPicMarking(dpbStorage_t * dpb, u32 picStruct);

static i32 FindDpbPic(dpbStorage_t * dpb, i32 picNum, u32 isShortTerm,
                      u32 field);

static /*@null@ */ dpbPicture_t *FindSmallestPicOrderCnt(dpbStorage_t * dpb);

static u32 OutputPicture(dpbStorage_t * dpb);

/*------------------------------------------------------------------------------

    Function: ComparePictures

        Functional description:
            Function to compare dpb pictures, used by the qsort() function.
            Order of the pictures after sorting shall be as follows:
                1) short term reference pictures starting with the largest
                   picNum
                2) long term reference pictures starting with the smallest
                   longTermPicNum
                3) pictures unused for reference but needed for display
                4) other pictures

        Returns:
            -1      pic 1 is greater than pic 2
             0      equal from comparison point of view
             1      pic 2 is greater then pic 1

------------------------------------------------------------------------------*/

i32 ComparePictures(const void *ptr1, const void *ptr2)
{

/* Variables */

    const dpbPicture_t *pic1, *pic2;

/* Code */

    ASSERT(ptr1);
    ASSERT(ptr2);

    pic1 = (dpbPicture_t *) ptr1;
    pic2 = (dpbPicture_t *) ptr2;

    /* both are non-reference pictures, check if needed for display */
    if(!IS_REFERENCE(*pic1, FRAME) && !IS_REFERENCE(*pic2, FRAME))
    {
        if(pic1->toBeDisplayed && !pic2->toBeDisplayed)
            return (-1);
        else if(!pic1->toBeDisplayed && pic2->toBeDisplayed)
            return (1);
        else
            return (0);
    }
    /* only pic 1 needed for reference -> greater */
    else if(!IS_REFERENCE(*pic2, FRAME))
        return (-1);
    /* only pic 2 needed for reference -> greater */
    else if(!IS_REFERENCE(*pic1, FRAME))
        return (1);
    /* both are short term reference pictures -> check picNum */
    else if(IS_SHORT_TERM(*pic1, FRAME) && IS_SHORT_TERM(*pic2, FRAME))
    {
        if(pic1->picNum > pic2->picNum)
            return (-1);
        else if(pic1->picNum < pic2->picNum)
            return (1);
        else
            return (0);
    }
    /* only pic 1 is short term -> greater */
    else if(IS_SHORT_TERM(*pic1, FRAME))
        return (-1);
    /* only pic 2 is short term -> greater */
    else if(IS_SHORT_TERM(*pic2, FRAME))
        return (1);
    /* both are long term reference pictures -> check picNum (contains the
     * longTermPicNum */
    else
    {
        if(pic1->picNum > pic2->picNum)
            return (1);
        else if(pic1->picNum < pic2->picNum)
            return (-1);
        else
            return (0);
    }
}

i32 CompareFields(const void *ptr1, const void *ptr2)
{

/* Variables */

    dpbPicture_t *pic1, *pic2;

/* Code */

    ASSERT(ptr1);
    ASSERT(ptr2);

    pic1 = (dpbPicture_t *) ptr1;
    pic2 = (dpbPicture_t *) ptr2;

    /* both are non-reference pictures, check if needed for display */
    if(!IS_REFERENCE_F(*pic1) && !IS_REFERENCE_F(*pic2))
        return (0);
    /* only pic 1 needed for reference -> greater */
    else if(!IS_REFERENCE_F(*pic2))
        return (-1);
    /* only pic 2 needed for reference -> greater */
    else if(!IS_REFERENCE_F(*pic1))
        return (1);
    /* both are short term reference pictures -> check picNum */
    else if(IS_SHORT_TERM_F(*pic1) && IS_SHORT_TERM_F(*pic2))
    {
        if(pic1->picNum > pic2->picNum)
            return (-1);
        else if(pic1->picNum < pic2->picNum)
            return (1);
        else
            return (0);
    }
    /* only pic 1 is short term -> greater */
    else if(IS_SHORT_TERM_F(*pic1))
        return (-1);
    /* only pic 2 is short term -> greater */
    else if(IS_SHORT_TERM_F(*pic2))
        return (1);
    /* both are long term reference pictures -> check picNum (contains the
     * longTermPicNum */
    else
    {
        if(pic1->picNum > pic2->picNum)
            return (1);
        else if(pic1->picNum < pic2->picNum)
            return (-1);
        else
            return (0);
    }
}

/*------------------------------------------------------------------------------

    Function: ComparePicturesB

        Functional description:
            Function to compare dpb pictures, used by the qsort() function.
            Order of the pictures after sorting shall be as follows:
                1) short term reference pictures with POC less than current POC
                   in descending order
                2) short term reference pictures with POC greater than current
                   POC in ascending order
                3) long term reference pictures starting with the smallest
                   longTermPicNum

        Returns:
            -1      pic 1 is greater than pic 2
             0      equal from comparison point of view
             1      pic 2 is greater then pic 1

------------------------------------------------------------------------------*/

i32 ComparePicturesB(const void *ptr1, const void *ptr2, i32 currPoc)
{

/* Variables */

    dpbPicture_t *pic1, *pic2;
    i32 poc1, poc2;

/* Code */

    ASSERT(ptr1);
    ASSERT(ptr2);

    pic1 = (dpbPicture_t *) ptr1;
    pic2 = (dpbPicture_t *) ptr2;

    /* both are non-reference pictures */
    if(!IS_REFERENCE(*pic1, FRAME) && !IS_REFERENCE(*pic2, FRAME))
        return (0);
    /* only pic 1 needed for reference -> greater */
    else if(!IS_REFERENCE(*pic2, FRAME))
        return (-1);
    /* only pic 2 needed for reference -> greater */
    else if(!IS_REFERENCE(*pic1, FRAME))
        return (1);
    /* both are short term reference pictures -> check picOrderCnt */
    else if(IS_SHORT_TERM(*pic1, FRAME) && IS_SHORT_TERM(*pic2, FRAME))
    {
        poc1 = MIN(pic1->picOrderCnt[0], pic1->picOrderCnt[1]);
        poc2 = MIN(pic2->picOrderCnt[0], pic2->picOrderCnt[1]);

        if(poc1 < currPoc && poc2 < currPoc)
            return (poc1 < poc2 ? 1 : -1);
        else
            return (poc1 < poc2 ? -1 : 1);
    }
    /* only pic 1 is short term -> greater */
    else if(IS_SHORT_TERM(*pic1, FRAME))
        return (-1);
    /* only pic 2 is short term -> greater */
    else if(IS_SHORT_TERM(*pic2, FRAME))
        return (1);
    /* both are long term reference pictures -> check picNum (contains the
     * longTermPicNum */
    else
    {
        if(pic1->picNum > pic2->picNum)
            return (1);
        else if(pic1->picNum < pic2->picNum)
            return (-1);
        else
            return (0);
    }
}

i32 CompareFieldsB(const void *ptr1, const void *ptr2, i32 currPoc)
{

/* Variables */

    dpbPicture_t *pic1, *pic2;
    i32 poc1, poc2;

/* Code */

    ASSERT(ptr1);
    ASSERT(ptr2);

    pic1 = (dpbPicture_t *) ptr1;
    pic2 = (dpbPicture_t *) ptr2;

    /* both are non-reference pictures */
    if(!IS_REFERENCE_F(*pic1) && !IS_REFERENCE_F(*pic2))
        return (0);
    /* only pic 1 needed for reference -> greater */
    else if(!IS_REFERENCE_F(*pic2))
        return (-1);
    /* only pic 2 needed for reference -> greater */
    else if(!IS_REFERENCE_F(*pic1))
        return (1);
    /* both are short term reference pictures -> check picOrderCnt */
    else if(IS_SHORT_TERM_F(*pic1) && IS_SHORT_TERM_F(*pic2))
    {
        poc1 = IS_SHORT_TERM(*pic1, FRAME) ?
            MIN(pic1->picOrderCnt[0], pic1->picOrderCnt[1]) :
            IS_SHORT_TERM(*pic1, TOPFIELD) ? pic1->picOrderCnt[0] :
            pic1->picOrderCnt[1];
        poc2 = IS_SHORT_TERM(*pic2, FRAME) ?
            MIN(pic2->picOrderCnt[0], pic2->picOrderCnt[1]) :
            IS_SHORT_TERM(*pic2, TOPFIELD) ? pic2->picOrderCnt[0] :
            pic2->picOrderCnt[1];

        if(poc1 <= currPoc && poc2 <= currPoc)
            return (poc1 < poc2 ? 1 : -1);
        else
            return (poc1 < poc2 ? -1 : 1);
    }
    /* only pic 1 is short term -> greater */
    else if(IS_SHORT_TERM_F(*pic1))
        return (-1);
    /* only pic 2 is short term -> greater */
    else if(IS_SHORT_TERM_F(*pic2))
        return (1);
    /* both are long term reference pictures -> check picNum (contains the
     * longTermPicNum */
    else
    {
        if(pic1->picNum > pic2->picNum)
            return (1);
        else if(pic1->picNum < pic2->picNum)
            return (-1);
        else
            return (0);
    }
}

/*------------------------------------------------------------------------------

    Function: h264bsdReorderRefPicList

        Functional description:
            Function to perform reference picture list reordering based on
            reordering commands received in the slice header. See details
            of the process in the H.264 standard.

        Inputs:
            dpb             pointer to dpb storage structure
            order           pointer to reordering commands
            currFrameNum    current frame number
            numRefIdxActive number of active reference indices for current
                            picture

        Outputs:
            dpb             'list' field of the structure reordered

        Returns:
            HANTRO_OK      success
            HANTRO_NOK     if non-existing pictures referred to in the
                           reordering commands

------------------------------------------------------------------------------*/

u32 h264bsdReorderRefPicList(dpbStorage_t * dpb,
                             refPicListReordering_t * order,
                             u32 currFrameNum, u32 numRefIdxActive)
{

/* Variables */

    u32 i, j, k, picNumPred, refIdx;
    i32 picNum, picNumNoWrap, index;
    u32 isShortTerm;

/* Code */

    ASSERT(order);
    ASSERT(currFrameNum <= dpb->maxFrameNum);
    ASSERT(numRefIdxActive <= MAX_NUM_REF_IDX_L0_ACTIVE);

    /* set dpb picture numbers for sorting */
    SetPicNums(dpb, currFrameNum);

    if(!order->refPicListReorderingFlagL0)
        return (HANTRO_OK);

    refIdx = 0;
    picNumPred = currFrameNum;

    i = 0;
    while(order->command[i].reorderingOfPicNumsIdc < 3)
    {
        /* short term */
        if(order->command[i].reorderingOfPicNumsIdc < 2)
        {
            if(order->command[i].reorderingOfPicNumsIdc == 0)
            {
                picNumNoWrap =
                    (i32) picNumPred - (i32) order->command[i].absDiffPicNum;
                if(picNumNoWrap < 0)
                    picNumNoWrap += (i32) dpb->maxFrameNum;
            }
            else
            {
                picNumNoWrap =
                    (i32) (picNumPred + order->command[i].absDiffPicNum);
                if(picNumNoWrap >= (i32) dpb->maxFrameNum)
                    picNumNoWrap -= (i32) dpb->maxFrameNum;
            }
            picNumPred = (u32) picNumNoWrap;
            picNum = picNumNoWrap;
            /*
             * if((u32) picNumNoWrap > currFrameNum)
             * picNum -= (i32) dpb->maxFrameNum;
             */
            isShortTerm = HANTRO_TRUE;
        }
        /* long term */
        else
        {
            picNum = (i32) order->command[i].longTermPicNum;
            isShortTerm = HANTRO_FALSE;

        }
        /* find corresponding picture from dpb */
        index = FindDpbPic(dpb, picNum, isShortTerm, FRAME);
        if(index < 0 || !IS_EXISTING(dpb->buffer[index], FRAME))
            return (HANTRO_NOK);

        /* shift pictures */
        for(j = numRefIdxActive; j > refIdx; j--)
            dpb->list[j] = dpb->list[j - 1];
        /* put picture into the list */
        dpb->list[refIdx++] = index;
        /* remove later references to the same picture */
        for(j = k = refIdx; j <= numRefIdxActive; j++)
            if(dpb->list[j] != index)
                dpb->list[k++] = dpb->list[j];

        i++;
    }

    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

    Function: Mmcop1

        Functional description:
            Function to mark a short-term reference picture unused for
            reference, memory_management_control_operation equal to 1

        Returns:
            HANTRO_OK      success
            HANTRO_NOK     failure, picture does not exist in the buffer

------------------------------------------------------------------------------*/

static u32 Mmcop1(dpbStorage_t * dpb, u32 currPicNum, u32 differenceOfPicNums,
                  u32 picStruct)
{

/* Variables */

    i32 index, picNum;
    u32 field = FRAME;

/* Code */

    ASSERT(currPicNum < dpb->maxFrameNum);

    if(picStruct == FRAME)
    {
        picNum = (i32) currPicNum - (i32) differenceOfPicNums;
        if(picNum < 0)
            picNum += dpb->maxFrameNum;
    }
    else
    {
        picNum = (i32) currPicNum *2 + 1 - (i32) differenceOfPicNums;

        if(picNum < 0)
            picNum += dpb->maxFrameNum * 2;
        field = (picNum & 1) ^ (u32)(picStruct == TOPFIELD);
        picNum /= 2;
    }

    index = FindDpbPic(dpb, picNum, HANTRO_TRUE, field);
    if(index < 0)
        return (HANTRO_NOK);

    SET_STATUS(dpb->buffer[index], UNUSED, field);
    if(IS_UNUSED(dpb->buffer[index], FRAME))
    {
        DpbBufFree(dpb, index);
    }

    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

    Function: Mmcop2

        Functional description:
            Function to mark a long-term reference picture unused for
            reference, memory_management_control_operation equal to 2

        Returns:
            HANTRO_OK      success
            HANTRO_NOK     failure, picture does not exist in the buffer

------------------------------------------------------------------------------*/

static u32 Mmcop2(dpbStorage_t * dpb, u32 longTermPicNum, u32 picStruct)
{

/* Variables */

    i32 index;
    u32 field = FRAME;

/* Code */

    if(picStruct != FRAME)
    {
        field = (longTermPicNum & 1) ^ (u32)(picStruct == TOPFIELD);
        longTermPicNum /= 2;
    }
    index = FindDpbPic(dpb, (i32) longTermPicNum, HANTRO_FALSE, field);
    if(index < 0)
        return (HANTRO_NOK);

    SET_STATUS(dpb->buffer[index], UNUSED, field);
    if(IS_UNUSED(dpb->buffer[index], FRAME))
    {
        DpbBufFree(dpb, index);
    }

    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

    Function: Mmcop3

        Functional description:
            Function to assing a longTermFrameIdx to a short-term reference
            frame (i.e. to change it to a long-term reference picture),
            memory_management_control_operation equal to 3

        Returns:
            HANTRO_OK      success
            HANTRO_NOK     failure, short-term picture does not exist in the
                           buffer or is a non-existing picture, or invalid
                           longTermFrameIdx given

------------------------------------------------------------------------------*/

static u32 Mmcop3(dpbStorage_t * dpb, u32 currPicNum, u32 differenceOfPicNums,
                  u32 longTermFrameIdx, u32 picStruct)
{

/* Variables */

    i32 index, picNum;
    u32 i;
    u32 field = FRAME;

/* Code */

    ASSERT(dpb);
    ASSERT(currPicNum < dpb->maxFrameNum);

    if(picStruct == FRAME)
    {
        picNum = (i32) currPicNum - (i32) differenceOfPicNums;
        if(picNum < 0)
            picNum += dpb->maxFrameNum;
    }
    else
    {
        picNum = (i32) currPicNum *2 + 1 - (i32) differenceOfPicNums;

        if(picNum < 0)
            picNum += dpb->maxFrameNum * 2;
        field = (picNum & 1) ^ (u32)(picStruct == TOPFIELD);
        picNum /= 2;
    }

    if((dpb->maxLongTermFrameIdx == NO_LONG_TERM_FRAME_INDICES) ||
       (longTermFrameIdx > dpb->maxLongTermFrameIdx))
        return (HANTRO_NOK);

    /* check if a long term picture with the same longTermFrameIdx already
     * exist and remove it if necessary */
    for(i = 0; i <= dpb->dpbSize; i++)
        if(IS_LONG_TERM_F(dpb->buffer[i]) &&
           (u32) dpb->buffer[i].picNum == longTermFrameIdx &&
           dpb->buffer[i].frameNum != picNum)
        {
            SET_STATUS(dpb->buffer[i], UNUSED, FRAME);
            if(IS_UNUSED(dpb->buffer[i], FRAME))
            {
                DpbBufFree(dpb, i);
            }
            break;
        }

    index = FindDpbPic(dpb, picNum, HANTRO_TRUE, field);
    if(index < 0)
        return (HANTRO_NOK);
    if(!IS_EXISTING(dpb->buffer[index], field))
        return (HANTRO_NOK);

    SET_STATUS(dpb->buffer[index], LONG_TERM, field);
    dpb->buffer[index].picNum = (i32) longTermFrameIdx;

    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

    Function: Mmcop4

        Functional description:
            Function to set maxLongTermFrameIdx,
            memory_management_control_operation equal to 4

        Returns:
            HANTRO_OK      success

------------------------------------------------------------------------------*/

static u32 Mmcop4(dpbStorage_t * dpb, u32 maxLongTermFrameIdx, u32 picStruct)
{

/* Variables */

    u32 i;

/* Code */

    dpb->maxLongTermFrameIdx = maxLongTermFrameIdx;

    for(i = 0; i <= dpb->dpbSize; i++)
    {
        if(IS_LONG_TERM(dpb->buffer[i], TOPFIELD) &&
           (((u32) dpb->buffer[i].picNum > maxLongTermFrameIdx) ||
            (dpb->maxLongTermFrameIdx == NO_LONG_TERM_FRAME_INDICES)))
        {
            SET_STATUS(dpb->buffer[i], UNUSED, TOPFIELD);
            if(IS_UNUSED(dpb->buffer[i], FRAME))
            {
                DpbBufFree(dpb, i);
            }
        }
        if(IS_LONG_TERM(dpb->buffer[i], BOTFIELD) &&
           (((u32) dpb->buffer[i].picNum > maxLongTermFrameIdx) ||
            (dpb->maxLongTermFrameIdx == NO_LONG_TERM_FRAME_INDICES)))
        {
            SET_STATUS(dpb->buffer[i], UNUSED, BOTFIELD);
            if(IS_UNUSED(dpb->buffer[i], FRAME))
            {
                DpbBufFree(dpb, i);
            }
        }
    }

    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

    Function: Mmcop5

        Functional description:
            Function to mark all reference pictures unused for reference and
            set maxLongTermFrameIdx to NO_LONG_TERM_FRAME_INDICES,
            memory_management_control_operation equal to 5. Function flushes
            the buffer and places all pictures that are needed for display into
            the output buffer.

        Returns:
            HANTRO_OK      success

------------------------------------------------------------------------------*/

static u32 Mmcop5(dpbStorage_t * dpb, u32 picStruct)
{

/* Variables */

    u32 i;

/* Code */

    for(i = 0; i < 16; i++)
    {
        if(IS_REFERENCE_F(dpb->buffer[i]))
        {
            SET_STATUS(dpb->buffer[i], UNUSED, FRAME);
            DpbBufFree(dpb, i);
        }
    }

    /* output all pictures */
    while(OutputPicture(dpb) == HANTRO_OK)
        ;
    dpb->numRefFrames = 0;
    dpb->maxLongTermFrameIdx = NO_LONG_TERM_FRAME_INDICES;
    dpb->prevRefFrameNum = 0;

    return (HANTRO_OK);

}

/*------------------------------------------------------------------------------

    Function: Mmcop6

        Functional description:
            Function to assign longTermFrameIdx to the current picture,
            memory_management_control_operation equal to 6

        Returns:
            HANTRO_OK      success
            HANTRO_NOK     invalid longTermFrameIdx or no room for current
                           picture in the buffer

------------------------------------------------------------------------------*/

static u32 Mmcop6(dpbStorage_t * dpb, u32 frameNum, i32 * picOrderCnt,
                  u32 longTermFrameIdx, u32 picStruct)
{

/* Variables */

    u32 i;

/* Code */

    ASSERT(frameNum < dpb->maxFrameNum);

    if((dpb->maxLongTermFrameIdx == NO_LONG_TERM_FRAME_INDICES) ||
       (longTermFrameIdx > dpb->maxLongTermFrameIdx))
        return (HANTRO_NOK);

    /* check if a long term picture with the same longTermFrameIdx already
     * exist and remove it if necessary */
    for(i = 0; i <= dpb->dpbSize; i++)
        if(IS_LONG_TERM_F(dpb->buffer[i]) &&
           (u32) dpb->buffer[i].picNum == longTermFrameIdx &&
           dpb->buffer + i != dpb->currentOut)
        {
            SET_STATUS(dpb->buffer[i], UNUSED, FRAME);
            if(IS_UNUSED(dpb->buffer[i], FRAME))
            {
                DpbBufFree(dpb, i);
            }
            break;
        }

    /* another field of current frame already marked */
    if(picStruct != FRAME && dpb->currentOut->status[(u32)!picStruct] != EMPTY)
    {
        dpb->currentOut->picNum = (i32) longTermFrameIdx;
        SET_POC(*dpb->currentOut, picOrderCnt, picStruct);
        SET_STATUS(*dpb->currentOut, LONG_TERM, picStruct);
        return (HANTRO_OK);
    }
    else if(dpb->numRefFrames < dpb->maxRefFrames)
    {
        dpb->currentOut->frameNum = frameNum;
        dpb->currentOut->picNum = (i32) longTermFrameIdx;
        SET_POC(*dpb->currentOut, picOrderCnt, picStruct);
        SET_STATUS(*dpb->currentOut, LONG_TERM, picStruct);
        if(dpb->noReordering)
            dpb->currentOut->toBeDisplayed = HANTRO_FALSE;
        else
            dpb->currentOut->toBeDisplayed = HANTRO_TRUE;
        dpb->numRefFrames++;
        dpb->fullness++;
        return (HANTRO_OK);
    }
    /* if there is no room, return an error */
    else
        return (HANTRO_NOK);

}

/*------------------------------------------------------------------------------

    Function: h264bsdMarkDecRefPic

        Functional description:
            Function to perform reference picture marking process. This
            function should be called both for reference and non-reference
            pictures.  Non-reference pictures shall have mark pointer set to
            NULL.

        Inputs:
            dpb         pointer to the DPB data structure
            mark        pointer to reference picture marking commands
            image       pointer to current picture to be placed in the buffer
            frameNum    frame number of the current picture
            picOrderCnt picture order count for the current picture
            isIdr       flag to indicate if the current picture is an
                        IDR picture
            currentPicId    identifier for the current picture, from the
                            application, stored along with the picture
            numErrMbs       number of concealed macroblocks in the current
                            picture, stored along with the picture

        Outputs:
            dpb         'buffer' modified, possible output frames placed into
                        'outBuf'

        Returns:
            HANTRO_OK   success
            HANTRO_NOK  failure

------------------------------------------------------------------------------*/

u32 h264bsdMarkDecRefPic(dpbStorage_t * dpb,
                         const decRefPicMarking_t * mark,
                         const image_t * image,
                         u32 frameNum, i32 * picOrderCnt,
                         u32 isIdr, u32 currentPicId, u32 numErrMbs,
                         u32 tiledMode )
{

/* Variables */

    u32 status;
    u32 markedAsLongTerm;
    u32 toBeDisplayed;
    u32 secondField = 0;

/* Code */

    ASSERT(dpb);
    ASSERT(mark || !isIdr);
    /* removed for XXXXXXXX compliance */
    /*
     * ASSERT(!isIdr ||
     * (frameNum == 0 &&
     * image->picStruct == FRAME ? MIN(picOrderCnt[0],picOrderCnt[1]) == 0 :
     * picOrderCnt[image->picStruct] == 0));
     */
    ASSERT(frameNum < dpb->maxFrameNum);
    ASSERT(image->picStruct <= FRAME);

    if(image->picStruct < FRAME)
    {
        ASSERT(dpb->currentOut->status[image->picStruct] == EMPTY);
        if(dpb->currentOut->status[(u32)!image->picStruct] != EMPTY)
        {
            secondField = 1;
            DEBUG_PRINT(("MARKING SECOND FIELD %d\n", dpb->currentOut - dpb->buffer));
        }
        else
            DEBUG_PRINT(("MARKING FIRST FIELD %d\n", dpb->currentOut - dpb->buffer));
    }
    else
        DEBUG_PRINT(("MARKING FRAME %d\n", dpb->currentOut - dpb->buffer));

    dpb->noOutput = DISPLAY_SMOOTHING && secondField && !dpb->delayedOut;

    if(!secondField && image->data != dpb->currentOut->data)
    {
        ERROR_PRINT("TRYING TO MARK NON-ALLOCATED IMAGE");
        return (HANTRO_NOK);
    }

    dpb->lastContainsMmco5 = HANTRO_FALSE;
    status = HANTRO_OK;

    toBeDisplayed = dpb->noReordering ? HANTRO_FALSE : HANTRO_TRUE;

    /* non-reference picture, stored for display reordering purposes */
    if(mark == NULL)
    {
        dpbPicture_t *currentOut = dpb->currentOut;

        SET_STATUS(*currentOut, UNUSED, image->picStruct);
        currentOut->frameNum = frameNum;
        currentOut->picNum = (i32) frameNum;
        SET_POC(*currentOut, picOrderCnt, image->picStruct);
        /* TODO: if current pic is first field of pic and will be output ->
         * output will only contain first field, second (if present) will
         * be output separately. This shall be fixed when field mode output
         * is implemented */
        if(!dpb->noReordering && (!secondField ||   /* first field of frame */
                                  (!dpb->delayedOut &&
                                   !currentOut->
                                  toBeDisplayed) /* first already output */ ))
            dpb->fullness++;
        currentOut->toBeDisplayed = toBeDisplayed;
    }
    /* IDR picture */
    else if(isIdr)
    {
        dpbPicture_t *currentOut = dpb->currentOut;

        /* flush the buffer */
        (void) Mmcop5(dpb, image->picStruct);
        /* added for XXXXXXXX compliance */
        dpb->prevRefFrameNum = frameNum;
        /* if noOutputOfPriorPicsFlag was set -> the pictures preceding the
         * IDR picture shall not be output -> set output buffer empty */
        if(mark->noOutputOfPriorPicsFlag)
        {
            u32 i, tmp;
            tmp = dpb->outIndexR;
            for (i = 0; i < dpb->numOut; i++)
            {
                OutBufFree(dpb, tmp);
                if (++tmp > dpb->dpbSize)
                    tmp = 0;
            }
            dpb->numOut = 0;
            dpb->outIndexW = dpb->outIndexR = 0;
        }

        if(mark->longTermReferenceFlag)
        {
            SET_STATUS(*currentOut, LONG_TERM, image->picStruct);
            dpb->maxLongTermFrameIdx = 0;
        }
        else
        {
            SET_STATUS(*currentOut, SHORT_TERM, image->picStruct);
            dpb->maxLongTermFrameIdx = NO_LONG_TERM_FRAME_INDICES;
        }
        /* changed for XXXXXXXX compliance */
        currentOut->frameNum = frameNum;
        currentOut->picNum = (i32) frameNum;
        SET_POC(*currentOut, picOrderCnt, image->picStruct);
        currentOut->toBeDisplayed = toBeDisplayed;
        dpb->fullness = 1;
        dpb->numRefFrames = 1;
    }
    /* reference picture */
    else
    {
        markedAsLongTerm = HANTRO_FALSE;
        if(mark->adaptiveRefPicMarkingModeFlag)
        {
            const memoryManagementOperation_t *operation;

            operation = mark->operation;    /* = &mark->operation[0] */

            while(operation->memoryManagementControlOperation)
            {

                switch (operation->memoryManagementControlOperation)
                {
                case 1:
                    (void)   Mmcop1(dpb,
                                    frameNum, operation->differenceOfPicNums,
                                    image->picStruct);
                    break;

                case 2:
                    (void)   Mmcop2(dpb, operation->longTermPicNum,
                                    image->picStruct);
                    break;

                case 3:
                    (void)   Mmcop3(dpb,
                                    frameNum,
                                    operation->differenceOfPicNums,
                                    operation->longTermFrameIdx,
                                    image->picStruct);
                    break;

                case 4:
                    status = Mmcop4(dpb, operation->maxLongTermFrameIdx,
                                    image->picStruct);
                    break;

                case 5:
                    status = Mmcop5(dpb, image->picStruct);
                    dpb->lastContainsMmco5 = HANTRO_TRUE;
                    frameNum = 0;
                    break;

                case 6:
                    status = Mmcop6(dpb,
                                    frameNum,
                                    picOrderCnt, operation->longTermFrameIdx,
                                    image->picStruct);
                    if(status == HANTRO_OK)
                        markedAsLongTerm = HANTRO_TRUE;
                    break;

                default:   /* invalid memory management control operation */
                    status = HANTRO_NOK;
                    break;
                }

                if(status != HANTRO_OK)
                {
                    break;
                }

                operation++;    /* = &mark->operation[i] */
            }
        }
        /* force sliding window marking if first field of current frame was
         * non-reference frame (don't know if this is allowed, but may happen
         * at least in erroneous streams) */
        else if(!secondField ||
                 dpb->currentOut->status[(u32)!image->picStruct] == UNUSED)
        {
            status = SlidingWindowRefPicMarking(dpb, image->picStruct);
        }
        /* if current picture was not marked as long-term reference by
         * memory management control operation 6 -> mark current as short
         * term and insert it into dpb (if there is room) */
        if(!markedAsLongTerm)
        {
            if(dpb->numRefFrames < dpb->maxRefFrames || secondField)
            {
                dpbPicture_t *currentOut = dpb->currentOut;

                currentOut->frameNum = frameNum;
                currentOut->picNum = (i32) frameNum;
                SET_STATUS(*currentOut, SHORT_TERM, image->picStruct);
                SET_POC(*currentOut, picOrderCnt, image->picStruct);
                if(!secondField)
                {
                    currentOut->toBeDisplayed = toBeDisplayed;
                    dpb->fullness++;
                    dpb->numRefFrames++;
                }
                /* first field non-reference and already output (kind of) */
                else if (dpb->currentOut->status[(u32)!image->picStruct] == UNUSED &&
                         dpb->currentOut->toBeDisplayed == 0)
                {
                    dpb->fullness++;
                    dpb->numRefFrames++;
                }
            }
            /* no room */
            else
            {
                status = HANTRO_NOK;
            }
        }
    }
    {
        dpbPicture_t *currentOut = dpb->currentOut;

        currentOut->isIdr = isIdr;
        currentOut->picId = currentPicId;
        currentOut->numErrMbs = numErrMbs;
        currentOut->tiledMode = tiledMode;
        currentOut->isFieldPic = image->picStruct != FRAME;
    }

    return (status);
}

/*------------------------------------------------------------------------------
    Function name   : h264DpbUpdateOutputList
    Description     :
    Return type     : void
    Argument        : dpbStorage_t * dpb
    Argument        : const image_t * image
------------------------------------------------------------------------------*/
void h264DpbUpdateOutputList(dpbStorage_t * dpb, const image_t * image)
{
    u32 i;

    /* dpb was initialized not to reorder the pictures -> output current
     * picture immediately */
    if(dpb->noReordering)
    {
        /* TODO: This does not work when field is missing from output */

        dpbOutPicture_t *dpbOut = &dpb->outBuf[dpb->outIndexW];
        const dpbPicture_t *currentOut = dpb->currentOut;

        dpbOut->data = currentOut->data;
        dpbOut->isIdr = currentOut->isIdr;
        dpbOut->picId = currentOut->picId;
        dpbOut->numErrMbs = currentOut->numErrMbs;
        dpbOut->interlaced = dpb->interlaced;
        dpbOut->fieldPicture = 0;
        dpbOut->memIdx = currentOut->memIdx;
        dpbOut->tiledMode = currentOut->tiledMode;

        if(currentOut->isFieldPic)
        {
            if(currentOut->status[0] == EMPTY || currentOut->status[1] == EMPTY)
            {
                dpbOut->fieldPicture = 1;
                dpbOut->topField = (currentOut->status[0] == EMPTY) ? 0 : 1;

                DEBUG_PRINT(("dec pic %d MISSING FIELD! %s\n", dpbOut->picId,
                       dpbOut->topField ? "BOTTOM" : "TOP"));
            }
        }

        dpb->numOut++;
        dpb->outIndexW++;
        if (dpb->outIndexW == dpb->dpbSize + 1)
            dpb->outIndexW = 0;

        if (DISPLAY_SMOOTHING)
            dpb->memStat[currentOut->memIdx] |= MEM_STAT_OUT;
    }
    else
    {
        /* output pictures if buffer full */
        while(dpb->fullness > dpb->dpbSize)
        {
            i = OutputPicture(dpb);
            ASSERT(i == HANTRO_OK);
            (void) i;
        }
    }

    /* if currentOut is the last element of list -> exchange with first empty
     * slot so that only first 16 elements used as reference */
    if(dpb->currentOut == dpb->buffer + dpb->dpbSize)
    {
        for(i = 0; i < dpb->dpbSize; i++)
        {
            if(!dpb->buffer[i].toBeDisplayed &&
               !IS_REFERENCE(dpb->buffer[i], 0) &&
               !IS_REFERENCE(dpb->buffer[i], 1))
            {
                dpbPicture_t tmpPic = *dpb->currentOut;

                *dpb->currentOut = dpb->buffer[i];
                dpb->currentOutPos = i;
                dpb->buffer[i] = tmpPic;
                dpb->currentOut = dpb->buffer + i;
                break;
            }
        }
    }
}

/*------------------------------------------------------------------------------

    Function: h264bsdGetRefPicData

        Functional description:
            Function to get reference picture data from the reference picture
            list

        Returns:
            pointer to desired reference picture data
            NULL if invalid index or non-existing picture referred

------------------------------------------------------------------------------*/

i32 h264bsdGetRefPicData(const dpbStorage_t * dpb, u32 index)
{

/* Variables */

/* Code */
    if(index > 16 || dpb->buffer[dpb->list[index]].data == NULL)
        return -1;
    else if(!IS_EXISTING(dpb->buffer[dpb->list[index]], FRAME))
        return -1;
    else
        return (i32) (dpb->list[index]);

}

/*------------------------------------------------------------------------------

    Function: h264bsdGetRefPicDataVlcMode

        Functional description:
            Function to get reference picture data from the reference picture
            list

        Returns:
            pointer to desired reference picture data
            NULL if invalid index or non-existing picture referred

------------------------------------------------------------------------------*/

u8 *h264bsdGetRefPicDataVlcMode(const dpbStorage_t * dpb, u32 index,
                                u32 fieldMode)
{

/* Variables */

/* Code */

    if(!fieldMode)
    {
        if(index >= dpb->dpbSize)
            return (NULL);
        else if(!IS_EXISTING(dpb->buffer[index], FRAME))
            return (NULL);
        else
            return (u8 *) (dpb->buffer[index].data->virtualAddress);
    }
    else
    {
        const u32 field = (index & 1) ? BOTFIELD : TOPFIELD;
        if(index / 2 >= dpb->dpbSize)
            return (NULL);
        else if(!IS_EXISTING(dpb->buffer[index / 2], field))
            return (NULL);
        else
            return (u8 *) (dpb->buffer[index / 2].data->virtualAddress);
    }

}

/*------------------------------------------------------------------------------

    Function: h264bsdAllocateDpbImage

        Functional description:
            function to allocate memory for a image. This function does not
            really allocate any memory but reserves one of the buffer
            positions for decoding of current picture

        Returns:
            pointer to memory area for the image

------------------------------------------------------------------------------*/

void *h264bsdAllocateDpbImage(dpbStorage_t * dpb)
{

/* Variables */

    u32 i;

/* Code */

    /*
     * ASSERT(!dpb->buffer[dpb->dpbSize].toBeDisplayed &&
     * !IS_REFERENCE(dpb->buffer[dpb->dpbSize]));
     * ASSERT(dpb->fullness <= dpb->dpbSize);
     */

    /* find first unused and not-to-be-displayed pic */
    for(i = 0; i <= dpb->dpbSize; i++)
    {
        if(!dpb->buffer[i].toBeDisplayed && !IS_REFERENCE_F(dpb->buffer[i]))
            break;
    }

    ASSERT(i <= dpb->dpbSize);
    dpb->currentOut = &dpb->buffer[i];
    dpb->currentOutPos = i;
    dpb->currentOut->status[0] = dpb->currentOut->status[1] = EMPTY;

    if (DISPLAY_SMOOTHING &&
          /* placed in the output buffer */
        ( (dpb->memStat[dpb->currentOut->memIdx] & MEM_STAT_OUT) ||
          /* just output from the decoder */
          dpb->currentOut->memIdx == dpb->prevOutIdx))
    {
        i = dpb->currentOut->memIdx;
        dpb->memStat[i] &= ~MEM_STAT_DPB;
        if (dpb->numFreeBuffers)
        {
            i = dpb->freeBuffers[--dpb->numFreeBuffers];
            /* do not overwrite last output picture */
            if (i == dpb->prevOutIdx)
            {
                i = dpb->freeBuffers[0];
                dpb->freeBuffers[0] = dpb->prevOutIdx;
            }
            /* currentOut not in the output buffer and not used as reference,
             * but was just output -> add to free buffers */
            if (dpb->currentOut->memIdx == dpb->prevOutIdx)
            {
                ASSERT(!dpb->memStat[dpb->currentOut->memIdx]);
                dpb->freeBuffers[dpb->numFreeBuffers++] = dpb->prevOutIdx;
            }
            dpb->currentOut->memIdx = i;
            dpb->currentOut->data = dpb->picBuffers + i;
            dpb->memStat[i] = MEM_STAT_DPB;
        }
        else
        {
            /* TODO: MITÄ PITÄS TEHÄ... */
        }
    }

    ASSERT(dpb->currentOut->data);

#if 0
    if (DISPLAY_SMOOTHING && dpb->prevOutIdx != INVALID_MEM_IDX)
    {
        extern u8 *lastOutputPic;
        if (lastOutputPic == dpb->currentOut->data->virtualAddress)
        {
            exit(112);
        }
    }
#endif

    return dpb->currentOut->data;
}

/*------------------------------------------------------------------------------

    Function: SlidingWindowRefPicMarking

        Functional description:
            Function to perform sliding window refence picture marking process.

        Outputs:
            HANTRO_OK      success
            HANTRO_NOK     failure, no short-term reference frame found that
                           could be marked unused

------------------------------------------------------------------------------*/

static u32 SlidingWindowRefPicMarking(dpbStorage_t * dpb, u32 picStruct)
{

/* Variables */

    i32 index, picNum;
    u32 i;

/* Code */

    if(dpb->numRefFrames < dpb->maxRefFrames)
    {
        return (HANTRO_OK);
    }
    else
    {
        index = -1;
        picNum = 0;
        /* find the oldest short term picture */
        for(i = 0; i < dpb->dpbSize; i++)
            if(IS_SHORT_TERM_F(dpb->buffer[i]))
            {
                if(dpb->buffer[i].picNum < picNum || index == -1)
                {
                    index = (i32) i;
                    picNum = dpb->buffer[i].picNum;
                }
            }
        if(index >= 0)
        {
            SET_STATUS(dpb->buffer[index], UNUSED, FRAME);
            DpbBufFree(dpb, index);

            return (HANTRO_OK);
        }
    }

    return (HANTRO_NOK);

}

/*------------------------------------------------------------------------------

    Function: h264bsdInitDpb

        Functional description:
            Function to initialize DPB. Reserves memories for the buffer,
            reference picture list and output buffer. dpbSize indicates
            the maximum DPB size indicated by the levelIdc in the stream.
            If noReordering flag is HANTRO_FALSE the DPB stores dpbSize pictures
            for display reordering purposes. On the other hand, if the
            flag is HANTRO_TRUE the DPB only stores maxRefFrames reference pictures
            and outputs all the pictures immediately.

        Inputs:
            picSizeInMbs    picture size in macroblocks
            dpbSize         size of the DPB (number of pictures)
            maxRefFrames    max number of reference frames
            maxFrameNum     max frame number
            noReordering    flag to indicate that DPB does not have to
                            prepare to reorder frames for display

        Outputs:
            dpb             pointer to dpb data storage

        Returns:
            HANTRO_OK       success
            MEMORY_ALLOCATION_ERROR if memory allocation failed

------------------------------------------------------------------------------*/

u32 h264bsdInitDpb(const void *dwl,
                   dpbStorage_t * dpb,
                   u32 picSizeInMbs,
                   u32 dpbSize,
                   u32 maxRefFrames, u32 maxFrameNum, u32 noReordering,
                   u32 displaySmoothing,
                   u32 monoChrome, u32 isHighSupported, u32 enable2ndChroma,
                   u32 multiBuffPp)
{

/* Variables */

    u32 i;

/* Code */

    ASSERT(picSizeInMbs);
    ASSERT(maxRefFrames <= MAX_NUM_REF_PICS);
    ASSERT(maxRefFrames <= dpbSize);
    ASSERT(maxFrameNum);
    ASSERT(dpbSize);

    /* we trust our memset; ignore return value */
    (void) G1DWLmemset(dpb, 0, sizeof(*dpb)); /* make sure all is clean */

    dpb->picSizeInMbs = picSizeInMbs;
    dpb->maxLongTermFrameIdx = NO_LONG_TERM_FRAME_INDICES;
    dpb->maxRefFrames = MAX(maxRefFrames, 1);

    if(noReordering)
        dpb->dpbSize = dpb->maxRefFrames;
    else
        dpb->dpbSize = dpbSize;

    dpb->maxFrameNum = maxFrameNum;
    dpb->noReordering = noReordering;
    dpb->fullness = 0;
    dpb->numRefFrames = 0;
    dpb->prevRefFrameNum = 0;
    dpb->numOut = 0;
    dpb->outIndexW = 0;
    dpb->outIndexR = 0;
    dpb->numFreeBuffers = 0;
    dpb->prevOutIdx = INVALID_MEM_IDX;

    dpb->totBuffers = dpb->dpbSize + 1;
    if (displaySmoothing)
        dpb->totBuffers += noReordering ? 1 : dpb->dpbSize + 1;
    else if (multiBuffPp)
        dpb->totBuffers++;

    for(i = 0; i < dpb->totBuffers; i++)
    {
        u32 pic_buff_size;

        if(isHighSupported)
        {
            /* yuv picture + direct mode motion vectors */
            pic_buff_size = picSizeInMbs * ((monoChrome ? 256 : 384) + 64);
            dpb->dirMvOffset = picSizeInMbs * (monoChrome ? 256 : 384);
        }
        else
        {
            pic_buff_size = picSizeInMbs * 384;
        }

        if (enable2ndChroma && !monoChrome)
        {
            dpb->ch2Offset = pic_buff_size;
            pic_buff_size += picSizeInMbs * 128;
        }

        if(G1DWLMallocRefFrm(dwl, pic_buff_size, dpb->picBuffers + i) != 0)
            return (MEMORY_ALLOCATION_ERROR);

        if (i < dpb->dpbSize + 1)
        {
            dpb->buffer[i].data = dpb->picBuffers + i;
            dpb->buffer[i].memIdx = i;
            dpb->memStat[i] = MEM_STAT_DPB;
        }
        else
        {
            dpb->freeBuffers[dpb->numFreeBuffers++] = i;
        }

        if(isHighSupported)
        {
            void * base = (char *) (dpb->picBuffers[i].virtualAddress) +
                      dpb->dirMvOffset;
            (void)G1DWLmemset(base, 0, picSizeInMbs * 64);
        }
    }

    dpb->outBuf = G1DWLmalloc((dpb->dpbSize + 1) * sizeof(dpbOutPicture_t));

    if(dpb->outBuf == NULL)
    {
        return (MEMORY_ALLOCATION_ERROR);
    }
	G1DWLmemset((void *)(dpb->outBuf),0,(dpb->dpbSize + 1) * sizeof(dpbOutPicture_t));

    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

    Function: h264bsdResetDpb

        Functional description:
            Function to reset DPB. This function should be called when an IDR
            slice (other than the first) activates new sequence parameter set.
            Function calls h264bsdFreeDpb to free old allocated memories and
            h264bsdInitDpb to re-initialize the DPB. Same inputs, outputs and
            returns as for h264bsdInitDpb.

------------------------------------------------------------------------------*/

u32 h264bsdResetDpb(const void *dwl,
                    dpbStorage_t * dpb,
                    u32 picSizeInMbs,
                    u32 dpbSize,
                    u32 maxRefFrames, u32 maxFrameNum, u32 noReordering,
                    u32 displaySmoothing,
                    u32 monoChrome, u32 isHighSupported, u32 enable2ndChroma,
                    u32 multiBuffPp)
{

/* Code */

    ASSERT(picSizeInMbs);
    ASSERT(maxRefFrames <= MAX_NUM_REF_PICS);
    ASSERT(maxRefFrames <= dpbSize);
    ASSERT(maxFrameNum);
    ASSERT(dpbSize);

    if(dpb->picSizeInMbs == picSizeInMbs)
    {
        u32 new_dpbSize;

        dpb->maxLongTermFrameIdx = NO_LONG_TERM_FRAME_INDICES;
        dpb->maxRefFrames = (maxRefFrames != 0) ? maxRefFrames : 1;

        if(noReordering)
            new_dpbSize = dpb->maxRefFrames;
        else
            new_dpbSize = dpbSize;

        dpb->noReordering = noReordering;
        dpb->maxFrameNum = maxFrameNum;
        dpb->flushed = 0;

        if(dpb->dpbSize == new_dpbSize)
        {
            /* number of pictures and DPB size are not changing */
            /* no need to reallocate DPB */
            return (HANTRO_OK);
        }
    }

    h264bsdFreeDpb(dwl, dpb);

    return h264bsdInitDpb(dwl, dpb, picSizeInMbs, dpbSize, maxRefFrames,
                          maxFrameNum, noReordering, displaySmoothing,
                          monoChrome, isHighSupported, enable2ndChroma,
                          multiBuffPp);
}

/*------------------------------------------------------------------------------

    Function: h264bsdInitRefPicList

        Functional description:
            Function to initialize reference picture list. Function just
            sets pointers in the list according to pictures in the buffer.
            The buffer is assumed to contain pictures sorted according to
            what the H.264 standard says about initial reference picture list.

        Inputs:
            dpb     pointer to dpb data structure

        Outputs:
            dpb     'list' field initialized

        Returns:
            none

------------------------------------------------------------------------------*/

void h264bsdInitRefPicList(dpbStorage_t * dpb)
{

/* Variables */

    u32 i;

/* Code */

    /*for(i = 0; i < dpb->numRefFrames; i++) */
    /*dpb->list[i] = &dpb->buffer[i]; */
    for(i = 0; i <= dpb->dpbSize; i++)
        dpb->list[i] = i;
    ShellSort(dpb, dpb->list, 0, 0);

}

/*------------------------------------------------------------------------------

    Function: FindDpbPic

        Functional description:
            Function to find a reference picture from the buffer. The picture
            to be found is identified by picNum and isShortTerm flag.

        Returns:
            index of the picture in the buffer
            -1 if the specified picture was not found in the buffer

------------------------------------------------------------------------------*/

static i32 FindDpbPic(dpbStorage_t * dpb, i32 picNum, u32 isShortTerm,
                      u32 field)
{

/* Variables */

    u32 i = 0;
    u32 found = HANTRO_FALSE;

/* Code */

    if(isShortTerm)
    {
        while(i <= dpb->dpbSize && !found)
        {
            if(IS_SHORT_TERM(dpb->buffer[i], field) &&
               dpb->buffer[i].frameNum == picNum)
                found = HANTRO_TRUE;
            else
                i++;
        }
    }
    else
    {
        ASSERT(picNum >= 0);
        while(i <= dpb->dpbSize && !found)
        {
            if(IS_LONG_TERM(dpb->buffer[i], field) &&
               dpb->buffer[i].picNum == picNum)
                found = HANTRO_TRUE;
            else
                i++;
        }
    }

    if(found)
        return ((i32) i);
    else
        return (-1);

}

/*------------------------------------------------------------------------------

    Function: SetPicNums

        Functional description:
            Function to set picNum values for short-term pictures in the
            buffer. Numbering of pictures is based on frame numbers and as
            frame numbers are modulo maxFrameNum -> frame numbers of older
            pictures in the buffer may be bigger than the currFrameNum.
            picNums will be set so that current frame has the largest picNum
            and all the short-term frames in the buffer will get smaller picNum
            representing their "distance" from the current frame. This
            function kind of maps the modulo arithmetic back to normal.

------------------------------------------------------------------------------*/

void SetPicNums(dpbStorage_t * dpb, u32 currFrameNum)
{

/* Variables */

    u32 i;
    i32 frameNumWrap;

/* Code */

    ASSERT(dpb);
    ASSERT(currFrameNum < dpb->maxFrameNum);

    for(i = 0; i <= dpb->dpbSize; i++)
        if(IS_SHORT_TERM(dpb->buffer[i], FRAME))
        {
            if(dpb->buffer[i].frameNum > currFrameNum)
                frameNumWrap =
                    (i32) dpb->buffer[i].frameNum - (i32) dpb->maxFrameNum;
            else
                frameNumWrap = (i32) dpb->buffer[i].frameNum;
            dpb->buffer[i].picNum = frameNumWrap;
        }

}

/*------------------------------------------------------------------------------

    Function: h264bsdCheckGapsInFrameNum

        Functional description:
            Function to check gaps in frame_num and generate non-existing
            (short term) reference pictures if necessary. This function should
            be called only for non-IDR pictures.

        Inputs:
            dpb         pointer to dpb data structure
            frameNum    frame number of the current picture
            isRefPic    flag to indicate if current picture is a reference or
                        non-reference picture

        Outputs:
            dpb         'buffer' possibly modified by inserting non-existing
                        pictures with sliding window marking process

        Returns:
            HANTRO_OK   success
            HANTRO_NOK  error in sliding window reference picture marking or
                        frameNum equal to previous reference frame used for
                        a reference picture

------------------------------------------------------------------------------*/
u32 h264bsdCheckGapsInFrameNum(dpbStorage_t * dpb, u32 frameNum, u32 isRefPic,
                               u32 gapsAllowed)
{

/* Variables */

    u32 unUsedShortTermFrameNum;
    const void *tmp;

/* Code */

    ASSERT(dpb);
    ASSERT(dpb->fullness <= dpb->dpbSize);
    ASSERT(frameNum < dpb->maxFrameNum);

    if(!gapsAllowed)
    {
        return HANTRO_OK;
    }

    if((frameNum != dpb->prevRefFrameNum) &&
       (frameNum != ((dpb->prevRefFrameNum + 1) % dpb->maxFrameNum)))
    {
        unUsedShortTermFrameNum = (dpb->prevRefFrameNum + 1) % dpb->maxFrameNum;

        /* store data pointer of last buffer position to be used as next
         * "allocated" data pointer. if last buffer position after this process
         * contains data pointer located in outBuf (buffer placed in the output
         * shall not be overwritten by the current picture) */
        (void)h264bsdAllocateDpbImage(dpb);
        tmp = dpb->currentOut->data;

        do
        {
            SetPicNums(dpb, unUsedShortTermFrameNum);

            if(SlidingWindowRefPicMarking(dpb, FRAME) != HANTRO_OK)
            {
                return (HANTRO_NOK);
            }

            /* output pictures if buffer full */
            while(dpb->fullness >= dpb->dpbSize)
            {
                u32 ret;

                ASSERT(!dpb->noReordering);
                ret = OutputPicture(dpb);
                ASSERT(ret == HANTRO_OK);
                (void) ret;
            }

            /* add to end of list */
            /*
             * ASSERT(!dpb->buffer[dpb->dpbSize].toBeDisplayed &&
             * !IS_REFERENCE(dpb->buffer[dpb->dpbSize])); */

            (void)h264bsdAllocateDpbImage(dpb);
            SET_STATUS(*dpb->currentOut, NON_EXISTING, FRAME);
            dpb->currentOut->frameNum = unUsedShortTermFrameNum;
            dpb->currentOut->picNum = (i32) unUsedShortTermFrameNum;
            dpb->currentOut->picOrderCnt[0] = 0;
            dpb->currentOut->picOrderCnt[1] = 0;
            dpb->currentOut->toBeDisplayed = HANTRO_FALSE;
            dpb->fullness++;
            dpb->numRefFrames++;

            unUsedShortTermFrameNum = (unUsedShortTermFrameNum + 1) %
                dpb->maxFrameNum;

        }
        while(unUsedShortTermFrameNum != frameNum);

        (void)h264bsdAllocateDpbImage(dpb);
        /* pictures placed in output buffer -> check that 'data' in
         * buffer position dpbSize is not in the output buffer (this will be
         * "allocated" by h264bsdAllocateDpbImage). If it is -> exchange data
         * pointer with the one stored in the beginning */
        if(dpb->numOut && !DISPLAY_SMOOTHING)
        {
            u32 i, idx;
            dpbPicture_t *buff = dpb->buffer;

            idx = dpb->outIndexR;
            for(i = 0; i < dpb->numOut; i++)
            {
                ASSERT(i < dpb->dpbSize);

                if(dpb->outBuf[idx].data == dpb->currentOut->data)
                {
                    /* find buffer position containing data pointer
                     * stored in tmp
                     */

                    for(i = 0; i < dpb->dpbSize; i++)
                    {
                        if(buff[i].data == tmp)
                        {
                            buff[i].data = dpb->currentOut->data;
                            dpb->currentOut->data = (void *) tmp;
                            break;
                        }
                    }

                    break;
                }
                if (++idx > dpb->dpbSize)
                    idx = 0;
            }
        }
    }
    /* frameNum for reference pictures shall not be the same as for previous
     * reference picture, otherwise accesses to pictures in the buffer cannot
     * be solved unambiguously */
    else if(isRefPic && frameNum == dpb->prevRefFrameNum)
    {
        return (HANTRO_NOK);
    }

    /* save current frame_num in prevRefFrameNum. For non-reference frame
     * prevFrameNum is set to frame number of last non-existing frame above */
    if(isRefPic)
        dpb->prevRefFrameNum = frameNum;
    else if(frameNum != dpb->prevRefFrameNum)
    {
        dpb->prevRefFrameNum =
            (frameNum + dpb->maxFrameNum - 1) % dpb->maxFrameNum;
    }

    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

    Function: FindSmallestPicOrderCnt

        Functional description:
            Function to find picture with smallest picture order count. This
            will be the next picture in display order.

        Returns:
            pointer to the picture, NULL if no pictures to be displayed

------------------------------------------------------------------------------*/

dpbPicture_t *FindSmallestPicOrderCnt(dpbStorage_t * dpb)
{

/* Variables */

    u32 i;
    i32 picOrderCnt;
    dpbPicture_t *tmp;

/* Code */

    ASSERT(dpb);

    picOrderCnt = 0x7FFFFFFF;
    tmp = NULL;

    for(i = 0; i <= dpb->dpbSize; i++)
    {
        /* TODO: currently only outputting frames, asssumes that fields of a
         * frame are output consecutively */
        if(dpb->buffer[i].toBeDisplayed &&
           GET_POC(dpb->buffer[i]) < picOrderCnt)
            /*
             * MIN(dpb->buffer[i].picOrderCnt[0],dpb->buffer[i].picOrderCnt[1]) <
             * picOrderCnt &&
             * dpb->buffer[i].status[0] != EMPTY &&
             * dpb->buffer[i].status[1] != EMPTY)
             */
        {
            if(dpb->buffer[i].picOrderCnt[1] >= picOrderCnt)
            {
                DEBUG_PRINT(("HEP %d %d\n", dpb->buffer[i].picOrderCnt[1],
                       picOrderCnt));
            }
            tmp = dpb->buffer + i;
            picOrderCnt = GET_POC(dpb->buffer[i]);
        }
    }

    return (tmp);

}

/*------------------------------------------------------------------------------

    Function: OutputPicture

        Functional description:
            Function to put next display order picture into the output buffer.

        Returns:
            HANTRO_OK      success
            HANTRO_NOK     no pictures to display

------------------------------------------------------------------------------*/

u32 OutputPicture(dpbStorage_t * dpb)
{

/* Variables */

    dpbPicture_t *tmp;
    dpbOutPicture_t *dpbOut;

/* Code */

    ASSERT(dpb);

    if(dpb->noReordering)
        return (HANTRO_NOK);

    tmp = FindSmallestPicOrderCnt(dpb);

    /* no pictures to be displayed */
    if(tmp == NULL)
        return (HANTRO_NOK);

    /* remove it from DPB */
    tmp->toBeDisplayed = HANTRO_FALSE;

    /* updated output list */
    dpbOut = &dpb->outBuf[dpb->outIndexW]; /* next output */
    dpbOut->data = tmp->data;
    dpbOut->isIdr = tmp->isIdr;
    dpbOut->picId = tmp->picId;
    dpbOut->numErrMbs = tmp->numErrMbs;
    dpbOut->interlaced = dpb->interlaced;
    dpbOut->fieldPicture = 0;
    dpbOut->memIdx = tmp->memIdx;
    dpbOut->tiledMode = tmp->tiledMode;

    dpb->memStat[tmp->memIdx] |= MEM_STAT_OUT;

    if(tmp->isFieldPic)
    {
        if(tmp->status[0] == EMPTY || tmp->status[1] == EMPTY)
        {
            dpbOut->fieldPicture = 1;
            dpbOut->topField = (tmp->status[0] == EMPTY) ? 0 : 1;

            DEBUG_PRINT(("dec pic %d MISSING FIELD! %s\n", dpbOut->picId,
                   dpbOut->topField ? "BOTTOM" : "TOP"));
        }
    }

    dpb->numOut++;
    dpb->outIndexW++;
    if (dpb->outIndexW == dpb->dpbSize + 1)
        dpb->outIndexW = 0;

    if(!IS_REFERENCE_F(*tmp))
    {
        dpb->fullness--;
    }

    /* if output buffer full -> ignore oldest. This can only happen with
     * corrupted mvc streams when there are no output pictures from the other
     * view */
    if (dpb->numOut > dpb->dpbSize + 1)
    {
        dpb->outIndexR++;
        if (dpb->outIndexR == dpb->dpbSize + 1)
            dpb->outIndexR = 0;
        dpb->numOut--;
    }

    return (HANTRO_OK);
}

/*------------------------------------------------------------------------------

    Function: h264bsdDpbOutputPicture

        Functional description:
            Function to get next display order picture from the output buffer.

        Return:
            pointer to output picture structure, NULL if no pictures to
            display

------------------------------------------------------------------------------*/

dpbOutPicture_t *h264bsdDpbOutputPicture(dpbStorage_t * dpb)
{

/* Variables */

    u32 tmpIdx;

/* Code */

    DEBUG_PRINT(("h264bsdDpbOutputPicture: index %d outnum %d\n",
            (dpb->numOut -
               ((dpb->outIndexW - dpb->outIndexR +
                 dpb->dpbSize + 1)%(dpb->dpbSize+1)), dpb->numOut)));
    if(dpb->numOut && !dpb->noOutput)
    {
        tmpIdx = dpb->outIndexR++;
        if (dpb->outIndexR == dpb->dpbSize + 1)
            dpb->outIndexR = 0;
        dpb->numOut--;
        dpb->prevOutIdx = dpb->outBuf[tmpIdx].memIdx;
        OutBufFree(dpb, tmpIdx);
        return (dpb->outBuf + tmpIdx);
    }
    else
        return (NULL);
}

/*------------------------------------------------------------------------------

    Function: h264bsdFlushDpb

        Functional description:
            Function to flush the DPB. Function puts all pictures needed for
            display into the output buffer. This function shall be called in
            the end of the stream to obtain pictures buffered for display
            re-ordering purposes.

------------------------------------------------------------------------------*/
void h264bsdFlushDpb(dpbStorage_t * dpb)
{

    if(dpb->delayedOut != 0)
    {
        DEBUG_PRINT(("h264bsdFlushDpb: Output all delayed pictures!\n"));
        dpb->delayedOut = 0;
        dpb->currentOut->toBeDisplayed = 0; /* remove it from output list */
    }

    dpb->flushed = 1;
    dpb->noOutput = 0;
    /* output all pictures */
    while(OutputPicture(dpb) == HANTRO_OK) ;

}

/*------------------------------------------------------------------------------

    Function: h264bsdFreeDpb

        Functional description:
            Function to free memories reserved for the DPB.

------------------------------------------------------------------------------*/

void h264bsdFreeDpb(const void *dwl, dpbStorage_t * dpb)
{

/* Variables */

    u32 i;

/* Code */

    ASSERT(dpb);

    for(i = 0; i < dpb->totBuffers; i++)
    {
        if(dpb->picBuffers[i].virtualAddress != NULL)
        {
            DWLFreeRefFrm(dwl, dpb->picBuffers+i);
        }
    }

    if(dpb->outBuf != NULL)
    {
        G1DWLfree(dpb->outBuf);
        dpb->outBuf = NULL;
    }

}

/*------------------------------------------------------------------------------

    Function: ShellSort

        Functional description:
            Sort pictures in the buffer. Function implements Shell's method,
            i.e. diminishing increment sort. See e.g. "Numerical Recipes in C"
            for more information.

------------------------------------------------------------------------------*/

void ShellSort(dpbStorage_t * dpb, u32 * list, u32 type, i32 par)
{
    u32 i, j;
    u32 step;
    u32 tmp;
    dpbPicture_t *pPic = dpb->buffer;
    u32 num = dpb->dpbSize + 1;

    step = 7;

    while(step)
    {
        for(i = step; i < num; i++)
        {
            tmp = list[i];
            j = i;
            while(j >= step &&
                  (type ?
                   ComparePicturesB(pPic + list[j - step], pPic + tmp,
                                    par) : ComparePictures(pPic + list[j -
                                                                       step],
                                                           pPic + tmp)) > 0)
            {
                list[j] = list[j - step];
                j -= step;
            }
            list[j] = tmp;
        }
        step >>= 1;
    }
}

/*------------------------------------------------------------------------------

    Function: ShellSortF

        Functional description:
            Sort pictures in the buffer. Function implements Shell's method,
            i.e. diminishing increment sort. See e.g. "Numerical Recipes in C"
            for more information.

------------------------------------------------------------------------------*/

void ShellSortF(dpbStorage_t * dpb, u32 * list, u32 type, i32 par)
{
    u32 i, j;
    u32 step;
    u32 tmp;
    dpbPicture_t *pPic = dpb->buffer;
    u32 num = dpb->dpbSize + 1;

    step = 7;

    while(step)
    {
        for(i = step; i < num; i++)
        {
            tmp = list[i];
            j = i;
            while(j >= step &&
                  (type ? CompareFieldsB(pPic + list[j - step], pPic + tmp, par)
                   : CompareFields(pPic + list[j - step], pPic + tmp)) > 0)
            {
                list[j] = list[j - step];
                j -= step;
            }
            list[j] = tmp;
        }
        step >>= 1;
    }
}

/* picture marked as unused and not to be displayed -> buffer is free for next
 * decoded pictures */
void DpbBufFree(dpbStorage_t *dpb, u32 i)
{

    dpb->numRefFrames--;

    if(!dpb->buffer[i].toBeDisplayed)
        dpb->fullness--;

}

/* picture removed from output list -> add to free buffers if not used for
 * reference anymore */
void OutBufFree(dpbStorage_t *dpb, u32 i)
{

    u32 idx = dpb->outBuf[i].memIdx;

    if (!DISPLAY_SMOOTHING)
        return;

    dpb->memStat[idx] &= ~MEM_STAT_OUT;
    if (!dpb->memStat[idx])
    {
        dpb->freeBuffers[dpb->numFreeBuffers++] = idx;
    }

}

/*------------------------------------------------------------------------------
    Function name   : h264DpbAdjStereoOutput
    Description     :
    Return type     : void
    Argument        : dpbStorage_t * dpb
    Argument        : const image_t * image
------------------------------------------------------------------------------*/
void h264DpbAdjStereoOutput(dpbStorage_t * dpb, u32 targetCount)
{

    while((dpb->numOut < targetCount) && (OutputPicture(dpb) == HANTRO_OK));

    if (dpb->numOut > targetCount)
    {
        dpb->numOut = targetCount;
        dpb->outIndexW = dpb->outIndexR + dpb->numOut;
        if (dpb->outIndexW > dpb->dpbSize)
            dpb->outIndexW -= dpb->dpbSize;
    }

}
