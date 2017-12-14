#include "list.h"

Reback_Node *Reback_data_list_head = NULL;

Node *data_list_head = NULL;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t register_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t read_data = PTHREAD_COND_INITIALIZER;
pthread_cond_t send_data = PTHREAD_COND_INITIALIZER;

int data_trans = 0;
int data_trans1 = 0;
int data_trans2 = 0;


/* table for search quickly */
struct eventd_register_list event_register_table[MAX_EVENT] = {0}; 

/*
*sigusr1 
*/
static void eventd_serv_signal_handler(int sig)
{
    SERVER_DBG("%s  %d  eventd_server caught signal %d\n", __FUNCTION__, __LINE__, sig);
    if(SIGUSR1 == sig) {
    	pthread_mutex_lock(&mut);
    	data_trans = 1;
    	pthread_cond_signal(&register_cond);
    	pthread_mutex_unlock(&mut);
    } else if(SIGUSR2 == sig) {
    	
    }
    return;
}

/*
* init signal 
*/
static void eventd_serv_init_signal()
{
    signal(SIGUSR1, eventd_serv_signal_handler);
    signal(SIGUSR2, eventd_serv_signal_handler);
}

/*
*and a new handler to the handler list
*/
static int add_new_handler_to_list(int i, struct event_register_data *event)
{
	int ret = 0;
	int status = 0;
	if((i < 0) || (i >= MAX_EVENT)) {
		return -1;	
	}
	struct handler_list *new_handler = NULL;

	if(NULL == event_register_table[i].handler_head) {
		SERVER_DBG("----%s--%d--i:%d.....\n",__FUNCTION__, __LINE__, i);
		ret  = createHandlerHead(i);
		if(0 != ret) {
			return -1;
		}
	}
	
	if((EVENT_RPC == event_register_table[i].handler_priority) && (event_register_table[i].handler_num >= 1)) {
		SERVER_DBG("----%s....%d.......\n",__FUNCTION__, __LINE__);
		if(event_register_table[i].rpc_handler_pid == event->pid) {
			return 0;
		} else {
			SERVER_DBG("----%s....%d.......\n",__FUNCTION__, __LINE__);
			status = kill(event_register_table[i].rpc_handler_pid, 0);
			if((-1 == status) && (ESRCH == errno)) {
				SERVER_DBG("----%s....%d..have the same rpc handler.....\n",__FUNCTION__, __LINE__);
				deleteHandler(i, event_register_table[i].handler_head->next);
				event_register_table[i].handler_priority = 0;
				event_register_table[i].handler_num = 0;		
			} else {
				return -1;
			}
		}
	} else if((event_register_table[i].handler_num >= 1) && (EVENT_RPC == event->priority)) {
		SERVER_DBG("----%s....%d.......\n",__FUNCTION__, __LINE__);
		if(event_register_table[i].rpc_handler_pid == event->pid) {
			return 0;
		} else {
			SERVER_DBG("----%s....%d.......\n",__FUNCTION__, __LINE__);
			status = kill(event_register_table[i].rpc_handler_pid, 0);
			if((-1 == status) && (ESRCH == errno)) {
				SERVER_DBG("----%s....%d..have the same rpc handler.....\n",__FUNCTION__, __LINE__);
				deleteHandler(i, event_register_table[i].handler_head->next);
				event_register_table[i].handler_priority = 0;
				event_register_table[i].handler_num = 0;		
			} else {
				return -1;
			}
		}
	} 
	
	SERVER_DBG("----%s-%d-add new handler to list.....\n",__FUNCTION__, __LINE__);
	
	new_handler = (struct handler_list *)malloc(sizeof(struct handler_list));
	if(NULL == new_handler) {
		return -1;
	}
	memset(new_handler, 0, sizeof(struct handler_list));
	strncpy(new_handler->event_name, event->name, strlen(event->name)+1);
	new_handler->handler_pid = event->pid;
	status = msgget(event->handler_key, IPC_EXCL | IPC_CREAT | 0660);
	if(status < 0) {
		status = msgget(event->handler_key,  0660);
	}
	new_handler->handler_id = status;
	new_handler->priority = event->priority;
	new_handler->next = NULL;
	event_register_table[i].handler_num++;
	event_register_table[i].handler_priority = event->priority;
	
	if(EVENT_RPC == event->priority) {
		event_register_table[i].rpc_handler_pid = event->pid;
	}
	
	ret = addHandler(i, new_handler);
	if(ret != 0) {
		SERVER_DBG("----%s--%d----handler list have the same pid handler-\n",__FUNCTION__, __LINE__);
		free(new_handler);
		new_handler = NULL;
	}	
	
	return 0;	
}

