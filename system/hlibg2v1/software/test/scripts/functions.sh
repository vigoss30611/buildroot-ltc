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
#--   Abstract     : Functions for the SW verification of x170 HW decoder     --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====-------------------------------------------------------------------------------=====#
#=====---            Define the list of test cases to be simulated              -----=====#
#=====-------------------------------------------------------------------------------=====#

getCases()
{
  if [ "$case_number" == "all" ]
  then
    echo "---> Test all cases"
    export sim_cases="$h264_cases $mpeg4_cases $jpeg_cases"

  elif [ "$case_number" == "h264" ]
  then
    echo "---> Test h264 cases"
    export sim_cases=$h264_cases

  elif [ "$case_number" == "mpeg4" ]
  then
    echo "---> Test mpeg4 cases"
    export sim_cases="$mpeg4_intra $mpeg4_inter $h263_svh $mpeg4_videop $mpeg4_datap_videop $mpeg4_datap $mpeg4_conformance"
#    export sim_cases="$mpeg4_intra $mpeg4_inter $h263_svh $mpeg4_videop $mpeg4_datap_videop $mpeg4_datap"
  elif [ "$case_number" == "asp" ]
  then
    echo "---> Test mpeg4 cases"
    export sim_cases="mpeg4_asp_conformance"

  elif [ "$case_number" == "jpeg" ]
  then
    echo "---> Test jpeg cases"
    export sim_cases="$jpeg_420 $jpeg_422 $jpeg_400 $jpeg_non16x16 $jpeg_thumbnail"

  else
    echo "---> Test single testcase"
    export single_case=y
    export sim_cases=$case_number
  fi
}
#=====-------------------------------------------------------------------------------=====#
#=====---      Initialize system model for reference data creation              -----=====#
#=====-------------------------------------------------------------------------------=====#

