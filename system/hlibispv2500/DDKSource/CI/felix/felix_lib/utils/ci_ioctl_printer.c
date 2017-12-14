
#include <ci_kernel/ci_ioctrl.h>
#ifdef FELIX_HAS_DG
#include <dg_kernel/dg_ioctl.h>
#endif

#include <stdlib.h>

#define PRINT_IO(N) \
    print_ioctl_value(#N, N, v)

void print_ioctl_value(const char *name, unsigned int value, int v)
{
    if (v)
    {
        printf("\t\"0x%08x\":\"%s\",\n", value, name);
    }
    else
    {
        printf("0x%08x:%s\n", value, name);
    }
}

int main(int argc, char **argv)
{
    int v = 0;
    int c = 1;

    while (c < argc)
    {
        if (0 == strncmp(argv[c], "-h", 2))
        {
            printf("Tool to print CI ioctls\n");
#ifdef FELIX_HAS_DG
            printf("Includes ExtDG ioctls\n");
#endif
            printf("Usage:\n");
            printf("-h to print help\n");
            printf("-json to print ioctl values as json format\n");
            return EXIT_SUCCESS;
        }
        if (0 == strncmp(argv[c], "-json", 2))
        {
            v = 1;
        }
        
        c++;
    }

    if (v)
    {
        printf("\"ioctl_num\":{\n");
    }

    // driver commands
    PRINT_IO(CI_IOCTL_INFO);
    PRINT_IO(CI_IOCTL_LINE_GET);
    PRINT_IO(CI_IOCTL_LINE_SET);
    PRINT_IO(CI_IOCTL_GMAL_GET);
    PRINT_IO(CI_IOCTL_GMAL_SET);
    PRINT_IO(CI_IOCTL_TIME_GET);
    PRINT_IO(CI_IOCTL_DPFI_GET);
    PRINT_IO(CI_IOCTL_RTM_GET);

    // configuration commands
    printf("\n");
    PRINT_IO(CI_IOCTL_PIPE_REG);
    PRINT_IO(CI_IOCTL_PIPE_DEL);

    PRINT_IO(CI_IOCTL_PIPE_WAI);
    PRINT_IO(CI_IOCTL_PIPE_AVL);
    PRINT_IO(CI_IOCTL_PIPE_PEN);
    PRINT_IO(CI_IOCTL_PIPE_ADD);
    PRINT_IO(CI_IOCTL_PIPE_REM);
    PRINT_IO(CI_IOCTL_PIPE_UPD);

    PRINT_IO(CI_IOCTL_CREATE_BUFF);
    PRINT_IO(CI_IOCTL_DEREG_BUFF);

    // capture commands
    printf("\n");
    PRINT_IO(CI_IOCTL_CAPT_STA);
    PRINT_IO(CI_IOCTL_CAPT_STP);
    PRINT_IO(CI_IOCTL_CAPT_TRG);
    PRINT_IO(CI_IOCTL_CAPT_BAQ);
    PRINT_IO(CI_IOCTL_CAPT_BRE);
    PRINT_IO(CI_IOCTL_CAPT_ISS);

    // gasket commands
    printf("\n");
    PRINT_IO(CI_IOCTL_GASK_ACQ);
    PRINT_IO(CI_IOCTL_GASK_REL);
    PRINT_IO(CI_IOCTL_GASK_NFO);

    // int dg commands
    PRINT_IO(CI_IOCTL_INDG_REG);
    PRINT_IO(CI_IOCTL_INDG_STA);
    PRINT_IO(CI_IOCTL_INDG_ISS);
    PRINT_IO(CI_IOCTL_INDG_STP);
    PRINT_IO(CI_IOCTL_INDG_ALL);
    PRINT_IO(CI_IOCTL_INDG_ACQ);
    PRINT_IO(CI_IOCTL_INDG_TRG);
    PRINT_IO(CI_IOCTL_INDG_FRE);

#ifdef FELIX_HAS_DG
    printf("\n");
    PRINT_IO(DG_IOCTL_INFO);
    
    PRINT_IO(DG_IOCTL_CAM_REG);
    PRINT_IO(DG_IOCTL_CAM_ADD);
    PRINT_IO(DG_IOCTL_CAM_STA);
    PRINT_IO(DG_IOCTL_CAM_STP);
    PRINT_IO(DG_IOCTL_CAM_BAQ);
    PRINT_IO(DG_IOCTL_CAM_BRE);
    PRINT_IO(DG_IOCTL_CAM_DEL);
#endif

    if (v)
    {
        printf("},\n");
    }
    
    return EXIT_SUCCESS;
}
