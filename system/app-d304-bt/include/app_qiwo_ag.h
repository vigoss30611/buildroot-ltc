#ifndef APP_QIWO_AG_H
#define APP_QIWO_AG_H

#include "bsa_api.h"
#include "app_disc.h"

typedef struct
{
    unsigned short num;
    tBSA_DISC_REMOTE_DEV device[APP_DISC_NB_DEVICES];
} tAPP_QIWO_AG_SCAN_RSP;

typedef struct 
{
    BOOLEAN is_need_disconnect;
    BD_ADDR curr_conn_addr;    
    BD_ADDR conn_addr;
}tAPP_QIWO_AG_CONN;

#endif
