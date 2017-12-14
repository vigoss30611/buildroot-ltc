/***************************************************************************** 
** iuw_base.c
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** ** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: IUW protocal base.
**
** Author:
**     Warits   <warits.wang@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.0  XXX 08/01/2011 XXX	Initialized by warits
*****************************************************************************/

#include <common.h>
#include <vstorage.h>
#include <malloc.h>
#include <iuw.h>
#include <ius.h>
#include <hash.h>
#include <net.h>
#include <udc.h>
#include <led.h>
#include <nand.h>
#include <cdbg.h>
#include <crypto.h>
#include <irerrno.h>
#include <asm/io.h>
#include <linux/types.h>

//#define IR_MAX_BUFFER 0x4000

static int tdev_current;
static uint8_t *pack = NULL;

const static struct trans_dev tdev[] = {
	{
		.connect = udc_connect,
		.read    = udc_read,
		.write   = udc_write,
		.name    = "usb",
	},
#if defined(CONFIG_ENABLE_ETH)
	{
		.connect = eth_connect,
		.read    = eth_read,
		.write   = eth_write,
		.name    = "ethernet",
	}
#endif
};

#define ptdev (&tdev[tdev_current])

/* command manage */

/* iuw command package:
 *
 * |<-1->|<---3--->|<-----4----->|<-----4----->|<----4---->|
 * |  m  |   cmd   |    para0    |    para1    |    sum    |
 */

inline int iuw_cmd_cmd(void *buf) {
	return (*(uint32_t *)buf) & 0xffffff;
}

static inline uint32_t iuw_cmd_para(void *buf, int num) {
	return *(uint32_t *)(buf + 4 + (num << 2));
}

static inline int iuw_cmd_id(void *buf) {
	return *(uint16_t *)(buf + 10);
}

static inline loff_t iuw_cmd_offs(void *buf) {
	loff_t tmp = *(uint16_t *)(buf + 8);
	tmp <<= 32;
	tmp |= *(uint32_t *)(buf + 4);
	return tmp;
}

static inline loff_t iuw_cmd_addr(void *buf) {
	return *(uint32_t *)(buf + 4);
}

static inline int iuw_cmd_crypt_isk(void *buf) {
	return !*(uint32_t *)(buf + 4);
}

static inline loff_t iuw_cmd_hash_type(void *buf) {
	return *(uint32_t *)(buf + 4);
}

static inline loff_t iuw_cmd_hash_len(void *buf) {
	return *(uint32_t *)(buf + 8);
}

static inline loff_t iuw_cmd_entry(void *buf) {
	return *(uint32_t *)(buf + 4);
}

inline int iuw_cmd_max(void *buf)
{
	return *(int *)(buf + 0x8) & ~0xffff;
}

inline int iuw_cmd_len(void *buf) {
	return *(int *)(buf + 0xc);
}

static int iuw_is_cmd(void *buf) {
	uint32_t i, tmp;

	if(*(uint8_t *)(buf + 3) != IUW_CMD_MAGIC) {
		printf("iuw cmd magic do not match6.\n");
		return 0;
	}

	for(i = tmp = 0; i < IUW_CMD_LEN - 4; i++)
	  tmp += *(uint8_t *)(buf + i);

	if(tmp != *(uint32_t *)(buf + IUW_CMD_LEN - 4)) {
		printf("iuw cmd checksum do not match.\n");
		return 0;
	}

	/* package in buffer is an iuw command */
	return 1;
}

int iuw_pack_cmd(void *buf,
			int cmd, uint32_t para0, uint32_t para1)
{
	uint32_t i, tmp;

	memset(buf, 0, IUW_CMD_LEN);
	*(uint32_t *)(buf + 0) = cmd | (IUW_CMD_MAGIC << 24);
	*(uint32_t *)(buf + 4) = para0;
	*(uint32_t *)(buf + 8) = para1;

	for(i = tmp = 0; i < IUW_CMD_LEN - 4; i++)
	  tmp += *(uint8_t *)(buf + i);

	*(uint32_t *)(buf + IUW_CMD_LEN - 4) = tmp;
	return 0;
}

static int iuw_send_cmd(int cmd, uint32_t para0, uint32_t para1)
{
	int ret;

	iuw_pack_cmd(pack, cmd, para0, para1);
	ret = ptdev->write(pack, IUW_CMD_LEN);

	if(ret < 0)
	  return -1;
	return 0;
}

