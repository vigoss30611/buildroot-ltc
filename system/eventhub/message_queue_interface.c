/*
===============================================================================
DATE:           3 10, 2016

REVISIONS:      3 11, 2016        (jicky)
                handler,Provider message create, destroy, read or write function for message
for one listener process have three message queue

1:eventd message queue for register handler ,provider, and provider send data to eventd from provider.

2:handler message queue for read data form eventd in handler process and send data from eventd to handler      
===============================================================================
*/

#include "message_queue_interface.h"

#define MSG_LOG_OPEN 0
#if MSG_LOG_OPEN
	#define MSG_DBG(arg...) printf("[message_queue_interface]" arg)
#else
	#define MSG_DBG(arg...)
#endif

/*
*create the register handler message queue
*return 0 if success
*return -1 if failed
*/
int eventd_handler_msg_queue_create(key_t key)
{
	if((-1 == eventd_handler_ID) || (0 == eventd_handler_ID)) {
		MSG_DBG("%s...eventd_handler_key:%d...\n", __FUNCTION__, key);
		eventd_handler_ID = msgget(key, IPC_EXCL | IPC_CREAT | 0777);
		if(eventd_handler_ID < 0) {
			switch(errno) {
				case EEXIST:
					eventd_handler_ID = msgget(key, 0777);
					if(eventd_handler_ID < 0) {
						MSG_DBG("%s  failed.....\n", __FUNCTION__);
						return -1;
					}
					break;
				case EIDRM:
					MSG_DBG("%s  Queue is marked for deletion.....\n", __FUNCTION__);
					break;
				case ENOENT:
					MSG_DBG("%s  Queue does not exist......\n", __FUNCTION__);
					break;
				case ENOMEM:
					MSG_DBG("%s  Not enough memory to create queue...\n", __FUNCTION__);
					break;
				case ENOSPC:
					MSG_DBG("%s  Maximum queue limit exceeded...\n", __FUNCTION__);
					break;
				default:
					break;
			}
		}
	}

	MSG_DBG("%s...eventd_handler_id:%d...\n", __FUNCTION__, eventd_handler_ID);

	return eventd_handler_ID;
}

/*
*create handler message queue for read data from eventd in handler process or write data to handler
*in eventd process
*return 0 if success
*return -1 if failed
*/
int handler_msg_queue_create()
{
	if(-1 == handler_key) {
		handler_key = getpid();  //get handler or provider pid for message key
		if(handler_key < 0) {
			MSG_DBG(" %s create message key failed......\n",__FUNCTION__);
			return -1;
		}

		handler_ID = msgget(handler_key, IPC_EXCL | IPC_CREAT | 0777);
		if(handler_ID < 0) {
			switch(errno) {
				case EEXIST:
					handler_ID = msgget(handler_key, 0777);
					if(handler_ID < 0) {
						MSG_DBG(" %s create message queue failed.....\n",__FUNCTION__);
						return -1;
					}
					break;
				case EIDRM:
					MSG_DBG("%s  Queue is marked for deletion.....\n", __FUNCTION__);
					break;
				case ENOENT:
					MSG_DBG("%s  Queue does not exist......\n", __FUNCTION__);
					break;
				case ENOMEM:
					MSG_DBG("%s  Not enough memory to create queue...\n", __FUNCTION__);
					break;
				case ENOSPC:
					MSG_DBG("%s  Maximum queue limit exceeded...\n", __FUNCTION__);
					break;
				default:
					break;
			}
		}
	}

	MSG_DBG("%s...handler_id:%d...\n", __FUNCTION__, handler_ID);

	return handler_ID;
}

/*
*create handler message queue for read data from eventd in handler process or write data to handler
*in eventd process
*return 0 if success
*return -1 if failed
*/
int msg_queue_create(key_t key)
{
	int msg_ID = -1;
	if(-1 == key) {
		MSG_DBG(" %s create message key failed......\n",__FUNCTION__);
		return -1;
	}

	msg_ID = msgget(key, IPC_EXCL | IPC_CREAT | 0777);
	if(msg_ID < 0) {
		switch(errno) {
			case EEXIST:
				msg_ID = msgget(key, 0777);
				if(msg_ID < 0) {
					MSG_DBG(" %s create message queue failed.....\n",__FUNCTION__);
					return -1;
				}
				break;
			case EIDRM:
				MSG_DBG("%s  Queue is marked for deletion.....\n", __FUNCTION__);
				break;
			case ENOENT:
				MSG_DBG("%s  Queue does not exist......\n", __FUNCTION__);
				break;
			case ENOMEM:
				MSG_DBG("%s  Not enough memory to create queue...\n", __FUNCTION__);
				break;
			case ENOSPC:
				MSG_DBG("%s  Maximum queue limit exceeded...\n", __FUNCTION__);
				break;
			default:
				break;
		}
	}

	MSG_DBG("%s...handler_id:%d...\n", __FUNCTION__, msg_ID);

	return msg_ID;
}

