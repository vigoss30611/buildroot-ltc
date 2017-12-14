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
#--   Abstract     : Script for setting up configuration(s). Generates        -- 
#--                  scripts for running and checking all the cases.          --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

. ./commonconfig.sh
. ./f_utils.sh "dummy"
if [ "$TB_TEST_DB" == "ENABLED" ]
then
    . ./f_testdb.sh
fi

g_add_execute_cases=1
if [ "$1" == "--no_test_session" ]
then
    g_add_execute_cases=0
fi

appendConfigurations()
{
    configurations=$1
    
    for configuration in $configurations
    do
        configuration_added=`echo "$CONFIGURATIONS" | grep "$configuration"`
        if [ -z "$configuration_added" ]
        then
            CONFIGURATIONS="${CONFIGURATIONS} ${configuration}"
        fi
    done
}

appendCases()
{
    configuration=$1
    cases=$2
    
    included_cases=`echo "$cases" | awk -F \-n '{print $1}'`
    excluded_cases=`echo "$cases" | awk -F \-n '{print $2}'`
    
    eval configuration_cases=\$${configuration}_cases
    included_all_cases=`echo "$configuration_cases" | awk -F \-n '{print $1}' | tr -d '"'`
    included_all_cases="${included_all_cases} ${included_cases}"
    excluded_all_cases=`echo "$configuration_cases" | awk -F \-n '{print $2}' | tr -d '"'`
    if [ ! -z "$excluded_cases" ]
    then
        excluded_all_cases="-n \"${excluded_all_cases} ${excluded_cases}\""
    else
        excluded_all_cases="-n \"${excluded_all_cases}\""
    fi

    if [ "$configuration" == "integration_default" ]
    then
        integration_default_cases="\"${included_all_cases}\" ${excluded_all_cases}"
    fi
    if [ "$configuration" == "integration_pbyp" ]
    then
        integration_pbyp_cases="\"${included_all_cases}\" ${excluded_all_cases}"
    fi
    if [ "$configuration" == "integration_wholestrm" ]
    then
        integration_wholestrm_cases="\"${included_all_cases}\" ${excluded_all_cases}"
    fi
    if [ "$configuration" == "integration_nal" ]
    then
        integration_nal_cases="\"${included_all_cases}\" ${excluded_all_cases}"
    fi
    if [ "$configuration" == "integration_vc1ud" ]
    then
        integration_vc1ud_cases="\"${included_all_cases}\" ${excluded_all_cases}"
    fi
    if [ "$configuration" == "integration_memext" ]
    then
        integration_memext_cases="\"${included_all_cases}\" ${excluded_all_cases}"
    fi
    if [ "$configuration" == "integration_stride" ]
    then
        integration_stride_cases="\"${included_all_cases}\" ${excluded_all_cases}"
    fi
    if [ "$configuration" == "integration_memext_stride" ]
    then
        integration_memext_stride_cases="\"${included_all_cases}\" ${excluded_all_cases}"
    fi
    if [ "$configuration" == "integration_rlc" ]
    then
        integration_rlc_cases="\"${included_all_cases}\" ${excluded_all_cases}"
    fi
    if [ "$configuration" == "integration_rlc_pbyp" ]
    then
        integration_rlc_pbyp_cases="\"${included_all_cases}\" ${excluded_all_cases}"
    fi
    if [ "$configuration" == "integration_rlc_nal" ]
    then
        integration_rlc_nal_cases="\"${included_all_cases}\" ${excluded_all_cases}"
    fi
    if [ "$configuration" == "integration_rlc_wholestrm" ]
    then
        integration_rlc_wholestrm_cases="\"${included_all_cases}\" ${excluded_all_cases}"
    fi
    if [ "$configuration" == "integration_mbuffer" ]
    then
        integration_mbuffer_cases="\"${included_all_cases}\" ${excluded_all_cases}"
    fi
    if [ "$configuration" == "integration_mbuffer_memext" ]
    then
        integration_mbuffer_memext_cases="\"${included_all_cases}\" ${excluded_all_cases}"
    fi
    if [ "$configuration" == "integration_relset" ]
    then
        integration_relset_cases="\"${included_all_cases}\" ${excluded_all_cases}"
    fi
}

