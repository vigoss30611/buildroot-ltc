
/*
===============================================================================
SOURCE FILE:    message_queue_interface.h

REVISIONS:      Mar 11, 2016        (jicky)
                    define the message queue function
PROGRAMMER:     jicky
===============================================================================
*/

#ifndef __MESSAGE_QUEUE_INTERFACE_H
#define __MESSAGE_QUEUE_INTERFACE_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mqueue.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/errno.h>
#include <wait.h>
#include "message_queue.h"

/*
*create the register handler message queue
*return 0 if success
*return -1 if failed
*/
int eventd_handler_msg_queue_create(key_t key);

/*
*create handler message queue for read data from eventd in handler process or write data to handler
*in eventd process
*return 0 if success
*return -1 if failed
*/
int handler_msg_queue_create();

/*
*create register provider message queue between evented and provider process
*return 0 if success
*return -1 if failed
*/
int eventd_provider_msg_queue_create(key_t key);

/*
*create message queue for change data between eventd and provider process
*return 0 if success
*return -1 if failed
*/
int provider_msg_queue_create(key_t key);

/*
* read message from the message queue in eventd process about handler register information
* return 0 if success
* return -1 if failed
*/
int Read_Handler_Register_Event(struct msg_register_buffer* handler_register_msg);

/*
* read message from the message queue int eventd process about provider register information
* return 0 if success
* return -1 if failed
*/
int Read_Provider_Register_Event(struct msg_register_buffer* provider_register_msg);

/*
* send data to message queue in handler process about handler register information
* return 0 if success
* return -1 if failed
*/
int Send_Handler_Register_Event(struct msg_register_buffer* handler_register_msg);

/*
* send provider data to message queue in provider process about provider register information
* return 0 if success
* return -1 if failed
*/
int Send_Provider_Register_Event(struct msg_register_buffer* provider_register_msg);

/*
* read data from message queue in handler process, the data from eventd process
* return 0 if success
* return -1 if failed
*/
int Read_Data_Event_From_Eventd(int msg_id, struct msg_data_buffer* handler_data_msg);

/*
* read message from the eventd message queue in eventd process,the data is form provider process,
* return 0 if success
* return -1 if failed
*/
int Read_Data_Event_From_Provider(struct msg_data_buffer* provider_data_msg);

/*
* write message data to the eventd message queue in provider process
* the message data read by eventd process
* return 0 if success
* return -1 if failed
*/
int Send_Data_Event_To_Eventd(struct msg_data_buffer* provider_data_msg);

/*
* write message to the handler message queue by eventd process
* the data is read by handler process
* return 0 if success
* return -1 if failed
*/
int Send_Data_Event_To_Handler(int msg_id, struct msg_data_buffer* handler_data_msg);

/*  destroy the handler message queue */
int eventd_handler_msg_queue_destroy();

/*  destroy the eventd message queue */
int eventd_provider_msg_queue_destroy();

/* destroy the handler data message queue */
int handler_data_msg_queue_destroy();

/* destroy the provider data message queue */
int provider_data_msg_queue_destroy();

/* destroy the specified message queue indicated by msg_ID */
int msg_queue_destroy(int msg_ID);

int Read_Provider_Register_Return(int msg_id, struct msg_register_buffer* provider_register_return);

int Send_Provider_Register_Return(int msg_id, struct msg_register_buffer* provider_register_return);

int Read_Handler_Register_Return(struct msg_register_buffer* handler_register_return);

int Send_Handler_Register_Return(int msg_id, struct msg_register_buffer* handler_register_return);

int Send_Return_To_Provider(int msg_id, struct msg_data_buffer* handler_data_msg);

int Read_Return_From_Handler(int msg_id, struct msg_data_buffer* handler_data_msg);

/*
成功执行时，msgsnd()返回0，msgrcv()返回拷贝到mtext数组的实际字节数。失败两者都返回-1，errno被设为以下的某个值
[对于msgsnd]
EACCES：调用进程在消息队列上没有写权能，同时没有CAP_IPC_OWNER权能
EAGAIN：由于消息队列的msg_qbytes的限制和msgflg中指定IPC_NOWAIT标志，消息不能被发送
EFAULT：msgp指针指向的内存空间不可访问
EIDRM：消息队列已被删除
EINTR：等待消息队列空间可用时被信号中断
EINVAL：参数无效
ENOMEM：系统内存不足，无法将msgp指向的消息拷贝进来
[对于msgrcv]
E2BIG：消息文本长度大于msgsz，并且msgflg中没有指定MSG_NOERROR
EACCES：调用进程没有读权能，同时没具有CAP_IPC_OWNER权能
EAGAIN：消息队列为空，并且msgflg中没有指定IPC_NOWAIT
EFAULT：msgp指向的空间不可访问
EIDRM：当进程睡眠等待接收消息时，消息已被删除
EINTR：当进程睡眠等待接收消息时，被信号中断
EINVAL：参数无效
ENOMSG：msgflg中指定了IPC_NOWAIT，同时所请求类型的消息不存在
*/

#endif
