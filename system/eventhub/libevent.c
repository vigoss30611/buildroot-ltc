#include <dirent.h>
#include <sys/syscall.h>
#include <sys/prctl.h>
#include "message_queue_interface.h"
#include "message_queue.h"
#include "event.h"
#include <pthread.h>

static int eventd_pid = -1;
#define EVENTD_PROCESS_NAME "eventhub"

#define MYSIGNAL (SIGRTMIN+5)

#define MAX_HANDLER    16
static int handler_num = 0;
static int flag_num = 0;

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

typedef struct func_list
{
	void (*func_handler)(char *event, void *arg);
	struct func_list* next;
}func;

typedef struct handler_list
{
	char name[EVENT_PATH_SIZE];
	func func_handler_list;
	int priority;
}handler_func;

handler_func func_array[MAX_HANDLER] = {0};

static pthread_t handler_ntid = 0;

#define EVENT_DEV_DBG 0
#if EVENT_DEV_DBG
#define DEV_DBG(arg...) printf("[libevent]" arg)
#else
#define DEV_DBG(arg...)
#endif

pid_t gettid() 
{
	return syscall(SYS_gettid);
}

/*
* get the eventd process id
*/
static int event_get_data(char *buf, int size, struct msg_data_buffer* handler_data_msg);

static int get_eventd_pid(void)
{
	DIR *dir;
    struct dirent *ptr;
    FILE *fp;
    char filepath[50];//cmdline file path
    char filetext[50];//cmdline
    dir = opendir("/proc"); //open proc path
    if (NULL != dir) {
        while (NULL != (ptr = readdir(dir))) {//read file or dir
            //if . or ..,break
            if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)) {
            	continue;
            }
            if (DT_DIR != ptr->d_type) {
            	continue;
            }

            sprintf(filepath, "/proc/%s/cmdline", ptr->d_name);//file path
            fp = fopen(filepath, "r");//open
            if (NULL != fp) {
                fread(filetext, 1, 50, fp);//read
                filetext[49] = '\0';//add the stop character

                if (filetext == strstr(filetext, EVENTD_PROCESS_NAME)) {
                	eventd_pid = atoi(ptr->d_name);
                	DEV_DBG(" %s PID:  0x%x   \n", __FUNCTION__, eventd_pid);
                }
				memset(filetext, 0, 50);
                fclose(fp);
            }
        }
        closedir(dir);//close path
    }
}

/*
* create eventd message queue to transform register information
* create the handler message queue for transform data
*/
int handler_init(void)
{
	DEV_DBG("%s-----\n", __FUNCTION__);
	if(-1 == eventd_pid) {
		get_eventd_pid();
		if(-1 == eventd_pid) {
			return -1;
		}
	}

	if((-1 == eventd_handler_key) || (-1 == eventd_handler_ID) || (-1 == handler_key) || (-1 == handler_ID)) {
		eventd_handler_key = eventd_pid;
		eventd_handler_msg_queue_create(eventd_handler_key);      //message queue for register handler
		handler_msg_queue_create();                               //message queue for read data
	}

	return 0;
}

/*
*get event count register in handler list
*/
int event_get_count(const char *event)
{
#if 0
	struct msg_event_info event_info;
	event_info.msg_type = GET_EVENT_COUNT;
	strncpy(event_info.info.name, event, strlen(event)+1);

	if(0 != Get_Event_count(&event_info)) {
		DEV_DBG("%s get event info failed........\n", __FUNCTION__);
		return -1;
	}

	event_info.msg_type = RETURN_EVENT_COUNT;
	if(0 == Get_Event_count_Return(&event_info)) {    //if no msg in msg queue,block
		return event_info.info.count;
	}

	return -1;
#else
	return 0;
#endif
}

/*
*get all event register in eventhub
*/
const char **event_get_list(void)
{
#if 0
	struct msg_event_info event_info;
	event_info.msg_type = GET_EVENT_NAME;

	if(0 != Get_Event_name(&event_info)) {
		DEV_DBG("%s get event info failed........\n", __FUNCTION__);
		return NULL;
	}

	event_info.msg_type = RETURN_EVENT_NAME;
	if(0 == Get_Event_name_Return(&event_info)) {    //if no msg in msg queue,block
		if(0 != event_info.info.count) {
		} else {
			return;
		}
	}

	return NULL;
#else
	return NULL;
#endif
}

