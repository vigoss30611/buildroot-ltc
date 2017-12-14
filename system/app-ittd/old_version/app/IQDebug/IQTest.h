
#ifndef __IQ_TEST_H__
#define __IQ_TEST_H__

#define IQ_TEST_DATA 1 /* 命令 */
#define MAX_DATA_BUFSZ 1024 * 1024 /* 接收数据缓冲大小 */

typedef struct {
    int cmd; /* 命令 */
    int len; /* 负载数据长度，不包括iq_test_msg_t头长度 */
    int xorcheck; /* 头校验 */
} iq_test_msg_t;

#define IQ_TEST_MSG_XOR(msg) (msg.cmd ^ msg.len)
#define SET_IQ_TEST_MSG(msg, msgcmd, datalen) do { msg.cmd=msgcmd;msg.len=datalen;msg.xorcheck=IQ_TEST_MSG_XOR(msg); } while (0)

/* 数据回调函数，puser对应不同的用户，pbuf表示接收到的数据存放缓冲区，len表示数据长度 */
/* 启动侦听后允许多个用户连接，puser表示该数据对应的用户连接 */
typedef void (*data_pcallback)(void *puser, void *pbuf, int len);

/* 启动服务，在指定网络端口侦听 */
int iq_test_start(unsigned short ipport);

/* 停止服务，释放资源 */
int iq_test_stop(void);

/* 设置数据回调函数 */
int iq_test_set_callback(data_pcallback pcallback);

/* 数据回传，puser为data_pcallback对应的puser参数，如果puser为空表示发送给所有已连接用户 */
int iq_test_data_send(void *puser, void *pbuf, int len);

#endif // #ifndef __IQ_TEST_H__
