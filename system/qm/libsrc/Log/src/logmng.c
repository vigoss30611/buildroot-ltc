
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "logmng.h"
#include "libsyslogapi.h"


#define QMAPI_LOG_MAGIC         "dms_log"
#define QMAPI_LOG_VERSION       "1.0"
#define QMAPI_LOG_DATE          20151026
#define QMAPI_LOG_FILE_SIZE     32*1024
#define SECOND_2000_YEAR        946656000 //2000相对1970年的秒数

static pthread_mutex_t g_pLogMutex;
static QMAPI_LOG_INDEX *g_pLogIndex = NULL;
static QMAPI_LOG_HEADER g_header;
static int g_log_fd = -1;

static int Write_Head();
static int Write_Block(unsigned long index, const char* pBlockBuf);

#if 0
static void Debug_Print()
{
	int i=0;
	printf("%s  %d, curr_index=%d, index_max=%d, index_size=%d, curr_block=%d\n",__FUNCTION__, __LINE__, g_header.curr_index, g_header.index_max, g_header.index_size, g_header.curr_block);
	for(i=0; i<g_header.index_max; i++)
	{
		if(0 != g_pLogIndex[i].byUsed)
			printf("i=%04d, bUsed=%d, mainType=%d, subType=%d,time=%lu, blockID=%d\n", i, g_pLogIndex[i].byUsed, g_pLogIndex[i].byMainType, g_pLogIndex[i].bySubType,g_pLogIndex[i].time, g_pLogIndex[i].byBlockID);
                //printf("i=%04d, bUsed=%d, mainType=%d, subType=%d,time=%lu, blockID=%d\n", i, g_pLogIndex[i].byUsed, g_pLogIndex[i].byMainType, g_pLogIndex[i].bySubType,g_pLogIndex[i].time, g_pLogIndex[i].byBlockID);
        }

       printf("%s  %d, end debug\n",__FUNCTION__, __LINE__);
	for(i=1; i<g_header.block_max; i++)
	{
		//printf("i=%03d, bUsed=%d\n", i, g_header.block_used[i]);
	}
}
#endif

static void Default_log_file()
{
	memset(&g_header, 0, sizeof(g_header));

	memcpy(g_header.magic, QMAPI_LOG_MAGIC,  sizeof(g_header.magic));
	memcpy(g_header.version, QMAPI_LOG_VERSION,  sizeof(g_header.version));
	g_header.ver_date = QMAPI_LOG_DATE;
	g_header.file_size = QMAPI_LOG_FILE_SIZE;

	//先预留block需要的空间
	g_header.block_size = QMAPI_LOG_BLOCK_SIZE;
	g_header.block_max = QMAPI_LOG_BLOCK_MAX;
	g_header.curr_block = 1;//1~255
	g_header.offset_block = g_header.file_size - (QMAPI_LOG_BLOCK_SIZE * QMAPI_LOG_BLOCK_MAX);

	//剩下的全部分配给index
	g_header.index_size = sizeof(QMAPI_LOG_INDEX);
	g_header.offset_index = (sizeof(QMAPI_LOG_HEADER) + 0xff) & (~0xff);//按照256字节对齐
	g_header.index_max =  (g_header.offset_block - g_header.offset_index)/g_header.index_size;
	g_header.curr_index = 0;
	g_header.index_used_flag = 1;

	memset(g_header.block_used, 0, sizeof(g_header.block_used));
}

static int CheckLogHead()
{
	if(0 != strcmp(g_header.magic, QMAPI_LOG_MAGIC))
	{
		printf("%s  %d, QMAPI_LOG_MAGIC:%s, magic:%s, not match\n",__FUNCTION__, __LINE__, QMAPI_LOG_MAGIC, g_header.magic);
		return 1;
	}

	if(0 != strcmp(g_header.version, QMAPI_LOG_VERSION))
	{
		printf("%s  %d, QMAPI_LOG_VERSION:%s, version:%s, not match\n",__FUNCTION__, __LINE__, QMAPI_LOG_VERSION, g_header.version);
		return 1;
	}
	
	if(g_header.ver_date != QMAPI_LOG_DATE || g_header.index_size != sizeof(QMAPI_LOG_INDEX))
	{
		printf("%s  %d, ver_date=%lu, block_size=%lu\n",__FUNCTION__, __LINE__, g_header.ver_date, g_header.index_size);
		return 1;
	}

	if(0==g_header.index_size || g_header.index_max !=  (g_header.offset_block - g_header.offset_index)/g_header.index_size)
	{
		printf("%s  %d, g_header.index_max=%lu, g_header.file_size=%lu, g_header.offset_index=%lu\n",__FUNCTION__, __LINE__, g_header.index_max, g_header.file_size, g_header.offset_index);
		return 1;
	}

	return 0;
}


