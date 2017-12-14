/** ===========================================================================
* @file sd_config.c
*
* @path $(IPNCPATH)\sys_server\src\
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2009
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */

#include <asm/types.h>
//#include <dm355_gio_util.h>
#include <stdio.h>
#include <stdlib.h>
//#include <file_msg_drv.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/statfs.h>
#include <sys/prctl.h> 
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/fs.h>   
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

#include "system_msg.h"
#include "hd_mng.h"
#include "AudioPrompt.h"
//#include "librecord.h"

#include <qsdk/event.h>
#include <qsdk/sys.h>
#include "ipc.h"
#define LOGTAG  "HD_MNG"

enum usb_ver
{
	VER_1_0 = 1,		//1.5Mbps
	VER_1_1,		//12Mbps
	VER_2_0,		//480Mbps
	VER_3_0,		//5Gbps
};

struct disk_information
{
	char DevPath[32];
	char MountPath[16];
	int  DiskNo;			//硬盘编号
	unsigned char type;		//0:  sd卡   1:  U盘
	unsigned char used;		//是否使用
	unsigned char Isformat;	//是否格式化0:未格式化,1:格式化正常,2:正在格式化,3:磁盘错误,4:mount过程中
	unsigned char IsMount;	//是否已经mount
	unsigned char partition;	//分区号, 0无分区
	unsigned char reserved[3];
};

struct format_status
{
	int DiskNo;			//硬盘编号
	unsigned char result;	//格式化结果:  0-成功  1-失败
	unsigned char status;	//硬盘状态： 0－格式化开始；1－正在格式化磁盘；2－格式化完成
	unsigned char process;	//进度:  0-100
	unsigned char formating;	//0: 格式线程没有运行     1: 正在格式化
	
};


//static DMS_NAS_SERVER_INFO g_nas_cfg;

//static int (*g_SetNas)(DMS_NET_NAS_CFG *);
//static int (*g_UnSetNas)();

//static pthread_mutex_t g_nas_lock = PTHREAD_MUTEX_INITIALIZER;

#define DEVICE_TYPE_SD	0
#define DEVICE_TYPE_USB	1
#define DEVICE_TYPE_NAS	2


#define HD_INSERTED 0x01
#define HD_MOUNTED  0x02
#define HD_INSERT_AND_MOUNT 0x03

#define PARTITION_MAX			4

#define __E(fmt, args...) fprintf(stderr, "Error: " fmt, ## args)

static unsigned char gbSD_WriteProtect = 0;
static unsigned char gSdStatus = 0x00;
static unsigned char gUsbStatus = 0x00;

#define SD_FS_PATH		"/mnt/mmc"
//#define DELETE_OLD_FILE_CMD 	"find /mnt/mmc/ipnc/ -type f | xargs ls -tr | head -1 | xargs rm  "

#if 0
#define SdIsInsert dm355_gio_read(GIO_SD_DETECT) ? 1 : 0
#define SdWRProtect dm355_gio_read(GIO_SD_WRITE_PROTECT) ? 0 : 1
#endif

static struct disk_information g_sdinfo[PARTITION_MAX];
static struct disk_information g_usbinfo[PARTITION_MAX];
//static struct disk_information g_nasinfo;//nas

static struct format_status g_formatinfo[PARTITION_MAX];

static unsigned char UsbDiskNum = 0, SdDiskNum = 0, UsbDiskNo_min = 0, SdDiskNo_min = 0;

static HDmngCallback g_eventcallback=NULL;
static HDmng_GetSdWriteProtectFun g_fWriteProtect=NULL;
static unsigned char g_usbversion = VER_2_0;

static unsigned char module_deinit = 0;
//static pthread_t usb_detect_tid = -1;



static void mSleep(unsigned int mSecond);


static int DiskEventNotify(int event, int diskNo, void *priv)
{
    DISK_INFO_T diskInfo;
    diskInfo.event = event;
    diskInfo.diskNo = diskNo;
    diskInfo.priv  = priv;    
    QM_Event_Send(EVENT_HD_NOTIFY, &diskInfo, sizeof(diskInfo));
}


#if 0
static int CheckUsbVersion()
{
	FILE *fp;
	char *p;
	int speed;
	char buf[128] = {0};
	char Tline[128] = {0};
	
	fp = fopen("/proc/bus/usb/devices", "r");
	if(!fp)
	{
		printf("%s  %d, fopen error!%s(errno:%d)\n", __FUNCTION__, __LINE__, strerror(errno), errno);
		return -1;
	}

	while(fgets(buf, 128, fp))
	{
		if(strstr(buf, "T:"))
		{
			strcpy(Tline, buf);
		}
		else if(strstr(buf, "usb-storage"))
		{
			p = strstr(Tline, "Spd=");
			if(!p)
			{
				fclose(fp);
				return -2;
			}

			speed = atoi(p+strlen("Spd="));
			if(speed == 480)
				g_usbversion = VER_2_0;
			else if(speed == 12)
				g_usbversion = VER_1_1;
			else if(speed > 480)
				g_usbversion = VER_3_0;
			else
				g_usbversion = VER_1_0;

			break;
		}
	}

	fclose(fp);

	return g_usbversion;
}
#endif
static int GetDiskNum_sd(BOOL bHasPartition)
{
	int i = 0, count=0;
	char path[32];
	char cmd[128];
	
	if (bHasPartition)
	{
		for(i=1;i<10 && count < PARTITION_MAX;i++)
		{
			sprintf(path, "/dev/mmcblk0p%d", i);
			if(access(path, F_OK)==0)
			{
				if(count+1 == PARTITION_MAX)
					strcpy(g_sdinfo[count+1].MountPath, "/mnt/mmc");
				else
					sprintf(g_sdinfo[count+1].MountPath, "/mnt/mmc%d", count+1);

				strcpy(g_sdinfo[count+1].DevPath, path);
				sprintf(cmd, "mkdir -p %s", g_sdinfo[count+1].MountPath);
				printf("%s.%d cmd:%s. \n",__FUNCTION__,__LINE__,cmd);
				SystemCall_msg(cmd, SYSTEM_MSG_BLOCK_RUN);
				g_sdinfo[count+1].used = 1;
				g_sdinfo[count+1].partition = i;
				g_sdinfo[count+1].type = DEVICE_TYPE_SD;
				count++;
				printf("%s.%d count:%d\n",__FUNCTION__,__LINE__,count);
			}
			else
			{
				printf("%s.%d count:%d\n",__FUNCTION__,__LINE__,count);
				break;
			}
		}
	}

	if(!count)
	{
		if(access("/dev/mmcblk0", F_OK)==0)
		{
			count =1;
			strcpy(g_sdinfo[0].DevPath, "/dev/mmcblk0");
			strcpy(g_sdinfo[0].MountPath, "/mnt/mmc");
			g_sdinfo[0].used = 1;
			g_sdinfo[0].partition = 0;
			g_sdinfo[0].type = DEVICE_TYPE_SD;
			printf("%s.%d \n",__FUNCTION__,__LINE__);
		}
	}

	return count;
}

