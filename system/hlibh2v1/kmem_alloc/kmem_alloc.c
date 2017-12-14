#include "kmem_alloc.h"
#include "IM_common.h"
//#include "IM_types.h"
#include "info_log.h"

static int s_ipc_cmem_fd;
static void *s_ipc_cmem_vir_addr = 0;
//int g_kmem_type = KMEM_USE_IPC_CMEM;
int g_kmem_type = KMEM_USE_ION_NEW; 

#ifdef __ARM
pthread_mutex_t g_KmemMutex = PTHREAD_MUTEX_INITIALIZER;
#else
pthread_mutex_t g_KmemMutex = {0};
#endif


void new_ion_alloc(int size_in, char* allocName, void** pVirAddr_out, void** pPhyAddr_out)
{
    static int s_alloc_counter = 0;
    static int s_alloc_total = 0;
    

    if(g_kmem_type == KMEM_USE_IPC_CMEM) return ipc_cmem_alloc(size_in, allocName, pVirAddr_out, pPhyAddr_out);
    
    int ionFd = open("/dev/ion", O_RDWR);
    struct ion_allocation_data allocData = {
            .len = size_in,
            .align = 64,
            .heap_id_mask = ION_HEAP_SYSTEM_CONTIG_MASK | ION_HEAP_TYPE_DMA_MASK,
            .flags = 0,
    };

    s_alloc_counter ++; 
    s_alloc_total += size_in;
    
    // alloc
    int ret = ioctl(ionFd, ION_IOC_ALLOC, &allocData);
    if(ret <0) {
        LOGI_SHM("new_ion_alloc[%d] ionFd=%d. name:%s size:%d. Fail!!!\n",s_alloc_counter,ionFd, allocName, size_in);   
        return;
        }

    //share
    struct ion_fd_data fdData = {
            .handle = allocData.handle,
    };
    ret = ioctl(ionFd, ION_IOC_SHARE, &fdData);
    if (ret < 0)
    {
        LOGE_SHM("ion_alloc ION_IOC_SHARE fail!\n");
        return ;
    }
    if (fdData.fd < 0) {            
        LOGI_SHM("ion_alloc ION_IOC_SHARE fail fd <0!\n");
        return;
    }

    //get physical addr
    void *phy_addr;
    //ret =  infotm_ion_get_phys(ionFd, fdData.fd, &phy_addr);
	struct infotm_ion_buffer_data infotm_data;
	infotm_data.fd = fdData.fd;
	struct ion_custom_data data = {
		.cmd = INFOTM_ION_IOC_GET_INFOS,
		.arg = (unsigned long)&infotm_data,
	};
	ret = ioctl(ionFd, ION_IOC_CUSTOM, &data);
	if (ret<0) {
		LOGE_SHM("ION_IOC_CUSTOM failed in ion_alloc. ret: %d", ret);
	}
	else 	phy_addr = (void*)infotm_data.phy_addr;

    //get virt addr
    void* vir_addr = mmap(NULL, size_in, PROT_READ | PROT_WRITE, MAP_SHARED, fdData.fd, 0);
    if(vir_addr == MAP_FAILED){
        LOGI_SHM("ion_alloc mmap() failed");
        return;
    }

    close(ionFd);

    if(pPhyAddr_out!=NULL) *pPhyAddr_out = phy_addr;
    if(pVirAddr_out!=NULL) *pVirAddr_out = vir_addr;
    
    struct timeval timeend;
    gettimeofday(&timeend, 0);
    LOGI_SHM("ion[%d]-%s, sz=%d, v=%p, p=%p. tol:%x, timest:%ld-%ld\n", 
        s_alloc_counter, allocName, size_in, vir_addr, phy_addr,s_alloc_total, timeend.tv_sec,timeend.tv_usec);
}

static int l_get_ipc_vir_addr()
{
    if(s_ipc_cmem_vir_addr == 0)
    {
        s_ipc_cmem_fd = open(IPC_CMEM_DEVICE_NAME,O_RDWR);
        if(s_ipc_cmem_fd < 0)
        {
            LOGI_SHM("open /dev/ipc_cmem fail. \n");
            return;
        }

        s_ipc_cmem_vir_addr = (void*)mmap(0, TOTAL_IPC_CMEM_ALLOC_SIZE, 
            PROT_READ | PROT_WRITE, MAP_SHARED,s_ipc_cmem_fd, 0);
        LOGI_SHM("mmap fd:%d, p_map(virt):%p, size:0x%x\n", s_ipc_cmem_fd, 
            s_ipc_cmem_vir_addr, TOTAL_IPC_CMEM_ALLOC_SIZE);
    }
    
    if(s_ipc_cmem_vir_addr == MAP_FAILED || s_ipc_cmem_vir_addr == 0)
    {
        LOGI_SHM("ipc_cmem_free\n");
        munmap(s_ipc_cmem_vir_addr, TOTAL_IPC_CMEM_ALLOC_SIZE);
        return -1;
    }

    return 0;    
}

