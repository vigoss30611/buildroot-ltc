#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/time.h>
#include <errno.h>
#include <time.h>

#define     FILE_NAME_LEN       64
static pthread_mutex_t   write_lock;
static int  sig_break_flag = 0;
static const char *mmc_short_options = "p:m:b:g:s:n:f:i:";
//no_argument required_argument optional_argument
static const struct option mmc_long_options[] = {
    {"partition", required_argument, NULL, 'p'},
	{"mountpoint", required_argument, NULL, 'm'},
	{"blockkb",  required_argument,NULL, 'b'},
	{"gapkb", required_argument, NULL, 'g'},
	{"filesize", required_argument, NULL, 's'},
	{"filenum", required_argument, NULL, 'n'},
	{"repeatnum", required_argument, NULL, 'r'},
	{"interrupt", required_argument, NULL, 'i'},
    {NULL, 0, NULL, 0 }
};

static char*       p_file_buf = NULL;

enum _fat32_cmd{
    WRITE_SPEED_TEST,
    READ_SPEED_TEST,
    CREATE_FILE_WRITE,
    MULTIP_FILE_WRITE,
    FRAME_STREAM_TEST,
    STATISTIC,
};

static void signal_handler(int sig)
{
    if(SIGINT == sig) {
        sig_break_flag = 1;
    }
}

static void print_usage_1(const char *program_name)
{	
    printf("version mmc_test 1\n");
    
	printf("%s 1.0.0 : for SD/MMC card test\n", program_name);
    printf("Usage: %s [OPTION]\n", program_name);
    printf("write_speed/read_speed  Start write or read speed test, can terminate with (ctrl+c)\n");
    printf("\t-p, --partition=	    used only if sd/mmc fat32 partition not mounted yet\n");
    printf("\t		                Specify sd/mmc fat32 partition device name\n");
    printf("\t-m, --mountpoint=	    Specify sd/mmc card fat32 partition mount point\n");
    printf("\t-s, --filesize=	    Specify file size(mbyte)\n");
    printf("\t-n, --filenum=	    Specify created file num\n");
}

static void print_usage_2(const char *program_name)
{
    int file_num = 4;
    int file_size = 512;
    printf("\n");
    printf("Please firstly mount SD card fat32 partition\n");
    printf("!!!!! Notice: You must run write_speed test firstly, which will create files needed by read_speed test !!!!!\n");
    printf("Suggested Test Case:\n");
	printf("Case 1: %s write_speed -m /mnt/sd0 -s %d -n %d\n", program_name, file_size, file_num);
    printf("\tCreate %d file in /mnt/sd0, each write %dMbytes, then print Write speed.\n", file_num, file_size);
    printf("Case 2: %s read_speed /mnt/sd0 -s %d -n %d\n", program_name, file_size, file_num);
    printf("\tRead from %d %dm file previously created, then print Read speed.\n", file_num, file_size);

    printf("\nYou could mount SD/MMC fat32 partition within mmc_test program too with the following command:\n");
    printf("Case 3: %s write_speed -p /dev/mmcblk1p1 -m /mnt/sd0 -s %d -n %d\n", program_name, file_size, file_num);
    printf("\tMount /dev/mmcblk1p1 at /mnt/sd0, then Create %d file, each write %dMbytes. Then print Write speed.\n", file_num, file_size);
}

ssize_t readn(int fd, char *ptr, size_t n)
{
    ssize_t left;
    ssize_t read_len;
    
    left = n;
    while (left >0)
    {
        if ((read_len = read(fd, ptr, left)) < 0)
        {
            if (errno == EINTR)
                read_len = 0;		/* and call read() again */
            else
                return -1;		/* error */
        }
        else if (read_len == 0)
            break;
        left -= read_len;
        ptr += read_len;
    }
    return (n-left);
}

ssize_t writen(int fd, char *ptr, size_t n)
{
    size_t		nleft;
    ssize_t		nwritten;
    int	        overtime = 50;

    nleft = n;
    while (nleft > 0)
    {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if (nwritten < 0 && ((errno == EINTR)||(errno == EAGAIN)))
            {
                if(overtime-- < 0){
                    printf("writen timeout\n");
                    return -1;
                }

				usleep(1000);
                nwritten = 0;		/* and call write() again */
            }
            else
                return(-1);		/* error */
        }

        nleft -= nwritten;
        ptr   += nwritten;
    }
    return(n);
}

struct file_transmit{
    char        filename[FILE_NAME_LEN];
    int         command;
    int         open_flags;
    int         total_size;
    int         blocksize;
    int         blocknum;
    int         gapsize;
    int         new_flag;
    int         int_flag;
    int         filenum;
    int         repeatnum;
    int         frame_control;
};
//write in loop, 
void thread_view_statistic(void* arg)
{
    struct timeval tim_val, tim_start;
    int            index = 0;

    while(0 == sig_break_flag){
        gettimeofday(&tim_start, NULL);
        printf("~~~~~~~~~~~~~index: %d at:%ld~~~~~~~~~~~~~~~~~~~\n", index, tim_start.tv_sec);
        system("cat /proc/interrupts | grep 164");
        system("cat /proc/interrupts | grep 165");
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        tim_start.tv_sec += 60;
        while(0 == sig_break_flag){
            sleep(1);
            gettimeofday(&tim_val, NULL);
            if(tim_val.tv_sec >= tim_start.tv_sec)
                break;
        }
    }

    pthread_exit(0);
}

//write in loop, 
void thread_write_frame(void* arg)
{
    struct file_transmit* file_arg= (struct file_transmit*)arg;
    int     i=0;
    int     file_id=0;

    if(NULL == p_file_buf){
        printf("p_file_buf:null!\n");
        return;
    }

    file_id = open(file_arg->filename, file_arg->open_flags, 0666);
    if(-1 == file_id){
        printf("open() file:%s failed!\n", file_arg->filename);
        
        return;
    }

    for(i=0;i<(file_arg->total_size/32);i++){
        if(sig_break_flag){
            printf("sig_break_flag: break at %d block!\n", i);
            break;
        }

        pthread_mutex_lock(&write_lock);
        write(file_id, p_file_buf, 32*1024);
        pthread_mutex_unlock(&write_lock);
        usleep(30000);
    }

    fsync(file_id);
    close(file_id);

    pthread_exit(0);
}

void thread_read_file(void* arg)
{
    struct file_transmit* file_arg= (struct file_transmit*)arg;
    int     i=0;
    int     file_id=0;

    if(NULL == p_file_buf){
        printf("p_file_buf:null!\n");
        return;
    }

    pthread_mutex_lock(&write_lock);
    file_arg->open_flags = O_RDONLY;
    file_id = open(file_arg->filename, file_arg->open_flags, 0666);
    if(-1 == file_id){
        printf("open() file:%s failed!\n", file_arg->filename);
        pthread_mutex_unlock(&write_lock);
        return;
    }

    for(i=0;i<file_arg->blocknum;i++){
        if(sig_break_flag){
            printf("sig_break_flag: break at %d block!\n", i);
            break;
        }

        int ret = read(file_id, p_file_buf, 1024*file_arg->blocksize);
        if(ret != 1024*file_arg->blocksize)
            printf("len err: %d\n", ret);

        if(file_arg->gapsize > 0)
            lseek(file_id, file_arg->gapsize*1024, SEEK_CUR);
    }

    fsync(file_id);
    close(file_id);
    pthread_mutex_unlock(&write_lock);
    pthread_exit(0);
}

void thread_write_file(void* arg)
{
    struct file_transmit* file_arg= (struct file_transmit*)arg;
    int     i=0;
    int     file_id=0;

    if(NULL == p_file_buf){
        printf("p_file_buf:null!\n");
        return;
    }

    pthread_mutex_lock(&write_lock);
    file_arg->open_flags = O_CREAT | O_WRONLY;
    file_id = open(file_arg->filename, file_arg->open_flags, 0666);
    if(-1 == file_id){
        printf("open() file:%s failed!\n", file_arg->filename);
        pthread_mutex_unlock(&write_lock);
        return;
    }

    for(i=0;i<file_arg->blocknum;i++){
        if(sig_break_flag){
            printf("sig_break_flag: break at %d block!\n", i);
            break;
        }

        write(file_id, p_file_buf, 1024*file_arg->blocksize);

        if(file_arg->gapsize > 0)
            lseek(file_id, file_arg->gapsize*1024, SEEK_CUR);
    }

    fsync(file_id);
    close(file_id);
    pthread_mutex_unlock(&write_lock);
    pthread_exit(0);
}

int    test_multifile_write(struct file_transmit *pfilepara)
{
    pthread_t           *pid_write = NULL;
    int                 ret = 0, i = 0, offset = 0, test_counter = 0;
    struct timeval      start_timval;
    struct timeval      end_timval;
    float               delta_seconds;
    float               total_write_sec = 0;

    struct file_transmit *pfile = (struct file_transmit*)malloc(sizeof(struct file_transmit)*pfilepara->filenum);
    if(NULL == pfile){
        printf("pfile malloc failed!\n");
        goto    out;
    }

    pid_write = (pthread_t*)malloc(sizeof(pthread_t)*pfilepara->filenum);
    if(NULL == pid_write){
        printf("pid_write malloc failed!\n");
        goto    out;
    }

    for(i=0; i<pfilepara->filenum; i++){
        memcpy(&pfile[i], pfilepara, sizeof(struct file_transmit));

        offset = snprintf(pfile[i].filename, sizeof(pfile[i].filename), "%s", pfilepara->filename);
        snprintf(pfile[i].filename+offset, sizeof(pfile[i].filename)-offset, "%d", i);
        remove(pfile[i].filename);

        pfile[i].new_flag = 1;
    }

    do{
        gettimeofday(&start_timval, NULL);
        if(pfilepara->int_flag){
            printf("~~~~~~~~~~~~~~~~~~~%u %d~~~~~~~~~~~~~~~~~~~\n", (unsigned int)start_timval.tv_sec, test_counter);
            system("cat /proc/interrupts | grep 164");
        }
        for(i=0; i<pfilepara->filenum; i++){
            if(FRAME_STREAM_TEST == pfilepara->command)
                ret = pthread_create(&pid_write[i], NULL,(void *)thread_write_frame, (void*)&pfile[i]);
            else
                ret = pthread_create(&pid_write[i], NULL,(void *)thread_write_file, (void*)&pfile[i]);
            if(ret != 0){
                printf("Create pthread failed!\n");
                pid_write[i] = 0;
                goto out;
            }
        }

        for(i=0; i<pfilepara->filenum; i++){
            if(0 != pid_write[i]){
                pthread_join(pid_write[i], NULL);
                pid_write[i] = 0;
            }
        }
        gettimeofday(&end_timval, NULL);
        delta_seconds = (end_timval.tv_sec- start_timval.tv_sec)*1000
            +(end_timval.tv_usec- start_timval.tv_usec)/1000;
        delta_seconds = delta_seconds/1000;
        total_write_sec += delta_seconds;
        if(delta_seconds > 0)
            printf("test:%d Write %dm in %fs, %fmbyte/s!\n", test_counter, (pfilepara->filenum*pfilepara->total_size)>>10, delta_seconds, ((float)(pfilepara->filenum*pfilepara->total_size/delta_seconds))/1024.0);
        if(pfilepara->int_flag){
            system("cat /proc/interrupts | grep 164");
            printf("~~~~~~~~~~~~~~~~~~~%u~~~~~~~~~a~~~~~~~~~~\n", (unsigned int)end_timval.tv_sec);
        }

        for(i=0; i<pfilepara->filenum; i++){
            if(WRITE_SPEED_TEST == pfilepara->command)
                break;

            if(pfile[i].new_flag)
                remove(pfile[i].filename);
        }
        test_counter++;
    }while(test_counter < pfilepara->repeatnum);

    printf("Average:Write %dm in %fs,%fmbyte/s!\n", pfilepara->repeatnum*pfilepara->filenum*pfilepara->total_size>>10, total_write_sec
        ,(((float)(pfilepara->repeatnum*pfilepara->filenum*pfilepara->total_size))/((float)total_write_sec))/1024.0);

out:
    if(pid_write)
        free(pid_write);
    if(pfile)
        free(pfile);
    return 0;
}

