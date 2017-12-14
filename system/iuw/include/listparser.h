#ifndef __IUW_LIST_PARSER_H__
#define __IUW_LIST_PARSER_H__

enum list_type {
	LIST_CONF = 0,
	LIST_EXEC,
	LIST_STRW,
	LIST_STNB,
	LIST_STNR,
	LIST_STNF,
	LIST_END,
};

extern const char *list_names[];

struct list_desc {
	uint32_t	info0;
	uint32_t	info1;
	uint32_t	length;
	uint32_t    offset;
    uint8_t		type;
	char		path[1024];
};

extern int list_get_info(struct list_desc *t, int n);
extern int list_init(const char *lst);

#endif /* __IUW_LIST_PARSER_H__ */
