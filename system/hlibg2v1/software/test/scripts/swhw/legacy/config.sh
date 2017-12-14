#!/bin/bash
#-------------------------------------------------------------------------------
#-                                                                            --
#-       This software is confidential and proprietary and may be used        --
#-        only as expressly authorized by a licensing agreement from          --
#-                                                                            --
#-                            Hantro Products Oy.                             --
#-                                                                            --
#-                   (C) COPYRIGHT 2011 HANTRO PRODUCTS OY                    --
#-                            ALL RIGHTS RESERVED                             --
#-                                                                            --
#-                 The entire notice above must be reproduced                 --
#-                  on all copies and should not be removed.                  --
#-                                                                            --
#-------------------------------------------------------------------------------
#-
#--   Abstract     : Configuration script for the execution of the SW/HW      --
#--                  verification test cases                                  --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

############################ Common for all decoders ###########################

# Product
export PRODUCT="g1"
export SWTAG="sw${PRODUCT}"
export HWBASETAG="hw${PRODUCT}"
export HWCONFIGTAG="hw${PRODUCT}"

############################ Decoder configuration #############################

# Values: ENABLED & DISABLED
# HW timeout support
export DEC_HW_TIMEOUT="DISABLED"

# Values: BIG_ENDIAN & LITTLE_ENDIAN
export DEC_OUTPUT_PICTURE_ENDIAN="LITTLE_ENDIAN"

# Values: 0, 1, 4, 8, 16 or 17 for AHB
# Values: 1 - 31 for OCP
# Values: 1 - 16 for AXI
export DEC_BUS_BURST_LENGTH="16"

# Values: 0 - 4
export DEC_ASIC_SERVICE_PRIORITY="0"