/*
* register a handler
*/
static int register_handler(struct event_register_data *event)
{
	int i = 0;
	int status = 0;
	SERVER_DBG("----%s--%d--\n",__FUNCTION__, __LINE__);
	for(i = 0; i < MAX_EVENT; i++) {
		if((0 == event_name_list[i][0]) || (0 == strcmp(event_name_list[i], event->name))) {
			if(0 == event_name_list[i][0]) {
				strncpy(event_name_list[i], event->name, strlen(event->name)+1);
			}
			SERVER_DBG("%s..%d..the list have handler....\n", __FUNCTION__, __LINE__);
			status = add_new_handler_to_list(i, event);
			SERVER_DBG("%s...%d...  status:%d....\n", __FUNCTION__, __LINE__, status);
			break;	
		}
	}
	return status;
}

/*
* the loop
*/
static void eventd_serv_loop()
{
	SERVER_DBG("the main loop...........\n");

    while(1){	
    	pause();
		SERVER_DBG("%s %d wake up by handler or provider process.......\n", __FUNCTION__, __LINE__);
//		pthread_kill(register_ntid, SIGUSR1);
//		msleep(10);
//		pthread_kill(receiver_data_ntid, SIGUSR1);
/*		pthread_mutex_lock(&mut);
    	data_trans = 1;
    	pthread_cond_signal(&register_cond);
    	pthread_mutex_unlock(&mut);
*/    	
    }
    
}

void * register_thread_fun(void *arg)
{
	int err,status;
	int msg_id = -1;
	struct msg_register_buffer register_data;

	while(1) {
//	    pause();
		pthread_mutex_lock(&mut);
    	while(data_trans != 1)
    	{
      		pthread_cond_wait(&register_cond, &mut);
    	}
    	pthread_mutex_unlock(&mut);
    	
		SERVER_DBG("%s   wakeup by the main process..\n", __FUNCTION__);

    	register_data.msg_type = HANDLER_REGISTER_TYPE;
		SERVER_DBG("%s   type:%d..  \n", __FUNCTION__, (int)register_data.msg_type);
    	err = Read_Handler_Register_Event(&register_data);
    	if(0 == err) {    
    		pthread_mutex_lock(&register_list_mutex);		
    		status = register_handler(&(register_data.event));	
    			
			if(0 != status) {
    			register_data.event.priority = status;	                 //register fail
    		} else {
    			register_data.event.priority = 0;                        //register success
    		}
    		register_data.msg_type = HANDLER_REGISTER_RETURN;
    		msg_id = msg_queue_create(register_data.event.pid);
    		Send_Provider_Register_Return(msg_id, &register_data);       //return the register info
    		pthread_mutex_unlock(&register_list_mutex);
    		kill(register_data.event.pid, SIGUSR1);
    		SERVER_DBG("%s  %d  register one handler success..\n", __FUNCTION__, __LINE__); 			
    	} else {
    		SERVER_DBG("%s  %d  no register handler receive..\n", __FUNCTION__, __LINE__);
//			pthread_kill(receiver_data_ntid, SIGUSR1);
			pthread_mutex_lock(&mut);
    		data_trans1 = 1;
    		pthread_cond_signal(&read_data);
    		pthread_mutex_unlock(&mut);
    	}
    	
    	pthread_mutex_lock(&data_list_mutex);
		if(NULL != data_list_head->pNext) {
			SERVER_DBG("%s.....%d..have data in the data list..\n", __FUNCTION__, __LINE__);
			pthread_mutex_unlock(&data_list_mutex);	
//			pthread_kill(send_data_ntid, SIGUSR1);
			pthread_mutex_lock(&mut);
    		data_trans2 = 1;
    		pthread_cond_signal(&send_data);
    		pthread_mutex_unlock(&mut);
		} else {					
			pthread_mutex_unlock(&data_list_mutex);	
		}
		
		pthread_mutex_lock(&mut);
    	data_trans = 0;
    	pthread_mutex_unlock(&mut);	 
    }
}

