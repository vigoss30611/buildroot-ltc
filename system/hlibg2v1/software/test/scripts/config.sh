# Configuration file for the SW test script

#=====------------------------------------------------------=====#
#=====---     System model configuration               -----=====#
#=====------------------------------------------------------=====#

# use latest system model version or set tag
# 'latest'  = get latest tag
# 'tagname' = get tag 'tagname'
export H264_model_version=latest
export MPEG4_model_version=latest
export JPEG_model_version=latest

# n = create new testdata, y = use old data
export USE_OLD_DATA=n

#=====------------------------------------------------------=====#
#=====---     SW compile time configuration            -----=====#
#=====------------------------------------------------------=====#

#### SW compile time configuration ####

# Make SW traces, 1=on, 0=off
export SW_tracing=1

# Force RLC/VLC mode
# 'rlc'     = 6150 interface forced (SW decodes stream)
# 'vlc'     = x170 interface forced (HW decodes stream)
# 'default' = using default mode defined by test data
export forced_interface=vlc

# Run cases packet by packet, y=on, n=off
export packetmode=n

# Disable packetizing, even if stream does not fit into buffer

export force_whole_stream=n

# Use picture freeze error concealment
export mb_error_concealment=n

# Make pc-linux vs. versatile version
# 'pc'       = pc-linux version
# 'versatile = versatile version
export SW_version=pc

# MPEG-4 whole stream mode
# "-W" whole stream mode
# " "  VOP by VOP mode
export mpeg4_whole_stream=" "
#export mpeg4_whole_stream="-W"

#=====------------------------------------------------------=====#
#=====---            Test configuration                -----=====#
#=====------------------------------------------------------=====#

# coverage analysis

export coverage=n

# Maximum number of frames to test per test case
export num_frames=10000

# Enable Make and System model run time information printing
export printing=1

# Place where test report is stored
export CSV_PATH=.

# rebuild SW
export BUILD_SW=y

# Test person
export USER=user

# Tag
export tagfield=sw8170x_y_z