/*
*create register provider message queue between evented and provider process
*return 0 if success
*return -1 if failed
*/
int eventd_provider_msg_queue_create(key_t key)
{
	if((-1 == eventd_provider_ID) || (0 == eventd_provider_ID)) {
		eventd_provider_key = key;
		MSG_DBG("%s...eventd_provider_key:%d...\n", __FUNCTION__, eventd_provider_key);
		eventd_provider_ID = msgget(eventd_provider_key, IPC_EXCL | IPC_CREAT | 0777);

		if(eventd_provider_ID < 0) {
			switch(errno) {
				case EEXIST:
					eventd_provider_ID = msgget(eventd_provider_key, 0777);
					if(eventd_provider_ID < 0) {
						MSG_DBG("%s create message queue failed.....\n", __FUNCTION__);
						return -1;
					}
					break;
				case EIDRM:
					MSG_DBG("%s  Queue is marked for deletion.....\n", __FUNCTION__);
					break;
				case ENOENT:
					MSG_DBG("%s  Queue does not exist......\n", __FUNCTION__);
					break;
				case ENOMEM:
					MSG_DBG("%s  Not enough memory to create queue...\n", __FUNCTION__);
					break;
				case ENOSPC:
					MSG_DBG("%s  Maximum queue limit exceeded...\n", __FUNCTION__);
					break;
				default:
					break;
			}
		}
	}

	MSG_DBG("%s...eventd_provider_ID:%d...\n", __FUNCTION__, eventd_provider_ID);

	return eventd_provider_ID;
}

/*
*create message queue for change data between eventd and provider process
*return 0 if success
*return -1 if failed
*/
int provider_msg_queue_create(key_t key)
{
	if((-1 == provider_ID) || (0 == provider_ID)) {
		provider_key = key;
		if(provider_key < 0) {
			MSG_DBG("%s create message key failed......\n", __FUNCTION__);
			return -1;
		}

		provider_ID = msgget(provider_key, IPC_EXCL | IPC_CREAT | 0777);
		if(provider_ID < 0) {
			switch(errno) {
				case EEXIST:
					provider_ID = msgget(provider_key, 0777);
					if(provider_ID < 0) {
						MSG_DBG("%s create message queue failed.....\n", __FUNCTION__);
						return -1;
					}
					break;
				case EIDRM:
					MSG_DBG("%s  Queue is marked for deletion.....\n", __FUNCTION__);
					break;
				case ENOENT:
					MSG_DBG("%s  Queue does not exist......\n", __FUNCTION__);
					break;
				case ENOMEM:
					MSG_DBG("%s  Not enough memory to create queue...\n", __FUNCTION__);
					break;
				case ENOSPC:
					MSG_DBG("%s  Maximum queue limit exceeded...\n", __FUNCTION__);
					break;
				default:
					break;
			}
		}
	}

	MSG_DBG("%s...provider_ID:%d...\n", __FUNCTION__, provider_ID);

	return provider_ID;
}

/*
* read message from the message queue in eventd process about handler register information
* return 0 if success
* return -1 if failed
*/
int Read_Handler_Register_Event(struct msg_register_buffer* handler_register_msg)
{
	int status = 0;
	MSG_DBG("%s---eventd_handler_ID:%d--%d-\n",__FUNCTION__, eventd_handler_ID, handler_register_msg->msg_type);
	if(NULL == handler_register_msg) {
		MSG_DBG("%s register msg is null...\n", __FUNCTION__);
		return -1;
	}

    status = msgrcv(eventd_handler_ID, handler_register_msg, sizeof(struct event_register_data), handler_register_msg->msg_type, IPC_NOWAIT);

    if(status < 0) {
		switch(errno) {
			case E2BIG:
				MSG_DBG("%s Message length is greater than msgsz,no MSG_NOERROR...\n", __FUNCTION__);
				break;
			case EACCES:
				MSG_DBG("%s  No read permission...\n", __FUNCTION__);
				break;
			case EFAULT:
				MSG_DBG("%s  Address pointed to by msgp is invalid...\n", __FUNCTION__);
				break;
			case EIDRM:
				MSG_DBG("%s  Queue was removed during retrieval...\n", __FUNCTION__);
				break;
			case EINTR:
				MSG_DBG("%s  Interrupted by arriving signal...\n", __FUNCTION__);
				do {
					status = msgrcv(eventd_handler_ID, handler_register_msg, sizeof(struct event_register_data), handler_register_msg->msg_type, IPC_NOWAIT);
				}while(status < 0);
				break;
			case EINVAL:
				MSG_DBG("%s  msgqid invalid, or msgsz less than 0...\n", __FUNCTION__);
				break;
			case ENOMSG:
				MSG_DBG("%s  IPC_NOWAIT asserted, and no message exists in the queue to satisfy the request...\n", __FUNCTION__);
				break;
			default:
				break;
		}
		MSG_DBG("%s failed..status:%d.\n", __FUNCTION__, status);
        if(status < 0) {
        	return -1;
        }
    }

	MSG_DBG("%s.....status:%d...\n", __FUNCTION__, status);

    return 0;
}

