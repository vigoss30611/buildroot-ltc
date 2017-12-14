/***************************************************************************** 
** 
** Copyright (c) 2012~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
**      
** Revision History: 
** ----------------- 
** v1.0.1	leo@2012/03/15: first commit.
** v1.4.0	leo@2012/08/13: add (index < 0) check in im_list_get_index().
** v2.0.1	larry@2012/08/15: add klist support.
**
*****************************************************************************/ 

#include <InfotmMedia.h>


//
//=============================================================================
//
typedef struct _node_t{
	void 		*me;
	struct _node_t 	*prev;
	struct _node_t 	*next;
}node_t;

typedef struct{
	node_t 		*head;
	node_t 		*tail;
	node_t 		*cur;
	IM_INT32 	length;
	IM_INT32	index;
	IM_BOOL		cpdata;

	im_mempool_handle_t	mpl;
	IM_INT32	dataSize;
}im_list_internal_t;


im_list_handle_t im_list_init(IN IM_INT32 dataSize, IN im_mempool_handle_t mpl)
{
	im_list_internal_t *lst;
	
	lst = (im_list_internal_t *)im_mpool_alloc(mpl, sizeof(im_list_internal_t));
	if(lst == IM_NULL){
		return IM_NULL;
	}

	lst->head = lst->tail = lst->cur = IM_NULL;
	lst->length = 0;
	lst->index = 0;
	lst->mpl = mpl;
	lst->dataSize = dataSize;
	lst->cpdata = (dataSize == 0)?IM_FALSE:IM_TRUE;
	return (im_list_handle_t)lst;
}

IM_RET im_list_deinit(IN im_list_handle_t hlst)
{
	im_list_clear(hlst);
	im_mpool_free(((im_list_internal_t *)hlst)->mpl, (void *)hlst);
	return IM_RET_OK;
}

IM_BOOL im_list_empty(IN im_list_handle_t hlst)
{
	return (((im_list_internal_t *)hlst)->length == 0) ? IM_TRUE : IM_FALSE;
}

IM_INT32 im_list_size(IN im_list_handle_t hlst)
{
	return ((im_list_internal_t *)hlst)->length;
}

void * im_list_get(IN im_list_handle_t hlst)
{
	if(((im_list_internal_t *)hlst)->cur != IM_NULL){
		return ((im_list_internal_t *)hlst)->cur->me;
	}
	return IM_NULL;
}

void * im_list_get_index(IN im_list_handle_t hlst, IM_INT32 index)
{
	im_list_internal_t *lst = (im_list_internal_t *)hlst;
	if((lst->length == 0) || (index >= lst->length) || (index < 0)){
		return IM_NULL;
	}

	if(lst->cur == IM_NULL){
		lst->cur = lst->tail;
		lst->index = lst->length - 1;
	}

	if(index == 0){
		lst->cur = lst->head;
		lst->index = 0;
	}else{
		if(lst->index > index){
			do{
				lst->cur = lst->cur->prev;
				lst->index--;
			}while(lst->index > index);
		}else if(lst->index < index){
			do{
				lst->cur = lst->cur->next;
				lst->index++;
			}while(lst->index < index);
		}
		// if index==lst->index, do nothing.
	}

	return lst->cur->me;
}

void * im_list_begin(IN im_list_handle_t hlst)
{
	im_list_internal_t *lst = (im_list_internal_t *)hlst;
	if(lst->head != IM_NULL){
		lst->cur = lst->head;
		lst->index = 0;
		return lst->cur->me;
	}
	return IM_NULL;
}

void * im_list_next(IN im_list_handle_t hlst)
{
	im_list_internal_t *lst = (im_list_internal_t *)hlst;
	if(lst->cur != IM_NULL){
		if(lst->cur->next != IM_NULL){
			lst->cur = lst->cur->next;
			lst->index++;
			return lst->cur->me;
		}
	}
	return IM_NULL;
}

