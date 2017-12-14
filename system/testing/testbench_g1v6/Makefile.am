AUTOMAKE_OPTIONS=foreign

hlibg1v6_dir = ../../hlibg1v6-1.0.0

lib_common_SOURCES=common/swhw/md5.c    \
                   common/swhw/trace.c    \
                   common/swhw/tb_cfg.c    \
                   common/swhw/tb_params.c    \
                   common/swhw/tb_tiled.c    \
                   common/swhw/tb_stream_corrupt.c    \
                   common/swhw/tb_md5.c    \
				   common/utils/async_task.c \
				   common/utils/fifo.c \ 
				   pp/external_mode/frame.c \
				   pp/external_mode/pptestbench.c \
		   		   pp/external_mode/cfg.c \
				   pp/external_mode/params.c \
				   pp/external_mode/main.c 		   

noinst_PROGRAMS=testbench_g1v6
testbench_g1v6_CPPFLAGS= -g -std=c99 	\
					-DENABLE_HEVC_SUPPORT -DDOWN_SCALER\
					-I$(top_srcdir)  \
					$(LIBFR_CFLAGS)   \
					$(LIBG1V6_CFLAGS) \
					-I./common  \
					-I./jpeg  \
					-I./common/swhw \
					-I./common/utils \
					-I$(hlibg1v6_dir)/source/inc \
				 	-I$(hlibg1v6_dir)/source/pp \
				    -I$(hlibg1v6_dir)/linux/memalloc  		

testbench_g1v6_LDFLAGS= $(LIBFR_LIBS)  $(LIBG1V6_LIBS) $(LIBFR_LIBS)  -lrt -lm -lpthread

testbench_g1v6_SOURCES=$(lib_common_SOURCES)

