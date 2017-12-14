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
#--   Abstract     : Various utility functions for the test execution scripts --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

if [ $1 == "run" ]
then
    . ../commonconfig.sh
    . ./config.sh
    TEST_CASE_LIST=$TEST_CASE_LIST_RUN
    TEST_DATA_HOME=$TEST_DATA_HOME_RUN
    
elif [ $1 == "check" ]
then
    . ../commonconfig.sh
    . ./config.sh
    TEST_CASE_LIST=$TEST_CASE_LIST_CHECK
    TEST_DATA_HOME=$TEST_DATA_HOME_CHECK
    
elif [ $1 == "summary" ]
then
    . ./commonconfig.sh
fi

. $TEST_CASE_LIST

#-------------------------------------------------------------------------------
#-                                                                            --
#- Gets the test case category for the test case.                             --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Exports                                                                    --
#- category : The category of the test case                                   --
#-                                                                            --
#- Returns                                                                    --
#- 1 : If category cannot be found                                            --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
getCategory()
{
    case_number=$1
    
    for category in $category_list_swhw
    do
        eval cases=\$$category 
        for i in $cases
        do
            if [ "$i" == "$case_number" ]
            then
                export category
                return 0
            fi
        done  
    done
    
    return 1
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Finds the decoder/post-processor from the category.                        --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Returns                                                                    --
#- 1 : Cannot find decoder/post processor for the category                    --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
parseDecoder()
{
    category=$1
    
    decoder="none"
    post_processor="none"
    
    # find the correct decoder by grepping the string representing the decoding 
    # format from the provided category
    decoding_formats="h264 svc mvc mpeg4 h263 sorenson jpeg vc1 mpeg2 mpeg1 rv vp6 vp7 vp8 divx avs webp"
    for format in $decoding_formats
    do
        present=`echo "$category" | grep "$format"`
        if [ ! -z "$present" ]
        then
            decoder="$format"
            
            # h263, sorenson and divx decoding format is handled by the mpeg4 decoder
            if [ "$format" == "h263" ] || [ "$format" == "sorenson" ] || [ "$format" == "divx" ]
            then
                decoder="mpeg4"
            fi
            
            # mpeg1 decoding format is handled by the mpeg2 decoder
            if [ "$format" == "mpeg1" ]
            then
                decoder="mpeg2"
            fi
            
            if [ "$format" == "rv" ]
            then
                decoder="real"
            fi
            
            if [ "$format" == "svc" ] || [ "$format" == "mvc" ]
            then
                decoder="h264"
            fi
            
            if [ "$format" == "vp7" ] || [ "$format" == "webp" ]
            then
                decoder="vp8"
            fi
            break
        fi
    done
    
    # check if the post processor is present
    present=`echo "$category" | grep pp`
    if [ ! -z "$present" ]
    then
        post_processor="pp"
    fi
    
    if [ "$post_processor" == "none" ] && [ "$decoder" == "none" ]
    then
        echo "Cannot find decoder/post processor: ${category}"
        return 1
    fi
    
    return 0
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Finds the decoder/post-processor for the test case.                        --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Exports                                                                    --
#- decoder        : Decoding format                                           --
#- post_processor : Possible post-processor                                   --
#-                                                                            --
#- Returns                                                                    --
#- 1 : - Cannot find decoder/post processor for test case                     --
#-     - Not a valid test case number                                         --
#- 0 : Otherwise                                                              --
#-                                                                            --
#-------------------------------------------------------------------------------
getComponent()
{
    case_number=$1
    
    getCategory "$case_number"
    if [ $? == 0 ]
    then
        # find the matching decoder for the case_number
        # find out also if the decoder is pipelined
        parseDecoder "$category"
        if [ $? == 0 ]
        then
            export decoder
            export post_processor
            return 0
        else
            return 1
        fi
    fi
    echo "Invalid case number: $case_number (see ${TEST_CASE_LIST})"
    return 1
}

##-------------------------------------------------------------------------------
##-                                                                            --
##- Determines whether the provided string contains only digits.               --
##-                                                                            --
##- Parameters                                                                 --
##- 1 : String                                                                 --
##-                                                                            --
##- Returns                                                                    --
##- 1 : The string containd only strings                                       --
##- 0 : Otherwise                                                              --
##-                                                                            --
##-------------------------------------------------------------------------------
#isDigit()
#{
#    [ $# -eq 1 ] || return 1
#    case $1 in
#        *[!0-9]*|"") return 1;;
#                  *) return 0;;
#    esac
#}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Determines the maximum picture width of a post-processor test case         --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Exports                                                                    --
#- pp_case_width : Test case maximum picture width or 0 if width cannot be    --
#- determine                                                                  --
#-                                                                            --
#-------------------------------------------------------------------------------
getPpCaseWidth()
{
    case_number=$1
    
    let 'pp_case_width=0'
    
    if [ ! -e ${TEST_DATA_HOME}/case_${case_number}/picture_ctrl_pp.trc ]
    then
        export pp_case_width
        return
    fi
    
    widths=`cat ${TEST_DATA_HOME}/case_${case_number}/picture_ctrl_pp.trc | grep -i "scaled width in pixels (output w)" | sed 's/[^0-9]*$//'`
    if [ -z "$widths" ]
    then
        export pp_case_width
        return
    fi
    
    let 'pp_case_width=0'
    for width in $widths
    do
        if [ $width -gt $pp_case_width ]
        then
            let 'pp_case_width=width'
        fi
    done
    export pp_case_width
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Determines the maximum picture width of a decoder test case                --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Exports                                                                    --
#- dec_case_width : Test case maximum picture width or 0 if width cannot be   --
#- determine                                                                  --
#-                                                                            --
#-------------------------------------------------------------------------------
getDecCaseWidth()
{
    case_number=$1
    let 'dec_case_width=0'
    
    if [ ! -e ${TEST_DATA_HOME}/case_${case_number}/picture_ctrl_dec.trc ]
    then
        export dec_case_width
        return
    fi
    
    widths=`cat ${TEST_DATA_HOME}/case_${case_number}/picture_ctrl_dec.trc | grep -i "picture width in macroblocks" | sed 's/[^0-9]*$//'`
    if [ -z "$widths" ]
    then
        export dec_case_width
        return
    fi
    
    for width in $widths
    do
        # Convert to pixel size
        let 'width=width*16'
        if [ $width -gt $dec_case_width ]
        then
            let 'dec_case_width=width'
        fi
    done
    export dec_case_width
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Determines the maximum picture height of a decoder test case               --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Exports                                                                    --
#- dec_case_width : Test case maximum picture height or 0 if width cannot be  --
#- determine                                                                  --
#-                                                                            --
#-------------------------------------------------------------------------------
getDecCaseHeight()
{
    case_number=$1
    let 'dec_case_height=0'
    
    if [ ! -e ${TEST_DATA_HOME}/case_${case_number}/picture_ctrl_dec.trc ]
    then
        export dec_case_height
        return
    fi
    
    heights=`cat ${TEST_DATA_HOME}/case_${case_number}/picture_ctrl_dec.trc | grep -i "picture height in macroblocks" | sed 's/[^0-9]*$//'`
    if [ -z "$heights" ]
    then
        export dec_case_height
        return
    fi
    
    for height in $heights
    do
        # Convert to pixel size
        let 'height=height*16'
        if [ $height -gt $dec_case_height ]
        then
            let 'dec_case_height=height'
        fi
    done
    export dec_case_height
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Gets test cases that match the filter.                                     --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Category list from which test cases are added                          --
#- 2 : Test case number                                                       --
#- 3 : Filter                                                                 --
#- 4 : Filter cases also based on resolution                                  --
#-                                                                            --
#- Exports                                                                    --
#- test_cases : List of test cases that match the filter.                     --
#-                                                                            --
#-------------------------------------------------------------------------------
getCases()
{
    given_category_list=$1
    case_number=$2
    case_filter=$3
    filter_resolution=$4
    silent=$5

    test_cases=$case_number
    
    # if all cases are numeric, directly set them to the list. Filters cannot be used.
    case_number_contains_letters=`echo $case_number | grep -i '[a-z]'`
    if [ "$case_number_contains_letters" == "" ]
    then
        # pixac cases are filtered out for non AXI bus
        if [ "$HW_BUS" != "AXI" ]
        then
            test_cases=""
            for test_case in $case_number
            do
                getCategory "$test_case"
                pixac_cat=`echo "$category" | grep "_pixac"`
                if [ ! -z "$pixac_cat" ]
                then
                    continue
                else
                    test_cases="${test_cases} ${test_case}"
                fi
            done
        fi
  
    # if there are alpha-numeric arguments, start category matching
    else
    
        # If argument is identified as one of the test case categories, 
        # use indirect reference to the matched category and set the cases it
        # contains into cases_in_category variable
        for category in $given_category_list
        do  
            category_added=0
            
            # Multiple sub-categories can be given; in this case, loop them all
            for argument in $case_number
            do
                
                arg_contains_letters=`echo $argument | grep -i '[a-z]'`

                # If argument is numeric, it cannot be used as a category wildcard
                if [ "$arg_contains_letters" == "" ]
                then
                    # see if this case is already in the list 
                    # use \< \> instead of -w
                    if eval "echo ${test_cases} | grep '\<${argument}\>' >/dev/null"
                    then
                        # do nothing
                        arg_contains_letters=""
                    else
                        # pixac cases are filtered out for non AXI bus
                        if [ "$HW_BUS" != "AXI" ]
                        then
                            getCategory "$argument"
                            pixac_cat=`echo "$category" | grep "_pixac"`
                            if [ ! -z "$pixac_cat" ]
                            then
                                continue
                            fi
                        fi
                        test_cases="$argument $test_cases" 
                    fi
         
                # If current category contains the letters present in argument 
                elif eval "echo ${category} | grep '${argument}'>/dev/null"
                then
                    # pixac cases are filtered out for non AXI bus
                    if [ "$HW_BUS" != "AXI" ]
                    then
                        pixac_cat=`echo "$category" | grep "_pixac"`
                        if [ ! -z "$pixac_cat" ]
                        then
                            case_filter="${case_filter} _pixac"
                        fi
                    fi
                    
                    #=====-------------------------------
                    # check don't-include category filtering 

                    filter_match=0

                    for filter in $case_filter 
                    do
                        if eval "echo ${category} | grep '${filter}'>/dev/null"
                        then
                            filter_match=1
                            break
                        fi
                    done 
        
                    if [ $filter_match -eq 0 ]
                    then
                        # Initialise test_cases to null if this is the first found category
                        if [ "$test_cases" == "$case_number" ]
                        then
                            test_cases=""
                        fi
                    
                        # Get cases in found category
                        eval cases_in_category=\$$category
                        
                        # see if this category is already in the list 
                        if eval "echo ${test_cases} | grep -v '\<${cases_in_category}\>' >/dev/null"
                        then
                            # Append the new cases to the list of cases to run
                            test_cases="$test_cases $cases_in_category"
                            #echo "Category added: ${category}"
                            category_added=1
                            #if [ $category == mpeg4_pp_error ]
                            #then
                            #    echo $cases_in_category
                            #fi
                            break
                        fi
                    fi
                fi
            done  # arguments loop
            
            if [ "$silent" -eq 0 ]
            then
                if [ $category_added == 1 ]
                then
                    echo "Category added: ${category}"
                else
                    echo "Category excluded: ${category}"
                fi
            fi
        done    # categories loop
    fi
    
    # if cases were not found
    case_number_contains_letters=`echo $test_cases | grep -i '[a-z]'`
    if [ ! -z "$case_number_contains_letters" ]
    then
        test_cases=""
        categories=""
        if [ "$silent" -eq 0 ]
        then
            echo "External categories added: ${case_number}"
        fi
        for category in $case_number
        do
            eval category_tmp=\$$category
            categories="${categories} ${category_tmp}"
        done
        for category in $categories
        do
            case_number_contains_letters=`echo $category | grep -i '[a-z]'`
            test_cases_in_category=""
            if [ ! -z $case_number_contains_letters ]
            then
                eval test_cases_in_category=\$$category
            
            else
                test_cases_in_category=$category
                
            fi
            test_cases="${test_cases} ${test_cases_in_category}"
        done
        # remove the not supported cases
        test_cases_tmp=$test_cases
        test_cases=""
        for test_case in $test_cases_tmp
        do
            getComponent "$test_case"
            match_pp="0"
            match_dec="0"
            for supported_format in $RUN_FORMATS_EXT
            do
                if [ "$post_processor" == "$supported_format" ]
                then
                    match_pp="1"
                fi
                    
                if [ "$decoder" == "$supported_format" ]
                then
                    match_dec="1"
                fi
            done
            if [ "$match_pp" == "1" ]  && [ "$decoder" != "none" ] && [ "$match_dec" == "1" ]  && [ "$post_processor" != "none" ]
            then
                test_cases="$test_cases $test_case"
            fi
            if [ "$match_pp" == "1" ]  && [ "$decoder" == "none" ]
            then
                test_cases="$test_cases $test_case"
            fi
            if [ "$match_dec" == "1" ]  && [ "$post_processor" == "none" ]
            then
                test_cases="$test_cases $test_case"
            fi
        done
    fi
    
#    # Exclude the specified cases
#exclude_cases="6090 6091 6092 6093 6218 6219 6220 6221 6222 6223 6224 6225 6226 6227 6228 \
#6229 6230 6231 6232 6233 6234 6235 6236 6237 6238 6239 6240 6241 6242 6243 \
#6244 6245 6246 6247 6248 6249 6250 6251 6252 6253 6254 6255 6300 6301 6302 \
#6303 6304 6305 6306 6307 6308 6309 6310 6311 6312 6313 6314 6315 6360 6361 \
#6362 6363 6364 6365 6366 6367 6368 6369 6370 6371 6372 6373 \
#6000 6001 6042 6043 6044 6045 6046 6047 6048 6049 6050 6051 6052 6053 6054 \
#6068 6069 6070 6071 6072 6086 6087 6088 6089 \
#425 6254 6255 6314 6315"
#    tmp_test_cases=$test_cases
#    test_cases=""
#    for test_case in $tmp_test_cases
#    do
#        found=0
#        for exclude_case in $exclude_cases
#        do
#            if [ $test_case == $exclude_case ]
#            then
#                echo "HOX HOX!!! CASE ${test_case} EXCLUDED!!!"
#                found=1
#            fi
#        done
#        if [ "$found" != 1 ]
#        then
#            test_cases="$test_cases $test_case"
#        fi
#    done
    
    # Now, check that case match within TB_MAX_*_OUTPUT
    if [ $filter_resolution == 1 ] && [[ $TB_MAX_PP_OUTPUT != "-1" && $TB_MAX_DEC_OUTPUT != "-1" ]]
    then
        tmp_cases=""
        for test_case in $test_cases
        do
            getComponent "$test_case"
        
            # Post-processor case
            if [ $post_processor == "pp" ] && [ $decoder == "none" ]
            then
                if [ $TB_MAX_PP_OUTPUT != "-1" ]
                then
                    getPpCaseWidth "$test_case"
                else
                    pp_case_width=$TB_MAX_PP_OUTPUT
                fi
            
                if [ $pp_case_width -le $TB_MAX_PP_OUTPUT ]
                then
                    tmp_cases="${tmp_cases} ${test_case}"
                
                else
                    if [ "$silent" -eq 0 ]
                    then
                        echo "Test case resolution above ${TB_MAX_PP_OUTPUT}; test case ${test_case} removed"
                    fi
                fi
            
            # Video case
            elif [ $decoder != "jpeg" ] && [ $decoder != "none" ] && [ $post_processor == "none" ]
            then
                if [ $TB_MAX_DEC_OUTPUT != "-1" ]
                then
                    getDecCaseWidth "$test_case"
                else
                    dec_case_width=$TB_MAX_DEC_OUTPUT
                fi
                    
                if [ $dec_case_width -le $TB_MAX_DEC_OUTPUT ]
                then
                    tmp_cases="${tmp_cases} ${test_case}"
                
                else
                    if [ "$silent" -eq 0 ]
                    then
                        echo "Test case resolution above ${TB_MAX_DEC_OUTPUT}; test case ${test_case} removed"
                    fi
                fi
            
            # Video pipelined case
            elif [ $decoder != "jpeg" ] && [ $decoder != "none" ] && [ $post_processor == "pp" ]
            then
                if [ $TB_MAX_PP_OUTPUT != "-1" ]
                then
                    getPpCaseWidth "$test_case"
                else
                    pp_case_width=$TB_MAX_PP_OUTPUT
                fi
                
                if [ $TB_MAX_DEC_OUTPUT != "-1" ]
                then
                    getDecCaseWidth "$test_case"
                else
                    dec_case_width=$TB_MAX_DEC_OUTPUT
                fi
            
                if [ $dec_case_width -le $TB_MAX_DEC_OUTPUT ] && [ $pp_case_width -le $TB_MAX_PP_OUTPUT ]
                then
                    tmp_cases="${tmp_cases} ${test_case}"
                
                else
                    if [ "$silent" -eq 0 ]
                    then
                        echo "Test case resolution above ${TB_MAX_DEC_OUTPUT} or ${TB_MAX_PP_OUTPUT}; test case ${test_case} removed"
                    fi
                fi
                
            # JPEG case
            elif [ $decoder == "jpeg" ] && [ $post_processor == "none" ] 
            then
                tmp_cases="${tmp_cases} ${test_case}"
            
            # JPEG pipelined case
            elif [ $decoder == "jpeg" ] && [ $post_processor == "pp" ]
            then
                if [ $TB_MAX_PP_OUTPUT != "-1" ]
                then
                    getPpCaseWidth "$test_case"
                else
                    pp_case_width=$TB_MAX_PP_OUTPUT
                fi
            
                if [ $pp_case_width -le $TB_MAX_PP_OUTPUT ]
                then
                    tmp_cases="${tmp_cases} ${test_case}"
                
                else
                    if [ "$silent" -eq 0 ]
                    then
                        echo "Test case resolution above ${TB_MAX_PP_OUTPUT}; test case ${test_case} removed"
                    fi
                fi
            fi
        done
        test_cases=$tmp_cases
    fi
    
    export test_cases
 }

printStringWithExtraSpaces()
{
    string=$1
    columnWidth=$2
    file=$3
    
    out_string_len=${#string}
    let 'extra_spaces=columnWidth-out_string_len'
    if [ ! -z $file ]
    then
        echo -n -e "$string" >> $file
    else
        echo -n -e "$string"
    fi
    let 'space_number=0'
    while [ "$space_number" -lt "$extra_spaces" ]
    do
        if [ ! -z $file ]
        then
            echo -n -e " " >> $file
        else
            echo -n -e " "
        fi
        let 'space_number+=1'
    done
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Prints out the test case number together with decoder/post-processor in    --
#- question.                                                                  -- 
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#- 2 : Decoding format                                                        --
#- 3 : Possible post-processor                                                --
#-                                                                            --
#-------------------------------------------------------------------------------
printCase()
{
    case_number=$1
    decoder=$2
    post_processor=$3
    
    getCategory "$case_number"
    if [ $? == 1 ]
    then
        out_string="UNKNOWN CATEGORY !!!"
    else
        out_string=$category
    fi
    
    #if [ "$post_processor" == "pp" ]
    #then
    #    out_string="PP CASE"
    #fi
    
    #if [ "$decoder" == "h264" ] && [ "$post_processor" == "pp" ]
    #then
    #    out_string="PP/H.264 CASE"
    #fi
    
    #if [ "$decoder" == "mpeg4" ]  && [ "$post_processor" == "pp" ]
    #then
    #    out_string="PP/MPEG-4 CASE"
    #fi
    
    #if [ "$decoder" == "vc1" ]  && [ "$post_processor" == "pp" ]
    #then
    #    out_string="PP/VC-1 CASE"
    #fi

    #if [ "$decoder" == "jpeg" ]  && [ "$post_processor" == "pp" ]
    #then
    #    out_string="PP/JPEG CASE"
    #fi
    
    #if [ "$decoder" == "mpeg2" ]  && [ "$post_processor" == "pp" ]
    #then
    #    out_string="PP/MPEG-2 CASE"
    #fi

    #if [ "$decoder" == "h264" ] && [ "$post_processor" != "pp" ]
    #then
    #    out_string="H.264 CASE"
    #fi

    #if [ "$decoder" == "mpeg4" ] && [ "$post_processor" != "pp" ]
    #then
    #    out_string="MPEG-4 CASE"
    #fi

    #if [ "$decoder" == "vc1" ] && [ "$post_processor" != "pp" ]
    #then
    #    out_string="VC-1 CASE"
    #fi

    #if [ "$decoder" == "jpeg" ] && [ "$post_processor" != "pp" ]
    #then
    #    out_string="JPEG CASE"
    #fi
    
    #if [ "$decoder" == "mpeg2" ] && [ "$post_processor" != "pp" ]
    #then
    #    out_string="MPEG-2 CASE"
    #fi
    
    # calculate extra spaces and print
    printStringWithExtraSpaces "$out_string" 30
    printStringWithExtraSpaces "$case_number" 7
}

#-------------------------------------------------------------------------------
#-                                                                            --
#- Determines the maximum picture width of a decoder test case                --
#-                                                                            --
#- Parameters                                                                 --
#- 1 : Test case number                                                       --
#-                                                                            --
#- Exports                                                                    --
#- dec_case_width : Test case maximum picture width or 0 if width cannot be   --
#-                                                                            --
#-------------------------------------------------------------------------------
getDecCaseWidthWithAwk()
{
    case_number=$1
    
    let 'dec_case_width=0'
    
    if [ ! -e ${TEST_DATA_HOME}/case_${case_number}/picture_ctrl_dec.trc ]
    then
        export dec_case_width
        return
    fi
    
    widths=`cat ${TEST_DATA_HOME}/case_${case_number}/picture_ctrl_dec.trc | grep -i "picture width in macroblocks" | awk '{print $1}'`
    if [ -z "$widths" ]
    then
        export dec_case_width
        return
    fi
    
    for width in $widths
    do
        # Convert to pixel size
        let 'width=width*16'
        if [ $width -gt $dec_case_width ]
        then
            let 'dec_case_width=width'
        fi
    done
    export dec_case_width
}

isValidRlcModeCase()
{
    case_number="$1"
    
    invalid_rlc_categs="_hp_ _mp_ _high_ _main_ _cabac_ cabac_ asp mpeg4_error divx sorenson"
    getCategory "$case_number"
    for categ in $invalid_rlc_categs
    do
        found=`echo "$category" | grep "$categ"`
        if [ ! -z "$found" ]
        then
            return 0
        fi
    done
    return 1
}

isValidMultiBufferCase()
{
    case_number="$1"
    
    getCategory "$case_number"
    parseDecoder "$category"
    if [ "$decoder" == "jpeg" ] || [ "$decoder" == "none" ]
    then
        return 0
    else
        return 1
    fi
}

isValidVC1UdCase()
{
    case_number="$1"
    
    getCategory "$case_number"
    found=`echo "$category" | grep "adv"`
    if [ ! -z "$found" ]
    then
        return 1
    fi
    return 0
}

isValidH264NalCase()
{
    case_number="$1"
    
    getCategory "$case_number"
    found=`echo "$category" | grep "h264"`
    if [ ! -z "$found" ]
    then
        return 1
    fi
    return 0
}
