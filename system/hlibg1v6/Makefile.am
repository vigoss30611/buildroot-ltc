AUTOMAKE_OPTIONS=foreign
lib_common_SOURCES = source/h264high/h264hwd_asic.c \
			source/h264high/h264hwd_decoder.c \
			source/h264high/h264hwd_dpb.c \
			source/h264high/h264hwd_macroblock_layer.c \
			source/h264high/legacy/h264hwd_byte_stream.c \
			source/h264high/legacy/h264hwd_vlc.c \
			source/h264high/legacy/h264hwd_vui.c \
			source/h264high/legacy/h264hwd_seq_param_set.c \
			source/h264high/legacy/h264hwd_slice_group_map.c \
			source/h264high/legacy/h264hwd_nal_unit.c \
			source/h264high/legacy/h264hwd_pic_param_set.c \
			source/h264high/legacy/h264hwd_util.c \
			source/h264high/legacy/h264hwd_slice_header.c \
			source/h264high/legacy/h264hwd_pic_order_cnt.c \
			source/h264high/legacy/h264hwd_neighbour.c \
			source/h264high/legacy/h264hwd_stream.c \
			source/h264high/h264decapi.c \
			source/h264high/h264hwd_cabac.c \
			source/h264high/h264hwd_cavlc.c \
			source/h264high/h264hwd_conceal.c \
			source/h264high/h264hwd_slice_data.c \
			source/h264high/h264hwd_inter_prediction.c \
			source/h264high/h264hwd_storage.c \
			source/h264high/h264decapi_e.c \
			source/h264high/h264_pp_pipeline.c \
			source/h264high/h264hwd_intra_prediction.c \
			source/jpeg/jpegdecapi.c \
			source/jpeg/jpegdecinternal.c \
			source/jpeg/jpegdecutils.c \
			source/jpeg/jpegdecscan.c \
			source/jpeg/jpegdechdrs.c \
			source/pp/ppapi.c \
			source/pp/ppinternal.c \
			source/common/workaround.c \
			source/common/regdrv.c \
			source/common/bqueue.c \
			source/common/tiledref.c \
			source/common/refbuffer.c \
			linux/dwl/dwl_linux_lock.c \
			linux/dwl/dwl_x170_linux_irq.c\
			linux/dwl/dwl_linux.c\
			linux/jpeg/jpegasicdbgtrace.c


#ACLOCAL_AMFLAGS = -I m4
#get static lib
EXTRA_CFLAGS=  -g -std=c99 -Wall -Os -fvisibility=hidden -D_GUN_SOURCE -pedantic -DNDEBUG -DION_USE\
			  $(LIBFR_CFLAGS) \
			  -DDEC_X170_OUTPUT_FORMAT=DEC_X170_OUTPUT_FORMAT_RASTER_SCAN \
			  -DDEC_X170_TIMEOUT_LENGTH=-1 \
			  -DTARGET_SYSTEM=FS_LINUX -DDEC_MODULE_PATH=\"/dev/vdec_g1\" -DMEMALLOC_MODULE_PATH=\"/dev/memalloc\" \
				-I$(srcdir)/include  -I$(srcdir) \
				-I$(top_srcdir)/linux/ldriver/kernel_26x \
			   -I$(top_srcdir)/source/inc \
			   -I$(top_srcdir)/source/config \
			   -I$(top_srcdir)/source/h264high \
			   -I$(top_srcdir)/source/h264high/legacy \
			   -I$(top_srcdir)/source/jpeg \
			   -I$(top_srcdir)/source/pp \
			   -I$(top_srcdir)/source/common \
			   -I$(top_srcdir)/source/vp6 \
			   -I$(top_srcdir)/source/mpeg4 \
			   -I$(top_srcdir)/source/avs \
			   -I$(top_srcdir)/linux/memalloc \
			   -I$(top_srcdir)/linux/vc1 \
			   -I$(top_srcdir)/linux/config \
			   -I$(top_srcdir)/linux/mpeg2 \
			   -I$(top_srcdir)/linux/vp8 \
			   -I$(top_srcdir)/linux/h264high \
			   -I$(top_srcdir)/linux/pp \
			   -I$(top_srcdir)/linux/vp6 \
			   -I$(top_srcdir)/linux/jpeg \
			   -I$(top_srcdir)/linux/mpeg4 \
			   -I$(top_srcdir)/linux/avs \
			   -I$(top_srcdir)/linux/rv 

EXTRA_LDLAGS=  -avoid-version 
#lib_LIBRARIES=libg1v6.a
#hlibg1v6_a_SOURCES=${lib_common_SOURCES}
#hlibg1v6_a_CFLAGS=${AM_CFLAGS}

#get shared lib
lib_LTLIBRARIES=libg1v6.la
libg1v6_la_CPPFLAGS=$(EXTRA_CFLAGS)
libg1v6_la_LDFLAGS= -avoid-version
libg1v6_la_SOURCES=${lib_common_SOURCES}

##get testbench
#bin_PROGRAMS=testbench
#testbench_SOURCES=test_log.c 
