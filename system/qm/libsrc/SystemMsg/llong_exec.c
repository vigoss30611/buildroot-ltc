
#include <stdlib.h> //for atof()
#include <stdio.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <stdint.h>
#include <fcntl.h>
#include <sys/mount.h> //BLKFLSBUF

//#include "exec_tools.h"
#include "llong_exec.h"

#define MAX_SEEK_LEN 60

char* get_str(char* str, char* res_str)
{
	int i = 0, j;
	char buf[50];
	
	for(i = 0; str[i] == ' '; i++)
	{
		if(i >= MAX_SEEK_LEN)
			return 0;
	}

	j = i;
	for(i = 0; (str[j] != ' '); j++, i++)
	{
		buf[i] = str[j];
	}

	buf[i] = 0;

	strcpy(res_str, buf);

//	lvr_printf("%d, j = %d", *num, j);
	return str+j+1;
}


char* get_num(char* str, int* num)
{
	int i = 0, j;
	char buf[50];

//	printf("get num %s\n", str);
//	fflush(0);
	
	for(i = 0; (str[i] != '-') && (str[i] > '9' || str[i] < '0'); i++)
	{
		if(i >= MAX_SEEK_LEN)
		{
//			printf("search i = %d\n", i);
//			fflush(0);
			return 0;
		}
	}

//	printf("start from %d\n", i);
//	fflush(0);
	
	j = i;
	for(i = 0; str[j] == '-' || (str[j] <= '9' && str[j] >= '0'); j++, i++)
	{
		buf[i] = str[j];
	}

	buf[i] = 0;

	*num = atoi(buf);

//	printf("buf, %s", buf);
//	fflush(0);

	return str+j;
}

char* get_double(char* str, double* num)
{
	int i = 0, j;
	char buf[50];

//	printf("get num %s\n", str);
//	fflush(0);
	
	for(i = 0; (str[i] != '-') && (str[i] > '9' || str[i] < '0'); i++)
	{
		if(i >= MAX_SEEK_LEN)
		{
//			printf("search i = %d\n", i);
//			fflush(0);
			return 0;
		}
	}

//	printf("start from %d\n", i);
//	fflush(0);
	
	j = i;
	for(i = 0; str[j] == '-' || (str[j] <= '9' && str[j] >= '0'); j++, i++)
	{
		buf[i] = str[j];
	}

	buf[i] = 0;

	*num = atof(buf);
	//printf("buf, %s, v = %f\n", buf, *num);

//	fflush(0);

	return str+j;
}

static int poll_file(int fd)
{
	int n = 1;
	struct timeval tv;
	fd_set rfds;

	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	tv.tv_sec = 1;
	tv.tv_usec = 0; //wait 1s
	n = select(fd+1, &rfds, 0, 0, &tv);
	if (n > 0) {
		return n;
	}
	return -1;
}

static int timer_expired(struct timeval * tv0, int ms)
{
	struct timeval tv1;
	
	gettimeofday(&tv1, 0);

	if((tv1.tv_sec*1000+tv1.tv_usec/1000) - 
		(tv0->tv_sec*1000+tv0->tv_usec/1000) > ms)
		return 1;

	return 0;
}

int llong_popen(const char *cmdstring, const char *type, pid_t* child_pid)
{
    int     pfd[2], fd;
	pid_t  pid;

    /* only allow "r" or "w" */
    if ((type[0] != 'r' && type[0] != 'w') || type[1] != 0) {
        return 0;
    }

    if (pipe(pfd) < 0)
        return 0;   /* errno set by pipe() */

    if ((pid = vfork()) < 0) {
        return 0;   /* errno set by fork() */
    } else if (pid == 0) {                           /* child */
		setpgid(getpid(), getpid());
        if (*type == 'r') {
            if(close(pfd[0]) == -1)
            {
            	perror("fail to close extra pipe0");
				return 0;
            }
			
            if (pfd[1] != STDOUT_FILENO) {
                if(dup2(pfd[1], STDOUT_FILENO) == -1)
                {
	                perror("Failed to redirect stdout");
					return 0;
                }
	            if(close(pfd[1]) == -1)
	            {
	            	perror("fail to close extra pipe1");
					return 0;
	            }
            }
        } else {
            if(close(pfd[1]) == -1)
            {
            	perror("fail to close extra pipe1");
				return 0;
            }
            if (pfd[0] != STDIN_FILENO) {
                if(dup2(pfd[0], STDIN_FILENO) == -1)
                {
	                perror("Failed to redirect stdin");
					return 0;
                }
	            if(close(pfd[0]) == -1)
	            {
	            	perror("fail to close extra pipe0");
					return 0;
	            }
            }
        }

        execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
        _exit(127);
    }

	setpgid(pid, pid);
    /* parent continues... */
    if (*type == 'r') {
        if(close(pfd[1]) == -1)
        {
        	perror("fail to close extra pipe1");
			return 0;
        }
//        if ((fp = fdopen(pfd[0], type)) == NULL)
//            return(NULL);
		fd = pfd[0];
    } else {
        if(close(pfd[0]) == -1)
        {
        	perror("fail to close extra pipe0");
			return 0;
        }
//       if ((fp = fdopen(pfd[1], type)) == NULL)
//            return(NULL);
		fd = pfd[1];
    }

    *child_pid = pid; /* remember child pid for this fd */
//    return(fp);
	return fd;
}


