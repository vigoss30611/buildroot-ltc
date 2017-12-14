#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <pthread.h>
#include <linux/input.h>
#include <stdarg.h>
#include <fcntl.h>

#include "ipc.h"
#include "looper.h"
#include "logger.h"

#include "recorder.h"
#include "QMAPIRecord.h" 

#define LOGTAG  "QM_RECORD"

typedef struct {
    looper_t schedule_lp;
    pthread_mutex_t mutex;
    int inited;
    int started;
} recorder_file_manager_t;


static recorder_file_manager_t sRecorderFileManager = {
    .inited = 0,
    .started = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
};

int recorder_file_manager_init(void)
{
    recorder_file_manager_t *rfm = (recorder_file_manager_t *)&sRecorderFileManager;

    pthread_mutex_lock(&rfm->mutex);
    if (rfm->inited) {
        ipclog_warn("%s already\n", __FUNCTION__);
        pthread_mutex_unlock(&rfm->mutex);
        return 0;
    }

    rfm->inited = 1;
    pthread_mutex_unlock(&rfm->mutex);
    return 0;

}

typedef enum {
	RECORDER_FILE_ADD,
	RECORDER_FILE_REMOVE,
} RECORDER_FILE_STATE_E;

static int recorder_file_manager_update(char *filename, RECORDER_FILE_STATE_E state)
{
	//TODO: QM_Event_Send
	return 0;
}

int recorder_file_manager_add_file(RECORD_FILE_T *file)
{
	char top_dir[64];
	char top_chn_dir[64];
    char video_name[64];
    char new_file_name[LOG_TEXT_LEN];
    char *filename = file->name;

    sscanf(filename, "%[0-9,a-z,/]_%s", top_dir, video_name);
    ipclog_info("filename:%s, top_dir:%s video_name:%s\n", filename,
            top_dir, video_name);

 	sprintf(top_chn_dir, "%s/%d", top_dir, 0);
    
    if (access(top_chn_dir, F_OK) != 0) {
        ipclog_info("make rec store path %s\n", top_chn_dir);
        char cmd[128];
        sprintf(cmd, "mkdir -p %s 0644", top_chn_dir);
        system(cmd);
    }
  

	char *temp_name = (char *)strrchr(filename, '/');
	if (!temp_name) {
		ipclog_warn("%s %s is invalid video file\n", __FUNCTION__, filename); 
		return -1;
	}
	temp_name += 1; /*!< skip '/' */
	int duration = 0; 
	struct tm tm = {0};
	char event_type[64];	 
	int scan_ret = sscanf(temp_name, "%4d%2d%2d_%2d%2d%2d_%[N,M]_%6d.mp4", 
                      &tm.tm_year,&tm.tm_mon,&tm.tm_mday, 
                      &tm.tm_hour,&tm.tm_min,&tm.tm_sec, 
                      event_type, &duration); 
   #define SSCANF_ALL_CNT 8
    if (scan_ret != SSCANF_ALL_CNT) { 
            ipclog_warn("%s %s is invalid video file\n", __FUNCTION__, filename);  
            return -1; 
    } 

	ipclog_debug("After sscanf:%04d%02d%02d_%02d%02d%02d_%s_%06d \n", tm.tm_year, tm.tm_mon, tm.tm_mday, 
							tm.tm_hour, tm.tm_min, tm.tm_sec,
							event_type, duration);
    
	sprintf(new_file_name, "%s/%02d%02d%02d.%s", top_chn_dir, tm.tm_hour, tm.tm_min, tm.tm_sec, CFG_RECORD_CONTAINER_TYPE);
    ipclog_info("new_file_name:%s\n", new_file_name);

    if (rename(filename, new_file_name) != 0) {
        ipclog_error("rename filename:%s new_file_name:%s failed\n", filename, new_file_name);
        return -1;
    }

    tm.tm_year = tm.tm_year-1900; 
    tm.tm_mon = tm.tm_mon-1; 
    time_t ts = mktime(&tm);

 	log_head_t *logHead = NULL;
    if (!logHead) {
        char log_name[128];
        sprintf(log_name, "%s/index.dat", top_dir);
        logHead = log_init(log_name, 1);
    }
		
	if (logHead) {
        ipclog_info("%s log:%d.\n",__FUNCTION__,logHead->log_pos);
		int r;
        log_content_t cont;
        cont.log_ch = 0;
        cont.log_type = file->trigger;
        cont.log_opt = 1;
        cont.log_time = ts;
        memcpy(cont.log_text, new_file_name, LOG_TEXT_LEN);
        ipclog_info("%s ch:%u. type:%u opt:%u. time:%ld text:%s.\n",__FUNCTION__,
                cont.log_ch,cont.log_type,cont.log_opt,cont.log_time,cont.log_text);
        r = log_write(logHead, &cont, 1);
        ipclog_info("%s log write ok. %d. \n",__FUNCTION__,r);
        log_exit(logHead);
    	logHead = NULL;
	}

	recorder_file_manager_update(new_file_name, RECORDER_FILE_ADD);

    return 0;
}