setConfigurations()
{
    # reset cases
    CONFIGURATIONS=""
    integration_default_cases=""
    integration_pbyp_cases=""
    integration_wholestrm_cases=""
    integration_nal_cases=""
    integration_vc1ud_cases=""
    integration_memext_cases=""
    integration_stride_cases=""
    integration_memext_stride_cases=""
    integration_rlc_cases=""
    integration_rndcfg_cases=""
    integration_rndtc_cases=""
    integration_rndtc_rlc_cases=""
    integration_rlc_pbyp_cases=""
    integration_rlc_nal_cases=""
    integration_rlc_wholestrm_cases=""
    integration_mbuffer_cases=""
    integration_mbuffer_memext_cases=""
    integration_relset_cases=""
    
    i=`expr index "$HWBASETAG" '_'`
    hw_buid_string=${HWBASETAG:$i}
    i=`expr index "$hw_buid_string" '_'`
    let 'i-=1'
    hw_major=${hw_buid_string:0:$i}
    let 'i+=1'
    hw_minor=${hw_buid_string:$i}
    
    # detect decoding formats
    h264bp=`echo $HWCONFIGTAG | grep h264bp`
    h264hp=`echo $HWCONFIGTAG | grep h264hp`
    mvc=`echo $HWCONFIGTAG | grep mvc`
    if [ "$PRODUCT" == "g1" ]
    then
        mpeg2=`echo $HWCONFIGTAG | grep mp2`
        mpeg4sp=`echo $HWCONFIGTAG | grep mp4sp`
        mpeg4asp=`echo $HWCONFIGTAG | grep mp4asp`
    else
        mpeg2=`echo $HWCONFIGTAG | grep mpeg2`
        mpeg4sp=`echo $HWCONFIGTAG | grep mpeg4sp`
        mpeg4asp=`echo $HWCONFIGTAG | grep mpeg4asp`
    fi
    sor=`echo $HWCONFIGTAG | grep sor`
    divx=`echo $HWCONFIGTAG | grep divx`
    jpeg=`echo $HWCONFIGTAG | grep jpeg`
    vc1mp=`echo $HWCONFIGTAG | grep vc1mp`
    vc1ap=`echo $HWCONFIGTAG | grep vc1ap`
    rv=`echo $HWCONFIGTAG | grep rv`
    vp6=`echo $HWCONFIGTAG | tr "_" "\n" | grep vp | grep 6`
    vp7=`echo $HWCONFIGTAG | tr "_" "\n" | grep vp | grep 7`
    vp8=`echo $HWCONFIGTAG | tr "_" "\n" | grep vp | grep 8`
    avs=`echo $HWCONFIGTAG | grep avs`
    pp_found=`echo $HWCONFIGTAG | grep pp`
    
    # detect pp configuration
    pp_config=`echo $HWCONFIGTAG | awk -F \pp '{print $2}' | tr "_" " " | awk '{print $1}'`
    PP_ALPHA_BLENDING="DISABLED"
    PP_SCALING="DISABLED"
    PP_DEINTERLACING="DISABLED"
    PP_DITHERING="DISABLED"
    ablend=`echo "$pp_config" | grep a`
    if [ ! -z "$ablend" ]
    then
        echo "Post-Processor Alpha Blending detected"
        PP_ALPHA_BLENDING="ENABLED"
    fi
    scaling=`echo "$pp_config" | grep s`
    if [ ! -z "$scaling" ]
    then
        echo "Post-Processor Scaling detected"
        PP_SCALING="ENABLED"
    fi
    deint=`echo "$pp_config" | grep de`
    if [ ! -z "$deint" ]
    then
        echo "Post-Processor Deinterlacing detected"
        PP_DEINTERLACING="ENABLED"
    fi
    dither=`echo "$pp_config" | grep di`
    if [ ! -z "$dither" ]
    then
        echo "Post-Processor Dithering detected"
        PP_DITHERING="ENABLED"
    fi
    
    # detect picture diemnsions
    DEC_MAX_OUTPUT="0"
    PP_MAX_OUTPUT="0"
    
    hw_config_tag_items=`echo $HWCONFIGTAG | tr "_" " "`
    for item in $hw_config_tag_items
    do
        dec_width=`echo $item | grep -e "d[1-9]"`
        if [ ! -z "$dec_width" ]
        then
            dec_width=`echo "$dec_width" | awk -F \d '{print $2}'`
            echo "Maximum Decoder width detected: ${dec_width}"
            DEC_MAX_OUTPUT="$dec_width"
        fi
        pp_width=`echo $item | grep -e "p[1-9]" | grep -v "v" | grep -v "m"`
        if [ ! -z "$pp_width" ]
        then
            pp_width=`echo "$pp_width" | awk -F \p '{print $2}'`
            echo "Maximum Post-Processor width detected: ${pp_width}"
            PP_MAX_OUTPUT="$pp_width"
        fi
    done
    
    # detect picture reference buffer
    DEC_REFERENCE_BUFFER="DISABLED"
    
    hw_config_tag_items=`echo $HWCONFIGTAG | tr "_" " "`
    for item in $hw_config_tag_items
    do
        refbuff=`echo $item | grep -w "rb2"`
        if [ ! -z "$refbuff" ]
        then
            echo "Reference Buffer 2 detected"
            DEC_REFERENCE_BUFFER="REF_BUFFER2"
        fi
        refbuff=`echo $item | grep -w "rb1"`
        if [ ! -z "$refbuff" ]
        then
            echo "Reference Buffer 1 detected"
            DEC_REFERENCE_BUFFER="REF_BUFFER"
        fi
        refbuff=`echo $item | grep -w "refbuff2"`
        if [ ! -z "$refbuff" ]
        then
            echo "Reference Buffer 2 detected"
            DEC_REFERENCE_BUFFER="REF_BUFFER2"
        fi
        refbuff=`echo $item | grep -w "refbuff"`
        if [ ! -z "$refbuff" ]
        then
            echo "Reference Buffer 1 detected"
            DEC_REFERENCE_BUFFER="REF_BUFFER"
        fi
    done
    
    exclude_formats=""
    h264_fourk_categories=""
    vc1_fourk_categories=""
    vp8_fourk_categories=""
    vp8_pp_fourk_categories=""
    if [ ! -z "$h264bp" ] || [ ! -z "$h264hp" ]
    then
        if [ "$FOURK_CASES_ONLY" == "DISABLED" ]
        then
            appendConfigurations "integration_relset integration_default integration_pbyp integration_wholestrm integration_nal integration_rlc integration_rlc_pbyp integration_rlc_nal integration_rlc_wholestrm"
        else
            appendConfigurations "integration_relset integration_default integration_pbyp integration_wholestrm integration_nal"
        fi
        if [ ! -z "$h264bp" ]
        then
            echo "H.264 Baseline detected"
            appendCases "integration_relset" "h264 -n _hp_ _mp_ _high_ _main_ _cabac_ cabac_"
            appendCases "integration_default" "h264 -n _hp_ _mp_ _high_ _main_ _cabac_ cabac_"
            appendCases "integration_pbyp" "h264 -n _hp_ _mp_ _high_ _main_ _cabac_ cabac_"
            appendCases "integration_wholestrm" "h264 -n _hp_ _mp_ _high_ _main_ _cabac_ cabac_"
            appendCases "integration_nal" "h264 -n _hp_ _mp_ _high_ _main_ _cabac_ cabac_"
            appendCases "integration_rlc" "h264 -n _hp_ _mp_ _high_ _main_ _cabac_ cabac_"
            appendCases "integration_rlc_pbyp" "h264 -n _hp_ _mp_ _high_ _main_ _cabac_ cabac_"
            appendCases "integration_rlc_nal" "h264 -n _hp_ _mp_ _high_ _main_ _cabac_ cabac_"
            appendCases "integration_rlc_wholestrm" "h264 -n _hp_ _mp_ _high_ _main_ _cabac_ cabac_"
        elif [ ! -z "$h264hp" ]
        then
            echo "H.264 High detected"
            if [ "$FOURK_CASES_ONLY" == "ENABLED" ]
            then
                fourk_categories=""
                for category in $category_list_swhw
                do
                    found_fourk=`echo "$category" |grep "h264" |grep "_hp_" |grep "_4k"`
                    if [ ! -z  "$found_fourk" ]
                    then
                        fourk_categories="${category} ${fourk_categories}"
                    fi
                done
                h264_fourk_categories="$fourk_categories"
                appendCases "integration_relset" "${fourk_categories}"
                appendCases "integration_default" "${fourk_categories}"
                appendCases "integration_pbyp" "${fourk_categories}"
                appendCases "integration_wholestrm" "${fourk_categories}"
                appendCases "integration_nal" "${fourk_categories}"
                
            else
                appendCases "integration_relset" "h264 svc"
                appendCases "integration_default" "h264 svc"
                appendCases "integration_pbyp" "h264 svc"
                appendCases "integration_wholestrm" "h264 svc"
                appendCases "integration_nal" "h264 svc"
                appendCases "integration_rlc" "h264 -n _hp_ _mp_ _high_ _main_ _cabac_ cabac_"
                appendCases "integration_rlc_pbyp" "h264 -n _hp_ _mp_ _high_ _main_ _cabac_ cabac_"
                appendCases "integration_rlc_nal" "h264 -n _hp_ _mp_ _high_ _main_ _cabac_ cabac_"
                appendCases "integration_rlc_wholestrm" "h264 -n _hp_ _mp_ _high_ _main_ _cabac_ cabac_"
            fi
        fi
        if [ "$FOURK_CASES_ONLY" == "DISABLED" ]
        then
            if [[ "$PRODUCT" == "g1" && "$hw_major" == "2" && "$hw_minor" -ge "15" ]] || [[ "$PRODUCT" == "g1" && "$hw_major" -ge "3" ]]
            then
                if [ ! -z "$mvc" ]
                then
                    echo "MVC detected"
                    appendCases "integration_relset" "mvc"
                    appendCases "integration_default" "mvc"
                    appendCases "integration_pbyp" "mvc"
                    appendCases "integration_wholestrm" "mvc"
                    appendCases "integration_nal" "mvc"
                else
                    exclude_formats="${exclude_formats} mvc"
                fi
            else
                appendCases "integration_relset" "mvc"
                appendCases "integration_default" "mvc"
                appendCases "integration_pbyp" "mvc"
                appendCases "integration_wholestrm" "mvc"
                appendCases "integration_nal" "mvc"
            fi
        fi
    else
        exclude_formats="${exclude_formats} h264 svc mvc"
    fi
   
    if [ ! -z "$mpeg2" ]
    then
        echo "MPEG-2 detected"
        appendConfigurations "integration_relset integration_default integration_pbyp integration_wholestrm"
        appendCases "integration_relset" "mpeg2 mpeg1"
        appendCases "integration_default" "mpeg2 mpeg1"
        appendCases "integration_pbyp" "mpeg2 mpeg1"
        appendCases "integration_wholestrm" "mpeg2 mpeg1"
    else
        exclude_formats="${exclude_formats} mpeg2 mpeg1"
    fi
   
    if [ ! -z "$mpeg4sp" ] || [ ! -z "$mpeg4asp" ] || [ ! -z "$sor" ] || [ ! -z "$divx" ]
    then
        appendConfigurations "integration_relset integration_default integration_pbyp integration_wholestrm integration_rlc integration_rlc_pbyp integration_rlc_wholestrm"
        appendCases "integration_relset" "mpeg4 h263"
        appendCases "integration_default" "mpeg4 h263"
        appendCases "integration_pbyp" "mpeg4 h263"
        appendCases "integration_wholestrm" "mpeg4 h263"
        appendCases "integration_rlc" "mpeg4 h263 -n asp mpeg4_error"
        appendCases "integration_rlc_pbyp" "mpeg4 h263 -n asp mpeg4_error"
        appendCases "integration_rlc_wholestrm" "mpeg4 h263 -n asp mpeg4_error"
        
        if [ ! -z "$mpeg4asp" ]
        then
            echo "MPEG-4 Advanced Simple detected"
        fi
        
        if [ ! -z "$mpeg4sp" ]
        then
            echo "MPEG-4 Simple detected"
            appendCases "integration_relset" "-n asp"
            appendCases "integration_default" "-n asp"
            appendCases "integration_pbyp" "-n asp"
            appendCases "integration_wholestrm" "-n asp"
        fi
        
        if [ ! -z "$sor" ]
        then
            echo "Sorenson detected"
            appendCases "integration_relset" "sor"
            appendCases "integration_default" "sor"
            appendCases "integration_pbyp" "sor"
            appendCases "integration_wholestrm" "sor"
        fi
        
        if [ ! -z "$divx" ]
        then
            echo "DivX detected"
            appendCases "integration_relset" "divx"
            appendCases "integration_default" "divx"
            appendCases "integration_pbyp" "divx"
            appendCases "integration_wholestrm" "divx"
        fi
    fi
    if [ -z "$mpeg4sp" ] && [ -z "$mpeg4asp" ]
    then
        exclude_formats="${exclude_formats} mpeg4 h263"    
    fi
    if [ -z "$sor" ]
    then
        exclude_formats="${exclude_formats} sor"
    fi
    if [ -z "$divx" ]
    then
        exclude_formats="${exclude_formats} divx"
    fi
   
    if [ ! -z "$jpeg" ]
    then
        echo "JPEG detected"
        appendConfigurations "integration_relset integration_default integration_memext"
        appendCases "integration_relset" "jpeg"
        appendCases "integration_default" "jpeg"
        appendCases "integration_memext" "jpeg"
    else
        exclude_formats="${exclude_formats} jpeg"
    fi
   
    if [ ! -z "$vc1mp" ] || [ ! -z "$vc1ap" ]
    then
        if [ "$FOURK_CASES_ONLY" == "DISABLED" ]
        then
            appendConfigurations "integration_relset integration_default"
            appendCases "integration_relset" "vc1"
            appendCases "integration_default" "vc1"
        else
            appendConfigurations "integration_relset integration_default integration_pbyp"
        fi
        if [ ! -z "$vc1ap" ]
        then
            echo "VC-1 Advanced detected"
            if [ "$FOURK_CASES_ONLY" == "ENABLED" ]
            then
                fourk_categories=""
                for category in $category_list_swhw
                do
                    found_fourk=`echo "$category" |grep "vc1" |grep "_adv_" |grep "_4k"`
                    if [ ! -z  "$found_fourk" ]
                    then
                        fourk_categories="${category} ${fourk_categories}"
                    fi
                done
                vc1_fourk_categories="$fourk_categories"
                appendCases "integration_relset" "${vc1_fourk_categories}"
                appendCases "integration_default" "${vc1_fourk_categories}"
                appendCases "integration_pbyp" "${vc1_fourk_categories}"
            else            
                appendConfigurations "integration_pbyp integration_vc1ud"
                appendCases "integration_pbyp" "vc1_adv vc1_pp_multi_inst vc1_ap"
                appendCases "integration_vc1ud" "2839 4346 4400 4401 4422 5802"
            fi
        else
            echo "VC-1 Main detected"
            appendCases "integration_relset" "-n vc1_adv vc1_ap"
            appendCases "integration_default" "-n vc1_adv vc1_ap"
        fi
    fi
    if [ -z "$vc1mp" ] && [ -z "$vc1ap" ]
    then
        exclude_formats="${exclude_formats} vc1"
    fi
   
    if [ ! -z "$rv" ] || [ ! -z "$vp6" ] || [ ! -z "$vp7" ] || [ ! -z "$vp8" ] || [ ! -z "$avs" ]
    then
        appendConfigurations "integration_relset integration_default"
        if [ ! -z "$rv" ] 
        then
            echo "RV detected"
            appendCases "integration_relset" "rv"
            appendCases "integration_default" "rv"
        fi
        if [ ! -z "$vp6" ] 
        then
            echo "VP6 detected"
            appendCases "integration_relset" "vp6"
            appendCases "integration_default" "vp6"
        fi
        if [ ! -z "$vp7" ] 
        then
            echo "VP7 detected"
            appendCases "integration_relset" "vp7"
            appendCases "integration_default" "vp7"
        fi
        if [ ! -z "$vp8" ]
        then
            echo "VP8 detected"
            
            if [ "$FOURK_CASES_ONLY" == "ENABLED" ]
            then
                fourk_categories=""
                for category in $category_list_swhw
                do
                    found_fourk=`echo "$category" |grep "vp8" |grep "_4k"`
                    if [ ! -z  "$found_fourk" ]
                    then
                        fourk_categories="${category} ${fourk_categories}"
                    fi
                done
                vp8_fourk_categories="$fourk_categories"
                appendCases "integration_relset" "${vp8_fourk_categories}"
                appendCases "integration_default" "${vp8_fourk_categories}"
            else
                appendCases "integration_relset" "vp8 webp"
                appendCases "integration_default" "vp8 webp"
            fi
            appendConfigurations "integration_memext"
            if [[ "$PRODUCT" == "g1" && "$hw_major" == "2" && "$hw_minor" -ge "70" ]] || [[ "$PRODUCT" == "g1" && "$hw_major" -ge "3" ]]
            then
                if [ "$FOURK_CASES_ONLY" == "ENABLED" ]
                then
                    appendCases "integration_memext" "${vp8_fourk_categories}"
                else
                    appendCases "integration_memext" "webp vp8"
                fi
                if [ "$HW_BUS" == "AXI" ]
                then
                    appendConfigurations "integration_stride integration_memext_stride"
                    if [ "$FOURK_CASES_ONLY" == "ENABLED" ]
                    then
                        appendCases "integration_stride" "${vp8_fourk_categories}"
                        appendCases "integration_memext_stride" "${vp8_fourk_categories}"
                    else
                        appendCases "integration_stride" "webp vp8"
                        appendCases "integration_memext_stride" "webp vp8"
                    fi
                fi
            else
                appendCases "integration_memext" "webp"
            fi
        fi
        if [ ! -z "$avs" ]
        then
            echo "AVS detected"
            appendConfigurations "integration_pbyp"
            appendCases "integration_relset" "avs"
            appendCases "integration_default" "avs"
            appendCases "integration_pbyp" "avs"
        fi
    fi
    if [ -z "$rv" ]
    then
        exclude_formats="${exclude_formats} rv"
    fi 
    if [ -z "$vp6" ]
    then
        exclude_formats="${exclude_formats} vp6"
    fi 
    if [ -z "$vp7" ]
    then
            exclude_formats="${exclude_formats} vp7"
    fi 
    if [ -z "$vp8" ]
    then
        exclude_formats="${exclude_formats} vp8 webp"
    fi 
    if [ -z "$avs" ]
    then
        exclude_formats="${exclude_formats} avs"
    fi 
   
    if [ ! -z "$pp_found" ]
    then
        appendConfigurations "integration_relset integration_default integration_mbuffer"
        if [ "$FOURK_CASES_ONLY" == "ENABLED" ]
        then
            fourk_categories=""
            for category in $category_list_swhw
            do
                found_fourk=`echo "$category" |grep "pp_" |grep "_4k" |grep -v "_pp_"`
                if [ ! -z  "$found_fourk" ]
                then
                    fourk_categories="${category} ${fourk_categories}"
                fi
            done
            appendCases "integration_relset" "${fourk_categories}"
            appendCases "integration_default" "${fourk_categories}"
            if [ ! -z "$h264_fourk_categories" ]
            then
                fourk_categories=""
                for category in $h264_fourk_categories
                do
                    found_fourk=`echo "$category" |grep "pp_"`
                    if [ ! -z  "$found_fourk" ]
                    then
                        fourk_categories="${category} ${fourk_categories}"
                    fi
                done
                appendCases "integration_mbuffer" "${fourk_categories}"
            fi
            if [ ! -z "$vc1_fourk_categories" ]
            then
                fourk_categories=""
                for category in $vc1_fourk_categories
                do
                    found_fourk=`echo "$category" |grep "pp_"`
                    if [ ! -z  "$found_fourk" ]
                    then
                        fourk_categories="${category} ${fourk_categories}"
                    fi
                done
                appendCases "integration_mbuffer" "${fourk_categories}"
            fi
            if [ ! -z "$vp8_fourk_categories" ]
            then
                fourk_categories=""
                for category in $vp8_fourk_categories
                do
                    found_fourk=`echo "$category" |grep "pp_"`
                    if [ ! -z  "$found_fourk" ]
                    then
                        fourk_categories="${category} ${fourk_categories}"
                    fi
                done
                vp8_pp_fourk_categories="$fourk_categories"
                appendCases "integration_mbuffer" "${vp8_pp_fourk_categories}"
            fi
        else
            appendCases "integration_relset" "pp"
            appendCases "integration_default" "pp"
            appendCases "integration_mbuffer" "_pp -n jpeg webp"
        fi
        if [ ! -z "$vc1mp" ]
        then
            appendCases "integration_mbuffer" "-n vc1_adv vc1_ap"
        fi
        if [ ! -z "$mpeg4sp" ]
        then
            appendCases "integration_mbuffer" "-n asp"
        fi
        if [ ! -z "$h264bp" ]
        then
            appendCases "integration_mbuffer" "-n _hp_ _mp_ _high_ _main_ _cabac_ cabac_"
        fi
        if [ ! -z "$vp8" ]
        then
            if [[ "$PRODUCT" == "g1" && "$hw_major" == "2" && "$hw_minor" -ge "70" ]] || [[ "$PRODUCT" == "g1" && "$hw_major" -ge "3" ]]
            then
                appendConfigurations "integration_mbuffer_memext"
                if [ "$FOURK_CASES_ONLY" == "ENABLED" ]
                then
                    appendCases "integration_mbuffer_memext" "${vp8_pp_fourk_categories}"
                else
                    appendCases "integration_mbuffer_memext" "vp8_pp"
                fi
            fi
            appendCases "integration_stride" "-n pp"
            appendCases "integration_memext_stride" "-n pp"
        fi
    else
        exclude_formats="${exclude_formats} pp"
    fi
    
    fourk_excluded="_4k"
    if [ "$MEM_SIZE_MB" -ge "383" ]
    then
        fourk_excluded=""
    fi
    appendCases "integration_mbuffer" "-n ${exclude_formats} ${fourk_excluded}"
    appendCases "integration_relset" "-n ${exclude_formats} ${fourk_excluded}"
    appendCases "integration_default" "-n ${exclude_formats} ${fourk_excluded}"
    appendCases "integration_vc1ud" "-n ${exclude_formats} ${fourk_excluded}"
    appendCases "integration_memext" "-n ${exclude_formats} ${fourk_excluded}"
    appendCases "integration_mbuffer_memext" "-n ${exclude_formats} ${fourk_excluded}"
    appendCases "integration_stride" "-n ${exclude_formats} ${fourk_excluded}"
    appendCases "integration_memext_stride" "-n ${exclude_formats} ${fourk_excluded}"
    appendCases "integration_pbyp" "-n ${exclude_formats} ${fourk_excluded}"
    appendCases "integration_wholestrm" "-n ${exclude_formats} ${fourk_excluded}"
    appendCases "integration_nal" "-n ${exclude_formats} ${fourk_excluded}"
    appendCases "integration_rlc" "-n ${exclude_formats} ${fourk_excluded}"
    appendCases "integration_rlc_pbyp" "-n ${exclude_formats} ${fourk_excluded}"
    appendCases "integration_rlc_nal" "-n ${exclude_formats} ${fourk_excluded}"
    appendCases "integration_rlc_wholestrm" "-n ${exclude_formats} ${fourk_excluded}"
}