//修复未刷入的curr_index序号
static void Repair()
{
	unsigned short bNeedWriteHead = 0;
	assert(0 == g_header.curr_index || g_header.index_used_flag == g_pLogIndex[g_header.curr_index-1].byUsed);
	if(0 < g_header.curr_index)
	{
		g_header.curr_index--;
	}
	
	while(g_header.index_used_flag == g_pLogIndex[g_header.curr_index].byUsed && g_header.curr_index < g_header.index_max)
	{
		printf("%s  %d, curr_index=%lu, used=%d\n",__FUNCTION__, __LINE__, g_header.curr_index, g_pLogIndex[g_header.curr_index].byUsed);
		if(0 != g_pLogIndex[g_header.curr_index].byBlockID)
		{
			g_header.block_used[g_pLogIndex[g_header.curr_index].byBlockID] = 1;
			g_header.curr_block = g_pLogIndex[g_header.curr_index].byBlockID;
		}
		g_header.curr_index++;
		bNeedWriteHead = 1;
	}
    
	g_header.curr_block ++;

	if(0 != bNeedWriteHead)
	{
		Write_Head();
	}
}

int DmsLog_Init(const char * plogPath)
{
	pthread_mutex_lock(&g_pLogMutex);
	int logDataLen = 0;
	int logBlockLen = 0;
	int len = 0;
	int ret = 0;
	char* pTmpBuf = NULL;
	
	if(NULL == plogPath || 0 == strlen(plogPath))
	{
		printf("%s  %d, error  log model can not be init by empty path\n",__FUNCTION__, __LINE__);
		goto err_out;
	}

	if(NULL != g_pLogIndex)
	{
		printf("%s  %d, error  log model be inited twice\n",__FUNCTION__, __LINE__);
		goto err_out;
	}

	if(-1 != g_log_fd)
	{
		printf("%s  %d, file is open\n",__FUNCTION__, __LINE__);
		goto err_out;
	}

	memset(&g_header, 0, sizeof(g_header));

	if(access(plogPath, F_OK)!=0)
	{
		g_log_fd = open(plogPath, O_RDWR|O_CREAT);
		if (g_log_fd < 0)
		{
			printf("%s  %d,open syslog file fd error !\n", __FUNCTION__, __LINE__);
			goto err_out;
		}
		
		Default_log_file();

		len = write(g_log_fd, &g_header, sizeof(g_header));
		if(len != sizeof(g_header))
		{
			printf("%s  %d, errno=%d\n",__FUNCTION__, __LINE__, errno);
			goto err_out;
		}
		
		logDataLen = g_header.index_max * g_header.index_size;
		g_pLogIndex = (QMAPI_LOG_INDEX*)malloc(logDataLen);
		assert(NULL != g_pLogIndex);
		memset(g_pLogIndex, 0, logDataLen);

		ret = lseek(g_log_fd, g_header.offset_index, SEEK_SET);
		if(g_header.offset_index != ret)
		{
			printf("%s  %d, errno=%d\n",__FUNCTION__, __LINE__, errno);
			goto err_out;
		}
		
		len = write(g_log_fd, g_pLogIndex, logDataLen);
		if(len != logDataLen)
		{
			printf("%s  %d, errno=%d\n",__FUNCTION__, __LINE__, errno);
			goto err_out;
		}
		
		logBlockLen = g_header.block_max * g_header.block_size;
		pTmpBuf = (char*)malloc(logBlockLen);
		if(NULL == pTmpBuf)
		{
			printf("%s:%d malloc failed\n", __FILE__, __LINE__);
			goto err_out;
		}
		memset(pTmpBuf, 0, logBlockLen);
		
		ret = lseek(g_log_fd, g_header.offset_block, SEEK_SET);
		if(g_header.offset_block != ret)
		{
			printf("%s  %d, errno=%d\n",__FUNCTION__, __LINE__, errno);
			goto err_out;
		}
		
		len = write(g_log_fd, pTmpBuf, logBlockLen);
		if(len != logBlockLen)
		{
			printf("%s  %d, errno=%d\n",__FUNCTION__, __LINE__, errno);
			goto err_out;
		}

		if(NULL != pTmpBuf)
		{
			free(pTmpBuf);
			pTmpBuf = NULL;
		}
	}
	else
	{
		g_log_fd = open(plogPath, O_RDWR);
		if (g_log_fd < 0)
		{
			printf("%s  %d,open syslog file fd error !\n", __FUNCTION__, __LINE__);
			goto err_out;
		}

		len = read(g_log_fd, &g_header, sizeof(g_header));
		if(len != sizeof(g_header))
		{
			printf("%s  %d, errno=%d\n",__FUNCTION__, __LINE__, errno);
			goto err_out;
		}

		CheckLogHead();

		logDataLen = g_header.index_max * g_header.index_size;
		g_pLogIndex = (QMAPI_LOG_INDEX*)malloc(logDataLen);
		assert(NULL != g_pLogIndex);
		memset(g_pLogIndex, 0, logDataLen);

		ret = lseek(g_log_fd, g_header.offset_index, SEEK_SET);
		if(g_header.offset_index != ret)
		{
			printf("%s  %d, errno=%d\n",__FUNCTION__, __LINE__, errno);
			goto err_out;
		}
		
		len = read(g_log_fd, g_pLogIndex, logDataLen);
		if(len != logDataLen)
		{
			printf("%s  %d, errno=%d\n",__FUNCTION__, __LINE__, errno);
			goto err_out;
		}

		Repair();
	}
    
	pthread_mutex_unlock(&g_pLogMutex);
	return 0;
err_out:
	printf("%s  %d, err out\n",__FUNCTION__, __LINE__);

	if(NULL != pTmpBuf)
	{
		free(pTmpBuf);
		pTmpBuf = NULL;
	}
	if(NULL != g_pLogIndex)
	{
		free(g_pLogIndex);
		g_pLogIndex = NULL;
	}

	if (-1 != g_log_fd)
	{
		close(g_log_fd);
		g_log_fd = -1;
	}
	pthread_mutex_unlock(&g_pLogMutex);
	return -1;
}