int recorder_file_manager_delete_file(char *filename)
{
 	if (remove(filename) == 0) {
        recorder_file_manager_update(filename, RECORDER_FILE_REMOVE);
    }
}

/* CheckAndMkdir
  建立/aa/bb/cc/file.txt,必须的文件
  返回值:0:成功, -1:出错
*/
int Local_CheckAndMkdir(const char *sFileName)
{
	int nRet=-1;
	const char *pServerFile=strrchr(sFileName, '/');
	if(pServerFile && pServerFile!=sFileName)
	{
		char sPath[1024]={0};
		char sPath0[1024]={0};
		strncpy(sPath, sFileName, pServerFile-sFileName+1);
		if(access(sPath, F_OK))
		{	//目录不存在
			char *p=strchr(sPath+1, '/');
			char *p0=sPath;
			do
			{
				if(!p) break;
				strncpy(sPath0, sPath, p-sPath);
				p++;
//				printf("check and mkdir [%s], access=%d\n", sPath0, access(sPath0, F_OK));
				nRet=0;
				if(access(sPath0, F_OK)){//目录不存在
					nRet=mkdir(sPath0, 0777);
//					printf("mkdir ret:%d by [%s], errno:(%d)\n", nRet, sPath0, errno);
					if(nRet==-1 && errno!=17) break;//建目录失败
				}
				p0=p;
				p=strchr(p0+1, '/');
			}while(p);
			return access(sPath, F_OK)==0?0:-1;
		}

		//return CheckOpenDir(sPath)?0:-1;//偿试打开目录
		nRet = 0; //如果目录无法打开，则创建文件也会失败
	}
	return nRet;
}

/*
	图片保存。返回实际写入的数据长度

*/
int snap_write(char *FileName, char *SnapData, int SnapLen)
{
	int fd, len = 0;
    printf("%s: %s\n", __FUNCTION__, FileName);

    int nChk=Local_CheckAndMkdir(FileName);//创建必须的目录	
    if(nChk==-1) return -1;//不能建立目录
	
	fd = open(FileName, O_CREAT | O_WRONLY | O_TRUNC);
	if (fd >= 0)
	{
		len = write(fd, SnapData, SnapLen);
		if (len != SnapLen)
		{
			perror("snap_write : ");
		}
		close(fd);
	}
    else
    {
        printf("create %s failed\n", FileName);
    }

	return len;
}

/*
	图片读取。返回实际读取的数据长度

*/
int snap_read(char *FileName, char *SnapData, int SnapLen)
{
	int fd, len = 0;
	
	fd = open(FileName, O_RDONLY | O_TRUNC);
	if (fd >= 0)
	{
		len = read(fd, SnapData, SnapLen);
		if (len != SnapLen)
		{
			perror("snap_write : ");
		}
		close(fd);
	}

	return len;
}

