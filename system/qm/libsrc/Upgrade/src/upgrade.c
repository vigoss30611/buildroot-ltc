
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>


#include "QMAPIUpgrade.h"
#include "system_msg.h"
#include "AudioPrompt.h"

#include <qsdk/upgrade.h>
#include <qsdk/sys.h>

static unsigned short updataflag = 0;

/*升级结构幻数*/
#define UPGRADE_MEMORYMAP_MAGIC 0xaa5555aa
#define UMM_STRUCT_VERSION      0x00000001

/*升级位图*/
#define UB_BOOT_BIT             (0x1<<0)
#define UB_KERNEL_BIT           (0x1<<1)
#define UB_ROOTFS_BIT           (0x1<<2)
#define UB_USERDATA_BIT           (0x1<<3)
#define UB_SETTING_BIT           (0x1<<4)
#define UB_APP_BIT              (0x1<<5)
#define UB_WEB_BIT     (0x1<<6)
#define UB_LOGO_BIT             (0x1<<7)
#define UB_FACTORY_PARAM_BIT           (0x1<<8)

#define TL_KERNEL_UPDATE_DEV			"/dev/mtd1"
#define TL_UPDATE_DEV				"/dev/mtd2"
#define TL_ERASE_BLOCK_SIZE 		64*1024
#define TL_KERNEL_BLOCK_SIZE				 (1024*1024*3)
#define TL_ROOT_BLOCK_SIZE					 (1024*11776)//11.5M
#define TL_APP_BLOCK_SIZE				  (1024*4608)	//4.5M

#define TL_WEB_BLOCK_SIZE                 (1024*1024*2)
#define TL_LOGO_BLOCK_SIZE                 (1024*30)


/*升级数据在内存中的布局*/
typedef struct _upgradememorymap_{
	unsigned long magic;/*UPGRADE_MEMORYMAP_MAGIC*/
	unsigned long crc;/*CRC是对从version开始的所有数据的CRC校验值*/
	unsigned long CheckCrc;/*~crc，是对CRC校验的判断*/
	unsigned long version;/*结构体的版本*/
	unsigned long UpgradeBitmap;/*升级位图，注只有需要升级的数据才会拷贝至内存*/
	unsigned long BootAddr;/*BOOT地址，内存地址，而非偏移量*/
	unsigned long BootLen;/*BOOT的长度*/
	unsigned long KernelAddr;/*内核的地址，内存地址，而非偏移量*/
	unsigned long KernelLen;/*内核的长度*/
	unsigned long RootfsAddr;/*文件系统的地址，内存地址，而非偏移量*/
	unsigned long RootfsLen;/*文件系统的长度*/
	unsigned long UserDataAddr;/*用户数据的地址，内存地址，而非偏移量*/
	unsigned long UserDataLen;/*用户数据的长度*/
	unsigned long SettingAddr;/*系统参数的地址，内存地址，而非偏移量*/
	unsigned long SettingLen;/*系统参数的长度*/
	unsigned long AppAddr;/*应用程序的地址，内存地址，而非偏移量*/
	unsigned long AppLen;/*应用程序的长度*/
	unsigned long WebAddr;    /*web的地址，内存地址，而非偏移量*/
	unsigned long WebLen;     /*Web的长度*/
	unsigned long LogoAddr;         /*logo 内存地址，而非偏移量*/
	unsigned long LogoLen;          /*logo 长度*/
	unsigned long FactoryParamAddr;         /*默认参数的内存地址，而非偏移量*/
	unsigned long FactoryParamLen;          /*默认参数的长度*/

	/*升级数据*/
}upgradememorymap_t;

typedef struct NVUPDATE_FILE_HEADER{
	DWORD	dwCRC;
	char	csFileName[64];
	DWORD	dwFileLen;
	DWORD	dwVer;
}UPDATE_FILE_HEADER;

