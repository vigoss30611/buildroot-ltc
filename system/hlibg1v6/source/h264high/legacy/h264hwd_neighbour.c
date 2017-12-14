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
--  Abstract : Get neighbour blocks
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: h264hwd_neighbour.c,v $
--  $Date: 2008/03/13 12:48:06 $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Table of contents
   
     1. Include headers
     2. External compiler flags
     3. Module defines
     4. Local function prototypes
     5. Functions
          h264bsdInitMbNeighbours
          h264bsdGetNeighbourMb
          h264bsdNeighbour4x4BlockA
          h264bsdNeighbour4x4BlockB
          h264bsdNeighbour4x4BlockC
          h264bsdNeighbour4x4BlockD

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "h264hwd_neighbour.h"
#include "h264hwd_util.h"

/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/* Following four tables indicate neighbours of each block of a macroblock.
 * First 16 values are for luma blocks, next 4 values for Cb and last 4
 * values for Cr. Elements of the table indicate to which macroblock the
 * neighbour block belongs and the index of the neighbour block in question.
 * Indexing of the blocks goes as follows
 *
 *          Y             Cb       Cr
 *      0  1  4  5      16 17    20 21
 *      2  3  6  7      18 19    22 23
 *      8  9 12 13
 *     10 11 14 15
 */

/* left neighbour for each block */
static const neighbour_t N_A_4x4B[24] = {
    {MB_A,5},    {MB_CURR,0}, {MB_A,7},    {MB_CURR,2},
    {MB_CURR,1}, {MB_CURR,4}, {MB_CURR,3}, {MB_CURR,6},
    {MB_A,13},   {MB_CURR,8}, {MB_A,15},   {MB_CURR,10},
    {MB_CURR,9}, {MB_CURR,12},{MB_CURR,11},{MB_CURR,14},
    {MB_A,17},   {MB_CURR,16},{MB_A,19},   {MB_CURR,18},
    {MB_A,21},   {MB_CURR,20},{MB_A,23},   {MB_CURR,22} };

/* above neighbour for each block */
static const neighbour_t N_B_4x4B[24] = {
    {MB_B,10},   {MB_B,11},   {MB_CURR,0}, {MB_CURR,1},
    {MB_B,14},   {MB_B,15},   {MB_CURR,4}, {MB_CURR,5},
    {MB_CURR,2}, {MB_CURR,3}, {MB_CURR,8}, {MB_CURR,9},
    {MB_CURR,6}, {MB_CURR,7}, {MB_CURR,12},{MB_CURR,13},
    {MB_B,18},   {MB_B,19},   {MB_CURR,16},{MB_CURR,17},
    {MB_B,22},   {MB_B,23},   {MB_CURR,20},{MB_CURR,21} };

#if 0 /* not in use currently */
/* above-right neighbour for each block */
static const neighbour_t N_C_4x4B[24] = {
    {MB_B,11},   {MB_B,14},   {MB_CURR,1}, {MB_NA,4},
    {MB_B,15},   {MB_C,10},   {MB_CURR,5}, {MB_NA,0},
    {MB_CURR,3}, {MB_CURR,6}, {MB_CURR,9}, {MB_NA,12},
    {MB_CURR,7}, {MB_NA,2},   {MB_CURR,13},{MB_NA,8},
    {MB_B,19},   {MB_C,18},   {MB_CURR,17},{MB_NA,16},
    {MB_B,23},   {MB_C,22},   {MB_CURR,21},{MB_NA,20} };
#endif

/* above-left neighbour for each block */
static const neighbour_t N_D_4x4B[24] = {
    {MB_D,15},   {MB_B,10},   {MB_A,5},    {MB_CURR,0},
    {MB_B,11},   {MB_B,14},   {MB_CURR,1}, {MB_CURR,4},
    {MB_A,7},    {MB_CURR,2}, {MB_A,13},   {MB_CURR,8},
    {MB_CURR,3}, {MB_CURR,6}, {MB_CURR,9}, {MB_CURR,12},
    {MB_D,19},   {MB_B,18},   {MB_A,17},   {MB_CURR,16},
    {MB_D,23},   {MB_B,22},   {MB_A,21},   {MB_CURR,20} };

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Function: h264bsdInitMbNeighbours

        Functional description:
            Initialize macroblock neighbours. Function sets neighbour
            macroblock pointers in macroblock structures to point to
            macroblocks on the left, above, above-right and above-left.
            Pointers are set NULL if the neighbour does not fit into the
            picture.
          
        Inputs:
            picWidth        width of the picture in macroblocks
            picSizeInMbs    no need to clarify
          
        Outputs:
            pMbStorage      neighbour pointers of each mbStorage structure
                            stored here

        Returns:
            none

------------------------------------------------------------------------------*/