void * im_list_prev(IN im_list_handle_t hlst)
{
	im_list_internal_t *lst = (im_list_internal_t *)hlst;
	if(lst->cur != IM_NULL){
		if(lst->cur->prev != IM_NULL){
			lst->cur = lst->cur->prev;
			lst->index--;
			return lst->cur->me;
		}
	}
	return IM_NULL;
}

void * im_list_end(IN im_list_handle_t hlst)
{
	im_list_internal_t *lst = (im_list_internal_t *)hlst;
	if(lst->tail != IM_NULL){
		lst->cur = lst->tail;
		lst->index = lst->length - 1;
		return lst->cur->me;
	}
	return IM_NULL;
}

IM_RET im_list_put_front(IN im_list_handle_t hlst, IN void *data)
{
	im_list_internal_t *lst = (im_list_internal_t *)hlst;

	node_t *tmp = (node_t *)im_mpool_alloc(lst->mpl, sizeof(node_t));
	if(tmp == IM_NULL){
		return IM_RET_NOMEMORY;
	}

	if(lst->cpdata){
		tmp->me = im_mpool_alloc(lst->mpl, lst->dataSize);
		if(tmp->me == IM_NULL){
			im_mpool_free(lst->mpl, (void *)tmp);
			return IM_RET_NOMEMORY;
		}
		memcpy(tmp->me, data, lst->dataSize);
	}else{
		tmp->me = data;
	}

	if(lst->head == IM_NULL){
		lst->head = tmp;
		lst->head->prev = IM_NULL;
		lst->head->next = IM_NULL;
		lst->tail = lst->head;
		lst->index = 0;
	}else{
		tmp->next = lst->head;
		tmp->prev = IM_NULL;
		lst->head->prev = tmp;
		lst->head = tmp;
		lst->index++;
	}

	lst->cur = lst->head;
	lst->length++;
	return IM_RET_OK;
}

IM_RET im_list_put_back(IN im_list_handle_t hlst, IN void *data)
{
	im_list_internal_t *lst = (im_list_internal_t *)hlst;

	node_t *tmp = (node_t *)im_mpool_alloc(lst->mpl, sizeof(node_t));
	if(tmp == IM_NULL){
		return IM_RET_NOMEMORY;
	}

	if(lst->cpdata){	
		tmp->me = im_mpool_alloc(lst->mpl, lst->dataSize);
		if(tmp->me == IM_NULL){
			im_mpool_free(lst->mpl, (void *)tmp);
			return IM_RET_NOMEMORY;
		}
		memcpy(tmp->me, data, lst->dataSize);
	}else{
		tmp->me = data;
	}

	if(lst->tail == IM_NULL){
		lst->head = tmp;
		lst->head->prev = IM_NULL;
		lst->head->next = IM_NULL;
		lst->tail = lst->head;
	}else{
		tmp->prev = lst->tail;
		tmp->next = IM_NULL;
		lst->tail->next = tmp;
		lst->tail = tmp;
	}

	lst->length++;
	lst->cur = lst->tail;
	lst->index = lst->length - 1;
	return IM_RET_OK;
}

IM_RET im_list_insert(IN im_list_handle_t hlst, IN void *data)
{
	node_t *tmp;
	im_list_internal_t *lst = (im_list_internal_t *)hlst;

	if((lst->cur == IM_NULL) || (lst->cur == lst->tail)){
		return im_list_put_back(hlst, data);
	}

	tmp = (node_t *)im_mpool_alloc(lst->mpl, sizeof(node_t));
	if(tmp == IM_NULL){
		return IM_RET_NOMEMORY;
	}
	
	if(lst->cpdata){	
		tmp->me = im_mpool_alloc(lst->mpl, lst->dataSize);
		if(tmp->me == IM_NULL){
			im_mpool_free(lst->mpl, (void *)tmp);
			return IM_RET_NOMEMORY;
		}
		memcpy(tmp->me, data, lst->dataSize);
	}else{
		tmp->me = data;
	}

	lst->cur->next->prev = tmp;
	tmp->next = lst->cur->next;
	tmp->prev = lst->cur;
	lst->cur->next = tmp;
	lst->cur = tmp;
	lst->index++;
	lst->length++;
	return IM_RET_OK;
}