# set up compile time flags

# HW base address
sed "s/#define[ \t]*HXDEC_LOGIC_MODULE0_BASE[ \t].*/#define HXDEC_LOGIC_MODULE0_BASE $HW_BASE_ADDRESS/" ../../../linux/ldriver/kernel_26x/hx170dec.c > tmp
mv tmp ../../../linux/ldriver/kernel_26x/hx170dec.c

# Memory base address
sed "s/#define[ \t]*HLINA_START_ADDRESS[ \t].*/#define HLINA_START_ADDRESS $MEM_BASE_ADDRESS/" ../../../linux/memalloc/memalloc.c > tmp
mv tmp ../../../linux/memalloc/memalloc.c

# Kernel directory for kernel modules compilation
sed "s,KDIR[ \t]*:=.*,KDIR := $KDIR," ../../../linux/memalloc/Makefile > tmp
mv tmp ../../../linux/memalloc/Makefile

sed "s,KDIR[ \t]*:=.*,KDIR := $KDIR," ../../../linux/ldriver/kernel_26x/Makefile > tmp
mv tmp ../../../linux/ldriver/kernel_26x/Makefile

# IRQ line
sed "s/#define[ \t]*VP_PB_INT_LT[ \t].*/#define VP_PB_INT_LT $IRQ_LINE/" ../../../linux/ldriver/kernel_26x/hx170dec.c > tmp
mv tmp ../../../linux/ldriver/kernel_26x/hx170dec.c

i=`expr index "$HWBASETAG" '_'`
hw_buid_string=${HWBASETAG:$i}
i=`expr index "$hw_buid_string" '_'`
let 'i-=1'
hw_major=${hw_buid_string:0:$i}
let 'i+=1'
hw_minor=${hw_buid_string:$i}

