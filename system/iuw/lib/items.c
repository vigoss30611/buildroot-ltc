#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <items.h>

static int   items_backlen = 0;
static struct imap_item items[ITEM_MAX_COUNT];

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

static char *_parse_line(struct imap_item *t, char *s)
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

int item_init(char *mem, int len)
{
	char *s = mem;
	int i;

	memset(items, 0, sizeof(struct imap_item) * ITEM_MAX_COUNT);
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
	return ret? 0: !strncmp(buf, value, ITEM_MAX_LEN);
}

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

