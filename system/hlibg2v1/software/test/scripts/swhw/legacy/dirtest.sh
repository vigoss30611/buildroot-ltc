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
#--   Abstract     : This script decodes streams from the provided directory  --
#--                  recursively using remote test execution. For reference   --
#--                  yuv, system model is also executed, and comparison done  --
#--                  between reference yuv and decoder yuv.                   --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

# Directory where streams can be found
TB_STREAM_DIR=/export/work/makar/venice/extdata/test/streams

# Directory where system models can be found
TB_BIN_DIR=/export/work/makar/venice/extdata/test/bins

# Directory where to write yuvs
TB_YUV_DIR=/export/work/makar/venice/extdata/test/yuvs

# To save disk space, passed YUVs can be removed.
TB_REMOVE_PASSED_YUV="ENABLED"

# IP address of the remote test board
TB_REMOTE_TEST_EXECUTION_IP="192.168.30.42"

# List of the streams to be decoded. If empty, all the streams will be decoded
# from the TB_STREAM_DIR recursively
TB_DECODED_STREAMS=""

#-------------------------------------------------------------------------------
#-                                                                            --
#- Decodes the stream with both system model and product. For product         --
#- decoding, remote execution (rexec) is used Compares the  resulting output  --
#- data and prints out the result.                                            --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : The stream to be decoded                                               --
#- 2 : The directory where the resulting outputs are placed                   --
#-                                                                            --
#-------------------------------------------------------------------------------
decodeStream()
{
    stream="$1"
    target_dir="$2"
    
    # Current directory contains the stream
    stream_dir="$PWD"
    
    echo -n "${stream_dir}/${stream} "
    # Check which format is used
    file_extension=`echo "$stream" | awk 'BEGIN{FS="."} {print $2}'`
    
    # output file cannot be defined for JPEG
    jpeg_dec_yuv=
    jpeg_dec_tn_yuv=

    if [ "$file_extension" == "rcv" ]
    then
        sys_yuv="semplanar.yuv"
        command_sys="${TB_BIN_DIR}/vx170dec_pclinux"
        command_dec="${TB_BIN_DIR}/vx170dec_versatile"
        
    elif [ "$file_extension" == "mpeg4" ]
    then
        sys_yuv="semplanar.yuv"
        command_sys="${TB_BIN_DIR}/mx170dec_pclinux"
        command_dec="${TB_BIN_DIR}/mx170dec_versatile"
        
    elif [ "$file_extension" == "mpeg2" ]
    then
        sys_yuv="semplanar.yuv"
        command_sys="${TB_BIN_DIR}/m2x170dec_pclinux" 
        command_dec="${TB_BIN_DIR}/m2x170dec_versatile"
        
    elif [ "$file_extension" == "jpeg" ] || [ "$file_extension" == "jpg" ]
    then
        sys_yuv="out.yuv"
        sys_tn_yuv="out_tn.yuv"
        command_sys="${TB_BIN_DIR}/jx170dec_pclinux"
        command_dec="${TB_BIN_DIR}/jx170dec_versatile"
        
        jpeg_dec_yuv="out.yuv"
        jpeg_dec_tn_yuv="out_tn.yuv"
        
    elif [ "$file_extension" == "h264" ] || [ "$file_extension" == "264" ]
    then
        sys_yuv="out.yuvsem"
        command_sys="${TB_BIN_DIR}/hx170dec_pclinux"
        command_dec="${TB_BIN_DIR}/hx170dec_versatile"
    
    # Unknown extension
    else
        echo "NOT_EXECUTED: unknown file ${file_extension}"
        return
    fi
    
    #====---- Check if stream is to be decoded
    decode=0
    
    for strm in $TB_DECODED_STREAMS
    do
        if [ "$strm" == "${stream_dir}/${stream}" ]
        then
            decode=1
            break
        fi
    done
    
    if [ "$decode" == 0 ]
    then
        echo "NOT_EXECUTED: stream excluded"
        return
    fi
    
    #====---- Decode with the system model
    if [ ! -e "$command_sys" ] && [ ! -z "$TB_DECODED_STREAMS" ]
    then
        echo "NOT_EXECUTED: ${command_sys} missing"
        return
    fi
    
    # Decode in the target directory
    cd "$target_dir"
    
    echo "${stream_dir}/${stream}" >> sys.log
    echo "${stream_dir}/${stream}" >> dec.log
    
    # trace.cfg is needed for raster scan yuv
    echo trc >> trace.cfg
    echo picture_ctrl_dec >> trace.cfg
    echo fpga >> trace.cfg
    
    # Run the system model
    "$command_sys" "${stream_dir}/$stream" >> sys.log 2>&1
    
    # Get the stream name without extension
    stream_name=`echo "$stream" | awk 'BEGIN{FS="."} {print $1}'`
    
    # Clean-up the unneccessary files and rename the reference yuv file
    mv -f "$sys_yuv" tmp >> /dev/null 2>&1
    mv -f "$sys_tn_yuv" tmp_tn >> /dev/null 2>&1
    rm -f decoder_out_tiled.yuv
    rm -f out*.yuv
    rm -f ref.yuv
    rm -f *.trc
    rm -f gmon.out
    rm -f trace.cfg
    rm -f decoder.cfg
    reference_yuv="sys_${stream_name}.yuv"
    reference_tn_yuv="sys_${stream_name}_tn.yuv"
    mv -f tmp "$reference_yuv" >> /dev/null 2>&1
    mv -f tmp_tn "$reference_tn_yuv" >> /dev/null 2>&1
    
    cd - >> /dev/null
    
    ##====---- Decode with the product
    if [ ! -e "$command_dec" ]
    then
        echo "NOT_EXECUTED: ${command_dec} missing"
        return
    fi
    
    # Decode in the target directory
    cd "$target_dir"
    
    # Not a JPEG stream
    if [ -z "$jpeg_dec_yuv" ] && [ -z "$jpeg_dec_yn_yuv" ]
    then
        # Use remote execution for test execution
        rexec -l root -p xxx ${TB_REMOTE_TEST_EXECUTION_IP} "cd ${target_dir} && "${command_dec} -Odec_${stream_name}.yuv ${stream_dir}/${stream}" && chmod 666 dec_${stream_name}.yuv" >> dec.log 2>&1
    else
        # Use remote execution for test execution
        echo "DecParams {" >> tb.cfg
        echo "    JpegInputBufferSize = 2096896;"  >> tb.cfg
        echo "}"  >> tb.cfg
        rexec -l root -p xxx ${TB_REMOTE_TEST_EXECUTION_IP} "cd ${target_dir} && "${command_dec} ${stream_dir}/${stream}" && chmod 666 ${jpeg_dec_yuv} && chmod 666 ${jpeg_dec_tn_yuv}" >> dec.log 2>&1
        mv -f "$jpeg_dec_yuv" "dec_${stream_name}.yuv" >> /dev/null 2>&1
        mv -f "$jpeg_dec_tn_yuv" "dec_${stream_name}_tn.yuv" >> /dev/null 2>&1
        rm -f out_*.yuv
        rm -f outChroma.yuv
        rm -f tb.cfg
    fi
    
    cd - >> /dev/null
    
    ##====---- Compare the actual yuv with the reference one
    
    # Compare in the target directory
    cd "$target_dir"
    
    if (cmp -s "$reference_yuv" "dec_${stream_name}.yuv")
    then
        
        # Compare also thumbnail if exists
        if [ -e "$reference_tn_yuv" ]
        then
            if (cmp -s "$reference_tn_yuv" "dec_${stream_name}_tn.yuv")
            then
                echo "OK" 
                if [ "$TB_REMOVE_PASSED_YUV" == "ENABLED" ]
                then
                    rm -f "$reference_yuv"
                    rm -f "dec_${stream_name}.yuv"
                    rm -f "$reference_tn_yuv"
                    rm -f "dec_${stream_name}_tn.yuv"
                fi
            else
                echo "FAILED"
            fi
        else
            echo "OK"
            if [ "$TB_REMOVE_PASSED_YUV" == "ENABLED" ]
            then
                rm -f "$reference_yuv"
                rm -f "dec_${stream_name}.yuv"
            fi
        fi
    else
        echo "FAILED"
    fi
        
    cd - >> /dev/null
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Decodes the streams of the current working directory, recursively.         --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : The directory where the resulting outputs are placed                   --
#-                                                                            --
#-------------------------------------------------------------------------------
decodeCurrentDir()
{
    # Directory where the resulting yuvs are copied
    target_dir="$1"
    
    # Loop through directory entries
    dir_entries=`ls`
    for dir_entry in $dir_entries
    do
        
        # If the entry is directory, handle it recursively
        if [ -d "$dir_entry" ]
        then
            # Save the current target directory for later restore
            previous_target_dir="$target_dir"
            
            # Calculate new target based on the provided target dir and the new directory
            target_dir="${target_dir}${dir_entry}/"
            
            # Create target directory if not exists
            if [ ! -d "$target_dir" ]
            then
                mkdir "$target_dir"
                chmod 777 -R "$target_dir"
            fi
            
            # Handle this directory recursively
            cd "$dir_entry"
            decodeCurrentDir "$target_dir"
            cd ..
            
            # Restore the target directory
            target_dir="$previous_target_dir"
        
        # Decode the file
        else
            decodeStream "${dir_entry}" "$target_dir"
        fi
    done
}

# Clean up previous yuvs
rm -rf "$TB_YUV_DIR"
mkdir "$TB_YUV_DIR"
chmod 777 -R "$TB_YUV_DIR"

# Decoding is started from $TB_STREAM_DIR
cd "$TB_STREAM_DIR"
decodeCurrentDir "${TB_YUV_DIR}/"
cd - >> /dev/null