static int GetDiskNum_usb(const char *usbbasename, BOOL bHasPartition)
{
	int i, count=0;
	char path[32];
	int wait_count = 0;

	while (bHasPartition && !count && wait_count <= 50)
	{
		
		mSleep(100);
		for(i=1;i<10 && count < PARTITION_MAX;i++)
		{
			sprintf(path, "%s%d", usbbasename, i);
			if(access(path, F_OK)==0)
			{
				count++;
				strcpy(g_usbinfo[count].DevPath, path);
				if(count == PARTITION_MAX)
					strcpy(g_usbinfo[count].MountPath, "/mnt/usb");
				else
					sprintf(g_usbinfo[count].MountPath, "/mnt/usb%d", count);
				g_usbinfo[count].used = 1;
				g_usbinfo[count].partition = i;
				g_usbinfo[count].type = DEVICE_TYPE_USB;

				printf("mount path is %s\n", g_usbinfo[count].MountPath);
			}
			else
			{
				break; //分区挂载都是连续的
			}
		}
	}
	
	if(!count)
	{
		if(access(usbbasename, F_OK)==0)
		{
			count =1;
			strcpy(g_usbinfo[0].DevPath, usbbasename);
			strcpy(g_usbinfo[0].MountPath, "/mnt/usb");
			g_usbinfo[0].used = 1;
			g_usbinfo[0].partition = 0;
			g_usbinfo[0].type = DEVICE_TYPE_USB;
		}
	}

	//CheckUsbVersion();

	return count;
}

static int IsSdInserted()
{
	if(!access("/dev/mmcblk0", F_OK))
	{
		return 1;
	}
	
	return 0;
}

/**
* @brief Check if USB is inserted
*
* @retval 1 Inserted
* @retval 0 Not inserted
*/
static int IsUsbInserted(char *pUsbBaseName)
{
	int i;
	char DevPath[16] = {0};

	for(i='a';i<='z';i++)
	{
		sprintf(DevPath, "/dev/sd%c", i);
		if(!access(DevPath, F_OK))
		{
			if(pUsbBaseName)
				strcpy(pUsbBaseName, DevPath);

			return 1;
		}
	}
	
	return 0;
}

#if 0
static int UmountAllSd()
{
	char cmd[32];
	int i, ret, count;

	if(!(gSdStatus&HD_MOUNTED))
	{
		return 0;
	}
	
	for(i=0;i<PARTITION_MAX;i++)
	{
		if(g_sdinfo[i].IsMount)
		{
			count = 0;
			g_sdinfo[i].used = 0;
			g_sdinfo[i].IsMount = 0;
			
			if(g_eventcallback)
			{
				g_eventcallback(HD_EVENT_UNMOUNT, g_sdinfo[i].DiskNo, NULL);
			}
			
			sprintf(cmd, "umount %s", g_sdinfo[i].MountPath);
			while((ret= SystemCall_msg(cmd, SYSTEM_MSG_BLOCK_RUN)) == 256)
			{
				mSleep(1000);
				if(count ++ > 10)
				{
					return -1;
				}
			}
		}
	}
	

	gSdStatus &= ~HD_MOUNTED;

	return 0;
}
#endif

static int StopDiskTask(int type, struct disk_information *pDiskInfo)
{
    int i;
    int ret = 0;

     memset(pDiskInfo, 0, sizeof(struct disk_information)*PARTITION_MAX);
    do
    {
        if (type == DEVICE_TYPE_USB)
        {
            if (!(gUsbStatus&HD_MOUNTED))
            {
                break;
            }
            memcpy(pDiskInfo, g_usbinfo, sizeof(g_usbinfo));
        }
        else
        {
            if (!(gSdStatus&HD_MOUNTED))
            {
                break;
            }
            memcpy(pDiskInfo, g_sdinfo, sizeof(g_sdinfo));
        }
        ret = 1;
    } while (0);

    if (type == DEVICE_TYPE_USB)
    {
        memset(g_usbinfo, 0, sizeof(g_usbinfo));
        for (i = 0; i < PARTITION_MAX; i++)
        {
            g_usbinfo[i].DiskNo = -1;
        }
        gUsbStatus = 0;
        UsbDiskNum = 0;
        UsbDiskNo_min = 0;
    }
    else
    {
        memset(g_sdinfo, 0, sizeof(g_sdinfo));
        for (i = 0; i < PARTITION_MAX; i++)
        {
            g_sdinfo[i].DiskNo = -1;
        }
        gSdStatus = 0;
        SdDiskNum = 0;
        SdDiskNo_min = 0;
    }
    for(i=0;i<PARTITION_MAX;i++)
    {
        if(pDiskInfo[i].IsMount && g_eventcallback)
        {
            if (g_eventcallback) {
                g_eventcallback(HD_EVENT_UNMOUNT, pDiskInfo[i].DiskNo, NULL);
                usleep(200*1000);
            }

            DiskEventNotify(HD_EVENT_UNMOUNT, pDiskInfo[i].DiskNo, NULL);
        }
    }
    return ret;
}

static void UmountDisk(const struct disk_information *pDiskInfo)
{
	printf("umount all disk!\n");
	char cmd[32];
	int i, ret, count;
	
	for(i=0;i<PARTITION_MAX;i++)
	{
		if(pDiskInfo[i].IsMount)
		{
			count = 0;
			
			sprintf(cmd, "umount %s", pDiskInfo[i].MountPath);
			while((ret= SystemCall_msg(cmd, SYSTEM_MSG_BLOCK_RUN)) == 256)
			//while((ret = system(cmd)) == 256)
			{
				printf("umount ret is %d\n", ret);
				mSleep(1000);
				if(count ++ > 3)
				{
					break;
				}
			}

			//printf("umount ret is %d\n", ret);
		}
	}
}

#if 0
static int UmountAllUsb()
{
	printf("umount all usb!\n");
	char cmd[32]={0};
	int i, ret, count;

	if(!(gUsbStatus&HD_MOUNTED))
	{
		return 0;
	}
	
	for(i=0;i<PARTITION_MAX;i++)
	{
		if(g_usbinfo[i].IsMount)
		{
			count = 0;
			g_usbinfo[i].used = 0;
			g_usbinfo[i].IsMount = 0;
			
			if(g_eventcallback)
			{
				g_eventcallback(HD_EVENT_UNMOUNT, g_usbinfo[i].DiskNo, NULL);
			}
			
			sprintf(cmd, "umount %s", g_usbinfo[i].MountPath);
			while((ret= SystemCall_msg(cmd, SYSTEM_MSG_BLOCK_RUN)) == 256)
			{
				printf("umount ret is %d\n", ret);
				mSleep(1000);
				if(count ++ > 10)
				{
					return -1;
				}
			}

			printf("umount ret is %d\n", ret);
		}
	}

	gUsbStatus &= ~HD_MOUNTED;

	return 0;
}
#endif

