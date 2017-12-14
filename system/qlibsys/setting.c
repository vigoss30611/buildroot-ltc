#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include "cJSON.h"
#include "sys.h"

#define ROOT "/config/settings"
#define DESCLEN 0x4000
#define PATHLEN 256

extern int ftruncate(int, off_t);
static pthread_mutex_t tlock = PTHREAD_MUTEX_INITIALIZER;
static int fd = -1;

static int Init(const char *owner)
{
	char path[PATHLEN];

	if(fd > 0) return 0;
	sprintf(path, "%s/%s", ROOT, owner);
	mkdir(ROOT, 0777);
	mkdir(path, 0777);

	strcat(path, "/.json");
	return fd = open(path, O_CREAT | O_RDWR | O_SYNC, 0666);
}

static cJSON *GetJson(const char *owner, int lock) {
	char *desc = malloc(DESCLEN);
	cJSON *js = NULL;
	int ret;

	if(Init(owner) < 0) goto end;

	fflush(stdout);
	flock(fd, lock);
	pthread_mutex_lock(&tlock);

	lseek(fd, 0, SEEK_SET);
	ret = read(fd, desc, DESCLEN);
	if(ret > 0 && ret < DESCLEN)
		js = cJSON_Parse(desc);

	if(!js) {
//		printf("cannot parse setting file (%d), create a new one\n", ret);
		js = cJSON_CreateObject();
	}

end:
	free(desc);
	return js;
}

static void PutJson(cJSON *js, int lock)
{
	int tmp = fd;
	if(lock != LOCK_EX) goto out;

	char *desc = cJSON_PrintUnformatted(js);
	cJSON_Delete(js);
	lseek(fd, 0, SEEK_SET);
//	printf(">> %s\n", desc);
	ftruncate(fd, write(fd, desc, strlen(desc)));
	free(desc);

out:
	fd = -1;
	fflush(stdout);
	pthread_mutex_unlock(&tlock);
	close(tmp);
}

#define get_setting(l, t, v) \
	t ret = v; \
	cJSON *js = GetJson(owner, LOCK_EX);  \
	if(!js) goto err; \
	cJSON *ks = cJSON_GetObjectItem(js, key);  \
	int lock = l;  

#define put_setting(l, r)  \
	ret = r; lock = l; \
err:  \
	PutJson(js, lock);  \
	return ret

const char * setting_get_string(const char *owner, const char *key,
								char *buf, int len) {
	get_setting(LOCK_SH, const char *, NULL);
	if(!ks || !ks->valuestring) buf = NULL;
	else strncpy(buf, ks->valuestring, len);
	put_setting(LOCK_SH, buf);
}

int setting_set_string(const char *owner, const char *key,
					   const char *value) {
	get_setting(LOCK_EX, int, -1);
	if (js) {
		if(ks) cJSON_ReplaceItemInObject(js, key, cJSON_CreateString(value));
		else cJSON_AddStringToObject(js, key, value);
	}
	put_setting(LOCK_EX, 0);
}

double setting_get_double(const char *owner, const char *key) {
	get_setting(LOCK_SH, double, SETTING_EINT);
	put_setting(LOCK_SH, ks? ks->valuedouble: SETTING_EINT);
}

int setting_set_double(const char *owner, const char *key, double value) {
	get_setting(LOCK_EX, int, -1);
	if(ks) cJSON_ReplaceItemInObject(js, key, cJSON_CreateNumber(value));
	else cJSON_AddNumberToObject(js, key, value);
	put_setting(LOCK_EX, 0);
}

int setting_get_int(const char *owner, const char *key) {
	get_setting(LOCK_SH, int, SETTING_EINT);
	put_setting(LOCK_SH, ks? ks->valueint: SETTING_EINT);
}

int setting_set_int(const char *owner, const char *key, int value) {
	return setting_set_double(owner, key, (int)value);
}

int setting_remove_key(const char *owner, const char *key) {
	get_setting(LOCK_EX, int, -1);
	if(ks) cJSON_DeleteItemFromObject(js, key);
	put_setting(LOCK_EX, 0);
}

int setting_remove_all(const char *owner) {
	const char *key = "nothing";
	get_setting(LOCK_EX, int, -1);
	cJSON_Delete(ks = js);
	js = cJSON_CreateObject();
	put_setting(LOCK_EX, 0);
}

