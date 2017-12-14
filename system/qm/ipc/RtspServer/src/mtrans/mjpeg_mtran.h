#ifndef _MJPEG_MTRANE_H_
#define _MJPEG_MTRANE_H_

struct jpeghdr 
{
    unsigned int tspec:8;
    unsigned int off:24;
    unsigned char type;
    unsigned char q;
    unsigned char width;
    unsigned char height;
};
struct jpeghdr_rst 
{
    unsigned short dri;
    unsigned int f:1;
    unsigned int l:1;
    unsigned int count:14;
};
struct jpeghdr_qtable 
{
    unsigned char  mbz;
    unsigned char  precision;
    unsigned short length;
};
#define RTP_JPEG_RESTART           0x40
#endif