void * im_list_erase(IN im_list_handle_t hlst, IN void *data)
{
	IM_INT32 i;
	im_list_internal_t *lst = (im_list_internal_t *)hlst;
	node_t *tmp = lst->head;

	for(i=0; i<lst->length; i++)
	{
		if(tmp->me == data){	// found it.
			if(tmp == lst->head){
				lst->head = tmp->next;
				if(lst->head == IM_NULL){	// just 1 element, head=tail=cur
					lst->tail = lst->cur = IM_NULL;
				}else{
					lst->head->prev = IM_NULL;
					lst->cur = lst->head;
				}
			}else if(tmp == lst->tail){
				lst->tail = tmp->prev;
				lst->tail->next = IM_NULL;
				lst->cur = IM_NULL;
			}else{
				tmp->prev->next = tmp->next;
				tmp->next->prev = tmp->prev;
				lst->cur = tmp->next;
			}

			if(lst->cpdata){	
				im_mpool_free(lst->mpl, tmp->me);
			}
			im_mpool_free(lst->mpl, (void *)tmp);
			lst->length--;
			if(lst->cur != IM_NULL){
				return lst->cur->me;
			}
			return IM_NULL;
		}else{
			tmp = tmp->next;
		}
	}
	return IM_NULL;
}

void im_list_clear(IN im_list_handle_t hlst)
{
	void *tmp;
	im_list_internal_t *lst = (im_list_internal_t *)hlst;
	if(lst->length == 0){
		return;
	}

	tmp = lst->head->me;
	while(tmp != IM_NULL){
		tmp = im_list_erase(hlst, tmp);
	}
}

//
//=============================================================================
//
typedef struct _knode_t{
	void 		*me;
	IM_UINT32	key;
	IM_INT32	bf;		//balance factor
	struct _knode_t	*p;	// parent.
	struct _knode_t *l;	// left.
	struct _knode_t *r;	// right.
}knode_t;

typedef struct{
	knode_t 	*head;
	knode_t 	*tail;
	knode_t 	*cur;
	knode_t		*root;

	IM_INT32 	length;	// number of node.

	im_mempool_handle_t	mpl;
	IM_INT32	dataSize;
	IM_BOOL		cpdata;
}im_klist_internal_t;

im_klist_handle_t im_klist_init(IN IM_INT32 dataSize, IN im_mempool_handle_t mpl)
{
	im_klist_internal_t *lst;
	
	lst = (im_klist_internal_t *)im_mpool_alloc(mpl, sizeof(im_klist_internal_t));
	if(lst == IM_NULL){
		return IM_NULL;
	}

	lst->head = lst->tail = lst->cur = lst->root = IM_NULL;
	lst->length = 0;
	lst->mpl = mpl;
	lst->dataSize = dataSize;
	lst->cpdata = (dataSize == 0)?IM_FALSE:IM_TRUE;
	return (im_klist_handle_t)lst;
}

IM_RET im_klist_deinit(IN im_klist_handle_t hlst)
{
	im_klist_clear(hlst);
	im_mpool_free(((im_klist_internal_t *)hlst)->mpl, (void *)hlst);
	return IM_RET_OK;
}

IM_BOOL im_klist_empty(IN im_klist_handle_t hlst)
{
	return (((im_klist_internal_t *)hlst)->length == 0) ? IM_TRUE : IM_FALSE;
}

IM_INT32 im_klist_size(IN im_klist_handle_t hlst)
{
	return ((im_klist_internal_t *)hlst)->length;
}

