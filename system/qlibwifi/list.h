#ifndef _LIST_H_
#define _LIST_H_

#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include "wifi.h"

typedef struct wifi_list_head list_head_t;
#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

#define list_for_each(pos, head) \
	for (pos = (head)->next ; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, npos, head) \
	for (pos = (head)->next, npos = pos->next ; pos != (head); pos = npos, npos = pos->next)

static void __list_add(list_head_t * _new, list_head_t * prev, list_head_t * next)
{
	next->prev = _new;
	_new->next = next;
	_new->prev = prev;
	prev->next = _new;
}

static void list_add_tail(list_head_t *_new, list_head_t *head)
{
	__list_add(_new, head->prev, head);
}

static void __list_del(list_head_t * prev, list_head_t * next)
{
	next->prev = prev;
	prev->next = next;
}

static void list_del(list_head_t *entry)
{
	__list_del(entry->prev, entry->next);
}

static int list_empty(list_head_t *head)
{
	return head->next == head;
}

#endif