int    test_multifile_read(struct file_transmit *pfilepara)
{
    pthread_t           *pid_write = NULL;
    int                 ret = 0, i = 0, offset = 0, test_counter = 0;
    struct timeval      start_timval;
    struct timeval      end_timval;
    float               delta_seconds;
    float               total_read_sec = 0;

    struct file_transmit *pfile = (struct file_transmit*)malloc(sizeof(struct file_transmit)*pfilepara->filenum);
    if(NULL == pfile){
        printf("pfile malloc failed!\n");
        goto    out;
    }

    pid_write = (pthread_t*)malloc(sizeof(pthread_t)*pfilepara->filenum);
    if(NULL == pid_write){
        printf("pid_write malloc failed!\n");
        goto    out;
    }

    for(i=0; i<pfilepara->filenum; i++){
        memcpy(&pfile[i], pfilepara, sizeof(struct file_transmit));

        offset = snprintf(pfile[i].filename, sizeof(pfile[i].filename), "%s", pfilepara->filename);
        snprintf(pfile[i].filename+offset, sizeof(pfile[i].filename)-offset, "%d", i);

        //pfile[i].open_flags = O_CREAT | O_WRONLY;
        pfile[i].new_flag = 1;
        //pfile[i].command = INTERRUPT_COUNTER;
    }

    do{
        gettimeofday(&start_timval, NULL);
        if(pfilepara->int_flag){
            printf("~~~~~~~~~~~~~~~~~~~%u %d~~~~~~~~~~~~~~~~~~~\n", (unsigned int)start_timval.tv_sec, test_counter);
            system("cat /proc/interrupts | grep 164");
        }
        for(i=0; i<pfilepara->filenum; i++){
            ret = pthread_create(&pid_write[i], NULL,(void *)thread_read_file, (void*)&pfile[i]);
            if(ret != 0){
                printf("Create pthread failed!\n");
                pid_write[i] = 0;
                goto out;
            }
        }

        for(i=0; i<pfilepara->filenum; i++){
            if(0 != pid_write[i]){
                pthread_join(pid_write[i], NULL);
                pid_write[i] = 0;
            }
        }
        gettimeofday(&end_timval, NULL);
        delta_seconds = (end_timval.tv_sec- start_timval.tv_sec)*1000
            +(end_timval.tv_usec- start_timval.tv_usec)/1000;
        delta_seconds = delta_seconds/1000;
        total_read_sec += delta_seconds;
        if(delta_seconds > 0)
            printf("test:%d Read %dm in %fs, %fmbyte/s!\n", test_counter, (pfilepara->filenum*pfilepara->total_size)>>10, delta_seconds, ((float)(pfilepara->filenum*pfilepara->total_size/delta_seconds))/1024.0);
        if(pfilepara->int_flag){
            system("cat /proc/interrupts | grep 164");
            printf("~~~~~~~~~~~~~~~~~~~%u~~~~~~~~~a~~~~~~~~~~\n", (unsigned int)end_timval.tv_sec);
        }
        test_counter++;
    }while(test_counter < pfilepara->repeatnum);

    printf("Average:Read %dm in %fs,%fmbyte/s!\n", pfilepara->repeatnum*pfilepara->filenum*pfilepara->total_size>>10, total_read_sec
            ,(((float)(pfilepara->repeatnum*pfilepara->filenum*pfilepara->total_size))/((float)total_read_sec))/1024.0);

out:
    if(pid_write)
        free(pid_write);
    if(pfile)
        free(pfile);
    return 0;
}

int    test_view_statistic()
{
    pthread_t   pid_write = 0;

    int ret = pthread_create(&pid_write, NULL,(void *)thread_view_statistic, NULL);
    if(ret != 0){
        printf("Create feeddog pthread failed!\n");
    }
    if(0 != pid_write){
        pthread_join(pid_write, NULL);
        pid_write = 0;
    }

    return 0;
}