void * im_klist_get(IN im_klist_handle_t hlst, OUT IM_UINT32 *key)
{
	if(((im_klist_internal_t *)hlst)->cur != IM_NULL){
		if(key){
			*key = ((im_klist_internal_t *)hlst)->cur->key;
		}
		return ((im_klist_internal_t *)hlst)->cur->me;
	}
	return IM_NULL;
}
/*
static void klist_print_recursion(knode_t *node)
{
	if(node != IM_NULL){
		printf(" %d->bf: %d ", node->key, node->bf);
		klist_print_recursion(node->l);
		printf("\n");
		klist_print_recursion(node->r);
	}
}

void show_klist(im_klist_handle_t hlst)
{
	im_klist_internal_t *lst = (im_klist_internal_t *)hlst;

	printf("\n***** begin show klist, just for dbg *****\n");
	if(lst->root == IM_NULL)
		printf("klist is empty.\n");
	else{
		klist_print_recursion(lst->root);
		if(lst->head == IM_NULL || lst->tail == IM_NULL){
			printf("head or tail is null.\n");
			return ;
		}	
		printf("head %d  tail %d  root %d\n", lst->head->key, \
											lst->tail->key, \
											lst->root->key);
		printf("length %d  \n", lst->length);
	}
	printf("***** end show klist, just for dbg *****\n\n");
}
*/
/*
static IM_INT32 get_depth(knode_t *node)
{
	IM_INT32 ldepth=0, rdepth=0;

	if(node == IM_NULL)
		return 0;

	ldepth=get_depth(node->l);
	rdepth=get_depth(node->r);

	return (ldepth >= rdepth ? ldepth : rdepth)+1;
}

static IM_INT32 get_bf(knode_t *node)
{
	IM_INT32 bf;

	if(node == IM_NULL)
		return 0;

	return get_depth(node->l) - get_depth(node->r);
}
*/
void * im_klist_begin(IN im_klist_handle_t hlst, OUT IM_UINT32 *key)
{
	im_klist_internal_t *lst = (im_klist_internal_t *)hlst;
	if(lst->head != IM_NULL){
		lst->cur = lst->head;
		if(key){
			*key = lst->cur->key;
		}
		return lst->cur->me;
	}
	return IM_NULL;
}

static knode_t *next_knode(knode_t *node)
{
	knode_t *next;

	if(node == IM_NULL)
		return IM_NULL;
	
	if(node->r != IM_NULL){
		next = node->r;
		while(next->l != IM_NULL)
			next = next->l;
	}else if(node->p != IM_NULL){	// no right child, has parent
		next = node;
		while(next->p->key < next->key){
			next = next->p;
			if(next->p == IM_NULL)
				return IM_NULL;
		}
		next = next->p;
	}else	// no right child, no parent
		return IM_NULL;	

	return next;
}

static knode_t *prev_knode(knode_t *node)
{
	knode_t *prev;

	if(node == IM_NULL)
		return IM_NULL;
	
	if(node->l != IM_NULL){
		prev = node->l;
		while(prev->r != IM_NULL)
			prev = prev->r;
	}else if(node->p != IM_NULL){	// no left child, has parent
		prev = node;
		while(prev->p->key > prev->key){
			prev = prev->p;
			if(prev->p == IM_NULL)
				return IM_NULL;
		}
		prev = prev->p;
	}else
		return IM_NULL;

	return prev;
}

void * im_klist_next(IN im_klist_handle_t hlst, OUT IM_UINT32 *key)
{
	im_klist_internal_t *lst = (im_klist_internal_t *)hlst;
	knode_t *next;	

	next = next_knode(lst->cur);
	if(next != IM_NULL){
		lst->cur = next;
		if(key != IM_NULL)
			*key = next->key;
		return next->me;
	}

	return IM_NULL;
}

void * im_klist_prev(IN im_klist_handle_t hlst, OUT IM_UINT32 *key)
{
	im_klist_internal_t *lst = (im_klist_internal_t *)hlst;
	knode_t *prev;	

	prev = prev_knode(lst->cur);
	if(prev != IM_NULL){
		lst->cur = prev;
		if(key != IM_NULL)
			*key = prev->key;
		return prev->me;
	}

	return IM_NULL;
}

