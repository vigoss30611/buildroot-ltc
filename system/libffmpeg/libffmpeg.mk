
################################################################################
#
# libffmpeg
#
################################################################################

LIBFFMPEG_VERSION = 0.8.15
LIBFFMPEG_SOURCE =
LIBFFMPEG_SITE  =

LIBFFMPEG_LICENSE =
LIBFFMPEG_LICENSE_FILES = README

LIBFFMPEG_MAINTAINED = YES
LIBFFMPEG_AUTORECONF = NO
LIBFFMPEG_INSTALL_STAGING = YES
LIBFFMPEG_DEPENDENCIES = host-pkgconf
#LIBFFMPEG_CONF_OPT =


ifeq ($(BR2_PACKAGE_LIBFFMPEG_ENCODER),y)
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_ENCODER_LIBFDK_AAC),y)
		LIBFFMPEG_DEPENDENCIES += libfaac
		LIBFFMPEG_CONFIG_ENCODER += --enable-gpl
		LIBFFMPEG_CONFIG_ENCODER += --enable-nonfree
		LIBFFMPEG_CONFIG_ENCODER += --enable-libfdk-aac
		LIBFFMPEG_CONFIG_ENCODER += --enable-encoder=libfdk_aac
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_ENCODER_AAC),y)
		LIBFFMPEG_CONFIG_ENCODER += --enable-encoder=aac
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_ENCODER_AC3),y)
		LIBFFMPEG_CONFIG_ENCODER += --enable-encoder=ac3
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_ENCODER_PCM_ALAW),y)
		LIBFFMPEG_CONFIG_ENCODER += --enable-encoder=pcm_alaw
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_ENCODER_PCM_G726),y)
		LIBFFMPEG_CONFIG_ENCODER += --enable-encoder=adpcm_g726
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_ENCODER_MJPEG),y)
		LIBFFMPEG_CONFIG_ENCODER += --enable-encoder=mjpeg
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_ENCODER_H264),y)
		LIBFFMPEG_CONFIG_ENCODER += --enable-encoder=h264
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_ENCODER_HEVC),y)
		LIBFFMPEG_CONFIG_ENCODER += --enable-encoder=hevc
	endif
else
LIBFFMPEG_CONFIG_ENCODER += --disable-encoders
endif

ifeq ($(BR2_PACKAGE_LIBFFMPEG_DECODER),y)
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_DECODER_LIBFDK_AAC),y)
		LIBFFMPEG_DEPENDENCIES += fdk-aac
		LIBFFMPEG_CONFIG_DECODER += --enable-gpl
		LIBFFMPEG_CONFIG_DECODER += --enable-nonfree
		LIBFFMPEG_CONFIG_DECODER += --enable-libfdk-aac
		LIBFFMPEG_CONFIG_DECODER += --enable-decoder=libfdk_aac
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_DECODER_AAC),y)
		LIBFFMPEG_CONFIG_DECODER += --enable-decoder=aac
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_DECODER_AC3),y)
		LIBFFMPEG_CONFIG_DECODER += --enable-decoder=ac3
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_DECODER_PCM_ALAW),y)
		LIBFFMPEG_CONFIG_DECODER += --enable-decoder=pcm_alaw
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_DECODER_PCM_G726),y)
		LIBFFMPEG_CONFIG_DECODER += --enable-decoder=adpcm_g726
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_DECODER_MJPEG),y)
		LIBFFMPEG_CONFIG_DECODER += --enable-decoder=mjpeg
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_DECODER_H264),y)
		LIBFFMPEG_CONFIG_DECODER += --enable-decoder=h264
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_DECODER_HEVC),y)
		LIBFFMPEG_CONFIG_DECODER += --enable-decoder=hevc
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_DECODER_MP3),y)
		LIBFFMPEG_CONFIG_DECODER += --enable-decoder=mp3
	endif
else
LIBFFMPEG_CONFIG_DECODER += --disable-decoders
endif

ifeq ($(BR2_PACKAGE_LIBFFMPEG_PARSER),y)
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_PARSER_MJPEG),y)
		LIBFFMPEG_CONFIG_PARSER += --enable-parser=mjpeg
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_PARSER_AAC),y)
		LIBFFMPEG_CONFIG_PARSER += --enable-parser=aac
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_PARSER_H264),y)
		LIBFFMPEG_CONFIG_PARSER += --enable-parser=h264
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_PARSER_HEVC),y)
		LIBFFMPEG_CONFIG_PARSER += --enable-parser=hevc
	endif
else
LIBFFMPEG_CONFIG_PARSER += --disable-parsers
endif