typedef struct _UPDATE_FILE_INFO
{
	ULONG	nFileLength;		
	ULONG	nFileOffset;		
	ULONG	nPackNum;		
	ULONG	nPackNo;		
	ULONG	nPackSize;		
	UPDATE_FILE_HEADER ufh;
	BOOL	bEndFile;			
	char	reserved[180];
}UPDATE_FILE_INFO;

///Updata Flash 
typedef struct {
	WORD					bUpgradeSuport; /* 支持升级，1支持，0不支持*/
    WORD					bUpdatestatus;			//升级状态0 普通状态1 开始升级2 升级中3升级完成
    QMAPI_NET_UPGRADE_REQ   stUpgradeFileInfo;
    UPGRADE_ITEM_HEADER		stItemHeader;
    int    					nFileOffset;
    int    					nPackNo;	
    char   					csFileName[128];
    FILE 					*fp;
    DWORD     				dwDataCRC;
    int             		nResult;
	char 					progess;   // 百分比0 - 100%

    UPDATE_FILE_INFO 		update_header;
    char 					*update_buffer;

	CBUpgradeProc           UpgradeProc; /* 升级进度回调函数 */
	CBUpgradeExit           UpgradeExit; /* 升级时内存释放 */
}UPGRADE_FLASH_KERNEL;

UPGRADE_FLASH_KERNEL g_tl_updateflash;

static int SaveUpgradeAddress(unsigned int upgrade_addr)
{
	int mtdfd;

	mtdfd = open("/dev/mtdblock4", O_RDWR);
	if(mtdfd<0)
	{
		printf("%s  %d, open /dev/mtdblock4 error!err:%s(%d)\n",__FUNCTION__, __LINE__,strerror(errno),errno);
		return -1;
	}

	int res;
	res = write(mtdfd, &upgrade_addr, sizeof(unsigned int));
	if(res<0)
		printf("%s  %d, write upgrade address error!err:%s(%d),res=%d\n",__FUNCTION__, __LINE__,strerror(errno),errno,res);

	fsync(mtdfd);
	close(mtdfd);
	
	return 0;
}

static void upgrade_state_callback_Q3(void *arg, int image_num, int state, int state_arg)
{
	UPGRADE_FLASH_KERNEL *pupgrade = &g_tl_updateflash;
	switch (state)
	{
		case UPGRADE_START:
		{
			pupgrade->progess = 0;
			printf("q3 upgrade start \n");
			break;
		}
		case UPGRADE_COMPLETE:
        {
			pupgrade->nResult       = 0;
            pupgrade->bUpdatestatus =  3;
            pupgrade->progess       = 100;
			if (system_update_check_flag() == UPGRADE_OTA)
			{
				system_update_clear_flag(UPGRADE_FLAGS_OTA);
			}
	        printf("\n q3 upgrade sucess\n");
			break;
		}
		case UPGRADE_WRITE_STEP:
		{
			printf("\r q3 %s image -- %d %% complete", image_name[image_num], state_arg);
			fflush(stdout);
			pupgrade->progess = state_arg;
			break;
		}
		case UPGRADE_VERIFY_STEP:
		{
			if (state_arg)
				printf("\n q3 %s verify failed %x \n", image_name[image_num], state_arg);
			else
				printf("\n q3 %s verify success\n", image_name[image_num]);
			break;
		}
		case UPGRADE_FAILED:
		{
			pupgrade->nResult       = -1;
            pupgrade->bUpdatestatus =  3;
            pupgrade->progess       = 100;
			printf("q3 upgrade %s failed %d \n", image_name[image_num], state_arg);
			break;
		}
	}

	return;
}

