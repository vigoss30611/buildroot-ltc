#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "IQTest.h"

typedef struct _iq_test_user_t {
    int fd; /* 命令连接 */
    int inactive; /* 连接是否活动的 */
    struct _iq_test_user_t *pnext;
} iq_test_user_t;

static char test_thread_exit; /* 线程退出控制 */
static char test_data_buf[MAX_DATA_BUFSZ]; /* 接收数据缓冲 */

static int listen_fd; /* 侦听连接 */
static pthread_t test_tid; /* 线程id */
static pthread_mutex_t test_mutex; /* 临界保护 */
static iq_test_user_t *list_phead; /* 连接用户队列 */

static data_pcallback test_data_pcallback; /* 接收数据回调函数 */

static int iq_test_msg_send(iq_test_user_t *puser, iq_test_msg_t *pmsg, void *pbuf, int len)
{
    if (puser && pmsg && pbuf && len > 0)
    {
        if (send(puser->fd, pmsg, sizeof(iq_test_msg_t), 0) == sizeof(iq_test_msg_t) &&
            send(puser->fd, pbuf, len, 0) == len)
        {
            return 0;
        }
        printf("data send fail\n");
    }
    return -1;
}

int iq_test_data_send(void *puser, void *pbuf, int len)
{
    int r = -1;
    if (pbuf && len > 0)
    {
        iq_test_user_t *ptmp;
        iq_test_msg_t msg;
        SET_IQ_TEST_MSG(msg, IQ_TEST_DATA, len);
        pthread_mutex_lock(&test_mutex);
        ptmp = list_phead;
        while (ptmp) {
            if (ptmp == puser || puser == NULL)
            {
                r = iq_test_msg_send(ptmp, &msg, pbuf, len);
                if (puser)
                {
                    break;
                }
            }
            ptmp = ptmp->pnext;
        }
        pthread_mutex_unlock(&test_mutex);
    }
    return r;
}

static void net_socket_accpet(int listenfd)
{
    int fd;
    iq_test_user_t *puser;
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if ((fd = accept(listenfd, (struct sockaddr *)&sin, &len)) < 0)
    {
        printf("accpet fail\n");
        return;
    }
    puser = (iq_test_user_t *)calloc(1, sizeof(iq_test_user_t));
    if (puser)
    {
        puser->fd = fd;
        pthread_mutex_lock(&test_mutex);
        puser->pnext = list_phead;
        list_phead   = puser;
        pthread_mutex_unlock(&test_mutex);
        return;
    }
    close(fd);
}

static void iq_test_msg(iq_test_user_t *puser)
{
    iq_test_msg_t msg;
    ssize_t r = recv(puser->fd, &msg, sizeof(iq_test_msg_t), 0);
    if (r <= 0)
    {
        printf("socket closed\n");
        puser->inactive = 1;
        return;
    }
    if (r < sizeof(iq_test_msg_t))
    {
        printf("recv fail\n");
        return;
    }
    if (msg.xorcheck != IQ_TEST_MSG_XOR(msg))
    {
        printf("data check error\n");
        return;
    }
    if (msg.len > 0)
    {
        if (msg.len > MAX_DATA_BUFSZ)
        {
            printf("data buf overflow\n");
            return;
        }
        r = recv(puser->fd, test_data_buf, msg.len, 0);
        if (r < msg.len)
        {
            printf("recv fail\n");
            return;
        }
    }
    pthread_mutex_lock(&test_mutex);
    if (test_data_pcallback)
    {
        test_data_pcallback(puser, test_data_buf, msg.len);
    }
    pthread_mutex_unlock(&test_mutex);
}

