#ifndef __EVENT_DEV_H
#define __EVENT_DEV_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

#define EVENT_RPC (1 << 8)
#define EVENT_PR0 (0 << 0)
#define EVENT_PR1 (1 << 0)
#define EVENT_PR2 (2 << 0)
#define EVENT_PATH_SIZE 64

// TODO: move these into common-provider.h

#include <sys/types.h>

#define TYPE_LENGTH 64
#define SUB_TYPE_LENGTH 128

typedef struct share_memory_info
{
    char ps8Type[TYPE_LENGTH];
    char ps8SubType[SUB_TYPE_LENGTH];
    int s32Key;
    int s32Size;
} ST_SHM_INFO;


typedef struct event_scatter
{
	char *buf;
	int len;
} scatter;

struct event_process {
	pid_t pid;
	char comm[EVENT_PATH_SIZE];
};

/*
*get event count register in handler list
*/
int event_get_count(const char *event);

/*
*get all event register in eventhub
*/
const char **event_get_list(void);

/*
*add a new handler register message information to the hash list table
*return 0 if register handler success
*send signal to wakeup eventd process
*fail return -1
*/
int event_register_handler(const char *event, const int priority, void (* handler)(char *event, void *arg));

/*
*delete a handler register message information from the hash list table
*return 0 if register handler success
*send signal to wakeup eventd process
*fail return -1
*/
int event_unregister_handler(const char *event, void (* handler)(char* event, void *arg));

/*
*send provider data to eventd process
*wake up the eventd process
*/
int event_send(const char *event, char *buf, int size);

/*
*send provider data to eventd process
*wake up the eventd process
*wait the handler return the modified data
*/
int event_rpc_call(const char *event, char *buf, int size);

/*
*like the event send
*/
int event_send_scatter(const char *name, scatter sc[], int count);

/*
*like the event rpc call
*/
int event_rpc_call_scatter(const char *name, scatter sc[], int count);

#include <qsdk/eventlist.h>

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif
