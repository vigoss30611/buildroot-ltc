/*
******************************************************************************
*
*               file : remlsub.c
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
 ** FUNCTION:      TRE_removeLeafFromSubTree
 **
 ** DESCRIPTION:   Remove leaf from subtree
 **
 ** RETURNS:       void *
 */

void           *
TRE_removeLeafFromSubTree(void *subTree, TRE_T * tree)
{

    TRE_LINKAGE_T  *subItem;

    subItem = subTree;

    /*
     * Catch a null 
     */
    if (subTree) {
	/*
	 * Step through to a leaf node 
	 */
	while (TRE_firstChild(subItem) != NULL) {
	    subItem = TRE_subTreeNext(subTree, subItem, tree);
	}

	TRE_removeLeaf(subItem, tree);
    }

    /*
     * Return the removed item 
     */
    return (subItem);

}
