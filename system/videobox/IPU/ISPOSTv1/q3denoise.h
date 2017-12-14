#ifndef Q3DENOISE_H_
#define Q3DENOISE_H_

#define T_MAX 254
#define T_MIN 2
#define M_MAX 9//5
#define M_MIN 1

#define Q3HW_PORTING 1
#define DENOISE_HERMITE_SEGMENTS 16
#define DENOISE_ABSDIFF_MAX 255

#define YUV_y 0
#define YUV_u 1
#define YUV_v 2

typedef struct
{
    int        xn;    // x coordinate
    double    yn; // y coordinate
    double    sn; // slope
    int        rn; // range spec (2 - 128, at power of 2)
} DN_HerM_ACDoubleInfo;

#endif