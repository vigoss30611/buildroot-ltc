/*****************************************************************************
** common/oem/config.c
**
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** Description: OEM configurations parser.
**
** Author:
**     Warits   <warits.wang@infotm.com>
**
** Revision History:
** -----------------
** 1.1  XXX 04/01/2012 XXX	draft
*****************************************************************************/


#define __U_BOOT__
#if defined(__U_BOOT__)
#include <items.h>
#include <asm-generic/errno.h>
#if defined(CONFIG_PRELOADER)
#include "preloader.h"
#define strnlen irf->strnlen
#define strncmp irf->strncmp
#define strncpy irf->strncpy
#define printf  irf->printf
#define strtol irf->simple_strtol
static struct imap_item *items = NULL;
#else /* !PRELOADER */
#define strtol simple_strtol
static struct imap_item items[ITEM_MAX_COUNT];
#endif /* PRELOADER */

#elif defined(__KERNEL__)
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/bootmem.h>
#include <mach/items.h>
#define printf(x...) printk(KERN_ERR x)
#define strtol simple_strtol
static struct imap_item items[ITEM_MAX_COUNT];
static char *items_backup  = NULL;
static int   items_backlen = 0;

#else /* not uboot */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "items.h"
static struct imap_item items[ITEM_MAX_COUNT];
#endif

#define PARSE_LIMIT 256

inline static const char *
skip_white(const char *s) { int c = 0;
	item_dbg("\n%s: ", __func__);
	for(; (*s == ' ' || *s == '\t')
			&& c < PARSE_LIMIT; s++, c++)
	  item_dbg("%c ", *s);
	return (c == PARSE_LIMIT)? NULL: s;
}

inline static const char *
skip_linef(const char *s) { int c = 0;
	for(; (*s == LINEF0 || *s == LINEF1) &&
				c < PARSE_LIMIT; s++, c++)
	  item_dbg("%c ", *s);
	return (c == PARSE_LIMIT)? NULL: s;
}

inline static const char *
skip_line(const char *s) { int c = 0;
	item_dbg("\n%s: ", __func__);
	for(; *s != LINEF0 && *s != LINEF1 &&
				c < PARSE_LIMIT; s++, c++)
	  item_dbg("%c ", *s);
	return (c == PARSE_LIMIT)? NULL: skip_linef(s);
}

static const char *__copy(char *buf, const char *src) {
	int c = 0;

	item_dbg("\n%s: ", __func__);
	for(; *src != ' ' && *src != '\t'
				&& *src != LINEF0 && *src != LINEF1
				&& c < PARSE_LIMIT;
				src++, buf++, c++) {
		*buf = *src;
		item_dbg("%c ", *buf);
	}

	*buf = '\0';

	return src;
}

static const char *_parse_line(struct imap_item *t, const char *s)
{
	s = skip_white(s);
	if(s) s = skip_linef(s);
	if(s && *s != '#') {
		if(strncmp(s, "items.end", 9) == 0)
		  return NULL;
		/* check for key */
		s = __copy(t->key, s);
		if(s) {
			s = skip_white(s);
			if(*s == LINEF0 || *s == LINEF1)
			  goto skip;
			if(s) s = skip_linef(s);
			/* check for string */
			s = __copy(t->string, s);
		}
	}
skip:
	s = skip_line(s);
	return s;
}

#if defined(__KERNEL__) && !defined(__U_BOOT__)
int item_export(char *buf, int len)
{
	int _l = (len > items_backlen)? items_backlen:
		len;
	memcpy(buf, items_backup, _l);

	return _l;
}
#endif

