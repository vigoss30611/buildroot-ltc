#ifndef __STAT_H
#define __STAT_H

#define MAX_NAME 32

typedef struct stat_info
{
	char event_name[MAX_NAME];  //the event name
	char p_name[MAX_NAME];      //process name
	pid_t provider_pid;          //the provider process pid
	unsigned int count;				//the provider process send event count
	unsigned int current_time;      //the last time
	unsigned int max_time;          //the max time
	unsigned int min_time;		   //the min time
	unsigned int datasize;          //the size of send data
	char flag;
	struct stat_info *pNext; //point to the next provider

}Provider_info;

typedef struct listener_info
{
	char event_name[MAX_NAME];
	char p_name[MAX_NAME];
	pid_t handler_pid;
	int count;
	struct listener_info *pNext;
}Listener_info;

int create_info_head();

int create_linstener_head();

int Generate_stat_info(Provider_info info);

int Generate_linstener_info(const char *name, pid_t pid);

int get_time(struct timeval *tv);

int destroy_stat();

int Pid_to_name(char *name, pid_t pid);

int printf_stat_info();

#endif
