
#ifndef __MAIN_H__
#define __MAIN_H__

#define IQ_TEST_DATA 1 /* 命令 */
#define MAX_DATA_BUFSZ 1024 * 1024 /* 接收數據緩衝大小 */

typedef struct
{
    int cmd; /* 命令 */
    int len; /* 負載數據長度，不包括iq_test_msg_t頭長度 */
    int xorcheck; /* 頭校驗 */
} iq_test_msg_t;

#define IQ_TEST_MSG_XOR(msg) (msg.cmd ^ msg.len)
#define SET_IQ_TEST_MSG(msg, msgcmd, datalen) do { msg.cmd=msgcmd;msg.len=datalen;msg.xorcheck=IQ_TEST_MSG_XOR(msg); } while (0)

/* 數據回調函數，puser對應不同的用戶，pbuf表示接收到的數據存放緩衝區，len表示數據長度 */
/* 啟動偵聽後允許多個用戶連接，puser表示該數據對應的用戶連接 */
typedef void (*data_pcallback)(void* puser, void* pbuf, int len);

/* 啟動服務，在指定網絡端口偵聽 */
int iq_test_start(unsigned short ipport);

/* 停止服務，釋放資源 */
int iq_test_stop(void);

/* 設置數據回調函數 */
int iq_test_set_callback(data_pcallback pcallback);

/* 數據回傳，puser為data_pcallback對應的puser參數，如果puser為空表示發送給所有已連接用戶 */
int iq_test_data_send(void* puser, void* pbuf, int len);

#endif // #ifndef __MAIN_H__
