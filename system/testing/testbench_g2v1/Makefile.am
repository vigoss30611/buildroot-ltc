AUTOMAKE_OPTIONS=foreign

lib_common_SOURCES=common/dectestbench.c    \
                   common/command_line_parser.c    \
                   common/swhw/tb_cfg.c    \
                   common/swhw/tb_params.c    \
                   common/swhw/tb_tiled.c    \
                   common/swhw/tb_stream_corrupt.c    \
                   common/swhw/tb_md5.c    \
                   common/swhw/md5.c    \
                   common/md5_sink.c    \
                   common/null_sink.c    \
                   common/bytestream_parser.c    \
						  common/yuvfilters.c  \
						common/vpxfilereader.c  \
						common/error_simulator.c  \
                   common/file_sink.c



noinst_PROGRAMS=testbench_hevc_dec
testbench_hevc_dec_CPPFLAGS= -g -std=c99 	\
					-DENABLE_HEVC_SUPPORT -DDOWN_SCALER\
					-I$(top_srcdir)  \
					$(LIBFR_CFLAGS)   \
					$(LIBG2V1_CFLAGS) \
					-I$(top_srcdir)/common  \
					-I$(top_srcdir)/hevc  \
					-I$(top_srcdir)/common/swhw


testbench_hevc_dec_LDFLAGS= $(LIBFR_LIBS)  $(LIBG2V1_LIBS) -lrt -lm

testbench_hevc_dec_SOURCES=$(lib_common_SOURCES)