void * im_klist_end(IN im_klist_handle_t hlst, OUT IM_UINT32 *key)
{
	im_klist_internal_t *lst = (im_klist_internal_t *)hlst;
	if(lst->tail != IM_NULL){
		lst->cur = lst->tail;
		if(key){
			*key = lst->cur->key;
		}
		return lst->cur->me;
	}
	return IM_NULL;
}

void * im_klist_key(IN im_klist_handle_t hlst, IN IM_UINT32 key)
{
	im_klist_internal_t *lst = (im_klist_internal_t *)hlst;

	knode_t *node = lst->root;
	while(node){
		if(node->key == key){
			lst->cur = node;
			return lst->cur->me;
		}else if(node->key < key){
			node = node->r;
		}else if(node->key > key){
			node = node->l;
		}
	}

	return IM_NULL;
}

static knode_t *balance_tree(im_klist_internal_t *lst, knode_t *node)
{
	knode_t *tmpA, *tmpB, *tmpC, *tmpD, *tmpE;
	knode_t *tmpPar;

	if(node->bf == 2 && node->l->bf >= 0){	// LL. bf==0 for balance when deleting
		tmpPar = node->p;					// when put, l->bf = 1
		tmpA = node;						// when delete, l->bf = 0 or 1 
		tmpB = node->l;
		tmpC = node->l->r;
		if(tmpPar != IM_NULL){
			if(tmpPar->l == node)
				tmpPar->l = tmpB; 
			else
				tmpPar->r = tmpB;
		}
		tmpA->l = tmpC;
		tmpA->p = tmpB;
		tmpB->r = tmpA;
		tmpB->p = tmpPar;
		if(tmpC != IM_NULL)
			tmpC->p = tmpA;
		if(tmpPar == IM_NULL){
			lst->root = tmpB;
		}
		tmpA->bf = (tmpB->bf == 1 ? 0 : 1); 
		tmpB->bf = (tmpB->bf == 1 ? 0 : -1); 
/*
		tmpA->bf = get_bf(tmpA);
		tmpB->bf = get_bf(tmpB);
*/
		return tmpB;
	}else if(node->bf == 2 && node->l->bf == -1){	// LR. 
		tmpPar = node->p;
		tmpA = node;
		tmpB = node->l;
		tmpC = node->l->r;
		tmpD = node->l->r->l;
		tmpE = node->l->r->r;
		if(tmpPar != IM_NULL){
			if(tmpPar->l == node)
				tmpPar->l = tmpC; 
			else
				tmpPar->r = tmpC;
		}
		tmpA->l = tmpE;
		tmpA->p = tmpC;
		tmpB->r = tmpD;
		tmpB->p = tmpC;
		tmpC->l = tmpB;
		tmpC->r = tmpA;
		tmpC->p = tmpPar;
		if(tmpD != IM_NULL)
			tmpD->p = tmpB;
		if(tmpE != IM_NULL)
			tmpE->p = tmpA;
		if(tmpPar == IM_NULL){
			lst->root = tmpC;
		}
		tmpA->bf = (tmpC->bf == 1 ? -1 : 0);
		tmpB->bf = (tmpC->bf == -1 ? 1 : 0); 
		tmpC->bf = 0;
/*
		tmpA->bf = get_bf(tmpA);
		tmpB->bf = get_bf(tmpB);
		tmpC->bf = get_bf(tmpC);
*/
		return tmpC;
	}else if(node->bf == -2 && node->r->bf <= 0){	// RR. bf==0 for balance when deleting
		tmpPar = node->p;							// when put, l->bf = -1
		tmpA = node;								// when delete, l->bf = 0 or -1
		tmpB = node->r;
		tmpC = node->r->l;
		if(tmpPar != IM_NULL){
			if(tmpPar->l == node)
				tmpPar->l = tmpB; 
			else
				tmpPar->r = tmpB;
		}
		tmpA->r = tmpC;
		tmpA->p = tmpB;
		tmpB->l = tmpA;
		tmpB->p = tmpPar;
		if(tmpC != IM_NULL)
			tmpC->p = tmpA;
		if(tmpPar == IM_NULL){
			lst->root = tmpB;
		}
		tmpA->bf = (tmpB->bf == -1 ? 0 : -1); 
		tmpB->bf = (tmpB->bf == -1 ? 0 : 1); 
/*
		tmpA->bf = get_bf(tmpA);
		tmpB->bf = get_bf(tmpB);
*/
		return tmpB;
	}else if(node->bf == -2 && node->r->bf == 1){	// RL. 
		tmpPar = node->p;
		tmpA = node;
		tmpB = node->r;
		tmpC = node->r->l;
		tmpD = node->r->l->r;
		tmpE = node->r->l->l;
		if(tmpPar != IM_NULL){
			if(tmpPar->l == node)
				tmpPar->l = tmpC; 
			else
				tmpPar->r = tmpC;
		}
		tmpA->r = tmpE;
		tmpA->p = tmpC;
		tmpB->l = tmpD;
		tmpB->p = tmpC;
		tmpC->r = tmpB;
		tmpC->l = tmpA;
		tmpC->p = tmpPar;
		if(tmpD != IM_NULL)
			tmpD->p = tmpB;
		if(tmpE != IM_NULL)
			tmpE->p = tmpA;
		if(tmpPar == IM_NULL){
			lst->root = tmpC;
		}
		tmpA->bf = (tmpC->bf == -1 ? 1 : 0);
		tmpB->bf = (tmpC->bf == 1 ? -1 : 0);
		tmpC->bf = 0;
/*
		tmpA->bf = get_bf(tmpA);
		tmpB->bf = get_bf(tmpB);
		tmpC->bf = get_bf(tmpC);
*/
		return tmpC;
	}

    return IM_NULL;
}

