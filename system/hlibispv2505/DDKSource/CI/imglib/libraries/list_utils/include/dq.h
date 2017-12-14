
#ifndef DQ_H
#define DQ_H
/*__TITLE__(Module interface definition for dqueues) */
/*
* This is an automatically generated file. Do Not Edit.
*
* Interface Definition for module: dqueues
*
*/


#if defined (__cplusplus)
extern "C" {
#endif

#ifdef DQ_INLINE
#define DQ_FQUALS inline static
#else
#ifdef DQ_FQUALS
#undef DQ_FQUALS
#endif
#define DQ_FQUALS 
#endif
	
/* Macro definitions */
	
#define  DQ_LINK DQ_LINKAGE_T DQ_link
		
/* Type definitions */
	
typedef struct DQ_tag { 
    struct DQ_tag *fwd;
    struct DQ_tag *back;
}DQ_LINKAGE_T ; /* Private  */
typedef struct {
    DQ_LINK;
}DQ_T ; /* Anonymous */

/* Function Prototypes */
DQ_FQUALS void DQ_addAfter(void *predecessor, void *item);
		DQ_FQUALS void DQ_addBefore(void *successor, void *item);
		DQ_FQUALS void DQ_addHead(DQ_T *queue, void *item);
		DQ_FQUALS void DQ_addTail(DQ_T *queue, void *item);
		DQ_FQUALS int DQ_empty(DQ_T *queue);
		DQ_FQUALS void * DQ_first(DQ_T *queue);
		DQ_FQUALS void * DQ_last(DQ_T *queue);
		DQ_FQUALS void DQ_init(DQ_T *queue);
		DQ_FQUALS void DQ_move(DQ_T *from, DQ_T *to);
		DQ_FQUALS void * DQ_next(void *item);
		DQ_FQUALS void * DQ_previous(void *item);
		DQ_FQUALS void DQ_remove(void *item);
		DQ_FQUALS void * DQ_removeHead(DQ_T *queue);
		DQ_FQUALS void * DQ_removeTail(DQ_T *queue);

#if defined (__cplusplus)
}
#endif
 

#ifdef DQ_INLINE
#define DQ_CINCLUDE
#include "dq.c"
#undef DQ_CINCLUDE
#endif



#endif /* #ifndef DQ_H */