/*
* read message from the message queue int eventd process about provider register information
* return 0 if success
* return -1 if failed
*/
int Read_Provider_Register_Event(struct msg_register_buffer* provider_register_msg)
{
	int status = 0;
	if(NULL == provider_register_msg) {
		MSG_DBG("%s register msg is null...\n", __FUNCTION__);
		return -1;
	}
	MSG_DBG("%s id:%d...\n", __FUNCTION__, eventd_provider_ID);
    status = msgrcv(eventd_provider_ID, provider_register_msg, sizeof(struct event_register_data), provider_register_msg->msg_type, IPC_NOWAIT);

    if(status < 0) {
		switch(errno) {
			case E2BIG:
				MSG_DBG("%s Message length is greater than msgsz,no MSG_NOERROR...\n", __FUNCTION__);
				break;
			case EACCES:
				MSG_DBG("%s  No read permission...\n", __FUNCTION__);
				break;
			case EFAULT:
				MSG_DBG("%s  Address pointed to by msgp is invalid...\n", __FUNCTION__);
				break;
			case EIDRM:
				MSG_DBG("%s  Queue was removed during retrieval...\n", __FUNCTION__);
				break;
			case EINTR:
				MSG_DBG("%s  Interrupted by arriving signal...\n", __FUNCTION__);
				do{
					status = msgrcv(eventd_provider_ID, provider_register_msg, sizeof(struct event_register_data), provider_register_msg->msg_type, IPC_NOWAIT);
				}while(status < 0);
				break;
			case EINVAL:
				MSG_DBG("%s  msgqid invalid, or msgsz less than 0...\n", __FUNCTION__);
				break;
			case ENOMSG:
				MSG_DBG("%s  IPC_NOWAIT asserted, and no message exists in the queue to satisfy the request...\n", __FUNCTION__);
				break;
			default:
				break;
		}
		MSG_DBG("%s failed...\n", __FUNCTION__);
        if(status < 0) {
        	return -1;
        }
    }

    return 0;
}

/*
* send data to message queue in handler process about handler register information
* return 0 if success
* return -1 if failed
*/
int Send_Handler_Register_Event(struct msg_register_buffer* handler_register_msg)
{
	int status = 0;
	MSG_DBG("%s---eventd_handler_ID:%d---\n",__FUNCTION__, eventd_handler_ID);
	if(NULL == handler_register_msg) {
		MSG_DBG("%s register_msg is nulll.......\n", __FUNCTION__);
		return -1;
	}
    status = msgsnd(eventd_handler_ID, handler_register_msg, sizeof(struct event_register_data), 0);

    if (status < 0) {
		switch(errno) {
			case EAGAIN:
				MSG_DBG("%s queue is full, and IPC_NOWAIT was asserted...\n", __FUNCTION__);
				break;
			case EACCES:
				MSG_DBG("%s  permission denied, no write permission...\n", __FUNCTION__);
				break;
			case EFAULT:
				MSG_DBG("%s  msgp address isn't accessable – invalid...\n", __FUNCTION__);
				break;
			case EIDRM:
				MSG_DBG("%s  The message queue has been removed...\n", __FUNCTION__);
				break;
			case EINTR:
				MSG_DBG("%s  Received a signal while waiting to write...\n", __FUNCTION__);
				do {
					status = msgsnd(eventd_handler_ID, handler_register_msg, sizeof(struct event_register_data), 0);
				}while(status < 0);
				break;
			case EINVAL:
				MSG_DBG("%s  Invalid message queue identifier, nonpositive message type, or invalid message size...\n", __FUNCTION__);
				break;
			case ENOMEM:
				MSG_DBG("%s  Not enough memory to copy message buffer...\n", __FUNCTION__);
				break;
			default:
				break;
		}
		MSG_DBG("%s failed...\n", __FUNCTION__);
        if(status < 0) {
        	return -1;
        }
    }

    return 0;
}

