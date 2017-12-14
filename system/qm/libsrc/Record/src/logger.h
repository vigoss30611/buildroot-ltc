#ifndef __LOGGER_H__
#define __LOGGER_H__


/*******************************************************/
/* 相关宏和结构定义                                    */
/*******************************************************/

#define LOG_TEXT_LEN 36
#define LOG_FILE_SIZE (1024*48)
#define CLR_PAGE_SIZE (56*48)

typedef struct {
	unsigned char log_type; /* 普通、计划、动检、报警、 */
	unsigned char log_opt; /* 1<<0 录像、 1<<1 抓图*/
	unsigned char log_ch; /* 通道 */
	unsigned char cres; /* 保留为0 */
	time_t log_time; /* 记录时间 */
	char log_text[LOG_TEXT_LEN]; /* 日志内容 */
	int ires;
} log_content_t;

typedef struct {
	unsigned char log_type; /* 检索类型 */
	unsigned char log_ch;
	unsigned char log_opt;
	unsigned char cres;
	time_t log_begin; /* 开始时间 */
	time_t log_end; /* 结束时间 */
} log_search_t;

typedef struct {
	int log_pos;
	FILE* log_pf;
	int log_mode;
	char log_name[LOG_TEXT_LEN];
}log_head_t;

/*******************************************************/
/* 如无特别说明,函数返回LIB_OK表示成功,LIB_ERR表示失败 */
/*******************************************************/

/* 指定日志存储路径并初始化 */

log_head_t* log_init(const char *logpath, int mode);

/* 反初始化并退出 */
int log_exit(log_head_t *head);

/* 写入日志记录，返回写入记录数 */
int log_write(log_head_t *head, log_content_t *pcontent, int num);

/*日志记录查询，返回符合条件索引数*/
unsigned int log_inquery(log_head_t *head, log_search_t *psearch);

/* 日志记录检索，返回符合条件的总记录数 */
int log_search(log_head_t *head, log_search_t *psearch);

int log_read(log_head_t *head, log_search_t *psearch, log_content_t *pcont, int num);

/* 清除所有日志记录 */
int log_clear(log_head_t *head);

#endif /* __LOGGER_H__ */
