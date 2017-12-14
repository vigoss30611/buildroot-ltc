/*
 * (C) Copyright 2008 - 2009
 * Windriver, <www.windriver.com>
 * Tom Rix <Tom.Rix@windriver.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Part of the rx_handler were copied from the Android project. 
 * Specifically rx command parsing in the  usb_rx_data_complete 
 * function of the file bootable/bootloader/legacy/usbloader/usbloader.c
 *
 * The logical naming of flash comes from the Android project
 * Thse structures and functions that look like fastboot_flash_* 
 * They come from bootable/bootloader/legacy/libboot/flash.c
 *
 * This is their Copyright:
 * 
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the 
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <asm/byteorder.h>
#include <common.h>
#include <command.h>
#include <nand.h>
#include <fastboot.h>
#include <watchdog.h> 
#include <image.h> 
#include <malloc.h> 
#include <u-boot/zlib.h> 
#include <bzlib.h> 
#include <environment.h> 
#include <lmb.h>
#include <linux/ctype.h>
#include <asm/byteorder.h>
#include <bootl.h>
#include <rtcbits.h>
#include <vstorage.h>
#include <asm/io.h>
//#include <imapx_base_reg.h>
#if  (CONFIG_FASTBOOT)
#define DEBUG_FASTBOOT

#ifdef DEBUG_FASTBOOT
#define debugp printf
#endif

/* Use do_reset for fastboot's 'reboot' command */
extern int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
/* Forward decl */
static int rx_handler (const char *buffer, unsigned int buffer_size);
static void reset_handler (void);

