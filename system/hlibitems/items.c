/* items.c
 *
 * item operate
 *
 * Copyright (c) 2014 Shanghai InfoTMIC Co.,Ltd.
 *
 * Version:
 * 1.0  Created.
 *      xecle, 03/18/2014 10:19:40 AM
 *
 */
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "items.h"

int query(struct item_query *q, int io)
{
	int fd;
	int ret;
	fd = open(ITEMS_NODE, O_RDONLY);
	if (fd < 0) {
		printf("ERROR: Can not open device %s\n", ITEMS_NODE);
		return -ENODEV;
	}
	ret = ioctl(fd, io, q);
	close(fd);
	return ret;
}


int item_string(char *value, const char *key, int section)
{
	int len;
	int ret;
	struct item_query q;
	len = strlen(key);
	strncpy(q.key, key, len>=ITEM_MAX_LEN?ITEM_MAX_LEN:len+1);
	q.section = section;
	ret = query(&q, ITEMS_STRING);
	len = strlen(q.fb);
	//printf("***query is %x fb is %x ret %d, len %d***\n", (unsigned long*)&q, q.value, ret, len);
	strncpy(value, q.fb, len>=ITEM_MAX_LEN?ITEM_MAX_LEN:len+1);
	return ret;
}
int item_integer(const char *key, int section)
{
	int len;
	int ret;
	struct item_query q;
	len = strlen(key);
	strncpy(q.key, key, len>=ITEM_MAX_LEN?ITEM_MAX_LEN:len+1);
	q.section = section;
	ret = query(&q, ITEMS_INTEGER);
	return ret;
}
int item_equal(const char *key, const char *value, int section)
{
	int len;
	int ret;
	struct item_query q;
	len = strlen(key);
	strncpy(q.key, key, len>=ITEM_MAX_LEN?ITEM_MAX_LEN:len+1);
	len = strlen(value);
	strncpy(q.value, value, len>=ITEM_MAX_LEN?ITEM_MAX_LEN:len+1);
	q.section = section;
	ret = query(&q, ITEMS_EQUAL);
	return ret;
}
int item_exist(const char *key)
{
	int len;
	int ret;
	struct item_query q;
	len = strlen(key);
	strncpy(q.key, key, len>=ITEM_MAX_LEN?ITEM_MAX_LEN:len+1);
	ret = query(&q, ITEMS_EXIST);
	return ret;
}
