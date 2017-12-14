#ifndef __TRANSFER_H__
#define __TRANSFER_H__

#include <listparser.h>

extern inline int svl_cmd_cmd(void *buf);
extern inline int svl_sub_cmd(void *buf);
extern inline uint32_t svl_cmd_para(void *buf, int num);
extern int svl_is_cmd(void *buf);
extern int svl_pack_cmd(void *buf, int cmd,                       
        uint32_t para0, uint32_t para1, uint32_t para2);
extern int svl_send_cmd(struct svline_node *svl, int cmd);
extern int svl_send_cmd_desc(struct svline_node *svl, struct list_desc *desc);
extern void svl_get_sub_content(void *buf, uint8_t *pack);
extern int svl_shake_hand(struct svline_node *svl, int sub_cmd, void *buf);
extern int svline_finish(struct svline_node *svl);
extern void init_folder_path(char *temporary);
extern int compare_file(char *path, char *path1);

/* test hash */
extern uint8_t *tt_digest_caculate_hash(char *path, int type, int offset);
extern uint8_t *tt_digest_caculate_by_iuw(struct svline_node *svl, 
        char *path, int type, int offset);
/* test dec&enc */
extern int aes_start(void);
extern int encrypt_the_file(char *path, char *enc_path, int offset);
extern int aes_encrypt_decrypt_by_iuw(char *path, char *path1, 
        struct svline_node *svl, int nokey, int direction, int offset);
/* test server */
extern int svline_dev_transfer(struct svline_node *svl, struct list_desc *desc);
/* test mmwr&mmrd */
extern int push_transfer(struct svline_node *svl, char *path, int offset);

#endif