void *handler_thread_fun(void* arg)
{
	int status = 0;
	int i =0;
	key_t key = 0;
	int msg_id = -1;
	struct msg_data_buffer handler_data_msg;
	func *current = NULL;
    DEV_DBG("%s %d\n", __FUNCTION__, sig);
    prctl(PR_SET_NAME, "event_receiver");
    char *buf = malloc(MAX_EVENT_DATA);
    if( NULL == buf ) {
    	printf("malloc failed, no memory...\n");
    	return ;
    }

    while(1) {
    	memset(buf, 0, MAX_EVENT_DATA);
    	status = event_get_data(buf, MAX_EVENT_DATA-1, &handler_data_msg);
   	 	if(0 == status) {
    		for(i = 0; i < MAX_HANDLER; i++) {
    			if(0 == strcmp(handler_data_msg.event.name, func_array[i].name)) {
					if(NULL != func_array[i].func_handler_list.func_handler) {
						func_array[i].func_handler_list.func_handler(handler_data_msg.event.name, (void *)buf);
					}
					current = func_array[i].func_handler_list.next;
					while(NULL != current) {
						if(NULL != current->func_handler){
							current->func_handler(handler_data_msg.event.name, (void *)buf);
						}
						current = current->next;
					}
    				break;
    			}
    		}
        	if(i < MAX_HANDLER) {
    	    	if(1 == handler_data_msg.event.need_wait) {
    		    	handler_data_msg.msg_type = handler_data_msg.event.tid;
    		   	 	key = handler_data_msg.event.pid;
    		    	msg_id = msg_queue_create(key);
    		   	 	memcpy(handler_data_msg.event.buf, buf, MAX_EVENT_DATA-1);
    		    	Send_Return_To_Provider(msg_id, &handler_data_msg);
    	    	}
        	}
   		}
    }
    DEV_DBG("%s----%d--signal process end -\n", __FUNCTION__, __LINE__);
    free(buf);
    buf = NULL;
}

/*
*add a new handler register message information to the hash list table
*return 0 if register handler success
*send signal to wakeup eventd process
*fail return -1
*/
int event_register_handler(const char *event, const int priority, void (* handler)(char* event, void *arg))
{
	int res = -1;
	int i = 0;
	func *current = NULL;
	func *new_func = NULL;
	if((NULL == handler) || (NULL == event)) {
		return -1;
	}

	if((EVENT_RPC != priority) && (EVENT_PR0 != priority) && (EVENT_PR1 != priority) && (EVENT_PR2 != priority)) {
		return -1;
	}

	if(handler_num >= MAX_HANDLER) {
		return -1;
	}

	for(i = 0; i < handler_num; i++) {
		if(0 == strcmp(func_array[i].name, event)){
			if(NULL == func_array[i].func_handler_list.func_handler){
				func_array[i].func_handler_list.func_handler = handler;
				func_array[i].func_handler_list.next = NULL;
			} else {
				current = &func_array[i].func_handler_list;
				while(NULL != current->next) {
					current = current->next;
				}
				new_func = (func *)malloc(sizeof(func));
				if(NULL == new_func) {
					return -1;
				}
				new_func->func_handler = handler;
				new_func->next = NULL;
				current->next = new_func;
			}
		}
	}

	strncpy(func_array[handler_num].name, event, strlen(event)+1);
	func_array[handler_num].func_handler_list.func_handler = handler;
	func_array[handler_num].func_handler_list.next = NULL;
	func_array[handler_num].priority = priority;

	res = handler_init();
	if(0 != res) {
		return -1;
	}

	if(0 == handler_ntid) {
		res = pthread_create(&handler_ntid, NULL, handler_thread_fun, NULL);
    	if (0 != res) {
       	 	DEV_DBG("%s\n", strerror(res));
       	 	return res;
    	}
    }

	memset(register_msg.event.name, 0, EVENT_PATH_SIZE);
	register_msg.msg_type = HANDLER_REGISTER_TYPE;
	strncpy(register_msg.event.name, event, strlen(event)+1);
	register_msg.event.pid = getpid();
	register_msg.event.priority = priority;
	register_msg.event.handler_key = getpid();
	register_msg.event.event_type = EVENT_REGISTER;

	DEV_DBG("%s----%d---\n", __FUNCTION__, register_msg.event.handler_key);

	if(0 != Send_Handler_Register_Event(&register_msg)) {
		DEV_DBG("%s send handler register info failed........\n", __FUNCTION__);
		return -1;
	}

	register_msg.msg_type = HANDLER_REGISTER_RETURN;
	if(0 == Read_Handler_Register_Return(&register_msg)) {    //if no msg in msg queue,block
		if(register_msg.event.priority != 0) {
			return -1;
		}
	}

	handler_num++;

	return 0;
}