int iuw_shake_hand(int subcmd, int para)
{
	int ret;
	printf("%s\n", __func__);
	if(iuw_send_cmd(IUW_CMD_READY, subcmd, para)<0)
	{
		return -1;
	}

	ret = ptdev->read(pack, IUW_CMD_LEN);
	if(ret < IUW_CMD_LEN)
	  return ret;

#if 0
	printf("rececied: \n");
	print_buffer(pack, pack, 1, IUW_CMD_LEN, 16);
#endif
	if(iuw_is_cmd(pack) && iuw_cmd_cmd(pack) == IUW_CMD_READY)
	  return 0;
	return -1;
}




/* API */
int iuw_set_dev(enum tdev_id id)
{
	if(id == TDEV_ETH || id == TDEV_UDC)
	  tdev_current = id;
	else {
		printf("unvailiable tdev_id.\n");
		return -1;
	}

	printf("iuw transport device set to %s\n", tdev[id].name);
	return 0;
}

int iuw_connect(void)
{
	int ret;
	if((ret = ptdev->connect()) < 0)
	  return ret;

	/* this function may be invoked multiple times without
	 * closing, so if the buffer remains, do not realloc it.
	 */
	if(!pack)
	  pack = malloc(IR_MAX_BUFFER);

	if(!pack) {
		printf("alloc iuw package buffer failed\n");
		goto __failed;
	}

	ret = iuw_shake_hand(0, 0);

	if(!ret)
	  return 0;

__failed:
	free(pack);
	pack = NULL;
	return ret;
}

int iuw_close(void)
{
	free(pack);
	pack = NULL;
	return 0;
}

static int iuw_local_rw(uint8_t *buf, loff_t offs, int len, int opt)
{
	int ret = 0;
	switch(opt)
	{
		case IUW_CMD_VSRD:
		case IUW_CMD_NBRD:
		case IUW_CMD_NRRD:
			ret = vs_read(buf, offs, len, 0);
			break;
		case IUW_CMD_VSWR:
		case IUW_CMD_NBWR:
		case IUW_CMD_NRWR:
		case IUW_CMD_NFWR:
			ret = vs_write(buf, offs, len, 0);
			break;
		case IUW_CMD_MMRD:
			memcpy(buf, (uint8_t *)(uint32_t)offs, len);
			break;
		case IUW_CMD_MMWR:
			memcpy((uint8_t *)(uint32_t)offs, buf, len);
			break;
		case IUW_CMD_HASH:
			hash_data(buf, len);
			break;
		case IUW_CMD_ENC:
		case IUW_CMD_DEC:
			ss_aes_data(buf, buf + (IR_MAX_BUFFER >> 1), len);
			break;
		default:
			printf("no rw option for cmd: %d\n", opt);
			break;
	}

	return ret;
}

static int iuw_check_identity(void)
{
	struct ius_hdr *ius = (struct ius_hdr *)pack;
	int ret, len = sizeof(struct ius_hdr), pass = 1;

	/* 1: shake hand */
	ret = iuw_shake_hand(IUW_SUB_STEP, len); 
	if(ret) {
		printf("did no got ready from PC before sending data.\n");
		return -2;
	}

	/* 2: read identification card */
	ptdev->read(pack, len);

	/* decrypt the card */

	if(ius_check_header(ius)) {
		/* check failed */
		pass = 0;
		printf("identification failed.\n");
	}
	else
	  cdbg_verify_burn_enable(0, NULL);

	/* 3: shake to tell result */
	if(!pass)
	  /* return 2 if identification failed */
	  iuw_send_cmd(IUW_CMD_ERROR, 2, 0);
	else {
		ret = iuw_shake_hand(0, 0); 
		if(ret) {
			printf("did no got ready from PC before sending data.\n");
			return -2;
		}
	}

	return 0;
}

