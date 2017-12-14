/*
 *	spv_properties.h
 *	read & write the config files
 */
#ifndef __SPV_PROPERTIES_H__
#define __SPV_PROPERTIES_H__

typedef struct Properties {
	char *key;
	char *value;
	struct Properties *pNext;
} Properties;

typedef struct PROPS_HANDLE {
	Properties *pHead;
	char *filepath;
} PROPS_HANDLE;

int trimeSpace(const char *src,char *dest);
int getCount(void *handle, int *count);

int addKey(void *handle, const char *key, const char *value);
int delKey(void *handle, const char *key);
int getKeys(void *handle, char ***keys, int *keyscount);
int freeKeys(char ***keys,int *keyscount);

int getValue(void *handle, const char *key, char *value);
int setValue(void *handle, const char *key, const char *value);
int getValues(void *handle, char ***values, int *valuescount);
int freeValues(char ***values, int *valuescount);

int releaseHandle(void **handle);
int PropertiesInit(const char *filepath, void **handle);
int saveConfig(const char *filepath,Properties *head);

#endif
