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
#--   Abstract     : Script functions for running the SW/HW verification      --
#                    test cases                                               --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

# imports
. ./config.sh
. ./f_utils.sh "run"
. $TEST_CASE_LIST_RUN

g_hw_major=""
g_hw_minor=""

randomizeSlice()
{
    case_number=$1
    webp_category=$2
    
    # Randomize JPEG_MCUS_SLICE
    random_value=$RANDOM; let "random_value %= 2"
    jpeg_slice_mode=$random_value
    if [ "$jpeg_slice_mode" == 0 ]
    then
        JPEG_MCUS_SLICE=0
    else
        # randomize selector of MCU rows/slice [0, max JPEG_MCUS_SLICE]
        if [ ! -e "${TEST_DATA_HOME_RUN}/case_${case_number}/picture_ctrl_dec.trc" ]
        then
            echo "${TEST_DATA_HOME_RUN}/case_${case_number}/picture_ctrl_dec.trc missing"
            return
        fi
    
        # calculate max JPEG_MCUS_SLICE value
        # parse sampling format from picture_ctrl_dec.trc
        if [ ! -z "$webp_category" ]
        then
            sampling_format=2
        else
            sampling_format=`cat ${TEST_DATA_HOME_RUN}/case_${case_number}/picture_ctrl_dec.trc | grep "JPEG sampling format"`
            sampling_format=${sampling_format:0:1}
        fi
    
        # parse picture height from picture_ctrl_dec.trc
        pic_heights=`cat ${TEST_DATA_HOME_RUN}/case_${case_number}/picture_ctrl_dec.trc | grep -i "picture height"` 
        declare -a tmp 
        tmp=($pic_heights)
        pic_height=${tmp[0]}
        pic_height=${pic_height%% *}
        let 'pic_height=pic_height*16'
    
        # parse picture width from picture_ctrl_dec.trc
        pic_widths=`cat ${TEST_DATA_HOME_RUN}/case_${case_number}/picture_ctrl_dec.trc | grep -i "picture width"` 
        tmp=($pic_widths)
        pic_width=${tmp[0]}
        pic_width=${pic_width%% *}
        let 'pic_width=pic_width*16'
    
        # caluculate amount of MCUs and MCUs in row based on picture dimensions and sampling format
        let 'amount_of_MCUs=0'
        let 'mcu_in_row=0'
        # 4:0:0
        if [ "$sampling_format" == 0 ]
        then
            let 'amount_of_MCUs=pic_width*pic_height/64'
            let 'mcu_in_row=pic_width/8'
        
        # 4:2:0
        elif [ "$sampling_format" == 2 ]
        then
            let 'amount_of_MCUs=pic_width*pic_height/256'
            let 'mcu_in_row=pic_width/16'
        
        # 4:2:2
        elif [ "$sampling_format" == 3 ]
        then
            let 'amount_of_MCUs=pic_width*pic_height/128'
            let 'mcu_in_row=pic_width/16'
        
        # 4:4:0
        elif [ "$sampling_format" == 5 ]
        then
            let 'amount_of_MCUs=pic_width*pic_height/128'
            let 'mcu_in_row=pic_width/8'
                
        # 4:4:4
        elif [[ "$PRODUCT" == "8190" || "$PRODUCT" == "9170" || "$PRODUCT" == "9190"  || "$PRODUCT" == "g1" ]] && [ "$sampling_format" == 4 ]
        then
            let 'amount_of_MCUs=pic_width*pic_height/64'
            let 'mcu_in_row=pic_width/8'
                
        # 4:1:1
        elif  [[ "$PRODUCT" == "8190" || "$PRODUCT" == "9170" || "$PRODUCT" == "9190"  || "$PRODUCT" == "g1" ]] && [ "$sampling_format" == 6 ]
        then
            let 'amount_of_MCUs=pic_width*pic_height/256'
            let 'mcu_in_row=pic_width/32'
        
        # assume not a JPEG case -> do no randomize anymore
        else
            return
        fi

        # calculate max MCUs slice
        #if [ "$PRODUCT" == "8170" ]
        #then
        if [ "$PRODUCT" == "8170" ]
        then
            jpeg_max_slice_size="4096"
                    
        elif  [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
        then
            jpeg_max_slice_size="8100"
        fi
        let 'max_mcus_slice=0'
        let 'condition=max_mcus_slice*mcu_in_row+mcu_in_row'
        while [ "$condition" -lt "$jpeg_max_slice_size" ]
        do
            let 'max_mcus_slice=max_mcus_slice+1'
            let 'condition=max_mcus_slice*mcu_in_row+mcu_in_row'

            let 'condition2=0'
            if [ "$sampling_format" == 0 ]
            then
                let 'condition2=max_mcus_slice*mcu_in_row*2'
        
            elif [ "$sampling_format" == 2 ]
            then
                let 'condition2=max_mcus_slice*mcu_in_row'
        
            elif [ "$sampling_format" == 3 ]
            then
                let 'condition2=max_mcus_slice*mcu_in_row'
            
            elif [ "$sampling_format" == 5 ]
            then
                let 'condition2=max_mcus_slice*mcu_in_row*2'
                        
            elif [[ "$PRODUCT" == "8190" || "$PRODUCT" == "9170" || "$PRODUCT" == "9190"  || "$PRODUCT" == "g1" ]] && [ "$sampling_format" == 4 ]
            then
                let 'condition2=max_mcus_slice*mcu_in_row*2'
                
            elif [[ "$PRODUCT" == "8190" || "$PRODUCT" == "9170" || "$PRODUCT" == "9190"  || "$PRODUCT" == "g1" ]] && [ "$sampling_format" == 6 ]
            then
                let 'condition2=max_mcus_slice*mcu_in_row'
            fi
        
            if [ $condition2 -gt $amount_of_MCUs ]
            then
                let 'max_mcus_slice=max_mcus_slice-1'
                break
            fi
        done
        
        if [ "$max_mcus_slice" -gt "255" ]
        then
            max_mcus_slice="255"
        fi
        # randomize the value [0, max_mcus_slice]
        let 'max_mcus_slice=max_mcus_slice+1'
        random_value=$RANDOM; let "random_value %= max_mcus_slice"
        JPEG_MCUS_SLICE=$random_value
    fi
}

randomizeStride()
{
    case_number=$1

    #echo "stride enabled"   
    getDecCaseWidth $case_number
    width="$dec_case_width"
    getDecCaseHeight $case_number
    height="$dec_case_height"

    VP8_LUMA_STRIDE="0"
    VP8_CHROMA_STRIDE="0"
    VP8_INTERLEAVED_PIC_BUFFERS="DISABLED"

    # Minimum luma and chroma strides
    if [ "$case_number" -eq "1407" ]
    then
         VP8_LUMA_STRIDE="1024"
         VP8_CHROMA_STRIDE="1024"
 
    # Luma and chroma stride are different
    elif [ "$case_number" -eq "1408" ]
    then
         VP8_LUMA_STRIDE="4096"
         VP8_CHROMA_STRIDE="2048"
    
    # Luma and chroma stride are different
    elif [ "$case_number" -eq "7714" ]
    then
         VP8_LUMA_STRIDE="1024"
         VP8_CHROMA_STRIDE="4096"

    # Large video frames for stride
    elif [ "$case_number" -eq "7707" ]
    then
         VP8_LUMA_STRIDE="16384"
         VP8_CHROMA_STRIDE="16384"

    # Maximum strides for video
    elif [ "$case_number" -eq "1404" ]
    then
         VP8_LUMA_STRIDE="262144"
         VP8_CHROMA_STRIDE="262144"

    # Maximum strides for still
    elif [ "$case_number" -eq "8706" ]
    then
         VP8_LUMA_STRIDE="262144"
         VP8_CHROMA_STRIDE="262144"

     # Check that slice mode limit is increased, should be tested with 256k x 4k 
    elif [ "$case_number" -eq "8707" ]
    then
         VP8_LUMA_STRIDE="8192"
         VP8_CHROMA_STRIDE="8192"

    # Maximum picture height with strides
    elif [ "$case_number" -eq "8712" ]
    then
         VP8_LUMA_STRIDE="1024"
         VP8_CHROMA_STRIDE="1024"
        
    # Slice mode test
    elif [ "$case_number" -eq "8714" ]
    then
        VP8_LUMA_STRIDE="16384"
        VP8_CHROMA_STRIDE="16384"
        
    # Large still picture width with strides; small increase of strides compared to picture dimensions
    elif [ "$case_number" -eq "8716" ]
    then
        VP8_LUMA_STRIDE="16384"
        VP8_CHROMA_STRIDE="16384"
       
    elif [ "$case_number" -eq "7702" ]
    then
        # Interleaved tests, same buffer
        if [ "$DEC_MEMORY_ALLOCATION" == "EXTERNAL" ]
        then
            VP8_LUMA_STRIDE="2048"
            VP8_CHROMA_STRIDE="4096"
            VP8_INTERLEAVED_PIC_BUFFERS="ENABLED"
        fi
    elif [ "$case_number" -eq "1409" ]
    then
        # Interleaved tests, same buffer, CIF picture (refbuf enable)
        if [ "$DEC_MEMORY_ALLOCATION" == "EXTERNAL" ]
        then
            VP8_LUMA_STRIDE="4096"
            VP8_CHROMA_STRIDE="4096"
            VP8_INTERLEAVED_PIC_BUFFERS="ENABLED"
        fi
    elif [ "$case_number" -eq "7703" ]
    then
        # Interleaved tests, different buffer
        if [ "$DEC_MEMORY_ALLOCATION" == "EXTERNAL" ]
        then
            VP8_LUMA_STRIDE="1024"
            VP8_CHROMA_STRIDE="1024"
            VP8_INTERLEAVED_PIC_BUFFERS="ENABLED"
        fi
    elif [ "$case_number" -eq "1410" ]
    then
        # Interleaved tests, same buffer, CIF picture (refbuf enable)
        if [ "$DEC_MEMORY_ALLOCATION" == "EXTERNAL" ]
        then
            VP8_LUMA_STRIDE="2048"
            VP8_CHROMA_STRIDE="2048"
            VP8_INTERLEAVED_PIC_BUFFERS="ENABLED"
        fi
    
    else
        if [ "$width" -gt 0 ] && [ "$height" -gt 0 ]
        then
            stride_values="1024 2048 4096 8192 16384 32768 65536 131072 262144"
            stride_max_value="262144"
            case_dir="${TEST_DATA_HOME_RUN}/case_${case_number}"
            # webp stride values
            if [ -e "${case_dir}/${WEBP_STREAM}" ]
            then
                # slice mode is disable by SW -> cannot allocate big strides
                if [ "$width" -le "$DEC_MAX_OUTPUT" ]
                then
                    # 30MB is reserved for other stuff
                    let 'stride_max_value=((MEM_SIZE_MB-30)*1024*1024*10)/(dec_case_height*15)'
                fi
            else
                # 4 picture buffers
		let 'stride_max_value=((MEM_SIZE_MB-30)*1024*1024*10)/(dec_case_height*15*4)'
            fi
             
            # set the minimum and maximum (and between) values for strides
            let 'stride_values_cnt=0'
            declare -a stride_values_arr
            for stride_value in $stride_values
            do
                if [ "$stride_value" -gt "$dec_case_width" ] && [ "$stride_value" -lt "$stride_max_value" ]
                then
                    stride_values_arr[$stride_values_cnt]="$stride_value"
                    let 'stride_values_cnt=stride_values_cnt+1'
                fi
            done
            if [ "$stride_values_cnt" -gt 0 ]
            then
                # randomize stride values
                random_i_luma=$RANDOM; let 'random_i_luma%=stride_values_cnt'
                random_value=$RANDOM; let "random_value%=2"
                if [ "$random_value" == 1 ]
                then
                   let 'random_i_chroma=random_i_luma'
                else
                   random_i_chroma=$RANDOM; let 'random_i_chroma%=stride_values_cnt'
                fi
                luma_stride=${stride_values_arr[$random_i_luma]}
                chroma_stride=${stride_values_arr[$random_i_chroma]}
                    
                # now, set the stride parameters
                VP8_LUMA_STRIDE="$luma_stride"
                VP8_CHROMA_STRIDE="$chroma_stride"
                  
                # check if we can use interleaved buffers
                VP8_INTERLEAVED_PIC_BUFFERS="DISABLED"
                if [ "$DEC_MEMORY_ALLOCATION" == "EXTERNAL" ] && [ -e "${case_dir}/${VP8_STREAM}" ]
                then
                    let 'interleaved_limit=width*4'
                    if [ "$luma_stride" -ge "$interleaved_limit" ] && [ "$chroma_stride" -ge "$interleaved_limit" ]
                    then
                        random_value=$RANDOM; let "random_value%=2"
                        if [ "$random_value" == 1 ]
		        then
		           VP8_INTERLEAVED_PIC_BUFFERS="ENABLED"
		        fi
                    fi
                fi
                if [ -e "${case_dir}/${WEBP_STREAM}" ]
                then
                    # set slice if > 16 mpix with strides
                    # needed by FPGA test enviroment
                    let 'pic_width=0'
                    if [ "$luma_stride" -gt "$chroma_stride" ]
                    then
                        let 'pic_width=luma_stride'
                    else
                        let 'pic_width=chroma_stride'
                    fi
                    let 'slice_limit=dec_case_height*pic_width'
                    if [ "$slice_limit" -gt 16370688 ]
                    then
                        #jpeg_slice=1
                        jpeg_slice=0
                        while [ "$jpeg_slice" -eq 0 ]
                        do
                            randomizeSlice "$case_number" "1"
                            jpeg_slice="$JPEG_MCUS_SLICE"
                        done
                    fi
                fi
            fi
        fi        
    fi
}
#-------------------------------------------------------------------------------
#-                                                                            --
#- Randomizes the configuration parameters.                                   --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#-------------------------------------------------------------------------------
randomizeCfg()
{
    case_number=$1
    
    getCategory "$case_number"
    ablend_category=`echo $category | grep ablend`
    webp_category=`echo $category | grep webp`
    
    # decoder randomization
    ## randomize output picture endian
    #random_value=$RANDOM; let "random_value %= 2"
    #if [ "$TEST_ENV_DATA_BUS_WIDTH" == "32_BIT" ] && [ -z "$ablend_category" ]
    #then
    #    if [ "$random_value" == 1 ]
    #    then
    #        DEC_OUTPUT_PICTURE_ENDIAN=LITTLE_ENDIAN
    #    else
    #        DEC_OUTPUT_PICTURE_ENDIAN=BIG_ENDIAN
    #    fi
    #else
    #    echo "DEC_OUTPUT_PICTURE_ENDIAN randomization disabled "
    #    DEC_OUTPUT_PICTURE_ENDIAN="LITTLE_ENDIAN"
    #fi

    # randomize bus burst length
    if [ $HW_BUS == "AHB" ]
    then
        random_value=$RANDOM; let "random_value %= 6"
        if [ $random_value -eq 0 ]
        then
            DEC_BUS_BURST_LENGTH=16
        elif [ $random_value -eq 1 ]
        then
            DEC_BUS_BURST_LENGTH=8
        elif [ $random_value -eq 2 ]
        then
            DEC_BUS_BURST_LENGTH=4
        elif [ $random_value -eq 3 ]
        then
            DEC_BUS_BURST_LENGTH=1
        elif [ $random_value -eq 4 ]
        then
            DEC_BUS_BURST_LENGTH=17
        else
            DEC_BUS_BURST_LENGTH=0
        fi
        
    elif [ $HW_BUS == "OCP" ]
    then
        random_value=$RANDOM; let "random_value %= 31"
        let "random_value = random_value + 1"
        DEC_BUS_BURST_LENGTH=$random_value
    
    elif [ $HW_BUS == "AXI" ]
    then
        random_value=$RANDOM; let "random_value %= 16"
        let "random_value = random_value + 1"
        DEC_BUS_BURST_LENGTH=$random_value
    fi

    if [ "$PRODUCT" == 8170 ]
    then
        # randomize asic service priority
        random_value=$RANDOM; let "random_value %= 5"
        DEC_ASIC_SERVICE_PRIORITY="$random_value"
    fi

    # randomize output format
    if [ "$PRODUCT" == "8170" ]
    then
        random_value=$RANDOM; let "random_value %= 2"
        if [ "$random_value" == 1 ]
        then
            DEC_OUTPUT_FORMAT="TILED"
        else
            DEC_OUTPUT_FORMAT="RASTER_SCAN"
        fi
    fi
    
    ## randomize latency compensation: [0, $TB_MAX_DEC_LATENCY_COMPENSATION]
    echo "DEC_LATENCY_COMPENSATION randomization disabled"
    #let 'max_latency_compensation=TB_MAX_DEC_LATENCY_COMPENSATION+1'
    #random_value=$RANDOM; let "random_value %= ${max_latency_compensation}"
    #DEC_LATENCY_COMPENSATION=$random_value

    # randomize clock gating
    random_value=$RANDOM; let "random_value %= 2"
    if [ "$random_value" == 1 ]
    then
        DEC_CLOCK_GATING="ENABLED"
    else
        DEC_CLOCK_GATING="DISABLED"
    fi
    
    # randomize data discard: enabled == 1, disabled == 0
    random_value=$RANDOM; let "random_value %= 2"
    if [ "$random_value" == 1 ]
    then
        DEC_DATA_DISCARD="ENABLED"
    else
        DEC_DATA_DISCARD="DISABLED"
    fi
    
    # post-processor randomization
    # randomize bus burst length
    if [ $HW_BUS == "AHB" ]
    then
        random_value=$RANDOM; let "random_value %= 6"
        if [ $random_value -eq 0 ]
        then
            PP_BUS_BURST_LENGTH=16
        elif [ $random_value -eq 1 ]
        then
            PP_BUS_BURST_LENGTH=8
        elif [ $random_value -eq 2 ]
        then
            PP_BUS_BURST_LENGTH=4
        elif [ $random_value -eq 3 ]
        then
            PP_BUS_BURST_LENGTH=1
        elif [ $random_value -eq 4 ]
        then
            DEC_BUS_BURST_LENGTH=17
        else
            PP_BUS_BURST_LENGTH=0
        fi
        
    elif [ $HW_BUS == "OCP" ]
    then
        random_value=$RANDOM; let "random_value %= 31"
        let "random_value = random_value + 1"
        PP_BUS_BURST_LENGTH=$random_value
        
    elif [ $HW_BUS == "AXI" ]
    then
        random_value=$RANDOM; let "random_value %= 16"
        let "random_value = random_value + 1"
        PP_BUS_BURST_LENGTH=$random_value
    fi
    
    # randomize clock gating: enabled == 1, disabled == 0
    random_value=$RANDOM; let "random_value %= 2"
    if [ "$random_value" == 1 ]
    then
        PP_CLOCK_GATING="ENABLED"
    else
        PP_CLOCK_GATING="DISABLED"
    fi
    
    # randomize data discard: enabled == 1, disabled == 0
    random_value=$RANDOM; let "random_value %= 2"
    if [ "$random_value" == 1 ]
    then
        PP_DATA_DISCARD="ENABLED"
    else
        PP_DATA_DISCARD="DISABLED"
    fi
    
    # randomize TB_USE_PEEK_FOR_OUTPUT: enabled == 1, disabled == 0
    random_value=$RANDOM; let "random_value %= 2"
    if [ "$random_value" == 1 ]
    then
        TB_USE_PEEK_FOR_OUTPUT="ENABLED"
    else
        TB_USE_PEEK_FOR_OUTPUT="DISABLED"
    fi
    
    if [ ! -z "$webp_category" ]
    then
        TB_USE_PEEK_FOR_OUTPUT="DISABLED"
    fi
    
    # randomize TB_SKIP_NON_REF_PICS: enabled == 1, disabled == 0
    random_value=$RANDOM; let "random_value %= 2"
    if [ "$random_value" == 1 ]
    then
        TB_SKIP_NON_REF_PICS="ENABLED"
    else
        TB_SKIP_NON_REF_PICS="DISABLED"
    fi
    
    #if [ "$TEST_ENV_DATA_BUS_WIDTH" == "32_BIT" ] && [ -z "$ablend_category" ]
    #then
    #    # Use same endianess as in decoder
    #    PP_INPUT_PICTURE_ENDIAN=$DEC_OUTPUT_PICTURE_ENDIAN
    #else
    #    echo "PP_INPUT_PICTURE_ENDIAN randomization disabled"
    #    PP_INPUT_PICTURE_ENDIAN="PP_CFG"
    #fi
    
    ## randomize output picture endian
    #random_value=$RANDOM; let "random_value %= 2"
    #if [ "$TEST_ENV_DATA_BUS_WIDTH" == "32_BIT" ] && [ -z "$ablend_category" ]
    #then
    #    if [ "$random_value" == 1 ]
    #    then
    #        PP_OUTPUT_PICTURE_ENDIAN="LITTLE_ENDIAN"
    #    else
    #        PP_OUTPUT_PICTURE_ENDIAN="BIG_ENDIAN"
    #    fi
    #else
    #    echo "PP_OUTPUT_PICTURE_ENDIAN randomization disabled"
    #    PP_OUTPUT_PICTURE_ENDIAN="PP_CFG"
    #fi
    
    ## randomize word swap
    #random_value=$RANDOM; let "random_value %= 2"
    #if [ "$TEST_ENV_DATA_BUS_WIDTH" == "32_BIT" ] && [ -z "$ablend_category" ]
    #then
    #    if [ "$random_value" == 1 ]
    #    then
    #        PP_WORD_SWAP="ENABLED"
    #    else
    #        PP_WORD_SWAP="DISABLED"
    #    fi
    #else
    #    echo "PP_WORD_SWAP randomization disabled"
    #    PP_WORD_SWAP="PP_CFG"
    #fi
    
    ## randomize word swap
    #random_value=$RANDOM; let "random_value %= 2"
    #if [ "$TEST_ENV_DATA_BUS_WIDTH" == "32_BIT" ] && [ -z "$ablend_category" ]
    #then
    #    if [ "$random_value" == 1 ]
    #    then
    #        PP_WORD_SWAP16="ENABLED"
    #    else
    #        PP_WORD_SWAP16="DISABLED"
    #    fi
    #else
    #    echo "PP_WORD_SWAP16 randomization disabled"
    #    PP_WORD_SWAP16="PP_CFG"
    #fi
    
    #if [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge 1 && "$g_hw_minor" -ge 41 ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge 2 ]]
    #then
    #    random_value=$RANDOM; let "random_value %= 2"
    #    if [ "$random_value" == 1 ]
    #    then
    #        PP_OUTPUT_FORMAT="RASTER_SCAN"
    #    else
    #        PP_OUTPUT_FORMAT="TILED"
    #    fi
    #fi
    
    if [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
    then
        # randomize reference buffer latency setting
        random_value=$RANDOM; let "random_value %= 501"
        DEC_REFERENCE_BUFFER_LATENCY=$random_value
    
        # randomize reference buffer nonseq setting
        random_value=$RANDOM; let "random_value %= 64"
        DEC_REFERENCE_BUFFER_NONSEQ=$random_value
    
        # randomize reference buffer seq setting
        random_value=$RANDOM; let "random_value %= 64"
        DEC_REFERENCE_BUFFER_SEQ=$random_value
    fi
    
    random_value=$RANDOM; let "random_value %= 2"
    if [ "$random_value" == 1 ]
    then
        TB_VP8_STRIDE="DISABLED"
    else
        TB_VP8_STRIDE="ENABLED"
    fi
    
    # JPEG specific randomization
    getComponent "$case_number"
    if [ "$decoder" == "jpeg" ] ||  [ ! -z "$webp_category" ]
    #if [ "$decoder" == "jpeg" ]
    then
        
        randomizeSlice "$case_number" "$webp_category"
    
        if [ ! -z "$webp_category" ]
        then
            return
        fi
        
        # randomize JPEG_INPUT_BUFFER_SIZE
        # first, decide whether to use the input buffer
        random_value=$RANDOM; let "random_value %= 2"
        jpeg_input_buffer=$random_value
        if [ "$jpeg_input_buffer" == 0 ]
        then
            JPEG_INPUT_BUFFER_SIZE=0
        else
            stream_size=0
            ls_info=`ls -l ${TEST_DATA_HOME_RUN}/case_${case_number}/${JPEG_STREAM}`
            let element_number=0
            for i in $ls_info
            do
                let 'element_number=element_number+1'
                if [ $element_number == 5 ]
                then
                    stream_size=$i
                fi
            done
            # the valid values for input buffer size are [5120, 16776960]
            # the size must be multipe of 256
            # however, the maximum input buffer size is limited to maximum stream size
            let 'tmp=stream_size-5120'
            if [ "$tmp" -ge 0 ]
            then
                # chunks is the number of possible increments for input buffer size
                let 'chunks=tmp/256'
                # randomize the number of chunks
                random_value=$RANDOM; let "random_value %= (chunks+1)"
                # number of chunks*256 are added to 5120 to define the input buffer size
                let JPEG_INPUT_BUFFER_SIZE=(random_value+20)*256
                if [ "$JPEG_INPUT_BUFFER_SIZE" -gt 16776960 ]
                then
                    JPEG_INPUT_BUFFER_SIZE=16776960
                fi
            
            else
                JPEG_INPUT_BUFFER_SIZE=0  
            fi
        fi
    fi
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Randomizes all possible parameters.                                        --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#-------------------------------------------------------------------------------
randomizeAll()
{
    case_number=$1
    
    randomizeCfg "$case_number"
    
    random_value=$RANDOM; let "random_value %= 2"
    if [ "$random_value" == 1 ]
    then
        TB_PACKET_BY_PACKET="ENABLED"
    else
        TB_PACKET_BY_PACKET="DISABLED"
    fi
    
    random_value=$RANDOM; let "random_value %= 2"
    if [ "$random_value" == 1 ]
    then
        TB_WHOLE_STREAM="ENABLED"
    else
        TB_WHOLE_STREAM="DISABLED"
    fi
    
    random_value=$RANDOM; let "random_value %= 2"
    if [ "$random_value" == 1 ]
    then
        isValidH264NalCase "$case_number"
        if [ $? == 1 ]
        then
            TB_NAL_UNIT_STREAM="ENABLED"
            TB_PACKET_BY_PACKET="ENABLED"
        else
            TB_NAL_UNIT_STREAM="DISABLED"
        fi
    else
        TB_NAL_UNIT_STREAM="DISABLED"
    fi
    
    random_value=$RANDOM; let "random_value %= 2"
    if [ "$random_value" == 1 ]
    then
        isValidVC1UdCase "$case_number"
        if [ $? == 1 ]
        then
            TB_VC1_UD="ENABLED"
            TB_PACKET_BY_PACKET="ENABLED"
        else
            TB_VC1_UD="DISABLED"
        fi
    else
        TB_VC1_UD="DISABLED"
    fi
    
    random_value=$RANDOM; let "random_value %= 2"
    if [ "$random_value" == 1 ]
    then
        DEC_MEMORY_ALLOCATION="INTERNAL"
    else
        DEC_MEMORY_ALLOCATION="EXTERNAL"
    fi
    
    random_value=$RANDOM; let "random_value %= 2"
    if [ "$random_value" == 1 ]
    then
        isValidRlcModeCase "$case_number"
        if [ $? == 1 ]
        then
            DEC_RLC_MODE_FORCED="ENABLED"
        else
            DEC_RLC_MODE_FORCED="DISABLED"
        fi
    else
        DEC_RLC_MODE_FORCED="DISABLED"
    fi
    
    random_value=$RANDOM; let "random_value %= 2"
    if [ "$random_value" == 1 ]
    then
        isValidMultiBufferCase "$case_number"
        if [ $? == 1 ]
        then
            PP_MULTI_BUFFER="ENABLED"
        else
            PP_MULTI_BUFFER="DISABLED"
        fi
    else
        PP_MULTI_BUFFER="DISABLED"
    fi

    random_value=$RANDOM; let "random_value %= 2"
    if [ "$random_value" == 1 ]
    then
        TB_VP8_STRIDE="ENABLED"
    else
        TB_VP8_STRIDE="DISABLED"
    fi
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Randomizes the order of the provided test cases.                           --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : List of test cases to be randomized                                    --
#-                                                                            --
#- Exports                                                                    --
#- rnd_cases : Randomized list of test cases                                  --
#-                                                                            --
#-------------------------------------------------------------------------------
randomizeCases()
{
    # convert the list of test cases to array
    all_cases_arr=($1)
    
    # calculate the number of test cases
    let number_of_cases=0
    for x in $1
    do
        let 'number_of_cases=number_of_cases+1'
    done
    
    # place a test case from the array of all test cases to the randomized array of test cases
    # the index of the test case is randomized
    let i=0
    declare -a rnd_cases_arr
    while [ 0 != "$number_of_cases" ]
    do
        # randomize the index
        random_i=$RANDOM; let "random_i %= number_of_cases"
        
        # get a randomized test case and place it to the array of test cases
        rnd_cases_arr[$i]=${all_cases_arr[$random_i]}
        
        # remove the randomized test case
        unset all_cases_arr[$random_i]
        all_cases_arr=(${all_cases_arr[*]})
        let 'number_of_cases=number_of_cases-1'
        let 'i=i+1'
    done
    
    # convert the randomized array of test cases to list
    rnd_cases=${rnd_cases_arr[*]}
    export rnd_cases
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Writes the configuration file used by the test bench binary.               --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#-------------------------------------------------------------------------------
writeConfiguration()
{
    case_number=$1
    
    rm -f tb.cfg

    echo "DecParams {" >> tb.cfg
    echo "    OutputPictureEndian = ${DEC_OUTPUT_PICTURE_ENDIAN};"  >> tb.cfg
    echo "    BusBurstLength = ${DEC_BUS_BURST_LENGTH};"  >> tb.cfg
    if [ "$PRODUCT" == 8170 ]
    then
        echo "    AsicServicePriority = ${DEC_ASIC_SERVICE_PRIORITY};"  >> tb.cfg
    fi
    echo "    OutputFormat = ${DEC_OUTPUT_FORMAT};"  >> tb.cfg
    echo "    LatencyCompensation = ${DEC_LATENCY_COMPENSATION};"  >> tb.cfg
    echo "    ClockGating = ${DEC_CLOCK_GATING};"  >> tb.cfg
    echo "    DataDiscard = ${DEC_DATA_DISCARD};"  >> tb.cfg
    echo "    MemoryAllocation = ${DEC_MEMORY_ALLOCATION};"  >> tb.cfg
    echo "    RlcModeForced = ${DEC_RLC_MODE_FORCED};"  >> tb.cfg
    if [ "$DEC_STANDARD_COMPLIANCE" == "STRICT" ]
    then
        echo "    SupportNonCompliant = 0;"  >> tb.cfg
    elif [ "$DEC_STANDARD_COMPLIANCE" == "LOOSE" ]
    then
        echo "    SupportNonCompliant = 1;"  >> tb.cfg
    fi
    if [ "$DEC_REFERENCE_BUFFER" == "REF_BUFFER" ]
    then
        echo "    refbuEnable = 1;"  >> tb.cfg
        echo "    refbuDisableDouble = 1;"  >> tb.cfg
        if [ "$DEC_REFERENCE_BUFFER_INTERLACED" == "ENABLED" ]
        then
            echo "    refbuDisableInterlaced = 0;"  >> tb.cfg
        else
            echo "    refbuDisableInterlaced = 1;"  >> tb.cfg
        fi
        if [ "$DEC_REFERENCE_BUFFER_TOP_BOT_SUM" == "ENABLED" ]
        then
            echo "    refbuDisableTopBotSum = 0;"  >> tb.cfg
        else
            echo "    refbuDisableTopBotSum = 1;"  >> tb.cfg
        fi
    elif [ "$DEC_REFERENCE_BUFFER" == "REF_BUFFER2" ]
    then
        echo "    refbuEnable = 1;"  >> tb.cfg
        echo "    refbuDisableDouble = 0;"  >> tb.cfg
        if [ "$DEC_REFERENCE_BUFFER_INTERLACED" == "ENABLED" ]
        then
            echo "    refbuDisableInterlaced = 0;"  >> tb.cfg
        else
            echo "    refbuDisableInterlaced = 1;"  >> tb.cfg
        fi
        if [ "$DEC_REFERENCE_BUFFER_TOP_BOT_SUM" == "ENABLED" ]
        then
            echo "    refbuDisableTopBotSum = 0;"  >> tb.cfg
        else
            echo "    refbuDisableTopBotSum = 1;"  >> tb.cfg
        fi
    elif [ "$DEC_REFERENCE_BUFFER" == "DISABLED" ]
    then
        echo "    refbuEnable = 0;"  >> tb.cfg
    fi
    if [ "$DEC_REFERENCE_BUFFER_EVAL_MODE" == "ENABLED" ]
    then
        echo "    refbuDisableEvalMode = 0;"  >> tb.cfg
    elif [ "$DEC_REFERENCE_BUFFER_EVAL_MODE" == "DISABLED" ]
    then
        echo "    refbuDisableEvalMode = 1;"  >> tb.cfg
    fi
    if [ "$DEC_REFERENCE_BUFFER_CHECKPOINT" == "ENABLED" ]
    then
        echo "    refbuDisableCheckpoint = 0;"  >> tb.cfg
    elif [ "$DEC_REFERENCE_BUFFER_CHECKPOINT" == "DISABLED" ]
    then
        echo "    refbuDisableCheckpoint = 1;"  >> tb.cfg
    fi
    if [ "$DEC_REFERENCE_BUFFER_OFFSET" == "ENABLED" ]
    then
        echo "    refbuDisableOffset = 0;"  >> tb.cfg
    elif [ "$DEC_REFERENCE_BUFFER_OFFSET" == "DISABLED" ]
    then
        echo "    refbuDisableOffset = 1;"  >> tb.cfg
    fi
    if [ "$HW_DATA_BUS_WIDTH" == "32_BIT" ]
    then
        echo "    BusWidthEnable64Bit = 0;"  >> tb.cfg
    elif [ "$HW_DATA_BUS_WIDTH" == "64_BIT" ]
    then
        echo "    BusWidthEnable64Bit = 1;"  >> tb.cfg
    fi
    if [ "$DEC_2ND_CHROMA_TABLE" == "ENABLED" ]
    then
        echo "    Ch8PixIleavOutput = 1;"  >> tb.cfg
    else
        echo "    Ch8PixIleavOutput = 0;"  >> tb.cfg
    fi
    
    # detect decoding formats
    dec_profile=`echo $HWCONFIGTAG | grep h264bp`
    if [ ! -z "$dec_profile"  ]
    then
        echo "    SupportH264 = 1;"  >> tb.cfg
    else
        dec_profile=`echo $HWCONFIGTAG | grep h264hp`
        if [ ! -z "$dec_profile"  ]
        then
            echo "    SupportH264 = 3;"  >> tb.cfg
        fi
    fi
    mpeg4_id=""
    if [ "$PRODUCT" == "g1" ]
    then
        mpeg4_id="mp4"
    else
        mpeg4_id="mpeg4"
        
    fi
    dec_profile=`echo $HWCONFIGTAG | grep ${mpeg4_id}sp`
    if [ ! -z "$dec_profile"  ]
    then
        echo "    SupportMpeg4 = 1;"  >> tb.cfg
    else
        dec_profile=`echo $HWCONFIGTAG | grep ${mpeg4_id}asp`
        if [ ! -z "$dec_profile"  ]
        then
            echo "    SupportMpeg4 = 2;"  >> tb.cfg
        fi
    fi
    dec_profile=`echo $HWCONFIGTAG | grep vc1mp`
    if [ ! -z "$dec_profile"  ]
    then
        echo "    SupportVc1 = 2;"  >> tb.cfg
    else
        dec_profile=`echo $HWCONFIGTAG | grep vc1ap`
        if [ ! -z "$dec_profile"  ]
        then
            echo "    SupportVc1 = 3;"  >> tb.cfg
        fi
    fi
    divx_configuration=`echo "$HWCONFIGTAG" | grep divx`
    if [ ! -z "$divx_configuration" ]
    then
        echo "    SupportCustomMpeg4 = 1;"  >> tb.cfg
    else
        echo "    SupportCustomMpeg4 = 0;"  >> tb.cfg
    fi
    if [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "2" && "$g_hw_minor" -ge "10" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge 3 ]]
    then
        echo "    SupportWebp = 1;"  >> tb.cfg
    else
        echo "    SupportWebp = 0;"  >> tb.cfg
    fi
    if [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "1" && "$g_hw_minor" -ge "88" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge 2 ]]
    then
        echo "    SupportTiledReference = 1;"  >> tb.cfg
    else
        echo "    SupportTiledReference = 0;"  >> tb.cfg
    fi
    if [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "3" && "$g_hw_minor" -ge "4" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge 4 ]]
    then
        echo "    SupportFieldDPB = 1;"  >> tb.cfg
    else
        echo "    SupportFieldDPB = 0;"  >> tb.cfg
    fi
    mvc_configuration=`echo "$HWCONFIGTAG" | grep mvc`
    h264hp_profile=`echo $HWCONFIGTAG | grep h264hp`
    # configuration support for MVC
    if [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "2" && "$g_hw_minor" -ge "14" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge 3 ]]
    then
        if [ -z "$mvc_configuration" ]
        then
            echo "    SupportMvc = 0;"  >> tb.cfg
        else
            echo "    SupportMvc = 1;"  >> tb.cfg
        fi
    # MVC is support, if h264 hp profile is supported
    elif [[ ! -z "$h264hp_profile" ]] && [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "1" && "$g_hw_minor" -ge "68" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge 2 ]]
    then
        echo "    SupportMvc = 1;"  >> tb.cfg
    else
        echo "    SupportMvc = 0;"  >> tb.cfg
    fi
    if [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "2" && "$g_hw_minor" -ge "70" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge "3" ]]
    then
        echo "    SupportStride = 1;"  >> tb.cfg
    else
        echo "    SupportStride = 0;"  >> tb.cfg
    fi
    echo "    MemLatencyClks = ${DEC_REFERENCE_BUFFER_LATENCY};"  >> tb.cfg
    echo "    MemNonSeqClks = ${DEC_REFERENCE_BUFFER_NONSEQ};"  >> tb.cfg
    echo "    MemSeqClks = ${DEC_REFERENCE_BUFFER_SEQ};"  >> tb.cfg
    echo "    JpegMcusSlice = ${JPEG_MCUS_SLICE};"  >> tb.cfg
    echo "    JpegInputBufferSize = ${JPEG_INPUT_BUFFER_SIZE};"  >> tb.cfg
    if [ "$PRODUCT" == "g1" ]
    then
        echo "    HwVersion = 10000;"  >> tb.cfg    
    else    
        echo "    HwVersion = ${PRODUCT};"  >> tb.cfg
    fi
    hw_build=""
    if [ "$PRODUCT" == "g1" ]
    then
        let 'hw_build=(g_hw_major*1000)+(g_hw_minor*10)+8'
    else
        let 'hw_build=(g_hw_major*1000)+(g_hw_minor*10)'
    fi
    echo "    HwBuild = ${hw_build};"  >> tb.cfg
    echo "    MaxDecPicWidth = ${DEC_MAX_OUTPUT};"  >> tb.cfg
    echo "}"  >> tb.cfg
    
    echo "PpParams {"  >> tb.cfg
    echo "    BusBurstLength = ${PP_BUS_BURST_LENGTH};"  >> tb.cfg
    echo "    ClockGating = ${PP_CLOCK_GATING};"  >> tb.cfg
    echo "    DataDiscard = ${PP_DATA_DISCARD};"  >> tb.cfg
    echo "    InputPictureEndian = ${PP_INPUT_PICTURE_ENDIAN};"  >> tb.cfg
    echo "    OutputPictureEndian = ${PP_OUTPUT_PICTURE_ENDIAN};"  >> tb.cfg
    echo "    WordSwap = ${PP_WORD_SWAP};"  >> tb.cfg
    echo "    WordSwap16 = ${PP_WORD_SWAP16};"  >> tb.cfg
    if [[ "$PRODUCT" == "9190" && "$g_hw_major" -eq "1" && "$g_hw_minor" -ge "46" ]] || [[ "$PRODUCT" == "9190" && "$g_hw_major" -ge 2 ]]
    then
        echo "    SupportPpOutEndianess = 1;"  >> tb.cfg
        
    elif [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "1" && "$g_hw_minor" -ge "14" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge 2 ]]
    then
        echo "    SupportPpOutEndianess = 1;"  >> tb.cfg
        
    else
        echo "    SupportPpOutEndianess = 0;"  >> tb.cfg
    fi
    if [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "1" && "$g_hw_minor" -ge "41" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge 2 ]]
    then
        echo "    SupportTiled = 1;"  >> tb.cfg
    else
        echo "    SupportTiled = 0;"  >> tb.cfg
    fi
    if [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "1" && "$g_hw_minor" -ge "57" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge 2 ]]
    then
        echo "    SupportScaling = 3;"  >> tb.cfg
    else
        echo "    SupportScaling = 1;"  >> tb.cfg
    fi
    if [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "1" && "$g_hw_minor" -ge "73" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge 2 ]]
    then
        echo "    SupportPixelAccurOut = 1;"  >> tb.cfg
        echo "    SupportAblendCrop = 1;"  >> tb.cfg
    else
        echo "    SupportPixelAccurOut = 0;"  >> tb.cfg
        echo "    SupportAblenCrop = 0;"  >> tb.cfg
    fi
    if [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "1" && "$g_hw_minor" -ge "88" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge 2 ]]
    then
        if [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "3" && "$g_hw_minor" -ge "4" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge 4 ]]
        then
            echo "    SupportTiledReference = 2;"  >> tb.cfg
        else
            echo "    SupportTiledReference = 1;"  >> tb.cfg
        fi
    else
        echo "    SupportTiledReference = 0;"  >> tb.cfg
    fi
    if [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "2" && "$g_hw_minor" -ge "53" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "2" && "$g_hw_minor" -ge "68" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge 3 ]]
    then
        echo "    SupportStripeRemoval = 1;"  >> tb.cfg
    else
        echo "    SupportStripeRemoval = 0;"  >> tb.cfg
    fi
    echo "    MaxPpOutPicWidth = ${PP_MAX_OUTPUT};"  >> tb.cfg
    echo "    MultiBuffer =  ${PP_MULTI_BUFFER};"  >> tb.cfg
    echo "}"  >> tb.cfg
    
    echo "TbParams {"  >> tb.cfg
    echo "    PacketByPacket = ${TB_PACKET_BY_PACKET};"  >> tb.cfg
    echo "    SeedRnd = ${TB_SEED_RND};"  >> tb.cfg
    echo "    StreamBitSwap = ${TB_STREAM_BIT_SWAP};"  >> tb.cfg
    echo "    StreamPacketLoss = ${TB_STREAM_PACKET_LOSS};"  >> tb.cfg
    echo "    StreamHeaderCorrupt = ${TB_STREAM_HEADER_CORRUPT};"  >> tb.cfg
    echo "    StreamTruncate = ${TB_STREAM_TRUNCATE};"  >> tb.cfg
    echo "    NalUnitStream = ${TB_NAL_UNIT_STREAM};"  >> tb.cfg
    echo "    SliceUdInPacket = ${TB_VC1_UD};"  >> tb.cfg
    if [ "$TB_VP8_STRIDE" == "ENABLED" ]
    then
        let 'frame_buffer_size=(MEM_SIZE_MB*1024*1024)'
        echo "    DwlRefFrmBufferSize = ${frame_buffer_size};"  >> tb.cfg
    fi
    echo "}"  >> tb.cfg
    
    rm -f case_${case_number}_whole_stream
    if [ "$TB_WHOLE_STREAM" == "ENABLED" ]
    then
        touch case_${case_number}_whole_stream
    fi
    
    rm -f case_${case_number}_tiled_mode
    if [ "$DEC_TILED_MODE" == "ENABLED" ]
    then
        touch case_${case_number}_tiled_mode
    fi
    
    rm -f case_${case_number}_peek_output
    if [ "$TB_USE_PEEK_FOR_OUTPUT" == "ENABLED" ]
    then
        touch case_${case_number}_peek_output
    fi
    
    rm -f case_${case_number}_non_ref_output
    if [ "$TB_SKIP_NON_REF_PICS" == "ENABLED" ]
    then
        touch case_${case_number}_non_ref_output
    fi
    
    cp tb.cfg "case_${case_number}.cfg"
    
    ## reset variables
    #JPEG_MCUS_SLICE=0
    #JPEG_INPUT_BUFFER_SIZE=0
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Runs a H.264 test case. Uses remote execution (i.e., rexec) if             -- 
#- TB_REMOTE_TEST_EXECUTION_IP != "LOCAL_HOST".                                --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : - Test data stream/post-processor configuration does not exist         --
#-     - Test bench binary does not exist                                     --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
runH264Case()
{
    case_number=$1
    execution_time=$2
    
    # remove the existing decoder outputs so that check script can correctly determine the output format
    rm -f "case_${case_number}${H264_OUT_TILED_SUFFIX}"
    rm -f "case_${case_number}${H264_OUT_RASTER_SCAN_SUFFIX}"
    
    # set the test data dir
    case_dir="${TEST_DATA_HOME_RUN}/case_${case_number}"
    
    # do nothing if test data doesn't exist
    if [ ! -e "${case_dir}/${H264_STREAM}" ]
    then
        echo "NOK (${case_dir}/${H264_STREAM} missing)"
        return 1
        
    else
        instream="${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${H264_STREAM}"
    fi

    # determine the correct decoder output file
    if [ "$DEC_OUTPUT_FORMAT" == "TILED" ] || [ "$DEC_TILED_MODE" == "ENABLED" ]
    then
        if [ "$PRODUCT" != "8170" ] && [ "$PRODUCT" != "g1" ]
        then
            echo "INTERNAL ERROR: TILED not supported for ${PRODUCT}"
        fi
        decoder_out="case_${case_number}${H264_OUT_TILED_SUFFIX}"
        
    else
        decoder_out="case_${case_number}${H264_OUT_RASTER_SCAN_SUFFIX}"
    fi

    # run the case
    tb_params=""
    if [ -e "$H264_DECODER" ]
    then
        if [ "$DEC_TILED_MODE" == "ENABLED" ]
        then
            tb_params="-E ${tb_params}"
        fi
        
        if [ "$DEC_SEPARATE_FIELDS" == "ENABLED" ]
        then
            tb_params="--separate-fields-in-dpb ${tb_params}"
        fi

        if [ "$TB_H264_LONG_STREAM" == "ENABLED" ]
        then
            tb_params="-L ${tb_params}"
        fi
        
        if [ "$TB_WHOLE_STREAM" == "ENABLED" ]
        then
            tb_params="-W ${tb_params}"
        fi
        
        if [ "$TB_USE_PEEK_FOR_OUTPUT" == "ENABLED" ]
        then
            tb_params="-Z ${tb_params}"
        fi
        
        if [ "$TB_SKIP_NON_REF_PICS" == "ENABLED" ]
        then
            tb_params="-Q ${tb_params}"
        fi
        
        if [ "$TB_TEST_EXECUTION_TIME" != "-1" ] 
        then
            tb_params="-R ${tb_params}"
        fi
        
        # Disable output reordering for Allegro cases and run them with long stream support enabled
        getCategory "$case_number"
        if [ "$category" == "h264_bp_allegro" ] && [ "$PRODUCT" == "8170" ]
        then
            tb_params="-R -L ${tb_params}"
            
        elif [ "$case_number" == 5090 ] || [ "$case_number" == 5183 ] || [ "$case_number" == 5081 ] || [ "$case_number" == 5168 ] || [ "$case_number" == 5169 ] || [ "$category" == "h264_bp_allegro" ] || [ "$category" == "h264_mp_allegro" ]  || [ "$category" == "h264_hp_allegro" ] && [[ "$PRODUCT" == "8190" || "$PRODUCT" == "9170" || "$PRODUCT" == "9190"  || "$PRODUCT" == "g1" ]]
        then
            tb_params="-L ${tb_params}"
        fi
        
        mvc_cat=`echo $category | grep mvc`
        if [ ! -z "$mvc_cat" ]
        then
            tb_params="-M ${tb_params}"
        fi
        
        fourk_cat=`echo $category | grep _4k`
        if [ ! -z "${fourk_cat}" ]
        then
            tb_params="-L ${tb_params}"
        fi
        
        run_case=1
        if [ "$TB_H264_TOOLS_NOT_SUPPORTED" != "DISABLED" ]
        then
            tools_not_supported=`echo "$TB_H264_TOOLS_NOT_SUPPORTED" | tr " " "_"`
            tools_not_supported=`echo "$tools_not_supported" | tr "," " "`
            for tool_not_supported in $tools_not_supported
            do
                tool_not_supported=`echo "$tool_not_supported" | tr "_" " "`
                if [ -e "${TEST_DATA_HOME_RUN}/case_${case_number}/decoding_tools.trc" ]
                then
                    not_supported_tool_in_strm_tmp=`grep -i "$tool_not_supported" ${TEST_DATA_HOME_RUN}/case_${case_number}/decoding_tools.trc | grep 1`
                    if [ ! -z "$not_supported_tool_in_strm_tmp" ]
                    then
                        run_case=0
                        echo "NOT_SUPPORTED"
                        echo "FLUSH" > "case_${case_number}.done"
                        chmod 666 "case_${case_number}.done"
                        break
                    fi
                fi
            done
        fi

        echo "${tb_params}" > case_${case_number}_tb_params

        tb_params="${tb_params} -N$TB_MAX_NUM_PICS -O$decoder_out $instream"
        
        if [ "$run_case" -eq 1 ]
        then
            # run the case locally
            if [ "$TB_REMOTE_TEST_EXECUTION_IP" == "LOCAL_HOST" ]
            then
                ./rexecruntest.sh "$PWD" "$H264_DECODER" "${tb_params}" \
                        "case_${case_number}${TB_LOG_FILE_SUFFIX}" "$decoder_out" "$case_number" "$execution_time"
                    
            # run the case on remote host using rexec
            else
                # remove the misc from the file path
                #wdir=`echo ${PWD} | awk -F "misc" '{print $2}' 2> /dev/null`
                wdir=`echo ${PWD}`
                rsh "root@${TB_REMOTE_TEST_EXECUTION_IP}" "cd ${wdir} && ./rexecruntest.sh \
                        "$wdir" "$H264_DECODER" '"${tb_params}"' "case_${case_number}${TB_LOG_FILE_SUFFIX}" \
                        "$decoder_out" "$case_number" "$execution_time""
            fi
        fi
        
    else
        echo "NOK ($H264_DECODER missing)"
        return 1
    fi
    return 0
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Runs a MPEG-4/H.263 test case. Uses remote execution (i.e., rexec) if      -- 
#- TB_REMOTE_TEST_EXECUTION_IP != "LOCAL_HOST".                                --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : - Test data stream/post-processor configuration does not exist         --
#-     - Test bench binary does not exist                                     --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
runMpeg4Case()
{
    case_number=$1
    execution_time=$2
    
    # remove the existing decoder outputs so that check script can correctly determine the output format
    rm -f "case_${case_number}${MPEG4_OUT_TILED_SUFFIX}"
    rm -f "case_${case_number}${MPEG4_OUT_RASTER_SCAN_SUFFIX}"
    
    # set the test data dir
    case_dir="${TEST_DATA_HOME_RUN}/case_${case_number}"

    # do nothing if test data doesn't exist (but first chech if the case is conformance case)
    instream=""
    if [ -e "${case_dir}/${MPEG4_STREAM}" ]
    then
        instream="${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/$MPEG4_STREAM"
        
    elif [ -e "${case_dir}/${DIVX_STREAM}" ]
    then
        instream="${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/$DIVX_STREAM"
        
    else
        echo "NOK (Stream missing)"
        return 1
    fi

    # determine the correct decoder output file
    if [ "$DEC_OUTPUT_FORMAT" == "TILED" ] || [ "$DEC_TILED_MODE" == "ENABLED" ]
    then
        if [ "$PRODUCT" != "8170" ] && [ "$PRODUCT" != "g1" ]
        then
            echo "INTERNAL ERROR: TILED not supported for ${PRODUCT}"
        fi
        decoder_out="case_${case_number}${MPEG4_OUT_TILED_SUFFIX}"
        
    else
        decoder_out="case_${case_number}${MPEG4_OUT_RASTER_SCAN_SUFFIX}"
    fi
    
    # run the case
    tb_params=""
    if [ -e "$MPEG4_DECODER" ]
    then
        if [ "$DEC_TILED_MODE" == "ENABLED" ]
        then
            tb_params="-E ${tb_params}"
        fi

        if [ "$DEC_SEPARATE_FIELDS" == "ENABLED" ]
        then
            tb_params="--separate-fields-in-dpb ${tb_params}"
        fi
        
        getCategory $case_number
        
        sorenson=`echo $category | grep sorenson`
        if [ ! -z "$sorenson" ]
        then
            tb_params="-F ${tb_params}"
        fi
        
        if [ "$TB_USE_PEEK_FOR_OUTPUT" == "ENABLED" ]
        then
            tb_params="-Z ${tb_params}"
        fi
        
        if [ "$TB_SKIP_NON_REF_PICS" == "ENABLED" ]
        then
            tb_params="-Q ${tb_params}"
        fi
        
        divx3_cat=`echo $category | grep divx3`
        if [ ! -z "$divx3_cat" ]
        then
            i=`expr index "${case_array[$case_number]}" ';'`
            wxh=${case_array[$case_number]:$i}
            tb_params="-D${wxh} ${tb_params}"
        fi
        
        divx4_cat=`echo $category | grep divx4`
        if [ ! -z "$divx4_cat" ]
        then
            tb_params="-J ${tb_params}"
        fi
        
        if [ "$TB_WHOLE_STREAM" == "ENABLED" ]
        then
            tb_params="-W ${tb_params}"
        fi

        echo "${tb_params}" > case_${case_number}_tb_params
        
        tb_params="${tb_params} -N$TB_MAX_NUM_PICS -O$decoder_out $instream"       
        
        # run the case locally
        if [ "$TB_REMOTE_TEST_EXECUTION_IP" == "LOCAL_HOST" ]
        then
            ./rexecruntest.sh "$PWD" "$MPEG4_DECODER" "${tb_params}" \
                    "case_${case_number}${TB_LOG_FILE_SUFFIX}" "$decoder_out" "$case_number" "$execution_time"
                    
        # run the case on remote host using rexec
        else
            # remove the misc from the file path
            wdir=`echo ${PWD}`
            rsh "root@${TB_REMOTE_TEST_EXECUTION_IP}" "cd ${wdir} && ./rexecruntest.sh \
                    "$wdir" "$MPEG4_DECODER" '"${tb_params}"' "case_${case_number}${TB_LOG_FILE_SUFFIX}" \
                    "$decoder_out" "$case_number" "$execution_time""
        fi
        
    else
        echo "NOK (${MPEG4_DECODER} missing)"
        return 1
    fi
    return 0
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Runs a VC-1 test case. Uses remote execution (i.e., rexec) if              -- 
#- TB_REMOTE_TEST_EXECUTION_IP != "LOCAL_HOST".                                --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : - Test data stream/post-processor configuration does not exist         --
#-     - Test bench binary does not exist                                     --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
runVc1Case()
{
    case_number=$1
    execution_time=$2
    
    # remove the existing decoder outputs so that check script can correctly determine the output format
    rm -f "case_${case_number}${VC1_OUT_TILED_SUFFIX}"
    rm -f "case_${case_number}${VC1_OUT_RASTER_SCAN_SUFFIX}"
    
    # set the test data dir
    case_dir="${TEST_DATA_HOME_RUN}/case_${case_number}"

    # do nothing if test data doesn't exist
    if [ ! -e "${case_dir}/${VC1_STREAM}" ]
    then
        echo "NOK ($case_dir/$VC1_STREAM missing)"
        return 1
        
    else
        #instream="${case_dir}/$VC1_STREAM"
        instream="${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${VC1_STREAM}"
    fi

    # determine the correct decoder output file
    if [ "$DEC_OUTPUT_FORMAT" == "TILED" ] || [ "$DEC_TILED_MODE" == "ENABLED" ]
    then
        if [ "$PRODUCT" != "8170" ] && [ "$PRODUCT" != "g1" ]
        then
            echo "INTERNAL ERROR: TILED not supported for ${PRODUCT}"
        fi
        decoder_out="case_${case_number}${VC1_OUT_TILED_SUFFIX}"
        
    else
        decoder_out="case_${case_number}${VC1_OUT_RASTER_SCAN_SUFFIX}"
    fi

    # run the case
    tb_params=""
    if [ -e "$VC1_DECODER" ]
    then
        if [ "$DEC_TILED_MODE" == "ENABLED" ]
        then
            tb_params="-E ${tb_params}"
        fi
        
        if [ "$TB_VC1_LONG_STREAM" == "ENABLED" ]
        then
            tb_params="-L ${tb_params}"
        fi
        
        if [ "$TB_USE_PEEK_FOR_OUTPUT" == "ENABLED" ]
        then
            tb_params="-Z ${tb_params}"
        fi
        
        if [ "$TB_SKIP_NON_REF_PICS" == "ENABLED" ]
        then
            tb_params="-Q ${tb_params}"
        fi

        if [ "$DEC_SEPARATE_FIELDS" == "ENABLED" ]
        then
            tb_params="--separate-fields-in-dpb ${tb_params}"
        fi

        echo "${tb_params}" > case_${case_number}_tb_params

        tb_params="${tb_params} -N$TB_MAX_NUM_PICS -O$decoder_out $instream"
        
        # run the case locally
        if [ "$TB_REMOTE_TEST_EXECUTION_IP" == "LOCAL_HOST" ]
        then
            ./rexecruntest.sh "$PWD" "$VC1_DECODER" "${tb_params}" \
                    "case_${case_number}${TB_LOG_FILE_SUFFIX}" "$decoder_out" "$case_number" "$execution_time"
                    
        # Run the case on remote host using rexec
        else
            wdir=`echo ${PWD}`
            rsh "root@${TB_REMOTE_TEST_EXECUTION_IP}" "cd ${wdir} && ./rexecruntest.sh \
                    "$wdir" "$VC1_DECODER" '"${tb_params}"' "case_${case_number}${TB_LOG_FILE_SUFFIX}" \
                    "$decoder_out" "$case_number" "$execution_time""
        fi
        
    else
        echo "NOK (${VC1_DECODER} missing)"
        return 1
    fi
    return 0
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Runs a JPEG test case. Uses remote execution (i.e., rexec) if              -- 
#- TB_REMOTE_TEST_EXECUTION_IP != "LOCAL_HOST".                                --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : - Test data stream/post-processor configuration does not exist         --
#-     - Test bench binary does not exist                                     --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
runJpegCase()
{
    case_number=$1
    execution_time=$2
    
    # remove the existing decoder outputs
    rm -f "tn_case_${case_number}.yuv"
    rm -f "case_${case_number}.yuv"

    # set the test data dir
    case_dir="${TEST_DATA_HOME_RUN}/case_${case_number}"
    
    if [ ! -e "${case_dir}/$JPEG_STREAM" ]
    then
        echo "NOK (${case_dir}/${JPEG_STREAM} missing)"
        return 1
        
    else
        # run the case
        tb_params=""
        if [ -e "$JPEG_DECODER" ]
        then
            echo "${tb_params}" > case_${case_number}_tb_params

            tb_params="${tb_params} ${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${JPEG_STREAM}"
            
            # run the case locally
            if [ "$TB_REMOTE_TEST_EXECUTION_IP" == "LOCAL_HOST" ]
            then
                ./rexecruntest.sh "$PWD" "$JPEG_DECODER" "${tb_params}" \
                        "case_${case_number}${TB_LOG_FILE_SUFFIX}" "$JPEG_OUT" "$case_number" "$execution_time"
                    
            # run the case on remote host using rexec
            else
                wdir=`echo ${PWD}`
            	rsh "root@${TB_REMOTE_TEST_EXECUTION_IP}" "cd ${wdir} && ./rexecruntest.sh \
                        "$wdir" "$JPEG_DECODER" '"${tb_params}"' "case_${case_number}${TB_LOG_FILE_SUFFIX}" \
                        "$JPEG_OUT" "$case_number" "$execution_time""
            fi
            
            #mv -f "$JPEG_OUT_THUMB" "tn_case_${case_number}.yuv" >> /dev/null 2>&1
            #mv -f "$JPEG_OUT" "case_${case_number}.yuv" >> /dev/null 2>&1
            
        else
            echo "NOK (${JPEG_DECODER} missing)"
            return 1
        fi
        
        return 0
    fi
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Runs a MPEG-2/MPEG-1 test case. Uses remote execution (i.e., rexec) if     --
#- TB_REMOTE_TEST_EXECUTION_IP != "LOCAL_HOST".                                --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : - Test data stream/post-processor configuration does not exist         --
#-     - Test bench binary does not exist                                     --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
runMpeg2Case()
{
    case_number=$1
    execution_time=$2
    
    # remove the existing decoder outputs so that check script can correctly determine the output format
    rm -f "case_${case_number}${MPEG2_OUT_TILED_SUFFIX}"
    rm -f "case_${case_number}${MPEG2_OUT_RASTER_SCAN_SUFFIX}"
    
    # set the test data dir
    case_dir="${TEST_DATA_HOME_RUN}/case_${case_number}"

    # do nothing if test data doesn't exist (but first chech if the case is conformance case)
    if [ ! -e "${case_dir}/${MPEG2_STREAM}" ]
    then
        if [ ! -e "${case_dir}/${MPEG1_STREAM}" ]
        then
            echo "NOK (${case_dir}/${MPEG2_STREAM} or ${case_dir}/${MPEG1_STREAM} missing)"
            return 1
        else
            #instream="${case_dir}/${MPEG1_STREAM}"
            instream="${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${MPEG1_STREAM}"
        fi
    else
        #instream="${case_dir}/${MPEG2_STREAM}"
        instream="${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${MPEG2_STREAM}"
    fi

    # determine the correct decoder output file
    if [ "$DEC_OUTPUT_FORMAT" == "TILED" ] || [ "$DEC_TILED_MODE" == "ENABLED" ]
    then
        if [ "$PRODUCT" != "8170" ] && [ "$PRODUCT" != "g1" ]
        then
            echo "INTERNAL ERROR: TILED not supported for ${PRODUCT}"
        fi
        decoder_out="case_${case_number}${MPEG2_OUT_TILED_SUFFIX}"
        
    else
        decoder_out="case_${case_number}${MPEG2_OUT_RASTER_SCAN_SUFFIX}"
    fi

    # run the case
    tb_params=""
    if [ -e "$MPEG2_DECODER" ]
    then
        if [ "$DEC_TILED_MODE" == "ENABLED" ]
        then
            tb_params="-E ${tb_params}"
        fi
        
        if [ "$TB_WHOLE_STREAM" == "ENABLED" ]
        then
            tb_params="-W ${tb_params}"
        fi
        
        if [ "$TB_USE_PEEK_FOR_OUTPUT" == "ENABLED" ]
        then
            tb_params="-Z ${tb_params}"
        fi
        
        if [ "$TB_SKIP_NON_REF_PICS" == "ENABLED" ]
        then
            tb_params="-Q ${tb_params}"
        fi

        if [ "$DEC_SEPARATE_FIELDS" == "ENABLED" ]
        then
            tb_params="--separate-fields-in-dpb ${tb_params}"
        fi

        echo "${tb_params}" > case_${case_number}_tb_params

        tb_params="${tb_params} -N$TB_MAX_NUM_PICS -O$decoder_out $instream"
        
        # run the case locally
        if [ "$TB_REMOTE_TEST_EXECUTION_IP" == "LOCAL_HOST" ]
        then
            ./rexecruntest.sh "$PWD" "$MPEG2_DECODER" "${tb_params}" \
                    "case_${case_number}${TB_LOG_FILE_SUFFIX}" "$decoder_out" "$case_number" "$execution_time"
                    
        # run the case on remote host using rexec
        else
            wdir=`echo ${PWD}`
            rsh "root@${TB_REMOTE_TEST_EXECUTION_IP}" "cd ${wdir} && ./rexecruntest.sh \
                    "$wdir" "$MPEG2_DECODER" '"${tb_params}"' "case_${case_number}${TB_LOG_FILE_SUFFIX}" \
                    "$decoder_out" "$case_number" "$execution_time""
        fi
        
    else
        echo "NOK (${MPEG2_DECODER} missing)"
        return 1
    fi
    return 0
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Runs a Real test case. Uses remote execution (i.e., rexec) if              --
#- TB_REMOTE_TEST_EXECUTION_IP != "LOCAL_HOST".                               --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : - Test data stream/post-processor configuration does not exist         --
#-     - Test bench binary does not exist                                     --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
runRealCase()
{
    case_number=$1
    execution_time=$2
    
    rm -f "case_${case_number}${REAL_OUT_TILED_SUFFIX}"
    rm -f "case_${case_number}${REAL_OUT_RASTER_SCAN_SUFFIX}"
    
    case_dir="${TEST_DATA_HOME_RUN}/case_${case_number}"
    if [ -e "${case_dir}/${REAL_STREAM}" ]
    then
        instream="${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${REAL_STREAM}"
        
    else
        echo "NOK (${case_dir}/${REAL_STREAM}"
        return 1
    fi
    
    # determine the correct decoder output file
    tb_params=""
    if [ "$DEC_TILED_MODE" == "ENABLED" ]
    then
        if [ "$PRODUCT" != "g1" ]
        then
            echo "INTERNAL ERROR: TILED not supported for ${PRODUCT}"
        fi
        decoder_out="case_${case_number}${REAL_OUT_TILED_SUFFIX}"
        
    else
        decoder_out="case_${case_number}${REAL_OUT_RASTER_SCAN_SUFFIX}"
    fi
    
    if [ "$DEC_TILED_MODE" == "ENABLED" ]
    then
        tb_params="-E ${tb_params}"
    fi
    
    if [ "$TB_USE_PEEK_FOR_OUTPUT" == "ENABLED" ]
    then
        tb_params="-Z ${tb_params}"
    fi
    
    if [ "$TB_SKIP_NON_REF_PICS" == "ENABLED" ]
    then
        tb_params="-Q ${tb_params}"
    fi

    echo "${tb_params}" > case_${case_number}_tb_params

    tb_params="${tb_params} -N$TB_MAX_NUM_PICS -O$decoder_out $instream"
    
    # run the case
    if [ -e "$REAL_DECODER" ]
    then
        # run the case locally
        if [ "$TB_REMOTE_TEST_EXECUTION_IP" == "LOCAL_HOST" ]
        then
            ./rexecruntest.sh "$PWD" "$REAL_DECODER" "${tb_params}" \
                    "case_${case_number}${TB_LOG_FILE_SUFFIX}" "$decoder_out" "$case_number" "$execution_time"
                    
        # run the case on remote host using rexec
        else
            wdir=`echo ${PWD}`
            rsh "root@${TB_REMOTE_TEST_EXECUTION_IP}" "cd ${wdir} && ./rexecruntest.sh \
                    "$wdir" "$REAL_DECODER" '"${tb_params}"' "case_${case_number}${TB_LOG_FILE_SUFFIX}" \
                    "$decoder_out" "$case_number" "$execution_time""
        fi
        
    else
        echo "NOK (${REAL_DECODER} missing)"
        return 1
    fi
    return 0
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Runs a VP6 test case. Uses remote execution (i.e., rexec) if               --
#- TB_REMOTE_TEST_EXECUTION_IP != "LOCAL_HOST".                               --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : - Test data stream/post-processor configuration does not exist         --
#-     - Test bench binary does not exist                                     --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
runVP6Case()
{
    case_number=$1
    execution_time=$2
    
    rm -f "case_${case_number}${VP6_OUT_TILED_SUFFIX}"
    rm -f "case_${case_number}${VP6_OUT_RASTER_SCAN_SUFFIX}"
    
    case_dir="${TEST_DATA_HOME_RUN}/case_${case_number}"
    if [ -e "${case_dir}/${VP6_STREAM}" ]
    then
        instream="${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${VP6_STREAM}"
        
    else
        echo "NOK (${case_dir}/${REAL_STREAM}"
        return 1
    fi
    
    # determine the correct decoder output file
    tb_params=""
    if [ "$DEC_TILED_MODE" == "ENABLED" ]
    then
        if [ "$PRODUCT" != "g1" ]
        then
            echo "INTERNAL ERROR: TILED not supported for ${PRODUCT}"
        fi
        decoder_out="case_${case_number}${VP6_OUT_TILED_SUFFIX}"
        
    else
        decoder_out="case_${case_number}${VP6_OUT_RASTER_SCAN_SUFFIX}"
    fi
    
    if [ "$DEC_TILED_MODE" == "ENABLED" ]
    then
        tb_params="-E ${tb_params}"
    fi
    
    if [ "$TB_USE_PEEK_FOR_OUTPUT" == "ENABLED" ]
    then
        tb_params="-Z ${tb_params}"
    fi
    
    echo "${tb_params}" > case_${case_number}_tb_params

    if [ "$TB_MAX_NUM_PICS" != 0 ]
    then
        tb_params="-N$TB_MAX_NUM_PICS ${tb_params}"
    fi
    if [ "$TB_MD5SUM" == "ENABLED" ]
    then
        tb_params="${tb_params} -m -O$decoder_out $instream"
    else
        tb_params="${tb_params} -O${decoder_out} ${instream}"
    fi
    
    # run the case
    if [ -e "$VP6_DECODER" ]
    then
        # run the case locally
        if [ "$TB_REMOTE_TEST_EXECUTION_IP" == "LOCAL_HOST" ]
        then
            ./rexecruntest.sh "$PWD" "$VP6_DECODER" "${tb_params}" \
                    "case_${case_number}${TB_LOG_FILE_SUFFIX}" "$decoder_out" "$case_number" "$execution_time"
                    
        # run the case on remote host using rexec
        else
            wdir=`echo ${PWD}`
            rsh "root@${TB_REMOTE_TEST_EXECUTION_IP}" "cd ${wdir} && ./rexecruntest.sh \
                    "$wdir" "$VP6_DECODER" '"${tb_params}"' "case_${case_number}${TB_LOG_FILE_SUFFIX}" \
                    "$decoder_out" "$case_number" "$execution_time""
        fi
        
    else
        echo "NOK (${VP6_DECODER} missing)"
        return 1
    fi
    return 0
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Runs a VP8 test case. Uses remote execution (i.e., rexec) if              --
#- TB_REMOTE_TEST_EXECUTION_IP != "LOCAL_HOST".                               --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : - Test data stream/post-processor configuration does not exist         --
#-     - Test bench binary does not exist                                     --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
runVP8Case()
{
    case_number=$1
    execution_time=$2
    
    rm -f "case_${case_number}${VP8_OUT_TILED_SUFFIX}"
    rm -f "case_${case_number}${VP8_OUT_RASTER_SCAN_SUFFIX}"
    
    case_dir="${TEST_DATA_HOME_RUN}/case_${case_number}"
    if [ -e "${case_dir}/${VP8_STREAM}" ]
    then
        instream="${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${VP8_STREAM}"
        
    elif [ -e "${case_dir}/${VP7_STREAM}" ]
    then
        instream="${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${VP7_STREAM}"
    
    elif [ -e "${case_dir}/${WEBP_STREAM}" ]
    then
        instream="${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${WEBP_STREAM}"
        
    else
        echo "NOK (${case_dir}/${VP8_STREAM}, ${case_dir}/${VP7_STREAM}, or ${case_dir}/${WEBP_STREAM})"
        return 1
    fi
    
    # determine the correct decoder output file
    tb_params=""
    if [ "$DEC_TILED_MODE" == "ENABLED" ] && [ ! -e "${case_dir}/${WEBP_STREAM}" ]
    then
        if [ "$PRODUCT" != "g1" ]
        then
            echo "INTERNAL ERROR: TILED not supported for ${PRODUCT}"
        fi
        decoder_out="case_${case_number}${VP8_OUT_TILED_SUFFIX}"
        tb_params="-E ${tb_params}"
        
    else
        decoder_out="case_${case_number}${VP8_OUT_RASTER_SCAN_SUFFIX}"
    fi
    
    if [ "$TB_USE_PEEK_FOR_OUTPUT" == "ENABLED" ]
    then
        tb_params="-Z ${tb_params}"
    fi
    
    if [ "$VP8_EXT_MEM_ALLOC_ORDER" == "ENABLED" ]
    then
        tb_params="-Xa ${tb_params}"
    fi

    # some special stride tests
    if [ "$TB_VP8_STRIDE" == "ENABLED" ]
    then
        #tb_params="-R ${tb_params}"
        #tb_params="${tb_params}"
        tb_params="-l${VP8_LUMA_STRIDE} ${tb_params}"
        tb_params="-c${VP8_CHROMA_STRIDE} ${tb_params}"
        if [ "$VP8_INTERLEAVED_PIC_BUFFERS" == "ENABLED" ]
        then
            tb_params="-I ${tb_params}"
        fi
    fi

    echo "${tb_params}" > case_${case_number}_tb_params

    tb_params="${tb_params} -N$TB_MAX_NUM_PICS -O$decoder_out $instream"
    
    # run the case
    if [ -e "$VP8_DECODER" ]
    then
        # run the case locally
        if [ "$TB_REMOTE_TEST_EXECUTION_IP" == "LOCAL_HOST" ]
        then
            ./rexecruntest.sh "$PWD" "$VP8_DECODER" "${tb_params}" \
                    "case_${case_number}${TB_LOG_FILE_SUFFIX}" "$decoder_out" "$case_number" "$execution_time"
                    
        # run the case on remote host using rexec
        else
            wdir=`echo ${PWD}`
            rsh "root@${TB_REMOTE_TEST_EXECUTION_IP}" "cd ${wdir} && ./rexecruntest.sh \
                    "$wdir" "$VP8_DECODER" '"${tb_params}"' "case_${case_number}${TB_LOG_FILE_SUFFIX}" \
                    "$decoder_out" "$case_number" "$execution_time""
        fi
        
    else
        echo "NOK (${VP8_DECODER} missing)"
        return 1
    fi
    return 0
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Runs an AVS test case. Uses remote execution (i.e., rexec) if              --
#- TB_REMOTE_TEST_EXECUTION_IP != "LOCAL_HOST".                               --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : - Test data stream/post-processor configuration does not exist         --
#-     - Test bench binary does not exist                                     --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
runAVSCase()
{
    case_number=$1
    execution_time=$2
    
    rm -f "case_${case_number}${AVS_OUT_TILED_SUFFIX}"
    rm -f "case_${case_number}${AVS_OUT_RASTER_SCAN_SUFFIX}"
    
    case_dir="${TEST_DATA_HOME_RUN}/case_${case_number}"
    if [ -e "${case_dir}/${AVS_STREAM}" ]
    then
        instream="${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${AVS_STREAM}"
        
    else
        echo "NOK (${case_dir}/${AVS_STREAM})"
        return 1
    fi
    
    # determine the correct decoder output file
    tb_params=""
    if [ "$DEC_TILED_MODE" == "ENABLED" ]
    then
        if [ "$PRODUCT" != "g1" ]
        then
            echo "INTERNAL ERROR: TILED not supported for ${PRODUCT}"
        fi
        decoder_out="case_${case_number}${AVS_OUT_TILED_SUFFIX}"
        tb_params="-E ${tb_params}"
        
    else
        decoder_out="case_${case_number}${AVS_OUT_RASTER_SCAN_SUFFIX}"
    fi
    
    if [ "$TB_USE_PEEK_FOR_OUTPUT" == "ENABLED" ]
    then
        tb_params="-Z ${tb_params}"
    fi
    
    if [ "$TB_SKIP_NON_REF_PICS" == "ENABLED" ]
    then
        tb_params="-Q ${tb_params}"
    fi

    if [ "$DEC_SEPARATE_FIELDS" == "ENABLED" ]
    then
        tb_params="--separate-fields-in-dpb ${tb_params}"
    fi

    echo "${tb_params}" > case_${case_number}_tb_params
    
    tb_params="${tb_params} -N$TB_MAX_NUM_PICS -O$decoder_out $instream"
    
    # run the case
    if [ -e "$AVS_DECODER" ]
    then
        # run the case locally
        if [ "$TB_REMOTE_TEST_EXECUTION_IP" == "LOCAL_HOST" ]
        then
            ./rexecruntest.sh "$PWD" "$AVS_DECODER" "${tb_params}" \
                    "case_${case_number}${TB_LOG_FILE_SUFFIX}" "$decoder_out" "$case_number" "$execution_time"
                    
        # run the case on remote host using rexec
        else
            wdir=`echo ${PWD}`
            rsh "root@${TB_REMOTE_TEST_EXECUTION_IP}" "cd ${wdir} && ./rexecruntest.sh \
                    "$wdir" "$AVS_DECODER" '"${tb_params}"' "case_${case_number}${TB_LOG_FILE_SUFFIX}" \
                    "$decoder_out" "$case_number" "$execution_time""
        fi
        
    else
        echo "NOK (${AVS_DECODER} missing)"
        return 1
    fi
    return 0
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Runs a post-processor test case including pipelined cases. Uses remote     -- 
#- execution (i.e., rexec) if  TB_REMOTE_TEST_EXECUTION_IP != "LOCAL_HOST".    --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : - Test data stream/post-processor configuration does not exist         --
#-     - Test bench binary does not exist                                     --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
runPpCase()
{
    pipelined_decoder=$1
    case_number=$2
    execution_time=$3
    
    # remove the existing pp outputs
    rm -f "case_${case_number}.yuv"
    rm -f "case_${case_number}.rgb"
    
    # set the test data dir
    case_dir="${TEST_DATA_HOME_RUN}/case_${case_number}"
    
    # do nothing if pp config missing
    if [ ! -e "${case_dir}/${PP_CFG}" ]
    then
        echo "NOK (${case_dir}/${PP_CFG} missing)"
        return 1
    else
        test_bench_args=""
        # set up external mode pp
        if [ "$pipelined_decoder" == "none" ]
        then
            getCategory "$case_number"
            test_bench_binary="$PP_EXTERNAL"
            pp_cfg="${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${PP_CFG}"
            if [ "$DEC_TILED_MODE" == "ENABLED" ]
            then
                deinterlace_case=`echo $category | grep deint`
                if [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "3" && "$g_hw_minor" -ge "4" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge 4 ]]
                then
                    test_bench_args="-E ${test_bench_args}"
                    # Set the correct input structre for HW; we don't need to do this for the model (reads same pp.cfg)
                    deinterlace_case=`echo $category |grep deint`
                    if [ ! -z $deinterlace_case ]
                    then
                        cat "$pp_cfg" |tr -d '\r\n' |sed "s/Input[ \t]*{/Input { Struct = 2;/" > "case_${case_number}_pp.cfg"
                        pp_cfg="case_${case_number}_pp.cfg"
                    fi
                # do not use -E if deint pp external case
                else
                    if [ -z $deinterlace_case ]
                    then
                       test_bench_args="-E ${test_bench_args}"
                    fi
                fi
            fi
            
            if eval "echo ${category} | grep 'pp_422order' >/dev/null"
            then
                cp ${case_dir}/input.yuv .
            fi

            echo "${test_bench_args}" > case_${case_number}_tb_params

            test_bench_args="${test_bench_args} ${pp_cfg}"
            
        # set up h264 + pp pipeline
        elif [ "$pipelined_decoder" == "h264" ]
        then
            # do nothing if test data doesn't exist
            if [ ! -e "${case_dir}/${H264_STREAM}" ]
            then
                echo "NOK (${case_dir}/${H264_STREAM} missing)"
                return 1
            
            else
                test_bench_binary="$PP_H264_PIPELINE"
            fi
            
            if [ "$TB_H264_LONG_STREAM" == "ENABLED" ]
            then
                test_bench_args="-L ${test_bench_args}"
            fi
            
            if [ "$TB_WHOLE_STREAM" == "ENABLED" ]
            then
                test_bench_args="-W ${test_bench_args}"
            fi

            if [ "$TB_SKIP_NON_REF_PICS" == "ENABLED" ]
            then
                test_bench_args="-Q ${test_bench_args}"
            fi
            
            if [ "$DEC_TILED_MODE" == "ENABLED" ]
            then
                test_bench_args="-E ${test_bench_args}"
            fi

            if [ "$DEC_SEPARATE_FIELDS" == "ENABLED" ]
            then
                test_bench_args="--separate-fields-in-dpb ${test_bench_args}"
            fi
            
            getCategory "$case_number"
            fourk_cat=`echo $category | grep _4k`
            if [ ! -z "${fourk_cat}" ]
            then
                test_bench_args="-L ${test_bench_args}"
            fi
            
            echo "${test_bench_args}" > case_${case_number}_tb_params

            test_bench_args="${test_bench_args} -N$TB_MAX_NUM_PICS ${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${H264_STREAM} ${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${PP_CFG}"
            
        # set up mpeg-4 + pp pipeline            
        elif [ "$pipelined_decoder" == "mpeg4" ]
        then
            instream=""
            if [ -e "${case_dir}/${MPEG4_STREAM}" ]
            then
                instream="$MPEG4_STREAM"
            elif [ -e "${case_dir}/${DIVX_STREAM}" ]
            then
                instream="$DIVX_STREAM"
            else
                echo "NOK (Stream missing)"
                return 1
            fi
            
            test_bench_binary="$PP_MPEG4_PIPELINE"
            
            if [ "$DEC_TILED_MODE" == "ENABLED" ]
            then
                test_bench_args="-E ${test_bench_args}"
            fi

            if [ "$DEC_SEPARATE_FIELDS" == "ENABLED" ]
            then
                test_bench_args="--separate-fields-in-dpb ${test_bench_args}"
            fi
            
            getCategory $case_number
            # sorenson category
            sorenson=`echo $category | grep sorenson`
            if [ ! -z "$sorenson" ]
            then
                test_bench_args="-F ${test_bench_args}"
            fi
        
            # divx3 category
            divx3_cat=`echo $category | grep divx3`
            if [ ! -z "$divx3_cat" ]
            then
                i=`expr index "${case_array[$case_number]}" ';'`
                wxh=${case_array[$case_number]:$i}
                test_bench_args="-D${wxh} ${test_bench_args}"
            fi
        
            # divx4 category
            divx4_cat=`echo $category | grep divx4`
            if [ ! -z "$divx4_cat" ]
            then
                test_bench_args="-J ${test_bench_args}"
            fi
            
            if [ "$TB_WHOLE_STREAM" == "ENABLED" ]
            then
                test_bench_args="-W ${test_bench_args}"
            fi

            echo "${test_bench_args}" > case_${case_number}_tb_params

            test_bench_args="${test_bench_args} -N$TB_MAX_NUM_PICS ${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${instream} ${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${PP_CFG}"
        
        # set up jpeg + pp pipeline
        elif [ "$pipelined_decoder" == "jpeg" ]
        then
            if [ ! -e "${case_dir}/$JPEG_STREAM" ]
            then
                echo "NOK (${case_dir}/${JPEG_STREAM} missing)"
                return 1
                
            else  
                test_bench_binary="$PP_JPEG_PIPELINE"
            fi

            echo "${test_bench_args}" > case_${case_number}_tb_params

            test_bench_args="${test_bench_args} ${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${JPEG_STREAM} ${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${PP_CFG}"
            
        # set up vc-1 + pp pipeline    
        elif [ "$pipelined_decoder" == "vc1" ]
        then
            if [ ! -e ${case_dir}/${VC1_STREAM} ]
            then
                echo "NOK (${case_dir}/${VC1_STREAM} missing)"
                return 1
                
            else  
                test_bench_binary="$PP_VC1_PIPELINE"
            fi
            
            if [ "$DEC_TILED_MODE" == "ENABLED" ]
            then
                test_bench_args="-E ${test_bench_args}"
            fi

            if [ "$DEC_SEPARATE_FIELDS" == "ENABLED" ]
            then
                test_bench_args="--separate-fields-in-dpb ${test_bench_args}"
            fi
            
            if [ "$TB_VC1_LONG_STREAM" == "ENABLED" ]
            then
                test_bench_args="-L ${test_bench_args}"
            fi
           
            echo "${test_bench_args}" > case_${case_number}_tb_params

            test_bench_args="${test_bench_args} -N$TB_MAX_NUM_PICS ${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${VC1_STREAM} ${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${PP_CFG}"
        
        # Set up mpeg2 + pp pipeline    
        elif [ "$pipelined_decoder" == "mpeg2" ]
        then
            if [ ! -e ${case_dir}/${MPEG2_STREAM} ]
            then
                echo "NOK (${case_dir}/${MPEG2_STREAM} missing)"
                return 1
            else  
                test_bench_binary="$PP_MPEG2_PIPELINE"
            fi
            
            if [ "$TB_WHOLE_STREAM" == "ENABLED" ]
            then
                test_bench_args="-W ${test_bench_args}"
            fi
            
            if [ "$DEC_TILED_MODE" == "ENABLED" ]
            then
                test_bench_args="-E ${test_bench_args}"
            fi

            if [ "$DEC_SEPARATE_FIELDS" == "ENABLED" ]
            then
                test_bench_args="--separate-fields-in-dpb ${test_bench_args}"
            fi

            echo "${test_bench_args}" > case_${case_number}_tb_params

            test_bench_args="${test_bench_args} -N$TB_MAX_NUM_PICS ${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${MPEG2_STREAM} ${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${PP_CFG}"
        
        # Set up real + pp pipeline    
        elif [ "$pipelined_decoder" == "real" ]
        then
            if [ ! -e ${case_dir}/${REAL_STREAM} ]
            then
                echo "NOK (${case_dir}/${REAL_STREAM} missing)"
                return 1
                
            else  
                test_bench_binary="$PP_REAL_PIPELINE"
            fi
            
            if [ "$DEC_TILED_MODE" == "ENABLED" ]
            then
                test_bench_args="-E ${test_bench_args}"
            fi

            echo "${test_bench_args}" > case_${case_number}_tb_params

            test_bench_args="${test_bench_args} -N$TB_MAX_NUM_PICS ${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${REAL_STREAM} ${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${PP_CFG}"
        
        # Set up vp6 + pp pipeline    
        elif [ "$pipelined_decoder" == "vp6" ]
        then
            if [ ! -e ${case_dir}/${VP6_STREAM} ]
            then
                echo "NOK (${case_dir}/${VP6_STREAM} missing)"
                return 1
                
            else  
                test_bench_binary="$PP_VP6_PIPELINE"
            fi
            
            if [ "$DEC_TILED_MODE" == "ENABLED" ]
            then
                test_bench_args="-E ${test_bench_args}"
            fi

            echo "${test_bench_args}" > case_${case_number}_tb_params

            if [ "$TB_MAX_NUM_PICS" != 0 ]
            then
                test_bench_args="${test_bench_args} -N$TB_MAX_NUM_PICS ${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${VP6_STREAM} ${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${PP_CFG}"
            else
                test_bench_args="${test_bench_args} ${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${VP6_STREAM} ${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${PP_CFG}"
            fi
        
        # Set up vp8 + pp pipeline    
        elif [ "$pipelined_decoder" == "vp8" ]
        then
            instream=""
            if [ -e "${case_dir}/${VP8_STREAM}" ]
            then
                instream="${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${VP8_STREAM}"
        
            elif [ -e "${case_dir}/${VP7_STREAM}" ]
            then
                instream="${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${VP7_STREAM}"
                
            elif [ -e "${case_dir}/${WEBP_STREAM}" ]
            then
                instream="${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${WEBP_STREAM}"
        
            else
                echo "NOK (${case_dir}/${VP8_STREAM}, ${case_dir}/${VP7_STREAM}, or ${case_dir}/${WEBP_STREAM})"
                return 1
            fi
            
            test_bench_binary="$PP_VP8_PIPELINE"
            
            if [ "$DEC_TILED_MODE" == "ENABLED" ] && [ ! -e "${case_dir}/${WEBP_STREAM}" ] 
            then
                test_bench_args="-E ${test_bench_args}"
            fi
            
            if [ "$VP8_EXT_MEM_ALLOC_ORDER" == "ENABLED" ]
            then
                test_bench_args="-Xa ${tb_params}"
            fi

            echo "${test_bench_args}" > case_${case_number}_tb_params

            test_bench_args="${test_bench_args} -N$TB_MAX_NUM_PICS ${instream} ${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${PP_CFG}"
            
        # Set up avs + pp pipeline    
        elif [ "$pipelined_decoder" == "avs" ]
        then
            instream=""
            if [ -e "${case_dir}/${AVS_STREAM}" ]
            then
                instream="${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${AVS_STREAM}"
        
            else
                echo "NOK (${case_dir}/${AVS_STREAM})"
                return 1
            fi
            
            test_bench_binary="$PP_AVS_PIPELINE"
            
            if [ "$DEC_TILED_MODE" == "ENABLED" ]
            then
                test_bench_args="-E ${test_bench_args}"
            fi

            if [ "$DEC_SEPARATE_FIELDS" == "ENABLED" ]
            then
                test_bench_args="--separate-fields-in-dpb ${test_bench_args}"
            fi

            echo "${test_bench_args}" > case_${case_number}_tb_params

            test_bench_args="${test_bench_args} -N$TB_MAX_NUM_PICS ${instream} ${TEST_DATA_HOME_ROOT_REXEC}/case_${case_number}/${PP_CFG}"
            
        else
            echo "Internal error in runPpCase()"
            return 1
        fi
        
        # run the case
        if [ -e "$test_bench_binary" ]
        then
            if [ -e ${case_dir}/${PP_OUT_RGB} ]
            then
                monitor="$PP_OUT_RGB"
                
            else
                monitor="$PP_OUT_YUV"
            fi
            
            # run the case locally
            if [ "$TB_REMOTE_TEST_EXECUTION_IP" == "LOCAL_HOST" ]
            then
                ./rexecruntest.sh "$PWD" "$test_bench_binary" "${test_bench_args}" \
                        "case_${case_number}${TB_LOG_FILE_SUFFIX}" "$monitor" "$case_number" "$execution_time"
                    
            # run the case on remote host using rexec
            else
                wdir=`echo ${PWD}`
            	rsh "root@${TB_REMOTE_TEST_EXECUTION_IP}" "cd ${wdir} && ./rexecruntest.sh \
                        "$wdir" "$test_bench_binary" '"${test_bench_args}"' "case_${case_number}${TB_LOG_FILE_SUFFIX}" \
                        "$monitor" "$case_number" "$execution_time""
            fi
            
            #mv -f "$PP_OUT_YUV" "case_${case_number}.yuv" >> /dev/null 2>&1
            #mv -f "$PP_OUT_RGB" "case_${case_number}.rgb" >> /dev/null 2>&1
            
        else
            echo "NOK (${test_bench_binary} missing)"
            return 1
        fi
        
    return 0
    fi
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Runs a test case by selecting correct decoder/post-processor for the test  --
#- case.                                                                      --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : - Test data stream/post-processor configuration does not exist         --
#-     - Test bench binary does not exist                                     --
#-     - Cannot determine correct decoder/post-processor for the test case    --
#- 0 : otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
runCase()
{
    case_number="$1"
    execution_time="$2"
    use_previous_cfg="$3"
    
    ret_val=1
    
    getComponent "$case_number"
    if [ $? == 1 ]
    then
        echo "DONE" > "case_${case_number}.done"
        chmod 666 "case_${case_number}.done"
        echo "0" > "case_${case_number}.time"
        chmod 666 "case_${case_number}.time"
        return 1
    fi 
    
    # if enabled, randomize all possible paramters
    if [ "$TB_RND_ALL" == "ENABLED" ] && [ "$use_previous_cfg" == "0" ]
    then
        randomizeAll "$case_number"
    fi
    
    # if enabled, randomize the configuration
    if [ "$TB_RND_CFG" == "ENABLED" ] && [ "$use_previous_cfg" == "0" ]
    then
        randomizeCfg "$case_number"
    fi

    tb_vp8_stride_current="$TB_VP8_STRIDE"
    dec_memory_allocation_current="$DEC_MEMORY_ALLOCATION"
    vp8_memory_allocation_order_current="$VP8_EXT_MEM_ALLOC_ORDER"
    
    if [ "$use_previous_cfg" == "0" ]
    then
        getComponent "$case_number"
        # Enable jpeg input buffering if the stream is too long
    	if [ "$decoder" == "jpeg" ] && [ "$JPEG_INPUT_BUFFER_SIZE" == 0 ]
	then
            stream_size=0
	    ls_info=`ls -l ${TEST_DATA_HOME_RUN}/case_${case_number}/${JPEG_STREAM}`
            let element_number=0
            for i in $ls_info
            do
                let 'element_number=element_number+1'
        	    if [ $element_number == 5 ]
        	    then
        	        stream_size=$i
        	    fi
	        done
        	# Actually, randomize the input buffer size
	        if [ "$stream_size" -gt 16777215 ]
        	then
        	    # the valid values for input buffer size are [5120, 16776960]
        	    # the size must be multipe of 256
        	    # however, the maximum input buffer size is limited to maximum stream size
        	    let 'tmp=stream_size-5120'
        	    if [ "$tmp" -ge 0 ]
        	    then
        	        # chunks is the number of possible increments for input buffer size
        	        let 'chunks=tmp/256'
        	        # randomize the number of chunks
        	        random_value=$RANDOM; let "random_value %= (chunks+1)"
        	        # number of chunks*256 are added to 5120 to define the input buffer size
        	        let JPEG_INPUT_BUFFER_SIZE=(random_value+20)*256
        	        if [ "$JPEG_INPUT_BUFFER_SIZE" -gt 16776960 ]
        	        then
        	            JPEG_INPUT_BUFFER_SIZE=16776960
        	        fi
        	    fi
        	fi
	    fi
    
        getCategory "$case_number"
        vp7_category=`echo $category | grep vp7`
        vp8_category=`echo $category | grep vp8`
        webp_category=`echo $category | grep webp`
        if [ "$PRODUCT" != "g1" ] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "2" && "$g_hw_minor" -lt "70" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -le "1" ]]
        then
            TB_VP8_STRIDE="DISABLED"
        fi
        if [ ! -z "$vp8_category" ]
        then
            if [ "$PRODUCT" != "g1" ] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "2" && "$g_hw_minor" -lt "70" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -le "1" ]]
            then
                DEC_MEMORY_ALLOCATION="INTERNAL"
            fi
        fi
        if [ ! -z "$vp7_category" ]
        then
            DEC_MEMORY_ALLOCATION="INTERNAL"
            TB_VP8_STRIDE="DISABLED"
        fi
        #ilaced_content=""
        #if [ -e "${TEST_DATA_HOME_RUN}/case_${case_number}/decoding_tools.trc" ]
        #then
        #    ilaced_content=`grep -i "sequence type" ${TEST_DATA_HOME_RUN}/case_${case_number}/decoding_tools.trc |grep -i "interlace" |grep 1`
        #else
        #    if [ -e "${TEST_DATA_HOME_RUN}/case_${case_number}/picture_ctrl_dec.trc" ]
        #    then
        #        avs_category=`echo $category |grep avs`
        #        if [ ! -z "$avs_category" ]
        #        then
        #            ilaced_content=`grep -i "interlace" ${TEST_DATA_HOME_RUN}/case_${case_number}/picture_ctrl_dec.trc |awk '{print $1}' |grep -m 1 1`
        #        fi
        #    else
        #        echo "Ilaced information missing!"
        #    fi
        #fi
        #if [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "1" && "$g_hw_minor" -ge "88" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge 2 ]]
        #then
        #    random_value=$RANDOM; let "random_value %= 2"
        #    if [ "$random_value" == 1 ] || [ ! -z "$ilaced_content" ]
        #    then
        #        DEC_TILED_MODE="DISABLED"
        #    else
        #        DEC_TILED_MODE="ENABLED"
        #    fi
        #fi
        #if [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "3" && "$g_hw_minor" -ge "4" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge 4 ]]
        #then
        #    random_value=$RANDOM; let "random_value %= 2"
        #    if [ "$random_value" == 1 ] && [ ! -z "$ilaced_content" ]
        #    then
        #        DEC_SEPARATE_FIELDS="ENABLED"
        #    elif [ ! -z "$ilaced_content" ] && [ "$DEC_TILED_MODE" == "ENABLED" ]
        #    then
        #        DEC_SEPARATE_FIELDS="ENABLED"
        #    else
        #        DEC_SEPARATE_FIELDS="DISABLED"
        #    fi
        #fi
        if [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "1" && "$g_hw_minor" -ge "88" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge 2 ]]
        then
            random_value=$RANDOM; let "random_value %= 2"
            if [ "$random_value" == 1 ]
            then
                DEC_TILED_MODE="DISABLED"
            else
                DEC_TILED_MODE="ENABLED"
            fi
        fi
        if [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "3" && "$g_hw_minor" -ge "4" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge 4 ]]
        then
            random_value=$RANDOM; let "random_value %= 2"
            if [ "$random_value" == 1 ]
            then
                DEC_SEPARATE_FIELDS="DISABLED"
            else
                DEC_SEPARATE_FIELDS="ENABLED"
            fi
        fi
        if [ "$TB_VP8_STRIDE" == "ENABLED" ] && [ "$post_processor" == "none" ] && [[ ! -z "$vp8_category" || ! -z "$webp_category" ]]
        then
            DEC_TILED_MODE="DISABLED"
            DEC_SEPARATE_FIELDS="DISABLED"
            # strides are not set -> randomize
            if [ "$TB_VP8_RND_STRIDE" == "ENABLED" ]
            then
                randomizeStride "$case_number"
            fi
        fi
        if [ "$DEC_MEMORY_ALLOCATION" == "EXTERNAL" ] && [ -z "$webp_category" ]
        then
            if [[ "$PRODUCT" == "g1" && "$g_hw_major" -eq "2" && "$g_hw_minor" -ge "70" ]] || [[ "$PRODUCT" == "g1" && "$g_hw_major" -ge "3" ]]
            then
                random_value=$RANDOM; let "random_value %= 2"
                if [ "$random_value" == 1 ]
                then
                    VP8_EXT_MEM_ALLOC_ORDER="ENABLED"
                else
                    VP8_EXT_MEM_ALLOC_ORDER="DISABLED"
                fi
                
            fi
        fi
    fi
    
    # if enabled, read the configuration from previous execution
    if [ "$use_previous_cfg" == "1" ]
    then
        #configuration=`grep "\<case ${case_number}\>" report_repeat.csv | awk -F\; '{print $ 11}' | tr ' ' '\n'`
        configuration=`grep "\<case ${case_number}\>" report_repeat.csv`
        let 'cntr=0'
        while [ "$cntr" -lt 10 ]
        do
            i=`expr index "$configuration" ";"`
            configuration=${configuration:$i}
            let 'cntr+=1'
        done
        i=`expr index "$configuration" ";"`
        let 'i-=1'
        configuration=${configuration:0:$i}
        configuration=`echo $configuration |  tr ' ' '\n'`
        
        if [ ! -z "$configuration" ]
        then

            configuration_items="\
                DEC_OUTPUT_PICTURE_ENDIAN \
                DEC_BUS_BURST_LENGTH \
                DEC_ASIC_SERVICE_PRIORITY \
                DEC_OUTPUT_FORMAT \
                DEC_TILED_MODE \
                DEC_SEPARATE_FIELDS \
                DEC_LATENCY_COMPENSATION \
                DEC_CLOCK_GATING \
                DEC_DATA_DISCARD \
                DEC_MEMORY_ALLOCATION \
                DEC_RLC_MODE_FORCED \
                DEC_REFERENCE_BUFFER_LATENCY \
                DEC_REFERENCE_BUFFER_NONSEQ \
                DEC_REFERENCE_BUFFER_SEQ \
                JPEG_INPUT_BUFFER_SIZE \
                JPEG_MCUS_SLICE \
                VP8_LUMA_STRIDE \
                VP8_CHROMA_STRIDE \
                VP8_INTERLEAVED_PIC_BUFFERS \
                VP8_EXT_MEM_ALLOC_ORDER \
                PP_BUS_BURST_LENGTH \
                PP_CLOCK_GATING \
                PP_DATA_DISCARD \
                PP_INPUT_PICTURE_ENDIAN \
                PP_OUTPUT_PICTURE_ENDIAN \
                PP_WORD_SWAP \
                PP_WORD_SWAP16 \
                PP_MULTI_BUFFER \
                TB_PACKET_BY_PACKET \
                TB_WHOLE_STREAM \
                TB_NAL_UNIT_STREAM \
                TB_VC1_UD \
                TB_VP8_STRIDE \
                TB_USE_PEEK_FOR_OUTPUT \
                TB_SKIP_NON_REF_PICS"

            for configuration_item in $configuration_items
            do
                #value=`echo "$configuration" | grep "$configuration_item" | awk -F = '{print $2}'`
                value=`echo "$configuration" | grep "\<${configuration_item}\>"`
                i=`expr index "$value" "="`
                value=${value:$i}
                if [ "$configuration_item" == "DEC_OUTPUT_PICTURE_ENDIAN" ]
                then
                    export DEC_OUTPUT_PICTURE_ENDIAN="$value"
                    
                elif [ "$configuration_item" == "DEC_BUS_BURST_LENGTH" ]
                then
                    export DEC_BUS_BURST_LENGTH="$value"
                    
                elif [ "$configuration_item" == "DEC_ASIC_SERVICE_PRIORITY" ]
                then
                    export DEC_ASIC_SERVICE_PRIORITY="$value"
                    
                elif [ "$configuration_item" == "DEC_OUTPUT_FORMAT" ]
                then
                    export DEC_OUTPUT_FORMAT="$value"
                
                elif [ "$configuration_item" == "DEC_TILED_MODE" ]
                then
                    export DEC_TILED_MODE="$value"
                
                elif [ "$configuration_item" == "DEC_SEPARATE_FIELDS" ]
                then
                    export DEC_SEPARATE_FIELDS="$value"
                    
                elif [ "$configuration_item" == "DEC_LATENCY_COMPENSATION" ]
                then
                    export DEC_LATENCY_COMPENSATION="$value"
                    
                elif [ "$configuration_item" == "DEC_CLOCK_GATING" ]
                then
                    export DEC_CLOCK_GATING="$value"
                    
                elif [ "$configuration_item" == "DEC_DATA_DISCARD" ]
                then
                    export DEC_DATA_DISCARD="$value"
                    
                elif [ "$configuration_item" == "DEC_MEMORY_ALLOCATION" ]
                then
                    export DEC_MEMORY_ALLOCATION="$value"
                    
                elif [ "$configuration_item" == "DEC_RLC_MODE_FORCED" ]
                then
                    export DEC_RLC_MODE_FORCED="$value"
                    
                elif [ "$configuration_item" == "DEC_REFERENCE_BUFFER_LATENCY" ]
                then
                    export DEC_REFERENCE_BUFFER_LATENCY="$value"
                    
                elif [ "$configuration_item" == "DEC_REFERENCE_BUFFER_NONSEQ" ]
                then
                    export DEC_REFERENCE_BUFFER_NONSEQ="$value"
                    
                elif [ "$configuration_item" == "DEC_REFERENCE_BUFFER_SEQ" ]
                then
                    export DEC_REFERENCE_BUFFER_SEQ="$value"
                    
                elif [ "$configuration_item" == "JPEG_INPUT_BUFFER_SIZE" ]
                then
                    export JPEG_INPUT_BUFFER_SIZE="$value"
                    
                elif [ "$configuration_item" == "JPEG_MCUS_SLICE" ]
                then
                    export JPEG_MCUS_SLICE="$value"

                elif [ "$configuration_item" == "VP8_LUMA_STRIDE" ]
                then
                    export VP8_LUMA_STRIDE="$value"

                elif [ "$configuration_item" == "VP8_CHROMA_STRIDE" ]
                then
                    export VP8_CHROMA_STRIDE="$value"

                elif [ "$configuration_item" == "VP8_INTERLEAVED_PIC_BUFFERS" ]
                then
                    export VP8_INTERLEAVED_PIC_BUFFERS="$value"

                elif [ "$configuration_item" == "VP8_EXT_MEM_ALLOC_ORDER" ]
                then
                    export VP8_EXT_MEM_ALLOC_ORDER="$value"
                    
                elif [ "$configuration_item" == "PP_BUS_BURST_LENGTH" ]
                then
                    export PP_BUS_BURST_LENGTH="$value"
                    
                elif [ "$configuration_item" == "PP_CLOCK_GATING" ]
                then
                    export PP_CLOCK_GATING="$value"
                    
                elif [ "$configuration_item" == "PP_DATA_DISCARD" ]
                then
                    export PP_DATA_DISCARD="$value"
                    
                elif [ "$configuration_item" == "PP_INPUT_PICTURE_ENDIAN" ]
                then
                    export PP_INPUT_PICTURE_ENDIAN="$value"
                    
                elif [ "$configuration_item" == "PP_OUTPUT_PICTURE_ENDIAN" ]
                then
                    export PP_OUTPUT_PICTURE_ENDIAN="$value"
                    
                elif [ "$configuration_item" == "PP_WORD_SWAP" ]
                then
                    export PP_WORD_SWAP="$value"
                    
                elif [ "$configuration_item" == "PP_WORD_SWAP16" ]
                then
                    export PP_WORD_SWAP16="$value"
                    
                elif [ "$configuration_item" == "PP_MULTI_BUFFER" ]
                then
                    export PP_MULTI_BUFFER="$value"
                    
                elif [ "$configuration_item" == "TB_PACKET_BY_PACKET" ]
                then
                    export TB_PACKET_BY_PACKET="$value"
                    
                elif [ "$configuration_item" == "TB_WHOLE_STREAM" ]
                then
                    export TB_WHOLE_STREAM="$value"
                    
                elif [ "$configuration_item" == "TB_NAL_UNIT_STREAM" ]
                then
                    export TB_NAL_UNIT_STREAM="$value"
                    
                elif [ "$configuration_item" == "TB_VC1_UD" ]
                then
                    export TB_VC1_UD="$value"
                    
                elif [ "$configuration_item" == "TB_USE_PEEK_FOR_OUTPUT" ]
                then
                    export TB_USE_PEEK_FOR_OUTPUT="$value"
                
                elif [ "$configuration_item" == "TB_SKIP_NON_REF_PICS" ]
                then
                    export TB_SKIP_NON_REF_PICS="$value"
                
                elif [ "$configuration_item" == "TB_VP8_STRIDE" ]
                then
                    export TB_VP8_STRIDE="$value"
                fi
            done
        
        else
            echo "Can't find configuration"
        fi
    fi

    writeConfiguration "$case_number" ""    

    printCase "$case_number" "$decoder" "$post_processor"

    if [ "$post_processor" == "pp" ]
    then
        runPpCase "$decoder" "$case_number" "$execution_time"
        ret_val=$?
    fi

    if [ "$decoder" == "h264" ] && [ "$post_processor" != "pp" ]
    then
        runH264Case "$case_number" "$execution_time"
        ret_val=$?
    fi

    if [ "$decoder" == "mpeg4" ] && [ "$post_processor" != "pp" ]
    then
        runMpeg4Case "$case_number" "$execution_time"
        ret_val=$?
    fi

    if [ "$decoder" == "vc1" ] && [ "$post_processor" != "pp" ]
    then
        runVc1Case "$case_number" "$execution_time"
        ret_val=$?
    fi

    if [ "$decoder" == "jpeg" ] && [ "$post_processor" != "pp" ]
    then
        runJpegCase "$case_number" "$execution_time"
        ret_val=$?
    fi
    
    if [ "$decoder" == "mpeg2" ] && [ "$post_processor" != "pp" ]
    then
        runMpeg2Case "$case_number" "$execution_time"
        ret_val=$?
    fi
    
    if [ "$decoder" == "real" ] && [ "$post_processor" != "pp" ]
    then
        runRealCase "$case_number" "$execution_time"
        ret_val=$?
    fi
    
    if [ "$decoder" == "vp6" ] && [ "$post_processor" != "pp" ]
    then
        runVP6Case "$case_number" "$execution_time"
        ret_val=$?
    fi
    
    if [ "$decoder" == "vp8" ] && [ "$post_processor" != "pp" ]
    then
        runVP8Case "$case_number" "$execution_time"
        ret_val=$?
    fi
    
    if [ "$decoder" == "avs" ] && [ "$post_processor" != "pp" ]
    then
        runAVSCase "$case_number" "$execution_time"
        ret_val=$?
    fi
    
    if [ $ret_val == 1 ]
    then
        echo "DONE" > "case_${case_number}.done"
        chmod 666 "case_${case_number}.done"
        echo "0" > "case_${case_number}.time"
        chmod 666 "case_${case_number}.time"
    fi
    ## indicate whether the component has crashed
    #crash=`ls core* 2> /dev/null`
    #if [ ! -z "$crash" ]
    #then
    #    mv -f core* "case_${case_number}.crash"
    #fi
    #
    #if [ -e "$HW_PERFORMANCE_LOG" ]
    #then
    #    mv -f "$HW_PERFORMANCE_LOG" "case_${case_number}_${HW_PERFORMANCE_LOG}"
    #fi
    
    # print out the configuration
    echo "Configuration: \
HW_LANGUAGE=${HW_LANGUAGE} \
HW_BUS=${HW_BUS} \
HW_DATA_BUS_WIDTH=${HW_DATA_BUS_WIDTH} \
TEST_ENV_DATA_BUS_WIDTH=${TEST_ENV_DATA_BUS_WIDTH} \
DEC_OUTPUT_PICTURE_ENDIAN=${DEC_OUTPUT_PICTURE_ENDIAN} \
DEC_BUS_BURST_LENGTH=${DEC_BUS_BURST_LENGTH} \
DEC_ASIC_SERVICE_PRIORITY=${DEC_ASIC_SERVICE_PRIORITY} \
DEC_OUTPUT_FORMAT=${DEC_OUTPUT_FORMAT} \
DEC_TILED_MODE=${DEC_TILED_MODE} \
DEC_SEPARATE_FIELDS=${DEC_SEPARATE_FIELDS} \
DEC_LATENCY_COMPENSATION=${DEC_LATENCY_COMPENSATION} \
DEC_CLOCK_GATING=${DEC_CLOCK_GATING} \
DEC_DATA_DISCARD=${DEC_DATA_DISCARD} \
DEC_MEMORY_ALLOCATION=${DEC_MEMORY_ALLOCATION} \
DEC_RLC_MODE_FORCED=${DEC_RLC_MODE_FORCED} \
DEC_MAX_OUTPUT=${DEC_MAX_OUTPUT} \
DEC_STANDARD_COMPLIANCE=${DEC_STANDARD_COMPLIANCE} \
DEC_REFERENCE_BUFFER=${DEC_REFERENCE_BUFFER} \
DEC_REFERENCE_BUFFER_INTERLACED=${DEC_REFERENCE_BUFFER_INTERLACED} \
DEC_REFERENCE_BUFFER_TOP_BOT_SUM=${DEC_REFERENCE_BUFFER_TOP_BOT_SUM} \
DEC_REFERENCE_BUFFER_EVAL_MODE=${DEC_REFERENCE_BUFFER_EVAL_MODE} \
DEC_REFERENCE_BUFFER_CHECKPOINT=${DEC_REFERENCE_BUFFER_CHECKPOINT} \
DEC_REFERENCE_BUFFER_OFFSET=${DEC_REFERENCE_BUFFER_OFFSET} \
DEC_REFERENCE_BUFFER_LATENCY=${DEC_REFERENCE_BUFFER_LATENCY} \
DEC_REFERENCE_BUFFER_NONSEQ=${DEC_REFERENCE_BUFFER_NONSEQ} \
DEC_REFERENCE_BUFFER_SEQ=${DEC_REFERENCE_BUFFER_SEQ} \
JPEG_MCUS_SLICE=${JPEG_MCUS_SLICE} \
JPEG_INPUT_BUFFER_SIZE=${JPEG_INPUT_BUFFER_SIZE} \
VP8_LUMA_STRIDE="${VP8_LUMA_STRIDE}" \
VP8_CHROMA_STRIDE="${VP8_CHROMA_STRIDE}" \
VP8_INTERLEAVED_PIC_BUFFERS="${VP8_INTERLEAVED_PIC_BUFFERS}" \
VP8_EXT_MEM_ALLOC_ORDER="${VP8_EXT_MEM_ALLOC_ORDER}" \
PP_CLOCK_GATING=${PP_CLOCK_GATING} \
PP_BUS_BURST_LENGTH=${PP_BUS_BURST_LENGTH} \
PP_DATA_DISCARD=${PP_DATA_DISCARD} \
PP_INPUT_PICTURE_ENDIAN=${PP_INPUT_PICTURE_ENDIAN} \
PP_OUTPUT_PICTURE_ENDIAN=${PP_OUTPUT_PICTURE_ENDIAN} \
PP_WORD_SWAP=${PP_WORD_SWAP} \
PP_WORD_SWAP16=${PP_WORD_SWAP16} \
PP_MAX_OUTPUT=${PP_MAX_OUTPUT} \
PP_ALPHA_BLENDING=${PP_ALPHA_BLENDING} \
PP_SCALING=${PP_SCALING} \
PP_DEINTERLACING=${PP_DEINTERLACING} \
PP_DITHERING=${PP_DITHERING} \
PP_MULTI_BUFFER=${PP_MULTI_BUFFER} \
TB_PACKET_BY_PACKET=${TB_PACKET_BY_PACKET} \
TB_SEED_RND=${TB_SEED_RND} \
TB_STREAM_BIT_SWAP=${TB_STREAM_BIT_SWAP} \
TB_STREAM_PACKET_LOSS=${TB_STREAM_PACKET_LOSS} \
TB_STREAM_HEADER_CORRUPT=${TB_STREAM_HEADER_CORRUPT} \
TB_STREAM_TRUNCATE=${TB_STREAM_TRUNCATE} \
TB_INTERNAL_TEST=${TB_INTERNAL_TEST} \
TB_MD5SUM=${TB_MD5SUM} \
TB_WHOLE_STREAM=${TB_WHOLE_STREAM} \
TB_NAL_UNIT_STREAM=${TB_NAL_UNIT_STREAM} \
TB_H264_LONG_STREAM=${TB_H264_LONG_STREAM} \
TB_VC1_LONG_STREAM=${TB_VC1_LONG_STREAM} \
TB_VC1_UD=${TB_VC1_UD} \
TB_USE_PEEK_FOR_OUTPUT=${TB_USE_PEEK_FOR_OUTPUT} \
TB_VP8_STRIDE=${TB_VP8_STRIDE} \
TB_SKIP_NON_REF_PICS=${TB_SKIP_NON_REF_PICS}" >> "case_${case_number}${TB_LOG_FILE_SUFFIX}"

    
    TB_VP8_STRIDE="$tb_vp8_stride_current"
    DEC_MEMORY_ALLOCATION="$dec_memory_allocation_current"
    VP8_EXT_MEM_ALLOC_ORDER="$vp8_memory_allocation_order_current"
    JPEG_INPUT_BUFFER_SIZE="0"
    JPEG_MCUS_SLICE="0"
    
    return "$ret_val"
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Gets the next test case starting from the provided case nmuber             --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#- 2 : Test cases to be searched from                                         --
#-                                                                            --
#- Exports                                                                    --
#- next_case : The test case number. 0 if next case cannot be found, or the   --
#-             provided case is the last case.                                --
#-                                                                            --
#-------------------------------------------------------------------------------
getNextCase()
{
    first_case=$1
    cases=$2
    
    # loop through all the cases in the category
    next_case=0
    found=0
    for case_number in $cases
    do
        # previous iteration found the first case
        # export current one
        if [ "$found" == "1" ]
        then
            next_case=$case_number
            break
        fi
        
        # if current case match, the next on will be returned
        if [ "$case_number" == "$first_case" ]
        then
            found=1
        fi
    done
    
    export next_case
}

runCases()
{
    test_cases=$1
    test_start_time=$2
    use_previous_cfg=$3

    # inform if random configuration enabled
    if [ "$TB_RND_CFG" == "ENABLED" ] && [ "$use_previous_cfg" == 0 ]
    then
        echo "Randomized test case configuration enabled"
    fi

    # run the test cases
    # get the first case if the previous execution was halted
    first_case=0
    case_done=""
    if [ -e test_exec_status ]
    then
        first_case=`tail -n 1 test_exec_status`
        case_done=`echo "$first_case" | grep "DONE"`
            
        # check maximum retries
        if [ -z "$case_done" ]
        then
            retries=3
            last_cases=`tail -n $retries test_exec_status`
            
            for last_case in $last_cases
            do
                if [ "$last_case" == "$first_case" ]
                then
                    let 'retries-=1'
                    if [ "$retries" == "0" ]
                    then
                        touch "case_${test_case}.done"
                        echo -n -e "\n${first_case} DONE (MAXIMUM RETRIES REACHED)" >> test_exec_status
                    fi
                fi
            done
        fi
        
        # check if the last case was executed completely
        first_case=`tail -n 1 test_exec_status`
        case_done=`echo "$first_case" | grep "DONE"`
        if [ ! -z "$case_done" ]
        then
            i=`expr index "$first_case" ' '`
            let 'i-=1'
            first_case=${first_case:0:$i}
            getNextCase "$first_case" "$test_cases"
            first_case="$next_case"
        fi
    fi
    
    # print out the cases for check use
    first_case_tmp=$first_case
    let 'cases_number=0'
    for test_case in $test_cases
    do
        if [ $test_case == $first_case ] || [ $first_case == 0 ]
        then
            let 'cases_number=cases_number+1'
            echo "${test_case}" >> test_cases
            rm -f "case_${test_case}.done"
            rm -f "case_${test_case}${TB_LOG_FILE_SUFFIX}"
            rm -f "case_${test_case}.timeout"
            rm -f "case_${test_case}.crash"
            rm -f "case_${test_case}.out_of_space"
            rm -f "case_${test_case}_${HW_PEFORMANCE_LOG}"
            rm -f "case_${test_case}.time_limit"
            rm -f "case_${test_case}.time"
            rm -f "case_${test_case}.cfg"
            rm -f "case_${test_case}_tb_params"
            first_case=0
        fi
    done
    touch test_cases_printed
    touch test_cases_printed_failed
    first_case=$first_case_tmp
        
    #if [ -z "$case_done" ] && [ -e test_exec_status ]
    #then
    #    echo -n " ABORTED" >> test_exec_status
    #fi
    
    touch test_exec_status
    chmod 666 test_exec_status
    chmod 666 test_cases
    let 'test_execution_time_sec=0'
    if [ "$TB_TEST_EXECUTION_TIME" != "-1" ]
    then
        #let 'test_execution_time_sec=TB_TEST_EXECUTION_TIME*60*60'
        script_end_time=$(date +%s)
        let 'test_execution_time_sec=TB_TEST_EXECUTION_TIME*60*60-(script_end_time-test_start_time)'
    fi
    
    # set the version numbers for golbal use
    i=`expr index "$HWBASETAG" '_'`
    hw_buid_string=${HWBASETAG:$i}
    i=`expr index "$hw_buid_string" '_'`
    let 'i-=1'
    g_hw_major=${hw_buid_string:0:$i}
    let 'i+=1'
    g_hw_minor=${hw_buid_string:$i}

    let 'switch_time=0'
    let 'switch_time_avarage=0'
    let 'switch_time_cumulative=0'
    let 'test_execution_time_sec=test_execution_time_sec-(cases_number*switch_time_avarage)'
    let 'cases_executed=0'
    # run the case starting from the previously halted test case
    for test_case in $test_cases
    do
        if [ $test_case == $first_case ] || [ $first_case == 0 ]
        then
            script_start_time=$(date +%s)
            echo -n -e "\n$test_case" >> test_exec_status
            if [ "$TB_TEST_EXECUTION_TIME" != "-1"  ]
            then
                let 'execution_time_per_case=test_execution_time_sec/cases_number'
                let 'test_execution_time_sec=test_execution_time_sec+(cases_number*switch_time_avarage)'
                runCase "$test_case" "$execution_time_per_case" "$use_previous_cfg"
                while [ ! -e "case_${case_number}.time" ]
                do
                    sleep 1
                done
                case_execution_time=`cat case_${case_number}.time`
                #let 'test_execution_time_sec=test_execution_time_sec-case_execution_time'
                let 'cases_number-=1'
                let 'cases_executed+=1'
                #touch "case_${test_case}.done"
                echo -n " DONE" >> test_exec_status
                first_case=0
                script_end_time=$(date +%s)
                let 'switch_time=script_end_time-script_start_time-case_execution_time'
                # just use the previous switch, not the avarage
                #let 'switch_time_cumulative+=switch_time'
                #let 'switch_time_avarage=switch_time_cumulative/cases_executed'
                let 'switch_time_avarage=switch_time'
                
                let 'test_execution_time_sec=TB_TEST_EXECUTION_TIME*60*60-(script_end_time-test_start_time)'
                let 'test_execution_time_sec=test_execution_time_sec-(cases_number*switch_time_avarage)'
            else
                runCase "$test_case" "$execution_time_per_case" "$use_previous_cfg"
                #touch "case_${test_case}.done"
                echo -n " DONE" >> test_exec_status
                first_case=0
            fi
        fi
    done
}