/*
* send the handler register information to handler from eventd
* return 0 if success
* return -1 if failed
*/
int Send_Handler_Register_Return(int msg_id, struct msg_register_buffer* handler_register_return)
{
	int status = 0;
	if(NULL == handler_register_return) {
		MSG_DBG("%s failed register_msg is nulll.......\n", __FUNCTION__);
		return -1;
	}
    status = msgsnd(msg_id, handler_register_return, sizeof(struct event_register_data), IPC_NOWAIT);

    if (status < 0) {
		switch(errno) {
			case EAGAIN:
				MSG_DBG("%s queue is full, and IPC_NOWAIT was asserted...\n", __FUNCTION__);
				break;
			case EACCES:
				MSG_DBG("%s  permission denied, no write permission...\n", __FUNCTION__);
				break;
			case EFAULT:
				MSG_DBG("%s  msgp address isn't accessable – invalid...\n", __FUNCTION__);
				break;
			case EIDRM:
				MSG_DBG("%s  The message queue has been removed...\n", __FUNCTION__);
				break;
			case EINTR:
				MSG_DBG("%s  Received a signal while waiting to write...\n", __FUNCTION__);
				do {
					status = msgsnd(msg_id, handler_register_return, sizeof(struct event_register_data), IPC_NOWAIT);		
				}while(status < 0);
				break;
			case EINVAL:
				MSG_DBG("%s  Invalid message queue identifier, nonpositive message type, or invalid message size...\n", __FUNCTION__);
				break;
			case ENOMEM:
				MSG_DBG("%s  Not enough memory to copy message buffer...\n", __FUNCTION__);
				break;
			default:
				break;
		}

		MSG_DBG("%s failed .......\n", __FUNCTION__);
        if(status < 0) {
        	return -1;
        }
    }

    return 0;
}

/*
* return the handler register information to handler from eventd
* return 0 if success
* return -1 if failed
*/
int Read_Handler_Register_Return(struct msg_register_buffer* handler_register_return)
{
	int status = 0;
	if(NULL == handler_register_return) {
		MSG_DBG("%s failed register_msg is nulll.......\n", __FUNCTION__);
		return -1;
	}
    status = msgrcv(handler_ID, handler_register_return, sizeof(struct event_register_data), handler_register_return->msg_type,0);

    if (status < 0) {
		switch(errno) {
			case EAGAIN:
				MSG_DBG("%s queue is full, and IPC_NOWAIT was asserted...\n", __FUNCTION__);
				break;
			case EACCES:
				MSG_DBG("%s  permission denied, no write permission...\n", __FUNCTION__);
				break;
			case EFAULT:
				MSG_DBG("%s  msgp address isn't accessable – invalid...\n", __FUNCTION__);
				break;
			case EIDRM:
				MSG_DBG("%s  The message queue has been removed...\n", __FUNCTION__);
				break;
			case EINTR:
				MSG_DBG("%s  Received a signal while waiting to write...\n", __FUNCTION__);
				do {
					status = msgrcv(handler_ID, handler_register_return, sizeof(struct event_register_data),handler_register_return->msg_type, 0);
				}while(status < 0);
				break;
			case EINVAL:
				MSG_DBG("%s  Invalid message queue identifier, nonpositive message type, or invalid message size...\n", __FUNCTION__);
				break;
			case ENOMEM:
				MSG_DBG("%s  Not enough memory to copy message buffer...\n", __FUNCTION__);
				break;
			default:
				break;
		}

		MSG_DBG("%s failed .......\n", __FUNCTION__);
        if(status < 0) {
        	return -1;
        }
    }

    return 0;
}

