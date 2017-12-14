/*
******************************************************************************
*
*               file : remsub.c
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
 ** FUNCTION:      TRE_removeSubTree
 **
 ** DESCRIPTION:   Remove a subtree
 **
 ** RETURNS:       void *
 */

void           *
TRE_removeSubTree(void *item, TRE_T * sourceTree)
{

    TRE_LINKAGE_T  *removeBefore,
                   *subItem,
                   *lastItem;

    /*
     * Find previous item, whose linkage must be changed. 
     */
    removeBefore = TRE_parent(item);

    while (removeBefore->next != item)
	removeBefore = removeBefore->next;

    /*
     * Step through the tree patching the links that need to be patched 
     */
    subItem = item;
    lastItem = item;

    while (NULL != (subItem = subItem->next)) {
	if (!TRE_isInSubTree(item, subItem, sourceTree)) {
	    removeBefore->next = subItem;
	    removeBefore = subItem;

	} else
	    lastItem = subItem;
    }

    removeBefore->next = NULL;

    /*
     * Return the final member of the subTree 
     */
    return (lastItem);
}
