
//  properties.h
//  读写配置文件
//

#ifndef __PROPERTIES_H__
#define __PROPERTIES_H__

#define KEY_SIZE        128 // key缓冲区大小
#define VALUE_SIZE      128 // value缓冲区大小

#define LINE_BUF_SIZE   256 // 读取配置文件中每一行的缓冲区大小

typedef struct Properties {
    char *key;
    char *value;
    struct Properties *pNext;
} Properties;

typedef struct PROPS_HANDLE {
    Properties *pHead;  // 属性链表头节点
    char *filepath;     // 属性文件路径
} PROPS_HANDLE;


// 初始化环境，成功返回0，失败返回非0值
int init(const char *filepath,void **handle);

// 根据KEY获取值，找到返回0，如果未找到返回非0值
int getValue(void *handle, const char *key, char *value);

// 修改key对应的属性值，修改成功返回0，失败返回非0值
int setValue(void *handle, const char *key, const char *value);

// 添加一个属性，添加成功返回0，失败返回非0值
int addKey(void *handle, const char *key, const char *value);

// 删除一个属性，删除成功返回0，失败返回非0值
int delKey(void *handle, const char *key);

// 获取属性文件中所有的key，获取成功返回0，失败返回非0值
int getKeys(void *handle, char ***keys, int *keyscount);

// 释放所有key的内存空间，成功返回0，失败返回非0值
int freeKeys(char ***keys,int *keyscount);

// 获取属性文件中所有的值，成功返回0，失败返回非0值
int getValues(void *handle, char ***values, int *valuescount);

// 释放所有value的内存空间，成功返回0，失败返回非0值
int freeValues(char ***values, int *valuescount);

// 获取属性数量，成功返回0，失败返回非0值
int getCount(void *handle, int *count);

// 释放环境资源，成功返回0，失败返回非0值
int releaseHandle(void **handle);

int saveConfig(const char *filepath,Properties *head);

#endif //__PROPERTIES_H__