static void upgrade_state_callback_Q3_ext(void *arg, int image_num, int state, int state_arg)
{
	UPGRADE_FLASH_KERNEL *pupgrade = &g_tl_updateflash;
	struct upgrade_t *upgrade = (struct upgrade_t *)arg;
	int cost_time;

	switch (state) {
		case UPGRADE_START:
			//system_set_led("red", 1000, 800, 1);
			pupgrade->progess = 0;
			gettimeofday(&upgrade->start_time, NULL);
			printf("q3 upgrade start \n");
			if (pupgrade->UpgradeProc)
			{
				pupgrade->UpgradeProc(0, 0);
			}
			break;
		case UPGRADE_COMPLETE:
            pupgrade->nResult       = 0;
            pupgrade->bUpdatestatus =  3;
            pupgrade->progess       = 100;
			if(pupgrade)
			{
				pupgrade->UpgradeProc(1, 100);
			}
			gettimeofday(&upgrade->end_time, NULL);
			cost_time = (upgrade->end_time.tv_sec - upgrade->start_time.tv_sec);
			//system_set_led("red", 1, 0, 0);
			if (system_update_check_flag() == UPGRADE_OTA)
			{
				system_update_clear_flag(UPGRADE_FLAGS_OTA);
			}
	        printf("\n q3 upgrade sucess time is %ds \n", cost_time);
            QMapi_sys_ioctrl(0, QMAPI_NET_STA_REBOOT, 0, NULL, 0);
			break;
		case UPGRADE_WRITE_STEP:
			printf("\r q3 %s image -- %d %% complete", image_name[image_num], state_arg);
			if(pupgrade)
			{
				pupgrade->progess = state_arg;
				pupgrade->UpgradeProc(0, state_arg);
			}
			fflush(stdout);
			break;
		case UPGRADE_VERIFY_STEP:
			if (state_arg)
				printf("\n q3 %s verify failed %x \n", image_name[image_num], state_arg);
			else
				printf("\n q3 %s verify success\n", image_name[image_num]);
			break;
		case UPGRADE_FAILED:
			//system_set_led("red", 300, 200, 1);
			pupgrade->nResult       = -1;
            pupgrade->bUpdatestatus =  3;
            pupgrade->progess       = 100;
			if (pupgrade)
			{
				pupgrade->UpgradeProc(-1, 100);
			}
			printf("q3 upgrade %s failed %d \n", image_name[image_num], state_arg);
			break;
	}

	return;
}

unsigned long Upgrade_GetDataCRC(DWORD dwLastCRC, Byte *lpData, int dwSize)
{
    int i = 0;
    Byte *buffer = NULL;
    int len = 0;
    unsigned long m_crc = dwLastCRC;
    unsigned long crc32_table[256];
    unsigned long ulPolynomial = 0x04C11DB7;

    for (i=0; i<=0xFF; i++)
    {
        unsigned long value = 0;
        int ref = i;
        int n = 0;
        int j = 0;
        
        for (n=1; n<(8+1); n++)
        {
            if (ref & 1)
            {
                value |= 1 << (8 - n);
            }
            ref >>= 1;
        }
        crc32_table[i]= value << 24;
        for (j=0; j<8; j++)
        {
            crc32_table[i] = (crc32_table[i] << 1) ^ (crc32_table[i] & (1 << 31) ? ulPolynomial : 0);
        }
        value = 0;
        ref = crc32_table[i];
        for (n=1; n<(32+1); n++)
        {
            if (ref & 1)
            {
                value |= 1 << (32 - n);
            }
            ref >>= 1;
        }
        crc32_table[i] = value;
    }
    
    //caculate the crc check sum
   // m_crc = -1;
    buffer = (Byte *)lpData;
    len = dwSize;
    while (len--)
    {
        m_crc = (m_crc >> 8) ^ crc32_table[(m_crc & 0xFF) ^ *buffer++];
    }
    
    return m_crc;//^ 0xffffffff;
}

