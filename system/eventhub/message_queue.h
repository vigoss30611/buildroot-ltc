#ifndef __MESSAGE_QUEUE_H
#define __MESSAGE_QUEUE_H

#define MAX_EVENT_DATA   384
#define MAX_EVENT        128

#include <sys/time.h>
#include <event.h>

typedef enum
{
	HANDLER_REGISTER_TYPE = 100,
	PROVIDER_REGISTER_TYPE,
	HANDLER_REGISTER_RETURN,
	PROVIDER_REGISTER_RETURN,
	DATA_TYPE,
	DATA_FROM_HANDLER,
	GET_EVENT_COUNT,
	GET_EVENT_NAME,
	RETURN_EVENT_COUNT,
	RETURN_EVENT_NAME,
} EVENT_MESSAGE_TYPE;

typedef enum
{
	EVENT_REGISTER = 200,
	EVENT_UNREGISTER,
} EVENT_TYPE;

struct event_register_data
{
	char name[EVENT_PATH_SIZE];         /* event name */
	pid_t pid;                         /* register handler or provider process ID */
    key_t handler_key;                 /* handler message key */
	int priority;
	int event_type;
};

struct msg_register_buffer
{
	long msg_type;                     /* messsage type */
	struct event_register_data event;  /* handler or provider register information  */
};

struct event_data
{
	char name[EVENT_PATH_SIZE];         /* event name */
	pid_t pid;                         /* provider process ID about provider send*/
	pid_t tid;
	char need_wait;
	struct timeval send_tv;
	char buf[MAX_EVENT_DATA]; /* event data information */
	int datasize;
};

struct msg_data_buffer
{
	long msg_type;                     /* message type */
	struct event_data event;	       /* message queue data buffer */
};

struct event_info
{
	char name[EVENT_PATH_SIZE];         /* event name */
	int count;
};

struct msg_event_info
{
	long msg_type;
	struct event_info info;
};

static key_t eventd_handler_key = -1;  /* handler register message queue key */
static int eventd_handler_ID = -1;     /* handler register message queue id */
static key_t eventd_provider_key = -1; /* provider register message queue key */
static int eventd_provider_ID = -1;    /* provider register message queue id */
static key_t handler_key = -1;         /* handler receiver data message queue key */
static int handler_ID = -1;            /* handler receiver data message queue id */
static key_t provider_key = -1;        /* provider send data message queue key */
static int provider_ID = -1;           /* provider send data message queue id */

static int rc;                          // Error message handler.

static struct msg_register_buffer register_msg;
static struct msg_data_buffer data_msg;

void (* func_handler)(char*event, void *arg);

#endif
