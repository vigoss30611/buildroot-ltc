AUTOMAKE_OPTIONS=foreign
lib_common_SOURCES=software/source/common/errorhandling.c  \
			software/source/common/sw_util.c  \
			software/source/common/commonconfig.c  \
			software/source/common/decapi.c  \
			software/source/common/fifo.c  \
			software/source/common/regdrv.c  \
			software/source/common/bqueue.c  \
			software/source/common/raster_buffer_mgr.c  \
			software/source/common/sw_stream.c  \
			software/source/hevc/hevc_slice_header.c  \
			software/source/hevc/hevc_pic_param_set.c  \
			software/source/hevc/hevc_storage.c  \
			software/source/hevc/hevc_fb_mngr.c  \
			software/source/hevc/hevc_byte_stream.c  \
			software/source/hevc/hevc_decoder.c  \
			software/source/hevc/hevc_seq_param_set.c  \
			software/source/hevc/hevc_vui.c  \
			software/source/hevc/hevc_dpb.c  \
			software/source/hevc/hevc_asic.c  \
			software/source/hevc/hevc_util.c  \
			software/source/hevc/hevc_exp_golomb.c  \
			software/source/hevc/hevc_nal_unit.c  \
			software/source/hevc/hevcdecapi.c  \
			software/source/hevc/hevc_pic_order_cnt.c  \
			software/linux/dwl/dwl_linux_hw.c  \
			software/linux/dwl/dwl_linux.c  \
			software/linux/dwl/dwl_buf_protect.c  \
			software/test/common/swhw/tb_params.c \
			software/test/common/command_line_parser.c \
			software/test/common/swhw/tb_cfg.c  \
			software/test/common/null_trace.c  \
			software/linux/dwl/dwl_activity_trace.c


#ACLOCAL_AMFLAGS = -I m4
#get static lib
EXTRA_CFLAGS=  -g -std=c99 -Os -fvisibility=hidden -DFIFO_DATATYPE=void* -D_ASSERT_USED \
			   -DNAME_MAX=64 -DCLEAR_OUT_BUFFER \
			   -D_RANGE_CHECK -D_ERROR_PRINT -DDEBUG -DENABLE_HEVC_SUPPORT \
			   -I$(top_srcdir)/include  -I$(top_srcdir)  $(LIBHWHEADER_CFLAGS)  -DDOWN_SCALER \
			   $(LIBFR_CFLAGS)\
           -I$(top_srcdir)/software/source/inc \
           -I$(top_srcdir)/software/source/hevc \
           -I$(top_srcdir)/software/source/config \
           -I$(top_srcdir)/software/source/dwl \
           -I$(top_srcdir)/software/source/common \
           -I$(top_srcdir)/software/source/vp9 \
           -I$(top_srcdir)/software/linux/memalloc \
           -I$(top_srcdir)/software/linux/dwl \
           -I$(top_srcdir)/system/models/g2hw \
           -I$(top_srcdir)/software/linux/ldriver  \
           -I$(top_srcdir)/software/test/common/swhw \
           -I$(top_srcdir)/system/models/g2hw/streamd_internal \
           -I$(top_srcdir)/system/models/g2hw/intrap_internal 

EXTRA_LDLAGS=  -avoid-version $(LIBFR_LIBS)
lib_LIBRARIES=libg2v1.a
libg2v1_a_SOURCES=${lib_common_SOURCES}
libg2v1_a_CFLAGS=${EXTRA_CFLAGS} $(LIBFR_CFLAGS)

#get shared lib
lib_LTLIBRARIES=libg2v1.la
libg2v1_la_CPPFLAGS= $(EXTRA_CFLAGS) $(LIBFR_CFLAGS)
libg2v1_la_LDFLAGS= -avoid-version
libg2v1_la_SOURCES=${lib_common_SOURCES}



##get testbench
bin_PROGRAMS=testbench_g2 testbench_hevc_g2
testbench_common_SOURCES= software/test/common/command_line_parser.c    \
                   software/test/common/swhw/tb_cfg.c    \
                   software/test/common/swhw/tb_params.c    \
                   software/test/common/swhw/tb_tiled.c    \
                   software/test/common/swhw/tb_stream_corrupt.c    \
                   software/test/common/swhw/tb_md5.c    \
                   software/test/common/swhw/md5.c    \
                   software/test/common/md5_sink.c    \
                   software/test/common/null_sink.c    \
                   software/test/common/bytestream_parser.c    \
						 software/test/common/yuvfilters.c  \
						 software/test/common/vpxfilereader.c  \
						 software/test/common/error_simulator.c  \
                   software/test/common/file_sink.c

testbench_g2_SOURCES=software/test/common/dectestbench.c  $(testbench_common_SOURCES)
testbench_g2_CFLAGS=$(EXTRA_CFLAGS)  -DENABLE_HEVC_SUPPORT -DDOWN_SCALER
testbench_g2_LDADD=  $(top_srcdir)/libg2v1.a
testbench_g2_LDFLAGS=  -lrt -lm -pthread $(LIBFR_LIBS)

##hevc testbench
testbench_hevc_g2_SOURCES=software/test/hevc/dectestbench.c $(testbench_common_SOURCES)
testbench_hevc_g2_CFLAGS=$(EXTRA_CFLAGS)  -DENABLE_HEVC_SUPPORT -DDOWN_SCALER
testbench_hevc_g2_LDADD=  $(top_srcdir)/libg2v1.a
testbench_hevc_g2_LDFLAGS=  -lrt -lm -pthread $(LIBFR_LIBS)