//size KB
static int checkwebsize(int size)
{
    FILE *fp;
    char buf[128] = {0};
    struct stat statbuff;
    float f;
    int nRes = 0,oldwebsize=0;
    //int totalspace, usespace, freespace;
    int totalspace, usespace;
    char strFspath[16] = {0},strTotal[16] = {0},strUse[16] = {0},strFree[16] = {0},strUsePercent[16] = {0},strMountPath[16] = {0};

    if( stat("/usr/netview/web.tgz", &statbuff) >= 0 )
    {
        oldwebsize = (int)statbuff.st_size/1024;
    }

    SystemCall_msg("df | grep /root > /tmp/spaceinfo", SYSTEM_MSG_BLOCK_RUN);
    fp = fopen("/tmp/spaceinfo", "r");
    fgets(buf, 128, fp);
    sscanf(buf, "%s %s %s %s %s %s", strFspath, strTotal, strUse, strFree, strUsePercent, strMountPath);
    totalspace = atoi(strTotal);
    usespace = atoi(strUse);
    //freespace = atoi(strFree);
    fclose(fp);

    f = (float)(usespace-oldwebsize+size)/(float)totalspace;
    if(f>0.96)
        nRes = QMAPI_ERR_UPGRADE_INVALID_FILE;
//printf("%s  %d, aaaaaaaaa,%d,%d,%d,%f\n",__FUNCTION__, __LINE__,totalspace,usespace,oldwebsize,f);
    return nRes;
}

int QMAPI_UpgradeFlashReq(int userId, QMAPI_NET_UPGRADE_REQ *lpUpgradeReq)
{
    int nRes = 0;
    UPGRADE_FLASH_KERNEL *upgrade = &g_tl_updateflash;
    UPGRADE_ITEM_HEADER  *lpItemHeader = (UPGRADE_ITEM_HEADER* )(&lpUpgradeReq->stItemHdr[0]);
    int nFileLength = lpItemHeader->dwDataLen;
	printf("#########  size %ld len %ld type %ld crc %ld \n" , lpItemHeader->dwSize , lpItemHeader->dwDataLen , lpItemHeader->dwDataType , lpItemHeader->dwCRC);
    do
    {
    	if (!upgrade->bUpgradeSuport)
    	{
    		nRes = QMAPI_ERR_UPGRADE_NO_SUPORT;
			break;
		}
        if (upgrade->bUpdatestatus || updataflag)
        {
            nRes = 	QMAPI_ERR_UPGRADING;
            break;
        }
        if (lpUpgradeReq->FileHdr.dwSize != sizeof(QMAPI_UPGRADE_FILE_HEADER))
        {
            nRes = QMAPI_ERR_UPGRADE_UNSUPPORT;
            break;
        }
        if (lpUpgradeReq->FileHdr.dwFlag != QMAPI_UPGRADE_FILE_FLAG2 
				&& lpUpgradeReq->FileHdr.dwFlag != QMAPI_UPGRADE_FILE_FLAG
				&& lpUpgradeReq->FileHdr.dwFlag != QMAPI_UPGRADE_FILE_FLAG3 )
        {
            nRes = QMAPI_ERR_UPGRADE_UNSUPPORT;
            break;
        }
		// need fixme yi.zhang
       // if(lpUpgradeReq->FileHdr.dwCPUType != g_tl_globel_param.TL_Server_Info.dwServerCPUType)
        {
      //      nRes = QMAPI_ERR_UPGRADE_CPUTYPE;
      //      break;
        }
        if(lpUpgradeReq->FileHdr.dwChannel != 1 )
        {
            nRes = QMAPI_ERR_UPGRADE_CHANNELNUM;
            break;
        }
        switch(lpItemHeader->dwDataType)
        {
            case QMAPI_ROM_FILETYPE_KERNEL:
            {
                if(nFileLength <= 0 || nFileLength >= TL_KERNEL_BLOCK_SIZE)
                {
                    nRes = QMAPI_ERR_UPGRADE_INVALID_FILE;
                    break;
                }
                updataflag |= UB_KERNEL_BIT;
            }
            break;
            case QMAPI_ROM_FILETYPE_ROOTFS:
            {
                if(nFileLength <= 0 || nFileLength >= TL_ROOT_BLOCK_SIZE)
                {
                    nRes = QMAPI_ERR_UPGRADE_INVALID_FILE;
                    break;
                }
                updataflag |= UB_ROOTFS_BIT;
            }
            break;
            case QMAPI_ROM_FILETYPE_APP:
            {
                if(nFileLength <= 0 || nFileLength >= TL_APP_BLOCK_SIZE)
                {
                    nRes = QMAPI_ERR_UPGRADE_INVALID_FILE;
                    break;
                }
                updataflag |= UB_APP_BIT;
            }
            break;
            case QMAPI_ROM_FILETYPE_WEB:
            {
                if(nFileLength <= 0 || nFileLength >= TL_WEB_BLOCK_SIZE)
                {
                    nRes = QMAPI_ERR_UPGRADE_INVALID_FILE;
                    break;
                }

                nRes = checkwebsize(nFileLength/1024);

                updataflag |= UB_WEB_BIT;
                
                break;
            }
            case QMAPI_ROM_FILETYPE_LOGO:
            {
                if(nFileLength <= 0 || nFileLength >= TL_LOGO_BLOCK_SIZE)
                {
                    nRes = QMAPI_ERR_UPGRADE_INVALID_FILE;
                }
                else
                {
                    updataflag |= UB_LOGO_BIT;
                }
                break;
            }
            default:
            {
                nRes = QMAPI_ERR_UPGRADE_INVALID_FILE;
            }
            break;
        }
        if(nRes != 0)
        {
            break;
        }
        sprintf(upgrade->csFileName, "/tmp/upgrade.ius");
        upgrade->fp = fopen(upgrade->csFileName,  "wb");
        if (upgrade->fp == NULL)
        {
            nRes = QMAPI_ERR_UPGRADE_OPEN;
            break;
        }
    }while(0);
    if (nRes != 0)
    {
        printf("%s(%d) Failed:0x%08X\n", __FUNCTION__, __LINE__, nRes);
        updataflag = 0;
        return nRes;
    }

    if((updataflag&UB_KERNEL_BIT) ||(updataflag&UB_ROOTFS_BIT) ||(updataflag&UB_BOOT_BIT))
    {
        if (upgrade->UpgradeExit)
		{
			upgrade->UpgradeExit();
        }
		upgrade->bUpdatestatus =TRUE;
    }
    upgrade->nPackNo     = 1;
    upgrade->nFileOffset = 0;
    upgrade->dwDataCRC   = -1;

    memcpy(&upgrade->stUpgradeFileInfo , lpUpgradeReq , sizeof(QMAPI_NET_UPGRADE_REQ));
    memcpy(&upgrade->stItemHeader , lpItemHeader, sizeof(UPGRADE_ITEM_HEADER));

    printf("Update %s Request Ret:%s, Ver:0x%x, request memory [%d]\n" , 
        lpUpgradeReq->FileHdr.csDescrip,  (upgrade->bUpdatestatus)?"SUCCESS":"FAILED", (int)lpUpgradeReq->FileHdr.dwVersion, nFileLength);

    return QMAPI_SUCCESS;
}


