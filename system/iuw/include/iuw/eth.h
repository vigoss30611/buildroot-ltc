#ifndef __IUW_ETH_H__
#define __IUW_ETH_H__
#include <netinet/ip.h>
#include <stdbool.h>

//#define DEBUG
#define eth_error(error, ...) do {printf(" " error, ##__VA_ARGS__);} while (0)

#ifdef DEBUG
#define eth_debug(debug, ...)
//#define eth_debug(debug, ...) do {printf(" " debug, ##__VA_ARGS__);} while (0)
#define eth_packet(packet,length)\
do\
{\
    int i;\
    printf("%s : length: %d\n",__func__, length);\
    for (i=0; i<length; i++)\
    {\
        if (i % 8 == 0)\
        {\
            printf("\n%s: %02x: ", __func__, i);\
        }\
        printf("%02x ", ((unsigned char *) packet)[i]);\
    }\
    printf("\n");\
} while(0)
#else
#define eth_debug(debug, ...)
#define eth_packet(packet,length)
#endif

/* for 4 bytes aligned */

struct eth_align {
    uint8_t so_null[2];
};
#define ETH_ALIGN (sizeof(struct eth_align))

/****************** socket api for listen to socket *****************/
#define MAX_PACKET_SIZE     (1536)
#define MAXINTERFACES 16


struct queue_node {
    struct queue_node *prev;
    struct queue_node *next;
    void *data;
    int pkt_len;
    int sum;
};

struct data_context {
    void *data;
    int pkt_len;
};

enum {
    COND_MODE_COUNT = 0,
    COND_MODE_VOLATILE,
    COND_MODE_RECORD,
    COND_MODE_DEFAULT = COND_MODE_COUNT,
};

enum {
    COND_RUNNING = 0,
    COND_CANCEL_START,
    COND_CANCEL_FINISH,
};

struct cond_context {
    pthread_cond_t cond;
    int cancel;
    pthread_mutex_t mutex;
    bool eth_wocao;
    int mark;
    int mode;
};
typedef struct cond_context * cond_t;

struct queue_context {
    pthread_mutex_t lock;
    cond_t cond;
    pthread_t listen;
    int cap;
    struct queue_node *start;
    struct queue_node *end;
};
typedef struct queue_context * queue_t;

void cond_delete(cond_t cond);

/*********************source for thread******************************/

struct irnet_context {
    int     status;         //status for transfer status
    /*   irnet */
    uint32_t irnet_pid;     /*    Packet number. */
    uint32_t irnet_status;      /*    Current status of irnet protocol. */
    uint32_t irnet_asize;       /*    Access data size. */
    uint8_t *irnet_buf;     /*    Data buf size. */
    uint32_t irnet_acs;     /*    Read/Write. */
    uint32_t irnet_last_op;     /*    Record last operation. */
    uint32_t irnet_last_ds;     /*    Record last sent data packet size, */
};

enum {
    ETH_C = 0,
    ETH_B,
    ETH_A,
};

struct eth_node {
    uint32_t eth_ip;
    uint8_t  eth_mac[6];
    uint32_t eth_cd;
    uint16_t eth_stat;
    uint16_t eth_disconnect;
    int so_flags;
    struct timespec ts;
    socklen_t eth_tx_len;
    struct sockaddr_in eth_client_addr;
    struct irnet_context irnet;
    queue_t  queue;
    uint8_t  eth_txbuf[MAX_PACKET_SIZE];
    uint8_t  eth_rxbuf[MAX_PACKET_SIZE];
};

enum {
    ETH_IDLE,
    ETH_READY,
    ETH_BUSY,
};

#define MAX_ETH_COUNT    20

struct eth_dev {
    int sock;
    int sock_send;
    pthread_t pid;
    pthread_t disconnect_pid;
    struct sv_callback cb;
    socklen_t eth_len;
    struct sockaddr_in eth_addr;
    struct eth_node nodes[MAX_ETH_COUNT];
};

/******************* manage ip ************************/

#define CFG_RX_PKTS_COUNT       4
#define PACKET_ADDR_ALIGN   (32)
#define SO_PORT                 12345

#define PROT_SOIP 0x2b2b

struct soip_head {
    uint32_t so_procotol;  /*   procotol */
    uint8_t  so_localmac[6];   /*   local mac */
    uint8_t  so_servermac[6];
    uint32_t so_local_ip;   /*   local ip  */
    uint32_t so_subnet_mask;
    uint32_t so_gateway_ip;
    uint32_t so_server_ip;
};

#define SOIP_HDR_SIZE  (sizeof (struct soip_head))
/*************************** sonet struct ****************/

enum {
    IRNET_ACS_READ = 1,
    IRNET_ACS_WRITE,
};

struct sonet_context {
    uint32_t local_ip;
    uint32_t ip_stat;
    uint32_t subnet_mask;
    uint32_t gateway_ip;
    uint32_t client_ip;
    uint32_t ip_restore;
    volatile uint8_t *rx_pkt;
    volatile uint8_t *tx_pkt;

    uint8_t local_mac[6];
    uint8_t client_mac[6];
};

/* TODO: add headings here */
extern int eth_register_callback(struct sv_callback *cb);
extern int eth_start(void);
extern int eth_stop(void);
extern int eth_read(int cd, uint8_t *buf, int len);
extern int eth_write(int cd, uint8_t *buf, int len);
extern int eth_put_cd(int cd);

/************************* irnet layer **********************/

#define IRNET_PKT_SLICE_SIZE    (1024)

/*  IRNET packet type. */
enum {
    IRNET_DATA = 0,
    IRNET_START,
    IRNET_RESEND,
    IRNET_ACK,
    IRNET_RESERVED_0100,
};

enum {       
    SONET_CONTINUE = 1,
    SONET_SUCCESS,
    SONET_FAIL,
};


struct irnet_head {
#define SO_IRNET            0x2500
#define IROPT_TYPE(opt)     (opt >> 28)
#define IROPT_ACCESS(opt)   ((opt >> 27) & 1)
#define IROPT_S_TYPE(type)  (type << 28)
#define IROPT_S_ACCESS(acs) ((acs & 1) << 27)
    uint32_t procotol;   /*  IRNET procotol */
    uint32_t opt;       /*  IRNET packet option and type. */
    uint32_t dlen;      /*  IRNET packet data len. */
    uint32_t pid;       /*  IRNET packet id, all data packet need an id. */
    uint32_t aligned;   /*  IRNET for gdma 8 bytes aligned */
};

#define IRNET_HEAD_SIZE  (sizeof(struct irnet_head))


#endif /* __IUW_ETH_H__ */
