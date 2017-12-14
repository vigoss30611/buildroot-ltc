#ifndef __SPV_DEBUG_H__
#define __SPV_DEBUG_H__

#include<stdio.h>

#define DEBUG_ENABLE

#ifdef DEBUG_ENABLE
#define INFO(format,...) printf("[boot-selector]("__FILE__"#%s:%d): "format"", __func__, __LINE__, ##__VA_ARGS__)
#else
#define INFO(format,...)
#endif

#define ERROR(format,...) printf("[boot-selector]("__FILE__"#%s:%d): "format"", __func__, __LINE__, ##__VA_ARGS__)

#endif
