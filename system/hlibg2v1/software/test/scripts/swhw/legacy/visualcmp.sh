#!/bin/bash

# Imports
. ./config.sh
. ./f_utils.sh "check"
. $TEST_CASE_LIST_CHECK

viewVideo()
{
    case_number=$1
    decoder_out=$2
    ref_out=$3
    scaling=$4
    
    # Check if outputs exist
    if [ -z $decoder_out ]
    then
        echo "Missing decoder output for test case ${case_number}!"
        return 1
    fi
    if [ -z $ref_out ]
    then
        echo "Missing reference output for test case ${case_number}!"
        return 1
    fi
    
    # Parse picture dimension
    if [ ! -e "$TEST_DATA_HOME_CHECK/case_$case_number/picture_ctrl_dec.trc" ]
    then
        echo "Missing $TEST_DATA_HOME_CHECK/case_$case_number/picture_ctrl_dec.trc!"
        return 1
    fi
    width=`grep -i -m 1 "Picture width" $TEST_DATA_HOME_CHECK/case_$case_number/picture_ctrl_dec.trc | awk '{print $1}'`
    let 'width*=16'
    height=`grep -i -m 1 "Picture height" $TEST_DATA_HOME_CHECK/case_$case_number/picture_ctrl_dec.trc | awk '{print $1}'`
    let 'height*=16'
    
    # Visual compare the video
    echo "/export/work/sw_tools/viewyuv -m -c -s${scaling} -w${width} -h${height} ${decoder_out} ${ref_out}"
    /export/work/sw_tools/viewyuv -m -c "-s${scaling}" "-w${width}" "-h${height}" $decoder_out $ref_out
    return 0
}

viewH264Case()
{
    case_number=$1
    scaling=$2
   
    # Set the test data dir
    if [ $TB_TEST_DATA_SOURCE == "DIR" ]
    then
        case_dir=$TEST_DATA_HOME_CHECK/case_$case_number
    else
        case_dir=case_$case_number
    fi
    
    # Determine the correct files to be compared
    if [ -e case_${case_number}${H264_OUT_TILED_SUFFIX} ]
    then
       decoder_out=case_${case_number}${H264_OUT_TILED_SUFFIX}
       ref_out=$case_dir/$H264_REFERENCE_TILED
       echo "WARNING! Tiled mode not properly supported"
    elif [ -e case_${case_number}${H264_OUT_RASTER_SCAN_SUFFIX} ]
    then
       decoder_out=case_${case_number}${H264_OUT_RASTER_SCAN_SUFFIX}
       ref_out=$case_dir/$H264_REFERENCE_RASTER_SCAN
    fi
    
    viewVideo "$case_number" "$decoder_out" "$ref_out" "$scaling"
    return $?
}

viewMpeg4Case()
{
    case_number=$1
    scaling="$2"
    
    # Set the test data dir
    if [ $TB_TEST_DATA_SOURCE == "DIR" ]
    then
        case_dir=$TEST_DATA_HOME_CHECK/case_$case_number
    else
        case_dir=case_$case_number
    fi
    
    # Determine the correct files to be compared
    if [ -e case_${case_number}${MPEG4_OUT_TILED_SUFFIX} ]
    then
       decoder_out=case_${case_number}${MPEG4_OUT_TILED_SUFFIX}
       ref_out=$case_dir/$MPEG4_REFERENCE_TILED
       echo "WARNING! Tiled mode not properly supported"
    elif [ -e case_${case_number}${MPEG4_OUT_RASTER_SCAN_SUFFIX} ]
    then
        decoder_out=case_${case_number}${MPEG4_OUT_RASTER_SCAN_SUFFIX}
        ref_out=$case_dir/$MPEG4_REFERENCE_RASTER_SCAN
    fi
    
    #interlaced_sequence=`cat ${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_dec.trc | grep "interlace enable" | awk '{print $1}' | grep -m 1 1`
    interlaced_sequence=`cat case_${case_number}${TB_LOG_FILE_SUFFIX} | grep -m 1 "INTERLACED SEQUENCE"`
    if [ ! -z "$interlaced_sequence" ] && [ -e "case_${case_number}${MPEG4_OUT_TILED_SUFFIX}" ]
    then
        # if interlaced sequence, the decoder writes out in raster scan format
        ref_out="${case_dir}/${MPEG4_REFERENCE_RASTER_SCAN}"
    fi
    
    viewVideo "$case_number" "$decoder_out" "$ref_out" "$scaling"
    return $?
}

