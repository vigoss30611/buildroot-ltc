/*
 * items_unit.c
 *
 *  Created on: Apr 20, 2017
 *      Author: zhaomh
 */
#include "items_utils.h"

struct path_item {
    char key[PATH_ITEM_MAX_LEN];
    char string[PATH_ITEM_MAX_LEN];
};

struct imap_item sensor_items[SENSOR_ITEM_MAX_COUNT];
struct path_item path_items[SENSOR_ITEM_MAX_COUNT];


static const char *skip_space(const char *s)
{
    int c = 0;

    for(; (*s == ' ' || *s == '\t') && c < PARSE_LIMIT; s++, c++)
        parse_db("%c ", *s);
    return (c == PARSE_LIMIT)? NULL: s;
}

static const char *skip_linef(const char *s)
{
    int c = 0;

    for(; (*s == LINEF0 || *s == LINEF1) && c < PARSE_LIMIT; s++, c++)
        parse_db("%c ", *s);
    return (c == PARSE_LIMIT)? NULL: s;
}

static const char *skip_line(const char *s)
{
    int c = 0;

    for(; *s != LINEF0 && *s != LINEF1 && c < PARSE_LIMIT; s++, c++)
        parse_db("%c ", *s);
    return (c == PARSE_LIMIT)? NULL: skip_linef(s);
}

static const char *copy(char *buf, const char *src)
{
    int c = 0;

    for(; *src != ' ' && *src != '\t' && *src != LINEF0 && *src != LINEF1
            && c < PARSE_LIMIT; src++, buf++, c++) {
        *buf = *src;
    }

    *buf = '\0';

    return src;
}

static const char *parse_item_file(struct imap_item *t, const char *s)
{
    s = skip_space(s);
    if (s) {
        s = skip_linef(s);
    }

    if (s && *s != '#') {
        s = copy(t->key, s);
        if (s) {
            s = skip_space(s);
            if (*s == LINEF0 || *s == LINEF1)
                goto skip;
            if(s) s = skip_linef(s);
            s = copy(t->string, s);
        }
    }

skip:
    s = skip_line(s);
    return s;
}

static const char *parse_path_item_file(struct path_item *t, const char *s)
{
    s = skip_space(s);
    if (s) {
        s = skip_linef(s);
    }

    if (s && *s != '#') {
        s = copy(t->key, s);
        if (s) {
            s = skip_space(s);
            if (*s == LINEF0 || *s == LINEF1)
                goto skip;
            if(s) s = skip_linef(s);
            s = copy(t->string, s);
        }
    }

skip:
    s = skip_line(s);
    return s;
}


static char *parse_path_info(const char *key)
{
    struct path_item *t = path_items;
    int len = strnlen(key, ITEM_MAX_LEN);

    for (; t->key[0]; t++) {
        if(strncmp(t->key, key, len) == 0 &&
                (t->key[len] == 0 || t->key[len] == '.'))
            return t->string;
    }

    return NULL;
}

static int parse_sensor_attr_info(void)
{
    int num = 0;
    char tmp[ITEM_MAX_LEN];
    char *path = NULL;

    for (num = 0; num < MAX_SENSORS_NUM; num++) {
        memset(&sensor_attr_info[num], 0, sizeof(struct _sensor_sttributes));

        memset(tmp, 0, sizeof(tmp));
        sprintf(tmp, "%s%d.%s", "sensor", num, "item");
        path = parse_path_info(tmp);
        if (path) {
            strcpy(sensor_attr_info[num].sensor_item_path, path);
        }

        memset(tmp, 0, sizeof(tmp));
        sprintf(tmp, "%s%d.%s", "sensor", num, "config");
        path = parse_path_info(tmp);
        if (path) {
            strcpy(sensor_attr_info[num].sensor_config_path, path);
        }

        memset(tmp, 0, sizeof(tmp));
        sprintf(tmp, "%s%d.%s", "sensor", num, "isp");
        path = parse_path_info(tmp);
        if (path) {
            strcpy(sensor_attr_info[num].isp_config_path, path);
        }

        memset(tmp, 0, sizeof(tmp));
        sprintf(tmp, "%s%d.%s", "sensor", num, "lsh");
        path = parse_path_info(tmp);
        if (path) {
            strcpy(sensor_attr_info[num].lsh_config_path, path);
        }
    }

    return 0;
}

static int path_item_init(const char *mem)
{
    int i = 0;
    const char *s = mem;

    memset(path_items, 0, sizeof(struct path_item) * SENSOR_ITEM_MAX_COUNT);
    for (i = 0; i < SENSOR_ITEM_MAX_COUNT; i++) {
        s = parse_path_item_file(&path_items[i], s);
        if (!s) break;
    }

    return 0;
}

static int replace_file(const char * dst, const char *src)
{
    struct file *src_fp = NULL;
    struct file *dst_fp = NULL;
    mm_segment_t old_fs = get_fs();
    loff_t src_pos = 0;
    loff_t dst_pos = 0;
    loff_t offset = 0, i = 0;
    char data = 0;

    src_fp = filp_open(src, O_RDWR,0644);
    if (IS_ERR(src_fp)) {
        printk("replace_file open src file failed\n");
        return -1;
    }

    dst_fp = filp_open(dst, O_CREAT | O_RDWR | O_TRUNC,0644);
    if (IS_ERR(dst_fp)) {
        printk("replace_file open dst file failed\n");
        return -1;
    }

    set_fs(KERNEL_DS);

    offset = vfs_llseek(src_fp, 0, SEEK_END);

    for (i = 0; i < offset; i++) {
        vfs_read(src_fp, &data, sizeof(char), &src_pos);
        vfs_write(dst_fp, &data, sizeof(char), &dst_pos);
    }

    filp_close(src_fp, NULL);
    filp_close(dst_fp, NULL);

    set_fs(old_fs);

    return 0;
}