void DmsLog_Quit()
{
	pthread_mutex_unlock(&g_pLogMutex);
	if(NULL != g_pLogIndex)
	{
		free(g_pLogIndex);
		g_pLogIndex = NULL;
	}

	if (-1 != g_log_fd)
	{
		close(g_log_fd);
		g_log_fd = -1;
	}
	
	pthread_mutex_unlock(&g_pLogMutex);
}

int Repair_LogTime()
{
	int i = 0;
	int bEnd= 0;//是否停止修复  修复到最近一次重启，或者日志全部修复完成
	pthread_mutex_unlock(&g_pLogMutex);
	if(NULL == g_pLogIndex || -1 == g_log_fd)
	{
		printf("%s  %d, log mode is not init\n",__FUNCTION__, __LINE__);
		pthread_mutex_unlock(&g_pLogMutex);
		return -1;
	}
	
	int ret = 0;
	int dataLen = 0;
	unsigned int differTime = 0;
	unsigned long currtime  = time(NULL);//curr time
	struct timespec tp;
	if(clock_gettime(CLOCK_MONOTONIC, &tp) < 0)
	{
		assert(0);
	}
	
	printf("%s  %d, time=%lu, curr Tick tp.tv_sec=%lu\n",__FUNCTION__, __LINE__, currtime, tp.tv_sec);
	if(currtime > tp.tv_sec)
	{
		differTime = currtime - tp.tv_sec;
	}
	else
	{
		assert(0);
	}

	if(0 != g_pLogIndex[g_header.curr_index-1].byUsed && SECOND_2000_YEAR > g_pLogIndex[g_header.curr_index-1].time)
	{
		for(i = g_header.curr_index-1;i>=0; i--)
		{
			if(0 != g_pLogIndex[i].byUsed && SECOND_2000_YEAR > g_pLogIndex[i].time)
			{
				g_pLogIndex[i].time += differTime;
				if(QMAPI_LOG_TYPE_OPER == g_pLogIndex[i].byMainType && QMAPI_LOG_OPER_POWERON == g_pLogIndex[i].bySubType)
				{
					i--;//跳到未改变的
					bEnd = 1;
					break;
				}
			}
			else
			{
				bEnd = 1;
				break;
			}
		}

		if(1 != bEnd)
		{
			for(i = g_header.index_max; i > g_header.curr_index-1; i--)
			{
				if(0 != g_pLogIndex[i].byUsed && SECOND_2000_YEAR > g_pLogIndex[i].time)
				{
					g_pLogIndex[i].time += differTime;
				}
				else
				{
					bEnd = 1;
					break;
				}
			}
		}

		do
		{
			if(i != g_header.curr_index-1)
			{
				i++;//回退1  表示最后修改的位置
				if(i <= g_header.curr_index-1)
				{
					//写入index
					ret = lseek(g_log_fd, g_header.offset_index + i * g_header.index_size, SEEK_SET);
					if(g_header.offset_index + i * g_header.index_size != ret)
					{
						printf("%s  %d, errno=%d\n",__FUNCTION__, __LINE__, errno);
						ret = -1;
						break;
					}

					dataLen = (g_header.curr_index- i )* sizeof(QMAPI_LOG_INDEX);
					ret= write(g_log_fd, &(g_pLogIndex[i]), dataLen);
					//printf("#### %s %d, errno=%d, len=%d\n", __FUNCTION__, __LINE__, errno, len);
					if(ret != dataLen)
					{
						printf("%s  %d, errno=%d, ret=%d, dataLen=%d\n",__FUNCTION__, __LINE__, errno, ret, dataLen);
						ret = -1;
						break;
					}
				}
				else
				{
					//写入g_header.curr_index之前的index
					ret = lseek(g_log_fd, g_header.offset_index, SEEK_SET);
					if(g_header.offset_index != ret)
					{
						printf("%s  %d, errno=%d\n",__FUNCTION__, __LINE__, errno);
						ret = -1;
						break;
					}

					dataLen =  i * sizeof(QMAPI_LOG_INDEX);
					ret= write(g_log_fd, g_pLogIndex, dataLen);
					//printf("#### %s %d, errno=%d, len=%d\n", __FUNCTION__, __LINE__, errno, len);
					if(ret != dataLen)
					{
						printf("%s  %d, errno=%d, ret=%d, dataLen=%d\n",__FUNCTION__, __LINE__, errno, ret, dataLen);
						ret = -1;
						break;
					}

					//写入g_header.curr_index之后的index
					ret = lseek(g_log_fd, g_header.offset_index + i * g_header.index_size, SEEK_SET);
					if(g_header.offset_index + i * g_header.index_size != ret)
					{
						printf("%s  %d, errno=%d\n",__FUNCTION__, __LINE__, errno);
						ret = -1;
						break;
					}

					dataLen = (g_header.index_max- i) * sizeof(QMAPI_LOG_INDEX);
					ret= write(g_log_fd, &(g_pLogIndex[i]), dataLen);
					//printf("#### %s %d, errno=%d, len=%d\n", __FUNCTION__, __LINE__, errno, len);
					if(ret != dataLen)
					{
						printf("%s  %d, errno=%d, ret=%d, dataLen=%d\n",__FUNCTION__, __LINE__, errno, ret, dataLen);
						ret = -1;
						break;
					}
				}
			}
		}while(0);
	}
	
	pthread_mutex_unlock(&g_pLogMutex);
	return ret;
}

