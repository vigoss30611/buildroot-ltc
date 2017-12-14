CC=arm-hismall-linux-gcc
CFLAGS += -I. -I../lib/inc  -lpthread

default:mt_sample

mt_sample:
	$(CC) $(CFLAGS) mt_sample.c  -o mt_sample ../lib/libmtran.a

clean:
	rm mt_sample *.o -Rf