static struct cmd_fastboot_interface interface = 
{
	.rx_handler            = rx_handler,
	.reset_handler         = reset_handler,
	.product_name          = "imapx820 devboard",
	.serial_no             = NULL,
	.nand_block_size       = 0,
	.transfer_buffer       = (unsigned char *)0x80008000,
	.transfer_buffer_size  = 0x18000000,
};
static unsigned int download_size;
static unsigned int download_bytes;
static unsigned int download_bytes_unpadded;
static unsigned int download_error;
extern int do_setenv (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int storage_device(void);
static void set_env(const char *var, char *val)
{
	char *setenv[4]  = { "setenv", NULL, NULL, NULL, };

	setenv[1] = (char*)var;
	setenv[2] = (char*)val;

	do_setenv(NULL, 0, 3, setenv);
}
static void reset_handler ()
{
	/* If there was a download going on, bail */
	download_size = 0;
	download_bytes = 0;
	download_bytes_unpadded = 0;
	download_error = 0;
}
static int fastboot_download_image(const char *buffer, unsigned int buffer_size)
{
	char str[128];
	memset(str, 0 ,128);
	int ptr = (int) str + 4 ; 
	ptr -= (ptr&0x3)? (ptr&0x3):4;
	char* response =(char*) ptr; 

	/* Something to download */

	printf("fastboot_download_image\n");
	if (buffer_size)
	{
		/* Handle possible overflow */
		unsigned int transfer_size = 
			download_size - download_bytes;

		if (buffer_size < transfer_size)
			transfer_size = buffer_size;

		/* Save the data to the transfer buffer */
		if(interface.transfer_buffer + download_bytes != (unsigned char*)buffer){
			memcpy (interface.transfer_buffer + download_bytes, 
					buffer, transfer_size);
		}

		download_bytes += transfer_size;

		/* Check if transfer is done */
		if (download_bytes >= download_size) {
			/* Reset global transfer variable,
			   Keep download_bytes because it will be
			   used in the next possible flashing command */
			download_size = 0;

			if (download_error) {
				/* There was an earlier error */
				sprintf(response, "ERROR");
			} else {
				/* Everything has transferred,
				   send the OK response */
				sprintf(response, "OKAY");
			}
			fastboot_tx_status((unsigned char*)response, strlen(response));

			printf ("\ndownloading of %d bytes finished\n",
					download_bytes);

			/* Pad to block length
			   In most cases, padding the download to be
			   block aligned is correct. The exception is
			   when the following flash writes to the oob
			   area.  This happens when the image is a
			   YAFFS image.  Since we do not know what
			   the download is until it is flashed,
			   go ahead and pad it, but save the true
			   size in case if should have
			   been unpadded */
			download_bytes_unpadded = download_bytes;
			debugp("%d\n",interface.nand_block_size);
			if (interface.nand_block_size)
			{
				if (download_bytes % 
						interface.nand_block_size)
				{
					unsigned int pad = interface.nand_block_size - 
						(download_bytes % interface.nand_block_size);
					unsigned int i;

					for (i = 0; i < pad; i++) 
					{
						if (download_bytes >= interface.transfer_buffer_size)
							break;

						interface.transfer_buffer[download_bytes] = 0;
						download_bytes++;
					}
				}
			}
		}

		/* Provide some feedback */
		if (interface.nand_block_size && download_bytes &&
				0 == (download_bytes %
					(16 * interface.nand_block_size)))
		{
			/* Some feeback that the
			   download is happening */
			if (download_error)
				printf("X");
			else
				printf(".");
			printf("***********************\n");
			if (0 == (download_bytes %
						(80 * 16 *
						 interface.nand_block_size)))
				printf("\n");

		}
	}
	else
	{
		/* Ignore empty buffers */
		printf ("Warning empty download buffer\n");
		printf ("Ignoring\n");
	}
	return 0;

}
/**********************************************
*reboot 
*Reboot the board. 
***********************************************/

static void fastboot_do_reboot(const char* para)
{
	if(memcmp(para, "-bootloader", 11)==0){
            rtcbit_set("fastboot", 1);
	} 
	fastboot_tx_status((unsigned char*)"OKAY", 4);
	udelay (100000); /* 1 sec */
	do_reset (NULL, 0, 0, NULL);

}	
/**********************************************
*getvar
*Get common fastboot variables
*Board has a chance to handle other variables 
***********************************************/

static int fastboot_req_getvar(const char* para , char* response)
{
	strcpy(response,"OKAY");
	printf("%s\n",para); 
	if(!strncmp(para , "version",7)) 
	{
		debugp("version\n");
		//			strcpy(response + 4, FASTBOOT_VERSION);
		sprintf(response,"OKAY%s",FASTBOOT_VERSION);
	} 
	else if(!strncmp(para, "product",strlen("product"))) 
	{
		if (interface.product_name) 
			strcpy(response + 4, interface.product_name);

	} else if(!strncmp(para, "serialno",8)) {
                sprintf(response + 4,"%s",getenv("serialex"));

	} else if(!strncmp(para, "downloadsize",12)) {
		if (interface.transfer_buffer_size) 
			sprintf(response + 4, "0x%8x", (int) interface.transfer_buffer_size);
	} 
	else 
	{
		//		fastboot_getvar(para , response + 4);
	}

	return 0;
}
/*************************************** 
*erase
*Erase a register flash partition
*Board has to set up flash partitions 
 **************************************/
static int fastboot_do_erase(const char* para , char* response )
{
       return 0;
}
/***********************************************************
* download
*download something .. 
*What happens to it depends on the next command after data 
************************************************************/
static int fastboot_pre_download(const char* size , char* response)
{
			/* save the size */
			memset(size+8,0,8);
			download_size = simple_strtoul (size, NULL, 16);
			/* Reset the bytes count, now it is safe */
			download_bytes = 0;
			/* Reset error */
			download_error = 0;

			printf ("Starting download of %d bytes\n", download_size);

			if (0 == download_size)
			{
				/* bad user input */
				sprintf(response, "FAILdata invalid size");
			}
			else if (download_size > interface.transfer_buffer_size)
			{
				/* set download_size to 0 because this is an error */
				download_size = 0;
				sprintf(response, "FAILdata too large");
			}
			else
			{
				/* The default case, the transfer fits
				   completely in the interface buffer */
				sprintf(response, "DATA%08x", download_size);
			}
			return 0;

}
/*****************************************************************
*boot
*boot the system to android in specified mode
*if there is predownload image, boot it.
******************************************************************/
static int fastboot_do_boot(const char* para,char* response)
{
	
	if(download_bytes == 0)
	{
		if(!strncmp(para, "recovery",8))
			set_boot_type(BOOT_FACTORY_INIT);
		bootl();
	}
	if (CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE > download_bytes)
	{
		sprintf(response, "FAILThere may has data lost");
		return 0;
	}
	/* boot
		   boot what was downloaded

		   WARNING WARNING WARNING

		   This is not what you expect.
		   The fastboot client does its own packaging of the
		   kernel.  The layout is defined in the android header
		   file bootimage.h.  This layeout is copied looks like this,

		 **
		 ** +-----------------+
		 ** | boot header     | 1 page
		 ** +-----------------+
		 ** | kernel          | n pages
		 ** +-----------------+
		 ** | ramdisk         | m pages
		 ** +-----------------+
		 ** | second stage    | o pages
		 ** +-----------------+
		 **

		 We only care about the kernel.
		 So we have to jump past a page.

		 What is a page size ?
		 The fastboot client uses 2048

		 The is the default value of

		 CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE

		 */

	/*
	 * Use this later to determine if a command line was passed
	 * for the kernel.
	 */
	struct fastboot_boot_img_hdr *fb_hdr = malloc(sizeof( struct fastboot_boot_img_hdr));
	if(NULL == fb_hdr){
		printf("Error failed to allocate fb_hdr\n");
		sprintf(response,"FAILFailed to allocate buffer");
		return -1;
	}
	memcpy(fb_hdr, interface.transfer_buffer, sizeof( struct fastboot_boot_img_hdr));
	/* Skip to the mkbootimage header */
	char* imagebase = (char*)(interface.transfer_buffer + CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE);
	/* Execution should jump to kernel so send the response
	   now and wait a bit.  */
	sprintf(response, "OKAY");
	fastboot_tx_status((unsigned char*)response, strlen(response));
	udelay (1000000); /* 1 sec */

	if (ntohl(((image_header_t *)imagebase)->ih_magic) != IH_MAGIC) {
		sprintf(response,"FAILInvalid image header magic");
		return 0;
	}
	/* Looks like a kernel.. */
	printf ("Booting kernel..\n");

	/*
	 * Check if the user sent a bootargs down.
	 * If not, do not override what is already there
	 */
	if (strlen ((char *) &fb_hdr->cmdline[0]))
                    setenv ("bootargs", (char *) &fb_hdr->cmdline[0]);

        /* FIXME: LOGO is remove because we use a new logo mechanism */
        /* FIXME: UBOOT0 is remove because we use a new logo mechanism */

	memcpy((void*)CONFIG_SYS_PHY_LK_BASE ,(const void*) imagebase,fb_hdr->kernel_size);  
	if(image_check_hcrc((image_header_t *)CONFIG_SYS_PHY_LK_BASE) 
			&&image_check_dcrc((image_header_t *)CONFIG_SYS_PHY_LK_BASE)){
		printf("linux kernel crc ok\n");

		/*******************************************************************
		  The image packet will be CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE aligned
		  ******************************************************************/
		imagebase += (fb_hdr->kernel_size+CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE)&
			(~(CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE -1));
		memcpy((void*)CONFIG_SYS_PHY_RD_BASE ,(const void*) imagebase , fb_hdr->ramdisk_size);
		if(image_check_hcrc((image_header_t *)CONFIG_SYS_PHY_RD_BASE) 
				&&image_check_dcrc((image_header_t *)CONFIG_SYS_PHY_RD_BASE)){
			printf("ramdisk crc ok \n");
			/*******************boot the system********************/
			do_boot((void*)CONFIG_SYS_PHY_LK_BASE,(void*)CONFIG_SYS_PHY_RD_BASE);
		}
	}
	/****************************************************
	  It will never reach the below if the image is valid
	 ****************************************************/
	free(fb_hdr);
	printf ("ERROR : bootting failed\n");
	printf ("You should reset the board\n");
	sprintf(response, "FAILinvalid boot image");
	return 0;
}
/***************************************************
*flash
*Here we write the predownload data to the partition 
*sepcified by pname
***************************************************/
static int fastboot_do_flash(const char* pname , char* response)
{
	if (download_bytes) 
	{
		char fsize[16];
		sprintf(fsize,"%x",download_bytes);
		setenv("filesize",fsize);
//		struct fastboot_ptentry *ptn;

		printf("%s\n",pname);
		sprintf(response, (const char*)"OKAY");
		image_header_t * imagebase = (image_header_t *) interface.transfer_buffer;
		if(!strncmp(pname,"uboot0",6)){
			if(!isi_check_header(imagebase))
				run_command("adr uboot0",0);
		}else if(!strncmp(pname,"uboot1",6)){
			if(!isi_check_header(imagebase))
				run_command("adr uboot1", 0);
		}else if(!strncmp(pname,"kernel0",7)){
			if((image_check_hcrc(imagebase))
					&&(image_check_dcrc(imagebase))){
				run_command("adr kernel0", 0);
			}else
				sprintf(response, "FAILcrc check invalid");
		}else if(!strncmp(pname,"kernel1",7)){
			if((image_check_hcrc(imagebase))
					&&(image_check_dcrc(imagebase))){
				run_command("adr kernel1",0);
			}else
				sprintf(response, "FAILcrc check invalid");

		}else if(!strncmp(pname,"ramdisk",7)){
			if((image_check_hcrc(imagebase))
					&&(image_check_dcrc(imagebase))){
				run_command("adr ramdisk",0);
			}else
				sprintf(response, "FAILcrc check invalid");

		}else if(!strncmp(pname,"recovery",8)){
			if((image_check_hcrc(imagebase))
					&&(image_check_dcrc(imagebase))){
				run_command("adr recovery",0);
			}else
				sprintf(response, (const char*)"FAILcrc check invalid");

		}else if(!strncmp(pname,"items",5)){
			run_command("adr items",0);
		}else if((!strncmp(pname,"system",6))||(!strncmp(pname,"logo",5))){
			/********************************TODO****************************/
			/*****************************************************************
			 *to complete cases which means it should enter the recovery mode*
			 *****************************************************************/
			fastboot_tx_status((unsigned char*)"OKAY", 4);
			set_boot_type(BOOT_FACTORY_INIT);
			bootl();
		}else if(!strncmp(pname,"burninit",7)){
			run_command("adr burninit",0);
		}else{
			sprintf(response, "FAILno such partition name");
		}
	}
	else
	{

		sprintf(response, "FAILno image downloaded");
	}

	return 0;

}

typedef enum bool{
    false = 0,
    true = 1,
}bool;

static int rx_handler (const char *buffer, unsigned int buffer_size)
{
	int ret = 1;
	/* Use 65 instead of 64
	   null gets dropped  
	   strcpy's need the extra byte */
	char str[128],*p;
	memset(str, 0 ,128);
	int ptr = (int) str + 4 ; 
	ptr -= (ptr&0x3)? (ptr&0x3):4;
	char* response =(char*) ptr;
        bool reconnect = false;
	if (download_size) 
	{
		ret = fastboot_download_image(buffer,buffer_size);
	}
	else
	{
		/* A command */

		/* Cast to make compiler happy with string functions */
		const char *cmdbuf = (const char *) buffer;

		/* Generic failed response */
		sprintf(response, "FAIL");
		printf("%s\n", buffer);
		if(memcmp(cmdbuf, "reboot", 6) == 0) {
			fastboot_do_reboot(cmdbuf+6);	
		}else if(memcmp(cmdbuf, "getvar:", 7) == 0) {
			ret = fastboot_req_getvar(cmdbuf + 7 , response);			
                }else if(memcmp(cmdbuf, "setserial:", 10) == 0){
                    memset(cmdbuf + 40, 0, 10);
                    if(p = strchr(cmdbuf + 10, '%')) {
                        *p = 0;
                        setenv("serialex",cmdbuf+10);
                        saveenv();
                        sprintf(response, "OKAY");
                        reconnect = true;
                    }else
                        sprintf(response, "FAILInvalid serial number");
                }else if(memcmp(cmdbuf, "setmac:", 7) == 0){
                    memset(cmdbuf + 40, 0, 10);
                    if(p = strchr(cmdbuf + 7, '%')) {
                        *p = 0;
                        setenv("macex",cmdbuf+7);
                        saveenv();
                        sprintf(response, "OKAY");
                        reconnect = true;
                    }else
                        sprintf(response, "FAILInvalid MAC address");
                }else if(memcmp(cmdbuf, "erase:", 6) == 0){
			ret = fastboot_do_erase(cmdbuf+6 , response);
		}else if(memcmp(cmdbuf, "download:", 9) == 0) {
			ret = fastboot_pre_download(cmdbuf+9,response);
		}else if(memcmp(cmdbuf, "boot", 4) == 0) {
			ret = fastboot_do_boot(cmdbuf+4, response);
		}else if(memcmp(cmdbuf, "flash:", 6) == 0) {
			ret = fastboot_do_flash(cmdbuf + 6, response);
		}else if(memcmp(cmdbuf, "bootos", 6) == 0){
			bootl();
		}else if(memcmp(cmdbuf, "continue", 8) == 0){
			set_boot_type(BOOT_FACTORY_INIT);
			bootl();
			//ret = fastboot_do_boot(cmdbuf+9 , response);
		}else{
			sprintf(response, "FAILNO such command");
		}
		int len = strlen(response);
		if(fastboot_tx_status((unsigned char*)response, len)){ 
			printf("Error transfer status code failed\n");
			return -1;
		}

	} /* End of command */
        if(true == reconnect)
        {
            udc_disconnect();
            udc_connect ();
        }
	return ret;
}
int fastboot(void)
{		
	int ret = 1;
	int len =0;
	printf("do fastboot\n");
	unsigned char* cmdbuf = (unsigned char*)malloc((size_t)0x40);
	if(NULL == cmdbuf) {
		printf("Error: failed to allocate buffer for fastboot command\n");
		return -1;
	}
	unsigned char* buffer = NULL;
	/* Initialize the board specific support */
	if (0 == fastboot_init(&interface))
	{
		printf ("Disconnect USB cable to finish fastboot..\n");

		/* If we got this far, we are a success */
		ret = 0;


		/* On disconnect or error, polling returns non zero */
		while (1)
		{
			if(download_size == 0){
				len = 0x40;
				buffer = cmdbuf;
			}else{ 
				len = download_size;
				buffer = interface.transfer_buffer + download_bytes;
			}
			if (fastboot_poll(buffer,len))
				break;
		}
	}

	/* Reset the board specific support */
	fastboot_shutdown();

	return ret;

}
int do_fastboot(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	return fastboot();
}
U_BOOT_CMD(
		fastboot,1,1,do_fastboot,
		"fastboot- use USB Fastboot protocol\n",
		NULL
		);
#endif	/* CONFIG_FASTBOOT */
