#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>



int main()
{
    int *io, id = ~0;
    int fd = open("/dev/mem", O_RDWR);

    io = (int *) mmap(0, 4096, PROT_READ | PROT_WRITE , MAP_SHARED, fd,
            0xc0000000);

    if(io != MAP_FAILED)
    {
        int i;
        printf("\n");
        for(i = 0; i < 39; i++)
            printf("\tswreg%d\t= 0x%08x\n",i, io[i]);
        printf("\n");                
        munmap(io, 4096);
    }
    
    close(fd);
           
}
