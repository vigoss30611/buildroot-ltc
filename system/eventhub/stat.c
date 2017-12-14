
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/time.h>
#include <wait.h>
#include "stat.h"

Provider_info *info_head = NULL;

Listener_info *listener_head = NULL;

#define MAX_PATH   255

/*
* create provider statistic info list hear
*/
int create_info_head()
{
	info_head = (Provider_info*) malloc(sizeof(Provider_info));
    if(NULL == info_head) {
        return -1;
    } else {
        memset(info_head, 0, sizeof(Provider_info));
        return 0;
    }

}

int create_linstener_head()
{
	listener_head = (Listener_info*) malloc(sizeof(Listener_info));
    if(NULL == listener_head) {
        return -1;
    } else {
        memset(listener_head, 0, sizeof(Provider_info));
        return 0;
    }
}

static int add_info(Provider_info* node)
{
	if((NULL == info_head) || (NULL == node)) {
        return -1;
    }
    Provider_info* p = info_head->pNext;
    Provider_info* q = info_head;
    while(NULL != p) {
        q = p;
        p = p->pNext;
    }
    q->pNext = node;
	node->pNext = NULL;
    return 1;
}

static int add_listener_info(Listener_info* node)
{
	if((NULL == listener_head) || (NULL == node)) {
        return -1;
    }
    Listener_info* p = listener_head->pNext;
    Listener_info* q = listener_head;
    while(NULL != p) {
        q = p;
        p = p->pNext;
    }
    q->pNext = node;
	node->pNext = NULL;
    return 1;
}

static Provider_info* search_stat_list(const char *name, pid_t pid)
{
	Provider_info* p = info_head->pNext;
    while(NULL != p) {
		if((0 == strcmp(p->event_name, name)) && (p->provider_pid == pid)) {
			return p;
		}
        p = p->pNext;
    }
	return NULL;
}

static Listener_info* search_listener_list(const char *name, pid_t pid)
{
	Listener_info* p = listener_head->pNext;
    while(NULL != p) {
		if((0 == strcmp(p->event_name, name)) && (p->handler_pid == pid)) {
			return p;
		}
        p = p->pNext;
    }
	return NULL;
}

int Pid_to_name(char *name, pid_t pid)
{
	int sz;
	FILE *fp;
	char filename[sizeof("readlink /proc/%u/exe") + sizeof(int)*3];
	char filetext[MAX_PATH] = {'\0'};
	char* pos;
	char* pos1;

	sprintf(filename, "readlink /proc/%u/exe", pid);
	fp = popen(filename, "r");
	if (NULL != fp) {
		if(NULL != fgets(filetext, MAX_PATH, fp)) {//read
			pos = strrchr(filetext,'/');
			if(pos == 0) {
        		strcpy(name,filetext);
    		} else {
        		strcpy(name,pos+1);
    		}
    	}
    	pclose(fp);
	}
}

int Generate_stat_info(Provider_info info)
{
	if(NULL == info_head) {
		create_info_head();
	}
	Provider_info* p = NULL;
	p = search_stat_list(info.event_name, info.provider_pid);

	if(NULL != p) {
		p->count = p->count + 1;
		p->current_time = info.current_time;
		if(info.max_time > p->max_time) {
			p->max_time = info.max_time;
		}
		if(info.min_time < p->min_time) {
			p->min_time = info.min_time;
		}
		return 0;
	} else {
		p = (Provider_info *)malloc(sizeof(Provider_info));
		if(NULL == p) {
			return -1;
		} else {
			strncpy(p->event_name, info.event_name, strlen(info.event_name) + 1);
			Pid_to_name(p->p_name, info.provider_pid);
			p->p_name[MAX_NAME-1] = 0;
			if(0x0a == p->p_name[strlen(p->p_name)-1]) {
				p->p_name[strlen(p->p_name)-1] = 0;
			}
			p->provider_pid = info.provider_pid;
			p->count = 1;
			p->current_time = info.current_time;
			p->max_time = info.max_time;
			p->min_time = info.min_time;
			p->datasize = info.datasize;
			p->flag = info.flag;
			p->pNext = NULL;
			add_info(p);
			return 0;
		}
	}
}

int Generate_linstener_info(const char *name, pid_t pid)
{
	Listener_info* p = NULL;

	if(NULL == listener_head) {
		create_linstener_head();
	}
	p = search_listener_list(name, pid);

	if(NULL != p) {
		p->count = p->count + 1;
		return 0;
	} else {
		p = (Listener_info *)malloc(sizeof(Listener_info));
		if(NULL == p) {
			return -1;
		} else {
			strncpy(p->event_name, name, strlen(name));
			Pid_to_name(p->p_name, p->handler_pid);
			p->p_name[MAX_NAME - 1] = 0;
			p->handler_pid = pid;
			p->count = 1;
			add_listener_info(p);
			return 0;
		}
	}
}

static int destroy_stat_list()
{
	if(NULL == info_head) {
        return -1;
    }

    if(NULL == info_head->pNext) {
        free(info_head);
        info_head = NULL;
        return 0;
    }
    Provider_info* p = info_head->pNext;
    while(NULL != p) {
        Provider_info* tmp = p;
        p = p->pNext;
        free(tmp);
    }
    free(info_head);
    info_head = NULL;
    return 0;
}

static int destroy_listener_list()
{
	if(NULL == listener_head) {
        return -1;
    }

    if(NULL == listener_head->pNext) {
        free(listener_head);
        listener_head = NULL;
        return 0;
    }
    Listener_info* p = listener_head->pNext;
    while(NULL != p) {
        Listener_info* tmp = p;
        p = p->pNext;
        free(tmp);
    }
    free(listener_head);
    listener_head = NULL;
    return 0;
}

int destroy_stat()
{
	destroy_stat_list();
	destroy_listener_list();
}

int get_time(struct timeval *tv)
{
	struct timezone tz;
	gettimeofday (tv , &tz);  //get time
}

int printf_stat_info()
{
	Provider_info *p = NULL;
	Listener_info *q =NULL;

	if(NULL != info_head) {
    	printf("EVENT\t\t\tPROVIDER\tCOUNT\tPSIZE\tTIME/MIN--MAX(ms)\t\t\tFLAG \n");
    	printf("---------------------------------------------------------------------------------------------------------\n");
    	p = info_head->pNext;
    	while(NULL != p) {
    		if(1 == p->flag) {
    			printf("%-32s %-16s(%-5d) \t%-9d %-9d %d/%d--%-16d RPC  \n", p->event_name, p->p_name, p->provider_pid, p->count,
    		   		p->datasize, p->current_time, p->min_time, p->max_time);
    		} else {
    			printf("%-32s %-16s(%-5d) \t%-9d %-9d %d/%-16d NO_RPC  \n", p->event_name, p->p_name, p->provider_pid, p->count,
    		   		p->datasize, p->current_time, p->max_time);
    		}

    		p = p->pNext;
    	}
    }

    printf("\n");
    printf("\n");

    if(NULL != listener_head) {
    	printf("LISTENER\tEVENT\tCOUNT \n");
    	printf("-----------------------------------------------------------\n");
    	q = listener_head->pNext;
    	while(NULL != q) {
    		printf("%s(%d)\t%s\t%d   \n", q->p_name, q->handler_pid, q->event_name, q->count);
    		q = q->pNext;
    	}
    }
}