#if 0
static int UmountAllNAS()
{
	char cmd[32];
	int ret, count;
	
	//for(i=0;i<8;i++)
	{
		if(g_nasstatus.IsMount)
		{
			count = 0;
			if(g_eventcallback)
				g_eventcallback(NAS_EVENT_UNMOUNT, g_nasstatus.diskNo, NULL);

			sprintf(cmd, "umount %s", g_nasstatus.MountPath);
			while((ret= SystemCall_msg(cmd, SYSTEM_MSG_BLOCK_RUN)) == 256)
			{
				mSleep(2000);
				if(count ++ > 3)
					return -1;
			}

			g_nasstatus.IsMount = 0;
			g_nasstatus.status = 0;
            		g_nasstatus.release = 1;
		}
	}

	return 0;
}
#endif

static int disk_mount(struct disk_information *pDiskInfo)
{
	int ret = 0;
	char cmd[64];

	sprintf(cmd, "mount -t vfat %s %s", pDiskInfo->DevPath, pDiskInfo->MountPath);
	printf("%s.%d cmd:%s. \n",__FUNCTION__,__LINE__,cmd);
	ret = SystemCall_msg(cmd, SYSTEM_MSG_BLOCK_RUN);
	//ret = system(cmd);
	if(ret)
	{
		if(g_eventcallback)
            g_eventcallback(HD_EVENT_ERROR, pDiskInfo->DiskNo, NULL);

        DiskEventNotify(HD_EVENT_ERROR, pDiskInfo->DiskNo, NULL);
    }
    else
    {
        if(g_eventcallback)
            g_eventcallback(HD_EVENT_MOUNT,pDiskInfo->DiskNo, NULL);

        DiskEventNotify(HD_EVENT_MOUNT,pDiskInfo->DiskNo, NULL);

        pDiskInfo->IsMount = 1;

        if(pDiskInfo->type == DEVICE_TYPE_SD)
		{
			gSdStatus = HD_INSERT_AND_MOUNT;
		}
		else
		{
			gUsbStatus = HD_INSERT_AND_MOUNT;
		}
	}

	return ret;
}

static int disk_umount(struct disk_information *pDiskInfo)
{
	int ret, count = 0;
	char cmd[32] = {0};

	if(!pDiskInfo->IsMount)
		return 0;

	if(g_eventcallback)
		g_eventcallback(HD_EVENT_UNMOUNT, pDiskInfo->DiskNo, NULL);

    DiskEventNotify(HD_EVENT_UNMOUNT, pDiskInfo->DiskNo, NULL);

    sprintf(cmd, "umount %s", pDiskInfo->MountPath);

	if(pDiskInfo->partition == 0)
	{
		if(pDiskInfo->type == DEVICE_TYPE_SD)
		{
			gSdStatus &= ~HD_MOUNTED;
		}
		else
		{
			gUsbStatus &= ~HD_MOUNTED;
		}
	}

	pDiskInfo->IsMount = 0;
	printf("disk_umount. %s \n",cmd);
	while((ret= SystemCall_msg(cmd, SYSTEM_MSG_BLOCK_RUN)) == 256)
	//while((ret = system(cmd)) == 256)
	{
		mSleep(2000);
		if(count ++ > 3)
			return -1;
	}
	return 0;
}

static void *format_thread(void *pArg)
{
	int i, ret = 0, find = 0;
	char cmd[32];
	int diskstatus = 0;
	struct format_status *pFormatStatus = NULL;
	struct disk_information *pDiskInfo = (struct disk_information *)pArg;

    for(i=0; i<PARTITION_MAX; i++)
	{
		if(pDiskInfo->DiskNo == g_formatinfo[i].DiskNo)
		{
			pFormatStatus = &g_formatinfo[i];
			pFormatStatus->formating = 1;
            find = 1;
			break;
		}
	}

    if(!find)
    {
//        free(pArg);
	    return (void *)0;
    }
    
    if(pDiskInfo->type == DEVICE_TYPE_SD || pDiskInfo->type == DEVICE_TYPE_USB)
    {
    	if(pDiskInfo->type == DEVICE_TYPE_SD)
    		diskstatus = gSdStatus;
    	else
    		diskstatus = gUsbStatus;

    	if(diskstatus == HD_INSERT_AND_MOUNT)
    	{
    		if(0 > disk_umount(pDiskInfo))
    		{
	    		__E("HD unmount fail so HD format fail\n");
    		} 
    	}
    	
    	mSleep(100);
    	pFormatStatus->status= 1;
    	pDiskInfo->Isformat = 2;
    	sprintf(cmd, "mkdosfs -F 32 %s\n", pDiskInfo->DevPath);
		printf("format_thread cmd: %s\n",cmd);
    	ret = SystemCall_msg(cmd, SYSTEM_MSG_BLOCK_RUN);
    	//ret = system(cmd);
    	if(ret)
    	{
    		__E("HD format fail\n");
    		pFormatStatus->result = 1;
    		pDiskInfo->Isformat = 0;
    		if(g_eventcallback)
    			g_eventcallback(HD_EVENT_ERROR, pDiskInfo->DiskNo, NULL);
    	
            DiskEventNotify(HD_EVENT_ERROR, pDiskInfo->DiskNo, NULL);
        }
        else
        {
            pFormatStatus->result = 0;
    		disk_mount(pDiskInfo);
    		pDiskInfo->Isformat = 1;
    	}

    	pFormatStatus->status = 2;
    	pFormatStatus->process = 100;
    	mSleep(100);
    	pFormatStatus->formating = 0;
    }
  
//    free(pArg);
	return (void *)0;
}

static int disk_format(struct disk_information *pDiskInfo)
{
	int i, ret = 0, find=0;
	int IsInsert = 0;

	if(pDiskInfo->type == DEVICE_TYPE_SD)
	{
		IsInsert = IsSdInserted();

		if(!IsInsert)
			return -1;
		
		gbSD_WriteProtect = g_fWriteProtect();
		if(gbSD_WriteProtect )
			return -2;
	}
	else
	{
		IsInsert = IsUsbInserted(NULL);
		if(!IsInsert)
			return -1;
	}

	for(i=0;i<PARTITION_MAX;i++)
	{
		if(pDiskInfo->DiskNo == g_formatinfo[i].DiskNo)
		{
			if(g_formatinfo[i].formating)//是否正在格式化
				return -2;
			
			memset(&g_formatinfo[i], 0, sizeof(struct format_status));
			g_formatinfo[i].DiskNo = pDiskInfo->DiskNo;
			find = 1;
			break;
		}
	}

	if(!find)
	{
		for(i=0;i<PARTITION_MAX;i++)
		{
			if(g_formatinfo[i].DiskNo == -1)
			{
				g_formatinfo[i].DiskNo = pDiskInfo->DiskNo;
				g_formatinfo[i].process = 0;
				g_formatinfo[i].status = 0;
				find = 1;
				break;
			}
		}
	}

	if(!find)
		return -1;
	
	if(g_eventcallback)
        g_eventcallback(HD_EVENT_FORMAT, pDiskInfo->DiskNo, NULL);
    
    DiskEventNotify(HD_EVENT_FORMAT, pDiskInfo->DiskNo, NULL);

    disk_umount(pDiskInfo);

    pthread_t  tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    ret = pthread_create(&tid, &attr, format_thread, pDiskInfo);
	if(ret)
	{
		printf("%s  %d, create thread fail!ret=%d,%s(errno:%d)\n", __FUNCTION__, __LINE__, ret, strerror(errno), errno);
	}

	pthread_attr_destroy(&attr);
		
	return ret;
}