void * rec_data_thread_fun(void *arg)
{
	int err;
	struct msg_data_buffer provider_data;
	int priority;
	Node *new_list = NULL;
	
	while(1) {		

		pthread_mutex_lock(&mut);
    	while(data_trans1 != 1)
    	{
      		pthread_cond_wait(&read_data, &mut);
    	}
    	pthread_mutex_unlock(&mut);
//		pause();
		SERVER_DBG("%s   wakeup by the main process..\n", __FUNCTION__);
	
		provider_data.msg_type = DATA_TYPE;

		err = Read_Data_Event_From_Provider(&provider_data);
		
		SERVER_DBG("%s.....%d....\n", __FUNCTION__, __LINE__);

		if(0 == err) {
			new_list = malloc(sizeof(struct data_list));
	
			if(NULL == new_list) {
				SERVER_DBG("%s %d malloc new data list failed.........\n", __FUNCTION__, __LINE__);
				continue;	
			}
	
			SERVER_DBG("%s.....%d..receive data from provider..\n", __FUNCTION__, __LINE__);
			memset(new_list, 0, sizeof(struct data_list));
			strncpy(new_list->event_name, provider_data.event.name, MAX_EVENT_NAME);
			memcpy(new_list->data, provider_data.event.buf, MAX_EVENT_DATA);
			new_list->need_wait = provider_data.event.need_wait;
			new_list->pid = provider_data.event.pid;
			new_list->priority = priority;
			new_list->time = provider_data.event.send_tv;
			new_list->datasize = provider_data.event.datasize;
			
			pthread_mutex_lock(&data_list_mutex);

			addNode(new_list);
				
			pthread_mutex_unlock(&data_list_mutex);
			SERVER_DBG("%s---%d add data to data list end ..\n", __FUNCTION__, __LINE__);
//			pthread_kill(send_data_ntid, SIGUSR1);
			pthread_mutex_lock(&mut);
    		data_trans2 = 1;
    		pthread_cond_signal(&send_data);
    		pthread_mutex_unlock(&mut);
		}else{
			SERVER_DBG("%s.....%d..no receive any data form provider..\n", __FUNCTION__, __LINE__);
			pthread_mutex_lock(&data_list_mutex);
			if(NULL != data_list_head->pNext) {
				SERVER_DBG("%s.....%d..have data in the data list..\n", __FUNCTION__, __LINE__);
				pthread_mutex_unlock(&data_list_mutex);
//				pthread_kill(send_data_ntid, SIGUSR1);
				pthread_mutex_lock(&mut);
    			data_trans2 = 1;
    			pthread_cond_signal(&send_data);
    			pthread_mutex_unlock(&mut);
			} else {
				pthread_mutex_unlock(&data_list_mutex);
			}									
		}
		
		pthread_mutex_lock(&mut);
    	data_trans1 = 0;
    	pthread_mutex_unlock(&mut);	
	}
}