void h264bsdInitMbNeighbours(mbStorage_t *pMbStorage, u32 picWidth,
    u32 picSizeInMbs)
{

/* Variables */

    u32 i, row, col;

/* Code */

    ASSERT(pMbStorage);
    ASSERT(picWidth);
    ASSERT(picWidth <= picSizeInMbs);
    ASSERT(((picSizeInMbs / picWidth) * picWidth) == picSizeInMbs);

    row = col = 0;

    for (i = 0; i < picSizeInMbs; i++)
    {

        if (col)
            pMbStorage[i].mbA = pMbStorage + i - 1;
        else
            pMbStorage[i].mbA = NULL;

        if (row)
            pMbStorage[i].mbB = pMbStorage + i - picWidth;
        else
            pMbStorage[i].mbB = NULL;

        if (row && (col < picWidth - 1))
            pMbStorage[i].mbC = pMbStorage + i - (picWidth - 1);
        else
            pMbStorage[i].mbC = NULL;

        if (row && col)
            pMbStorage[i].mbD = pMbStorage + i - (picWidth + 1);
        else
            pMbStorage[i].mbD = NULL;

        col++;
        if (col == picWidth)
        {
            col = 0;
            row++;
        }
    }

}

/*------------------------------------------------------------------------------

    Function: h264bsdGetNeighbourMb

        Functional description:
            Get pointer to neighbour macroblock.
          
        Inputs:
            pMb         pointer to macroblock structure of the macroblock
                        whose neighbour is wanted
            neighbour   indicates which neighbour is wanted
          
        Outputs:
            none

        Returns:
            pointer to neighbour macroblock
            NULL if not available

------------------------------------------------------------------------------*/

mbStorage_t* h264bsdGetNeighbourMb(mbStorage_t *pMb, neighbourMb_e neighbour)
{

/* Variables */


/* Code */

    ASSERT((neighbour <= MB_CURR) || (neighbour == MB_NA));

    if (neighbour == MB_A)
        return(pMb->mbA);
    else if (neighbour == MB_B)
        return(pMb->mbB);
    else if (neighbour == MB_D)
        return(pMb->mbD);
    else if (neighbour == MB_CURR)
        return(pMb);
    else if (neighbour == MB_C)
        return(pMb->mbC);
    else
        return(NULL);

}

/*------------------------------------------------------------------------------

    Function: h264bsdNeighbour4x4BlockA

        Functional description:
            Get left neighbour of the block. Function returns pointer to
            the table defined in the beginning of the file.
          
        Inputs:
            blockIndex  indicates the block whose neighbours are wanted
          
        Outputs:
        
        Returns:
            pointer to neighbour structure

------------------------------------------------------------------------------*/

const neighbour_t* h264bsdNeighbour4x4BlockA(u32 blockIndex)
{

/* Variables */

/* Code */

    ASSERT(blockIndex < 24);

    return(N_A_4x4B+blockIndex);

}

/*------------------------------------------------------------------------------

    Function: h264bsdNeighbour4x4BlockB

        Functional description:
            Get above neighbour of the block. Function returns pointer to
            the table defined in the beginning of the file.
          
        Inputs:
            blockIndex  indicates the block whose neighbours are wanted
          
        Outputs:
        
        Returns:
            pointer to neighbour structure

------------------------------------------------------------------------------*/

const neighbour_t* h264bsdNeighbour4x4BlockB(u32 blockIndex)
{

/* Variables */

/* Code */

    ASSERT(blockIndex < 24);

    return(N_B_4x4B+blockIndex);

}

/*------------------------------------------------------------------------------

    Function: h264bsdNeighbour4x4BlockC

        Functional description:
            Get above-right  neighbour of the block. Function returns pointer
            to the table defined in the beginning of the file.
          
        Inputs:
            blockIndex  indicates the block whose neighbours are wanted
          
        Outputs:
        
        Returns:
            pointer to neighbour structure

------------------------------------------------------------------------------*/
#if 0 /* not used */
const neighbour_t* h264bsdNeighbour4x4BlockC(u32 blockIndex)
{

/* Variables */

/* Code */

    ASSERT(blockIndex < 24);

    return(N_C_4x4B+blockIndex);

}
#endif

/*------------------------------------------------------------------------------

    Function: h264bsdNeighbour4x4BlockD

        Functional description:
            Get above-left neighbour of the block. Function returns pointer to
            the table defined in the beginning of the file.
          
        Inputs:
            blockIndex  indicates the block whose neighbours are wanted
          
        Outputs:
        
        Returns:
            pointer to neighbour structure

------------------------------------------------------------------------------*/

const neighbour_t* h264bsdNeighbour4x4BlockD(u32 blockIndex)
{

/* Variables */

/* Code */
    
    ASSERT(blockIndex < 24);

    return(N_D_4x4B+blockIndex);

}

/*------------------------------------------------------------------------------

    Function: h264bsdIsNeighbourAvailable

        Functional description:
            Check if neighbour macroblock is available. Neighbour macroblock
            is considered available if it is within the picture and belongs
            to the same slice as the current macroblock.
          
        Inputs:
            pMb         pointer to the current macroblock
            pNeighbour  pointer to the neighbour macroblock
          
        Outputs:
            none

        Returns:
            HANTRO_TRUE    neighbour is available
            HANTRO_FALSE   neighbour is not available

------------------------------------------------------------------------------*/

u32 h264bsdIsNeighbourAvailable(mbStorage_t *pMb, mbStorage_t *pNeighbour)
{

/* Variables */

/* Code */

    if ( (pNeighbour == NULL) || (pMb->sliceId != pNeighbour->sliceId) )
        return(HANTRO_FALSE);
    else
        return(HANTRO_TRUE);

}
