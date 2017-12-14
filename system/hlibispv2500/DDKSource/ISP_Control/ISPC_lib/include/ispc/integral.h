#ifndef INTEGRAL_H
#define INTEGRAL_H

typedef enum _integral_err_e
{
	INTERGRAL_NO_ERROR = 0,
	INTERGRAL_NULL_PARAMTER,
	INTERGRAL_INVALID_PARAMTER,
} integral_err_e;

typedef struct _integral_roi
{
	int x;
	int y;
	int width;
	int height;
}integral_roi;

//#ifdef __cplusplus
//extern "C" {
//#endif

extern integral_err_e integral_process(unsigned char *src, int src_rowsize, unsigned long long *dst, int dst_rowsize, integral_roi *roi);

//#ifdef __cplusplus
//}
//#endif
#endif
