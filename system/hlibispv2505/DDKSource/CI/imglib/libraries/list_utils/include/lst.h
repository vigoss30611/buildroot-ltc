
#ifndef LST_H
#define LST_H
/*__TITLE__(Module interface definition for lists) */
/*
* This is an automatically generated file. Do Not Edit.
*
* Interface Definition for module: lists
*
*/

#if defined (__cplusplus)
extern "C" {
#endif


#ifdef LST_INLINE
#define LST_FQUALS inline static
#else
#ifdef LST_FQUALS
#undef LSTFQUALS
#endif
#define LST_FQUALS 
#endif
	
/* Macro definitions */
	
#define  LST_LINK LST_LINKAGE_T LST_link
		
/* Type definitions */
	
typedef void **LST_LINKAGE_T
			 ; /* Private  */
typedef struct {
    void **first;
    void **last;

		}LST_T ; /* Anonymous */

/* Function Prototypes */
LST_FQUALS void LST_add(LST_T *list, void *item);
		LST_FQUALS void LST_addHead(LST_T *list, void *item);
		LST_FQUALS int LST_empty(LST_T *list);
		LST_FQUALS void * LST_first(LST_T *list);
		LST_FQUALS void LST_init(LST_T *list);
		LST_FQUALS void * LST_last(LST_T *list);
		LST_FQUALS void * LST_next(void *item);
		LST_FQUALS void LST_remove(LST_T *list, void *item);
		LST_FQUALS void * LST_removeHead(LST_T *list);
		

#if defined (__cplusplus)
}
#endif
 

#ifdef LST_INLINE
#define LST_CINCLUDE
#include "lst.c"
#endif



#endif /* #ifndef LST_H */

	
