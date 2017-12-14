

#include <sys/prctl.h>
#include <stdlib.h>
#include <sys/vfs.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <mntent.h>
#include <sys/stat.h>
//#include <sys/mount.h>
#define SIG_START
/* player for push button */
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <errno.h>
#include <pthread.h>

#include "QMAPIType.h"
#include "QMAPICommon.h"
#include "QMAPINetSdk.h"
#include "QMAPI.h"
#include "QMAPIRecord.h"
#include "librecord.h"
#include "hd_mng.h"
#include "system_msg.h"
#include "RecType.h"
//#include "SnapMng.h"
#include "ipc.h"

#define LOGTAG "DISK_MNG"

#define RemoveFile remove

typedef struct {
    pthread_t tid;
    volatile  int run;

    int ioctrl_handle;
} DISK_MANAGER_T; 

static DISK_MANAGER_T sDiskManager = {
    .tid = 0,
    .run = 0,
    .ioctrl_handle = -1,
};

extern BOOL IsRecordingFile(const char *sFileName, int nChannel, int nStorType);
extern BOOL IsLocalHdRecording(int nStorType);
extern void WriteIOErrorLog(int nCount, int nLast);
extern int ReadIOErrorLog(int *nLast);

//extern NFS_MngConfig gs_Rec_NFSConfig;

static const char* mybasename(const char* filename);
static int DeleteIndex(const char *lpDir);
static int getTimeFromPath(const char *fileName);
static BOOL  isInOneHour(int iOldest,int iCurTime);
static int delete_nulldir(const char *dir_name);

#define IsSysDir(s) ((s[0]=='.' && s[1]=='\0')||(s[0]=='.' && s[1]=='.' && s[2]=='\0'))

void record_sleep(unsigned int mSecond)
{
	struct timeval tv;
	tv.tv_sec = mSecond / 1000;
	tv.tv_usec = (mSecond % 1000) * 1000;

	select(0, NULL, NULL, NULL, &tv);
}

int DeleteIndex(const char *lpDir)
{
    char csBuf[64];
    int nstrlen;
    int nret;
    
    nstrlen = strlen(lpDir);
    if (nstrlen == 0)
        return 0;
    if(lpDir[nstrlen - 1] == '/')
        sprintf(csBuf,"%sindex.dat",lpDir);
    else
        sprintf(csBuf,"%s/index.dat",lpDir);
	if (access(csBuf, F_OK) == 0)
	{
		ipclog_info("DeleteIndex : %s.\n",csBuf);
    	nret = RemoveFile(csBuf);
	}
	
    return nret;
}
int DeletePictureDir(const char *lpDir)
{
    char csBuf[64];
    int nstrlen;
    int nret;

    nstrlen = strlen(lpDir);
    if (nstrlen == 0)
        return 0;
    if(lpDir[nstrlen - 1] == '/')
        sprintf(csBuf,"%spicture/",lpDir);
    else
        sprintf(csBuf,"%s/picture/",lpDir);
	if (access(csBuf, F_OK) == 0)
	{
        DIR *dir = NULL;
        struct stat buf;
        struct dirent s_dir;
        struct dirent* ptr = NULL;

		ipclog_info("DeletePictureDir : %s.\n",csBuf);
        dir = opendir(csBuf);
        if(dir == NULL)
        {
            nret = RemoveFile(csBuf);
            return nret;
        }

        int nCount=0;
        char fullPath[64] = {0};
        while(!readdir_r(dir, &s_dir, &ptr) && ptr)
        {
            if(IsSysDir(ptr->d_name)) continue;
            sprintf(fullPath, "%s%s", csBuf, ptr->d_name);
            nret = RemoveFile(fullPath);
            if(nret)
            {
                ipclog_info("Delete file[%s] return %d errno=%d %s\n", fullPath, nret, errno, strerror(errno));
            }
            else
            {
                nCount++;
            }
            int nErr=errno;
            if(nret && nErr != 2)
            {
                if(nErr==30) return -2;
            }
        }

        closedir(dir);
        remove(csBuf);
	}

    return nret;
}
int getTimeFromPath(const char *fileName)
{
    const char *baseName = mybasename(fileName);
    char nameBuf[64] = {0};
    strcpy(nameBuf,baseName);
    nameBuf[6] = 0;
    int iTime = atoi(nameBuf);
    if(nameBuf[0]>='0' && nameBuf[0]<='9')	//数值开头
		return iTime;
	else
		return -1;
}
BOOL  isInOneHour(int iOldest,int iCurTime)
{
    if(iOldest > iCurTime)
    {
        return TRUE;
    }
    
    int iHourOld =  iOldest / 10000;
    int iHourCur =  iCurTime /10000;
    
    iOldest = iOldest % 10000;
    iCurTime = iCurTime % 10000;
    
    int iMinOld = iOldest / 100;
    int iMinCur  = iCurTime / 100;
    
    int iSecOld = iOldest % 100;
    int iSecCur = iCurTime % 100;
    
    
    
    int iDur = (iHourCur - iHourOld) * 3600;
    
    iDur += (  (iMinCur - iMinOld) * 60);
    
    iDur += ( iSecCur - iSecOld);
    
    
    return (iDur <= 3600);
    
}

