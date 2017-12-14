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
#-                                                                            --
#--   Abstract     : Script for adding test results to test data base.        --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#

. ./commonconfig.sh
. ./f_testdbquery.sh

getUserID()
{
    local query="SELECT id FROM users WHERE handle = \"$USER\";"
    local db_result=$(dbQuery "$query")
    # is user not found, add $USER
    if [ -z "$db_result" ]
    then
        name=`finger "$USER" |grep -m 1 "$USER" |awk -F \Name: '{print $2}'| sed 's/^[ \t]*//;s/[ \t]*$//'`
        # if there is no user (which should not be possible)
        if [ -z "$name" ]
        then
            name="$USER"
        fi
        query="INSERT INTO users(id,handle,name) VALUES (NULL,\"$USER\", \"$name\"); SELECT LAST_INSERT_ID();"
        db_result=$(dbQuery "$query")
    fi
    echo "$db_result"
}

getProductID()
{
    local query="SELECT id FROM product WHERE name = UPPER(\"${PRODUCT}\");"
    local db_result=$(dbQuery "$query")
    echo "$db_result"
}

insertTag()
{
    local tag=$1
  
    local user_id=$(getUserID)
    local product_id=$(getProductID)
    if [ -z "$product_id" ]
    then
        echo ""
        return
    fi
  
    local major=`echo "$tag" | awk -F_ '{print $2}'`
    local minor=`echo "$tag" | awk -F_ '{print $3}'`
    local query="LOCK TABLES tag WRITE; SELECT tag_insert(\"$tag\", $user_id, $product_id, $major, $minor); UNLOCK TABLES;"
    local db_result=$(dbQuery "$query")
    echo $db_result
}

insertSwTag()
{
    local tag=$1
  
    local user_id=$(getUserID)
    local product_id=$(getProductID)
    if [ -z "$product_id" ]
    then
        echo ""
        return
    fi
  
    local major=`echo "$tag" | awk -F_ '{print $2}'`
    local minor=`echo "$tag" | awk -F_ '{print $3}'`
    local query="LOCK TABLES sw_tag WRITE; SELECT sw_tag_insert(\"$tag\", $user_id, $product_id, $major, $minor); UNLOCK TABLES;"
    local db_result=$(dbQuery "$query")
    echo $db_result
}

insertSystemTag()
{
    local tag=$1
  
    local user_id=$(getUserID)
    local product_id=$(getProductID)
    if [ -z "$product_id" ]
    then
        echo ""
        return
    fi
  
    local major=`echo "$tag" | awk -F_ '{print $2}'`
    local minor=`echo "$tag" | awk -F_ '{print $3}'`
    local query="LOCK TABLES system_tag WRITE; SELECT system_tag_insert(\"$tag\", $user_id, $product_id, $major, $minor); UNLOCK TABLES;"
    local db_result=$(dbQuery "$query")
    echo $db_result
}

getHwConfigTagId()
{
    local hw_config_tag=$1
    
    local query="SELECT id FROM config_tag WHERE name = \"${hw_config_tag}\";"
    local db_result=$(dbQuery "$query")
    echo "$db_result"
}

checkHWConfigTag()
{
    local hw_config_tag=$1
    
    local query="SELECT id FROM config_tag WHERE name = \"${hw_config_tag}\";"
    local db_result=$(dbQuery "$query")
    if [ -z "$db_result" ]
    then
        return 1
    fi
    return 0
}



testcase_id=`echo $1 |awk -F \_ '{print $2}'`
test_round=`echo $1 |awk -F \_ '{print $3}'`
status_date=$2
exec_time=$3
status=$4
status_comment=$5

tag_id=$(insertTag "$hwtag")
if [ -z "$tag_id" ]
then
    echo "Cannot find ID for ${hwtag}"
    exit 1
fi
sw_tag_id=""
if [ "$hwtag" != "$swtag" ]
then
    sw_tag_id=$(insertSwTag "$swtag")
    if [ -z "$sw_tag_id" ]
    then
        echo "Cannot find ID for ${swtag}"
        exit 1
    fi
fi
sys_tag_id=""
if [ "$hwtag" != "$systag" ]
then
    sys_tag_id=$(insertSystemTag "$systag")
    if [ -z "$sys_tag_id" ]
    then
        echo "Cannot find ID for ${systag}"
        exit 1
    fi
fi

hw_config_id=$(getHwConfigTagId "$cfgtag")

tester_id=$(getUserID)

query="INSERT INTO lab_test_results (id, tag_id, testcase_id, tester_id, status, status_comment, exec_time, test_device, test_round) \
VALUES (NULL, ${tag_id}, ${testcase_id}, ${tester_id}, \"${status}\", \"${status_comment}\", ${exec_time}, ${testdeviceip}, ${test_round}); SELECT LAST_INSERT_ID();"
results_id=$(dbQuery "$query")
if [ ! -z "$hw_config_id" ]
then
    query="UPDATE lab_test_results SET hw_config_id=${hw_config_id} WHERE id = ${results_id};"
    $(dbQuery "$query")
fi
if [ ! -z "$sw_tag_id" ]
then
    query="UPDATE lab_test_results SET sw_tag_id=${sw_tag_id} WHERE id = ${results_id};"
    $(dbQuery "$query")
fi
if [ ! -z "$sys_tag_id" ]
then
    query="UPDATE lab_test_results SET system_tag_id=${sys_tag_id} WHERE id = ${results_id};"
    $(dbQuery "$query")
fi