void * send_data_thread_fun(void *arg)
{
	int err;
	int status = 0;
	struct msg_data_buffer provider_data;
	Node *current_list = NULL;
	struct handler_list *current_handler_list = NULL;
	Provider_info info;
	while(1) {
		SERVER_DBG("%s---%d begin pause ..\n", __FUNCTION__, __LINE__);
//		pause();
		pthread_mutex_lock(&mut);
    	while(data_trans2 != 1)
    	{
      		pthread_cond_wait(&send_data, &mut);
    	}
    	pthread_mutex_unlock(&mut);
		SERVER_DBG("%s   %d wakeup by the main process..\n", __FUNCTION__, __LINE__);
		pthread_mutex_lock(&data_list_mutex);
		pthread_mutex_lock(&register_list_mutex);
		
		SERVER_DBG("%s.....%d....\n", __FUNCTION__, __LINE__);
	
		while(NULL != (data_list_head->pNext)) {
		
			SERVER_DBG("%s %d data_list_head is not NULL..\n", __FUNCTION__, __LINE__);	
					
			current_list = data_list_head->pNext;

			int i = 0;
			for(i = 0; i < MAX_EVENT; i++) {
				if(strcmp(current_list->event_name, event_name_list[i]) == 0) {
					break;	
				}	
			}
			
			SERVER_DBG("%s %d ..i:%d........\n", __FUNCTION__, __LINE__, i);	
	
			if(MAX_EVENT == i) {
				pthread_mutex_unlock(&data_list_mutex);
				pthread_mutex_unlock(&register_list_mutex);
				continue;	
			}
			
			SERVER_DBG("%s %d ..........\n", __FUNCTION__, __LINE__);
		
			int current_time = 0;
			int max_time = 0;
			int min_time = 0;
			current_handler_list = event_register_table[i].handler_head;
			if((NULL == current_handler_list) && (1 == current_list->need_wait)) {
				SERVER_DBG("%s %d ..no handler registered........\n", __FUNCTION__, __LINE__);	
				provider_data.msg_type = DATA_FROM_HANDLER;
				key_t key = current_list->pid;
    		    int msg_id = msg_queue_create(key);
				memset(provider_data.event.name, 0, MAX_EVENT_NAME);
				strncpy(provider_data.event.name, current_list->event_name, strlen(current_list->event_name)+1);
				memcpy(provider_data.event.buf, current_list->data, MAX_EVENT_DATA);
				Send_Return_To_Provider(msg_id, &provider_data);
			}	
			while(NULL != current_handler_list) {
				SERVER_DBG("%s %d ..have handler registered........\n", __FUNCTION__, __LINE__);
				provider_data.msg_type = DATA_TYPE;
				provider_data.event.pid = current_list->pid;
				provider_data.event.need_wait = current_list->need_wait;
				memset(provider_data.event.name, 0, MAX_EVENT_NAME);
				strncpy(provider_data.event.name, current_list->event_name, strlen(current_list->event_name)+1);
				memcpy(provider_data.event.buf, current_list->data, MAX_EVENT_DATA);

				do {
					status = Send_Data_Event_To_Handler(current_handler_list->handler_id, &provider_data);	
					if(0 != status) {
						SERVER_DBG("%s-%d-send data -failed....\n", __FUNCTION__, __LINE__);
					}
				}while(0 != status);	

				kill(current_handler_list->handler_pid, SIGUSR1);
				current_handler_list = current_handler_list->next;
				
				struct timeval tv;
				get_time(&tv);
				int ms = (tv.tv_sec - current_list->time.tv_sec)*1000 + (tv.tv_usec - current_list->time.tv_usec)/1000;
				if(ms > max_time) {
					max_time = ms;
				}
				if(ms < min_time) {
					min_time = ms;
				}
				current_time = ms;
			}
			
			strncpy(info.event_name, current_list->event_name, strlen(current_list->event_name)+1);
			info.provider_pid = current_list->pid;
			info.current_time = current_time;
			info.min_time = min_time;
			info.max_time = max_time;
			info.datasize = current_list->datasize;
			info.flag = current_list->need_wait;
			Generate_stat_info(info); 

			deleteNode(current_list);
		}
		SERVER_DBG("%s.....%d....\n", __FUNCTION__, __LINE__);	
		pthread_mutex_unlock(&register_list_mutex);
		pthread_mutex_unlock(&data_list_mutex);
		SERVER_DBG("%s---%d process on time end ..\n", __FUNCTION__, __LINE__);
//		pthread_kill(receiver_data_ntid, SIGUSR1);
		pthread_mutex_lock(&mut);
    	data_trans1 = 1;
    	pthread_cond_signal(&read_data);
    	pthread_mutex_unlock(&mut);
    	
    	if(NULL != data_list_head->pNext) {
			SERVER_DBG("%s.....%d..have data in the data list..\n", __FUNCTION__, __LINE__);
			pthread_mutex_unlock(&data_list_mutex);
//			pthread_kill(send_data_ntid, SIGUSR1);
			pthread_mutex_lock(&mut);
    		data_trans2 = 1;
    		pthread_cond_signal(&send_data);
    		pthread_mutex_unlock(&mut);
		} else {
			pthread_mutex_lock(&mut);
    		data_trans2 = 0;
    		pthread_mutex_unlock(&mut);
		}	
	}
}

void * stat_thread_fun(void *arg)
{

}

/*
* server init
*/
static int eventd_serv_init()
{     
	eventd_handler_key = eventd_provider_key = getpid();
	eventd_handler_ID = eventd_handler_msg_queue_create(getpid());
	
	eventd_provider_ID = eventd_provider_msg_queue_create(getpid());

	provider_ID = provider_msg_queue_create(getpid());
	
    return 0;
}