if [ "$SYSTEM_ENDIANESS" == "LITTLE_ENDIAN" ] && [ "$TEST_ENV_DATA_BUS_WIDTH" == "32_BIT" ]
then
    # decoder settings
    sed "s/#define[ \t]*DEC_X170_INPUT_STREAM_ENDIAN[ \t].*/#define DEC_X170_INPUT_STREAM_ENDIAN DEC_X170_LITTLE_ENDIAN/" ../../../source/config/deccfg.h > tmp
    mv tmp ../../../source/config/deccfg.h
    
    sed "s/#define[ \t]*DEC_X170_OUTPUT_PICTURE_ENDIAN[ \t].*/#define DEC_X170_OUTPUT_PICTURE_ENDIAN DEC_X170_LITTLE_ENDIAN/" ../../../source/config/deccfg.h > tmp
    mv tmp ../../../source/config/deccfg.h
    
    sed "s/#define[ \t]*DEC_X170_OUTPUT_SWAP_32_ENABLE[ \t].*/#define DEC_X170_OUTPUT_SWAP_32_ENABLE 0/" ../../../source/config/deccfg.h > tmp
    mv tmp ../../../source/config/deccfg.h
    
    sed "s/#define[ \t]*DEC_X170_INPUT_DATA_SWAP_32_ENABLE[ \t].*/#define DEC_X170_INPUT_DATA_SWAP_32_ENABLE 0/" ../../../source/config/deccfg.h > tmp
    mv tmp ../../../source/config/deccfg.h
    
    sed "s/#define[ \t]*DEC_X170_INPUT_STREAM_SWAP_32_ENABLE[ \t].*/#define DEC_X170_INPUT_STREAM_SWAP_32_ENABLE 0/" ../../../source/config/deccfg.h > tmp
    mv tmp ../../../source/config/deccfg.h
    
    # HOX! DEC_X170_INPUT_DATA_BIG_ENDIAN is the correct value
    # SW writes 32 bit values so the input data is set correctly by the operating system for HW and no swapping is needed
    sed "s/#define[ \t]*DEC_X170_INPUT_DATA_ENDIAN[ \t].*/#define DEC_X170_INPUT_DATA_ENDIAN DEC_X170_BIG_ENDIAN/" ../../../source/config/deccfg.h > tmp
    mv tmp ../../../source/config/deccfg.h
    
    # pp settings
	if [[ "$PRODUCT" == "9190" && "$hw_major" -ge "1" && "$hw_minor" -ge "46" ]] || [[ "$PRODUCT" == "9190" && "$hw_major" -ge "2" ]] || [[ "$PRODUCT" == "g1" && "$hw_major" -ge "1" && "$hw_minor" -ge "14" ]] || [[ "$PRODUCT" == "g1" && "$hw_major" -ge "2" ]]
	then
	    sed "s/#define[ \t]*PP_X170_SWAP_32_WORDS[ \t].*/#define PP_X170_SWAP_32_WORDS 0/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_SWAP_16_WORDS[ \t].*/#define PP_X170_SWAP_16_WORDS 0/" ../../../source/config/ppcfg.h > tmp
	    mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_SWAP_32_WORDS_RGB32[ \t].*/#define PP_X170_SWAP_32_WORDS_RGB32 0/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_SWAP_16_WORDS_RGB32[ \t].*/#define PP_X170_SWAP_16_WORDS_RGB32 0/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_SWAP_32_WORDS_RGB16[ \t].*/#define PP_X170_SWAP_32_WORDS_RGB16 0/" ../../../source/config/ppcfg.h > tmp
	    mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_SWAP_16_WORDS_RGB16[ \t].*/#define PP_X170_SWAP_16_WORDS_RGB16 1/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_INPUT_PICTURE_ENDIAN[ \t].*/#define PP_X170_INPUT_PICTURE_ENDIAN PP_X170_PICTURE_LITTLE_ENDIAN/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_OUTPUT_PICTURE_ENDIAN[ \t].*/#define PP_X170_OUTPUT_PICTURE_ENDIAN PP_X170_PICTURE_LITTLE_ENDIAN/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_OUTPUT_PICTURE_ENDIAN_RGB32[ \t].*/#define PP_X170_OUTPUT_PICTURE_ENDIAN_RGB32 PP_X170_PICTURE_BIG_ENDIAN/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_OUTPUT_PICTURE_ENDIAN_RGB16[ \t].*/#define PP_X170_OUTPUT_PICTURE_ENDIAN_RGB16 PP_X170_PICTURE_BIG_ENDIAN/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_DATA_BUS_WIDTH[ \t].*/#define PP_X170_DATA_BUS_WIDTH PP_X170_DATA_BUS_WIDTH_32/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_SWAP_32_WORDS_INPUT[ \t].*/#define PP_X170_SWAP_32_WORDS_INPUT 0/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
	else
		sed "s/#define[ \t]*PP_X170_SWAP_32_WORDS[ \t].*/#define PP_X170_SWAP_32_WORDS 0/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
	    sed "s/#define[ \t]*PP_X170_INPUT_PICTURE_ENDIAN[ \t].*/#define PP_X170_INPUT_PICTURE_ENDIAN PP_X170_PICTURE_LITTLE_ENDIAN/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_OUTPUT_PICTURE_ENDIAN[ \t].*/#define PP_X170_OUTPUT_PICTURE_ENDIAN PP_X170_PICTURE_LITTLE_ENDIAN/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_DATA_BUS_WIDTH[ \t].*/#define PP_X170_DATA_BUS_WIDTH PP_X170_DATA_BUS_WIDTH_32/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_SWAP_32_WORDS_INPUT[ \t].*/#define PP_X170_SWAP_32_WORDS_INPUT 0/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
	fi
    
elif [ "$SYSTEM_ENDIANESS" == "LITTLE_ENDIAN" ] && [ "$TEST_ENV_DATA_BUS_WIDTH" == "64_BIT" ]
then
    # decoder settings
    sed "s/#define[ \t]*DEC_X170_INPUT_STREAM_ENDIAN[ \t].*/#define DEC_X170_INPUT_STREAM_ENDIAN DEC_X170_LITTLE_ENDIAN/" ../../../source/config/deccfg.h > tmp
    mv tmp ../../../source/config/deccfg.h
    
    sed "s/#define[ \t]*DEC_X170_OUTPUT_PICTURE_ENDIAN[ \t].*/#define DEC_X170_OUTPUT_PICTURE_ENDIAN DEC_X170_LITTLE_ENDIAN/" ../../../source/config/deccfg.h > tmp
    mv tmp ../../../source/config/deccfg.h
    
    sed "s/#define[ \t]*DEC_X170_OUTPUT_SWAP_32_ENABLE[ \t].*/#define DEC_X170_OUTPUT_SWAP_32_ENABLE 1/" ../../../source/config/deccfg.h > tmp
    mv tmp ../../../source/config/deccfg.h
    
    sed "s/#define[ \t]*DEC_X170_INPUT_DATA_SWAP_32_ENABLE[ \t].*/#define DEC_X170_INPUT_DATA_SWAP_32_ENABLE 1/" ../../../source/config/deccfg.h > tmp
    mv tmp ../../../source/config/deccfg.h
    
    sed "s/#define[ \t]*DEC_X170_INPUT_STREAM_SWAP_32_ENABLE[ \t].*/#define DEC_X170_INPUT_STREAM_SWAP_32_ENABLE 1/" ../../../source/config/deccfg.h > tmp
    mv tmp ../../../source/config/deccfg.h
    
    # HOX! DEC_X170_INPUT_DATA_BIG_ENDIAN is the correct value
    # SW writes 32 bit values so the input data is set correctly by the operating system for HW and no swapping is needed
    sed "s/#define[ \t]*DEC_X170_INPUT_DATA_ENDIAN[ \t].*/#define DEC_X170_INPUT_DATA_ENDIAN DEC_X170_BIG_ENDIAN/" ../../../source/config/deccfg.h > tmp
    mv tmp ../../../source/config/deccfg.h
    
    # pp settings
    if [[ "$PRODUCT" == "9190" && "$hw_major" -ge "1" && "$hw_minor" -ge "46" ]] || [[ "$PRODUCT" == "9190" && "$hw_major" -ge "2" ]] || [[ "$PRODUCT" == "g1" && "$hw_major" -ge "1" && "$hw_minor" -ge "14" ]] || [[ "$PRODUCT" == "g1" && "$hw_major" -ge "2" ]] 
	then
		sed "s/#define[ \t]*PP_X170_SWAP_32_WORDS[ \t].*/#define PP_X170_SWAP_32_WORDS 1/" ../../../source/config/ppcfg.h > tmp
	    mv tmp ../../../source/config/ppcfg.h    	
        sed "s/#define[ \t]*PP_X170_SWAP_16_WORDS[ \t].*/#define PP_X170_SWAP_16_WORDS 0/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_SWAP_32_WORDS_RGB32[ \t].*/#define PP_X170_SWAP_32_WORDS_RGB32 1/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_SWAP_16_WORDS_RGB32[ \t].*/#define PP_X170_SWAP_16_WORDS_RGB32 0/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_SWAP_32_WORDS_RGB16[ \t].*/#define PP_X170_SWAP_32_WORDS_RGB16 1/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_SWAP_16_WORDS_RGB16[ \t].*/#define PP_X170_SWAP_16_WORDS_RGB16 1/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_INPUT_PICTURE_ENDIAN[ \t].*/#define PP_X170_INPUT_PICTURE_ENDIAN PP_X170_PICTURE_LITTLE_ENDIAN/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_OUTPUT_PICTURE_ENDIAN[ \t].*/#define PP_X170_OUTPUT_PICTURE_ENDIAN PP_X170_PICTURE_LITTLE_ENDIAN/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_OUTPUT_PICTURE_ENDIAN_RGB32[ \t].*/#define PP_X170_OUTPUT_PICTURE_ENDIAN_RGB32 PP_X170_PICTURE_BIG_ENDIAN/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_OUTPUT_PICTURE_ENDIAN_RGB16[ \t].*/#define PP_X170_OUTPUT_PICTURE_ENDIAN_RGB16 PP_X170_PICTURE_BIG_ENDIAN/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_DATA_BUS_WIDTH[ \t].*/#define PP_X170_DATA_BUS_WIDTH PP_X170_DATA_BUS_WIDTH_64/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_SWAP_32_WORDS_INPUT[ \t].*/#define PP_X170_SWAP_32_WORDS_INPUT 1/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
	else
		sed "s/#define[ \t]*PP_X170_SWAP_32_WORDS[ \t].*/#define PP_X170_SWAP_32_WORDS 1/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_INPUT_PICTURE_ENDIAN[ \t].*/#define PP_X170_INPUT_PICTURE_ENDIAN PP_X170_PICTURE_LITTLE_ENDIAN/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_OUTPUT_PICTURE_ENDIAN[ \t].*/#define PP_X170_OUTPUT_PICTURE_ENDIAN PP_X170_PICTURE_LITTLE_ENDIAN/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_DATA_BUS_WIDTH[ \t].*/#define PP_X170_DATA_BUS_WIDTH PP_X170_DATA_BUS_WIDTH_64/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
    
    	sed "s/#define[ \t]*PP_X170_SWAP_32_WORDS_INPUT[ \t].*/#define PP_X170_SWAP_32_WORDS_INPUT 1/" ../../../source/config/ppcfg.h > tmp
    	mv tmp ../../../source/config/ppcfg.h
	fi

else
    echo "BIG_ENDIAN setting for SYSTEM_ENDIANESS is not defined"
    exit 1
fi

if [ "$HW_DATA_BUS_WIDTH" == "32_BIT" ]
then    
    sed "s/#define[ \t]*DEC_X170_REFBU_WIDTH[ \t].*/#define DEC_X170_REFBU_WIDTH 32/" ../../../source/config/deccfg.h > tmp
    mv tmp ../../../source/config/deccfg.h
    
elif [ "$HW_DATA_BUS_WIDTH" == "64_BIT" ]
then    
    sed "s/#define[ \t]*DEC_X170_REFBU_WIDTH[ \t].*/#define DEC_X170_REFBU_WIDTH 64/" ../../../source/config/deccfg.h > tmp
    mv tmp ../../../source/config/deccfg.h
    
else
    echo "HW_DATA_BUS_WIDTH is invalid"
    exit 1
fi

if [ "$DWL_IMPLEMENTATION" == "IRQ" ]
then
    sed "s/USE_DEC_IRQ[ \t]*=.*/USE_DEC_IRQ = y/" ../../../linux/dwl/Makefile > tmp
    mv tmp ../../../linux/dwl/Makefile

elif [ "$DWL_IMPLEMENTATION" == "POLLING" ]
then
    sed "s/USE_DEC_IRQ[ \t]*=.*/USE_DEC_IRQ = n/" ../../../linux/dwl/Makefile > tmp
    mv tmp ../../../linux/dwl/Makefile
fi

if [ "$DEC_HW_TIMEOUT" == "ENABLED" ]
then
    sed "s/#define[ \t]*DEC_X170_HW_TIMEOUT_INT_ENA[ \t].*/#define DEC_X170_HW_TIMEOUT_INT_ENA 1/" ../../../source/config/deccfg.h > tmp
    mv tmp ../../../source/config/deccfg.h

elif [ "$DEC_HW_TIMEOUT" == "DISABLED" ]
then
    sed "s/#define[ \t]*DEC_X170_HW_TIMEOUT_INT_ENA[ \t].*/#define DEC_X170_HW_TIMEOUT_INT_ENA 0/" ../../../source/config/deccfg.h > tmp
    mv tmp ../../../source/config/deccfg.h