# Values: RASTER_SCAN & TILED
export DEC_OUTPUT_FORMAT="RASTER_SCAN"
if [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
then
    # Force RASTER_SCAN format; do not touch
    export DEC_OUTPUT_FORMAT="RASTER_SCAN"
fi

# Values: 0 - 63
export DEC_LATENCY_COMPENSATION="0"

# Values: DISABLED & ENABLED
export DEC_CLOCK_GATING="DISABLED"

# Values: DISABLED & ENABLED
export DEC_DATA_DISCARD="DISABLED"

# Values: INTERNAL & EXTERNAL
export DEC_MEMORY_ALLOCATION="INTERNAL"

# Values: DISABLED & ENABLED
# Force RLC mode (DISABLED is for VLC mode). In RLC mode, software does entropy coding. 
# For internal use, only.
export DEC_RLC_MODE_FORCED="DISABLED"

# Maximum resolution
export DEC_MAX_OUTPUT="1920"

# Values: STRICT & LOOSE
# Handling of standard violations
# For internal use, only.
export DEC_STANDARD_COMPLIANCE="LOOSE"
if [ $PRODUCT == "8170" ]
then
    # Force STRICT for 8170
    export DEC_STANDARD_COMPLIANCE="STRICT"
fi

# Values: REF_BUFFER, REF_BUFFER2 & DISABLED
# Decoder reference buffer support
export DEC_REFERENCE_BUFFER="DISABLED"    
if [ $PRODUCT == "8170" ]
then
    # Force DISABLED for 8170
    export DEC_REFERENCE_BUFFER="DISABLED"
fi

# Values: ENABLED & DISABLED
# Decoder reference buffer interlaced support
# For internal use, only.
export DEC_REFERENCE_BUFFER_INTERLACED="ENABLED"
if [ "$PRODUCT" == "8190" ]
then
    # HOX !!! Disable for HW version <= 1.89
    export DEC_REFERENCE_BUFFER_INTERLACED="ENABLED"
fi

# Values: ENABLED & DISABLED
# Decoder reference buffer top/bottom sum support
# For internal use, only.
export DEC_REFERENCE_BUFFER_TOP_BOT_SUM="ENABLED"
if [ "$PRODUCT" == "8190" ]
then
    # HOX !!! Disable for HW version <= 2.83
    export DEC_REFERENCE_BUFFER_TOP_BOT_SUM="ENABLED"
fi

# Values: ENABLED & DISABLED
# For internal use, only
export DEC_REFERENCE_BUFFER_EVAL_MODE="ENABLED"

# Values: ENABLED & DISABLED
# For internal use, only
export DEC_REFERENCE_BUFFER_CHECKPOINT="ENABLED"

# Values: ENABLED & DISABLED
# For internal use, only
export DEC_REFERENCE_BUFFER_OFFSET="DISABLED"

export DEC_REFERENCE_BUFFER_LATENCY="20"

export DEC_REFERENCE_BUFFER_NONSEQ="8"

export DEC_REFERENCE_BUFFER_SEQ="1"

# Values: ENABLED & DISABLED
# Second chrominance table
export DEC_2ND_CHROMA_TABLE="DISABLED"
if [ "$PRODUCT" != 9170 ]
then
    export DEC_2ND_CHROMA_TABLE="DISABLED"
fi

# Values: ENABLED & DISABLED
# HW operates in tiled mode
export DEC_TILED_MODE="DISABLED"
if [ "$PRODUCT" == "8170" ] || [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ]
then
    # Force tiled mode disabled format; do not touch
    export DEC_TILED_MODE="DISABLED"
fi

export DEC_SEPARATE_FIELDS="DISABLED"

# Values: ENABLED & DISABLED
# output pictures using DecPeek() function
export TB_USE_PEEK_FOR_OUTPUT="DISABLED"

# Values: ENABLED & DISABLED
# Skip decoding non-reference pictures.
export TB_SKIP_NON_REF_PICS="DISABLED"

export HW_BUS="AHB"

export HW_DATA_BUS_WIDTH="32_BIT"

export TEST_ENV_DATA_BUS_WIDTH="32_BIT"

########################### End decoder configuration ##########################


######################### Post processor configuration #########################

# Values: 0, 1, 4, 8, 16 or 17 for AHB
# Values: 1 - 31 for OCP
# Values: 1 - 16 for AXI
export PP_BUS_BURST_LENGTH="16"

# Values: DISABLED & ENABLED
export PP_CLOCK_GATING="DISABLED"

# Values: DISABLED & ENABLED
export PP_DATA_DISCARD="DISABLED"

# Values: BIG_ENDIAN, LITTLE_ENDIAN & PP_CFG
export PP_INPUT_PICTURE_ENDIAN="PP_CFG"

# Values: BIG_ENDIAN, LITTLE_ENDIAN & PP_CFG
export PP_OUTPUT_PICTURE_ENDIAN="PP_CFG"

# Values: DISABLED, ENABLED & PP_CFG
export PP_WORD_SWAP="PP_CFG"

# Values: DISABLED, ENABLED & PP_CFG
export PP_WORD_SWAP16="PP_CFG"

# Maximum resolution
export PP_MAX_OUTPUT="1920"

# Values: DISABLED & ENABLED
export PP_MULTI_BUFFER="DISABLED"


####################### End post processor configuration #######################


########################### Test bench configuration  ##########################

# Values: DISABLED & ENABLED
# If TB_TEST_DB is ENABLED, test results are written to data a base (in addition to CSV file)
export TB_TEST_DB="DISABLED"

# Maximum number of pictures to be decoded (0 == do not restrict the decoding)
export TB_MAX_NUM_PICS="0"

# Values: DISABLED & ENABLED
# if TB_NAL_UNIT_STREAM is ENABLED, TB_PACKET_BY_PACKET must be set to ENABLED.
export TB_PACKET_BY_PACKET="DISABLED"

# Values: DISABLED & ENABLED
# Give stream to decoder in one chunk
export TB_WHOLE_STREAM="DISABLED"

# Values: ENABLED & DISABLED
# Internal tracing support
# For internal use, only
export TB_INTERNAL_TEST="DISABLED"

# Seed for stream corruption (for internal use only)
export TB_SEED_RND="0"

# Changes to corrupt a bit in a stream (i.e., bit value is swapped)
# E.g., "1 : 10", 0 is for disabling  (for internal use only)
export TB_STREAM_BIT_SWAP="0"

# Changes to lose a packet in a stream
# E.g., "1 : 100", 0 is for disabling  (for internal use only)
# If TB_STREAM_PACKET_LOSS != 0, TB_PACKET_BY_PACKET must be set to ENABLED
export TB_STREAM_PACKET_LOSS="0"
if [ "$TB_STREAM_PACKET_LOSS" != "0" ] && [ "$TB_PACKET_BY_PACKET" == "DISABLED" ]
then
    echo "Stream packet loss requires enabled packet by packet mode (see config.sh)"
    exit 1
fi

# Values: ENABLED & DISABLED
# If ENABLED, bits in stream headers are also corrupted  (for internal use only)
export TB_STREAM_HEADER_CORRUPT="DISABLED"

# Values: ENABLED & DISABLED
# If enabled, bits in end of stream headers are removed  (for internal use only)
export TB_STREAM_TRUNCATE="DISABLED"

# Values: 0 - 63
# The maximum DEC_LATENCY_COMPENSATION used in random configuration
export TB_MAX_DEC_LATENCY_COMPENSATION="63"

# Values: ENABLED & DISABLED
# Randomize test case execution order
export TB_RND_TC="DISABLED"

# Values: ENABLED & DISABLED
# Randomize configuration
export TB_RND_CFG="DISABLED"

# Values: ENABLED & DISABLED
# Randomize all possible parameters
export TB_RND_ALL="DISABLED"

# The maximum picture width for decoder (only for video) test case(s) to be executed
# Use -1 not to restrict cases to be executed
export TB_MAX_DEC_OUTPUT="-1"

# The maximum picture width for post-processor test case(s) to be executed
# Use -1 not to restrict cases to be executed
export TB_MAX_PP_OUTPUT="-1"

# IP address to be used in remote test execution
# "LOCAL_HOST" disables remote execution
# For internal use, only
export TB_REMOTE_TEST_EXECUTION_IP="LOCAL_HOST"

# Values: SYSTEM & DIR
# Source for reference data
export TB_TEST_DATA_SOURCE="DIR"

# Values: ENABLED & DISABLED
# MD5SUM are written instead of YUV
# For internal use, only.
export TB_MD5SUM="DISABLED"

# Test scripts wait time in seconds for SW/HW picture decoding
# use the maximum value that takes to decode a picture (-1 == do not use timer)
export TB_TIMEOUT="900"
#if [ "$TB_MD5SUM" == "ENABLED" ]
#then
#    # Make sure that timeout value is big enough for MD5_SUM testing
#    export TB_TIMEOUT="2400"
#fi

# Limits test execution time (-1 == do not limit test execution time)
export TB_TEST_EXECUTION_TIME="-1"

# Values: ENABLED & DISABLED
# Remove yuvs, logs, etc. if test status == OK or NOT_FAILED
export TB_REMOVE_RESULTS="ENABLED"

# Testbench and SW log file
export TB_LOG_FILE_SUFFIX=".log"

# Values: ENABLED & DISABLED
# Model writes trace files
# For internal use, only.
export TB_MODEL_TRACE="DISABLED"

######################### Test data and test case list #########################

TEST_DATA_HOME_ROOT_CHECK="/export/work/testdata/${PRODUCT}_testdata"
TEST_DATA_HOME_ROOT_RUN="/export/work/testdata/${PRODUCT}_testdata"
TEST_DATA_HOME_ROOT_REXEC="/export/work/testdata/${PRODUCT}_testdata"

if [ "$PRODUCT" == 8170 ]
then
    export TEST_CASE_LIST_CHECK="${TEST_DATA_HOME_ROOT_CHECK}/testcase_list.${PRODUCT}"
    export TEST_DATA_HOME_CHECK="$TEST_DATA_HOME_ROOT_CHECK"
    export TEST_CASE_LIST_RUN="${TEST_DATA_HOME_ROOT_RUN}/testcase_list.${PRODUCT}"
    export TEST_DATA_HOME_RUN="$TEST_DATA_HOME_ROOT_RUN"

elif [ "$PRODUCT" == 8190 ] || [ "$PRODUCT" == 9170 ] || [ "$PRODUCT" == 9190 ] || [ "$PRODUCT" == "g1" ]
then
    export TEST_CASE_LIST_CHECK="../testcase_list.${PRODUCT}"
    export TEST_DATA_HOME_CHECK="$TEST_DATA_HOME_ROOT_CHECK"
    export TEST_CASE_LIST_RUN="../testcase_list.${PRODUCT}"
    export TEST_DATA_HOME_RUN="$TEST_DATA_HOME_ROOT_RUN"
fi

####################### End test data and test case list #######################


################################### Reporting ##################################

export HW_LANGUAGE="VERILOG"
export REPORTIME=`date +%d%m%y_%k%M | sed -e 's/ //'`
export CSV_FILE="integrationreport_${HWBASETAG}_${SWTAG}_${USER}_${REPORTIME}.csv"
export COMMENT_FILE="fail.txt"
export HW_PERFORMANCE_LOG="hw_performance.log"

################################# End reporting ################################

######################### End test bench configuration #########################

########################## End common for all decoders #########################


################################# H.264 decoder ################################

# Test bench configuration
# Values: DISABLED & ENABLED
# Decoder test bench is run with -L switch, required for long streams
export TB_H264_LONG_STREAM="DISABLED"

# Test cases that match the provided tools are not checked (for internal testing)
export TB_H264_TOOLS_NOT_SUPPORTED="DISABLED"

# Values: DISABLED & ENABLED
# If ENABLED, TB_PACKET_BY_PACKET must be set to ENABLED
export TB_NAL_UNIT_STREAM="DISABLED"
if [ $TB_NAL_UNIT_STREAM == "ENABLED" ] && [ $TB_PACKET_BY_PACKET ==  "DISABLED" ]
then
    echo "Enabled NAL unit stream requires enabled packet by packet mode (see config.sh)"
    exit 1
fi

# Decoder testbench
if [ "$PRODUCT" == "8170" ]
then
    export H264_DECODER="../../../h264/hx170dec_versatile"

elif [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
then
    export H264_DECODER="../../../h264high/hx170dec_versatile"
fi

# For internal testing, only
if [ "$PRODUCT" == "8170" ]
then
    export H264_DECODER_REF="../../${SWTAG}/software/test/h264/hx170dec_pclinux"

elif [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == 9190 ] || [ "$PRODUCT" == "g1" ]
then
    export H264_DECODER_REF="../../${SWTAG}/software/test/h264high/hx170dec_pclinux"
fi

# Input streams
export H264_STREAM="stream.h264"

# Output streams
export H264_OUT_TILED_SUFFIX="_tiled.yuv"
export H264_OUT_RASTER_SCAN_SUFFIX="_raster_scan.yuv"

# Reference files
export H264_REFERENCE_TILED="out_tiled.yuv"
export H264_REFERENCE_RASTER_SCAN="out.yuv"

export H264_2ND_CHROMA_TABLE_OUTPUT="out_ch8pix.yuv"

############################### End H.264 decoder ##############################


################################ MPEG-4 decoder ################################

# Decoder testbench
export MPEG4_DECODER="../../../mpeg4/mx170dec_versatile"

# For internal testing, only
export MPEG4_DECODER_REF="../../${SWTAG}/software/test/mpeg4/mx170dec_pclinux"

# Input streams
export MPEG4_STREAM="stream.mpeg4"
export DIVX_STREAM="stream.divx"

# Output streams
export MPEG4_OUT_TILED_SUFFIX="_tiled.yuv"
export MPEG4_OUT_RASTER_SCAN_SUFFIX="_raster_scan.yuv"

# Reference files
export MPEG4_REFERENCE_TILED="out_tiled.yuv"
export MPEG4_REFERENCE_RASTER_SCAN="out_w*h*.yuv"

############################## End MPEG-4 decoder ##############################


################################# VC-1 decoder #################################

# Test bench configuration
# Values: DISABLED & ENABLED
# Decoder test bench is run with -L switch, required for long streams
export TB_VC1_LONG_STREAM="ENABLED"

# Values: DISABLED & ENABLED
# User data together with picture data is included into single decode unit (i.e., packet)
export TB_VC1_UD="DISABLED"

# Decoder testbench
export VC1_DECODER="../../../vc1/vx170dec_versatile"

# For internal testing, only
export VC1_DECODER_REF="../../${SWTAG}/software/test/vc1/vx170dec_pclinux"

# Input streams
export VC1_STREAM="stream.rcv"

# Output streams
export VC1_OUT_TILED_SUFFIX="_tiled.yuv"
export VC1_OUT_RASTER_SCAN_SUFFIX="_raster_scan.yuv"

# Reference files
export VC1_REFERENCE_TILED="out_tiled.yuv"
export VC1_REFERENCE_RASTER_SCAN="out_w*h*.yuv"

############################### End VC-1 decoder ###############################


################################# JPEG decoder #################################

# Test bench configuration
export JPEG_MCUS_SLICE="0"
export JPEG_INPUT_BUFFER_SIZE="0"

# Decoder testbench
export JPEG_DECODER="../../../jpeg/jx170dec_versatile"

# For internal testing, only
export JPEG_DECODER_REF="../../${SWTAG}/software/test/jpeg/jx170dec_pclinux"

# Input streams
export JPEG_STREAM="stream.jpg"

# Output streams & Reference files
export JPEG_OUT="out.yuv"
export JPEG_OUT_THUMB="out_tn.yuv"

############################### End JPEG decoder ###############################

################################ MPEG-2 decoder ################################

# Decoder testbench
export MPEG2_DECODER="../../../mpeg2/m2x170dec_versatile"

# For internal testing, only
export MPEG2_DECODER_REF="../../${SWTAG}/software/test/mpeg2/m2x170dec_pclinux"

# Input streams
export MPEG2_STREAM="stream.mpeg2"
export MPEG1_STREAM="stream.mpeg1"

# Output streams
export MPEG2_OUT_TILED_SUFFIX="_tiled.yuv"
export MPEG2_OUT_RASTER_SCAN_SUFFIX="_raster_scan.yuv"

# Reference files
export MPEG2_REFERENCE_TILED="out_tiled.yuv"
export MPEG2_REFERENCE_RASTER_SCAN="out_w*h*.yuv"

############################## End MPEG-2 decoder ##############################

################################ Real decoder ################################

# Decoder testbench
export REAL_DECODER="../../../rv/rvx170dec_versatile"

# For internal testing, only
export REAL_DECODER_REF="../../${SWTAG}/software/test/rv/rvx170dec_pclinux"

# Input stream
export REAL_STREAM="stream.rm"

# Output stream
export REAL_OUT_TILED_SUFFIX="_tiled.yuv"
export REAL_OUT_RASTER_SCAN_SUFFIX="_raster_scan.yuv"

# Reference file
export REAL_REFERENCE_RASTER_SCAN="out_w*h*.yuv"

############################## End Real decoder ##############################

################################ VP6 decoder ################################

# Decoder testbench
export VP6_DECODER="../../../vp6/vp6dec_versatile"

# For internal testing, only
export VP6_DECODER_REF="../../${SWTAG}/software/test/vp6/vp6dec_pclinux"

# Input stream
export VP6_STREAM="stream.vp6"

# Output stream
export VP6_OUT_TILED_SUFFIX="_tiled.yuv"
export VP6_OUT_RASTER_SCAN_SUFFIX="_raster_scan.yuv"

# Reference file
export VP6_REFERENCE_RASTER_SCAN="out.yuv"

############################## End VP6 decoder ##############################

################################ VP8 decoder ################################

# Test bench configuration
export TB_VP8_STRIDE="DISABLED"

export TB_VP8_RND_STRIDE="ENABLED"

export VP8_LUMA_STRIDE="0"

export VP8_CHROMA_STRIDE="0"

export VP8_INTERLEAVED_PIC_BUFFERS="DISABLED"

export VP8_EXT_MEM_ALLOC_ORDER="DISABLED"

# Decoder testbench
export VP8_DECODER="../../../vp8/vp8x170dec_versatile"

# For internal testing, only
export VP8_DECODER_REF="../../${SWTAG}/software/test/vp8/vp8x170dec_pclinux"

# Input streams
export VP8_STREAM="stream.vp8"
export VP7_STREAM="stream.vp7"
export WEBP_STREAM="stream.webp"

# Output stream
export VP8_OUT_TILED_SUFFIX="_tiled.yuv"
export VP8_OUT_RASTER_SCAN_SUFFIX="_raster_scan.yuv"

# Reference file
export VP8_REFERENCE_RASTER_SCAN="out.yuv"

############################## End VP8 decoder ##############################

################################ AVS decoder ################################

# Decoder testbench
export AVS_DECODER="../../../avs/ax170dec_versatile"

# For internal testing, only
export AVS_DECODER_REF="../../${SWTAG}/software/test/avs/ax170dec_pclinux"

# Input streams
export AVS_STREAM="stream.avs"

# Output stream
export AVS_OUT_TILED_SUFFIX="_tiled.yuv"
export AVS_OUT_RASTER_SCAN_SUFFIX="_raster_scan.yuv"

# Reference file
export AVS_REFERENCE_RASTER_SCAN="out.yuv"

############################## End VP8 decoder ##############################

################################ Post processor ################################

# Post-processor configuration
# Values: DISABLED & ENABLED
export PP_ALPHA_BLENDING="ENABLED"
export PP_SCALING="ENABLED"
export PP_DEINTERLACING="ENABLED"
export PP_DITHERING="ENABLED"

# Post processor test benches
export PP_EXTERNAL="../../../pp/external_mode/px170dec_versatile"
if [ "$PRODUCT" == "8170" ]
then
    export PP_H264_PIPELINE="../../../pp/h264_pipeline_mode/pp_h264_pipe"
    
    elif [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
then
    export PP_H264_PIPELINE="../../../pp/h264high_pipeline_mode/pp_h264_pipe"
fi
export PP_MPEG4_PIPELINE="../../../pp/mpeg4_pipeline_mode/pp_mpeg4_pipe"
export PP_MPEG2_PIPELINE="../../../pp/mpeg2_pipeline_mode/pp_mpeg2_pipe"
export PP_JPEG_PIPELINE="../../../pp/jpeg_pipeline_mode/pp_jpeg_pipe"
export PP_VC1_PIPELINE="../../../pp/vc1_pipeline_mode/pp_vc1_pipe"
export PP_REAL_PIPELINE="../../../pp/rv_pipeline_mode/pp_rv_pipe"
export PP_VP6_PIPELINE="../../../pp/vp6_pipeline_mode/pp_vp6_pipe"
export PP_VP8_PIPELINE="../../../pp/vp8_pipeline_mode/pp_vp8_pipe"
export PP_AVS_PIPELINE="../../../pp/avs_pipeline_mode/pp_avs_pipe"

# For internal testing, only
export PP_EXTERNAL_REF="../../${SWTAG}/software/test/pp/external_mode/px170dec_pclinux"
if [ "$PRODUCT" == "8170" ]
then
    export PP_H264_PIPELINE_REF="../../${SWTAG}/software/test/pp/h264_pipeline_mode/pp_h264_pipe"

elif [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
then
    export PP_H264_PIPELINE_REF="../../${SWTAG}/software/test/pp/h264high_pipeline_mode/pp_h264_pipe"
fi
export PP_MPEG4_PIPELINE_REF="../../${SWTAG}/software/test/pp/mpeg4_pipeline_mode/pp_mpeg4_pipe"
export PP_MPEG2_PIPELINE_REF="../../${SWTAG}/software/test/pp/mpeg2_pipeline_mode/pp_mpeg2_pipe"
export PP_JPEG_PIPELINE_REF="../../${SWTAG}/software/test/pp/jpeg_pipeline_mode/pp_jpeg_pipe"
export PP_VC1_PIPELINE_REF="../../${SWTAG}/software/test/pp/vc1_pipeline_mode/pp_vc1_pipe"
export PP_REAL_PIPELINE_REF="../../${SWTAG}/software/test/pp/rv_pipeline_mode/pp_rv_pipe"
export PP_VP6_PIPELINE_REF="../../${SWTAG}/software/test/pp/vp6_pipeline_mode/pp_vp6_pipe"
export PP_VP8_PIPELINE_REF="../../${SWTAG}/software/test/pp/vp8_pipeline_mode/pp_vp8_pipe"
export PP_AVS_PIPELINE_REF="../../${SWTAG}/software/test/pp/avs_pipeline_mode/pp_avs_pipe"

# Post processor configuration
export PP_CFG="pp.cfg"

# Output streams & Reference files
export PP_OUT_RGB="pp_out.rgb"
export PP_OUT_YUV="pp_out.yuv"

############################## End post processor ##############################
