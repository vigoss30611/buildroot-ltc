#ifndef GSENSOR_H
#define GSENSOR_H

#include <stdint.h>

//__BEGIN__DECLS
#ifdef __cplusplus
extern "C" {
#endif

#define GSENSOR_ENABLE	"/sys/class/misc/%s/attribute/enable"

int enable_gsensor(int);

#ifdef __cplusplus
}
#endif
//__END__DECLS

#endif

