#ifndef __LIST_H
#define __LIST_H

#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <dirent.h>
#include "message_queue_interface.h"
#include "message_queue.h"
#include "stat.h"


#define SERVER_LOG_OPEN 0
#if SERVER_LOG_OPEN
	#define SERVER_DBG(arg...) printf("[eventhub]" arg)
#else
	#define SERVER_DBG(arg...)
#endif

#define EVENT_RPC (1 << 8)
#define EVENT_PR0 (0 << 0)
#define EVENT_PR1 (1 << 0)
#define EVENT_PR2 (2 << 0)

static char event_name_list[MAX_EVENT][EVENT_PATH_SIZE] = {0}; /* save all the event name */

static pthread_mutex_t register_list_mutex; //

static pthread_mutex_t data_list_mutex; //

/*
* the handler register in the eventd
*/
struct handler_list
{
	char event_name[EVENT_PATH_SIZE];   /* listener event name */
	pid_t handler_pid;	               /* handler process id */
	int handler_id;                    /* handler receiver data message queue */
	int priority;                      /* handler process data priority */
	struct handler_list *next;         /* point to the next register handler list */
};

/*
* manage the register handler and provider for one event(event name)
*/
struct eventd_register_list
{
	int handler_num;
	int handler_priority;
	pid_t rpc_handler_pid;
	struct handler_list *handler_head;       /* registered handler list head */
};

/*
*create thread for do something
*/
static pthread_t register_ntid;
static pthread_t receiver_data_ntid;
static pthread_t send_data_ntid;
static pthread_t stat_ntid;
/*
*
*/
typedef struct data_list
{
	char event_name[EVENT_PATH_SIZE];
	char data[MAX_EVENT_DATA];
	char need_wait;
	char datasize;
	pid_t pid;
	pid_t tid;
	int priority;
	struct timeval time;
	struct data_list *pNext;
}Node;

typedef struct reback_data_list
{
	char data[MAX_EVENT_DATA];
	pid_t provider_pid;
	struct reback_data_list *pNext;
}Reback_Node;


//create head node
int createNodeList();
//add node
int addNode(Node* node);
//delete node
int deleteNode(Node* node);
//create handler head
int createHandlerHead(int i);
//add handler to list
int addHandler(int i, struct handler_list* handler);
//delete handler from list
int deleteHandler(int i, struct handler_list* handler);
//create head node
int createReNodeList();
//add node
int addReNode(Reback_Node* node);
//delete node
int deleteReNode(Reback_Node* node);

#endif