#define UEVENT_BUFFER_SIZE (2 * 1024)

static int init_hotplug_sock() 
{ 
	const int buffersize = 1024; 
	int ret; 

	struct sockaddr_nl snl; 
	bzero(&snl, sizeof(struct sockaddr_nl)); 
	snl.nl_family = AF_NETLINK; 
	snl.nl_pid = getpid(); 
	snl.nl_groups = 1; 

	int s = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT); 
	if (s == -1)  
	{ 
		perror("socket"); 
		return -1; 
	} 
	setsockopt(s, SOL_SOCKET, SO_RCVBUF, &buffersize, sizeof(buffersize)); 

	ret = bind(s, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl)); 
	if (ret < 0)  
	{ 
		perror("bind"); 
		close(s); 
		return -1; 
	} 

	return s; 
} 

#if 0
static void *usb_detect_thread() 
{ 
	int i;
	char buf[UEVENT_BUFFER_SIZE]; 

	int hotplug_sock = init_hotplug_sock();
	if(hotplug_sock<0)
	{
		return (void *)-1;
	}
	prctl(PR_SET_NAME, (unsigned long)"usb_detect", 0,0,0);

	while(!module_deinit) 
	{
		/* Netlink message buffer */ 
		memset(buf, 0, UEVENT_BUFFER_SIZE);
		recv(hotplug_sock, &buf, sizeof(buf), 0); 
		if(module_deinit)
			break;
		
		printf("usb_detect_thread: %s\n", buf); 
		/* 
		USB 设备的插拔会出现字符信息，
		通过比较不同的信息确定特定设备的插拔
		*/
		if(strstr(buf, "remove"))
		{
			if(strstr(buf, "usb")&&(gUsbStatus&HD_INSERTED))
			{
				printf("umount usb!\n");
				UmountAllUsb();

				memset(&g_usbinfo, 0, sizeof(struct disk_information)*PARTITION_MAX);
				for(i=0;i<PARTITION_MAX;i++)
				{
					g_usbinfo[i].DiskNo = -1;
				}
				gUsbStatus = 0;
				UsbDiskNum = 0;
				UsbDiskNo_min = 0;
				break;
			}
			else if(strstr(buf, "mmcblk0")&&(gSdStatus&HD_INSERTED))
			{
				UmountAllSd();

				memset(&g_sdinfo, 0, sizeof(struct disk_information)*PARTITION_MAX);
				for(i=0;i<PARTITION_MAX;i++)
				{
					g_sdinfo[i].DiskNo = -1;
				}
				gSdStatus = 0;
				SdDiskNum = 0;
				SdDiskNo_min = 0;
				break;
			}
			
		}

	}
	
	close(hotplug_sock);
	usb_detect_tid = -1;
	
	return (void *)0; 
}
#endif

#if 0
static void UmountSD()
{
	if (g_sdinfo[0].used && g_sdinfo[0].IsMount)
	{
		disk_umount(&g_sdinfo[0]);
		g_sdinfo[0].Isformat = 4;
		g_sdinfo[0].IsMount = 0;
	}
	MountSD(TRUE);
}
#endif

static void MountSD(BOOL bHasPartition)
{
	int sd_insert = 0;
	int i = 0;
	int ret = 0;
	char cmd[64] = {0};
	
    //sd_insert = dm355_gio_read(GIO_SD_DETECT) ? 1 : 0;
    sd_insert = IsSdInserted();
    printf("sd_insert:%d gSdStatus:0x%x\n", sd_insert, gSdStatus);
    
    if(sd_insert && !(gSdStatus&HD_INSERTED))
    {
    	fprintf(stderr,"%s  %d, aaaaaaaaa, sd insert!\n",__FUNCTION__, __LINE__);
    	gbSD_WriteProtect = g_fWriteProtect();
    	int sd_count = 0;
    
    	sd_count = GetDiskNum_sd(bHasPartition);
    	if(sd_count == 0)
    	{
    		printf("%s  %d, mmc gpio detect error!\n",__FUNCTION__, __LINE__);
    		return ;
    	}
    
    	SdDiskNum = sd_count;
    	
    	if(!UsbDiskNum || (sd_count <= UsbDiskNo_min))
    	{
    		SdDiskNo_min = 0;
    	}
    	else //if(sd_count > UsbDiskNo_min)
    	{
    		SdDiskNo_min = UsbDiskNo_min+UsbDiskNum;
    			
    	}
    	
    	//有分区
    	if(g_sdinfo[0].used==0)
    	{
    		for(i=1; i<=sd_count; i++)
    		{
    			g_sdinfo[i].Isformat = 4;//Mount过程中
    		}
    		for(i=1; i<=sd_count; i++)
    		{
    			g_sdinfo[i].DiskNo = SdDiskNo_min+ i -1;
    			sprintf(cmd, "mount -t vfat %s %s", g_sdinfo[i].DevPath, g_sdinfo[i].MountPath);
				printf("%s.%d cmd:%s. \n",__FUNCTION__,__LINE__,cmd);
    			ret = SystemCall_msg(cmd, SYSTEM_MSG_BLOCK_RUN);
                if(ret)
    			{
    				g_sdinfo[i].Isformat = 0;
    				g_sdinfo[i].IsMount = 0;
    				gSdStatus |= HD_INSERTED;
    				if(g_eventcallback)
    					g_eventcallback(HD_EVENT_UNFORMAT, g_sdinfo[i].DiskNo, NULL);

                    DiskEventNotify(HD_EVENT_UNFORMAT, g_sdinfo[i].DiskNo, NULL);
                }
                else
                {
                    g_sdinfo[i].Isformat = 1;
                    g_sdinfo[i].IsMount = 1;
                    gSdStatus = HD_INSERT_AND_MOUNT;
                    if(g_eventcallback)
                        g_eventcallback(HD_EVENT_MOUNT, g_sdinfo[i].DiskNo, NULL);

                    DiskEventNotify(HD_EVENT_MOUNT, g_sdinfo[i].DiskNo, NULL);
                }

            }
        }
        else		//如无分区就对大区进行操作
    	{
    		g_sdinfo[0].DiskNo = SdDiskNo_min;
    		g_sdinfo[0].Isformat = 4;//Mount过程中
    		sprintf(cmd, "mount -t vfat %s %s", g_sdinfo[0].DevPath, g_sdinfo[0].MountPath);
			printf("%s.%d cmd:%s. \n",__FUNCTION__,__LINE__,cmd);
    		ret = SystemCall_msg(cmd, SYSTEM_MSG_BLOCK_RUN);
    		//ret = system(cmd);
    		if(ret)
    		{
    			g_sdinfo[0].Isformat = 0;
    			g_sdinfo[0].IsMount = 0;
    			gSdStatus = HD_INSERTED;
    			if(g_eventcallback)
    				g_eventcallback(HD_EVENT_UNFORMAT, g_sdinfo[0].DiskNo, NULL);

                DiskEventNotify(HD_EVENT_UNFORMAT, g_sdinfo[0].DiskNo, NULL);
            }
            else
            {
                g_sdinfo[0].Isformat = 1;
                g_sdinfo[0].IsMount = 1;
                gSdStatus = HD_INSERT_AND_MOUNT;
                if(g_eventcallback)
                    g_eventcallback(HD_EVENT_MOUNT, g_sdinfo[0].DiskNo, NULL);
                
                DiskEventNotify(HD_EVENT_MOUNT, g_sdinfo[0].DiskNo, NULL);
            }

        }
        
        QMAPI_AudioPromptPlayFile("/root/qm_app/res/sound/mount.wav", 0);
    }
}