/*
*delete a handler register message information from the hash list table
*return 0 if register handler success
*send signal to wakeup eventd process
*fail return -1
*/
int event_unregister_handler(const char *event, void (* handler)(char* event, void *arg))
{
	int res = -1;
	int i = 0;
	func *current = NULL;
	func *pre_func = NULL;
	if((NULL == handler) || (NULL == event)) {
		return -1;
	}

	res = handler_init();
	if(0 != res) {
		return -1;
	}

	if(0 == handler_ntid) {
		res = pthread_create(&handler_ntid, NULL, handler_thread_fun, NULL);
    	if (0 != res) {
       	 	DEV_DBG("%s\n", strerror(res));
       	 	return res;
    	}
    }

	memset(register_msg.event.name, 0, EVENT_PATH_SIZE);
	register_msg.msg_type = HANDLER_REGISTER_TYPE;
	strncpy(register_msg.event.name, event, strlen(event)+1);
	register_msg.event.pid = getpid();
	register_msg.event.handler_key = getpid();
	register_msg.event.event_type = EVENT_UNREGISTER;

	DEV_DBG("%s----%d---\n", __FUNCTION__, register_msg.event.handler_key);

	if(0 != Send_Handler_Register_Event(&register_msg)) {
		DEV_DBG("%s send handler register info failed........\n", __FUNCTION__);
		return -1;
	}

	register_msg.msg_type = HANDLER_REGISTER_RETURN;
	if(0 == Read_Handler_Register_Return(&register_msg)) {    //if no msg in msg queue,block
		if(register_msg.event.priority != 0) {
			return -1;
		}
	}

	for(i = 0; i < handler_num; i++) {
		if(0 == strcmp(func_array[i].name, event)){
			if(NULL == func_array[i].func_handler_list.func_handler){
				return 0;
			} else {
				current = &func_array[i].func_handler_list;
				pre_func = current;
				while(NULL != current->next) {
					if(current->func_handler == handler) {
						pre_func->next = current->next;
						current->func_handler = NULL;
						free(current);
						break;
					}
					pre_func = current;
					current = current->next;
				}
			}
		}
	}

	handler_num--;

	return 0;
}

/*
* get eventd process id
* create eventd message queue to transform register information
* create the provider message queue for transform data
*/
int provider_init(void)
{
	if(eventd_pid == -1) {
		get_eventd_pid();
		if(-1 == eventd_pid) {
			return -1;
		}
	}

	if((eventd_provider_key == -1) || (eventd_provider_ID == -1) || (provider_key == -1) || (provider_ID == -1)) {
		eventd_provider_key = eventd_pid;
		eventd_provider_msg_queue_create(eventd_provider_key);      //message queue for register handler
		provider_msg_queue_create(eventd_pid);                      //message queue for read data
		pthread_mutex_init(&mut, NULL);
	}

	return 0;
}

/*
*send provider data to eventd process
*wake up the eventd process
*/
int event_send(const char *event, char *buf, int size)
{
	int i = 0;
	struct timezone tz;
	struct msg_data_buffer provider_data_msg;
	if((event == NULL) || (NULL == buf)) {
		return -1;
	}

	if(size > MAX_EVENT_DATA) {
		return -1;
	}

	if(-1 == provider_init()) {
		return -1;
	}

	memset(provider_data_msg.event.name, 0, EVENT_PATH_SIZE);

	provider_data_msg.msg_type = DATA_TYPE;
	strncpy(provider_data_msg.event.name, event, strlen(event)+1);
	provider_data_msg.event.pid = getpid();
	provider_data_msg.event.datasize = size;
	provider_data_msg.event.need_wait = 0;
	memcpy((char*)(provider_data_msg.event.buf), buf, size);
	gettimeofday(&provider_data_msg.event.send_tv , &tz);  //get time
	
	if(Send_Data_Event_To_Eventd(&provider_data_msg) != 0) {
		return -1;
	}

	return 0;
}

/*
*send provider data to eventd process
*wake up the eventd process
*/
int event_rpc_call(const char *event, char *buf, int size)
{
	int ret = -1;
	int i = 0;
	struct msg_data_buffer provider_data_msg;
	struct timezone tz;
	if((event == NULL) || (buf == NULL)) {
		return -1;
	}

	if(size > MAX_EVENT_DATA) {
		return -1;
	}

	if(-1 == provider_init()) {
		return -1;
	}

	memset(provider_data_msg.event.name, 0, EVENT_PATH_SIZE);

	provider_data_msg.msg_type = DATA_TYPE;
	strncpy(provider_data_msg.event.name, event, strlen(event)+1);
	provider_data_msg.event.pid = getpid();
	provider_data_msg.event.tid = gettid();
	provider_data_msg.event.need_wait = 1;
	provider_data_msg.event.datasize = size;
	memcpy(provider_data_msg.event.buf, buf, size);
	gettimeofday(&provider_data_msg.event.send_tv , &tz);  //get time

	if(Send_Data_Event_To_Eventd(&provider_data_msg) != 0) {
		return -1;
	}

	int msg_id = msg_queue_create(provider_data_msg.event.pid);
	provider_data_msg.msg_type = gettid();

	if(0 == Read_Return_From_Handler(msg_id, &provider_data_msg)) { //if no msg in the queue block

		if(0 == provider_data_msg.event.need_wait) {                //no handler register in eventhub
			ret = -2;
		} else {
			memcpy(buf, provider_data_msg.event.buf, size);
			ret = 0;
		}
	}

//	msg_queue_destroy(msg_id);

	return ret;
}