const char* mybasename(const char* filename)
{
    const char* szTmp = strrchr(filename,'/');
    if(szTmp)
    {
        return (szTmp + 1);
    }
    else
    {
        return filename;
    }
}
/* RemoveHourFile
    返回值:正常删除文件个数,-1:出错, -2:系统出错(只读)
*/
static int RemoveHourFile(int channel,const char *dirPath,const char *fileName, BOOL isLocal)
{
    if((dirPath == NULL) || (fileName == NULL))
    {
        return -1;
    }
	int remove_file_size = 0;
    DIR *dir = NULL;
	struct stat buf;
    struct dirent s_dir;
    struct dirent* ptr = NULL;
    ipclog_info("OpenDir[%s]\n", dirPath);    
    dir = opendir(dirPath);
    if(dir == NULL)
    {
        ipclog_error("%s Opendir  %s failed\n",__FUNCTION__, dirPath);
        return -1;
    }
    
    //char *basename = mybasename(fileName);
    int iOldTime = getTimeFromPath(fileName);
    char fullPath[64] = {0};
	int nCount=0;
    while(!readdir_r(dir, &s_dir, &ptr) && ptr)
    {
    	if(IsSysDir(ptr->d_name)) continue;
        sprintf(fullPath,"%s%s",dirPath,ptr->d_name);
#if 0
        if(IsRecordingFile(fullPath, channel, STOR_LOCAL_HD) == TRUE) /* 正在录像的文件不能删除 */
        {
            continue;
        }
#endif
        
        if(strcmp("..",ptr->d_name) == 0)
        {
            continue;
        }
        
        if(strcmp(".",ptr->d_name) == 0)
        {
            continue;
        }
        
        int iCurTime = getTimeFromPath(fullPath);
        if(iCurTime == -1)
        {
            continue;
        }

        if(!isInOneHour(iOldTime,iCurTime))
        {
            continue;
        }
		stat(fullPath, &buf);
        errno = 0; 
        ipclog_info("@@@removing file %s [%d,%d]\n",fullPath, iOldTime, iCurTime);
        int ret = RemoveFile(fullPath);
        if(ret)
        {
            ipclog_error("Delete file[%s] Return %d errno=%d %s\n",fullPath,ret,errno,strerror(errno));
        }
		else
		{
			remove_file_size += buf.st_size;
			nCount++;//正常删除文件
        }
		int nErr=errno;
        if(ret && nErr != 2)       //if(errno == 30)  Read-only file system  errno == 2 No such file or directory
        {
        	if(nErr==30) return -2;//只读,出错了
        }
        
		if (remove_file_size > (30 * 1024 * 1024))
		{
			break; /* 每次删除的文件大小不能超过30M*/
		}
    }
    
    closedir(dir);

    return nCount;
}

