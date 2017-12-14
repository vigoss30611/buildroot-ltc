
#ifndef __IROM_IUW_H__
#define __IROM_IUW_H__

#define IUW_CMD_VSWR		0x10
#define IUW_CMD_VSRD		0x11
#define IUW_CMD_NBWR        0x12
#define IUW_CMD_NBRD        0x13
#define IUW_CMD_NRWR        0x14
#define IUW_CMD_NRRD        0x15
#define IUW_CMD_NFWR        0x16
#define IUW_CMD_NFRD        0x17

#define IUW_CMD_MMWR		0x20
#define IUW_CMD_MMRD		0x21

#define IUW_CMD_CONFIG		0x30
#define IUW_CMD_JUMP		0x31
#define IUW_CMD_IDENTIFY	0x33

#define IUW_CMD_ENC			0x42
#define IUW_CMD_DEC			0x44
#define IUW_CMD_HASH		0x46

#define IUW_CMD_MISC		0x50
#define IUW_CMD_SYSM		0x52
#define IUW_CMD_CACH		0x54
#define IUW_CMD_USER		0x56

#define IUW_CMD_READY		0x02
#define IUW_CMD_FINISH		0xe0
#define IUW_CMD_ERROR		0xf0

#define IUW_SUB_STEP        0x40
#define IUW_SUB_MAC         0x41

#define IUW_CMD_MAGIC		0xaa
#define IUW_CMD_LEN			0x20
#define IUW_AES_KEYLEN		0x20

#define IUW_RETRY			100
#define opt_is_read(x)		(x & 0x1)
#define opt_is_write(x)		(!(x & 0x1))

#define aes_option(o)		((o == IUW_CMD_ENC)? 0: 1)

struct trans_dev {
	int	(*connect)(void);
	int     (*disconnect)(void);
	int	(*read)(uint8_t * buf, int len);
	int	(*write)(uint8_t * buf, int len);
	char name[12];
};

enum tdev_id {
	TDEV_UDC = 0,
	TDEV_ETH,
};


extern int init_iuw(void);
extern int iuw_set_dev(enum tdev_id id);
extern int iuw_connect(void);
extern int iuw_close(void);
extern int iuw_server(void);
extern int init_iuw(void);



extern inline int iuw_cmd_cmd(void *buf);
extern int iuw_pack_cmd(void *buf, int cmd, uint32_t para0, uint32_t para1);
extern int iuw_process_rw(void * buf);
extern int iuw_shake_hand(int subcmd, int para);
extern inline int iuw_cmd_len(void *buf);
extern inline int iuw_cmd_max(void *buf);
extern int iuw_extra(void *pack, int (*tget)(uint8_t *, int));

#endif /* __IROM_IUW_H__ */