int llong_pclose(int fd, pid_t pid, int wait_sec, int* status)
{
    int  ret = 0;
	pid_t wait_res;

	wait_sec *= 10;

//    fd = fileno(fp);

//    if (fclose(fp) == EOF)
//        return(-1);
	close(fd);

    while (1)
    {
    	wait_res = waitpid(pid, status, WNOHANG);

		if(wait_res < 0)
		{
	        if (errno != EINTR)
	        {     /* error other than EINTR from waitpid() */
	        	ret = -1;
				break;
	        }
		}
		else if(wait_res == 0)
		{
			if(wait_sec-- > 0)
				usleep(100000);
			else
			{
				kill(pid, SIGKILL);
				ret = -2;
				break;
			}
		}
		else
			break;
    }

    return ret;   /* return child's termination status */
}


char* llong_fgets(int fd, char* line, int num, int timeout)
{
	int ret, i, nread = 0;

//	printf("llong_fgets\n");
//	fflush(0);
	
	for(i = 0; i < timeout/100; i++)
	{
		if((ret = read(fd, line+nread, num-nread)) < 0)
		{//read error
			//printf("\nfgets1\n");
			//perror("llong_fgets read");
			return 0;
		}
		else if(ret == 0)
		{//end of file?
			//printf("\nfgets2\n");
			//printf("end of file\n");
			//fflush(0);

			return (char*)1;
		}
		else
		{
			//printf("\nfgets3\n");
			nread += ret;
			line[nread] = 0;
//			printf(line);
//			printf("%d\n", nread);
//			fflush(0);
			if(strchr(line, '\n'))
				return line;
		}

		usleep(100000);
	}

	//printf("\nfgets4\n");

	return 0;
}


//针对只是简单执行命令行，无过程参数输入的命令的执行
int tools_exec(char* cmd, int timeout, char* ret_str, int out_msg, int* status)
{
	char *str, line[1024*80], *got_line;
	struct timeval tv0;
	//int is_timer_expired, err = 0;
	int err = 0;
//	FILE* fp;
	int fd, exit_status = 0;
	pid_t child_pid;

	if(out_msg)
	{
		printf(cmd);
		printf("\n");
		fflush(0);
	}
	
	fd = llong_popen(cmd, "r", &child_pid);
	if(fd <= 0)
	{
		sprintf(line, "exec open %s fail\n", cmd);
		perror(line);
		fflush(0);
		return EXEC_ERR_CMD_OPEN;
	}

	gettimeofday(&tv0, 0);
	//is_timer_expired = 0;

	while(1)
	{
		if(timer_expired(&tv0, timeout)) //超时，毫秒为单位
		{
			printf("exec timeout\n");
			fflush(0);
			err = EXEC_ERR_TIMEOUT;
			break;
		}

//		printf("poll for some external report\n");
//		fflush(0);
		if(poll_file(fd) > 0)
		{
//			printf("recv some external report\n");
//			fflush(0);
			line[0] = 0;
			if((got_line = llong_fgets(fd, line, sizeof(line), timeout)))
//			if((fgets(line, sizeof(line), fp)))
			{	
				if(got_line == (char*)1)
				{// end of file
					err = 0;
					break;
				}
				
				if(out_msg && (strlen(line) < 1024))
				{//太多输出不打印
					printf(line);
//					printf("\n");
					fflush(0);
				}

				if((str = strstr(line, "[pppf]:")))
				{
					if((str = get_num(str, &err)))
					{
						if(ret_str)
						{
							if(strlen(str) > 256)
							{
								memcpy(ret_str, str, 256);
								ret_str[256] = 0;
							}
							else
								strcpy(ret_str, str);
						}
						break;						
					}
					else
					{
						err = EXEC_ERR_CMD_RETURN;
						printf("exec error cmd return\n");
						fflush(0);
						break;
					}
				}
			}
			else
			{
				printf("exec read error\n");
				fflush(0);
				err = 0;
				break;
			}
		}

//		usleep(100000);
	}

	if(err == EXEC_ERR_TIMEOUT)
		llong_pclose(fd, child_pid, 0, &exit_status);
	else
		llong_pclose(fd, child_pid, 2, &exit_status);	

	if(status)
		*status = exit_status;
	
	return err;
}
