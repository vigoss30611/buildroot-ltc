/*
******************************************************************************
*
*               file : copysub.c
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
 ** FUNCTION:      _TRE_subTreeSteps
 **
 ** DESCRIPTION:   Counts the number of steps within the underlying list structure
 **                from a to b counting only those objects that lie within the subtree.
 **                Strictly private function
 **
 ** RETURNS:       int
 */

static int
_TRE_subTreeSteps(void *a, void *b, TRE_T * tree)
{
    void           *next;
    int             steps = -1;

    next = a;

    do {
	steps++;

	if (next == b)
	    return (steps);

    } while (NULL != (next = TRE_subTreeNext(a, next, tree)));

    return (-1);
}
/*
 ** FUNCTION:      TRE_initCopySubTree
 **
 ** DESCRIPTION:   Prepare for subtree copy
 **
 ** RETURNS:       void *
 */
void           *
TRE_initCopySubTree(void *subTree, TRE_T * tree, void *destSubTree,
		    TRE_T * destTree, TRE_COPY_T * copy)
{
    copy->sourceRoot = subTree;
    copy->destRoot = destSubTree;
    copy->destCurrent = destSubTree;
    copy->sourceCurrent = subTree;
    copy->sourcePrev = NULL;
    copy->sourceTree = tree;

    /*
     * ascertain that the destination is not a subtree of the source 
     */

    if ((TRE_isInSubTree(subTree, destSubTree, destTree)))
	return (NULL);

    return (copy->sourceRoot);
}


/*
 ** FUNCTION:      TRE_copyNextInSubTree
 **
 ** DESCRIPTION:   Copy one item of a sub-tree
 **
 ** RETURNS:       void *
 */
void           *
TRE_copyNextInSubTree(TRE_COPY_T * copy, void *looseItem)
{
    int             steps;

    if (!copy->sourcePrev) {
	TRE_addChild(looseItem, copy->destRoot);
	copy->subRoot = looseItem;	/* Log this item for later */
    } else if (TRE_areSiblings(copy->sourceCurrent, copy->sourcePrev)) {
	TRE_addSibling(looseItem, copy->destCurrent);
    } else {
	/*
	 * must link to correct parent in new subtree: do this by
	 * exploiting isomorphism in those parts of the source and
	 * destination subtrees already copied so far. 
	 */
	steps = _TRE_subTreeSteps(copy->sourceRoot,
				  TRE_parent(copy->sourceCurrent),
				  copy->sourceTree);

	{
	    void           *a;
	    int             i;

	    a = copy->subRoot;
	    for (i = 0; i < steps; i++)
		a = TRE_subTreeNext(copy->subRoot, a, copy->sourceTree);
	    TRE_addChild(looseItem, a);
	}
    }

    /*
     * Increment the source and destination item pointers 
     */
    copy->sourcePrev = copy->sourceCurrent;
    copy->sourceCurrent = TRE_subTreeNext(copy->sourceRoot,
					  copy->sourceCurrent,
					  copy->sourceTree);
    copy->destCurrent = looseItem;

    /*
     * return the next source item to copy or NULL if it is time to stop. 
     */
    return (copy->sourceCurrent);
}