/*
* send provider data to message queue in provider process about provider register information
* return 0 if success
* return -1 if failed
*/
int Send_Provider_Register_Event(struct msg_register_buffer* provider_register_msg)
{
	int status = 0;
	if(NULL == provider_register_msg) {
		MSG_DBG("%s failed register_msg is nulll.......\n", __FUNCTION__);
		return -1;
	}
    status = msgsnd(eventd_provider_ID, provider_register_msg, sizeof(struct event_register_data), IPC_NOWAIT);

    if (status < 0) {
		switch(errno) {
			case EAGAIN:
				MSG_DBG("%s queue is full, and IPC_NOWAIT was asserted...\n", __FUNCTION__);
				break;
			case EACCES:
				MSG_DBG("%s  permission denied, no write permission...\n", __FUNCTION__);
				break;
			case EFAULT:
				MSG_DBG("%s  msgp address isn't accessable – invalid...\n", __FUNCTION__);
				break;
			case EIDRM:
				MSG_DBG("%s  The message queue has been removed...\n", __FUNCTION__);
				break;
			case EINTR:
				MSG_DBG("%s  Received a signal while waiting to write...\n", __FUNCTION__);
				do {
					status = msgsnd(eventd_provider_ID, provider_register_msg, sizeof(struct event_register_data), IPC_NOWAIT);
				}while(status < 0);
				break;
			case EINVAL:
				MSG_DBG("%s  Invalid message queue identifier, nonpositive message type, or invalid message size...\n", __FUNCTION__);
				break;
			case ENOMEM:
				MSG_DBG("%s  Not enough memory to copy message buffer...\n", __FUNCTION__);
				break;
			default:
				break;
		}

		MSG_DBG("%s failed .......\n", __FUNCTION__);
        if(status < 0) {
        	return -1;
        }
    }

    return 0;
}

/*
* send provider register information from eventd
* return 0 if success
* return -1 if failed
*/
int Send_Provider_Register_Return(int msg_id, struct msg_register_buffer* provider_register_return)
{
	int status = 0;
	if(NULL == provider_register_return) {
		MSG_DBG("%s failed register_msg is nulll.......\n", __FUNCTION__);
		return -1;
	}
    status = msgsnd(msg_id, provider_register_return, sizeof(struct event_register_data), IPC_NOWAIT);

    if (status < 0) {
		switch(errno) {
			case EAGAIN:
				MSG_DBG("%s queue is full, and IPC_NOWAIT was asserted...\n", __FUNCTION__);
				break;
			case EACCES:
				MSG_DBG("%s  permission denied, no write permission...\n", __FUNCTION__);
				break;
			case EFAULT:
				MSG_DBG("%s  msgp address isn't accessable – invalid...\n", __FUNCTION__);
				break;
			case EIDRM:
				MSG_DBG("%s  The message queue has been removed...\n", __FUNCTION__);
				break;
			case EINTR:
				MSG_DBG("%s  Received a signal while waiting to write...\n", __FUNCTION__);
				do{
					status = msgsnd(msg_id, provider_register_return, sizeof(struct event_register_data), IPC_NOWAIT);
				}while(status < 0);
				break;
			case EINVAL:
				MSG_DBG("%s  Invalid message queue identifier, nonpositive message type, or invalid message size...\n", __FUNCTION__);
				break;
			case ENOMEM:
				MSG_DBG("%s  Not enough memory to copy message buffer...\n", __FUNCTION__);
				break;
			default:
				break;
		}

		MSG_DBG("%s failed .......\n", __FUNCTION__);
       if(status < 0) {
        	return -1;
        }
    }

    return 0;
}

/*
* return provider register information from eventd, this msgrcv will block when no msg in the msg queue
* return 0 if success
* return -1 if failed
*/
int Read_Provider_Register_Return(int msg_ID, struct msg_register_buffer* provider_register_return)
{
	int status = 0;
	if(NULL == provider_register_return) {
		MSG_DBG("%s failed register_msg is nulll.......\n", __FUNCTION__);
		return -1;
	}
    status = msgrcv(msg_ID, provider_register_return, sizeof(struct event_register_data), provider_register_return->msg_type, 0);

    if (status < 0) {
		switch(errno) {
			case EAGAIN:
				MSG_DBG("%s queue is full, and IPC_NOWAIT was asserted...\n", __FUNCTION__);
				break;
			case EACCES:
				MSG_DBG("%s  permission denied, no write permission...\n", __FUNCTION__);
				break;
			case EFAULT:
				MSG_DBG("%s  msgp address isn't accessable – invalid...\n", __FUNCTION__);
				break;
			case EIDRM:
				MSG_DBG("%s  The message queue has been removed...\n", __FUNCTION__);
				break;
			case EINTR:
				MSG_DBG("%s  Received a signal while waiting to write...\n", __FUNCTION__);
				do{
					status = msgrcv(msg_ID, provider_register_return, sizeof(struct event_register_data), provider_register_return->msg_type, 0);
				}while(status < 0);
				break;
			case EINVAL:
				MSG_DBG("%s  Invalid message queue identifier, nonpositive message type, or invalid message size...\n", __FUNCTION__);
				break;
			case ENOMEM:
				MSG_DBG("%s  Not enough memory to copy message buffer...\n", __FUNCTION__);
				break;
			default:
				break;
		}

		MSG_DBG("%s failed .......\n", __FUNCTION__);
        if(status < 0) {
        	return -1;
        }
    }

    return 0;
}