//返回值:0-正常删除或无需删除,-1:只读,-2:目录不存在
static int delete_nulldir(const char *dir_name)
{
    DIR *dir;
    struct stat buf;
    struct dirent s_dir;
    struct dirent *ptr;
    int  nFileCount = 0;
	
    stat(dir_name,&buf);
    if (S_ISDIR(buf.st_mode))
    {
        dir = opendir(dir_name);
        if (dir == 0){
			if(errno==30) return -1;//只读
            return -2;//其它错误
        }
		//文件计数,不包含"index.dat"
        while(!readdir_r(dir, &s_dir, &ptr) && ptr)
        {
            if(strcmp(ptr->d_name,"index.dat") != 0 && strcmp(ptr->d_name, "picture") != 0)
                nFileCount++;
            if(nFileCount > 2)
                break;
        }
        closedir(dir);
        if (nFileCount == 2)//只有"."和".."
        {
            DeleteIndex(dir_name);      
            DeletePictureDir(dir_name);
            ipclog_info("delete_nulldir removedir [%s]\n",dir_name);
            remove(dir_name);
            return 0;
        }
    }
    return -1;
}

/*RemovePerferFile
	pPath : /mnt/mmc1/20160908 最小天
    返回值:0: 成功, -2 :只读
*/
static int RemovePerferFile(const char *pPath)
{
    DIR *dir = NULL;
    struct dirent s_dir;
    struct dirent* ptr = NULL;
    char file_path [64];
    char file_full_name[64];
    char file_min_second[64] = {0};
    int  nTmpData = 0;
    int  nsecondly =-1;
    int  i;
	int nDelCount=0;//正常删除文件的个数


	//找到最早日期
    for (i = 0; i < QMAPI_MAX_CHANNUM; i++)//32通道 m_nRecordChNum
    {
        nsecondly = -1;
        sprintf(file_path,"%s/%d/",pPath,i);
		if (access(file_path, F_OK)) continue;//目录不存在
        dir = opendir(file_path);
        if(dir == NULL)
        {
            if(errno!=2)	//目录不存在
				ipclog_error("## opendir [%s] failed, errno:%d\n",file_path, errno);
            continue;
        }
        while(!readdir_r(dir, &s_dir, &ptr) && ptr)
        {
            //ipclog_debug("[%s,%d]ReadDir: ptr->d_name=[%s]\n", __FUNCTION__, __LINE__, ptr->d_name);
			if(IsSysDir(ptr->d_name)) continue;
			//过滤"." 和".."
            sprintf(file_full_name,"%s%s",file_path,ptr->d_name);
            if(strlen(ptr->d_name) != 10)	//hhmmss.asf
                continue;
            ptr->d_name[6] = 0;	//hhmmss
            nTmpData = atoi(ptr->d_name);
            
            if (nsecondly == -1 || nsecondly > nTmpData)/*找出最小时分秒*/
            {
                nsecondly = nTmpData;
                strcpy(file_min_second,file_full_name);
            }
        }
        closedir(dir);

        if (nsecondly != -1)
        {
            nDelCount=RemoveHourFile(i,file_path,file_min_second, TRUE);
			ipclog_warn("RemoveFile count:[%s] %d\n", file_path, nDelCount);
			if (nDelCount==0) {//删除失败
				ipclog_error("### RemoveFile failed:%s, time:%s\n", file_path, file_min_second);
			}else if(nDelCount==-2 || nDelCount==-1){
				return -2;//出错了,需要重启,修复磁盘
			}
        }
        delete_nulldir(file_path);
    }           //end of each chinnel travel
	delete_nulldir(pPath);
	
	ipclog_info("##delete_nulldir(%s), file_min_second:(%s), nCount=%d\n", pPath, file_min_second, nDelCount);
	
    return 0;
}


