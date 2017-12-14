#ifndef __LLONG_EXEC_H__
#define __LLONG_EXEC_H__

#define MKDIR_TIMEOUT 3000
#define MOUNT_TIMEOUT 15000
#define UMOUNT_TIMEOUT 15000
#define HDPARM_I_TIMEOUT 5000
#define HDPARM_C_TIMEOUT 5000
#define HDPARM_Y_TIMEOUT 5000
#define MKE2FS_TIMEOUT 2000000
#define TUNE2FS_TIMEOUT 5000
#define HDPARM_G_TIMEOUT 10000
#define MDEV_S_TIMEOUT 5000
#define DUMPE2FS_TIMEOUT 5000
#define E2FSCK_TIMEOUT 1200000
#define HDPARM_S_TIMEOUT 15000
#define SMART_TIMEOUT 15000
#define MAX_TIMEOUT 360000000 //100 hours

#define EXEC_ERR_CMD_OPEN -101
#define EXEC_ERR_CMD_RETURN -102
#define EXEC_ERR_TIMEOUT -103

int tools_exec(char* cmd, int timeout, char* ret_str, int out_msg, int* status);
char* get_num(char* str, int* num);
char* get_str(char* str, char* res_str);
char* get_double(char* str, double* num);

#endif