int QMAPI_UpgradeFlashData(int userId, QMAPI_NET_UPGRADE_DATA *lpUpgradeData)
{
	DWORD dwFileCRC = 0;
	int nRes = 0, ret;
	int nFileLength = 0;
	UPGRADE_FLASH_KERNEL *upgrade = &g_tl_updateflash;
	
	do
	{
		if (!upgrade->bUpgradeSuport)
    	{
    		nRes = QMAPI_ERR_UPGRADE_NO_SUPORT;
			break;
		}
		if (upgrade->bUpdatestatus == FALSE &&  updataflag == 0)
		{
			printf("%s(%d) bUpdatestatus == FALSE\n", __FUNCTION__, __LINE__);
			nRes = QMAPI_ERR_UPGRADE_WAITING;
			break;
		}

		if(lpUpgradeData->dwPackSize <= 0)
		{
			printf("%s(%d) dwPackSize(%d) <= 0\n", __FUNCTION__, __LINE__, (int)lpUpgradeData->dwPackSize);
			nRes = QMAPI_ERR_UPGRADE_NO_ITEM;
			break;
		}

		if (upgrade->nPackNo != lpUpgradeData->dwPackNo )
		{
			printf("%s(%d) nPackNo:%d, now dwPackNo:%d\n", __FUNCTION__, __LINE__, upgrade->nPackNo, (int)lpUpgradeData->dwPackNo);
			nRes = QMAPI_ERR_UPGRADE_DATA;
			break;
		}

		if(lpUpgradeData->pData == NULL)
		{
			printf("%s  %d, upgrade data is NULL!\n",__FUNCTION__, __LINE__);
			nRes = QMAPI_ERR_UPGRADE_DATA;
			break;
		}
	}while(0);
	if( nRes != 0)
	{
		printf("%s nRes:%d\n",__FUNCTION__,nRes);
		goto ErrDispose;
		
	}
	// printf("111111111111111111 line %d %ld %ld %ld %ld %d\n" , __LINE__ , lpUpgradeData->dwSize , lpUpgradeData->dwFileLength , 
	// 	lpUpgradeData->dwPackNo , lpUpgradeData->dwPackSize , lpUpgradeData->bEndFile);
	// printf("@@@@@@@@offset %d nfilelength %d packesize %ld start %p , dataaddr %p\n" , g_tl_updateflash.nFileOffset , nFileLength ,lpUpgradeData->dwPackSize 
	// 	, lpUpgradeData , lpUpgradeData->pData );
	///Copy Update File to buffer
	nRes = fwrite(lpUpgradeData->pData , 1, lpUpgradeData->dwPackSize, upgrade->fp);
	upgrade->nFileOffset += lpUpgradeData->dwPackSize;
	upgrade->nPackNo ++;
	dwFileCRC = upgrade->dwDataCRC;
	upgrade->dwDataCRC = Upgrade_GetDataCRC(dwFileCRC, (unsigned char*)lpUpgradeData->pData, lpUpgradeData->dwPackSize);
    nFileLength = upgrade->stItemHeader.dwDataLen;
	printf("@@@@@@@@offset %d nfilelength %d packesize %ld\n" , upgrade->nFileOffset , nFileLength ,lpUpgradeData->dwPackSize );
    if (upgrade->nFileOffset < nFileLength)
    {
        return 0;
    }
	
	if (upgrade->nFileOffset != nFileLength)
	{
		nRes = QMAPI_ERR_UPGRADE_DATA;
		printf("%s(%d) nFileOffset(%d) != nFileLength(%d)\n", __FUNCTION__, __LINE__,upgrade->nFileOffset,  nFileLength);		
		goto ErrDispose;
	}
	
	upgrade->dwDataCRC ^= 0xffffffff;
	if (upgrade->dwDataCRC != upgrade->stItemHeader.dwCRC)
	{	
		nRes = QMAPI_ERR_UPGRADE_CRC_FAILED;
		printf("%s(%d) dwDataCRC(%lu) != dwFileCRC(%lu)\n", __FUNCTION__, __LINE__, upgrade->dwDataCRC, upgrade->stItemHeader.dwCRC);			
		goto ErrDispose;
	}

	if (upgrade->fp != NULL)
	{
		fclose(upgrade->fp);
		upgrade->fp = NULL;
	}
	upgrade->bUpdatestatus = 2;
	if (upgrade->UpgradeExit)
	{
		//upgrade->UpgradeExit();
	}
    usleep(10);
    printf("system_update_upgrade start.............,\n");
#if 0
    ret = system_update_upgrade(upgrade->csFileName, upgrade_state_callback_Q3, (void *)&userId);
    if (ret)
    {
        printf("%s(%d) system_update_upgrade failed!!ret:%d\n", __FUNCTION__, __LINE__, ret);
        goto ErrDispose;
    }
#else
	char cmdbuf[128] = {0};
	sprintf(cmdbuf, "upgrade_file:%s", upgrade->csFileName);
    SystemCall_msg(cmdbuf, SYSTEM_MSG_BLOCK_RUN);
	printf("%s  %d, Main process exit!!!!!!!!!\n",__FUNCTION__, __LINE__);
	exit(0);
#endif

	return 0;
	
ErrDispose:
	printf("%s(%d) Failed nRes:0x%08X\n", __FUNCTION__, __LINE__, nRes);
    
	if (upgrade->fp != NULL)
	{
		fclose(upgrade->fp);
		upgrade->fp = NULL;
	}
	upgrade->bUpdatestatus = FALSE;
	if((updataflag&UB_KERNEL_BIT) ||(updataflag&UB_ROOTFS_BIT) ||(updataflag&UB_BOOT_BIT))
	{
		QMapi_sys_ioctrl(0, QMAPI_NET_STA_REBOOT, 0, NULL, 0);
	}
	updataflag = 0;
	return nRes;
}