static void MountUsb(BOOL bHasPartition)
{
    char cmd[64] = {0};
    char usbbasename[16]={0};
	int i = 0;
	int ret = 0;
	
	int usb_insert = IsUsbInserted(usbbasename);
	if(usb_insert && !(gUsbStatus&HD_INSERTED))
	{
		UsbDiskNum = GetDiskNum_usb(usbbasename, bHasPartition);
	
		if(!SdDiskNum || (UsbDiskNum<=SdDiskNo_min))
			UsbDiskNo_min = 0;
		else //if(usb_count > SdDiskNo_min)
		{
			UsbDiskNo_min = SdDiskNo_min+SdDiskNum;
				
		}
	
		//有分区
		if(g_usbinfo[0].used==0)
		{
			for(i=1;i<=UsbDiskNum;i++)
			{
				g_usbinfo[i].Isformat = 4;//Mount过程中
			}
			for(i=1;i<=UsbDiskNum;i++)
			{
				g_usbinfo[i].used = 1;
				g_usbinfo[i].partition = i;
				g_usbinfo[i].DiskNo = UsbDiskNo_min+ i -1;
				sprintf(cmd, "mount -t vfat %s %s", g_usbinfo[i].DevPath, g_usbinfo[i].MountPath);
				ret = SystemCall_msg(cmd, SYSTEM_MSG_BLOCK_RUN);
				//ret = system(cmd);
				printf("mount: %s ret:%d\n", cmd, ret);
				if(ret)
				{
					g_usbinfo[i].Isformat = 0;
					g_usbinfo[i].IsMount = 0;
					gUsbStatus = HD_INSERTED;
                    if(g_eventcallback)
                        g_eventcallback(HD_EVENT_UNFORMAT, g_usbinfo[i].DiskNo, NULL);

                    DiskEventNotify(HD_EVENT_UNFORMAT, g_usbinfo[i].DiskNo, NULL);
                }
				else
				{
					g_usbinfo[i].Isformat = 1;
					g_usbinfo[i].IsMount = 1;
					gUsbStatus = HD_INSERT_AND_MOUNT;
					if(g_eventcallback)
						g_eventcallback(HD_EVENT_MOUNT, g_usbinfo[i].DiskNo, NULL);
						
                    DiskEventNotify(HD_EVENT_MOUNT, g_usbinfo[i].DiskNo, NULL);
                }

            }
        }
        else		//如无分区就对大区进行操作
        {
			g_usbinfo[0].DiskNo = UsbDiskNo_min;		//DiskNo从0开始
			g_usbinfo[0].Isformat = 4;//Mount过程中
			sprintf(cmd, "mount -t vfat %s %s", g_usbinfo[0].DevPath, g_usbinfo[0].MountPath);
			ret = SystemCall_msg(cmd, SYSTEM_MSG_BLOCK_RUN);
			//ret = system(cmd);
			if(ret)
			{
				g_usbinfo[0].Isformat = 0;
				g_usbinfo[0].IsMount = 0;
				gUsbStatus = HD_INSERTED;
				if(g_eventcallback)
					g_eventcallback(HD_EVENT_UNFORMAT, g_usbinfo[0].DiskNo, NULL);
		        
                DiskEventNotify(HD_EVENT_UNFORMAT, g_usbinfo[0].DiskNo, NULL);
            }
			else
			{
				g_usbinfo[0].Isformat = 1;
				g_usbinfo[0].IsMount = 1;
				gUsbStatus = HD_INSERT_AND_MOUNT;
				if(g_eventcallback)
					g_eventcallback(HD_EVENT_MOUNT, g_usbinfo[0].DiskNo, NULL);
			
                DiskEventNotify(HD_EVENT_MOUNT, g_usbinfo[0].DiskNo, NULL);
            }
        }
    }	
}

