#ifndef CODEC_VOLUME_H
#define CODEC_VOLUME_H

#include <stdint.h>
//#include <sys/cdef.h>

#define ERROR_OUTOFRANGE	-1
#define ERROR_NO_CONTROL	-2

#define CODEC_VOLUME_MAX 	46

//__BEGIN__DECLS
#ifdef __cplusplus
extern "C" {
#endif

#define CODEC_VOLUME_MODULE_ID "VOLUME"

int set_volume(int volume);
int get_volume(void);
int set_mute(int sw);

#ifdef __cplusplus
}
#endif
//__END__DECLS

#endif

