#include "qm_event.h"

#include <dirent.h>
#include <sys/syscall.h>
#include <sys/prctl.h>
#include <pthread.h>

#include "list.h"
#include "ipc.h"

#define LOGTAG "QM_EVENT"

#define EVENT_PATH_SIZE 64

typedef struct event_handler
{
	void (*func_handler)(char *event, void *arg, int size);
	struct listnode node;
}event_handler_t;

typedef struct event_mode
{
	char name[EVENT_PATH_SIZE];
	int priority;
    pthread_mutex_t mutex;
    struct listnode handler_list;
    struct listnode node;
}event_mode_t;

typedef struct qm_event 
{
    pthread_mutex_t mutex;
    int inited;
    struct listnode event_list;
}qm_event_t;

/**
 * inst-->event_list-->event_a-->event_b
 *                       |         |  
 *                       |.handle_list-->handle_a1-->handl_a2
 *                                 |
 *                                 |.handle_list-->handle_b1-->handle_b2
 *
 * */

static qm_event_t qm_event_inst = {
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .inited = 0,
};

static int event_init(qm_event_t *inst)
{
    if (inst->inited) {
        ipclog_warn("%s alreay\n", __FUNCTION__);
        return 0;
    }

    list_init(&inst->event_list);

    inst->inited = 1;

    ipclog_info("%s done\n", __FUNCTION__);
    return 0;   
}

static event_handler_t *event_handler_create(event_mode_t *event_node, void *handler)
{
    event_handler_t *new = (event_handler_t *)malloc(sizeof(event_handler_t));
    if (!new) {
        ipclog_error("Out of memory for handler_new\n");
        return NULL;
    }
    new->func_handler = handler;
    
    pthread_mutex_lock(&event_node->mutex);
    list_add_tail(&event_node->handler_list, &new->node);
    pthread_mutex_unlock(&event_node->mutex);
    
    ipclog_info("%s event:%s handler:%p\n", __FUNCTION__, event_node->name, handler);
    return new;
}

static event_handler_t *event_handler_find(event_mode_t *event_node, void *handler) 
{
    struct listnode *node;
    event_handler_t *find = NULL;
    
    pthread_mutex_lock(&event_node->mutex);

    list_for_each(node, &event_node->handler_list) {
        find = node_to_item(node, event_handler_t, node);
        if (find->func_handler == handler) {
            pthread_mutex_unlock(&event_node->mutex);
            return find;
        }
    }

    pthread_mutex_unlock(&event_node->mutex);

    return NULL;
} 

static int event_handler_remove(event_mode_t *event_node, void *handler)
{
    struct listnode *node;
    struct listnode *l;
    event_handler_t *find = NULL;
    
    pthread_mutex_lock(&event_node->mutex);

    list_for_each_safe(node, l, &event_node->handler_list) {
        find = node_to_item(node, event_handler_t, node);
        if ((find->func_handler == handler)) {
            list_remove(node);
            free((void *)find);
            ipclog_info("%s event:%s handler:%p\n", __FUNCTION__, event_node->name, handler);
            pthread_mutex_unlock(&event_node->mutex);
            return 0;
        }
    }

    pthread_mutex_unlock(&event_node->mutex);

    return -1;
}

static void event_handler_distribute(event_mode_t *event_node, void *arg, int size)
{
    struct listnode *node;
    event_handler_t *find = NULL;

    pthread_mutex_lock(&event_node->mutex);

    list_for_each(node, &event_node->handler_list) {
        find = node_to_item(node, event_handler_t, node);
        if (find->func_handler) {
            find->func_handler(event_node->name, arg, size);
        }
    }

    pthread_mutex_unlock(&event_node->mutex);

    return;
}


static event_mode_t *event_mode_create(qm_event_t *inst, char *event, int priority)
{
    event_mode_t *new = (event_mode_t *)malloc(sizeof(event_mode_t));
    if (!new) {
        ipclog_error("%s Out of memory for event_mode\n", __FUNCTION__);
        return NULL;
    }

    list_init(&new->handler_list);
    pthread_mutex_init(&new->mutex, NULL);
    strncpy(new->name, event, strlen(event) + 1);
    new->priority = priority;
    
    list_add_tail(&inst->event_list, &new->node);

    ipclog_info("%s event:%s \n", __FUNCTION__, new->name);

    return new;
}


static event_mode_t *event_mode_find(qm_event_t *inst, char *event) 
{
    struct listnode *node;
    event_mode_t *find = NULL;
    list_for_each(node, &inst->event_list) {
        find = node_to_item(node, event_mode_t, node);
        if (strcmp(find->name, event) == 0) {
            return find;
        }
    }

    return NULL;
}