/* RM#4268: Remove the oldest record file.  by Henry Li, 2017/09/15 */
/*RemoveOldestFile
	pPath : /mnt/mmc1/20160908 最小天
    返回值:0: 成功, -2 :只读
*/
static int RemoveOldestFile(const char *pPath)
{
    int channel = -1;
    char cmd[80] = {0};
    char file_path[64] = {0};
    char file_oldest[64] = {0};
    int nDelCount = 0;//正常删除文件的个数
    FILE *pFile = NULL;
    int status = -1;

    for (channel = 0; channel < QMAPI_MAX_CHANNUM; channel ++) {//32通道 m_nRecordChNum
        sprintf(file_path, "%s/%d/", pPath, channel);

        if (access(file_path, F_OK) != 0) {
            continue;//目录不存在
        }

        /* make the file list sort by time, use cmd "ls -tr1" */
        sprintf(cmd, "ls %s -tr1 > /tmp/RecordFileList", file_path);
        //system(cmd);
		SystemCall_msg(cmd, SYSTEM_MSG_BLOCK_RUN);

        /* read the oldest filename from the file list */
        pFile = fopen("/tmp/RecordFileList", "r");

        if (pFile == NULL) {
            ipclog_error("Can not open /tmp/RecordFileList!\n");
            continue;
        }

        while (!feof(pFile)) {
            if (fgets(file_oldest, 63, pFile) == NULL) {
                ipclog_error("Can not read more record filename!\n");
                break;
            }

            /* get the oldest record filename */
            file_oldest[strlen(file_oldest) - 1] = '\0';
            //ipclog_debug("the oldest record filename is [%s]\n", file_oldest);

            /* remove the oldest record file */
            sprintf(cmd, "rm -f %s%s", file_path, file_oldest);
            //ipclog_info("Do:[%s]\n", cmd);
            //system(cmd);
            status = SystemCall_msg(cmd, SYSTEM_MSG_BLOCK_RUN);

            if (status == -1) {
                ipclog_error("Run system cmd [%s] error!\n", cmd);
            } else {
                if (WIFEXITED(status)) {
                    if (0 == WEXITSTATUS(status)) {
                        ipclog_info("Run system cmd [%s] successful!\n", cmd);
                        nDelCount ++;
                        if (nDelCount >= 3) {
                            break;
                        }
                    } else {
                        ipclog_error("Run system cmd [%s] error! Exit code: %d\n", cmd, WEXITSTATUS(status));
                    }
                }
            }
        }

        fclose(pFile);
        pFile = NULL;

        delete_nulldir(file_path);
    }

    delete_nulldir(pPath);
    ipclog_info("##delete_nulldir(%s), nCount=%d\n", pPath, nDelCount);

    if (nDelCount == 0) {
        ipclog_error("Delete record file failed!\n");
        return -1;
    }

    return 0;
}


/*
	获取最小天
*/
static int GetMinDateFileTime(const char *pPath, char *FilePath)
{
    DIR *dir;
    struct dirent s_dir;
    struct dirent *ptr;
    int  nTmpData;
    char file_path[32];
    int  nminday = 0;
    
    strcpy(file_path, pPath);
    strcat(file_path, "/");
    dir = opendir(file_path);
    if (dir == NULL)
    {
        ipclog_error("Opendir:%s failed:%s\n", pPath, strerror(errno));
        return 0;
    }
    while (!readdir_r(dir, &s_dir, &ptr) && ptr)
    {
        //ipclog_debug("[%s,%d]ReadDir: ptr->d_name=[%s]\n", __FUNCTION__, __LINE__, ptr->d_name);
    	if (IsSysDir(ptr->d_name)) /*过滤"." 和".."*/
			continue;
		
        if (strlen(ptr->d_name) != 8)
            continue;
        
        nTmpData = atoi(ptr->d_name);
        if (nTmpData % 100 <= 0)
            continue;
        if (nTmpData < 19000000)
            continue;
        if (nminday == 0 || nminday > nTmpData)
        {
            nminday = nTmpData;
			if (FilePath)
			{
				sprintf(FilePath, "%s%s", file_path, ptr->d_name);
			}
        }
    }
    closedir(dir);

    return nminday;
}