viewVc1Case()
{
    case_number=$1
    scaling=$2
    
    # Set the test data dir
    if [ $TB_TEST_DATA_SOURCE == "DIR" ]
    then
        case_dir=$TEST_DATA_HOME_CHECK/case_$case_number
    else
        case_dir=case_$case_number
    fi
    
    # Determine the correct files to be compared
    if [ -e case_${case_number}${VC1_OUT_TILED_SUFFIX} ]
    then
        decoder_out=case_${case_number}${VC1_OUT_TILED_SUFFIX}
        ref_out=$case_dir/$VC1_REFERENCE_TILED
        echo "WARNING! Tiled mode not properly supported"
    elif [ -e case_${case_number}${VC1_OUT_RASTER_SCAN_SUFFIX} ]
    then
        decoder_out=case_${case_number}${VC1_OUT_RASTER_SCAN_SUFFIX}
        ref_out=$case_dir/$VC1_REFERENCE_RASTER_SCAN
    fi
    
    #interlaced_sequence=`cat ${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_dec.trc | grep "interlace enable" | awk '{print $1}' | grep -m 1 1`
    interlaced_sequence=`cat case_${case_number}${TB_LOG_FILE_SUFFIX} | grep -m 1 "INTERLACED SEQUENCE"`
    if [ ! -z "$interlaced_sequence" ] && [ -e "case_${case_number}${VC1_OUT_TILED_SUFFIX}" ]
    then
        # if interlaced sequence, the decoder writes out in raster scan format
        ref_out="${case_dir}/${VC1_REFERENCE_RASTER_SCAN}"
    fi
    
    viewVideo "$case_number" "$decoder_out" "$ref_out" "$scaling"
    return $?
}

viewMpeg2Case()
{
    case_number=$1
    scaling="$2"
    
    # Set the test data dir
    if [ $TB_TEST_DATA_SOURCE == "DIR" ]
    then
        case_dir=$TEST_DATA_HOME_CHECK/case_$case_number
    else
        case_dir=case_$case_number
    fi
    
    # Determine the correct files to be compared
    if [ -e case_${case_number}${MPEG2_OUT_TILED_SUFFIX} ]
    then
       decoder_out=case_${case_number}${MPEG2_OUT_TILED_SUFFIX}
       ref_out=$case_dir/$MPEG4_REFERENCE_TILED
       echo "WARNING! Tiled mode not properly supported"
    elif [ -e case_${case_number}${MPEG2_OUT_RASTER_SCAN_SUFFIX} ]
    then
        decoder_out=case_${case_number}${MPEG2_OUT_RASTER_SCAN_SUFFIX}
        ref_out=$case_dir/$MPEG2_REFERENCE_RASTER_SCAN
    fi
    
    #interlaced_sequence=`cat ${TEST_DATA_HOME_CHECK}/case_${case_number}/picture_ctrl_dec.trc | grep "interlace enable" | awk '{print $1}' | grep -m 1 1`
    interlaced_sequence=`cat case_${case_number}${TB_LOG_FILE_SUFFIX} | grep -m 1 "INTERLACED SEQUENCE"`
    if [ ! -z "$interlaced_sequence" ] && [ -e "case_${case_number}${MPEG2_OUT_TILED_SUFFIX}" ]
    then
        # if interlaced sequence, the decoder writes out in raster scan format
        ref_out="${case_dir}/${MPEG2_REFERENCE_RASTER_SCAN}"
    fi
    
    viewVideo "$case_number" "$decoder_out" "$ref_out" "$scaling"
    return $?
}

viewRealCase()
{
    case_number=$1
    scaling="$2"
    
    # Set the test data dir
    if [ $TB_TEST_DATA_SOURCE == "DIR" ]
    then
        case_dir=$TEST_DATA_HOME_CHECK/case_$case_number
    else
        case_dir=case_$case_number
    fi
    
    decoder_out="case_${case_number}${REAL_OUT_RASTER_SCAN_SUFFIX}"
    ref_out="$case_dir/$REAL_REFERENCE_RASTER_SCAN"
    
    viewVideo "$case_number" "$decoder_out" "$ref_out" "$scaling"
    return $?
}

