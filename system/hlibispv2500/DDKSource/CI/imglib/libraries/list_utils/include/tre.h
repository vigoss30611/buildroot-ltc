/*
******************************************************************************
*
*               file : tre.h
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

#ifndef TRE_H
#define TRE_H

#if defined (__cplusplus)
extern "C" {
#endif


/* Macro definitions */
    
#define  TRE_LINK TRE_LINKAGE_T tre_link

/* Type definitions */
    

typedef struct TRE_linkage_tag TRE_LINKAGE_T ; /* Private  */

typedef struct TRE_tag TRE_T ; /* Anonymous */

typedef struct TRE_copy_tag TRE_COPY_T ; /* Anonymous */

/* Function Prototypes */
//#ifdef __STDC__
 void TRE_addChild(void *item, void *parent);
         void TRE_addSibling(void *item, void *sibling);
         static int TRE_areSiblings(void *item1, void *item2);
         void * TRE_copyNextInSubTree(TRE_COPY_T *copy, void *looseItem);
         void * TRE_finalSibling(void *item);
         void * TRE_firstChild(void *item);
         void * TRE_initCopySubTree(void *subTree, TRE_T *tree, void *destParent, TRE_T *destTree, TRE_COPY_T *copy);
         void TRE_initTree(TRE_T *tree);
         void * TRE_isInSubTree(void *subTree, void *item, TRE_T *tree);
         void * TRE_nextSibling(void *item);
         static void * TRE_parent(void *item);
         void * TRE_prevSibling(void *item);
         void TRE_removeLeaf(void *item, TRE_T *tree);
         void * TRE_removeLeafFromSubTree(void *subTree, TRE_T *tree);
         void * TRE_removeSubTree(void *item, TRE_T *tree);
         static void * TRE_root(TRE_T *tree);
         void * TRE_subTreeNext(void *subTree, void *item, TRE_T *tree);
        
//#endif /* __STDC__ */

/* complete the anonymous type definitions */
struct TRE_linkage_tag{
    void *next;
    void *parent;
};

struct TRE_tag {
    void *root;
    void *tip;
};

struct TRE_copy_tag {
    void *sourceRoot;
    void *destRoot;
    void *subRoot;
    void *destCurrent;
    void *sourceCurrent;
    void *sourcePrev;
    void *sourceTree;
};

#ifdef __GNUC__
#	define TRE_INLINE inline
#else
#	define TRE_INLINE
#endif

/* Some trivial functions are implemented as "inline static" for efficiency */
static TRE_INLINE void *TRE_root(TRE_T *tree)
{
    return tree->root;
}
static TRE_INLINE int TRE_areSiblings(void *t1, void *t2)
{
    return ((((TRE_LINKAGE_T *)t1)->parent == ((TRE_LINKAGE_T *)t2)->parent ) ? 1 : 0);
}
static TRE_INLINE void *TRE_parent(void *item)
{
    return (((TRE_LINKAGE_T *)item)->parent);
}

        
#if defined (__cplusplus)
}
#endif

#endif /* #ifndef TRE_H */