int GetFreeSpaceDisk(char *pPath)
{
    int nRet = 0;
    QMAPI_NET_DEVICE_INFO info;
    QMAPI_NET_HDCFG hd;
    int i = 0;
    int nMinFreeSpace = 100; 		//  按0.5MB per second的码流计算

    //U盘/SD卡/硬盘 录像
    nRet = QMapi_sys_ioctrl(sDiskManager.ioctrl_handle, QMAPI_SYSCFG_GET_DEVICECFG, 0,&info, sizeof(QMAPI_NET_DEVICE_INFO));
    if ( nRet != 0 ) {
        ipclog_error(" QMAPI_SYSCFG_GET_DEVICECFG Failed nRet:%d\n", nRet);
        return -2;		
    }
    //保证有足够的空间存储录像文件
    if (info.byRecordLen <= 0) {
        info.byRecordLen = 10;		//默认10分钟
    }
    
    nMinFreeSpace = info.byRecordLen * 50; 		//  按0.5MB per second的码流计算30*1024;//
    
    if (nMinFreeSpace < 100) { /*异常情况也,也要限制在100M */
        nMinFreeSpace = 100;
    }

    /* for test, Henry Li, 2017/09/14 */
    //nMinFreeSpace = 28 * 1024;

    nRet = QMapi_sys_ioctrl(sDiskManager.ioctrl_handle, QMAPI_SYSCFG_GET_HDCFG, 0, &hd, sizeof(QMAPI_NET_HDCFG));
    if ( nRet != 0 || hd.dwHDCount <= 0 ) {
        //		ipclog_error(" QMAPI_SYSCFG_GET_HDCFG nRet:%d, hd.dwHDCount:%d\n", nRet, (int)hd.dwHDCount);
        return -3;
    }

    //Detect Disk is Full
    for( i=0; i<hd.dwHDCount; i++)
    {
        if( hd.stHDInfo[i].dwHdStatus != 0) 
		{
			continue;
		}
		if( nMinFreeSpace  < hd.stHDInfo[i].dwFreeSpace)
		{
			break;
		}
	}

	if (i < hd.dwHDCount )
	{
		if (pPath != NULL)
		{
			strcpy(pPath, (char *)hd.stHDInfo[i].byPath);
		}
//		ipclog_debug(" bFullDisk == FALSE Min:%d MB FreeSpace:%lu MB, %d\n", nMinFreeSpace, hd.stHDInfo[i].dwFreeSpace, (int)(hd.stHDInfo[i].dwFreeSpace-nMinFreeSpace));
		return i;
	}

	ipclog_debug(" bFullDisk == TRUE Min:%d MB FreeSpace:%lu MB\n", nMinFreeSpace, hd.stHDInfo[0].dwFreeSpace);
	return -1;
}

/* HDFullDetectThread
  删除早期创建的文件
 */