static int sensor_item_init(const char *mem)
{
    int i = 0;
    const char *s = mem;

    memset(sensor_items, 0, sizeof(struct imap_item) * SENSOR_ITEM_MAX_COUNT);
    for (i = 0; i < SENSOR_ITEM_MAX_COUNT; i++) {
        s = parse_item_file(&sensor_items[i], s);
        if (!s) break;
    }

    return 0;
}

static int ddk_parse_ext_path_item(void)
{
    char path[128];
    struct file *fp = NULL;
    mm_segment_t old_fs = get_fs();
    loff_t offset = 0;
    char *buf = NULL;
    loff_t pos = 0;
    int num = 0;

    for (num = 0; num < MAX_SENSORS_NUM; num++) {
        memset(&sensor_attr_info[num], 0, sizeof(struct _sensor_sttributes));
    }

    memset((void *)path, 0, sizeof(path));
    sprintf(path, "%s",DEFAULT_PATH);

    fp = filp_open(path, O_RDWR,0644);
    if (IS_ERR(fp)) {
        printk("no ext sensor path item, or read ext sensor path item failed\n");
        return -1;
    }

    set_fs(KERNEL_DS);
    offset = vfs_llseek(fp, 0, SEEK_END);

    buf = (char *)vmalloc(offset * sizeof (char));
    if (buf == NULL) {
        printk("parse item vmalloc buf error\n");
        return -1;
    }

    vfs_read(fp, buf, offset * sizeof(char), &pos);

    path_item_init(buf);
    parse_sensor_attr_info();

    filp_close(fp, NULL);
    vfree(buf);
    set_fs(old_fs);

    return 0;
}

static int ddk_parse_ext_sensor_item(const char *mem)
{
    char path[64];
    struct file *fp = NULL;
    mm_segment_t old_fs = get_fs();
    loff_t offset = 0;
    char *buf = NULL;
    loff_t pos = 0;

    memset((void *)path, 0, sizeof(path));
    memcpy(path, mem, sizeof(path));

    fp = filp_open(path, O_RDWR,0644);
    if (IS_ERR(fp)) {
        printk("no ext sensor item, or read ext sensor item failed\n");
        return -1;
    }

    set_fs(KERNEL_DS);
    offset = vfs_llseek(fp, 0, SEEK_END);

    buf = (char *)vmalloc(offset * sizeof (char));
    if (buf == NULL) {
        printk("parse item vmalloc buf error\n");
        return -1;
    }

    vfs_read(fp, buf, offset * sizeof(char), &pos);

    sensor_item_init(buf);

    filp_close(fp, NULL);
    vfree(buf);
    set_fs(old_fs);

    return 0;

}

static int ddk_replace_config_file(void)
{
    int ret = 0;
    int num = 0;
    char path[64];

    for(num = 0; num < MAX_SENSORS_NUM; num++){
        if (sensor_attr_info[num].isp_config_path[0] == 0 ||
                sensor_attr_info[num].lsh_config_path[0] == 0) {
            continue;
        }

        if (sensor_attr_info[num].sensor_config_path != 0) {
            memset((void *)path, 0, sizeof(path));
            sprintf(path, "%s%s%d-%s",SETUP_PATH, "sensor",num,"config.txt");
            ret = replace_file(path, sensor_attr_info[num].sensor_config_path);
            if (ret) {
                printk("ddk_replace_config_file sensor_config_path failed\n");
                return -1;
            }
        }

        memset((void *)path, 0, sizeof(path));
        sprintf(path, "%s%s%d-%s",SETUP_PATH, "sensor",num,"isp-config.txt");
        ret = replace_file(path, sensor_attr_info[num].isp_config_path);
        if (ret) {
            printk("ddk_replace_config_file isp_config_path failed\n");
            return -1;
        }

        memset((void *)path, 0, sizeof(path));
        sprintf(path, "%s%s%d-%s",SETUP_PATH, "sensor",num,"lsh-config.lsh");
        ret = replace_file(path, sensor_attr_info[num].lsh_config_path);
        if (ret) {
            printk("ddk_replace_config_file lsh_config_path failed\n");
            return -1;
        }
    }

    return ret;
}
char *ddk_get_sensor_path(int index)
{
    return sensor_attr_info[index].sensor_config_path;
}
EXPORT_SYMBOL(ddk_get_sensor_path);

char *ddk_get_isp_path(int index)
{
    return sensor_attr_info[index].isp_config_path;
}
EXPORT_SYMBOL(ddk_get_isp_path);

int ddk_parse_ext_item(void)
{
    int ret = 0;
    int num = 0;
    char *path = NULL;
    ret = ddk_parse_ext_path_item();
    if (ret) {
        return -1;
    }

    for(num = 0; num < MAX_SENSORS_NUM; num++) {
        if (sensor_attr_info[num].sensor_item_path[0] == 0 ||
                sensor_attr_info[num].isp_config_path[0] == 0 ||
                sensor_attr_info[num].lsh_config_path[0] == 0) {
            continue;
        }
            path = sensor_attr_info[num].sensor_item_path;

            ret = ddk_parse_ext_sensor_item(path);
            if (ret) {
                printk("ddk_parse_ext_sensor_item failed\n");
                return -1;
            }

            // add item to global item
            item_sync(sensor_items);
    }

    return 0;
}
EXPORT_SYMBOL(ddk_parse_ext_item);