void ipc_cmem_free()
{
    if(l_get_ipc_vir_addr() == -1) return;

    LOGI_SHM("IPC_CMEM_IOCTL_FREE_MEM\n");            
    ioctl(s_ipc_cmem_fd, IPC_CMEM_IOCTL_FREE_MEM, 0);
        
    return;
}
void ipc_cmem_alloc(int sizeM_in, char* allocName_in, void** pVirAddr_out, void** pPhyAddr_out)
{
        static unsigned long cnt=0;
        cnt++;
        
        sizeM_in = (sizeM_in-1)/4096;
        sizeM_in = (sizeM_in+1) * 4096;
        if(l_get_ipc_vir_addr() == -1) return;
    
        T_ALLOC_CMD_DATA data;
    
        data.alloc_size = sizeM_in;
        if(allocName_in!=0)strcpy(data.alloc_name, allocName_in);
        int ret = ioctl(s_ipc_cmem_fd, IPC_CMEM_IOCTL_ALLOC_MEM, &data);

        void *virAddr = s_ipc_cmem_vir_addr + data.mem_offset;
        if(pVirAddr_out!=0) *pVirAddr_out = virAddr;
        if(pPhyAddr_out!=0) *pPhyAddr_out = data.alloc_phy_addr;
        LOGI_SHM("ioctl[%d] name:%s, ret:%d, sz:%x, addr:%x, free:%x, virt:%x, base:%x, off:%x\n", 
            cnt, allocName_in, ret, data.alloc_size, data.alloc_phy_addr, data.free_mem, virAddr, s_ipc_cmem_vir_addr, data.mem_offset);
        return;
}
void ipc_cmem_set(int sizeM_in, char *virAddr, void* phyAddr, char* val)
{
        if(l_get_ipc_vir_addr() == -1) return;
    
        T_GET_VAL_CMD_DATA data;
    
        data.size = sizeM_in;
        data.phy_addr= phyAddr;
        memcpy(data.val, val, sizeM_in);
        int ret = ioctl(s_ipc_cmem_fd, IPC_CMEM_IOCTL_SET_MEM, &data);
        LOGI_SHM("ipc_cmem_set vir:%x, phy:%x, val:%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x \n",
            virAddr, data.phy_addr,  
            *(virAddr+0),*(virAddr+1),*(virAddr+2),*(virAddr+3),
            *(virAddr+4),*(virAddr+5),*(virAddr+6),*(virAddr+7));

        return;
}

void ipc_cmem_get(int sizeM_in, char *virAddr, void* phyAddr, char* val)
{
        if(l_get_ipc_vir_addr() == -1) return;
    
        T_GET_VAL_CMD_DATA data;
    
        data.size = sizeM_in;
        data.phy_addr= phyAddr;
        int ret = ioctl(s_ipc_cmem_fd, IPC_CMEM_IOCTL_GET_MEM, &data);

        if(val!=0)memcpy(val, data.val, data.size);
        
        usleep(100*1000);
        LOGI_SHM("\nipc_cmem_get vir:%x, phy:%x, val:%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x \n",
            virAddr, data.phy_addr, 
            *(virAddr+0),*(virAddr+1),*(virAddr+2),*(virAddr+3),
            *(virAddr+4),*(virAddr+5),*(virAddr+6),*(virAddr+7));

        virAddr = data.val;
        LOGI_SHM("\nipc_cmem_get2 vir:%x, phy:%x, val:%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x \n",
            virAddr, data.phy_addr, 
            *(virAddr+0),*(virAddr+1),*(virAddr+2),*(virAddr+3),
            *(virAddr+4),*(virAddr+5),*(virAddr+6),*(virAddr+7));

        return;
}


int alc_open(void *alc, void* owner)
{
    return 0;
}

int alc_alloc(void *alc, int size, IM_INT32 flag, IM_Buffer *buffer)
{ 
    
    printf("kmem alc, buffer:%x\n", buffer);
    new_ion_alloc(size, "alc_alloc", &buffer->vir_addr, &buffer->phy_addr);
    buffer->size = size;
    printf("after kmem alc, buffer vir:%x, phy:%x\n", buffer->vir_addr, buffer->phy_addr);

    return 0;
}


int alc_close(void *alc)
{
    return 0;
}

int alc_free(void *alc, IM_Buffer *buffer)
{
    return 0;
}

