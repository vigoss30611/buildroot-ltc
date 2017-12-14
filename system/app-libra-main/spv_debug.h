#ifndef __SPV_DEBUG_H__
#define __SPV_DEBUG_H__

#include<stdio.h>

#include"libramain.h"

#ifdef GUI_DEBUG_ENABLE
#define INFO(format,...) printf("[GUI]("__FILE__"#%s): "format"", __func__, ##__VA_ARGS__)
#else
#define INFO(format,...)
#endif

#define ERROR(format,...) printf("[GUI]("__FILE__"#%s): "format"", __func__, ##__VA_ARGS__)

#endif