fi

if [ "$BUILD_IN_TEST" == "ENABLED" ]
then
    if [ "$PRODUCT" == "8170" ]
    then
        sed "s/DEBUG[ \t]*=.*/DEBUG = y/" ../../../linux/h264/Makefile > tmp
        mv tmp ../../../linux/h264/Makefile
     
    elif [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ] 
    then
        sed "s/DEBUG[ \t]*=.*/DEBUG = y/" ../../../linux/h264high/Makefile > tmp
        mv tmp ../../../linux/h264high/Makefile
    fi
    
    sed "s/DEBUG[ \t]*=.*/DEBUG = y/" ../../../linux/mpeg4/Makefile > tmp
    mv tmp ../../../linux/mpeg4/Makefile
    
    sed "s/DEBUG[ \t]*=.*/DEBUG = y/" ../../../linux/jpeg/Makefile > tmp
    mv tmp ../../../linux/jpeg/Makefile
    
    sed "s/DEBUG[ \t]*=.*/DEBUG = y/" ../../../linux/vc1/Makefile > tmp
    mv tmp ../../../linux/vc1/Makefile
    
    sed "s/DEBUG[ \t]*=.*/DEBUG = y/" ../../../linux/mpeg2/Makefile > tmp
    mv tmp ../../../linux/mpeg2/Makefile
    
    if [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ]  || [ "$PRODUCT" == "g1" ] 
    then
        sed "s/DEBUG[ \t]*=.*/DEBUG = y/" ../../../linux/rv/Makefile > tmp
        mv tmp ../../../linux/rv/Makefile
    fi
    
    if [ "$PRODUCT" == "9190" ]  || [ "$PRODUCT" == "g1" ] 
    then
        sed "s/DEBUG[ \t]*=.*/DEBUG = y/" ../../../linux/vp6/makefile > tmp
        mv tmp ../../../linux/vp6/makefile
    fi
    
    if [ "$PRODUCT" == "g1" ] 
    then
        sed "s/DEBUG[ \t]*=.*/DEBUG = y/" ../../../linux/vp8/Makefile > tmp
        mv tmp ../../../linux/vp8/Makefile
        
        sed "s/DEBUG[ \t]*=.*/DEBUG = y/" ../../../linux/avs/Makefile > tmp
        mv tmp ../../../linux/avs/Makefile
    fi
    
    sed "s/DEBUG[ \t]*=.*/DEBUG = y/" ../../../linux/pp/Makefile > tmp
    mv tmp ../../../linux/pp/Makefile