#handle muxer
ifeq ($(BR2_PACKAGE_LIBFFMPEG_MUXER),y)
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_MUXER_MP4_MOV),y)
		LIBFFMPEG_CONFIG_MUXER += --enable-muxer=mp4
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_MUXER_MKV),y)
		LIBFFMPEG_CONFIG_MUXER += --enable-muxer=matroska
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_MUXER_MJPEG),y)
		LIBFFMPEG_CONFIG_MUXER += --enable-muxer=mjpeg
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_MUXER_WAV),y)
		LIBFFMPEG_CONFIG_MUXER += --enable-muxer=wav
	endif
else
LIBFFMPEG_CONFIG_MUXER += --disable-muxers
endif

ifeq ($(BR2_PACKAGE_LIBFFMPEG_DEMUXER),y)
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_DEMUXER_MP4_MOV),y)
		LIBFFMPEG_CONFIG_DEMUXER += --enable-demuxer=mp4
		LIBFFMPEG_CONFIG_DEMUXER += --enable-demuxer=mov --enable-demuxer=avi
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_DEMUXER_MKV),y)
		LIBFFMPEG_CONFIG_DEMUXER += --enable-demuxer=matroska
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_DEMUXER_MJPEG),y)
		LIBFFMPEG_CONFIG_DEMUXER += --enable-demuxer=mjpeg,image_jpeg_pipe
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_DEMUXER_WAV),y)
		LIBFFMPEG_CONFIG_DEMUXER += --enable-demuxer=wav
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_DEMUXER_MP3),y)
		LIBFFMPEG_CONFIG_DEMUXER += --enable-demuxer=mp3
	endif
else
LIBFFMPEG_CONFIG_DEMUXER += --disable-demuxers
endif


ifeq ($(BR2_PACKAGE_LIBFFMPEG_AVFILTER),y)
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_AVFILTER_CROP),y)
		LIBFFMPEG_CONFIG_AVFILTER += --enable-filter=crop
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_AVFILTER_SCALE),y)
		LIBFFMPEG_CONFIG_AVFILTER += --enable-filter=scale
	endif
else
LIBFFMPEG_CONFIG_AVFILTER += --disable-avfilter
endif

ifeq ($(BR2_PACKAGE_LIBFFMPEG_BSF_FILTER),y)
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_BSF_FILTER_H264_MP4TOANNEXB),y)
		LIBFFMPEG_CONFIG_BSF_FILTER += --enable-bsf=h264_mp4toannexb
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_BSF_FILTER_HEVC_MP4TOANNEXB),y)
		LIBFFMPEG_CONFIG_BSF_FILTER += --enable-bsf=hevc_mp4toannexb
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_BSF_FILTER_AAC_ADTSTOASC),y)
		LIBFFMPEG_CONFIG_BSF_FILTER += --enable-bsf=aac_adtstoasc
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_BSF_FILTER_MJPEG2JPEG),y)
		LIBFFMPEG_CONFIG_BSF_FILTER += --enable-bsf=mjpeg2jpeg
	endif
else
LIBFFMPEG_CONFIG_BSF_FILTER += --disable-bsfs
endif



ifeq ($(BR2_PACKAGE_LIBFFMPEG_SWRESAMPLE),y)
	LIBFFMPEG_CONFIG_SWRESAMPLE += --enable-swresample
else
	LIBFFMPEG_CONFIG_SWRESAMPLE += --disable-swresample
endif

ifeq ($(BR2_PACKAGE_LIBFFMPEG_SWSCALE),y)
	LIBFFMPEG_CONFIG_SWSCALE += --enable-swscale
else
	LIBFFMPEG_CONFIG_SWSCALE += --disable-swscale
endif

ifeq ($(BR2_PACKAGE_LIBFFMPEG_PROGRAMES),y)
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_FFMPEG),y)
		LIBFFMPEG_CONFIG_TOOLS += --enable-ffmpeg
	endif
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_FFPROBE),y)
		LIBFFMPEG_CONFIG_TOOLS += --enable-ffprobe
	endif
else
LIBFFMPEG_CONFIG_TOOLS += --disable-programs
endif

ifeq ($(BR2_PACKAGE_LIBFFMPEG_PROTOCOL),y)
	ifeq ($(BR2_PACKAGE_LIBFFMPEG_PROTOCOL_HTTP),y)
		LIBFFMPEG_CONFIG_PROTOCOL += --enable-network
		LIBFFMPEG_CONFIG_PROTOCOL += --enable-protocol=http
	endif