/*
* read data from message queue in handler process, the data from eventd process
* return 0 if success
* return -1 if failed
*/
int Read_Data_Event_From_Eventd(int msg_id, struct msg_data_buffer* handler_data_msg)
{
	int status = 0;
	if(NULL == handler_data_msg) {
		MSG_DBG("%s data_msg is nulll.......\n",__FUNCTION__);
		return -1;
	}

    status = msgrcv(msg_id, handler_data_msg, sizeof(struct event_data), handler_data_msg->msg_type, 0);

    if(status < 0) {
		switch(errno) {
			case E2BIG:
				MSG_DBG("%s Message length is greater than msgsz,no MSG_NOERROR...\n", __FUNCTION__);
				break;
			case EACCES:
				MSG_DBG("%s  No read permission...\n", __FUNCTION__);
				break;
			case EFAULT:
				MSG_DBG("%s  Address pointed to by msgp is invalid...\n", __FUNCTION__);
				break;
			case EIDRM:
				MSG_DBG("%s  Queue was removed during retrieval...\n", __FUNCTION__);
				break;
			case EINTR:
				MSG_DBG("%s  Interrupted by arriving signal...\n", __FUNCTION__);
				do {
					status = msgrcv(msg_id, handler_data_msg, sizeof(struct event_data), handler_data_msg->msg_type, 0);
				}while(status < 0);
				break;
			case EINVAL:
				MSG_DBG("%s  msgqid invalid, or msgsz less than 0...\n", __FUNCTION__);
				break;
			case ENOMSG:
				MSG_DBG("%s  IPC_NOWAIT asserted, and no message exists in the queue to satisfy the request...\n", __FUNCTION__);
				break;
			default:
				break;
		}
		MSG_DBG("%s ...read data failed....\n",__FUNCTION__);
        if(status < 0) {
        	return -1;
        }
    }

    return 0;
}

/*
* read message from the eventd message queue in eventd process,the data is form provider process,
* return 0 if success
* return -1 if failed
*/
int Read_Data_Event_From_Provider(struct msg_data_buffer* provider_data_msg)
{
	int status = 0;
	if(NULL == provider_data_msg) {
		MSG_DBG("%s data msg is null....\n",__FUNCTION__);
		return -1;
	}

    status = msgrcv(provider_ID, provider_data_msg, sizeof(struct event_data), provider_data_msg->msg_type, IPC_NOWAIT);

    if(status < 0) {
		switch(errno) {
			case E2BIG:
				MSG_DBG("%s Message length is greater than msgsz,no MSG_NOERROR...\n", __FUNCTION__);
				break;
			case EACCES:
				MSG_DBG("%s  No read permission...\n", __FUNCTION__);
				break;
			case EFAULT:
				MSG_DBG("%s  Address pointed to by msgp is invalid...\n", __FUNCTION__);
				break;
			case EIDRM:
				MSG_DBG("%s  Queue was removed during retrieval...\n", __FUNCTION__);
				break;
			case EINTR:
				MSG_DBG("%s  Interrupted by arriving signal...\n", __FUNCTION__);
				do {
					status = msgrcv(provider_ID, provider_data_msg, sizeof(struct event_data), provider_data_msg->msg_type, IPC_NOWAIT);
				}while(status < 0);
				break;
			case EINVAL:
				MSG_DBG("%s  msgqid invalid, or msgsz less than 0...\n", __FUNCTION__);
				break;
			case ENOMSG:
				MSG_DBG("%s  IPC_NOWAIT asserted, and no message exists in the queue to satisfy the request...\n", __FUNCTION__);
				break;
			default:
				break;
		}

		MSG_DBG("%s failed.....\n", __FUNCTION__);
        if(status < 0) {
        	return -1;
        }
    }

    return 0;
}

