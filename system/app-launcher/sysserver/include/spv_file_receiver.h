#ifndef __SPV_FILE_RECEIVER_H__
#define __SPV_FILE_RECEIVER_H__
/**
 * Package Format.
 * magic_number + FRHeader + data
 *
 **/
#include <inttypes.h>

#define FILE_RECEIVER_PORT 8890

#define MAGIC_NUMBER 0x12345678

enum {
    PACKAGE_START = 1,
    PACKAGE_MIDDLE,
    PACKAGE_END,
    PACKAGE_ALL,
};

enum {
    FILE_CALIBRATION = 1,
    FILE_UPGRADE,
    FILE_PCB_TEST,
    //FILE_SN,
};

typedef struct _FRHeader {
    uint8_t flag; // set to 1
    uint32_t id; //1 2 3...
    uint32_t type; //1 means calibration file, 2 means upgrade file
    uint32_t length; //data length
} FRHeader;

void init_file_receiver_server();
int SpvSendSocketMsg(char *addr, int port, char *msg, int length);
#endif