static void *hd_mng_thread()
{
    prctl(PR_SET_NAME, (unsigned long)"hdmng", 0,0,0);
    pthread_detach(pthread_self());

	int hotplug_sock = init_hotplug_sock();
	if (hotplug_sock < 0)
	{
		printf("create netlink socket failed.\n");
		return NULL;
	}
	
	struct timeval tv;
	fd_set read_set;
	int len = 0;
	
	char buf[UEVENT_BUFFER_SIZE];

    sleep(2);
    //MountUsb(TRUE);
    MountSD(TRUE);
	while(!module_deinit)
	{
		FD_ZERO(&read_set);
		FD_SET(hotplug_sock, &read_set);

		tv.tv_sec = 1;
		tv.tv_usec = 0;

		if (select(hotplug_sock + 1, &read_set, NULL, NULL, &tv) <= 0)
		{
			continue;
		}

		len = recv(hotplug_sock, buf, UEVENT_BUFFER_SIZE, 0);
		if (len < 3)
		{
			continue;
		}

		buf[len] = '\0';
		//printf("hd_mng_thread: %s\n", buf);

		if(!strncmp(buf, "add@", strlen("add@")) && strstr(buf, "block"))
		{
			BOOL isMMC = FALSE;
			if (strstr(buf, "/mmc_host/"))
			{
				isMMC = TRUE;
			}

			char tmp_str[UEVENT_BUFFER_SIZE];
			
			tv.tv_sec = 0;
			tv.tv_usec = 100000;

			FD_ZERO(&read_set);
			FD_SET(hotplug_sock, &read_set);

			//收取挂载分区的消息
			BOOL bHasPartition = FALSE;
			while (select(hotplug_sock + 1, &read_set, NULL, NULL, &tv) > 0)
			{
				len = recv(hotplug_sock, tmp_str, UEVENT_BUFFER_SIZE, MSG_PEEK);
				if (len > 3)
				{
					tmp_str[len] = '\0';
					if (!strncmp(tmp_str, buf, strlen(buf)))
					{
						bHasPartition = TRUE;
						
						printf("hd_mng_thread partition: %s\n", tmp_str);
						recv(hotplug_sock, tmp_str, UEVENT_BUFFER_SIZE, 0);
					}
				}

				tv.tv_sec = 0;
				tv.tv_usec = 100000;

				FD_ZERO(&read_set);
				FD_SET(hotplug_sock, &read_set);				
			}

			if (isMMC)
			{
				MountSD(bHasPartition);
			}
			else
			{
				//MountUsb(bHasPartition);
			}
		}
		else if(!strncmp(buf, "remove@", strlen("remove@")))
		{
			int ret = 0;
			struct disk_information info[PARTITION_MAX];
			if (strstr(buf, "mmcblk0"))
			{
				ret = StopDiskTask(DEVICE_TYPE_SD, info);
			}
			else if (strstr(buf, "usb"))
			{
				//ret = StopDiskTask(DEVICE_TYPE_USB, info);
			}

			if (!ret)
			{
				continue;
			}
			
			tv.tv_sec = 2;
			tv.tv_usec = 0;

			FD_ZERO(&read_set);
			FD_SET(hotplug_sock, &read_set);

			//收取卸载完成的消息
			BOOL bRun = TRUE;
			while (bRun && select(hotplug_sock + 1, &read_set, NULL, NULL, &tv) > 0)
			{
				len = recv(hotplug_sock, buf, UEVENT_BUFFER_SIZE, MSG_PEEK);
				if (len > 3)
				{
					buf[len] = '\0';
					if (!strncmp(buf, "remove@", strlen("remove@")))
					{
						printf("hd_mng_thread partition: %s\n", buf);
						if (!strncmp(buf, "remove@/devices/virtual", strlen("remove@/devices/virtual")))
						{
							bRun = FALSE;
						}

						recv(hotplug_sock, buf, UEVENT_BUFFER_SIZE, 0);
					}
				}

				tv.tv_sec = 2;
				tv.tv_usec = 0;

				FD_ZERO(&read_set);
				FD_SET(hotplug_sock, &read_set);				
			}
			
			UmountDisk(info);
		}
     }

	close(hotplug_sock);
	return NULL;
}

static int GetDiskSpaceInfo(const char *path, int *pTotal, int *pFree)
{
	int nFree, nAll;
	struct statfs disk_statfs;

	if( statfs(path, &disk_statfs) >= 0 )
	{
		nAll = (((long long int)disk_statfs.f_bsize  * (long long int)disk_statfs.f_blocks)/(long long int)(1024*1024));
		nFree = (((long long int)disk_statfs.f_bsize  * (long long int)disk_statfs.f_bfree)/(long long int)(1024*1024));
//		printf("\nHD nAll : %dMB   nFree : %dMB\n", nAll, nFree);
		*pTotal = nAll;
		*pFree = nFree;

		return 0;
	}

	printf("%s  %d, path:%s, errno: %d, %s\n",__FUNCTION__, __LINE__, path, errno, strerror(errno));
	return -1;
}

static int GetDiskSpaceByDev(const char *path)
{
    unsigned long long v64;  
    unsigned long longsectors;  
    int fd;  
    fd = open(path, O_RDONLY);  
    if(fd == -1) 
   	{  
        return 0;  
    }  
    if (ioctl(fd, BLKGETSIZE64, &v64) == 0) 
    {  
    	/* Got bytes, convert to 512 byte sectors */
		close(fd);
		return (v64 >> 9)/(2*1024);  
    }  
    /* Needs temp of type long */  
    if (ioctl(fd, BLKGETSIZE, &longsectors))  
    {
		longsectors = 0;  
    }
    close(fd);
    return longsectors/(2*1024);  
}
/* HdSetDiskInfo
  设置磁盘信息
*/
int HdSetDiskInfo(int nDiskNo, int nStat)
{
	int i;
	//获取SD卡信息
	for(i=0;i<PARTITION_MAX;i++)
	{
		if(g_sdinfo[i].used && g_sdinfo[i].DiskNo==nDiskNo)
			g_sdinfo[i].Isformat=nStat;
	}
	//获取U盘信息
	for(i=0;i<PARTITION_MAX;i++)
	{
		if(g_usbinfo[i].used && g_usbinfo[i].DiskNo==nDiskNo)
			g_usbinfo[i].Isformat=nStat;
	}
	return 0;
}

