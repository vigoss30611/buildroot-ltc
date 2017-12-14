#ifndef SD_STUFF_H
#define SD_STUFF_H

//__BEGIN__DECLS
#ifdef __cplusplus
extern "C" {
#endif

#define ERROR_FGETS	-1

#define SD_FORMAT_OK	0
#define SD_FORMAT_ERROR	-1

#define SD_MOUNTED	1
#define SD_UNMOUNTED	0

#define SD_WRITABLE	1
#define SD_UNWRITABLE	0

int sd_format(void);
int is_sd_node(void);
int is_sd_mounted(void);

#ifdef __cplusplus
}
#endif
//__END__DECLS

#endif