systemInit()
{
  export PRODUCT_ROOT=$PWD/../../../..
  export SYSTEM_SCRIPT_ROOT=$PRODUCT_ROOT/6170_decoder/system/scripts
  export TESTDATA_ROOT=$PRODUCT_ROOT/6170_decoder/system/testdata
  export H264_MODEL_HOME=$PRODUCT_ROOT/6170_decoder/system/models/h264
  export MPEG4_MODEL_HOME=$PRODUCT_ROOT/6170_decoder/system/models/mpeg4
  export POST_PROCESSING_MODEL_HOME=$PRODUCT_ROOT/6170_decoder/system/models/post_processing
  export JPEG_MODEL_HOME=$PWD/../../../../6170_decoder/system/models/jpeg
  export TESTDATA_OUT=$PWD/testdata
  export TEST_DATA_FORMAT=HEX
  export TEST_DATA_FILES=FPGA

  # Remove old output files
  rm -f mbcontrol.hex intra4x4_modes.hex motion_vectors.hex rlc.hex \
        stream_control.hex picture_ctrl_dec.hex dc_separate_coeffs.trc rlc.trc

  # Check if 6170_decoder exists
  if [ ! -e "$PRODUCT_ROOT/6170_decoder/system" ]
  then
    cd $PRODUCT_ROOT
    cvs co 6170_decoder/system/models
    cvs co 6170_decoder/system/scripts
    cd -
  fi

  # Check if 8170 system exists
  if [ ! -e "$PRODUCT_ROOT/8170_decoder/system" ]
  then
    cd $PRODUCT_ROOT
    cvs co 8170_decoder/system/
    cd -
  fi

  if [ "$H264_model_version" == "latest" ]
  then
    export LATEST_H264_TAG=`cvs status -v $H264_MODEL_HOME/Makefile | grep -A5 Tags: | tr -d '\n' | awk '{print$3}'`
  else
    export LATEST_H264_TAG=$H264_model_version
  fi

  if [ "$MPEG4_model_version" == "latest" ]
  then
    export LATEST_MPEG4_TAG=`cvs status -v $MPEG4_MODEL_HOME/Makefile | grep -A5 Tags: | tr -d '\n' | awk '{print$3}'`
  else
    export LATEST_MPEG4_TAG=$MPEG4_model_version
  fi

  if [ "$JPEG_model_version" == "latest" ]
  then
    export LATEST_JPEG_TAG=`cvs status -v $JPEG_MODEL_HOME/Makefile | grep -A5 Tags: | tr -d '\n' | awk '{print$3}'`
  else
    export LATEST_JPEG_TAG=$JPEG_model_version
  fi

  export STICKY_H264_TAG=`cvs status -v $H264_MODEL_HOME/Makefile | grep "Sticky Tag:" | awk '{print$3}'`
  export STICKY_MPEG4_TAG=`cvs status -v $MPEG4_MODEL_HOME/Makefile | grep "Sticky Tag:" | awk '{print$3}'`
  export STICKY_JPEG_TAG=`cvs status -v $JPEG_MODEL_HOME/Makefile | grep "Sticky Tag:" | awk '{print$3}'`
  export STICKY_PP_TAG=`cvs status -v $POST_PROCESSING_MODEL_HOME/Makefile | grep "Sticky Tag:" | awk '{print$3}'`

  # SW/HW interface forcing
  if [ "$forced_interface" == rlc ]
  then
    export RLC_MODE=1
  else
    export RLC_MODE=0
  fi

# Error concealment method
  if [ "$mb_error_concealment" == y ]
  then
    export MB_ERROR_CONCEAL=1
  else
    export MB_ERROR_CONCEAL=0
  fi

#force whole stream
  if [ "$force_whole_stream" == y ]
  then
  export FORCE_WHOLE_STREAM="-W"
  else
    export FORCE_WHOLE_STREAM=
  fi

  #update list of testcases in system folder and copy it to sim folder
  if [ -e "$SYSTEM_SCRIPT_ROOT/testcase_list" ]
  then
    echo Updating list of test cases...
    cvs up $SYSTEM_SCRIPT_ROOT/testcase_list
    cp -f $SYSTEM_SCRIPT_ROOT/testcase_list .
  else
    ls ./$SYSTEM_SCRIPT_ROOT
    echo testcase_list not found in system folder
   fi


  cd $PRODUCT_ROOT
  if [ "$STICKY_H264_TAG" != "$LATEST_H264_TAG" ]
  then
    echo
    echo Updating H.264 system model from $STICKY_H264_TAG to $LATEST_H264_TAG
    echo
    cvs co -r $LATEST_H264_TAG 6170_decoder/system/models/h264
  fi

  if [ "$STICKY_MPEG4_TAG" != "$LATEST_MPEG4_TAG" ]
  then
    echo
    echo Updating MPEG4 system model from $STICKY_MPEG4_TAG to $LATEST_MPEG4_TAG
    echo
    cvs co -r $LATEST_MPEG4_TAG 6170_decoder/system/models/mpeg4
  fi

  if [ "$STICKY_JPEG_TAG" != "$LATEST_JPEG_TAG" ]
  then
    echo
    echo Updating JPEG system model from $STICKY_JPEG_TAG to $LATEST_JPEG_TAG
    echo
    cvs co -r $LATEST_JPEG_TAG 6170_decoder/system/models/jpeg
  fi

  # Initialisation run variable
  initialized=0

  cd -
}
#=====-------------------------------------------------------------------------------=====#
#=====------------------              Check case             ------------------------=====#
#=====-------------------------------------------------------------------------------=====#