int HdGetDiskInfo(QMAPI_NET_HDCFG *pHdInfo)
{
	int i, ret = 0;
//	int RecStatus = 0;
	int totalspace=0, freespace=0;
	QMAPI_NET_HDCFG hd_info;

	memset(&hd_info, 0, sizeof(QMAPI_NET_HDCFG));
	hd_info.dwSize = sizeof(QMAPI_NET_HDCFG);

	//获取SD卡信息
	for(i=0;i<PARTITION_MAX;i++)
	{
		if(g_sdinfo[i].used)
		{
			//printf("sd info IsMount:%d, IsFormat:%d\n", g_sdinfo[i].IsMount, g_sdinfo[i].Isformat);			
			if(g_sdinfo[i].IsMount)
			{
				ret = GetDiskSpaceInfo(g_sdinfo[i].MountPath, &totalspace, &freespace);
				if(ret)
				{
					hd_info.stHDInfo[hd_info.dwHDCount].dwCapacity = 0;
					hd_info.stHDInfo[hd_info.dwHDCount].dwFreeSpace = 0;
				}
				else
				{
					hd_info.stHDInfo[hd_info.dwHDCount].dwCapacity = totalspace;
					hd_info.stHDInfo[hd_info.dwHDCount].dwFreeSpace = freespace;
				}
			
				strcpy((char *)(hd_info.stHDInfo[hd_info.dwHDCount].byPath), g_sdinfo[i].MountPath);
				if(g_sdinfo[i].Isformat == 1)
				{
					hd_info.stHDInfo[hd_info.dwHDCount].dwHdStatus = 0;
				}
				else if(g_sdinfo[i].Isformat==0)
				{
					hd_info.stHDInfo[hd_info.dwHDCount].dwHdStatus = 1;
				}
				else if(g_sdinfo[i].Isformat == 2)
				{
					hd_info.stHDInfo[hd_info.dwHDCount].dwHdStatus = 1;
				}
				else if(g_sdinfo[i].Isformat == 3)
				{
					hd_info.stHDInfo[hd_info.dwHDCount].dwHdStatus = 2;//磁盘错误
				}
			}
			else
			{
				hd_info.stHDInfo[hd_info.dwHDCount].dwCapacity = GetDiskSpaceByDev(g_sdinfo[i].DevPath);
				hd_info.stHDInfo[hd_info.dwHDCount].dwFreeSpace = 0;
				
				if(g_sdinfo[i].Isformat == 2)
				{
					hd_info.stHDInfo[hd_info.dwHDCount].dwHdStatus = 6;//正在格式化
				}
				else if(g_sdinfo[i].Isformat == 4)
				{
					hd_info.stHDInfo[hd_info.dwHDCount].dwHdStatus = 3;//Mount过程中
				}
				else
				{
					hd_info.stHDInfo[hd_info.dwHDCount].dwHdStatus = 2;
				}
			}
			
			hd_info.stHDInfo[hd_info.dwHDCount].dwHDType = 0;
			if(gbSD_WriteProtect)
			{
				hd_info.stHDInfo[hd_info.dwHDCount].byHDAttr = 2;
			}
			else
			{
				hd_info.stHDInfo[hd_info.dwHDCount].byHDAttr = 0;
			}
			if(i>0 && g_usbinfo[i].DiskNo==-1)
				hd_info.stHDInfo[hd_info.dwHDCount].dwHDNo = g_sdinfo[i-1].DiskNo+1;
			else
				hd_info.stHDInfo[hd_info.dwHDCount].dwHDNo = g_sdinfo[i].DiskNo;
			hd_info.dwHDCount++;

			if(i == 0)
				break;
		}
		
	}

	//获取U盘信息
	for(i=0;i<PARTITION_MAX;i++)
	{
		if(g_usbinfo[i].used)
		{
			if(g_usbinfo[i].IsMount)
			{
				ret = GetDiskSpaceInfo(g_usbinfo[i].MountPath, &totalspace, &freespace);
				if(ret)
				{
					hd_info.stHDInfo[hd_info.dwHDCount].dwCapacity = 0;
					hd_info.stHDInfo[hd_info.dwHDCount].dwFreeSpace = 0;
				}
				else
				{
					hd_info.stHDInfo[hd_info.dwHDCount].dwCapacity = totalspace;
					hd_info.stHDInfo[hd_info.dwHDCount].dwFreeSpace = freespace;
				}
				strcpy((char *)(hd_info.stHDInfo[hd_info.dwHDCount].byPath), g_usbinfo[i].MountPath);

				if(g_usbinfo[i].Isformat == 1)
					hd_info.stHDInfo[hd_info.dwHDCount].dwHdStatus = 0;
				else if(g_usbinfo[i].Isformat==0)
					hd_info.stHDInfo[hd_info.dwHDCount].dwHdStatus = 1;
				else if(g_usbinfo[i].Isformat==2)
					hd_info.stHDInfo[hd_info.dwHDCount].dwHdStatus = 1;			
				else if(g_usbinfo[i].Isformat == 3)
					hd_info.stHDInfo[hd_info.dwHDCount].dwHdStatus = 2;//磁盘错误
			}
			else
			{
				hd_info.stHDInfo[hd_info.dwHDCount].dwCapacity = 0;
				hd_info.stHDInfo[hd_info.dwHDCount].dwFreeSpace = 0;
				
				if(g_usbinfo[i].Isformat == 2)
				{
					hd_info.stHDInfo[hd_info.dwHDCount].dwHdStatus = 6;//正在格式化
				}
				else if(g_usbinfo[i].Isformat == 4)
				{
					hd_info.stHDInfo[hd_info.dwHDCount].dwHdStatus = 3;//Mount过程中
				}
				else
				{
					hd_info.stHDInfo[hd_info.dwHDCount].dwHdStatus = 2;
				}
			}

			hd_info.stHDInfo[hd_info.dwHDCount].dwHDType = 1;
			hd_info.stHDInfo[hd_info.dwHDCount].byRes1[0] = g_usbversion;
			if(i>0 && g_usbinfo[i].DiskNo==-1)
				hd_info.stHDInfo[hd_info.dwHDCount].dwHDNo = g_usbinfo[i-1].DiskNo+1;
			else
				hd_info.stHDInfo[hd_info.dwHDCount].dwHDNo = g_usbinfo[i].DiskNo;
			hd_info.dwHDCount++;

			if(i == 0)
				break;
		}
	}

    memcpy(pHdInfo, &hd_info, sizeof(QMAPI_NET_HDCFG));

    return hd_info.dwHDCount ? 0 : -1;
}

int HdFormat(QMAPI_NET_DISK_FORMAT *pFormatInfo)
{
	int i;
	int ret = 0;

	if(module_deinit)
		return 0;

	if(!pFormatInfo || pFormatInfo->dwHDNo<0)
		return -1;

	if (pFormatInfo->dwHDNo < 8)
	{
		for(i=0;i<PARTITION_MAX;i++)
		{
			if(g_sdinfo[i].DiskNo == pFormatInfo->dwHDNo)
			{
				ret = disk_format(&g_sdinfo[i]);
				break;
			}

			if(g_usbinfo[i].DiskNo == pFormatInfo->dwHDNo)
			{
				printf("usb:%d DiskNo:%d,%lu[%s]\n", i, g_usbinfo[i].DiskNo, pFormatInfo->dwHDNo, g_usbinfo[i].DevPath);
				ret = disk_format(&g_usbinfo[i]);
				break;
			}
		}
	}

	return ret;
}

int HdGetFormatStatus(QMAPI_NET_DISK_FORMAT_STATUS *pStatus)
{
	int i, find=0;

	struct format_status *pFormatStatus = NULL;
	for(i=0;i<PARTITION_MAX; i++)
	{
		if(g_formatinfo[i].DiskNo >= 0)
		{
			find = 1;
			pFormatStatus = &g_formatinfo[i];
			pStatus->dwHDNo = g_formatinfo[i].DiskNo;
			if(g_formatinfo[i].status == 0)
			{
				pStatus->dwHdStatus = 0;
				pStatus->dwResult = 0;
				g_formatinfo[i].process = 5;
				pStatus->dwProcess = g_formatinfo[i].process;
			}
			else if(g_formatinfo[i].status == 1)
			{
				if(g_formatinfo[i].process<=98)
					g_formatinfo[i].process += 2;

				pStatus->dwProcess = g_formatinfo[i].process;
				pStatus->dwHdStatus = 1;
				pStatus->dwResult = 0;
			}
			else		//g_formatinfo[i].status == 2
			{
				if(pStatus->dwProcess<100)
					pStatus->dwHdStatus = 1;		//第一次还是继续显示为正在格式化，让客户端可显示成100%
				else
					pStatus->dwHdStatus = 2;
				pStatus->dwProcess = 100;
				
				pStatus->dwResult = g_formatinfo[i].result;
				memset(&g_formatinfo[i], 0, sizeof(struct format_status));
				g_formatinfo[i].DiskNo = -1;
			}

			break;
		}
	}

	if(!find)
	{
		for(i=0;i<PARTITION_MAX; i++)
		{
			if(g_sdinfo[i].DiskNo == pStatus->dwHDNo)
			{
				find = 1;
				pStatus->dwHDNo = g_sdinfo[i].DiskNo;
				if(gSdStatus&HD_MOUNTED)
					pStatus->dwResult = 0;
				else
					pStatus->dwResult = 1;

				break;
			}

			if(g_usbinfo[i].DiskNo == pStatus->dwHDNo)
			{
				find = 1;
				pStatus->dwHDNo = g_usbinfo[i].DiskNo;
				if(gUsbStatus&HD_MOUNTED)
					pStatus->dwResult = 0;
				else
					pStatus->dwResult = 1;

				break;
			}
				
		}
		
		if(find)
		{
			pStatus->dwHdStatus = 2;
			pStatus->dwProcess = 100;
		}
		else
			return -1;
	}
	else if(pFormatStatus && pFormatStatus->status == 2)
	{
		memset(pFormatStatus, 0, sizeof(struct format_status));
		pFormatStatus->DiskNo = -1;
	}

//	printf("%s  %d, aaaaaaaaa,diskno=%lu  sta:%lu   process=%lu\n",__FUNCTION__, __LINE__, pStatus->dwHDNo, pStatus->dwHdStatus,pStatus->dwProcess);

	return 0;
}