viewVP6Case()
{
    case_number=$1
    scaling="$2"
    
    # Set the test data dir
    if [ $TB_TEST_DATA_SOURCE == "DIR" ]
    then
        case_dir=$TEST_DATA_HOME_CHECK/case_$case_number
    else
        case_dir=case_$case_number
    fi
    
    decoder_out="case_${case_number}${VP6_OUT_RASTER_SCAN_SUFFIX}"
    ref_out="$case_dir/$VP6_REFERENCE_RASTER_SCAN"
    
    viewVideo "$case_number" "$decoder_out" "$ref_out" "$scaling"
    return $?
}

viewVP8Case()
{
    case_number=$1
    scaling="$2"
    
    # Set the test data dir
    if [ $TB_TEST_DATA_SOURCE == "DIR" ]
    then
        case_dir=$TEST_DATA_HOME_CHECK/case_$case_number
    else
        case_dir=case_$case_number
    fi
    
    decoder_out="case_${case_number}${VP8_OUT_RASTER_SCAN_SUFFIX}"
    ref_out="$case_dir/$VP8_REFERENCE_RASTER_SCAN"
    
    viewVideo "$case_number" "$decoder_out" "$ref_out" "$scaling"
    return $?
}

viewAVSCase()
{
    case_number=$1
    scaling="$2"
    
    # Set the test data dir
    if [ $TB_TEST_DATA_SOURCE == "DIR" ]
    then
        case_dir=$TEST_DATA_HOME_CHECK/case_$case_number
    else
        case_dir=case_$case_number
    fi
    
    decoder_out="case_${case_number}${AVS_OUT_RASTER_SCAN_SUFFIX}"
    ref_out="$case_dir/$AVS_REFERENCE_RASTER_SCAN"
    
    viewVideo "$case_number" "$decoder_out" "$ref_out" "$scaling"
    return $?
}

parseJpegPicCtrlDec()
{
    case_number=$1
    pic=$2
    
    # Set the test data dir
    if [ $TB_TEST_DATA_SOURCE == "DIR" ]
    then
        case_dir=$TEST_DATA_HOME_CHECK/case_$case_number
    else
        case_dir=case_$case_number
    fi
    
    width=`grep -A 4 -m 1 "picture=${pic}" $case_dir/picture_ctrl_dec.trc | grep -i "Picture width in" | awk '{print $1}'`
    let 'width*=16'
    height=`grep -A 4 -m 1 "picture=${pic}" $case_dir/picture_ctrl_dec.trc | grep -i "Picture height in" | awk '{print $1}'`
    let 'height*=16'
    sampling_format=`grep -A 4 -m 1 "picture=${pic}" $case_dir/picture_ctrl_dec.trc | grep -i "sampling format" | awk '{print $1}'`
}

viewJpegCase()
{
    case_number=$1
    scaling=$2
    
    # Set the test data dir
    if [ $TB_TEST_DATA_SOURCE == "DIR" ]
    then
        case_dir=$TEST_DATA_HOME_CHECK/case_$case_number
    else
        case_dir=case_$case_number
    fi
    
    # Parse picture dimension
    if [ ! -e "$case_dir/picture_ctrl_dec.trc" ]
    then
        echo "Missing $case_dir/picture_ctrl_dec.trc!"
        return 1
    fi
    
    # check if thumbnail case
    thumbnail=`grep "picture=1" $case_dir/picture_ctrl_dec.trc`
    if [ ! -z $thumbnail ]
    then
        thumbnail=1
    else
        thumbnail=0
    fi
    
    # loop through pictures (i.e.m, normal and thumbnail)
    let 'pic=-1'
    while [[ $pic -lt 1 && $thumbnail == 1 ]] || [[ $pic -lt 0 && $thumbnail == 0 ]]
    do
        let 'pic+=1'
        
        if [ $thumbnail == 1 ] && [ $pic == 0 ]
        then
            # Determine the correct files to be compared
            decoder_out=tn_case_${case_number}.yuv
            ref_out=$case_dir/$JPEG_OUT_THUMB
        
        else
            # Determine the correct files to be compared
            decoder_out=case_${case_number}.yuv
            ref_out=$case_dir/$JPEG_OUT
        fi
        
        # Check if output exist
        if [ ! -e $decoder_out ]
        then
            echo "Missing decoder output for test case ${case_number}!"
            if [ $thumbnail == 1 ]
            then
                continue
            else
                return 1
            fi
        fi
        if [ ! -e $ref_out ]
        then
            echo "Missing reference output for test case ${case_number}!"
            if [ $thumbnail == 1 ]
            then
                continue
            else
                return 1
            fi
        fi
        
        # Determine picture dimensions and sampling format
        parseJpegPicCtrlDec "$case_number" "$pic"
        
        only_luma="-m"
        if [ $sampling_format == 0 ]
        then
            only_luma="-b"
            
        elif  [ $sampling_format == 1 ]
        then
            echo "Not supported sampling format ${sampling_format}!"
            return 1
            
        elif  [ $sampling_format == 2 ]
        then
            # Dummy
            echo -n
            
        elif  [ $sampling_format == 3 ]
        then
            echo "WARNING! 4:2:2 sampling format not properly supported for viewing"
            echo "    Only luma comparison enabled!"
            
            only_luma="-b"
        elif  [ $sampling_format == 4 ]
        then
            echo "Not supported JPEG sampling format ${sampling_format}!"
            return 1
            
        elif  [ $sampling_format == 5 ]
        then
            echo "WARNING! 4:4:0 sampling format not properly supported for viewing"
            echo "    Only luma comparison enabled!"
            only_luma="-b"
            
        else
            echo "Unknown sampling format ${sampling_format}!"
            return 1
        fi
    
        # Visual compare the JPEG image
        /export/work/sw_tools/viewyuv "$only_luma" -c "-s${scaling}" "-w${width}" "-h${height}" $decoder_out $ref_out
    done
    return 0
}

