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


#if defined(__U_BOOT__)
#include <common.h>
#include <items.h>
#include <asm-generic/errno.h>
#if defined(CONFIG_PRELOADER)
#include "preloader.h"
#define strnlen irf.strnlen
#define strncmp irf.strncmp
#define strncpy irf.strncpy
#define printf  irf.printf
#define strtol irf.simple_strtol
#endif /* PRELOADER */
static struct imap_item *items = (void *)(GRAM_BASE_PA + BL_CFG_NORMAL);

#elif defined(__KERNEL__)
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/bootmem.h>
#include <mach/items.h>
#define printf(x...) printk(KERN_ERR x)
#define strtol simple_strtol
static struct imap_item items[ITEM_MAX_COUNT];
static char *items_backup  = NULL, *items_mem;
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

inline static const char *
cut_string(char *s) { char *p = s, c = 0;
    for(; *p && *p != '.' && *p != LINEF0 && *p != LINEF1
            && c < ITEM_MAX_LEN; p++, c++);
    *p = 0;
    return s;
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
		if(strncmp(s, "items.end", 9) == 0) {
			items_backlen = (uint32_t)s - (uint32_t)items_mem + 10;
			return NULL;
		}
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
static int item_pos = 0;

int item_export(char *buf, int len)
{
	int _l;

	if(!buf) {
	    item_pos = 0;
	    return 0;
	}

	if(item_pos >= items_backlen)
	    return 0;

	_l = (len > (items_backlen - item_pos))?
	    (items_backlen - item_pos): len;

	if(_l) {
	    memcpy(buf, items_backup + item_pos, _l);
	    item_pos += _l;
	}

	return _l;
}
#endif

int __init item_init(char *mem, int len)
{
	const char *s = mem;
	int i;
	
	items_mem    = mem;
	printk("items_mem is 0x%p\n", items_mem);
	for(i = 0; i < ITEM_MAX_COUNT
			&& s < mem + len;)
	{
		items[i].key[0] = items[i].string[0] = 0;

		s = _parse_line(&items[i], s);
		if(!s) break;
		if(!items[i].key[0])
		  continue ;

		i++;
	}

#if defined(__KERNEL__) && !defined(__U_BOOT__)
	if(!items_backup) {
		items_backup = alloc_bootmem_low((len + 0xfff) & ~0xfff);

		if(!items_backup)
			printf("failed to allocate memory for backup\n");
		else {
			memcpy(items_backup, mem, len);
			printf("items length: %d\n", items_backlen);
		}
	}
#endif
	if(i == ITEM_MAX_COUNT)
	  printf("truncated config items: %d\n", i);

	return i;
}

void item_list(const char *subset)
{
	struct imap_item *t = items;

	for(; t->key[0]; t++)
	  if(!subset || strncmp(t->key, subset,
		  strnlen(subset, ITEM_MAX_LEN)) == 0)
		printf("%3d: %-24s  %s\n", (t - items), t->key, t->string);
}

int item_exist(const char *key)
{
	struct imap_item *t = items;
        int len = strnlen(key, ITEM_MAX_LEN);

	for(; t->key[0]; t++)
            if(strncmp(t->key, key, len) == 0 &&
                    (t->key[len] == 0 || t->key[len] == '.'))
		return 1;

	/* not found */
	return 0;
}

EXPORT_SYMBOL(item_exist);

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
	const char *s;
    int cut = 1;
    if(section < 0)
        section = cut = 0;

	for(; t->key[0]; t++)
	  if(strncmp(t->key, key, ITEM_MAX_LEN) == 0) {
		  s = __string_sec(t->string, section);
		  if(s) {
			  strncpy(buf, s, ITEM_MAX_LEN);
              if(cut)cut_string(buf);
			  return 0;
		  }
	  }

	return -EFAULT;
}
EXPORT_SYMBOL(item_string);

int item_integer(const char *key, int section)
{
	char buf[ITEM_MAX_LEN];
	int ret;

	ret = item_string(buf, key, section);
	return ret? -ITEMS_EINT: strtol(buf, NULL, 10);
}
EXPORT_SYMBOL(item_integer);

static int notp = 0;
void tp_disable(void) {
	notp = 1;
}
EXPORT_SYMBOL(tp_disable);

int item_equal(const char *key, const char *value, int section)
{
	char buf[ITEM_MAX_LEN];
	int ret;

	if(notp && strncmp(key, "ts.model", 8) == 0)
		return 0;

	ret = item_string(buf, key, section);
	return ret? 0: !strncmp(buf, value, ITEM_MAX_LEN);
}

EXPORT_SYMBOL(item_equal);

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

int item_sync(struct imap_item *string)
{
    struct imap_item *t = items;
    int items_len = 0;
    int num = 0;
    int repalce_status = 0;

    for (; t->key[0]; t++) {
        items_len++;
    }

    for (; string->key[0]; string++) {
        t = items;
        num = 0;
        repalce_status = 0;

        for (; t->key[0]; t++, num++) {
            if (strncmp(t->key, string->key, ITEM_MAX_LEN) == 0) {
                memset(t->string, 0 , ITEM_MAX_LEN);
                strcpy(t->string, string->string);
                repalce_status = 1;
            }
        }

        if (num == items_len && repalce_status != 1) {
            strcpy(t->key, string->key);
            strcpy(t->string, string->string);
            items_len++;
        }
    }

    return 0;
}


EXPORT_SYMBOL(item_sync);
