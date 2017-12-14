/*
******************************************************************************
*
*               file : remleaf.c
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
 ** FUNCTION:      TRE_removeLeaf
 **
 ** DESCRIPTION:   Remove a leaf
 **
 ** RETURNS:       void
 */

void
TRE_removeLeaf(void *item, TRE_T * tree)
{
    TRE_LINKAGE_T  *jtem;

    jtem = tree->root;

    /*
     * catch remove of root 
     */
    if (jtem == item) {
	TRE_initTree(tree);
	return;
    }

    jtem = TRE_parent(item);
    while (jtem != NULL) {
	if (jtem->next == item) {
	    jtem->next = ((TRE_LINKAGE_T *) item)->next;
	    return;
	}
	jtem = jtem->next;
    }
}