getRgbMask()
{
    mask=`awk -v pad=$1 -v mask=$2 -v rgb16=$3 'BEGIN {
        if (mask < 0) { 
            mask=-mask
            mask-=1
            mask=xor(0xFFFFFFFF, mask)
        }
        if(rgb16)
            mask=and(0x0000FFFF, mask)
        printf "%X", mask
        }'`
    export mask
}

removeRgbAlphaMask()
{
    mask=`awk -v mask_name=$1 -v r_mask=$2 -v g_mask=$3 -v b_mask=$4 'BEGIN {
        alpha_mask=and(and(r_mask, g_mask), b_mask)
        if (mask_name == "R") { 
            mask=xor(alpha_mask, r_mask)
        }
        else if (mask_name == "G") {
            mask=xor(alpha_mask, g_mask)
        }
        else if (mask_name == "B") {
            mask=xor(alpha_mask, b_mask)
        }
        printf "%X", mask
        }'`
    export mask
}

viewPpCase()
{
    case_number=$1
    scaling=$2
    
    # Set the test data dir
    if [ $TB_TEST_DATA_SOURCE == "DIR" ]
    then
        case_dir=$TEST_DATA_HOME_CHECK/case_$case_number
    else
        case_dir=case_$case_number
    fi
    
    # Parse PP output format
    if [ ! -e "$case_dir/picture_ctrl_pp.trc" ]
    then
        echo "Missing $case_dir/picture_ctrl_pp.trc!"
        return 1
    fi
    
    format=`grep -i -m 1 "PP output format" $case_dir/picture_ctrl_pp.trc | awk '{print $1}'`
    
    if [ $format == 0 ]
    then
        rgb16=`grep -i -m 1 "RGB pixel amount" $case_dir/picture_ctrl_pp.trc | awk '{print $1}'`
        
        r_pad=`grep -i -m 1 "R zero padd" $case_dir/picture_ctrl_pp.trc | awk '{print $1}'`
        r_mask=`grep -i -m 1 "R mask" $case_dir/picture_ctrl_pp.trc | awk '{print $1}'`
        
        getRgbMask "$r_pad" "$r_mask" "$rgb16"
        r_mask=$mask
        
        g_pad=`grep -i -m 1 "G zero padd" $case_dir/picture_ctrl_pp.trc | awk '{print $1}'`
        g_mask=`grep -i -m 1 "G mask" $case_dir/picture_ctrl_pp.trc | awk '{print $1}'`
        getRgbMask "$g_pad" "$g_mask" "$rgb16"
        g_mask=$mask
        
        b_pad=`grep -i -m 1 "B zero padd" $case_dir/picture_ctrl_pp.trc | awk '{print $1}'`
        b_mask=`grep -i -m 1 "B mask" $case_dir/picture_ctrl_pp.trc | awk '{print $1}'`
        getRgbMask "$b_pad" "$b_mask" "$rgb16"
        b_mask=$mask
        
        #echo "$r_mask"
        #echo "$g_mask"
        #echo "$b_mask"
        
        r_mask_dec=`echo "obase=10;ibase=16;$r_mask" | bc`
        g_mask_dec=`echo "obase=10;ibase=16;$g_mask" | bc`
        b_mask_dec=`echo "obase=10;ibase=16;$b_mask" | bc`
        
        #echo "$r_mask_dec"
        #echo "$g_mask_dec"
        #echo "$b_mask_dec"
        
        removeRgbAlphaMask "R" "$r_mask_dec" "$g_mask_dec" "$b_mask_dec"
        r_mask=$mask
        removeRgbAlphaMask "G" "$r_mask_dec" "$g_mask_dec" "$b_mask_dec"
        g_mask=$mask
        removeRgbAlphaMask "B" "$r_mask_dec" "$g_mask_dec" "$b_mask_dec"
        b_mask=$mask
        
        #echo "$r_mask"
        #echo "$g_mask"
        #echo "$b_mask"
        
    elif [ $format == 3 ]
    then
        echo "YCbCr 4:2:2 Interleaved not supported format for viewing!"
        return 1
        
    elif [ $format == 5 ]
    then
        # Dummy
        echo -n
        
    elif [ $format == 1 ] || [ $format == 2 ] || [ $format == 4 ] || [ $format == 6 ] || [ $format == 7 ]
    then
        echo "Not supported format ${format}!"
        return 1
    
    else
        echo "Unknown format ${format}!"
        return 1
    fi
    
    # Determine the correct files to be compared
    if [ $format != 0 ]
    then
        pp_out=case_${case_number}.yuv
        ref_out=$case_dir/$PP_OUT_YUV
    
    else
        pp_out=case_${case_number}.rgb
        ref_out=$case_dir/$PP_OUT_RGB
    fi
    
    # Check if output exist
    if [ ! -e $pp_out ]
    then
        echo "Missing post-processor output for test case ${case_number}!"
    fi
    if [ ! -e $ref_out ]
    then
        echo "Missing reference output for test case ${case_number}!"
    fi
    
    # Parse picture dimensions
    width=`grep -i -m 1 "display width" $case_dir/picture_ctrl_pp.trc | awk '{print $1}'`
    height=`grep -i -m 1 "display height" $case_dir/picture_ctrl_pp.trc | awk '{print $1}'`
    
    if [ $format == 5 ]
    then
        /export/work/sw_tools/viewyuv -m -c "-s${scaling}" "-w${width}" "-h${height}" $pp_out $ref_out
        
    elif [ $format == 0 ]
    then
        if [ $rgb16 == 1 ]
        then
            bits="-b16"
        else
            bits="-b32"
        fi
        /export/work/sw_tools/viewrgb -c "$bits" -f0 "$r_mask $g_mask $b_mask" "-w${width}" "-h${height}" $pp_out $ref_out
    fi
    
    return 0
}

