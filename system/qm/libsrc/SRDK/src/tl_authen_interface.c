/******************************************************************************
******************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>


#include "tl_authen_interface.h"


#define ENCRYPT_FILE "/config/.encrypt.txt"


#define encypt_error() printf("err:%s:%d\n", __FUNCTION__, __LINE__)

int encrypt_info_write(char *data, int len)
{
    int ret;
    int flashfd;

    flashfd = open(ENCRYPT_FILE, O_RDWR|O_CREAT|O_SYNC);
    if (flashfd < 0)
    {
        printf("Open flash fail\n");
        close(flashfd);
        return -1;
    }

    ret = write(flashfd, data, len);
    if (ret != len)
    {
        printf("Flash write fail\n");
        close(flashfd);
        return -1;
    }

    close(flashfd);

    return 0;
}

int encrypt_info_read(char *data, int len)
{
    int r = 0;
    FILE *  fp = NULL;
    
    
    do {
        if ((access(ENCRYPT_FILE, F_OK)) == -1)
        {
            r = -3;
            break;
        }
        
        fp = fopen(ENCRYPT_FILE, "rb+");
        if (fp == NULL)
        {
            encypt_error();
            r = -1;
            break;
        }

        if (fread(data, 1, len, fp) != len)
        {
            encypt_error();
            r = -2;
            break;
        }
        
    }while(0);

    if (fp)
    {
        fclose(fp);
    }
    
    return r;
}

int encrypt_info_check(char *data, int len, unsigned short checksum)
{
    unsigned short val = 0;

    while (len > 0)
    {
        val += data[--len];
    }
    if (checksum != val) // ¨ºy?YD¡ê?¨¦
    {
        return -1;
    }

    return 0;
}

