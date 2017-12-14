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
#--   Abstract     : Script for the SW verification of x170 HW decoder        --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

#include functions & list of testcases

. ./functions.sh
. ./config.sh
systemInit

. ./testcase_list

#=====--------------------------------------------------------------------=====#

# Get command line arguments
if [ $# -lt 1 ] || [ $# -gt 3 ]
then
  # Invalid amount of arguments, show instructions and exit
  showInstructions
else
  case_number=$1

  parseArguments

  getCases

  for CURRENT_CASENUM in $sim_cases
  do
    echo
    export TESTCASE=case_${CURRENT_CASENUM}

    # Check case validity
    validCase

    echo -e "\E[36;40m Compiling software"; tput sgr0; echo
    if [ $printing == 0 ]
    then
      makeSW > /dev/null
    else
      makeSW
    fi

    echo -e "\E[36;40m Generating test data for testcase $CURRENT_CASENUM"; tput sgr0; echo
    if [ $printing == 0 ]
    then
      getTestData > /dev/null
    else
      getTestData
    fi

    echo -e "\E[36;40m Running software"; tput sgr0; echo
    runSW

    echo -e "\E[36;40m Comparing case $CURRENT_CASENUM outputs"; tput sgr0; echo
    checkCase

    reportCSV

  done
  writeCSVFile

fi