#=====---- Main

# check the command line parameters
# test case number
if [ -z $1 ]
then
    echo "Missing test case number!"
    echo "    ./visualcmp.sh <test case number> <image scaling; optional>"
    exit 1
else
    case_number=$1
fi
# image scaling
if [ -z "$2" ]
then
    scaling="1"
else
    scaling="${2}"
fi

# decide component for test cases
getComponent "$case_number"
if [ $? == 1 ]
then
    echo "Cannot find decoder/post-processor for ${case_number} test case"
    exit 1
fi

if [ "$post_processor" == "pp" ]
then
    viewPpCase "$case_number" "$scaling"
    exit $?
fi

if [ "$decoder" == "h264" ]
then
    viewH264Case "$case_number" "$scaling"
    exit $?
fi

if [ "$decoder" == "mpeg4" ]
then
    viewMpeg4Case "$case_number" "$scaling"
    exit $?
fi

if [ "$decoder" == "vc1" ]
then
    viewVc1Case "$case_number" "$scaling"
    exit $?
fi

if [ "$decoder" == "mpeg2" ]
then
    viewMpeg2Case "$case_number" "$scaling"
    exit $?
fi

if [ "$decoder" == "real" ]
then
    viewRealCase "$case_number" "$scaling"
    exit $?
fi

if [ "$decoder" == "vp6" ]
then
    viewVP6Case "$case_number" "$scaling"
    exit $?
fi

if [ "$decoder" == "vp8" ]
then
    viewVP8Case "$case_number" "$scaling"
    exit $?
fi

if [ "$decoder" == "avs" ]
then
    viewAVSCase "$case_number" "$scaling"
    exit $?
fi

if [ "$decoder" == "jpeg" ]
then
    viewJpegCase "$case_number" "$scaling"
    exit $?
fi

exit 0