int Update_PowerOnTime()
{
    //printf("%s  %d, log update time \n",__FUNCTION__, __LINE__);
	int i = 0;
	pthread_mutex_unlock(&g_pLogMutex);
	if(NULL == g_pLogIndex || -1 == g_log_fd)
	{
		printf("%s  %d, log mode is not init\n",__FUNCTION__, __LINE__);
		pthread_mutex_unlock(&g_pLogMutex);
		return -1;
	}

	int ret = 1;
	struct timespec tp;
    char exInfo[QMAPI_LOG_BLOCK_SIZE] = {0};
	if(clock_gettime(CLOCK_MONOTONIC, &tp) < 0)
	{
		assert(0);
	}
    //printf("%s  %d, time=%lu, curr_index=%lu\n",__FUNCTION__, __LINE__, tp.tv_sec, g_header.curr_index);
    //Debug_Print();

	for(i = g_header.curr_index-1;i>=0; i--)
	{
	    if(QMAPI_LOG_TYPE_OPER== g_pLogIndex[i].byMainType && QMAPI_LOG_OPER_POWERON == g_pLogIndex[i].bySubType)
    	{
            sprintf(exInfo, "%08lu", tp.tv_sec);
            //printf("%s  %d, power on inde=%d, byBlockID=%d, exInfo:%s\n",__FUNCTION__, __LINE__, i, g_pLogIndex[i].byBlockID, exInfo);
            Write_Block(g_pLogIndex[i].byBlockID,  exInfo);
            ret = 0;
            break;
    	}
	}
	
	pthread_mutex_unlock(&g_pLogMutex);
	return ret;
}