elif [ "$BUILD_IN_TEST" == "DISABLED" ]
then
    if [ "$PRODUCT" == "8170" ]
    then
        sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/h264/Makefile > tmp
        mv tmp ../../../linux/h264/Makefile
    
    elif [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
    then
        sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/h264high/Makefile > tmp
        mv tmp ../../../linux/h264high/Makefile
    fi
    
    sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/mpeg4/Makefile > tmp
    mv tmp ../../../linux/mpeg4/Makefile
    
    sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/jpeg/Makefile > tmp
    mv tmp ../../../linux/jpeg/Makefile
    
    sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/vc1/Makefile > tmp
    mv tmp ../../../linux/vc1/Makefile
    
    sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/mpeg2/Makefile > tmp
    mv tmp ../../../linux/mpeg2/Makefile
    
    if [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
    then
        sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/rv/Makefile > tmp
        mv tmp ../../../linux/rv/Makefile
    fi
    
    if [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
    then
        sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/vp6/makefile > tmp
        mv tmp ../../../linux/vp6/makefile
    fi
    
    if [ "$PRODUCT" == "g1" ]
    then
        sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/vp8/Makefile > tmp
        mv tmp ../../../linux/vp8/Makefile
        
        sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/avs/Makefile > tmp
        mv tmp ../../../linux/avs/Makefile
    fi
    
    sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/pp/Makefile > tmp
    mv tmp ../../../linux/pp/Makefile

fi

if [ "$TB_INTERNAL_TEST" == "ENABLED" ]
then
    sed "s/INTERNAL_TEST[ \t]*=.*/INTERNAL_TEST = y/" ../../../linux/dwl/Makefile > tmp
    mv tmp ../../../linux/dwl/Makefile

elif [ "$TB_INTERNAL_TEST" == "DISABLED" ]
then
    sed "s/INTERNAL_TEST[ \t]*=.*/INTERNAL_TEST = n/" ../../../linux/dwl/Makefile > tmp
    mv tmp ../../../linux/dwl/Makefile
fi

if [ "$TB_MD5SUM" == "ENABLED" ]
then
    if [ "$PRODUCT" == "8170" ]
    then
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../h264/Makefile > tmp
        mv tmp ../../h264/Makefile
    
    elif [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
    then
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../h264high/Makefile > tmp
        mv tmp ../../h264high/Makefile
    fi
    
    sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../mpeg4/Makefile > tmp
    mv tmp ../../mpeg4/Makefile
    
    # JPEG uses YUV
    #sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../jpeg/Makefile > tmp
    #mv tmp ../../jpeg/Makefile
    
    sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../vc1/Makefile > tmp
    mv tmp ../../vc1/Makefile
    
    sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../mpeg2/Makefile > tmp
    mv tmp ../../mpeg2/Makefile
    
    if [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
    then
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../rv/Makefile > tmp
        mv tmp ../../rv/Makefile
    fi
    
    if [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
    then
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../vp6/makefile > tmp
        mv tmp ../../vp6/makefile
    fi
    
    if [ "$PRODUCT" == "g1" ]
    then
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../vp8/Makefile > tmp
        mv tmp ../../vp8/Makefile
        
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../avs/Makefile > tmp
        mv tmp ../../avs/Makefile
    fi
    
    sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/external_mode/Makefile > tmp
    mv tmp ../../pp/external_mode/Makefile
    
    if [ "$PRODUCT" == "8170" ]
    then
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/h264_pipeline_mode/Makefile > tmp
        mv tmp ../../pp/h264_pipeline_mode/Makefile
        
    elif [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
    then
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/h264high_pipeline_mode/Makefile > tmp
        mv tmp ../../pp/h264high_pipeline_mode/Makefile
    fi
            
    sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/mpeg4_pipeline_mode/Makefile > tmp
    mv tmp ../../pp/mpeg4_pipeline_mode/Makefile
            
    sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/jpeg_pipeline_mode/Makefile > tmp
    mv tmp ../../pp/jpeg_pipeline_mode/Makefile
            
    sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/vc1_pipeline_mode/Makefile > tmp
    mv tmp ../../pp/vc1_pipeline_mode/Makefile
            
    sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/mpeg2_pipeline_mode/Makefile > tmp
    mv tmp ../../pp/mpeg2_pipeline_mode/Makefile
    
    if [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
    then
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/rv_pipeline_mode/Makefile > tmp
        mv tmp ../../pp/rv_pipeline_mode/Makefile
    fi
    
    if [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
    then
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/vp6_pipeline_mode/Makefile > tmp
        mv tmp ../../pp/vp6_pipeline_mode/Makefile
    fi
    
    if [ "$PRODUCT" == "g1" ]
    then
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/vp8_pipeline_mode/Makefile > tmp
        mv tmp ../../pp/vp8_pipeline_mode/Makefile
        
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/avs_pipeline_mode/Makefile > tmp
        mv tmp ../../pp/avs_pipeline_mode/Makefile
    fi

elif [ "$TB_MD5SUM" == "DISABLED" ]
then
    if [ "$PRODUCT" == "8170" ]
    then
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../h264/Makefile > tmp
        mv tmp ../../h264/Makefile
     
    elif [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
    then
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../h264high/Makefile > tmp
        mv tmp ../../h264high/Makefile
    fi
    
    sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../mpeg4/Makefile > tmp
    mv tmp ../../mpeg4/Makefile
    
    # JPEG uses YUV
    #sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../jpeg/Makefile > tmp
    #mv tmp ../../jpeg/Makefile
    
    sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../vc1/Makefile > tmp
    mv tmp ../../vc1/Makefile
    
    sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../mpeg2/Makefile > tmp
    mv tmp ../../mpeg2/Makefile
    
    if [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
    then
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../rv/Makefile > tmp
        mv tmp ../../rv/Makefile
    fi
    
    if [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
    then
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../vp6/makefile > tmp
        mv tmp ../../vp6/makefile
    fi
    
    if [ "$PRODUCT" == "g1" ]
    then
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../vp8/Makefile > tmp
        mv tmp ../../vp8/Makefile
        
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../avs/Makefile > tmp
        mv tmp ../../avs/Makefile
    fi
    
    sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/external_mode/Makefile > tmp
    mv tmp ../../pp/external_mode/Makefile
    
    if [ "$PRODUCT" == "8170" ]
    then
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/h264_pipeline_mode/Makefile > tmp
        mv tmp ../../pp/h264_pipeline_mode/Makefile
        
    elif  [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
    then
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/h264high_pipeline_mode/Makefile > tmp
        mv tmp ../../pp/h264high_pipeline_mode/Makefile
    fi
            
    sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/mpeg4_pipeline_mode/Makefile > tmp
    mv tmp ../../pp/mpeg4_pipeline_mode/Makefile
            
    sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/jpeg_pipeline_mode/Makefile > tmp
    mv tmp ../../pp/jpeg_pipeline_mode/Makefile
            
    sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/vc1_pipeline_mode/Makefile > tmp
    mv tmp ../../pp/vc1_pipeline_mode/Makefile
            
    sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/mpeg2_pipeline_mode/Makefile > tmp
    mv tmp ../../pp/mpeg2_pipeline_mode/Makefile
    
    if [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
    then
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/rv_pipeline_mode/Makefile > tmp
        mv tmp ../../pp/rv_pipeline_mode/Makefile
    fi
    
    if [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
    then
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/vp6_pipeline_mode/Makefile > tmp
        mv tmp ../../pp/vp6_pipeline_mode/Makefile
    fi
    
    if [ "$PRODUCT" == "g1" ]
    then
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/vp8_pipeline_mode/Makefile > tmp
        mv tmp ../../pp/vp8_pipeline_mode/Makefile
        
        sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/avs_pipeline_mode/Makefile > tmp
        mv tmp ../../pp/avs_pipeline_mode/Makefile
    fi
fi

# set-up test data generation
if [ "$TB_TEST_DATA_SOURCE" == "SYSTEM" ]
then
    echo -n "Searching test data generation (${SWTAG})... "
        
    if [ -e tb_md5sum_enabled ] && [ "$TB_MD5SUM" == "DISABLED" ]
    then
        rm -rf "$SWTAG"
         
    elif [ -e tb_md5sum_disabled ] && [ "$TB_MD5SUM" == "ENABLED" ]
    then
        rm -rf "$SWTAG" 
    fi
    
    if [ -e tb_internal_test_enabled ] && [ "$TB_INTERNAL_TEST" == "DISABLED" ]
    then
        rm -rf "$SWTAG"
        
    elif [ -e tb_internal_test_disabled ] && [ "$TB_INTERNAL_TEST" == "ENABLED" ]
    then
        rm -rf "$SWTAG" 
    fi
    
    #divx_configuration=`echo "$HWCONFIGTAG" | grep divx`
    #if [ -e tb_custom_fmt_enabled ] && [ -z "$divx_configuration" ]
    #then
    #    rm -rf "$SWTAG"
    #    
    #elif [ -e tb_custom_fmt_test_disabled ] && [ ! -z "$divx_configuration" ]
    #then
    #    rm -rf "$SWTAG" 
    #fi
    
    if [ -e tb_2nd_chroma_table_enabled ] && [ "$DEC_2ND_CHROMA_TABLE" == "DISABLED" ]
    then
        rm -rf "$SWTAG"
         
    elif [ -e tb_2nd_chroma_table_disabled ] && [ "$DEC_2ND_CHROMA_TABLE" == "ENABLED" ]
    then
        rm -rf "$SWTAG" 
    fi
    
    if [ ! -d "$SWTAG" ]
    then
        echo "NOK"
        echo -n "    -> Setting up test data generation (${SWTAG})... "
        wdir="$PWD"
        mkdir "${SWTAG}"
        cd "${SWTAG}"

        if  [ -d "$USE_DIR_MODEL" ]
        then
            if [ -d "${USE_DIR_MODEL}/software/" ]
            then
                cp -r ${USE_DIR_MODEL}/software/ .
            else
                echo "FAILED: cannot find software directory"
                cd "$wdir"
                rm -rf "${SWTAG}"
                exit 1
            fi
            
            if [ -d "${USE_DIR_MODEL}/system/" ]
            then
                cp -r ${USE_DIR_MODEL}/system/ .
            else
                echo "FAILED: cannot find system directory"
                cd "$wdir"
                rm -rf "${SWTAG}"
                exit 1
            fi

        elif [ "$USE_DIR_MODEL" == "DISABLED" ]
	    then
	    i=`expr index "$SWTAG" '_'`
            sw_build_string=${SWTAG:$i}
            i=`expr index "$sw_build_string" '_'`
            let 'i-=1'
            sw_major=${sw_build_string:0:$i}
            let 'i+=1'
            sw_build_string=${sw_build_string:$i}
            i=`expr index "$sw_build_string" '_'`
            let 'i-=1'
            sw_minor=${sw_build_string:0:$i}
        
            if [ "$sw_major" -eq 0 ] && [ "$sw_minor" -le 109 ]
            then
                cvs -Q -d /export/cvs/ co -r "$SWTAG" 8170_decoder
                mv 8170_decoder/software .
                mv 8170_decoder/system .
                rm -rf 8170_decoder
            else
                # copy sources from current directory tree        
                # copy stuff to tmp directory            
                temp_dir=`mktemp -d`
                if [ -z "$temp_dir" ]
                then
                    echo "FAILED: cannot create tmp directory"
                    cd "$wdir"
                    rm -rf "${SWTAG}"
                    exit 1
                fi
                cp -r ../../../../../software/ "$temp_dir"
                if [ -d "../../../../../system/" ]
                then
                    cp -r ../../../../../system/ "$temp_dir"
                else
                    echo "FAILED: failed cannot find system directory"
                    cd "$wdir"
                    rm -rf "${SWTAG}"
                    exit 1
                fi
                # move the stuff to "${SWTAG}"
                mv "$temp_dir/software" .
                mv "$temp_dir/system" .
            fi
        else
            echo "FAILED: don't know hot to fetch system model"
            cd "$wdir"
            rm -rf "${SWTAG}"
            exit 1
        fi
            
        cd software/test/scripts/swhw
        # write PRODUCT to model config
        sed "s/PRODUCT=.*/PRODUCT=\"${PRODUCT}\"/" commonconfig.sh > tmp
        mv tmp commonconfig.sh
        # write HWCONFIGTAG to model config
        sed "s/HWCONFIGTAG=.*/HWCONFIGTAG=\"${HWCONFIGTAG}\"/" commonconfig.sh > tmp
        mv tmp commonconfig.sh
        # write DEC_2ND_CHROMA_TABLE to model config
        sed "s/DEC_2ND_CHROMA_TABLE=.*/DEC_2ND_CHROMA_TABLE=\"${DEC_2ND_CHROMA_TABLE}\"/" commonconfig.sh > tmp
        mv tmp commonconfig.sh
        if [ "$TB_MD5SUM" == "ENABLED" ]
        then
            if [ "$PRODUCT" == "8170" ]
            then
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../h264/Makefile > tmp
                mv tmp ../../h264/Makefile
            
            elif [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
            then
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../h264high/Makefile > tmp
                mv tmp ../../h264high/Makefile
            fi
    
            sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../mpeg4/Makefile > tmp
            mv tmp ../../mpeg4/Makefile
    
            # JPEG uses YUV
            #sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../jpeg/Makefile > tmp
            #mv tmp ../../jpeg/Makefile
    
            sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../vc1/Makefile > tmp
            mv tmp ../../vc1/Makefile
    
            sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../mpeg2/Makefile > tmp
            mv tmp ../../mpeg2/Makefile
            
            if [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
            then
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../rv/Makefile > tmp
                mv tmp ../../rv/Makefile
            fi
            
            if [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
            then
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../vp6/makefile > tmp
                mv tmp ../../vp6/makefile
            fi
            
            if [ "$PRODUCT" == "g1" ]
            then
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../vp8/Makefile > tmp
                mv tmp ../../vp8/Makefile
                
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../avs/Makefile > tmp
                mv tmp ../../avs/Makefile
            fi
    
            sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/external_mode/Makefile > tmp
            mv tmp ../../pp/external_mode/Makefile
            
            if [ "$PRODUCT" == "8170" ]
            then
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/h264_pipeline_mode/Makefile > tmp
                mv tmp ../../pp/h264_pipeline_mode/Makefile
                
            elif [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
            then
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/h264high_pipeline_mode/Makefile > tmp
                mv tmp ../../pp/h264high_pipeline_mode/Makefile
            fi
            
            sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/mpeg4_pipeline_mode/Makefile > tmp
            mv tmp ../../pp/mpeg4_pipeline_mode/Makefile
            
            sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/jpeg_pipeline_mode/Makefile > tmp
            mv tmp ../../pp/jpeg_pipeline_mode/Makefile
            
            sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/vc1_pipeline_mode/Makefile > tmp
            mv tmp ../../pp/vc1_pipeline_mode/Makefile
            
            sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/mpeg2_pipeline_mode/Makefile > tmp
            mv tmp ../../pp/mpeg2_pipeline_mode/Makefile
            
            if [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
            then
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/rv_pipeline_mode/Makefile > tmp
                mv tmp ../../pp/rv_pipeline_mode/Makefile
            fi
            
            if [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
            then
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/vp6_pipeline_mode/Makefile > tmp
                mv tmp ../../pp/vp6_pipeline_mode/Makefile
            fi
            
            if [ "$PRODUCT" == "g1" ]
            then
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/vp8_pipeline_mode/Makefile > tmp
                mv tmp ../../pp/vp8_pipeline_mode/Makefile
                
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = y/" ../../pp/avs_pipeline_mode/Makefile > tmp
                mv tmp ../../pp/avs_pipeline_mode/Makefile
            fi

        elif [ "$TB_MD5SUM" == "DISABLED" ]
        then
            if [ "$PRODUCT" == "8170" ]
            then
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../h264/Makefile > tmp
                mv tmp ../../h264/Makefile
            
            elif [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
            then
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../h264high/Makefile > tmp
                mv tmp ../../h264high/Makefile
            
            fi
                
            sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../mpeg4/Makefile > tmp
            mv tmp ../../mpeg4/Makefile
    
            # JPEG uses YUV
            #sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../jpeg/Makefile > tmp
            #mv tmp ../../jpeg/Makefile
    
            sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../vc1/Makefile > tmp
            mv tmp ../../vc1/Makefile
    
            sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../mpeg2/Makefile > tmp
            mv tmp ../../mpeg2/Makefile
            
            if [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
            then
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../rv/Makefile > tmp
                mv tmp ../../rv/Makefile
            fi
            
            if [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
            then
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../vp6/makefile > tmp
                mv tmp ../../vp6/makefile
            fi
            
            if [ "$PRODUCT" == "g1" ]
            then
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../vp8/Makefile > tmp
                mv tmp ../../vp8/Makefile
                
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../avs/Makefile > tmp
                mv tmp ../../avs/Makefile
            fi
    
            sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/external_mode/Makefile > tmp
            mv tmp ../../pp/external_mode/Makefile
            
            if [ "$PRODUCT" == "8170" ]
            then
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/h264_pipeline_mode/Makefile > tmp
                mv tmp ../../pp/h264_pipeline_mode/Makefile
                
            elif [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
            then
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/h264high_pipeline_mode/Makefile > tmp
                mv tmp ../../pp/h264high_pipeline_mode/Makefile
            fi
            
            sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/mpeg4_pipeline_mode/Makefile > tmp
            mv tmp ../../pp/mpeg4_pipeline_mode/Makefile
            
            sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/jpeg_pipeline_mode/Makefile > tmp
            mv tmp ../../pp/jpeg_pipeline_mode/Makefile
            
            sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/vc1_pipeline_mode/Makefile > tmp
            mv tmp ../../pp/vc1_pipeline_mode/Makefile
            
            sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/mpeg2_pipeline_mode/Makefile > tmp
            mv tmp ../../pp/mpeg2_pipeline_mode/Makefile
            
            if [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
            then
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/rv_pipeline_mode/Makefile > tmp
                mv tmp ../../pp/rv_pipeline_mode/Makefile
            fi
            
            if [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
            then
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/vp6_pipeline_mode/Makefile > tmp
                mv tmp ../../pp/vp6_pipeline_mode/Makefile
            fi
            
            if [ "$PRODUCT" == "g1" ]
            then
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/vp8_pipeline_mode/Makefile > tmp
                mv tmp ../../pp/vp8_pipeline_mode/Makefile
                
                sed "s/USE_MD5SUM[ \t]*=.*/USE_MD5SUM = n/" ../../pp/avs_pipeline_mode/Makefile > tmp
                mv tmp ../../pp/avs_pipeline_mode/Makefile
            fi
        fi
        
        # make sure that built in tests are disabled in the model control SW
        if [ "$PRODUCT" == "8170" ]
        then
            sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/h264/Makefile > tmp
            mv tmp ../../../linux/h264/Makefile
        
        elif [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
        then
            sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/h264high/Makefile > tmp
            mv tmp ../../../linux/h264high/Makefile
        fi
    
        sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/mpeg4/Makefile > tmp
        mv tmp ../../../linux/mpeg4/Makefile
    
        sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/jpeg/Makefile > tmp
        mv tmp ../../../linux/jpeg/Makefile
    
        sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/vc1/Makefile > tmp
        mv tmp ../../../linux/vc1/Makefile
    
        sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/mpeg2/Makefile > tmp
        mv tmp ../../../linux/mpeg2/Makefile
        
        if [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
        then
            sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/rv/Makefile > tmp
            mv tmp ../../../linux/rv/Makefile
        fi
        
        if [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
        then
            sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/vp6/makefile > tmp
            mv tmp ../../../linux/vp6/makefile
        fi
        
        if [ "$PRODUCT" == "g1" ]
        then
            sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/vp8/Makefile > tmp
            mv tmp ../../../linux/vp8/Makefile
            
            sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/avs/Makefile > tmp
            mv tmp ../../../linux/avs/Makefile
        fi
    
        sed "s/DEBUG[ \t]*=.*/DEBUG = n/" ../../../linux/pp/Makefile > tmp
        mv tmp ../../../linux/pp/Makefile
        
        if [ "$TB_INTERNAL_TEST" == "ENABLED" ]
        then
            sed "s/INTERNAL_TEST[ \t]*=.*/INTERNAL_TEST = y/" ../../../linux/dwl/Makefile > tmp
            mv tmp ../../../linux/dwl/Makefile

        elif [ "$TB_INTERNAL_TEST" == "DISABLED" ]
        then
            sed "s/INTERNAL_TEST[ \t]*=.*/INTERNAL_TEST = n/" ../../../linux/dwl/Makefile > tmp
            mv tmp ../../../linux/dwl/Makefile
        fi
        cp ../../../../../../../../source/config/deccfg.h ../../../source/config/
        cp ../../../../../../../../source/config/ppcfg.h ../../../source/config/
        ./make_all.sh pclinux >> /dev/null 2>&1
        cd "$wdir"
        echo "OK"
            
    elif [ -d "$SWTAG" ]
    then
        echo "OK"
    fi
fi

# create the test case list
#if [ "$PRODUCT" == "8190" ] || [ "$PRODUCT" == "9170" ] || [ "$PRODUCT" == "9190" ] || [ "$PRODUCT" == "g1" ]
#then
#    cp "$TEST_DATA_HOME}/incremental_regression_list.sh" .
#    cp "$TEST_DATA_HOME}/external_cats.sh" .
#    # what is this
#    #cd - >> /dev/null
#    rm -f testcase_list.${PRODUCT}
#    cat "$TEST_CASE_LIST" incremental_regression_list.sh >> testcase_list.${PRODUCT}
#    cat external_cats.sh >> testcase_list.${PRODUCT}
#fi

cp "$TEST_CASE_LIST" .
    
# store the compilation status for recompilation of the model
if [ "$TB_MD5SUM" == "DISABLED" ]
then
    rm -rf tb_md5sum_enabled
    touch tb_md5sum_disabled
    
elif [ "$TB_MD5SUM" == "ENABLED" ]
then
    rm -rf tb_md5sum_disabled
    touch tb_md5sum_enabled
fi

if [ "$TB_INTERNAL_TEST" == "DISABLED" ]
then
    rm -rf tb_internal_test_enabled
    touch tb_internal_test_disabled
    
elif [ "$TB_INTERNAL_TEST" == "ENABLED" ]
then
    rm -rf tb_internal_test_disabled
    touch tb_internal_test_enabled
fi

#divx_configuration=`echo "$HWCONFIGTAG" | grep divx`
#if [ -z "$divx_configuration" ]
#then
#    rm -rf tb_custom_fmt_test_enabled
#    touch tb_custom_fmt_test_disabled
#        
#elif [ ! -z "$divx_configuration" ]
#then
#    rm -rf tb_custom_fmt_test_disabled
#    touch tb_custom_fmt_test_enabled
#fi
    
if  [ "$DEC_2ND_CHROMA_TABLE" == "DISABLED" ]
then
    rm -rf tb_2nd_chroma_table_enabled
    touch tb_2nd_chroma_table_disabled
         
elif [ "$DEC_2ND_CHROMA_TABLE" == "ENABLED" ]
then
    rm -rf tb_2nd_chroma_table_disabled
    touch tb_2nd_chroma_table_enabled
fi

# remove the previous test execution status
rm -f test_exec_status

# create directory for test reports
rm -rf test_reports
mkdir test_reports

# generate scripst (initial phase)
rm -f loadmodules.sh
touch loadmodules.sh
chmod 755 loadmodules.sh
echo "#!/bin/bash" >>  loadmodules.sh
echo "" >>  loadmodules.sh

if [ "$MEM_SIZE_MB" -eq 0 ]
then
    alloc_method="alloc_method=0"
    h264bp=`echo $HWCONFIGTAG | grep h264bp`
    h264hp=`echo $HWCONFIGTAG | grep h264hp`
    i=`expr index "$HWBASETAG" '_'`
    hw_buid_string=${HWBASETAG:$i}
    i=`expr index "$hw_buid_string" '_'`
    let 'i-=1'
    hw_major=${hw_buid_string:0:$i}
    let 'i+=1'
    hw_minor=${hw_buid_string:$i}
    mvc=""
    if [[ "$PRODUCT" == "g1" && "$hw_major" == "2" && "$hw_minor" -ge "15" ]] || [[ "$PRODUCT" == "g1" && "$hw_major" -ge 3 ]]
    then
        mvc=`echo $HWCONFIGTAG | grep mvc`
    elif [ ! -z "$h264hp" ]
    then
        mvc="mvc"
    else
        mvc=""
    fi
    webp=`echo $HWCONFIGTAG | tr "_" "\n" | grep vp | grep 8`
    if [ ! -z "$mvc" ]
    then
        alloc_method="alloc_method=4"
    
    elif [ ! -z "$webp"  ]
    then
        alloc_method="alloc_method=3"
    fi
else
    alloc_method="alloc_method=6 alloc_size=${MEM_SIZE_MB}"
fi

# memmalloc path, load script & module
MEMALLOC_PATH="${PWD}/../../../linux/memalloc"
MEMALLOC_LOAD_SCRIPT="./memalloc_load.sh"
MEMALLOC_MODULE="memalloc"

# ldriver path, load script & module
LDRIVER_PATH="${PWD}/../../../linux/ldriver/kernel_26x"
LDRIVER_LOAD_SCRIPT="./driver_load.sh"
LDRIVER_MODULE="hx170dec"

if [ "$TB_REMOTE_TEST_EXECUTION_IP" == "LOCAL_HOST" ]
then
    echo 'wdir=$PWD' >>  loadmodules.sh
    echo "rm -rf /tmv/dev/*" >>  loadmodules.sh
    echo "/sbin/rmmod memalloc" >>  loadmodules.sh
    echo "cd ../../../linux/memalloc && ./memalloc_load.sh ${alloc_method}" >>  loadmodules.sh
    echo "/sbin/rmmod hx170dec" >>  loadmodules.sh
    echo "cd ../ldriver/kernel_26x/ && ./driver_load.sh" >>  loadmodules.sh
    echo 'cd $wdir' >>  loadmodules.sh
    echo "" >>  loadmodules.sh
else
    echo "rsh "root@${TB_REMOTE_TEST_EXECUTION_IP}" \"/sbin/rmmod memalloc\"" >>  loadmodules.sh
    echo "rsh "root@${TB_REMOTE_TEST_EXECUTION_IP}" \"/sbin/rmmod hx170dec\"" >>  loadmodules.sh
    echo "rsh "root@${TB_REMOTE_TEST_EXECUTION_IP}" \"rm -rf /tmp/dev/*\"" >>  loadmodules.sh
    echo -n "rsh root@${TB_REMOTE_TEST_EXECUTION_IP} "  >>  loadmodules.sh
    wdir="$PWD"
    echo "\"cd ${wdir}/../../../linux/memalloc && ./memalloc_load.sh ${alloc_method}\""  >>  loadmodules.sh
    echo -n "rsh root@${TB_REMOTE_TEST_EXECUTION_IP} "  >>  loadmodules.sh
    echo "\"cd ${wdir}/../../../linux/ldriver/kernel_26x/ && ./driver_load.sh\""  >>  loadmodules.sh
fi

rm -f runall.sh
touch runall.sh
chmod 755 runall.sh
echo "#!/bin/bash" >>  runall.sh
echo "" >>  runall.sh

echo "rm -f run_done" >>  runall.sh
echo "" >>  runall.sh

echo "./loadmodules.sh" >>  runall.sh
echo "" >>  runall.sh

echo "configuration=0" >>  runall.sh
echo "if [ -e test_exec_status ]"  >>  runall.sh
echo "then"  >>  runall.sh
echo '    configuration=`tail -n 1 test_exec_status`' >>  runall.sh
echo "fi"  >>  runall.sh

rm -f checkall.sh
touch checkall.sh
chmod 755 checkall.sh
echo "#!/bin/bash" >>  checkall.sh
echo "" >>  checkall.sh

# set insmod path to load scripts
sbin_insmod=`grep /sbin/insmod ${MEMALLOC_PATH}/${MEMALLOC_LOAD_SCRIPT}`
if [ -z "$sbin_insmod" ]
then
    sed "s/insmod/\/\sbin\/insmod/" ${MEMALLOC_PATH}/${MEMALLOC_LOAD_SCRIPT} > tmp
    mv tmp ${MEMALLOC_PATH}/${MEMALLOC_LOAD_SCRIPT}
fi
cd "$MEMALLOC_PATH" && chmod 755 "$MEMALLOC_LOAD_SCRIPT"
cd - >> /dev/null

sbin_insmod=`grep /sbin/insmod ${LDRIVER_PATH}/${LDRIVER_LOAD_SCRIPT}`
if [ -z "$sbin_insmod" ]
then
    sed "s/insmod/\/\sbin\/insmod/" ${LDRIVER_PATH}/${LDRIVER_LOAD_SCRIPT} > tmp
    mv tmp ${LDRIVER_PATH}/${LDRIVER_LOAD_SCRIPT}
fi
cd "$LDRIVER_PATH" && chmod 755 "$LDRIVER_LOAD_SCRIPT"
cd - >> /dev/null

# choose configurations according to HW configuration
if [ "$TEST_CASE_SELECTION" == "HW_CONFIG" ]
then
    setConfigurations
fi

# write configuration directories and update runall.sh & check.all script
for configuration in $CONFIGURATIONS
do
    echo -n "Setting up $configuration... "
    
    # set up default for all configuration
    
    # create directory
    rm -rf $configuration
    mkdir $configuration
    cd $configuration
    if [ $? != 0 ]
    then
        echo "FAILED"
	    exit 1
    fi
    
    # set up scripts
    ln -s ../f_run.sh .
    ln -s ../f_check.sh .
    ln -s ../f_utils.sh .
    ln -s ../checktest.sh .
    ln -s ../runtest.sh .
    ln -s ../visualcmp.sh .
    ln -s ../rexecruntest.sh .
    ln -s ../monitor.sh .
    ln -s ../commonconfig.sh .
    if [ "$TB_TEST_DB" == "ENABLED" ]
    then
        ln -s ../f_testdb.sh .
    fi
    
    # write common parameters for all the configurations
    rm -f config.sh
    cp ../config.sh tmpcfg
    
    # write PRODUCT
    sed "s/export PRODUCT=.*/export PRODUCT=\"${PRODUCT}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write SWTAG
    sed "s/export SWTAG=.*/export SWTAG=\"${SWTAG}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write HWBASETAG
    sed "s/export HWBASETAG=.*/export HWBASETAG=\"${HWBASETAG}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write HWCONFIGTAG
    sed "s/export HWCONFIGTAG=.*/export HWCONFIGTAG=\"${HWCONFIGTAG}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write DEC_HW_TIMEOUT
    sed "s/export DEC_HW_TIMEOUT=.*/export DEC_HW_TIMEOUT=\"${DEC_HW_TIMEOUT}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write DEC_REFERENCE_BUFFER
    sed "s/export DEC_REFERENCE_BUFFER=.*/export DEC_REFERENCE_BUFFER=\"${DEC_REFERENCE_BUFFER}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write DEC_REFERENCE_BUFFER_INTERLACED
    sed "s/export DEC_REFERENCE_BUFFER_INTERLACED=.*/export DEC_REFERENCE_BUFFER_INTERLACED=\"${DEC_REFERENCE_BUFFER_INTERLACED}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write DEC_REFERENCE_BUFFER_TOP_BOT_SUM
    sed "s/export DEC_REFERENCE_BUFFER_TOP_BOT_SUM=.*/export DEC_REFERENCE_BUFFER_TOP_BOT_SUM=\"${DEC_REFERENCE_BUFFER_TOP_BOT_SUM}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write DEC_2ND_CHROMA_TABLE
    sed "s/export DEC_2ND_CHROMA_TABLE=.*/export DEC_2ND_CHROMA_TABLE=\"${DEC_2ND_CHROMA_TABLE}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write DEC_TILED_MODE
    sed "s/export DEC_TILED_MODE=.*/export DEC_TILED_MODE=\"${DEC_TILED_MODE}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    ## write DEC_STANDARD_COMPLIANCE
    #sed "s/export DEC_STANDARD_COMPLIANCE=.*/export DEC_STANDARD_COMPLIANCE=\"${DEC_STANDARD_COMPLIANCE}\"/" tmpcfg > tmp
    #mv tmp tmpcfg
    
    # write TB_REMOTE_TEST_EXECUTION_IP
    sed "s/export TB_REMOTE_TEST_EXECUTION_IP=.*/export TB_REMOTE_TEST_EXECUTION_IP=\"${TB_REMOTE_TEST_EXECUTION_IP}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write TB_TEST_DATA_SOURCE
    sed "s/export TB_TEST_DATA_SOURCE=.*/export TB_TEST_DATA_SOURCE=\"${TB_TEST_DATA_SOURCE}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write TB_MD5SUM
    sed "s/export TB_MD5SUM=.*/export TB_MD5SUM=\"${TB_MD5SUM}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write PP_MAX_OUTPUT
    sed "s/export PP_MAX_OUTPUT=.*/export PP_MAX_OUTPUT=\"${PP_MAX_OUTPUT}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write DEC_MAX_OUTPUT
    sed "s/export DEC_MAX_OUTPUT=.*/export DEC_MAX_OUTPUT=\"${DEC_MAX_OUTPUT}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write TB_MAX_DEC_OUTPUT
    sed "s/export TB_INTERNAL_TEST=.*/export TB_INTERNAL_TEST=\"${TB_INTERNAL_TEST}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write TB_MAX_DEC_OUTPUT
    sed "s/export TB_MAX_DEC_OUTPUT=.*/export TB_MAX_DEC_OUTPUT=\"${TB_MAX_DEC_OUTPUT}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write TB_MAX_PP_OUTPUT
    sed "s/export TB_MAX_PP_OUTPUT=.*/export TB_MAX_PP_OUTPUT=\"${TB_MAX_PP_OUTPUT}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write TB_SEED_RND
    sed "s/export TB_SEED_RND=.*/export TB_SEED_RND=\"${TB_SEED_RND}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write TB_STREAM_BIT_SWAP
    sed "s/export TB_STREAM_BIT_SWAP=.*/export TB_STREAM_BIT_SWAP=\"${TB_STREAM_BIT_SWAP}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write TB_STREAM_PACKET_LOSS
    sed "s/export TB_STREAM_PACKET_LOSS=.*/export TB_STREAM_PACKET_LOSS=\"${TB_STREAM_PACKET_LOSS}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write TB_STREAM_HEADER_CORRUPT
    sed "s/export TB_STREAM_HEADER_CORRUPT=.*/export TB_STREAM_HEADER_CORRUPT=\"${TB_STREAM_HEADER_CORRUPT}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write TB_STREAM_TRUNCATE
    sed "s/export TB_STREAM_TRUNCATE=.*/export TB_STREAM_TRUNCATE=\"${TB_STREAM_TRUNCATE}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write TB_MAX_DEC_LATENCY_COMPENSATION
    sed "s/export TB_MAX_DEC_LATENCY_COMPENSATION=.*/export TB_MAX_DEC_LATENCY_COMPENSATION=\"${TB_MAX_DEC_LATENCY_COMPENSATION}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write TB_MAX_NUM_PICS
    sed "s/export TB_MAX_NUM_PICS=.*/export TB_MAX_NUM_PICS=\"${TB_MAX_NUM_PICS}\"/" tmpcfg > tmp
    mv tmp tmpcfg

    # write TB_TEST_DB
    sed "s/export TB_TEST_DB=.*/export TB_TEST_DB=\"${TB_TEST_DB}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write PP_ALPHA_BLENDING
    sed "s/export PP_ALPHA_BLENDING=.*/export PP_ALPHA_BLENDING=\"${PP_ALPHA_BLENDING}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write PP_SCALING
    sed "s/export PP_SCALING=.*/export PP_SCALING=\"${PP_SCALING}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write PP_DEINTERLACING
    sed "s/export PP_DEINTERLACING=.*/export PP_DEINTERLACING=\"${PP_DEINTERLACING}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write PP_DITHERING
    sed "s/export PP_DITHERING=.*/export PP_DITHERING=\"${PP_DITHERING}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write LANGUAGE
    sed "s/export HW_LANGUAGE=.*/export HW_LANGUAGE=\"${HW_LANGUAGE}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write BUS
    sed "s/export HW_BUS=.*/export HW_BUS=\"${HW_BUS}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    # write HW_DATA_BUS_WIDTH
    sed "s/export HW_DATA_BUS_WIDTH=.*/export HW_DATA_BUS_WIDTH=\"${HW_DATA_BUS_WIDTH}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
     # write TEST_ENV_DATA_BUS_WIDTH
    sed "s/export TEST_ENV_DATA_BUS_WIDTH=.*/export TEST_ENV_DATA_BUS_WIDTH=\"${TEST_ENV_DATA_BUS_WIDTH}\"/" tmpcfg > tmp
    mv tmp tmpcfg
    
    mv tmpcfg config.sh

    
    eval configuration_cases=\$${configuration}_cases
    if [ "$TB_TEST_DB" == "ENABLED" ] && [ "$g_add_execute_cases" == 1 ]
    then
        addTestCasesExecute2TestDB "$configuration_cases"
    fi
    
    # write specific values for the configuration
    # the function must be implemented in commonconfig.sh
    $configuration
    if [ $? == 0 ]
    then
         echo "OK"
    else
         echo "FAILED"
         exit 1
    fi
    cd ..
    
    # write configuration specific test cases to runall.sh & checkall.sh scripts
    echo "" >>  runall.sh
    echo "echo ${configuration}" >>  runall.sh
    echo "echo ${configuration} >> test_exec_status" >>  runall.sh
    echo -n "if [ ${configuration} == " >>  runall.sh
    echo '$configuration ] || [ $configuration == 0 ]' >>  runall.sh
    echo "then"  >>  runall.sh
    echo "    cd ${configuration}" >>  runall.sh
    echo "    ./runtest.sh ${configuration_cases}" >>  runall.sh
    echo "    ./runtest.sh --failed" >>  runall.sh
    echo "    cd .." >>  runall.sh
    echo '    configuration=0' >>  runall.sh
    echo "fi"  >>  runall.sh
    echo "" >>  runall.sh
    echo "echo ${configuration}" >>  checkall.sh
    echo "cd ${configuration}" >>  checkall.sh
    #echo -n "./checktest.sh \"${CHECK_FORMATS}\" " >>  checkall.sh
    echo -n "./checktest.sh ${configuration_cases} " >>  checkall.sh
    echo '$1' >>  checkall.sh
    echo -n "./checktest.sh --failed " >>  checkall.sh
    echo '$1' >>  checkall.sh
    echo "cd .." >>  checkall.sh
    echo "" >>  checkall.sh
done

#for configuration in $CONFIGURATIONS
#do
#    
#done

echo "" >>  runall.sh
echo "touch run_done" >>  runall.sh

if [ ! -e report_cache.csv  ]
then
    echo "Creating report cache..."
    . $TEST_CASE_LIST
    getCases "$category_list_swhw" "$CHECK_FORMATS" "" "0" "1"
    echo "Test Case;Category;Pictures;Decoder Picture Width;PP Picture Width;Status Date;Execution Time;Status;HW Tag;SW Tag;Configuration;Decoder HW Performance (frame average 10^-5s);PP HW Performance (frame average 10^-5s);Decoder SW Performance (full decode);PP SW Performance (full decode);Comments;" >> "report_cache.csv"
    for test_case in $test_cases
    do
        getCategory "$test_case"
        getPpCaseWidth "$test_case"
        if [ $pp_case_width == 0 ]
        then
            pp_case_width="N/A"
        fi
        getDecCaseWidthWithAwk "$test_case"
        if [ $dec_case_width == 0 ]
        then
            dec_case_width="N/A"
        fi
        echo "case $test_case;$category;$TB_MAX_NUM_PICS;$dec_case_width;$pp_case_width;N/A;N/A;NOT_EXECUTED;N/A;N/A;N/A;N/A;N/A;N/A;N/A;;" >> "report_cache.csv"
    done
fi

chmod 777 .
chmod -R 777 *
exit 0