checkCase()
{
    timeform1=`date +%d.%m.20%y`
    timeform2=`date +%k:%M:%S`

    DECODER_OUT_FILE=out.yuv
    if [ -e hexcmp.txt ]
    then
      rm hexcmp.txt
    fi

        trace_files="semplanar.yuv"

        if [ "$CURRENT_MODEL" == h264 ]
        then
            DECODER_OUT_FILE=out.yuv
            trace_files="out.yuvsem"
        elif [ "$CURRENT_MODEL" == mpeg4 ]
        then
            DECODER_OUT_FILE=out.yuv
        elif [ "$CURRENT_MODEL" == jpeg ]
        then
            trace_files="out.yuv"
	    DECODER_OUT_FILE=out.yuv
        fi

        # Check existance of files
        for trace in $trace_files
        do
            if [ ! -e $DECODER_OUT_FILE ]
            then
                echo "SW $DECODER_OUT_FILE MISSING"
                echo "SW $DECODER_OUT_FILE MISSING" >> hexcmp.txt
            fi

            if [ ! -e testdata/$trace ]
            then
                ls -la testdata/$trace
                echo "Reference $trace MISSING"
                echo "Reference $trace MISSING" >> hexcmp.txt
            fi
        done

        for trace in $trace_files
        do

            if (cmp $DECODER_OUT_FILE testdata/$trace)
            then
                echo -e "$trace\tOK"
                echo -e "$trace\tOK" >> hexcmp.txt
            else
                echo -e "$trace\tNOK"
                echo -e "$trace\tNOK" >> hexcmp.txt
                err=1
            fi
        done

}


#=====-------------------------------------------------------------------------------=====#
#=====---      Show instructions when script called with bad arguments          -----=====#
#=====-------------------------------------------------------------------------------=====#

showInstructions()
{
  echo -e '\E[32;40m'
  echo "  ---=== X170 Decoder SW verification script ===---"
  echo "  USAGE: "
  echo " ./test.sh [case_nbr] [options]"
  tput sgr0
  echo
  echo "    case_nbr  : ['case number' for running a single test case] "
  echo "    case_nbr  : ['all' for running all cases] "
  echo "    case_nbr  : ['h264' for running all H.264 cases] "
  echo "    case_nbr  : ['mpeg4' for running all MPEG-4 cases] "
  echo "    case_nbr  : ['jpeg' for running all JPEG cases] "
  echo "    case_nbr  : ['api' for running all API cases] "
  echo
  echo " Additional Options: "
  echo "    rlc       : Force RLC (6150 interface) mode"
  echo "    vlc       : Force VLC (x170 interface) mode"
  echo "    pc        : Compile PC-linux version of the SW (default)"
  echo "    versatile : Compile Versatile version of the SW"
  echo
  echo -e '\E[36;40m'
  echo " Valid H.264 cases  : "
  tput sgr0
  echo " [$h264_cases]"
  echo -e '\E[36;40m'
  echo " Valid MPEG-4 cases : "
  tput sgr0
  echo " [$mpeg4_cases]"
  echo -e '\E[36;40m'
  echo " Valid JPEG cases   : "
  tput sgr0
  echo " [$jpeg_cases]"
  exit
}


#=====-------------------------------------------------------------------------------=====#
#=====---            Check if given test case(s) is valid                       -----=====#
#=====-------------------------------------------------------------------------------=====#

validCase()
{
  if eval "echo ${h264_cases[*]}|grep -w '${CURRENT_CASENUM}'>/dev/null"
  then
    echo "$CURRENT_CASENUM is an H.264 testcase"
    export CURRENT_MODEL="h264"
    if [ "$coverage" == "y" ]
    then
        export CURRENT_SW="hx170dec_coverage"
    else
        export CURRENT_SW="hx170dec_pclinux"
    fi
    export CURRENT_SYSTAG=$LATEST_H264_TAG
  elif eval "echo ${mpeg4_all[*]}|grep -w '${CURRENT_CASENUM}'>/dev/null"
  then
    echo "$CURRENT_CASENUM is an MPEG-4 testcase"
    export CURRENT_MODEL="mpeg4"
    if [ "$coverage" == "y" ]
    then
    export CURRENT_SW="mx170dec_coverage"

    else
    export CURRENT_SW="mx170dec_pclinux"

    fi
    export CURRENT_SYSTAG=$LATEST_MPEG4_TAG
  elif eval "echo ${jpeg_cases[*]}|grep -w '${CURRENT_CASENUM}'>/dev/null"
  then
    echo "$CURRENT_CASENUM is a JPEG testcase"
    export CURRENT_MODEL="jpeg"
    export CURRENT_SW="jx170dec_pclinux"
    export CURRENT_SYSTAG=$LATEST_JPEG_TAG
  else
    echo "not valid testcase"
    exit
  fi

  # Test case is valid, get the test data from CVS
  if [ ! -e "$PRODUCT_ROOT/6170_decoder/system/testdata/case_${CURRENT_CASENUM}" ]
  then
    cd $PRODUCT_ROOT
    cvs co 6170_decoder/system/testdata/case_${CURRENT_CASENUM}
    cd -
  fi

}