void HdUnloadAllDisks()
{
	struct disk_information info[PARTITION_MAX];
	
	if (StopDiskTask(DEVICE_TYPE_USB, info))
	{
		mSleep(3000);
		UmountDisk(info);
	}
	
	if (StopDiskTask(DEVICE_TYPE_SD, info))
	{
		mSleep(3000);
		UmountDisk(info);
	}
	
    //UmountAllNAS(); NAS不支持卸载
	return;
}
//重新检测Usb/SD设备，nFlag默认为0
void HdScanAllDisks(int nFlag)
{
	if(!module_deinit)
	{
		char sBasename[64];
		int bInsert;
		bInsert = IsUsbInserted(sBasename);
		if(bInsert && !(gUsbStatus & HD_MOUNTED))
			gUsbStatus=0x00;//重新检测并加载
		bInsert = IsSdInserted(sBasename);
		if(bInsert && !(gSdStatus & HD_MOUNTED))
			gSdStatus=0x00;//重新检测并加载
	}
}

#define CFG_RECORD_STORE_MOUNTPOINT         "/mnt/sd0"

static void sdcard_state_handler(char *event, void *arg)
{
    storage_info_t si;

    ipclog_debug("sdcard_state_handler(%s)\n", event);
    if (!strcmp(event, EVENT_MOUNT)) {
        int sd_insert = IsSdInserted();
        ipclog_info("sdcard mount sd_insert:%d gSdStatus:0x%x\n", sd_insert, gSdStatus);
        if (sd_insert && !(gSdStatus & HD_INSERTED))
        {
            system_get_storage_info(CFG_RECORD_STORE_MOUNTPOINT, &si);
            ipclog_info("recstore: total %dMB, free %dMB\n", si.total>>10, si.free>>10);
            if (si.total > 0) {
                //TODO: move in one function
                g_sdinfo[0].IsMount = 1; 
                g_sdinfo[0].Isformat = 1;
                g_sdinfo[0].partition = 1;
                g_sdinfo[0].type = DEVICE_TYPE_SD;

                strcpy(g_sdinfo[0].DevPath, "/dev/mmcblk0p1");
                strcpy(g_sdinfo[0].MountPath, "/mnt/sd0");			
                g_sdinfo[0].DiskNo = 0;
                g_sdinfo[0].used = 1;
                
                gSdStatus |= HD_INSERT_AND_MOUNT;
                
                if(g_eventcallback)
                    g_eventcallback(HD_EVENT_MOUNT, g_sdinfo[0].DiskNo, NULL);

                DiskEventNotify(HD_EVENT_MOUNT, g_sdinfo[0].DiskNo, NULL);
                
                QMAPI_AudioPromptPlayFile("/root/qm_app/res/sound/mount.wav", 0);
            }
        }
    } else if (!strcmp(event, EVENT_UNMOUNT)) {
        int sd_insert = IsSdInserted();
        ipclog_info("sdcard unmount sd_insert:%d gSdStatus:0x%x\n", sd_insert, gSdStatus);
        if ((gSdStatus & HD_INSERTED))
        {
            //TODO: move in one function
            g_sdinfo[0].used = 0;
            g_sdinfo[0].IsMount = 0; 
            g_sdinfo[0].Isformat = 0;
            g_sdinfo[0].partition = 0;
            g_sdinfo[0].type = DEVICE_TYPE_SD;
            g_sdinfo[0].DiskNo = -1;

            gSdStatus = 0;
            
            if (g_eventcallback)
                g_eventcallback(HD_EVENT_UNMOUNT, g_sdinfo[0].DiskNo, NULL);

            DiskEventNotify(HD_EVENT_UNMOUNT, g_sdinfo[0].DiskNo, NULL);
        }
    }
}

int QMAPI_Hdmng_Init(HDmngCallback fHdmng, HDmng_GetSdWriteProtectFun fWriteProtect)
{
    int ret = 0;
    int i = 0;

    if(!fHdmng)
        return -1;

    module_deinit = 0;
	memset(&g_sdinfo, 0, sizeof(struct disk_information)*PARTITION_MAX);
	memset(&g_usbinfo, 0, sizeof(struct disk_information)*PARTITION_MAX);
	memset(&g_formatinfo, 0, sizeof(struct format_status)*PARTITION_MAX);
	for(i=0;i<PARTITION_MAX;i++)
	{
		g_sdinfo[i].DiskNo = -1;
		g_usbinfo[i].DiskNo = -1;
		g_formatinfo[i].DiskNo = -1;
	}

	gSdStatus = 0;
	gUsbStatus = 0;
	g_eventcallback = fHdmng;
	g_fWriteProtect = fWriteProtect;

    event_register_handler(EVENT_MOUNT, 0, sdcard_state_handler);
    event_register_handler(EVENT_UNMOUNT, 0, sdcard_state_handler);

#if 0
    pthread_t hdmng_id;
    ret = pthread_create(&hdmng_id, NULL, (void *)&hd_mng_thread, NULL);
    if(ret)
    {
		printf("%s  %d, create hdmng thread fail!ret=%d,%s(errno:%d)\n", __FUNCTION__, __LINE__, 
			ret, strerror(errno), errno);
	}
#endif
	
	printf("Hd_mng Module Build:%s %s\n", __DATE__, __TIME__);
	return ret;
}


int QMAPI_Hdmng_DeInit(void)
{
	module_deinit = 1;

	//HdUnloadAllDisks();

    event_unregister_handler(EVENT_MOUNT, sdcard_state_handler);
    event_unregister_handler(EVENT_UNMOUNT, sdcard_state_handler);

	g_eventcallback = NULL;
	return 0;
}

void mSleep(unsigned int mSecond)
{
	struct timeval tv;
	tv.tv_sec = mSecond / 1000;
	tv.tv_usec = (mSecond % 1000) * 1000;

	select(0, NULL, NULL, NULL, &tv);
}