IM_RET im_klist_put(IN im_klist_handle_t hlst, IN IM_UINT32 key, IN void *data)
{
	im_klist_internal_t *lst = (im_klist_internal_t *)hlst;
	knode_t *index = IM_NULL;
	knode_t *tmp;
	IM_BOOL balflag=IM_TRUE;	//balance flag

	if(lst->root != IM_NULL){	// if tree not empty
		index = lst->root;		
		while(IM_TRUE){		// One of situations below must occur.
			if(key == index->key){		// key already existed, return.
				return IM_RET_FAILED;		
			}else if(key > index->key){		// put right or search down
				if(index->r == IM_NULL){
					break;
				}else
					index = index->r;
			}else{							// put left or search down
				if(index->l == IM_NULL){
					break;
				}else
					index = index->l;
			}
		}//endwhile, index pointing at the parent of new node tmp
	}

	//no same key in tree or tree is empty, put node in tree
	tmp = (knode_t *)im_mpool_alloc(lst->mpl, sizeof(knode_t));
	if(tmp == IM_NULL){
		return IM_RET_NOMEMORY;
	}

	if(lst->cpdata){
		tmp->me = im_mpool_alloc(lst->mpl, lst->dataSize);
		if(tmp->me == IM_NULL){
			im_mpool_free(lst->mpl, (void *)tmp);
			return IM_RET_NOMEMORY;
		}
		memcpy(tmp->me, data, lst->dataSize);
	}else{
		tmp->me = data;
	}
	tmp->key = key;
	tmp->bf = 0;
	tmp->l = tmp->r = tmp->p = IM_NULL;

	if(lst->root == IM_NULL){	// if the tree is empty, put as root and return
		lst->root = lst->head = lst->tail = tmp;
		lst->length++;
		return IM_RET_OK;
	}
	
	if(key > index->key){
		index->r = tmp;
	}else{
		index->l = tmp;
	}
	tmp->p = index;

	while(index != IM_NULL){	// update the balance factor of the nodes
		if(key > index->key)	// on the path from root to new node tmp
			index->bf--;		// If the height of subtree not changed,
		else					// bf of root of subtree must be 0
			index->bf++;

		if(index->bf == 0)		// The binary tree is balanced
			break;	
		if(index->bf == 2 || index->bf == -2){	// insert causes the tree unbalanced
			balflag = IM_FALSE;					// and index is pointing at the root
			break;								// of minimum unbalance tree
		}
		index = index->p;
	}

	if(!balflag){	// if unbalanced
		balance_tree(lst, index);
	}

	// the root of whole tree has updated in the balance_tree()
	lst->cur = tmp;
	if(tmp->key < lst->head->key)
		lst->head = tmp;
	if(tmp->key > lst->tail->key)
		lst->tail =tmp;
	lst->length++;

	return IM_RET_OK;
}