/*
*get data from the eventd process by handler message queue
*return 0 if success
*return -1 if failed
*/
static int event_get_data(char *buf, int size, struct msg_data_buffer *handler_data_msg)
{
	key_t key;
	int handler_msg_id;

	if((NULL == buf) || (size > MAX_EVENT_DATA)) {
		return -1;
	}

	key = getpid();

	handler_data_msg->msg_type = DATA_TYPE;

	handler_msg_id = msgget(key, IPC_EXCL | IPC_CREAT | 0660);
	if(handler_msg_id < 0){
		handler_msg_id = msgget(key,  0660);
	}

	if(Read_Data_Event_From_Eventd(handler_msg_id, handler_data_msg) != 0) {
		return -1;
	}
	
	memcpy(buf, handler_data_msg->event.buf, size);

	return 0;
}

/*
*
*
*
*/
int event_send_scatter(const char *event, scatter sc[], int count)
{
	int i = 0;
	int size = 0;
	int len = 0;
	struct timezone tz;
	struct msg_data_buffer provider_data_msg;
	for(i = 0; i < count; i++) {
		size += sc[i].len;
	}
	if((event == NULL) || (size > MAX_EVENT_DATA)) {
		return -1;
	}

	if(-1 == provider_init()) {
		return -1;
	}

	len = 0;
	for(i = 0; i < count; i++) {
		if(NULL == sc[i].buf) {
			continue;
		}
		memcpy(provider_data_msg.event.buf + len, sc[i].buf, sc[i].len);
		len += sc[i].len;
	}

	memset(provider_data_msg.event.name, 0, EVENT_PATH_SIZE);

	provider_data_msg.msg_type = DATA_TYPE;
	strncpy(provider_data_msg.event.name, event, strlen(event)+1);
	provider_data_msg.event.pid = getpid();
	provider_data_msg.event.need_wait = 0;
	provider_data_msg.event.datasize = len;
	gettimeofday(&provider_data_msg.event.send_tv , &tz);  //get time

	if(Send_Data_Event_To_Eventd(&provider_data_msg) != 0) {
		return -1;
	}

	return 0;
}


int event_rpc_call_scatter(const char *event, scatter sc[], int count)
{
	int ret = -1;
	int i = 0;
	int size = 0;
	int len = 0;
	struct timezone tz;
	struct msg_data_buffer provider_data_msg;
	for(i = 0; i < count; i++) {
		size += sc[i].len;
	}
	if((event == NULL) || (size > MAX_EVENT_DATA)) {
		return -1;
	}

	if(-1 == provider_init()) {
		return -1;
	}

	len = 0;
	for(i = 0; i < count; i++) {
		if(NULL == sc[i].buf) {
			continue;
		}
		memcpy(provider_data_msg.event.buf + len, sc[i].buf, sc[i].len);
		len += sc[i].len;
	}

	memset(provider_data_msg.event.name, 0, EVENT_PATH_SIZE);

	provider_data_msg.msg_type = DATA_TYPE;
	strncpy(provider_data_msg.event.name, event, strlen(event)+1);
	provider_data_msg.event.pid = getpid();
	provider_data_msg.event.tid = gettid();
	provider_data_msg.event.need_wait = 1;
	provider_data_msg.event.datasize = len;
	gettimeofday(&provider_data_msg.event.send_tv , &tz);  //get time

	if(Send_Data_Event_To_Eventd(&provider_data_msg) != 0) {
		return -1;
	}

	int msg_id = msg_queue_create(provider_data_msg.event.pid);
	provider_data_msg.msg_type = gettid();

	if(0 == Read_Return_From_Handler(msg_id, &provider_data_msg)) { //if no msg in the queue block

		if(0 == provider_data_msg.event.need_wait) {                //no handler register in eventhub
			ret = -2;
		} else {
			len = 0;
			DEV_DBG("%s----%d---\n", __FUNCTION__, __LINE__);
			for(i = 0; i < count; i++) {
				memcpy(sc[i].buf, provider_data_msg.event.buf + len, sc[i].len);
				len += sc[i].len;
			}
			ret = 0;
		}
	}

//	msg_queue_destroy(msg_id);

	return ret;
}