/*
* write message data to the eventd message queue in provider process 
* the message data read by eventd process
* return 0 if success
* return -1 if failed
*/
int Send_Data_Event_To_Eventd(struct msg_data_buffer* provider_data_msg)
{
	int status = 0;
	if(NULL == provider_data_msg) {
		MSG_DBG("%s data msg is null...\n", __FUNCTION__);
		return -1;
	}
    status = msgsnd(provider_ID, provider_data_msg, sizeof(struct event_data), 0);

    if (status < 0) {
		switch(errno) {
			case EAGAIN:
				MSG_DBG("%s queue is full, and IPC_NOWAIT was asserted...\n", __FUNCTION__);
				break;
			case EACCES:
				MSG_DBG("%s  permission denied, no write permission...\n", __FUNCTION__);
				break;
			case EFAULT:
				MSG_DBG("%s  msgp address isn't accessable – invalid...\n", __FUNCTION__);
				break;
			case EIDRM:
				MSG_DBG("%s  The message queue has been removed...\n", __FUNCTION__);
				break;
			case EINTR:
				MSG_DBG("%s  Received a signal while waiting to write...\n", __FUNCTION__);
				do {
					status = msgsnd(provider_ID, provider_data_msg, sizeof(struct event_data), 0);
				} while(status < 0);
				break;
			case EINVAL:
				MSG_DBG("%s  Invalid message queue identifier, nonpositive message type, or invalid message size...\n", __FUNCTION__);
				break;
			case ENOMEM:
				MSG_DBG("%s  Not enough memory to copy message buffer...\n", __FUNCTION__);
				break;
			default:
				break;
		}

		MSG_DBG("%s failed....\n", __FUNCTION__);
		if(status < 0) {
        	return -1;
        }
    }

    return 0;
}

/*
* write message to the handler message queue by eventd process
* the data is read by handler process
* return 0 if success
* return -1 if failed
*/
int Send_Data_Event_To_Handler(int msg_id, struct msg_data_buffer* handler_data_msg)
{
	int status = 0;
	if(NULL == handler_data_msg) {
		MSG_DBG("%s data msg is null...\n", __FUNCTION__);
		return -1;
	}

    status = msgsnd(msg_id, handler_data_msg, sizeof(struct event_data), 0);

    if (status < 0) {
		switch(errno) {
			case EAGAIN:
				MSG_DBG("%s queue is full, and IPC_NOWAIT was asserted...\n", __FUNCTION__);
				break;
			case EACCES:
				MSG_DBG("%s  permission denied, no write permission...\n", __FUNCTION__);
				break;
			case EFAULT:
				MSG_DBG("%s  msgp address isn't accessable – invalid...\n", __FUNCTION__);
				break;
			case EIDRM:
				MSG_DBG("%s  The message queue has been removed...\n", __FUNCTION__);
				break;
			case EINTR:
				MSG_DBG("%s  Received a signal while waiting to write...\n", __FUNCTION__);
				do{
					status = msgsnd(msg_id, handler_data_msg, sizeof(struct event_data), IPC_NOWAIT);
				}while(status < 0);
				break;
			case EINVAL:
				MSG_DBG("%s  Invalid message queue identifier, nonpositive message type, or invalid message size...\n", __FUNCTION__);
				break;
			case ENOMEM:
				MSG_DBG("%s  Not enough memory to copy message buffer...\n", __FUNCTION__);
				break;
			default:
				break;
		}
		if(status < 0) {
			MSG_DBG("%s failed...\n", __FUNCTION__);
        	return -1;
        }
    }

    return 0;
}

int Read_Return_From_Handler(int msg_id, struct msg_data_buffer* handler_data_msg)
{
	int status = 0;
	if(NULL == handler_data_msg) {
		MSG_DBG("%s data msg is null...\n", __FUNCTION__);
		return -1;
	}

    status = msgrcv(msg_id, handler_data_msg, sizeof(struct event_data), handler_data_msg->msg_type, 0);

    if (status < 0) {
		switch(errno) {
			case E2BIG:
				MSG_DBG("%s Message length is greater than msgsz,no MSG_NOERROR...\n", __FUNCTION__);
				break;
			case EACCES:
				MSG_DBG("%s  No read permission...\n", __FUNCTION__);
				break;
			case EFAULT:
				MSG_DBG("%s  Address pointed to by msgp is invalid...\n", __FUNCTION__);
				break;
			case EIDRM:
				MSG_DBG("%s  Queue was removed during retrieval...\n", __FUNCTION__);
				break;
			case EINTR:
				MSG_DBG("%s  Received a signal while waiting to read..\n", __FUNCTION__);
				do {
					status = msgrcv(msg_id, handler_data_msg, sizeof(struct event_data), handler_data_msg->msg_type, 0);
					usleep(1000);
				} while(status < 0);
				break;
			case EINVAL:
				MSG_DBG("%s  msgqid invalid, or msgsz less than 0...\n", __FUNCTION__);
				break;
			case ENOMSG:
				MSG_DBG("%s  IPC_NOWAIT asserted, and no message exists in the queue to satisfy the request...\n", __FUNCTION__);
				break;
			default:
				break;
		}

		MSG_DBG("%s failed...\n", __FUNCTION__);
		if(status < 0) {
        	return -1;
        }
    }

    return 0;
}