#=====-------------------------------------------------------------------------------=====#
#=====---                      Parse given arguments                            -----=====#
#=====-------------------------------------------------------------------------------=====#

parseArguments()
{

  if [ "$2" == "b" ] || [ "$3" == "b" ]
  then
    export BUILD_SW=n
  fi



}

#=====-------------------------------------------------------------------------------=====#
#=====-------------  Set up packet mode                      ------------------------=====#
#=====-------------------------------------------------------------------------------=====#

setupPacketMode()
{
# Packet decode mode
     if [ "$packetmode" == y ] || [ "$CURRENT_CASENUM" == 813 ]

     then
        export PACKET_MODE="-U"
     else
        export PACKET_MODE=
     fi
}



#=====-------------------------------------------------------------------------------=====#
#=====---                           Make SW                                     -----=====#
#=====-------------------------------------------------------------------------------=====#

makeSW()
{
if [ $BUILD_SW == "y" ]
then

    echo "Compile $CURRENT_MODEL SW"
    pwd
    if [ "$coverage" == "y" ]
    then
        make -C ../$CURRENT_MODEL/ coverage
    else
        make -C ../$CURRENT_MODEL/ pclinux
    fi
    cp ../$CURRENT_MODEL/$CURRENT_SW .
    echo "build $BUILD_SW"

    export BUILD_SW=n
    echo "build $BUILD_SW"
else
    echo "Do not build software"
fi

}
#=====-------------------------------------------------------------------------------=====#
#=====--------  Get test data from reference C-model or zip  ------------------------=====#
#=====-------------------------------------------------------------------------------=====#