static int event_mode_remove(qm_event_t *inst, event_mode_t *event_node) 
{
    struct listnode *node;
    struct listnode *l;
    event_mode_t *find = NULL;
    list_for_each_safe(node, l, &inst->event_list) {
        find = node_to_item(node, event_mode_t, node);
        if (find == event_node) {
            //TODO: should checkout event handler_list is empty
            ipclog_info("%s event:%s \n", __FUNCTION__, event_node->name);
            pthread_mutex_destroy(&find->mutex);
            list_remove(node);
            free((void *)find);
            return 0;
        }
    }

    return -1;
}

static int qm_event_register_handler(const char *event, const int priority, 
        void (* handler)(char* event, void *arg, int size))
{
    if ((NULL == handler) || (NULL == event)) {
        ipclog_error("%s Invalid event:%p or handler:%p \n", __FUNCTION__, event, handler);
        return -1;
    }

    pthread_mutex_lock(&qm_event_inst.mutex);
    if (!qm_event_inst.inited) {
        event_init(&qm_event_inst);
    }

    //find or create new event
    event_mode_t *event_find = NULL; 
    event_find = event_mode_find(&qm_event_inst, (char *)event);
    if (!event_find) {
        event_find = event_mode_create(&qm_event_inst, (char *)event, priority);
        if (!event_find) {
            pthread_mutex_unlock(&qm_event_inst.mutex);
            return -1;
        }
    }

    //check handler register already or not
    event_handler_t *handler_find = event_handler_find(event_find, handler); 
    if (handler_find) {
        ipclog_warn("%s event:%s handler:%p already\n", __FUNCTION__, event, handler); 
        pthread_mutex_unlock(&qm_event_inst.mutex);
        return 0;
    }

    //create new handler
    event_handler_t *handler_new = event_handler_create(event_find, handler);
    if (!handler_new) {
        pthread_mutex_unlock(&qm_event_inst.mutex);
        return -1;
    }

    pthread_mutex_unlock(&qm_event_inst.mutex);

    ipclog_info("%s event:%s handler:%p done\n", __FUNCTION__, event, handler); 
    return 0;
}

static int qm_event_unregister_handler(const char *event, void (* handler)(char* event, void *arg, int size))
{
	if((NULL == handler) || (NULL == event)) {
        ipclog_info("%s Invalid event:%p or handler:%p \n", __FUNCTION__, event, handler);
        return -1;
	}
  
    pthread_mutex_lock(&qm_event_inst.mutex);
    if (!qm_event_inst.inited) {
        ipclog_warn("%s inst not inited already\n", __FUNCTION__);
        pthread_mutex_unlock(&qm_event_inst.mutex);
        return -1;
    }
   
    event_mode_t *event_find = NULL; 
    event_find = event_mode_find(&qm_event_inst, (char *)event);
    if (!event_find) {
        ipclog_error("%s cannot find event:%s\n", __FUNCTION__, event);
        pthread_mutex_unlock(&qm_event_inst.mutex);
        return -1;
    }

    event_handler_t *handler_find = NULL;
    handler_find = event_handler_find(event_find, handler);
    if (!handler_find) {
        ipclog_error("%s cannot find handler:%p\n", __FUNCTION__, handler);
        pthread_mutex_unlock(&qm_event_inst.mutex);
        return -1;
    }
    
    event_handler_remove(event_find, handler);
    //event_mode_remove(event_find);
    
    pthread_mutex_unlock(&qm_event_inst.mutex);

    ipclog_info("%s event:%s handler:%p done\n", __FUNCTION__, event, handler);
    return 0;
}

static int qm_event_send(const char *event, void *arg, int size)
{
    if ((event == NULL) || (NULL == arg)) {
        ipclog_error("%s Invalid event:%p or arg:%p\n", __FUNCTION__, event, arg);
        return -1;
    }

    pthread_mutex_lock(&qm_event_inst.mutex);
    if (!qm_event_inst.inited) {
        ipclog_warn("%s inst not inited already\n", __FUNCTION__);
        pthread_mutex_unlock(&qm_event_inst.mutex);
        return -1;
    }

    event_mode_t *event_find = NULL;
    event_find = event_mode_find(&qm_event_inst, (char *)event);
    if (!event_find) {
        ipclog_warn("%s cannot find registered event:%s\n", __FUNCTION__, event);
        pthread_mutex_unlock(&qm_event_inst.mutex);
        return -1;
    }

    pthread_mutex_unlock(&qm_event_inst.mutex);

    //case,we don`t remove event mode in event_unregister_handler, 
    //so, it is safe lock event mutex only
    //TODO: may add thread & queue to make event distribute more effective
    event_handler_distribute(event_find, arg, size);

    return 0;
}

int QM_Event_RegisterHandler(const char *event, const int priority, 
        void (* handler)(char* event, void *arg, int size))
{
    return qm_event_register_handler(event, priority, handler);
}

int QM_Event_UnregisterHandler(const char *event, void (* handler)(char* event, void *arg, int size))
{
    return qm_event_unregister_handler(event, handler);
}

int QM_Event_Send(const char *event, void *arg, int size)
{
    return qm_event_send(event, arg, size);
}