int Send_Return_To_Provider(int msg_id, struct msg_data_buffer* handler_data_msg)
{
	int status = 0;
	if(NULL == handler_data_msg) {
		MSG_DBG("%s data msg is null...\n", __FUNCTION__);
		return -1;
	}

    status = msgsnd(msg_id, handler_data_msg, sizeof(struct event_data), IPC_NOWAIT);

    if (status < 0) {
		switch(errno) {
			case EAGAIN:
				MSG_DBG("%s queue is full, and IPC_NOWAIT was asserted...\n", __FUNCTION__);
				break;
			case EACCES:
				MSG_DBG("%s  permission denied, no write permission...\n", __FUNCTION__);
				break;
			case EFAULT:
				MSG_DBG("%s  msgp address isn't accessable – invalid...\n", __FUNCTION__);
				break;
			case EIDRM:
				MSG_DBG("%s  The message queue has been removed...\n", __FUNCTION__);
				break;
			case EINTR:
				MSG_DBG("%s  Received a signal while waiting to write...\n", __FUNCTION__);
				do {
					status = msgsnd(msg_id, handler_data_msg, sizeof(struct event_data), IPC_NOWAIT);
				} while(status < 0);
				break;
			case EINVAL:
				MSG_DBG("%s  Invalid message queue identifier, nonpositive message type, or invalid message size...\n", __FUNCTION__);
				break;
			case ENOMEM:
				MSG_DBG("%s  Not enough memory to copy message buffer...\n", __FUNCTION__);
				break;
			default:
				break;
		}

		MSG_DBG("%s failed...\n", __FUNCTION__);
		if(status < 0) {
        	return -1;
        }
    }

    return 0;

}

int Read_Return_Data_From_eventd(int msg_id, struct msg_data_buffer* handler_data_msg)
{
	int status = 0;
	if(NULL == handler_data_msg) {
		MSG_DBG("%s data msg is null...\n", __FUNCTION__);
		return -1;
	}

    status = msgrcv(msg_id, handler_data_msg, sizeof(struct event_data),handler_data_msg->msg_type,  0);

    if (status < 0) {
		switch(errno) {
			case E2BIG:
				MSG_DBG("%s Message length is greater than msgsz,no MSG_NOERROR...\n", __FUNCTION__);
				break;
			case EACCES:
				MSG_DBG("%s  No read permission...\n", __FUNCTION__);
				break;
			case EFAULT:
				MSG_DBG("%s  Address pointed to by msgp is invalid...\n", __FUNCTION__);
				break;
			case EIDRM:
				MSG_DBG("%s  Queue was removed during retrieval...\n", __FUNCTION__);
				break;
			case EINTR:
				MSG_DBG("%s  Received a signal while waiting to write...\n", __FUNCTION__);
				do {
					status = msgrcv(msg_id, handler_data_msg, sizeof(struct event_data), handler_data_msg->msg_type, 0);
				} while(status < 0);
				break;
			case EINVAL:
				MSG_DBG("%s  msgqid invalid, or msgsz less than 0...\n", __FUNCTION__);
				break;
			case ENOMSG:
				MSG_DBG("%s  IPC_NOWAIT asserted, and no message exists in the queue to satisfy the request...\n", __FUNCTION__);
				break;
			default:
				break;
		}

		MSG_DBG("%s failed...\n", __FUNCTION__);
		if(status < 0) {
        	return -1;
        }
    }

    return 0;

}

/*  destroy the handler message queue */
int eventd_handler_msg_queue_destroy()
{
	int status;
    if (( status= msgctl(eventd_handler_ID, IPC_RMID, NULL) ) < 0) {
        return 1;
    }
    return 0;
}

/*  destroy the eventd message queue */
int eventd_provider_msg_queue_destroy()
{
	int status;
    if (( status= msgctl(eventd_provider_ID, IPC_RMID, NULL) ) < 0) {
        return 1;
    }
    return 0;
}

/* destroy the handler data message queue */
int handler_data_msg_queue_destroy()
{
	int status;
    if (( status= msgctl(handler_ID, IPC_RMID, NULL) ) < 0) {
        return 1;
    }
    return 0;
}

/* destroy the provider data message queue */
int provider_data_msg_queue_destroy()
{
	int status;
    if (( status= msgctl(provider_ID, IPC_RMID, NULL) ) < 0) {
        return 1;
    }
    return 0;
}

/* destroy the specified message queue indicated by msg_ID */
int msg_queue_destroy(int msg_ID)
{
	int status;
    if (( status= msgctl(msg_ID, IPC_RMID, NULL) ) < 0) {
        return 1;
    }
    return 0;
}