static int Write_Pos(int fd, unsigned long pos, const unsigned char * pBuf, unsigned long len)
{
	int ret = 0;
	
	ret = lseek(fd, pos, SEEK_SET);
	if(pos != ret)
	{
		printf("%s  %d, errno=%d\n",__FUNCTION__, __LINE__, errno);
		return -1;
	}

	ret= write(fd, pBuf, len);
	//printf("#### %s %d, errno=%d, len=%d\n", __FUNCTION__, __LINE__, errno, len);
	if(ret != len)
	{
		printf("%s  %d, errno=%d, ret=%d, len=%lu\n",__FUNCTION__, __LINE__, errno, ret, len);
		return -1;
	}
	
	return 0;
}

static int Write_Head()
{
	int dataLen = 0;
	int offset = 0;

	dataLen = sizeof(QMAPI_LOG_HEADER);

	return Write_Pos(g_log_fd, offset, (unsigned char*)&g_header, dataLen);
}

static int Write_Index(unsigned long index)
{
	int dataLen = 0;
	int offset = 0;

	offset = g_header.offset_index + index * g_header.index_size;
	dataLen = g_header.index_size;

	return Write_Pos(g_log_fd, offset, (unsigned char*)&(g_pLogIndex[index]), dataLen);
}


static int Write_Block(unsigned long index, const char* pBlockBuf)
{
	int dataLen = 0;
	int offset = 0;

	offset = g_header.offset_block+ index * g_header.block_size;
	dataLen = g_header.block_size;

	return Write_Pos(g_log_fd, offset, (unsigned char *)pBlockBuf, dataLen);
}


static int Read_Pos(int fd, unsigned long pos, unsigned char * pBuf, unsigned long len)
{
	int ret = 0;
	
	ret = lseek(fd, pos, SEEK_SET);
	if(pos != ret)
	{
		printf("%s  %d, errno=%d\n",__FUNCTION__, __LINE__, errno);
		return -1;
	}

	ret= read(fd, pBuf, len);
	//printf("#### %s %d, errno=%d, len=%d\n", __FUNCTION__, __LINE__, errno, len);
	if(ret != len)
	{
		printf("%s  %d, errno=%d, ret=%d, len=%lu\n",__FUNCTION__, __LINE__, errno, ret, len);
		return -1;
	}
	
	return 0;
}


static int Read_Block(unsigned long index, char* pBlockBuf, unsigned int* pLen)
{
	assert(g_header.block_size <= *pLen);
	int dataLen = 0;
	int offset = 0;

	offset = g_header.offset_block+ index * g_header.block_size;
	dataLen = g_header.block_size;

	return Read_Pos(g_log_fd, offset, (unsigned char*)pBlockBuf, dataLen);
}