void * im_klist_erase(IN im_klist_handle_t hlst, IN IM_UINT32 key)
{
	im_klist_internal_t *lst = (im_klist_internal_t *)hlst;
	knode_t *index = lst->root;
	knode_t *Par;
	IM_BOOL exchflag = IM_FALSE;

	while(index != IM_NULL){
		if(index->key == key)
			break;
		else if(index->key > key)
			index = index->l;
		else
			index = index->r;
	}
	if(index == IM_NULL)
		return IM_NULL;		// key not found or the tree is empty, return

	if(index == lst->head)
		lst->head = next_knode(lst->head);
	if(index == lst->tail)
		lst->tail = prev_knode(lst->tail);
	lst->cur = next_knode(index);	// cur move to next node of erased node

	Par = index->p;		// index is the node to be deleted.
	if(index->l == IM_NULL && index->r == IM_NULL){	// index has no child
		if(Par == IM_NULL)		// if index is the root
			lst->root = IM_NULL;
		else if(Par->r == index)
			Par->r = IM_NULL;
		else
			Par->l = IM_NULL;
	}else if(index->l == IM_NULL && index->r != IM_NULL){	// has only right child
		if(Par == IM_NULL)
			lst->root = index->r; 
		else if(Par->r == index)
			Par->r = index->r;
		else
			Par->l = index->r;
		index->r->p = Par;
	}else if(index->r == IM_NULL && index->l != IM_NULL){	// has only left child
		if(Par == IM_NULL)
			lst->root = index->l;
		else if(Par->r == index)
			Par->r = index->l;
		else
			Par->l = index->l;
		index->l->p = Par;
	}else{								// has both left and right child 
		knode_t *exch;			// for exchange
		void *tmp_me;

		exchflag = IM_TRUE;

		exch = index->l;
		while(exch->r != NULL){	// exch is maximum in index's right subtree 
			exch = exch->r;
		}
		// exchange the me between index and exch
		tmp_me = index->me;
		index->me = exch->me;
		exch->me = tmp_me;
		index->key = exch->key;

		// The head, tail, cur may be pointing at the exch
		// so we should update them
		if(lst->head == exch)
			lst->head = index;
		if(lst->tail == exch)
			lst->tail = index;
		if(lst->cur == exch)
			lst->cur = index;

		if(exch->p == index){
			Par = exch->p;
			Par->l = exch->l;
			if(exch->l != IM_NULL)
				exch->l->p = Par;
		}else{
			Par = exch->p;
			Par->r = exch->l;
			if(exch->l != IM_NULL)
				exch->l->p = Par;
		}
		// because the deleted node has exchanged, so the root need not change
		index = exch;
		key = exch->key;	// delete key should update
	}

	if(lst->cpdata)
		im_mpool_free(lst->mpl, index->me);
	im_mpool_free(lst->mpl, index);

	index = Par;
	while(index != IM_NULL){
		if(key <= index->key)	// key may equals to index->key, because the exchange
			index->bf--;
		else
			index->bf++;
		
		if(index->bf == 1 || index->bf == -1)
			break;
		if(index->bf == 2 || index->bf == -2){
			index = balance_tree(lst, index);
			if(index->bf == 1 || index->bf == -1)
				break;
		}
		index = index->p;
	}

	lst->length--;
	return (lst->cur != IM_NULL ? lst->cur->me : IM_NULL);
}

