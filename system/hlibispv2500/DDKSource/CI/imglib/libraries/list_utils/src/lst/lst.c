/*__TITLE__(Lists - single linked queues)*/
/*
** FILE NAME:     lst.c
**
** PROJECT:       Meta/DSP Software Library
**
** AUTHOR:        Ensigma Technologies
**
** DESCRIPTION:   List processing primitives
**
*/

/* controls for inline compilation */
#define LST_COMPILE
#ifdef LST_INLINE
#ifndef LST_CINCLUDE
#undef LST_COMPILE /* only compile through the H file when inlining */
#endif
#endif

#ifdef LST_COMPILE

#ifndef LST_INLINE
#include "lst.h"  /* don't re-include H file if inlining */
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

LST_FQUALS void LST_add(LST_T *list, void *item)
{
    if (list->first == NULL)
    {
	list->first = item;
	list->last = item;
    }
    else
    {
	*(list->last) = item;   /* link to last item   */
	list->last = item;      /* update tail pointer */
    }
    *((void **)item) = NULL; /* terminate list      */
}
LST_FQUALS void LST_addHead(LST_T *list, void *item)
{
    if (list->first == NULL)
    {
	list->first = item;
	list->last = item;
    *((void **)item) = NULL; /* terminate list      */
    }
    else
    {
	*((void **)item) = list->first;
	list->first = item;   /* link to first item   */
//Original	*(list->first) = item;   /* link to first item   */
    }
}
LST_FQUALS int LST_empty(LST_T *list)
{
    return list->first == NULL;
}

LST_FQUALS void *LST_first(LST_T *list)
{
    return list->first;
}

LST_FQUALS void LST_init(LST_T *list)
{
    list->first = NULL;
    list->last = NULL;
}

LST_FQUALS void *LST_last(LST_T *list)
{
    return list->last;
}

LST_FQUALS void *LST_next(void *item)
{
    return *((void **)item);
}

LST_FQUALS void *LST_removeHead(LST_T *list)
{
    void **temp = list->first;
    if (temp != NULL)
    {
	if ((list->first = *temp) == NULL)
	    list->last = NULL;
    }
    return temp;
}

LST_FQUALS void LST_remove(LST_T *list, void *item)
{
    void **p;
    void **q;

    p = (void **)list;
    q = *p;
    while (q != NULL)
    {
	if (q == item)
	{
	    *p = *q; /* unlink item */
	    if (list->last == q)
	        list->last = p; /* update tail pointer when last item removed */
	    return; /* premature return if item flocated and removed */
	}
	p = q;
	q = *p;
    }
}

#endif



