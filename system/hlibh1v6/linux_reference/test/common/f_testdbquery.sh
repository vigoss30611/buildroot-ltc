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
#--   Abstract     : Script for make mysql queries.                  --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

DB_PASSWD="laboratorio"
DB_USER="db_user"
DB_HOST="172.28.107.116"
DB_NAME="testDBEnc"

MYSQL_OPTIONS="--skip-column-names"
DB_CMD="mysql ${MYSQL_OPTIONS} -h${DB_HOST} -u${DB_USER} -p${DB_PASSWD} -e"
DB_QUERY_ERROR="0"

dbQuery()
{
    local query=$1
   
    local db_result=`$DB_CMD "use $DB_NAME; ${query}"`
    DB_QUERY_ERROR="$?"
    echo "$db_result"
}
