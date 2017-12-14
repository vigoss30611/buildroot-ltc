#ifndef _LST_LIB_H
#define _LST_LIB_H


#if defined( __cplusplus )
extern "C" {
#endif

typedef struct node		/* node of a linked list. */
{
	struct node* next;	/* Points at the next node in the list */
	struct node* previous;	/* Points at the previous node in the list */
} NODE;

typedef struct		/* Header for a linked list. */
{
	NODE node;	/* Header list node */
	int count;	/* Number of nodes in list */
} LIST;

#define LIST_FOR_EACH(type,var,list) for(var = (type*)lstFirst(list); var != NULL; var=(type*)lstNext(&var->node))

/* find first node in list */
NODE* lstFirst(LIST* pList);

/* delete and return the first node from a list */
NODE* lstGet(LIST* pList);

/* find last node in a list */
NODE* lstLast(LIST* pList);

/* find a list node nStep steps away from a specified node */
NODE* lstNStep(NODE* pNode, int nStep);

/* find the next node in a list */
NODE* lstNext(NODE* pNode);

/* find the Nth node in a list */
NODE* lstNth(LIST* pList, int nodenum);

/* find the previous node in a list */
NODE* lstPrevious(NODE* pNode);

/* report the number of nodes in a list */
int lstCount(LIST* pList);

/* find a node in a list */
int lstFind(LIST* pList, NODE* pNode);

/* add a node to the end of a list */
void lstAdd(LIST* pList, NODE* pNode);

/* concatenate two lists */
void lstConcat(LIST* pDstList, LIST* pAddList);

/* delete a specified node from a list */
void lstDelete(LIST* pList, NODE* pNode);

/* extract a sublist from a list */
void lstExtract(LIST* pSrcList, NODE* pStartNode, NODE* pEndNode, LIST* pDstList);

/* free up a list */
void lstFree(LIST* pList);

/* initializes a list descriptor */
void lstInit(LIST* pList);

/* insert a node in a list after a specified node */
void lstInsert(LIST* pList, NODE* pPrev, NODE* pNode);

#if defined( __cplusplus )
}
#endif


#endif /* end of _LST_LIB_H */
