
#ifndef _IMAP_ITEMS_H_
#define	_IMAP_ITEMS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ITEM_MAX_LEN   64
#define ITEM_MAX_COUNT 200

#define ITEM_SIZE_EMBEDDED		0x0002000
#define ITEM_SIZE_NORMAL		0x0004000

#define LINEF0     10
#define LINEF1     13

int item_init(char *mem, int len);
void item_list(const char *subset);
int item_exist(const char *item);
int item_equal(const char *item, const char *value, int section);
int item_string(char *buf, const char *item, int section);
int item_integer(const char *item, int section);
int item_export(char *buf, int len);

int item_string_item(char *buf, const char *item, int section);

int init_config(void);

/* not implemented in uboot0 */
//const char *item_string_find(const char *str);

struct imap_item {

	char key[ITEM_MAX_LEN];
	char string[ITEM_MAX_LEN];
};

//#define item_dbg(x...) printf(x)
#define item_dbg(x...)

#define ITEMS_EINT  9999
#define ITEMS_MAJOR 167
#define ITEMS_MAGIC 'i'
#define ITEMS_IOCMAX 10
#define ITEMS_NAME "items"
#define ITEMS_NODE "/dev/items"

#define ITEMS_EXIST		_IOR(ITEMS_MAGIC, 1, unsigned long)
#define ITEMS_STRING		_IOR(ITEMS_MAGIC, 2, unsigned long)
#define ITEMS_INTEGER		_IOR(ITEMS_MAGIC, 3, unsigned long)
#define ITEMS_EQUAL		_IOR(ITEMS_MAGIC, 4, unsigned long)
#define ITEMS_ITEM		_IOR(ITEMS_MAGIC, 5, unsigned long)
#define ITEMS_REINIT		_IOR(ITEMS_MAGIC, 6, unsigned long)
#define ITEMS_GETTR		_IOR(ITEMS_MAGIC, 7, unsigned long)
#define ITEMS_LOWBASE		0x3c000000

struct items_query {

	char   key[ITEM_MAX_LEN];
	char value[ITEM_MAX_LEN];
	char    fb[ITEM_MAX_LEN];
	int    section;
};
  


#ifdef __cplusplus
}
#endif

#endif /* _IMAP_ITEMS_H_ */

