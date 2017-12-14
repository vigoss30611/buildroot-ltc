#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "speex/speex_echo.h"
#include "speex/speex_preprocess.h"
#include <sys/time.h>

//#define NN 128
//#define TAIL 1024

int main(int argc, char **argv)
{
   int echo_fd, ref_fd, e_fd;
   int NN=128;
   int TAIL=1024;
   int time=10;
   NN=atoi(argv[1]);
   TAIL=atoi(argv[2]);
   time=atoi(argv[3]);
   printf("hello22 %d,%d,%d\n",NN,TAIL,time);
   short echo_buf[NN], ref_buf[NN], e_buf[NN];
   SpeexEchoState *st;
   SpeexPreprocessState *den;

  /* if (argc != 4)
   {
      fprintf (stderr, "testecho mic_signal.sw speaker_signal.sw output.sw\n");
      exit(1);
   }
      printf("hello 8 %s,%s,%s\n",argv[1],argv[2],argv[3]);*/
   echo_fd = open ("cap1", O_RDONLY);
   ref_fd  = open ("ain",  O_RDONLY);
   e_fd    = open ("aout", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    
   st = speex_echo_state_init(NN, TAIL);
   den = speex_preprocess_state_init(NN, 8000);
   int tmp = 8000;
   speex_echo_ctl(st, SPEEX_ECHO_SET_SAMPLING_RATE, &tmp);
   speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_ECHO_STATE, st);
     struct timeval tv_begin, tv_end;
   printf("hello speex aec usleep 10\n");
     while (read(ref_fd, ref_buf, NN*2))
   {
      read(echo_fd, echo_buf, NN*2);
      gettimeofday(&tv_begin, NULL);
      speex_echo_cancellation(st, ref_buf, echo_buf, e_buf);
      speex_preprocess_run(den, e_buf);
       gettimeofday(&tv_end, NULL);
      // printf("before:%u.%u, now time:%u.%u,\n",tv_begin.tv_sec, (tv_begin.tv_usec/1000),tv_end.tv_sec, (tv_end.tv_usec/1000));
      write(e_fd, e_buf, NN*2);
        usleep(time*1000);
   }
   printf("echo end\n");
   speex_echo_state_destroy(st);
   speex_preprocess_state_destroy(den);
   close(e_fd);
   close(echo_fd);
   close(ref_fd);
   return 0;
}