void *HDFullDetectThread(void *param)
{
	//pthread_detach(pthread_self());
	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	int nRet;
//	BOOL     bLocalHdStor = FALSE;
	QMAPI_NET_HDCFG hd;
	int i;
	time_t nMintime = -1;
	time_t timeTmp;
	int nMinDisk = -1;
	char FilePath[56];
	QMAPI_NET_DEVICE_INFO info;
	prctl(PR_SET_NAME, (unsigned long)"HDFullDetect", 0,0,0);
	int checkcount = 0;
	//char topdir[200];
    ipclog_info("%s enter\n", __FUNCTION__);
    DISK_MANAGER_T *dm = (DISK_MANAGER_T *)param;

	while(dm->run)
	{	
		if (checkcount++ > 20)
		{
			checkcount = 0;
			//U盘/SD卡/硬盘 录像
			nRet = QMapi_sys_ioctrl(sDiskManager.ioctrl_handle, QMAPI_SYSCFG_GET_DEVICECFG, 0,&info, sizeof(QMAPI_NET_DEVICE_INFO));
			if( nRet != 0 )
			{
				ipclog_error(" QMAPI_SYSCFG_GET_DEVICECFG Failed nRet:%d\n", nRet);
				record_sleep(2000);
				continue;		
			}
			if (!info.byRecycleRecord) //不需要删除文件
			{	
				ipclog_error(" byRecycleRecord is FALSE\n");
				record_sleep(2000);
				continue;
			}
			
			nMinDisk =  GetFreeSpaceDisk(NULL);//-2:dms 错误,-3:获取磁盘信息错误(无磁盘): 
			if (nMinDisk != -1 ) //有磁盘,但不满,不需要删除
			{
				record_sleep(2000);
				continue;
			}

            RECORD_NOTIFY_T notify;
            notify.type = RECORD_NOTIFY_HD_FULL;
            notify.arg  = NULL;
            QM_Event_Send(EVENT_RECORD, &notify, sizeof(notify));

			//磁盘快满,需要删除文件
			nMinDisk = -1;
			nMintime = -1;
			nRet = QMapi_sys_ioctrl(sDiskManager.ioctrl_handle, QMAPI_SYSCFG_GET_HDCFG, 0, &hd, sizeof(QMAPI_NET_HDCFG));
			if (nRet != 0 || hd.dwHDCount <= 0)//没接U盘/SD卡
			{
				record_sleep(2000);
				continue;
			}
			//磁盘已正常挂接
			for( i=0; i<hd.dwHDCount; i++)
			{
				if ( hd.stHDInfo[i].dwHdStatus != 0 || hd.stHDInfo[i].byHDAttr != 0 || strlen((char *)hd.stHDInfo[i].byPath) <= 0) 
				{
					continue;
				}				
				//-----------
				timeTmp = GetMinDateFileTime((const char *)(hd.stHDInfo[i].byPath), FilePath);
				if (timeTmp == 0 )//没找到可删除的文件
				{
					continue;
				}
				
				if( nMintime == -1 || timeTmp < nMintime )
				{
					nMinDisk = i;
					nMintime = timeTmp;//最早时间
				}
			}
			
            if(nMinDisk != -1)//需要删除文件的磁盘编号
			{
				ipclog_info("HDFullDetectThread. FilePath:%s. \n",FilePath);
			    //nRet = RemovePerferFile(FilePath);
			    nRet = RemoveOldestFile(FilePath); /* RM#4268: Remove the oldest record file.  by Henry Li, 2017/09/15 */

			    if (nRet<0)
			    {
			        REC_ERR("Delete Failed,Umount,fsck,Delete again, stat:%lu( nRet:%d)\n", hd.stHDInfo[nMinDisk].dwHdStatus, nRet);
			        int nLast=0;
					//int nErrCount = ReadIOErrorLog(&nLast);
					
                    if(nRet == -2) {
                        //TODO: stop record
                    }

					if(nRet==-2 /*&& nErrCount<3*/){//只读:停止录像,
						//WriteIOErrorLog(nErrCount+1, nLast+1);
						QMapi_sys_ioctrl(0,  QMAPI_NET_STA_REBOOT, 0, 0, 0);
					}
			    }
			}
		}
		record_sleep(1000);
	}
	
	return NULL;
}

int disk_manager_start(int ioctrl_handle)
{
    ipclog_info("%s \n", __FUNCTION__);
    sDiskManager.run = 1;
    sDiskManager.ioctrl_handle = ioctrl_handle;
    
    if (pthread_create(&sDiskManager.tid, NULL, HDFullDetectThread, &sDiskManager) < 0) {
        ipclog_error("%s failed\n", __FUNCTION__);
        sDiskManager.run = 0;
        return -1;
    }

    return 0;
}

int disk_manager_stop(void)
{
    ipclog_info("%s \n", __FUNCTION__);

    if (sDiskManager.run == 0) {
        ipclog_warn("%s already\n", __FUNCTION__);
        return 0;
    }
    sDiskManager.run = 0;

    if (sDiskManager.tid) {
        pthread_join(sDiskManager.tid, NULL);
        sDiskManager.tid = 0;
    }

    return 0;
}