else
LIBFFMPEG_CONFIG_PROTOCOL += --disable-network
endif

#LIBFFMPEG_CONFIG_TOOLS += --disable-avfilter
#LIBFFMPEG_CONFIG_TOOLS += --disable-avdevice
LIBFFMPEG_CONFIG_MIN += --disable-pixelutils
LIBFFMPEG_CONFIG_MIN += --disable-swscale-alpha

define LIBFFMPEG_AUTORECONF_CMDS
	echo "autoreconfig"
endef
define LIBFFMPEG_CONFIGURE_CMDS
	@echo "################################"
	@echo "configure para ---->"
	@echo  tools $(LIBFFMPEG_CONFIG_TOOLS)
	@echo  muxer $(LIBFFMPEG_CONFIG_MUXER)
	(cd $(@D) && rm -rf config.cache &&\
	$(TARGET_CONFIGURE_OPTS) \
	$(TARGET_CONFIGURE_ARGS) \
	$(LIBFFMPEG_CONF_ENV) \
		./configure  --enable-cross-compile --target-os=linux \
		--arch=$(BR2_ARCH) \
		--extra-cflags="-fPIC -march=armv7-a -mtune=cortex-a5  -mcpu=cortex-a5" \
		--enable-neon\
		--cross-prefix=$(TARGET_CROSS) \
		--sysroot=$(STAGING_DIR) \
		--host-cc="$(HOSTCC)" \
		--disable-everything \
		--enable-small \
		--enable-nonfree \
		--enable-gpl \
		--disable-doc \
		--disable-podpages\
		--disable-avdevice\
		--disable-postproc\
		--enable-protocol=file\
		$(LIBFFMPEG_CONFIG_MIN)\
		$(LIBFFMPEG_CONFIG_TOOLS)\
		$(LIBFFMPEG_CONFIG_MUXER)\
		$(LIBFFMPEG_CONFIG_DEMUXER)\
		$(LIBFFMPEG_CONFIG_BSF_FILTER)\
		$(LIBFFMPEG_CONFIG_ENCODER)\
		$(LIBFFMPEG_CONFIG_DECODER)\
		$(LIBFFMPEG_CONFIG_PARSER)\
		$(LIBFFMPEG_CONFIG_SWRESAMPLE)\
		$(LIBFFMPEG_CONFIG_SWSCALE)\
		$(LIBFFMPEG_CONFIG_AVFILTER)\
		$(LIBFFMPEG_CONFIG_PROTOCOL)\
    	--disable-debug \
		--disable-armv5te\
		--disable-armv6\
		--disable-armv6t2\
    	--enable-asm \
    	--enable-thumb \
        --enable-static \
		--enable-shared \
		--disable-iconv )
endef


#install headers
define LIBFFMPEG_POST_INSTALL_STAGING_HEADERS
	@echo "+++++++++++++++++install staging header++++++++++++++++++++++"
#	mkdir -p $(STAGING_DIR)/usr/include/ffmpeg
#	mkdir -p $(STAGING_DIR)/usr/include/ffmpeg/libavformat
#	mkdir -p $(STAGING_DIR)/usr/include/ffmpeg/libavutil
#	mkdir -p $(STAGING_DIR)/usr/include/ffmpeg/libavcodec
#	cp -rfv $(@D)/*.h  $(STAGING_DIR)/usr/include/ffmpeg
	#cp -rfv $(@D)/libavformat/*.pc  $(STAGING_DIR)/usr/lib/pkgconfig
	#cp -rfv $(@D)/libavutil/*.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
	#cp -rfv $(@D)/libavcodec/*.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
	#cp -rfv $(@D)/libswscale/*.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
	#cp -rfv $(@D)/libswresample/*.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
	cp -rfv $(@D)/libavformat/url.h   $(STAGING_DIR)/usr/local/include/libavformat/
endef
LIBFFMPEG_POST_INSTALL_STAGING_HOOKS  += LIBFFMPEG_POST_INSTALL_STAGING_HEADERS

define LIBFFMPEG_POST_INSTALL_TARGET_SHARED
	@echo "++++++++++++++++install target +++++++++++++++++++++++"
	rm -rf $(TARGET_DIR)/usr/local/include
	rm -rf $(TARGET_DIR)/usr/local/lib/pkgconfig
	mv $(TARGET_DIR)/usr/local/lib/*  $(TARGET_DIR)/usr/lib/
endef
LIBFFMPEG_POST_INSTALL_TARGET_HOOKS  += LIBFFMPEG_POST_INSTALL_TARGET_SHARED
$(eval $(autotools-package))