int main(int argc, char** argv)
{
#define     DEFAULT_BLOCK_SIZE      64   //Kilo bytes
#define     DEFAULT_GAP_SIZE        0   //Kilo bytes
#define     DEFAULT_TOTAL_SIZE      32  //Mega bytes
    int      val = 0;
    char     partition[32] = "";
    char     mount_piont[64] = "";
    
    struct file_transmit    test_para;
    char* ptr_mount = NULL;
    test_para.total_size = 1024*DEFAULT_TOTAL_SIZE;
    test_para.blocksize = DEFAULT_BLOCK_SIZE;
    test_para.gapsize = DEFAULT_GAP_SIZE;
    test_para.filenum = 1;
    test_para.repeatnum = 1;
    test_para.frame_control = 0;
    test_para.command = 0;
    test_para.int_flag = 0;

    signal(SIGINT, signal_handler);

    if(argc > 1) {
        if(0 == strcmp(argv[1], "multiwrite")){//create some files, t=0 will delete all
            test_para.command = MULTIP_FILE_WRITE;
        }else if(0 == strcmp(argv[1], "write_speed")){
            test_para.command = WRITE_SPEED_TEST;
        }else if(0 == strcmp(argv[1], "read_speed")){
            test_para.command = READ_SPEED_TEST;
        }else if(0 == strcmp(argv[1], "frame")){
            test_para.frame_control = 1;
            test_para.command = FRAME_STREAM_TEST;
        }else if(0 == strcmp(argv[1], "status")){
            test_para.command = STATISTIC;
        }else{
            printf("Invalid command, please check: \n");
            print_usage_1(argv[0]);
            print_usage_2(argv[0]);
            return -1;
        }
    }else {
    	print_usage_1(argv[0]);
        print_usage_2(argv[0]);
        return -1;
    }

    while ((val = getopt_long(argc-1, argv+1, mmc_short_options,
					mmc_long_options, NULL)) != -1) {
		switch (val) {
            case 'p':
				memcpy(partition, optarg, strlen(optarg)+1);
				break;
			case 'm':
				memcpy(mount_piont, optarg, strlen(optarg)+1);
				break;
			case 'b':
				test_para.blocksize = strtol(optarg, NULL, 0);
				break;
			case 'g':
				test_para.gapsize = strtol(optarg, NULL, 0);
				break;
            case 's':
				test_para.total_size = 1024*strtol(optarg, NULL, 0);
				break;
            case 'n':
				test_para.filenum = strtol(optarg, NULL, 0);
				break;
            case 'r':
				test_para.repeatnum = strtol(optarg, NULL, 0);
				break;
            case 'i':
				test_para.int_flag = strtol(optarg, NULL, 0);
				break;
			default:
				printf("Invalid argument\n");
				return -1;
		}
	}

    if(strlen(mount_piont) <= 0){
        printf("incorrect -m parameter, mount_piont not assigned!\n");
        return -1;
    }

    if(strstr(argv[0], "mmc_test") && (strlen(partition) > 0)){
        ptr_mount = (char*)malloc(strlen(partition)+strlen(mount_piont) + 32);
        if(NULL == ptr_mount){
            printf("mount command malloc failed!\n");
            return -1;
        }
        sprintf(ptr_mount, "mount %s %s", partition, mount_piont);
        system(ptr_mount);
    }

    snprintf(test_para.filename, sizeof(test_para.filename), "%s/rwtest", mount_piont);

    pthread_mutex_init(&write_lock, NULL);
    p_file_buf = (char*)malloc(test_para.blocksize*1024);
    if(NULL == p_file_buf){
        printf("p_file_buf malloc failed!\n");
        return -1;
    }

    test_para.blocknum = test_para.total_size/test_para.blocksize;

    if(test_para.filenum < 1) return -1;

    switch (test_para.command) {
        case WRITE_SPEED_TEST:
            test_multifile_write(&test_para);
            break;
        case READ_SPEED_TEST:
            test_multifile_read(&test_para);
            break;
        case CREATE_FILE_WRITE:
        case MULTIP_FILE_WRITE:
        case FRAME_STREAM_TEST:
            test_multifile_write(&test_para);
            break;
        case STATISTIC:
            test_view_statistic();
            break;
        default:
            printf("Invalid argument\n");
            break;
    }

    if(p_file_buf){
        free(p_file_buf);
        p_file_buf = NULL;
    }

    if(ptr_mount){
        sprintf(ptr_mount, "umount %s", mount_piont);
        system(ptr_mount);
        free(ptr_mount);
        ptr_mount = NULL;
    }
    return 0;
}