static void* iq_test_thread(void *args)
{
	int maxfd;
	fd_set fdset;
	struct timeval tv;
    iq_test_user_t *puser;
    iq_test_user_t *ptmp;

    while (test_thread_exit == 0) {

        maxfd = listen_fd;

        FD_ZERO(&fdset);
        FD_SET(listen_fd, &fdset);

        tv.tv_sec  = 0;
		tv.tv_usec = 10;
        
        ptmp  = NULL;
        puser = list_phead;
        while (puser) {
            if (puser->inactive)
            {
                /* 连接已断开，关闭并释放 */
                pthread_mutex_lock(&test_mutex);
                if (ptmp)
                {
                    ptmp->pnext = puser->pnext;
                }
                else
                {
                    list_phead = puser->pnext;
                }
                pthread_mutex_unlock(&test_mutex);
                close(puser->fd);
                free(puser);
                puser = ptmp ? ptmp->pnext : list_phead;
            }
            else
            {
                if (maxfd < puser->fd)
                {
                    maxfd = puser->fd;
                }
                FD_SET(puser->fd, &fdset);
                ptmp  = puser;
                puser = puser->pnext;
            }
        }

        if (select(maxfd + 1, &fdset, NULL, NULL, &tv) > 0)
		{
            puser = list_phead;
            while (puser) {
                if (FD_ISSET(puser->fd, &fdset))
                {
                    iq_test_msg(puser);
                }
                puser = puser->pnext;
            }
            if (FD_ISSET(listen_fd, &fdset))
			{
                net_socket_accpet(listen_fd);
			}
        }
    }
    /* 释放 */
    pthread_mutex_lock(&test_mutex);
    puser = list_phead;
    while (puser) {
        ptmp  = puser;
        puser = puser->pnext;
        close(ptmp->fd);
        free(ptmp);
    }
    pthread_mutex_unlock(&test_mutex);
    return NULL;
}

static int net_socket_listen(unsigned short port)
{
#define QLEN	16
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd != -1)
	{
		struct sockaddr_in sin;
		memset(&sin, 0, sizeof(sin));
		sin.sin_family      = AF_INET;
		sin.sin_port        = htons(port);
		sin.sin_addr.s_addr = INADDR_ANY;
		if (bind(fd, (struct sockaddr*)&sin, sizeof(sin)) != -1 &&
			listen(fd, QLEN) != -1)
		{
			return fd;
		}
		close(fd);
	}
	return -1;
}

int iq_test_start(unsigned short ipport)
{
    pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&test_mutex, &attr);
    if ((listen_fd = net_socket_listen(ipport)) < 0)
    {
        printf("listen fail\n");
        return -1;
    }
    test_thread_exit    = 0;
    test_data_pcallback = NULL;
    if (pthread_create(&test_tid, NULL, iq_test_thread, NULL) != 0)
    {
        printf("thread create fail\n");
        close(listen_fd);
        return -1;
    }
    return 0;
}

int iq_test_stop(void)
{
    test_thread_exit = 1;
    pthread_join(test_tid, NULL);
    pthread_mutex_destroy(&test_mutex);
    close(listen_fd);
    return 0;
}

int iq_test_set_callback(data_pcallback pcallback)
{
    pthread_mutex_lock(&test_mutex);
    test_data_pcallback = pcallback;
    pthread_mutex_unlock(&test_mutex);
    return 0;
}

/**********************************************************/
/* 以下为接口测试程序，运行时需要指定监听端口 例如./IQTest 11111 */
/**********************************************************/
#include <signal.h>

static char test_exit_flags;

static void iq_test_data_callback(void *puser, void *pbuf, int len)
{
    printf("data callback user %p data %p len %d\n", puser, pbuf, len);
    /* 原样回写 */
    iq_test_data_send(puser, pbuf, len);
}

static void test_exit(int signo)
{
    test_exit_flags = 1;
}

int main(int argc, unsigned char** argv)
{
    signal(SIGINT, test_exit); /* CTRL+C退出 */
    if (argc > 1)
    {
        int port = atoi(argv[1]);
        if (iq_test_start(port) == 0)
        {
            iq_test_set_callback(iq_test_data_callback);
            while (test_exit_flags == 0) {
                sleep(1);
            }
            iq_test_stop();
        }
    }
    return 0;
}