int Wirte_Log(const QMAPI_LOG_INFO* pLogInfo)
{
	int ret = 0;
	int needWriteHead = 0;
	BYTE usableBlock = 0;
	unsigned short bFind = 0;
	unsigned long subTypeMinLimit = 0;
	unsigned long subTypeMaxLimit = 0;
    char exInfo[QMAPI_LOG_BLOCK_SIZE];
	

	printf("%s  %d, maintype=%d, subtype=%lu\n",__FUNCTION__, __LINE__,pLogInfo->maintype, pLogInfo->subtype);
	switch(pLogInfo->maintype)
	{
		case QMAPI_LOG_TYPE_OPER:
		{
			subTypeMinLimit = QMAPI_LOG_OPER_ALL;
			subTypeMaxLimit = QMAPI_LOG_OPER_BUTT;
			break;
		}
		case QMAPI_LOG_TYPE_ALARM:     /* 告警类型 */
		{
			subTypeMinLimit = QMAPI_LOG_ALARM_ALL;
			subTypeMaxLimit = QMAPI_LOG_ALARM_BUTT;
			break;
		}
		case QMAPI_LOG_TYPE_SMART:     /* 告警类型 */
		{
			subTypeMinLimit = QMAPI_LOG_SMART_ALL;
			subTypeMaxLimit = QMAPI_LOG_SMART_BUTT;
			break;
		}
		case QMAPI_LOG_TYPE_ACCESS:	/* 访问类型 */
		{
			subTypeMinLimit = QMAPI_LOG_ACCESS_ALL;
			subTypeMaxLimit = QMAPI_LOG_ACCESS_BUTT;
			break;
		}
		case QMAPI_LOG_TYPE_INFO:      /* 提示信息类型 */
		{
			subTypeMinLimit = QMAPI_LOG_INFO_ALL;
			subTypeMaxLimit = QMAPI_LOG_INFO_BUTT;
			break;
		}
		case QMAPI_LOG_TYPE_ABNORMAL:  /* 异常类型 */
		{
			subTypeMinLimit = QMAPI_LOG_ERROR_ALL;
			subTypeMaxLimit = QMAPI_LOG_ERROR_BUTT;
			break;
		}
		default:
		{
			printf("%s  %d, blockID is not find from index\n",__FUNCTION__, __LINE__);
			break;
		}
	}

	assert(pLogInfo->subtype>subTypeMinLimit && pLogInfo->subtype<subTypeMaxLimit);
	if(pLogInfo->subtype<=subTypeMinLimit || pLogInfo->subtype >= subTypeMaxLimit)
	{
		printf("%s  %d,input log type error, subTypeMinLimit=%lu, subTypeMaxLimit=%lu\n",__FUNCTION__, __LINE__, subTypeMinLimit, subTypeMaxLimit);
		return 1;
	}

        strcpy(exInfo, pLogInfo->pExInfo);
	if(QMAPI_LOG_TYPE_OPER== pLogInfo->maintype && QMAPI_LOG_OPER_POWERON == pLogInfo->subtype)
	{
		struct timespec tp;
		if(clock_gettime(CLOCK_MONOTONIC, &tp) < 0)
		{
            		assert(0);
		}

		sprintf(exInfo, "%08lu", tp.tv_sec);
	}

	
	QMAPI_LOG_INDEX* pLogIndex = NULL;
	
	pthread_mutex_lock(&g_pLogMutex);
	do
	{
		if(NULL == g_pLogIndex || -1 == g_log_fd)
		{
			ret = 1;
			break;
		}

		//找到可用的块，先写入块信息，然后再更新索引和头
		if(pLogInfo->byUsedExInfo)
		{
			printf("%s  %d, pExInfo:%s\n",__FUNCTION__, __LINE__, exInfo);
			//被使用中的block
			if(0 != g_header.block_used[g_header.curr_block])
			{
				int i = 0;
				//从curr_index开始  向后查找到使用这个block的index，擦除
				for(i=g_header.curr_index; i<g_header.index_max; i++)
				{
					if(g_header.curr_block == g_pLogIndex[i].byBlockID)
					{
						g_pLogIndex[i].byBlockID = 0;
						//写入index
						if(0 != Write_Index(i))
						{
							ret = -1;
							break;
						}
						
						g_header.block_used[g_header.curr_block] = 0;
						bFind = 1;
						break;
					}
					//else if(g_header.curr_block < g_pLogIndex[i].blockID)
					//{
					//	break;
					//}
				}

				if(!bFind && i == g_header.index_max)
				{
					for(i=0; i<g_header.curr_index; i++)
					{
						if(g_header.curr_block == g_pLogIndex[i].byBlockID)
						{
							g_pLogIndex[i].byBlockID = 0;
							//写入index
							Write_Index(i);
							
							g_header.block_used[g_header.curr_block] = 0;
							
							bFind = 1;
							break;
						}
					}
				}

				if(!bFind)
				{
					printf("%s  %d, blockID is not find from index\n",__FUNCTION__, __LINE__);
				}
			}

			usableBlock = g_header.curr_block;
			g_header.curr_block++;
			if(g_header.curr_block >= g_header.block_max)
			{
				g_header.curr_block = 1;
			}
			needWriteHead = 1;
		}

		pLogIndex = &(g_pLogIndex[g_header.curr_index]);
		if(0 != usableBlock)
		{
			g_header.block_used[usableBlock] = 1;
			needWriteHead = 1;
		}
		memset(pLogIndex, 0, sizeof(QMAPI_LOG_INDEX));
		pLogIndex->byBlockID 	= usableBlock;
		pLogIndex->byUsed 	= g_header.index_used_flag;
		pLogIndex->byMainType 	= pLogInfo->maintype;
		pLogIndex->bySubType 	= pLogInfo->subtype;
		pLogIndex->ip 		= pLogInfo->ip;
		pLogIndex->byUserID 	= pLogInfo->byUserID;
		pLogIndex->time 		= time(NULL);

		//return tp.tv_sec * 1000 + tp.tv_nsec / 1000000;

		printf("%s  %d, curr_index=%lu, bUsed=%d, mainType=%d, curr_block=%lu\n",__FUNCTION__, __LINE__, g_header.curr_index, g_pLogIndex[g_header.curr_index].byUsed, g_pLogIndex[g_header.curr_index].byMainType, g_header.curr_block);
		//g_pLogIndex[g_header.curr_index] = logIndex;

		//写入block
		if(pLogInfo->byUsedExInfo)
		{
			Write_Block(pLogIndex->byBlockID, exInfo);
		}

		//写入index
		if(0 != Write_Index(g_header.curr_index))
		{
			ret = -1;
			break;
		}
		
		g_header.curr_index++;
		if(g_header.curr_index >= g_header.index_max)
		{
			g_header.curr_index = 0;
			g_header.index_used_flag = (1==g_header.index_used_flag)? 2 : 1;
			needWriteHead = 1;
		}
	
		//写入head
		if(needWriteHead)
		{
			if(0 != Write_Head())
			{
				ret = -1;
				break;
			}
		}
		
	}while(0);
	

	pthread_mutex_unlock(&g_pLogMutex);
	return ret;
}