int iuw_process_rw(void * buf)
{
	int step = IR_MAX_BUFFER, exstep = IR_MAX_BUFFER, ret,
		id = DEV_NONE;
	int led = 0, len, progress, encrypt = 0,
		remain = iuw_cmd_len(buf);
	int opt = iuw_cmd_cmd(buf), size, hash = -1, crypt = -1;
	uint32_t max = iuw_cmd_max(buf);
	loff_t offs = iuw_cmd_offs(buf);
	struct isi_hdr *isi;
	void * key = NULL;

	printf("iuw: got command: 0x%02x\n", opt);
	if(opt == IUW_CMD_VSRD || opt == IUW_CMD_VSWR)
	  id = iuw_cmd_id(buf);
	else if(opt == IUW_CMD_NBRD || opt == IUW_CMD_NBWR)
	  id = DEV_BND;
	else if(opt == IUW_CMD_NRRD || opt == IUW_CMD_NRWR)
	  id = DEV_NAND;
	else if(opt == IUW_CMD_NFWR || opt == IUW_CMD_NFRD)
	  id = DEV_FND;
	else if(opt == IUW_CMD_MMRD || opt == IUW_CMD_MMWR) {
		offs = iuw_cmd_addr(buf);
		remain = iuw_cmd_para(buf, 1);
	}
	else if(opt == IUW_CMD_ENC || opt == IUW_CMD_DEC) {
		crypt = 1;
		exstep = IR_MAX_BUFFER >> 1;
		remain = iuw_cmd_para(buf, 1);
	}
	else if(opt == IUW_CMD_HASH) {
		hash = iuw_cmd_hash_type(buf);
		hash_init(hash, iuw_cmd_hash_len(buf));
		remain = iuw_cmd_para(buf, 1);
	}

	if(id != DEV_NONE) {
		printf("iuw: target device: %s\n", vs_device_name(id));

		/* check if the required device is capable for reading
		 * at current time. if not send error information to 
		 * indicate PC this is a wrong operation.
		 */
		ret = vs_assign_by_id(id, 1);
		if(ret < 0/* || !vs_is_opt_available(id, opt)*/) {
			printf("assign device(%s) failed.\n", vs_device_name(id));
			goto __exit_error__;
		}

		exstep = step;

		if(vs_is_device(id, "bnd") || vs_is_device(id, "nnd")
						|| vs_is_device(id, "fnd")) {
#if 0
			struct nand_config *nc;
			if(vs_is_device(id, "bnd"))
			  nc = nand_get_config(NAND_CFG_BOOT);
			else
			  nc = nand_get_config(NAND_CFG_NORMAL);
			if(!nc) {
				printf("get nand config failed\n");
				return -ENOCFG;
			}

			skip = nc->blocksize;
#endif
			if(vs_is_device(id, "fnd")) {
				if(!fnd_size_match(remain)) {
					printf("fnd data size do not match.\n");
					goto __exit_error__;
				}
				exstep = fnd_align();
				step = vs_align(id);
			}
		}
	}

	if(opt_is_write(opt) && vs_device_erasable(id))
	  vs_erase(offs, max);

	if(crypt > 0) {
		if(!iuw_cmd_crypt_isk(buf)) {
			printf("geting key from PC.\n");
			ptdev->read(pack, IUW_AES_KEYLEN);
			key = pack;
		}

		/* init AES engine */
		ret = ss_aes_init(aes_option(opt), key, __irom_ver);
		if(ret < 0) {
			printf("initialize aes engine failed.\n");
			goto  __exit_error__;
		}
	}


	/* tell PC that we are ready to send data
	 * wait PC say ready for accepting data
	 */
	ret = iuw_shake_hand(IUW_SUB_STEP, exstep);
	if(ret) {
		printf("did no got ready from PC before sending data.\n");
		return -2;
	}

	//leeway = max - remain;
	progress = remain;
	size = remain;
	printf("iuw: offset=0x%llx, size=0x%x, step=0x%x\n", offs, remain, exstep);
	while(remain > 0) {
		if((progress - remain) >> 20)
		  printf("\r%dM remains", (progress = remain) >> 20);
		/* toggle led light to indicate that we are alive */
		led_ctrl(led++ > 0);
		if(led > 50) led -= 100;

		len = min(remain, exstep);
		if(opt_is_write(opt)) {
			/* read data from PC */
			ptdev->read(pack, len);

			/* check secure if first packet */
			if(size == remain /* TODO: && opt is NBWR or VSWR */) {
				isi = (struct isi_hdr *)pack;
				if(isi_check_header(isi) == 0)
				  if(isi_is_secure(isi)) {
					  printf("iuw: data will be encrypted.\n");
					  ss_aes_init(ENCRYPTION, NULL, __irom_ver);
					  encrypt = 1;
				  }
			}
		}

		/* decrypt data if needed */
		if(encrypt)
		  ss_aes_data(pack, pack, len);
#if 0
		/* escape bad block if NANDFLASH */
		if(skip && !(offs & (skip - 1))) {
			for(; vs_isbad(offs); ) {
				if(leeway > skip) {
					offs   += skip;
					leeway -= skip;
					printf("burn skip bad block at 0x%llx\n", offs);
				} else {
					printf("no more good block availible\n");
					goto __exit_error__;
				}
			}
		}
#endif

		/* read/write data from requested device */
		if(hash >= 0)
		  iuw_local_rw(pack, offs, len, opt);
		else
		  iuw_local_rw(pack, offs, exstep, opt);

		offs += step;
		remain -= len;

		if(opt_is_read(opt)) {
//			printf("write to pc??\n");
			/* send data to PC */
			ptdev->write(pack, len);
		}

		if(crypt > 0) {
			/* handshake with PC */
			iuw_shake_hand(0, 0);
			ptdev->write(pack + (IR_MAX_BUFFER >> 1), len);
			/* handshake with PC */
			iuw_shake_hand(0, 0);
		}
	}

	if(hash >= 0) {
		iuw_shake_hand(IUW_SUB_STEP, hash_len());
		if(hash_value(pack) < 0) {
			printf("get hash value failed.\n");
			goto __exit_error__;
		}
		ptdev->write(pack, hash_len());
	}

	/* crypte mode do not need final shake */
	if(crypt < 0)
	  /* final hand shake */
	  iuw_shake_hand(0, 0);

	return 0;
__exit_error__:
	iuw_send_cmd(IUW_CMD_ERROR, 0, 0);
	return -1;
}

