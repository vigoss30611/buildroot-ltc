/*
******************************************************************************
*
*               file : addchild.c
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
 ** FUNCTION:      TRE_addChild
 **
 ** DESCRIPTION:   Adds children to the tree such that siblings are contiguous
 **                in the list and children are subsequent to their parents.
 **                Overloaded to add as root if tree object is passed in.
 **
 ** RETURNS:       void
 */

void
TRE_addChild(void *item, void *parent)
{
    TRE_LINKAGE_T  *addBefore;

    /*
     * root item points to the tree object 
     */
    ((TRE_LINKAGE_T *) item)->parent = parent;

    /*
     * Add as final sibling if there are any other siblings, otherwise at
     * end of the whole list. 
     */
    while ((addBefore = ((TRE_LINKAGE_T *) parent)->next) != NULL) {
	if (TRE_areSiblings(addBefore, item)) {
	    parent = TRE_finalSibling(addBefore);
	    break;
	}
	parent = addBefore;
    }

    /*
     * Insert the item 
     */
    ((TRE_LINKAGE_T *) item)->next = ((TRE_LINKAGE_T *) parent)->next;	/* root 
									 * starts 
									 * with 
									 * null 
									 * next 
									 */
    ((TRE_LINKAGE_T *) parent)->next = item;	/* root is also tree tip */

    return;
}
