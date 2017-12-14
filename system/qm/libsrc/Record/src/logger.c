
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "logger.h"

#define LOG_MAX_NUM	( LOG_FILE_SIZE / sizeof(log_content_t) ) /* 总记录数 */
#define CLR_LOG_NUM	( CLR_PAGE_SIZE / sizeof(log_content_t) ) /* 每次覆盖的记录数 */

/* 日志检索类型和时间匹配 */
#define LOG_SEARCH_MATCH(psearch, plog) (((plog)->log_type & (psearch)->log_type) != 0 && \
										(plog)->log_time >= (psearch)->log_begin && \
										(plog)->log_time <= (psearch)->log_end)

log_head_t* log_init(const char *logpath, int mode)
{
	log_head_t *head = NULL;
	
	if (LOG_MAX_NUM <= CLR_LOG_NUM)
	{
		return NULL;
	}
	head = malloc(sizeof(log_head_t));
	if (!head)
	{
		return NULL;
	}
	sprintf(head->log_name, logpath);
	head->log_mode = mode;
	head->log_pos = 0;
	do {
		if (mode)
		{
			head->log_pf = fopen(logpath, "r+b");
			if (head->log_pf != NULL)
			{
				if (fseek(head->log_pf, 0L, SEEK_END) != -1)
				{
					long lsize = ftell(head->log_pf);
					if (lsize)
					{
						lsize /= sizeof(log_content_t);
						if (lsize < LOG_MAX_NUM)
						{
							head->log_pos = lsize;
							break;
						}
					}
				}
				fclose(head->log_pf);
			}
			/* 创建日志文件 */
			head->log_pf = fopen(logpath, "w+b");
			if (head->log_pf != NULL)
			{
				break;
			}
		}
		else
		{
			head->log_pf = fopen(logpath, "r+b");
			if (head->log_pf != NULL)
			{
				break;
			}
		}
		return NULL;
	} while(0);
	
	return head;
}

int log_exit(log_head_t *head)
{
	if (head)
	{
		if (head->log_pf)
		{
			if (head->log_mode)
			{
				fflush(head->log_pf);
			}
			fclose(head->log_pf);
		}
		free(head);
	}
	return 0;
}

int log_write(log_head_t *head, log_content_t *pcont, int num)
{
	int r = 0;
	if (head->log_pf && head->log_mode && pcont)
	{
		long curr_offset;
		/* 定位到当前写位置 */
		curr_offset = head->log_pos * sizeof(log_content_t);
		if (fseek(head->log_pf, curr_offset, SEEK_SET) != -1)
		{
			int i;
			for (i=0; i<num; i++)
			{
				/* 写入记录 */
				if (fwrite(&pcont[i], sizeof(log_content_t), 1, head->log_pf) == 1)
				{
					head->log_pos++;
					r++;
				}
				else
				{
					perror("log_write:");
					break;
				}
			}
		}
		fflush(head->log_pf);
	}
	return r;
}

int log_search(log_head_t *head, log_search_t *psearch)
{
	int r = 0, pos = 0;
	int startpos = 0;
	if (head && head->log_pf && psearch)
	{
		if (fseek(head->log_pf, 0L, SEEK_SET) != -1)
		{
			int i;
			log_content_t cont;
			for (i=0; i<LOG_MAX_NUM; i++)
			{
				if (fread(&cont, sizeof(log_content_t), 1, head->log_pf) == 1)
				{
					/* 日志匹配 */
					if (LOG_SEARCH_MATCH(psearch, &cont))
					{
						r++;
						if (startpos == 0)
						{
							head->log_pos = pos;
							startpos = 1;
						}
					}
				}
				else
				{
					break;
				}
				pos += 1;
			}
		}
	}
	return r;
}

unsigned int log_inquery(log_head_t *head, log_search_t *psearch)
{
	unsigned int r = 0;
	if (head && head->log_pf && psearch)
	{
		if (fseek(head->log_pf, 0L, SEEK_SET) != -1)
		{
			int i,range;
			log_content_t cont;
			int dat_2_seconds = 24 * 60 * 60;
			
			for (i=0; i<LOG_MAX_NUM; i++)
			{
				if (fread(&cont, sizeof(log_content_t), 1, head->log_pf) == 1)
				{
					/* 日志匹配 */
					if (LOG_SEARCH_MATCH(psearch, &cont))
					{
						range = cont.log_time / dat_2_seconds - psearch->log_begin /dat_2_seconds;
						r |= 1 << range;
					}
				}
				else
				{
					perror("log_search:");
					break;
				}
			}
		}
	}
	return r;
}

int log_read(log_head_t *head, log_search_t *psearch, log_content_t *pcont, int num)
{
	int r = 0;
	if (head && head->log_pf && pcont && num > 0)
	{
		int i, pos;
		long loffset;

		pos = head->log_pos;
		loffset = pos * sizeof(log_content_t);
		if (fseek(head->log_pf, loffset, SEEK_SET) != -1)
		{
			for (i=0; i<LOG_MAX_NUM; i++)
			{
				if (fread(pcont + r, sizeof(log_content_t), 1, head->log_pf) == 1)
				{
					/* 日志匹配 */
					if (LOG_SEARCH_MATCH(psearch, &pcont[r]))
					{
						if (++r >= num)
						{
							break;
						}
					}
				}
				else
				{
					break;
				}
				pos += 1;
			}
			head->log_pos = pos;
		}
	}
	return r;
}

int log_clear(log_head_t *head)
{
	int r = -1;
	if (head && head->log_pf)
	{
		if (fseek(head->log_pf, 0L, SEEK_SET) != -1)
		{
			int i;
			log_content_t cont;
			/* 清除日志文件 */
			memset(&cont, 0, sizeof(log_content_t));
			for (i=0; i<LOG_MAX_NUM; i++)
			{
				fwrite(&cont, sizeof(log_content_t), 1, head->log_pf);
			}
			fflush(head->log_pf);
			head->log_pos = 0;
			r = 0;
		}
	}
	return r;
}



