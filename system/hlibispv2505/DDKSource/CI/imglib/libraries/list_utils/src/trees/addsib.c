/*
******************************************************************************
*
*               file : addsib.c
*             author : Imagination Technolgies
*          copyright : 2002,2005 by Imagination Technologies.
*                      All rights reserved.
*                      No part of this software, either material or conceptual
*                      may be copied or distributed, transmitted, transcribed,
*                      stored in a retrieval system or translated into any
*                      human or computer language in any form by any means,
*                      electronic, mechanical, manual or otherwise, or
*                      disclosed to third parties without the express written
*                      permission of:
*                        Imagination Technologies, Home Park Estate,
*                        Kings Langley, Hertfordshire, WD4 8LZ, U.K.
*
******************************************************************************
*/
/*
******************************************************************************
*/
//#include <metag/machine.inc>
#include "tre.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

/*
 ** FUNCTION:      TRE_addSibling
 **
 ** DESCRIPTION:   Adds children to the tree such that siblings are contiguous
 **                in the list and children are subsequent to their parents.
 **                Faster than TRE_addChild.
 **
 ** RETURNS:       void
 */

void
TRE_addSibling(void *newSibling, void *sibling)
{
    sibling = TRE_finalSibling(sibling);

    ((TRE_LINKAGE_T *) newSibling)->parent =
	((TRE_LINKAGE_T *) sibling)->parent;

    ((TRE_LINKAGE_T *) newSibling)->next =
	((TRE_LINKAGE_T *) sibling)->next;

    ((TRE_LINKAGE_T *) sibling)->next = newSibling;

    return;
}
