/*
******************************************************************************
*
*               file : isinsub.c
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
 ** FUNCTION:      TRE_isInSubTree
 **
 ** DESCRIPTION:   Tests whether item is in a subtree
 **
 ** RETURNS:       void *
 */

void           *
TRE_isInSubTree(void *subTree, void *item, TRE_T * tree)
{
    while (item != subTree) {
	if (!item)
	    return (NULL);

	item = TRE_parent(item);

	if (item == tree)
	    return (NULL);
    }
    return (item);
}
