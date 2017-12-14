#!/bin/bash
#-------------------------------------------------------------------------------
#-                                                                            --
#-       This software is confidential and proprietary and may be used        --
#-        only as expressly authorized by a licensing agreement from          --
#-                                                                            --
#-                            Hantro Products Oy.                             --
#-                                                                            --
#-                   (C) COPYRIGHT 2006 HANTRO PRODUCTS OY                    --
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

.  ./commonconfig.sh


startTimeCounter()
{
    case_dir=$1
    rm -f "${case_dir}/case.time"
    start_time=$(date +%s)
    echo "START:${start_time}" >> "${case_dir}/case.time"
}

endTimeCounter()
{
    case_dir=$1
    end_time=$(date +%s)
    echo "END:${end_time}" >> "${case_dir}/case.time"
}