void Search_Log(QMAPI_LOG_MAIN_TYPE_E maintype, unsigned long subType, QMAPI_LOG_INDEX* pLogInfo, unsigned int* pNum, char (*pLogExtInfo)[QMAPI_LOG_BLOCK_SIZE], unsigned int extInfoBufLen)
{
	assert(NULL != pLogInfo && NULL != pLogExtInfo);
	if(NULL == pLogInfo || NULL == pLogExtInfo || 0 == *pNum || 0 == extInfoBufLen)
	{
		printf("%s  %d, input is error\n",__FUNCTION__, __LINE__);
		return ;
	}

	printf("%s  %d, input:mainType=%d, subType=%lu, num=%u\n",__FUNCTION__, __LINE__, maintype, subType, *pNum);
	memset(pLogExtInfo, 0, extInfoBufLen);

	//printf("%s  %d, input:mainType=%lu, subType=%lu, num=%d\n",__FUNCTION__, __LINE__, maintype, subType, *pNum);
	unsigned long beginIndex = 0;
	int i = 0;
	unsigned int j=0;
	pthread_mutex_lock(&g_pLogMutex);
	//Debug_Print();
	if(0 == g_header.curr_index)
	{
		beginIndex = g_header.index_max;
	}
	else
	{
		beginIndex = g_header.curr_index-1;
	}

	if(g_header.index_max <= *pNum)
	{
		*pNum = g_header.index_max;
	}	

	unsigned short bJudgeMainType = 1;//是否需要判断主类型
	unsigned short bJudgeSubType = 1;//是否需要判断子类型

	switch(maintype)
	{
		case QMAPI_LOG_TYPE_ALL:
		{
			bJudgeMainType = 0;
			bJudgeSubType = 0;
			break;
		}
		case QMAPI_LOG_TYPE_OPER:
		{
			bJudgeMainType = 1;
			bJudgeSubType = (QMAPI_LOG_OPER_ALL == subType) ? 0:1;
			break;
		}
		case QMAPI_LOG_TYPE_ALARM:
		{
			bJudgeMainType = 1;
			bJudgeSubType = (QMAPI_LOG_ALARM_ALL == subType) ? 0:1;
			break;
		}
		case QMAPI_LOG_TYPE_SMART:
		{
			bJudgeMainType = 1;
			bJudgeSubType = (QMAPI_LOG_ERROR_ALL == subType) ? 0:1;
			break;
		}
		case QMAPI_LOG_TYPE_ACCESS:
		{
			bJudgeMainType = 1;
			bJudgeSubType = (QMAPI_LOG_ACCESS_ALL == subType) ? 0:1;
			break;
		}
		case QMAPI_LOG_TYPE_INFO:
		{
			bJudgeMainType = 1;
			bJudgeSubType = (QMAPI_LOG_INFO_ALL == subType) ? 0:1;
			break;
		}
		case QMAPI_LOG_TYPE_ABNORMAL:
		{
			bJudgeMainType = 1;
			bJudgeSubType = (QMAPI_LOG_ERROR_ALL == subType) ? 0:1;
			break;
		}
		default:
		{
			//默认搜索所有类型日志
			printf("%s  %d, default search all logType, input:maintype=%d, subType=%lu\n",__FUNCTION__, __LINE__, maintype, subType);
			bJudgeMainType = 0;
			bJudgeSubType = 0;
			break;
		}
	}

	//遍历到g_pLogIndex的最开始位置，这部分是比较新的日志
	for(i=beginIndex, j=0; i>=0 && j < *pNum; i--)
	{
		if(0 != g_pLogIndex[i].byUsed && (!bJudgeMainType || maintype == g_pLogIndex[i].byMainType) && (!bJudgeSubType || subType== g_pLogIndex[i].bySubType))
		{
			pLogInfo[j] = g_pLogIndex[i];
			j++;
		}
		//memcpy(&(pLogInfo[j]), &(g_pLogIndex[i]), sizeof(QMAPI_LOG_INDEX));
	}
		//不是从最后开始遍历，还需要遍历尾部
	if(g_header.index_max != beginIndex)
	{
		for(i = g_header.index_max; i>beginIndex && j < *pNum; i--)
		{
			if(0 != g_pLogIndex[i].byUsed && (!bJudgeMainType || maintype == g_pLogIndex[i].byMainType) && (!bJudgeSubType || subType== g_pLogIndex[i].bySubType))
			{
				pLogInfo[j] = g_pLogIndex[i];
				j++;
				//printf("========  i=%03d, bUsed=%d, mainType=%d, subType=%d,time=%lu\n", i, g_pLogIndex[i].bUsed, g_pLogIndex[i].mainType, g_pLogIndex[i].subType,g_pLogIndex[i].time);
			}
			//memcpy(&(pLogInfo[j]), &(g_pLogIndex[i]), sizeof(QMAPI_LOG_INDEX));
		}
	}
	printf("============= result log index num=%03d\n", j);
	*pNum = j;

	unsigned int allBlockLen = g_header.block_size * g_header.block_max;
	unsigned char pBlock[QMAPI_LOG_BLOCK_MAX][QMAPI_LOG_BLOCK_SIZE];

	memset(pBlock, 0, allBlockLen);
	int ret = Read_Pos(g_log_fd, g_header.offset_block, (unsigned char*)pBlock, allBlockLen);
	if(0 != ret)
	{
		printf("%s  %d, read failed\n",__FUNCTION__, __LINE__);
		*pNum = 0;
		return ;
	}

	j = 1;
	for(i=0; i<*pNum && g_header.block_size * (j+1) <= extInfoBufLen; i++)
	{
		//printf("i=%03d, bUsed=%d, mainType=%d, subType=%d,time=%lu, blockID=%d\n", i, pLogInfo[i].bUsed, pLogInfo[i].mainType, pLogInfo[i].subType,pLogInfo[i].time, pLogInfo[i].blockID);
		if(0 != pLogInfo[i].byBlockID)
		{
			//printf("newID=%03d, block:%s,   %p\n", j, pBlock[pLogInfo[i].blockID], pLogExtInfo[j]);
			memcpy(pLogExtInfo[j], &(pBlock[pLogInfo[i].byBlockID]), g_header.block_size);
			pLogInfo[i].byBlockID = j;
			j++;
		}
	}

	
	pthread_mutex_unlock(&g_pLogMutex);
}


int Get_LogExtInfo(const QMAPI_LOG_INDEX *pLogInfo, char* pBuf, unsigned int* pLen)
{
	if(NULL == pBuf || NULL == pLogInfo || 0 == *pLen)
	{
		printf("%s  %d, input is error\n",__FUNCTION__, __LINE__);
		return 1;
	}

	pthread_mutex_lock(&g_pLogMutex);
	//check QMAPI_LOG_INDEX is correct
	if(0 == pLogInfo->byBlockID)
	{
		printf("%s  %d, input is error\n",__FUNCTION__, __LINE__);
		return 1;
	}

	if(0 == g_header.block_used[pLogInfo->byBlockID])
	{
		printf("%s  %d, blockID=%d is not in used\n",__FUNCTION__, __LINE__, pLogInfo->byBlockID);
		//return 1;
	}

	//read block
	Read_Block(pLogInfo->byBlockID, pBuf, pLen);

	pthread_mutex_unlock(&g_pLogMutex);
	return 0;
}


