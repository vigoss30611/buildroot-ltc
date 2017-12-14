
/*__TITLE__(dqueues module) */
/*
** FILE NAME:     dq.c
**
** PROJECT:       Meta/DSP Software Library
**
** AUTHOR:        Ensigma Technologies
**
** DESCRIPTION:   Utility module for doubly linked queues
**
*/
/* controls for inline compilation */
#define DQ_COMPILE
#ifdef DQ_INLINE
#ifndef DQ_CINCLUDE
#undef DQ_COMPILE /* only compile through the H file when inlining */
#endif
#endif

#include "img_defs.h"

#ifdef DQ_COMPILE

#ifndef DQ_INLINE
#include "dq.h"  /* don't re-include H file if inlining */
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#define DQ_EMPTY(Q) ((Q)->DQ_link.fwd == (DQ_LINKAGE_T *)(Q))

DQ_FQUALS void DQ_init(DQ_T *queue)
{
    queue->DQ_link.fwd = (DQ_LINKAGE_T *)queue;
    queue->DQ_link.back= (DQ_LINKAGE_T *)queue;
}

DQ_FQUALS void DQ_addHead(DQ_T *queue, void *item)
{
    IMG_ASSERT(((DQ_LINKAGE_T *)queue)->back != NULL);
    IMG_ASSERT(((DQ_LINKAGE_T *)queue)->fwd != NULL);

    ((DQ_LINKAGE_T *)item)->back = (DQ_LINKAGE_T *)queue;
    ((DQ_LINKAGE_T *)item)->fwd = ((DQ_LINKAGE_T *)queue)->fwd;
    ((DQ_LINKAGE_T *)queue)->fwd->back = (DQ_LINKAGE_T *)item;
    ((DQ_LINKAGE_T *)queue)->fwd = (DQ_LINKAGE_T *)item;
}

DQ_FQUALS void DQ_addTail(DQ_T *queue, void *item)
{
    IMG_ASSERT(((DQ_LINKAGE_T *)queue)->back != NULL);
    IMG_ASSERT(((DQ_LINKAGE_T *)queue)->fwd != NULL);

    ((DQ_LINKAGE_T *)item)->fwd = (DQ_LINKAGE_T *)queue;
    ((DQ_LINKAGE_T *)item)->back = ((DQ_LINKAGE_T *)queue)->back;
    ((DQ_LINKAGE_T *)queue)->back->fwd = (DQ_LINKAGE_T *)item;
    ((DQ_LINKAGE_T *)queue)->back = (DQ_LINKAGE_T *)item;
}


DQ_FQUALS int DQ_empty(DQ_T *queue)
{
    IMG_ASSERT(((DQ_LINKAGE_T *)queue)->back != NULL);
    IMG_ASSERT(((DQ_LINKAGE_T *)queue)->fwd != NULL);

    return DQ_EMPTY(queue);
}

DQ_FQUALS void *DQ_first(DQ_T *queue)
{
    DQ_LINKAGE_T *temp = queue->DQ_link.fwd;

    IMG_ASSERT(((DQ_LINKAGE_T *)queue)->back != NULL);
    IMG_ASSERT(((DQ_LINKAGE_T *)queue)->fwd != NULL);

    return temp == (DQ_LINKAGE_T *)queue ? NULL : temp;
}

DQ_FQUALS void *DQ_last(DQ_T *queue)
{
    DQ_LINKAGE_T *temp = queue->DQ_link.back;

    IMG_ASSERT(((DQ_LINKAGE_T *)queue)->back != NULL);
    IMG_ASSERT(((DQ_LINKAGE_T *)queue)->fwd != NULL);

    return temp == (DQ_LINKAGE_T *)queue ? NULL : temp;
}

DQ_FQUALS void *DQ_next(void *item)
{
    IMG_ASSERT(((DQ_LINKAGE_T *)item)->back != NULL);
    IMG_ASSERT(((DQ_LINKAGE_T *)item)->fwd != NULL);

    return ((DQ_LINKAGE_T *)item)->fwd;
}

DQ_FQUALS void *DQ_previous(void *item)
{
    IMG_ASSERT(((DQ_LINKAGE_T *)item)->back != NULL);
    IMG_ASSERT(((DQ_LINKAGE_T *)item)->fwd != NULL);

    return ((DQ_LINKAGE_T *)item)->back;
}