int item_init(char *mem, int len)
{
	const char *s = mem;
	int i;

	item_dbg("%s\n", __func__);
#if defined(CONFIG_PRELOADER)
	items = (void *)irf->malloc(ITEM_SIZE_EMBEDDED);
    if (NULL == items) {
        printf("Items init error \n");
        return -1;
    }
	irf->memset(items, 0, ITEM_SIZE_EMBEDDED);
	for(i = 0; i < ITEM_MAX_COUNT_PRELOADER && s < mem + len;)
#else
	memset(items, 0, sizeof(struct imap_item) * ITEM_MAX_COUNT);
	for(i = 0; i < ITEM_MAX_COUNT
				&& s < mem + len;)
#endif
	{
		items[i].key[0] = items[i].string[0] = 0;
		s = _parse_line(&items[i], s);

		if(!s) break;
		if(!items[i].key[0])
		  continue ;

		i++;
	}

#if defined(__KERNEL__) && !defined(__U_BOOT__)
	items_backup = alloc_bootmem_low((len + 0xfff) & ~0xfff);

	if(!items_backup)
	  printf("failed to allocate memory for backup\n");
	else {
		memcpy(items_backup, mem, len);
		items_backlen = s - mem;
	}
#endif

	if(i == ITEM_MAX_COUNT)
	  printf("truncated config items: %d\n", i);


    printf("config_items: %d \n", i);

    if (item_exist("board.disk")) {
        printf("board.disk exist \n");
    } else {
        printf("board.disk doesn't has \n");
    }

	return i;
}

#if !defined(CONFIG_PRELOADER)
void item_list(const char *subset)
{
	struct imap_item *t = items;

	for(; t->key[0]; t++)
	  if(!subset || strncmp(t->key, subset,
		  strnlen(subset, ITEM_MAX_LEN)) == 0)
		printf("%3d: %-24s  %s\n", (t - items), t->key, t->string);
}
#endif

int item_exist(const char *key)
{
	struct imap_item *t = items;

	for(; t->key[0]; t++)
	  if(strncmp(t->key, key, strnlen(key, ITEM_MAX_LEN)) == 0)
		return 1;

	/* not found */
	return 0;
}

const char *__string_sec(const char *str, int sec)
{
	const char *p = str;
	int c = 0;

	for(; *p != '\0' && c <= sec; p++) {
		if(*p == '.') {c++; p++; }
		if( c == sec)  return p;
	}

	return NULL;
}

int item_string(char *buf, const char *key, int section)
{
	struct imap_item *t = items;
	const char *s = NULL;
	char *p;
	int c = 0, flag = 0;

	for(; t->key[0]; t++) {
		if(strncmp(t->key, key, ITEM_MAX_LEN) == 0) {
			//s = __string_sec(t->string, section);
			c = 0;
			flag = 0;
			p = t->string;
			s = t->string;
			while (*p != '\0') {
				if (*p == '.')
					c++;
				if (c == (section + 1)) {
					flag = 1;
					break;
				}
				if (*p == '.')
					s = p + 1;
				p++;
			}
			if ((flag || (section == c)) && (p != s)) {
				irf->memset(buf, 0, (unsigned)p - (unsigned)s + 1);
				strncpy(buf, s, (unsigned)p - (unsigned)s);
				return 0;
			} else if (section == 0) {
				strncpy(buf, s, ITEM_MAX_LEN);
				return 0;
			}
		}
	}

	return -EFAULT;
}

int item_integer(const char *key, int section)
{
	char buf[ITEM_MAX_LEN];
	int ret;

	ret = item_string(buf, key, section);
	return ret? -ITEMS_EINT: strtol(buf, NULL, 10);
}

int item_equal(const char *key, const char *value, int section)
{
	char buf[ITEM_MAX_LEN];
	int ret;

	ret = item_string(buf, key, section);
#if 0
	if (ret)
	{
		printf("-->read items %s fail\n", key);
	}
	else
	{
		printf("-->items %s value is %s\n", key, buf);
	}
#endif
	return ret? 0: !strncmp(buf, value, strnlen(value, ITEM_MAX_LEN));
}

#if !defined(CONFIG_PRELOADER)
int item_string_item(char *buf, const char *string, int section)
{
	struct imap_item *t = items;
	const char *s;

	for(; t->key[0]; t++)
	  if(strncmp(t->string, string, ITEM_MAX_LEN) == 0) {
		  s = __string_sec(t->key, section);
		  if(s) {
			  strncpy(buf, s, ITEM_MAX_LEN);
			  return 0;
		  }
	  }

	return -EFAULT;
}
#endif