int iuw_server(void)
{
	int ret, cmd, retry;
	//uint32_t entry;

//	iuw_shake_hand();
	for(retry = 0;;) {
		ret =  ptdev->read(pack, IUW_CMD_LEN);

#if 0
		printf("get command:\n");
		print_buffer(pack, pack, 1, IUW_CMD_LEN, 16);
#endif

		if(ret < 0) {
			printf("get data failed. ret = %d\n", ret);
			if(retry++ < IUW_RETRY)
			  continue;
			else {
				printf("wait command timeout.\n");
				break;
			}
        }
#if 0
		else
		  printf("%x bytes received from: %s\n", ret, ptdev->name);
#endif

		if(!iuw_is_cmd(pack))
		  printf("this is not an iuw command.\n");

		cmd = iuw_cmd_cmd(pack);
		//entry = iuw_cmd_entry(pack);
		switch(cmd) {
			case IUW_CMD_READY:
				printf("unexpected ready signal.\n");
				break;
			case IUW_CMD_ERROR:
				printf("unsupported, omitted.\n");
				break;
			case IUW_CMD_IDENTIFY:
				printf("checking identity.\n");
				iuw_check_identity();
				continue;
			case IUW_CMD_FINISH:
				printf("iuw finished.\n");
				goto __iuw_finish__;
			default:
				if(!cdbg_verify_burn())
				  goto __secure_cmds;

				/* iROM is in secure_burn state
				 * commands beside above is not
				 * accepted. */
				printf("command: %d denied.\n", cmd);
		}

		iuw_send_cmd(IUW_CMD_ERROR, 0, 0);
		continue;

__secure_cmds:
		/* secure command */
		switch(cmd) {
			case IUW_CMD_VSRD:
			case IUW_CMD_VSWR:
			case IUW_CMD_MMRD:
			case IUW_CMD_MMWR:
			case IUW_CMD_NBWR:
			case IUW_CMD_NBRD:
			case IUW_CMD_NRWR:
			case IUW_CMD_NRRD:
			case IUW_CMD_NFWR:
			case IUW_CMD_HASH:
			case IUW_CMD_ENC:
			case IUW_CMD_DEC:
				iuw_process_rw(pack);
				break;
			case IUW_CMD_JUMP:
			case IUW_CMD_CONFIG:
				printf("unsupported, omitted.\n");
				iuw_send_cmd(IUW_CMD_ERROR, 0, 0);
				break;
			default:
				if(iuw_extra(pack, ptdev->read)) {
					printf("unknown iuw command: %d.\n", cmd);
					iuw_send_cmd(IUW_CMD_ERROR, 0, 0);
				}
		}
	}

__iuw_finish__:
	return iuw_cmd_para(pack, 0);
}


/* this function is invoked by main loop at initial time
 * FIXME: the default device should usb. But we can only
 * use ethernet instead as usb is not ready.
 */
int init_iuw(void)
{
	tdev_current = TDEV_ETH;
	return 0;
}