DQ_FQUALS void DQ_remove(void *item)
{
    IMG_ASSERT(((DQ_LINKAGE_T *)item)->back != NULL);
    IMG_ASSERT(((DQ_LINKAGE_T *)item)->fwd != NULL);

    ((DQ_LINKAGE_T *)item)->fwd->back = ((DQ_LINKAGE_T *)item)->back;
    ((DQ_LINKAGE_T *)item)->back->fwd = ((DQ_LINKAGE_T *)item)->fwd;

    /* make item linkages safe for "orphan" removes */
    ((DQ_LINKAGE_T *)item)->fwd = item;
    ((DQ_LINKAGE_T *)item)->back = item;
}

DQ_FQUALS void *DQ_removeHead(DQ_T *queue)
{
    DQ_LINKAGE_T *temp;
    IMG_ASSERT(((DQ_LINKAGE_T *)queue)->back != NULL);
    IMG_ASSERT(((DQ_LINKAGE_T *)queue)->fwd != NULL);

    if (DQ_EMPTY(queue))
        return NULL;

    temp = ((DQ_LINKAGE_T *)queue)->fwd;
    temp->fwd->back = temp->back;
    temp->back->fwd = temp->fwd;

    /* make item linkages safe for "orphan" removes */
    temp->fwd = temp;
    temp->back = temp;
    return temp;
}

DQ_FQUALS void *DQ_removeTail(DQ_T *queue)
{
    DQ_LINKAGE_T *temp;
    IMG_ASSERT(((DQ_LINKAGE_T *)queue)->back != NULL);
    IMG_ASSERT(((DQ_LINKAGE_T *)queue)->fwd != NULL);

    if (DQ_EMPTY(queue))
        return NULL;

    temp = ((DQ_LINKAGE_T *)queue)->back;
    temp->fwd->back = temp->back;
    temp->back->fwd = temp->fwd;

    /* make item linkages safe for "orphan" removes */
    temp->fwd = temp;
    temp->back = temp;

    return temp;
}

DQ_FQUALS void DQ_addBefore(void *successor, void *item)
{
    IMG_ASSERT(((DQ_LINKAGE_T *)successor)->back != NULL);
    IMG_ASSERT(((DQ_LINKAGE_T *)successor)->fwd != NULL);

    ((DQ_LINKAGE_T *)item)->fwd = (DQ_LINKAGE_T *)successor;
    ((DQ_LINKAGE_T *)item)->back = ((DQ_LINKAGE_T *)successor)->back;
    ((DQ_LINKAGE_T *)item)->back->fwd = (DQ_LINKAGE_T *)item;
    ((DQ_LINKAGE_T *)successor)->back = (DQ_LINKAGE_T *)item;
}

DQ_FQUALS void DQ_addAfter(void *predecessor, void *item)
{
    IMG_ASSERT(((DQ_LINKAGE_T *)predecessor)->back != NULL);
    IMG_ASSERT(((DQ_LINKAGE_T *)predecessor)->fwd != NULL);

    ((DQ_LINKAGE_T *)item)->fwd = ((DQ_LINKAGE_T *)predecessor)->fwd;
    ((DQ_LINKAGE_T *)item)->back = (DQ_LINKAGE_T *)predecessor;
    ((DQ_LINKAGE_T *)item)->fwd->back = (DQ_LINKAGE_T *)item;
    ((DQ_LINKAGE_T *)predecessor)->fwd = (DQ_LINKAGE_T *)item;
}

DQ_FQUALS void DQ_move(DQ_T *from, DQ_T *to)
{
    IMG_ASSERT(((DQ_LINKAGE_T *)from)->back != NULL);
    IMG_ASSERT(((DQ_LINKAGE_T *)from)->fwd != NULL);
    IMG_ASSERT(((DQ_LINKAGE_T *)to)->back != NULL);
    IMG_ASSERT(((DQ_LINKAGE_T *)to)->fwd != NULL);

    if (DQ_EMPTY(from))
        DQ_init(to);
    else
    {
        *to = *from;
        to->DQ_link.fwd->back = (DQ_LINKAGE_T *)to;
        to->DQ_link.back->fwd = (DQ_LINKAGE_T *)to;
        DQ_init(from);
    }
}

#endif

