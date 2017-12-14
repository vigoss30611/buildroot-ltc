#ifndef _KMEM_ALLOC_H
#define _KMEM_ALLOC_H
#include <stdio.h>
#include <unistd.h>  
#include "info_log.h"
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
 #include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h> 

typedef int ion_user_handle_t;

enum ion_heap_type {
 ION_HEAP_TYPE_SYSTEM,
 ION_HEAP_TYPE_SYSTEM_CONTIG,
 ION_HEAP_TYPE_CARVEOUT,
 ION_HEAP_TYPE_CHUNK,
 ION_HEAP_TYPE_DMA,
 ION_HEAP_TYPE_CUSTOM,
 ION_NUM_HEAPS = 16,
};

#define ION_HEAP_SYSTEM_MASK (1 << ION_HEAP_TYPE_SYSTEM)
#define ION_HEAP_SYSTEM_CONTIG_MASK (1 << ION_HEAP_TYPE_SYSTEM_CONTIG)
#define ION_HEAP_CARVEOUT_MASK (1 << ION_HEAP_TYPE_CARVEOUT)
#define ION_HEAP_TYPE_DMA_MASK (1 << ION_HEAP_TYPE_DMA)
#define INFOTM_ION_IOC_GET_INFOS 3

struct ion_allocation_data {
 size_t len;
 size_t align;
 unsigned int heap_id_mask;
 unsigned int flags;
 ion_user_handle_t handle;
};

struct ion_fd_data {
 ion_user_handle_t handle;
 int fd;
};

struct ion_handle_data {
 ion_user_handle_t handle;
};

struct ion_custom_data {
 unsigned int cmd;
 unsigned long arg;
};

struct infotm_ion_buffer_data {
	int fd;
	int buf_uid;
	unsigned int dev_addr;
	unsigned int phy_addr;
	size_t size;
	size_t num_pages;
};

#define ION_IOC_MAGIC 'I'
#define ION_IOC_ALLOC _IOWR(ION_IOC_MAGIC, 0,   struct ion_allocation_data)
#define ION_IOC_FREE _IOWR(ION_IOC_MAGIC, 1, struct ion_handle_data)
#define ION_IOC_MAP _IOWR(ION_IOC_MAGIC, 2, struct ion_fd_data)
#define ION_IOC_SHARE _IOWR(ION_IOC_MAGIC, 4, struct ion_fd_data)
#define ION_IOC_IMPORT _IOWR(ION_IOC_MAGIC, 5, struct ion_fd_data)
#define ION_IOC_SYNC _IOWR(ION_IOC_MAGIC, 7, struct ion_fd_data)
#define ION_IOC_CUSTOM _IOWR(ION_IOC_MAGIC, 6, struct ion_custom_data)


// following is for ipc_cmem
#ifndef PAGE_SIZE
    #define PAGE_SIZE 4096
#endif

#define IPC_CMEM_DEVICE_NAME "/dev/ipc_cmem"
#define LOOP_ipc_cmem_MAX 200
#define ONE_ipc_cmem_LENGTH (1024*1024)
#define TOTAL_IPC_CMEM_ALLOC_SIZE (5*1024*1024)

extern pthread_mutex_t g_KmemMutex;

#define NEW_ION_ALLOC_CMEM(buf_fd, size_in, pBlk_in, name)\
{\
    int l_size = size_in;\
    BUFFER_BlockInfo *pBlk= pBlk_in;\
    if(g_kmem_type==KMEM_USE_ION_OLD)\
    {\
        buffer_alloc_block(buf_fd, l_size, pBlk);\
    }\
    else\
    {\
        pthread_mutex_lock(&g_KmemMutex);\
        new_ion_alloc(l_size, name, &(pBlk->vir_addr), &(pBlk->phy_addr));\
        pBlk->size = l_size;\
        pthread_mutex_unlock(&g_KmemMutex);\
    }\
}


enum E_IPC_CMEM_IOCTL_CMD{
    IPC_CMEM_IOCTL_ALLOC_MEM = 10001,
    IPC_CMEM_IOCTL_FREE_MEM  = 10002,
    
    IPC_CMEM_IOCTL_GET_MEM  = 10003,
    IPC_CMEM_IOCTL_SET_MEM  = 10004,
};

typedef struct{
    char alloc_name[100];
    unsigned long  alloc_size;
    void *alloc_phy_addr;
    unsigned long  total_mem;
    unsigned long  mem_offset; // comared to base
    unsigned long  free_mem;
}T_ALLOC_CMD_DATA;

typedef struct{
    void *phy_addr;
    char val[100];
    int  size;
}T_GET_VAL_CMD_DATA;

enum
{
    KMEM_USE_ION_OLD,
    KMEM_USE_ION_NEW,
    KMEM_USE_IPC_CMEM,
};
    

#ifdef __cplusplus 
extern "C" {
#endif

void new_ion_alloc(int sizeM_in, char* allocName_in, void** pVirAddr_out, void** pPhyAddr_out);
void ipc_cmem_alloc(int sizeM_in, char* allocName_in, void** pVirAddr_out, void** pPhyAddr_out);
void ipc_cmem_free();
void ipc_cmem_get(int sizeM_in, char *virAddr, void* phyAddr, char* val);
    void ipc_cmem_set(int sizeM_in, char *virAddr, void* phyAddr, char* val);


extern int g_kmem_type;

#ifdef __cplusplus 
}
#endif

//for ion
typedef int IM_INT32;
typedef unsigned int IM_UINT32;

//log
#define LOGI_SHM  printk
#define LOGE_SHM  printk


#endif