static void klist_clear_node_recursion(im_klist_internal_t *lst, knode_t *node)
{
	if(node->l){
		klist_clear_node_recursion(lst, node->l);
	}
	if(node->r){
		klist_clear_node_recursion(lst, node->r);
	}

	if(lst->cpdata){
		im_mpool_free(lst->mpl, node->me);
	}
	im_mpool_free(lst->mpl, node);
}

void im_klist_clear(IN im_klist_handle_t hlst)
{
	im_klist_internal_t *lst = (im_klist_internal_t *)hlst;
	if(lst->root){
		klist_clear_node_recursion(lst, lst->root);
	}
	
	lst->root = lst->head = lst->tail = lst->cur = IM_NULL;
	lst->length = 0;
}


//
//=============================================================================
//
typedef struct{
	im_list_handle_t 	queue;
	IM_INT32 		depth;

	im_mempool_handle_t	mpl;
	IM_INT32		dataSize;
}im_ring_internal_t;


im_ring_handle_t im_ring_init(IN IM_INT32 dataSize, IN IM_INT32 depth, IN im_mempool_handle_t mpl)
{
	im_ring_internal_t *ring = (im_ring_internal_t *)im_mpool_alloc(mpl, sizeof(im_ring_internal_t));
	if(ring == IM_NULL){
		return IM_NULL;
	}

	ring->queue = im_list_init(dataSize, mpl);
	if(ring->queue == IM_NULL){
		im_mpool_free(mpl, (void *)ring);
		return IM_NULL;
	}

	if(depth < 1){
		ring->depth = 1;
	}else{
		ring->depth = depth;
	}
	ring->mpl = mpl;
	ring->dataSize = dataSize;

	return (im_ring_handle_t)ring;
}

IM_RET im_ring_deinit(IN im_ring_handle_t hring)
{
	im_ring_clear(hring);
	im_list_deinit(((im_ring_internal_t *)hring)->queue);
	im_mpool_free(((im_ring_internal_t *)hring)->mpl, (void *)hring);
	return IM_RET_OK;
}

IM_RET im_ring_put(IN im_ring_handle_t hring, IN void *data, OUT void *overflow)
{
	void *over;
	im_ring_internal_t *ring = (im_ring_internal_t *)hring;

	if(im_list_size(ring->queue) < ring->depth){
		return im_list_put_back(ring->queue, data);
	}else{
		over = im_list_begin(ring->queue);
		if(overflow != IM_NULL){
			memcpy((void *)overflow, (void *)over, ring->dataSize);
		}
		im_list_erase(ring->queue, over);
		if(im_list_put_back(ring->queue, data) != IM_RET_OK){
			return IM_RET_FAILED;
		}
		return IM_RET_OVERFLOW;
	}
}

IM_RET im_ring_get(IN im_ring_handle_t hring, OUT void *data)
{
	void *tmp;
	im_ring_internal_t *ring = (im_ring_internal_t *)hring;

	tmp = im_list_begin(ring->queue);
	if(tmp != IM_NULL){
		memcpy((void *)data, (void *)tmp, ring->dataSize);
		im_list_erase(ring->queue, tmp);
		return IM_RET_OK;
	}else{
		return IM_RET_FAILED;
	}
}

IM_INT32 im_ring_size(IN im_ring_handle_t hring)
{
	return im_list_size(((im_ring_internal_t *)hring)->queue);
}

IM_BOOL im_ring_empty(IN im_ring_handle_t hring)
{
	return im_list_empty(((im_ring_internal_t *)hring)->queue);
}

void im_ring_clear(IN im_ring_handle_t hring)
{
	return im_list_clear(((im_ring_internal_t *)hring)->queue);
}