getTestData()
{
setupPacketMode
if [  $USE_OLD_DATA == y  ]
then

   echo "use old data"
else
  if [ -e testdata ]
  then
    rm -rf testdata
  fi

  mkdir testdata

  setupPacketMode
  if [ ! -e "$SYSTEM_SCRIPT_ROOT/$CURRENT_MODEL/test.sh" ]
  then
    echo "  Exit script:"
    echo "  System model not found."
    echo "$SYSTEM_SCRIPT_ROOT/$CURRENT_MODEL/test.sh"
    exit
  else
    $SYSTEM_SCRIPT_ROOT/$CURRENT_MODEL/test.sh $CURRENT_CASENUM $num_frames $PACKET_MODE
    if [ "${CURRENT_MODEL}" == "h264" ]
    then
     mv -f ./testdata/$TESTCASE/stream.264 ./testdata/stream.h264
    elif [ "${CURRENT_MODEL}" == "jpeg" ]
    then
     mv -f ./testdata/$TESTCASE/stream.jpg ./testdata/stream.jpg
    fi
     mv -f ./testdata/$TESTCASE/* ./testdata/
     rm -rf ./testdata/$TESTCASE/
     rm trace.cfg
  fi
fi
}
#=====-------------------------------------------------------------------------------=====#
#=====------------------           Run the testcase          ------------------------=====#
#=====-------------------------------------------------------------------------------=====#

runSW()
{
    if [ -e $TESTDATA_OUT/stream.h264 ]
    then
      export SW_INPUT_PARAMETERS="-Oout.yuv $TESTDATA_OUT/stream.h264"

    elif [ -e $TESTDATA_OUT/stream.mpeg4 ]
    then
      export SW_INPUT_PARAMETERS="-Oout.yuv $TESTDATA_OUT/stream.mpeg4"

    elif [ -e $TESTDATA_OUT/stream.jpg ]
    then
      export SW_INPUT_PARAMETERS="$TESTDATA_OUT/stream.jpg"

    else
      echo Input stream for test case $CURRENT_CASENUM not found!
    fi

    rm -f mbcontrol.hex intra4x4_modes.hex motion_vectors.hex rlc.hex stream_control.hex picture_ctrl_dec.hex
    if [ -e $TESTDATA_OUT/stream.jpg ]
    then
        rm -f jpeg_tables.hex
    fi

    echo -e "./$CURRENT_SW $SW_INPUT_PARAMETERS"; tput sgr0; echo
    ./$CURRENT_SW $SW_INPUT_PARAMETERS
    echo "Case fails? Are you sure these parameters are what you wanted?:"
    echo -e "Used test parameters: packetmode=$packetmode, rlcmode=$RLC_MODE, use_old_data=$USE_OLD_DATA, mb_error_conceal=$mb_error_concealment"; tput sgr0; echo
    echo "Check also: All the parameters in the makefiles in linux/dwl and linux/current_decoder. Especially check that _SWSW_TESTING is enabled!"
}
#=====-------------------------------------------------------------------------------=====#
#=====--- Report simulations results to a CSV (Comma Separated Values) file ---------=====#
#=====-------------------------------------------------------------------------------=====#

reportCSV()
{

  echo "reportCSV"
  if [ $initialized == 0 ]
  then
    # New file, write title row
    export initialized=1
    echo "TestCase;TestPhase;Category;TestedPictures;SWHW_Interface;StatusDate;Status;SWTag;DecSystemTag;TestPerson;Failure Reason;Performance" > ./CSVreport.tmp
  fi

  if [ ! -e hexcmp.txt ]
  then
    statusfield="Not started"
  else
    fail_reason=`grep NOK hexcmp.txt | awk '{print$1}'`
    if [ "$fail_reason" == "" ]
    then
      statusfield=PASSED
    else
      statusfield=FAIL
    fi
  fi

  sw_tag_field=`cvs status -v ../../linux/$CURRENT_MODEL/Makefile | grep -A5 Tags: | tr -d '\n' | awk '{print$3}'`
  sw_tag_field=$tagfield

  timeform1=`date +%d.%m.20%y`
  timeform2=`date +%k:%M:%S`

  # Case types
  casetype=notDefined

  # Write report line
  echo "case $CURRENT_CASENUM;SW/SW Integration;$casetype;$num_frames;$forced_interface;$timeform1 $timeform2;$statusfield;$sw_tag_field;$CURRENT_SYSTAG;$USER;$fail_reason;;" >> ./CSVreport.tmp

  # Write log
  echo "Case $CURRENT_CASENUM comparison" >> comparing_hex.log
  cat hexcmp.txt  >> comparing_hex.log
}

#=====-------------------------------------------------------------------------------=====#
#=====-------------  Rename & copy CVS file to target folder ------------------------=====#
#=====-------------------------------------------------------------------------------=====#

writeCSVFile()
{
    echo copy CSV file
    if [ "$single_case" == "y" ]
    then
        echo No CSV report created for single case
    else
        echo copy report
        reporttime=`date +%d%m%y_%k%M | sed -e 's/ //'`
        cp -f ./CSVreport.tmp $CSV_PATH/simulationreport_${tagfield}_${USER}_${reporttime}.csv
        mv -f ./CSVreport.tmp ./CSVreport.bak
        echo ----------------------------------------------------------------------
        echo CSV file written to $CSV_PATH/simulationreport_${tagfield}_${USER}_${reporttime}.csv
        echo ----------------------------------------------------------------------
    fi
}


