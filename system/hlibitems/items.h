/* items.h
 *
 * item header file
 *
 * Copyright (c) 2014 Shanghai InfoTMIC Co.,Ltd.
 *
 * Version:
 * 1.0  Created.
 *      xecle, 03/18/2014 09:39:34 AM
 *
 */
#ifndef _ITEMS_H_
#define _ITEMS_H_

#define ITEM_MAX_LEN   64

int item_exist(const char *key);
int item_equal(const char *key, const char *value, int section);
int item_string(char *value, const char *key, int section);
int item_integer(const char *key, int section);
int item_export(char *value, int len);
int item_string_item(char *value, const char *key, int section);

#define ITEMS_NODE	"/dev/items"
#define ITEMS_MAGIC	'i'
#define ITEMS_EXIST	_IOR(ITEMS_MAGIC, 1, unsigned long)
#define ITEMS_STRING	_IOR(ITEMS_MAGIC, 2, unsigned long)
#define ITEMS_INTEGER	_IOR(ITEMS_MAGIC, 3, unsigned long)
#define ITEMS_EQUAL	_IOR(ITEMS_MAGIC, 4, unsigned long)
#define ITEMS_ITEM	_IOR(ITEMS_MAGIC, 5, unsigned long)

struct item_query {
	char key[ITEM_MAX_LEN];
	char value[ITEM_MAX_LEN];
	char fb[ITEM_MAX_LEN];
	int section;
};

#endif /* _ITEMS_H_ */