int  QMAPI_UpgradeFlashGetState(QMAPI_NET_UPGRADE_RESP * pPara)
{
	pPara->dwSize    = sizeof(QMAPI_NET_UPGRADE_RESP);
	pPara->nResult   = g_tl_updateflash.nResult ;
	pPara->state     = g_tl_updateflash.bUpdatestatus ;
	pPara->progress  = g_tl_updateflash.progess;

	return pPara->nResult ;
}

int  QMAPI_PrepareUpgrade(CBUpgradeProc pFunc)
{
	UPGRADE_FLASH_KERNEL *upgrade = &g_tl_updateflash;
	
	if (!upgrade->bUpgradeSuport)
	{
		printf("%s  %d, error,Suport upgrade!!!!!!!!!!!!\n",__FUNCTION__, __LINE__);
		return -1;
	}
	if(upgrade->bUpdatestatus)
    {
        printf("%s  %d, error,repeat upgrade!!!!!!!!!!!!\n",__FUNCTION__, __LINE__);
        return -1;
    }

    upgrade->bUpdatestatus = 1;
	if (upgrade->UpgradeExit)
	{
		upgrade->UpgradeExit();
	}
	
    upgrade->UpgradeProc = pFunc;

    return 0;
}

void upgrade_thread(void *arg)
{
    if(!arg)
        return;
    char *pFile = (char *)arg;
    int ret = 0;
    printf("system start upgrade.............\n");
    
    //QMAPI_AudioPromptPlay(AUDIO_PROMPT_UPGRADE_ING, 1);

    printf("%s  %d, play audio notify end!!\n",__FUNCTION__, __LINE__);
    ret = system_update_upgrade(pFile, upgrade_state_callback_Q3_ext, NULL);
    if(ret)
    {
        printf("%s(%d) system_update_upgrade failed!!ret:%d\n", __FUNCTION__, __LINE__, ret);
    }
	printf("system upgrade end.............\n");

    return;
}

int  QMAPI_StartUpgrade(char *pFilePath)
{
	UPGRADE_FLASH_KERNEL *upgrade = &g_tl_updateflash;
	
	if (!upgrade->bUpgradeSuport)
	{
		printf("%s  %d, error,no bUpgradeSuport !!!!!!!!!!!!\n",__FUNCTION__, __LINE__);
		return -1;
	}
	if (!upgrade->bUpdatestatus)
    {
        printf("%s  %d, error,need QMAPI_PrepareUpgrade first!!!!!!!!!!!!\n",__FUNCTION__, __LINE__);
        return -1;
    }

    if(!pFilePath)
        return -1;

    upgrade->bUpdatestatus = 2;

    pthread_t tid;
    pthread_create(&tid, NULL, (void *)&upgrade_thread, (void *)pFilePath);

    return 0;
}

int QMAPI_Upgrade_Init(CBUpgradeExit pFunc)
{
	memset(&g_tl_updateflash, 0, sizeof(UPGRADE_FLASH_KERNEL));
	g_tl_updateflash.bUpgradeSuport = 1;
	g_tl_updateflash.UpgradeExit    = pFunc;

	return 0;
}