/*
*server destroy
*/
static void eventd_serv_exit()
{
   eventd_handler_msg_queue_destroy();
   
   eventd_provider_msg_queue_destroy();
}

static int destroy_handler_list(int i)
{
	if(NULL == event_register_table[i].handler_head) {  
        return 0;  
    }  
	
    if(NULL == event_register_table[i].handler_head->next) {  
        free(event_register_table[i].handler_head);  
        event_register_table[i].handler_head = NULL;  
        return 0;  
    }  
    struct handler_list* p = event_register_table[i].handler_head->next;  
    while(NULL != p) {  
        struct handler_list* tmp = p;  
        p = p->next;  
        free(tmp);  
    }  
    free(event_register_table[i].handler_head);  
    event_register_table[i].handler_head = NULL; 
}

static int destroy_register_list()
{
	int i = 0;
	for(i = 0; i < MAX_EVENT; i++) {
		if(event_name_list[i][0] != 0) {
			destroy_handler_list(i);		
		}
	}
}

static int destroy_data_list()
{
    if(NULL == data_list_head) {  
        return -1;  
    }  
	
    if(NULL == data_list_head->pNext) {  
        free(data_list_head);  
        data_list_head = NULL;  
        return 0;  
    }  
    Node* p = data_list_head->pNext;  
    while(NULL != p) {  
        Node* tmp = p;  
        p = p->pNext;  
        free(tmp);  
    }  
    free(data_list_head);  
    data_list_head = NULL;  
    return 0; 
}

static int create_threads(void)
{
	int err; 
	
	pthread_mutex_init(&register_list_mutex, NULL); 
	
	pthread_mutex_init(&data_list_mutex, NULL); 
	
	pthread_mutex_init(&mut, NULL);
	
	if (pthread_cond_init(&register_cond, NULL) != 0)
  	{
    	printf("cond init error\n");
  	}
  	
  	if (pthread_cond_init(&read_data, NULL) != 0)
  	{
    	printf("cond init error\n");
  	}
  	
  	if (pthread_cond_init(&send_data, NULL) != 0)
  	{
    	printf("cond init error\n");
  	}
	 
    err = pthread_create(&register_ntid, NULL, register_thread_fun, NULL);  
    if (0 != err) {  
        SERVER_DBG("%s\n", strerror(err)); 
        return err; 
    } 	
    SERVER_DBG("create register thread success,ntid:0x%x......\n", getpid());
    err = pthread_create(&receiver_data_ntid, NULL, rec_data_thread_fun, NULL);  
    if (0 != err)   
    {  
        SERVER_DBG("%s\n", strerror(err));  
        return err;
    } 

	SERVER_DBG("create receiver data thread success,ntid:0x%x.......\n",getpid());
    
    err = pthread_create(&send_data_ntid, NULL, send_data_thread_fun, NULL);  
    if (0 != err)   
    {  
        SERVER_DBG("%s\n", strerror(err));
        return err;  
    }

	SERVER_DBG("create send data thread success, ntid:0x%x.........\n",getpid());
    
	err = pthread_create(&stat_ntid, NULL, stat_thread_fun, NULL);
	if(0 != err)
	{
		SERVER_DBG("%s\n", strerror(err));
		return err;
	}


	SERVER_DBG("create stat thread success, ntid:0x%x.........\n",getpid());

    return err;
}

static int destroy_threads(void)
{
	pthread_join(register_ntid, NULL);
	pthread_join(receiver_data_ntid, NULL);
	pthread_join(send_data_ntid, NULL);


    pthread_mutex_destroy(&register_list_mutex);
    pthread_mutex_destroy(&data_list_mutex);   	
}

/*
* the main function 
*/
int main(int argc, char *argv[])
{
	//init signal
	SERVER_DBG("start event server...........");
    eventd_serv_init_signal();

    //init
    if (eventd_serv_init()) {
        SERVER_DBG("eventd server init failed\n");
        return -1;
    }

	createNodeList();
    
    create_threads();

    // loop
    eventd_serv_loop();
    
    destroy_threads();

    // quit
    eventd_serv_exit();

	destroy_register_list();  //destroy register list handler and provider

	destroy_data_list();  //destroy data list buffer
	
	destroy_stat();
    return 0;
}



