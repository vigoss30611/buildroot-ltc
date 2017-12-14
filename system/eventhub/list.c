#include "list.h"

extern Reback_Node *Reback_data_list_head;

extern Node *data_list_head;

extern struct eventd_register_list event_register_table[MAX_EVENT];

//create head node
int createNodeList()
{
    data_list_head = (Node*) malloc(sizeof(Node));
    if(NULL == data_list_head) {
        return -1;
    } else {
        memset(data_list_head, 0, sizeof(Node));
        return 0;
    }
}

//add node
int addNode(Node* node)
{
    if((NULL == data_list_head) || (NULL == node)) {
        return -1;
    }
    Node* p = data_list_head->pNext;
    Node* q = data_list_head;
    while((NULL != p) && (node->priority > p->priority)) {
        q = p;
        p = p->pNext;
    }
    q->pNext = node;
	node->pNext = p;

    return 0;
}

//delete node
int deleteNode(Node* node)
{
	SERVER_DBG("%s.....%d..+++++++++++++++++++\n", __FUNCTION__, __LINE__);
    if((NULL == data_list_head) || (NULL == node)) {
        return -1;
    }
    Node* p = data_list_head->pNext;
	Node* q = data_list_head;

    while(node != p) {
		q = p;
        p = p->pNext;
    }

	Node* t = p->pNext;
    q->pNext = t;
    free(p);
  	p =NULL;

  	SERVER_DBG("%s.....%d..--------------------\n", __FUNCTION__, __LINE__);

    return 0;
}

//create handler head
int createHandlerHead(int i)
{
    event_register_table[i].handler_head = (struct handler_list*) malloc(sizeof(struct handler_list));
    if(NULL == event_register_table[i].handler_head) {
        return -1;
    } else {
    	SERVER_DBG("%s %d ..malloc handler_head success........\n", __FUNCTION__, __LINE__);
        memset(event_register_table[i].handler_head, 0, sizeof(struct handler_list));
        return 0;
    }
}

//add handler to list
int addHandler(int i, struct handler_list* handler)
{
    if((NULL == event_register_table[i].handler_head) || (NULL == handler)) {
        return -1;
    }
    struct handler_list* p = event_register_table[i].handler_head->next;
    struct handler_list* q = event_register_table[i].handler_head;
    while((NULL != p) && (handler->priority > p->priority)) {
    	if(handler->handler_pid == p->handler_pid) {
    		return -1;
    	}
        q = p;
        p = p->next;
    }
    q->next = handler;
	handler->next = p;

    return 0;
}

//delete handler from list
int deleteHandler(int i, struct handler_list* handler)
{
    if((NULL == event_register_table[i].handler_head) || (NULL == handler)) {
        return -1;
    }
    struct handler_list* p = event_register_table[i].handler_head->next;
    struct handler_list* q = event_register_table[i].handler_head;

    while(handler != p) {
		q = p;
        p = p->next;
    }

	struct handler_list* t = p->next;
    q->next = t;
    free(p);
  	p =NULL;

    return 0;
}

//create head node
int createReNodeList()
{
    Reback_data_list_head = (Reback_Node*) malloc(sizeof(Reback_Node));
    if(NULL == Reback_data_list_head) {
        return -1;
    } else {
        memset(Reback_data_list_head, 0, sizeof(Node));
        return 0;
    }
}

//add node
int addReNode(Reback_Node* node)
{
    if((NULL == Reback_data_list_head) || (NULL == node)) {
        return -1;
    }
    Reback_Node* p = Reback_data_list_head->pNext;
    Reback_Node* q = Reback_data_list_head;
    while(NULL != p) {
        q = p;
        p = p->pNext;
    }
    q->pNext = node;
	node->pNext = p;

    return 1;
}

//delete node
int deleteReNode(Reback_Node* node)
{
    if((NULL == Reback_data_list_head) || (NULL == node)) {
        return -1;
    }
    Reback_Node* p = Reback_data_list_head->pNext;
	Reback_Node* q = Reback_data_list_head;

    while(node != p) {
		q = p;
        p = p->pNext;
    }

	Reback_Node* t = p->pNext;
    q->pNext = t;
    free(p);
  	p =NULL;

    return 0;
}
