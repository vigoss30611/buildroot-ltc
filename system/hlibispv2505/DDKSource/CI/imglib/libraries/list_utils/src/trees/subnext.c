/*
******************************************************************************
*
*               file : subnext.c
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
 ** FUNCTION:      TRE_subTreeNext
 **
 ** DESCRIPTION:   Remove leaf from subtree
 **
 ** RETURNS:       void *
 */

void           *
TRE_subTreeNext(void *subTreeRootNode, void *node, TRE_T * tree)
{
    TRE_LINKAGE_T  *next,
                   *parent,
                   *jtem;

    /*
     * if root, then can only return child: not siblings. 
     */
    if (node == subTreeRootNode)
	return (TRE_firstChild(subTreeRootNode));

    /*
     * Else scan the tree like a list and check ancestry. 
     */
    next = ((TRE_LINKAGE_T *) node)->next;
    if (!next)
	return (NULL);

    parent = next->parent;

    while (parent != subTreeRootNode) {
	if (parent == (TRE_LINKAGE_T *) tree) {	/* start checking ancestry 
						 * again with the next in
						 * the tree that is not of 
						 * this family */
	    while (NULL != (jtem = TRE_nextSibling(next)))
		next = jtem;

	    next = ((TRE_LINKAGE_T *) next)->next;
	    /*
	     * Break if list end 
	     */
	    if (next == NULL)
		return (NULL);
	    parent = ((TRE_LINKAGE_T *) next)->parent;
	} else {
	    parent = ((TRE_LINKAGE_T *) parent)->parent;
	}
    }

    /*
     * Have found next item in subtree 
     */
    return (next);
}
