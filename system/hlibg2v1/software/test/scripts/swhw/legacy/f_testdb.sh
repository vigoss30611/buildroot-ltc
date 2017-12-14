#!/bin/bash

if [ -e ../commonconfig.sh ]
then
    . ../commonconfig.sh
fi

if [ -e ./commonconfig.sh ]
then
    . ./commonconfig.sh
fi

DB_PASSWD="laboratorio"
DB_USER="db_user"
DB_HOST="172.28.107.116"
DB_NAME="testDB"

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

getHwTagId()
{
    local hw_tag=$1
    
    local query="SELECT id FROM tag WHERE name = \"${hw_tag}\";"
    local db_result=$(dbQuery "$query")
    echo "$db_result"
}

getHwConfigTagId()
{
    local hw_config_tag=$1
    
    local query="SELECT id FROM config_tag WHERE name = \"${hw_config_tag}\";"
    local db_result=$(dbQuery "$query")
    echo "$db_result"
}

getSwTagId()
{
    local sw_tag=$1

    local query="SELECT id FROM system_tag WHERE name = \"${sw_tag}\";"
    local db_result=$(dbQuery "$query")
    echo $db_result
}

#insertHwTag()
#{
#    local hw_tag=$1
#
#    local user_id=$(getUserID)
#    local product_id=$(getProductID)
#    if [ "$user_id" != "0" ]
#    then
#        local hw_tag_id=$(getHwTagId "$hw_tag")
#        if [ -z "$hw_tag_id" ]
#        then
#            major=`echo "$hw_tag" | awk -F_ '{print $2}'`
#            minor=`echo "$hw_tag" | awk -F_ '{print $3}'`
#            local query="INSERT INTO tag(id, name, created, tagger_id, product_id, major, minor) VALUES (NULL, \"${hw_tag}\", NULL, ${user_id}, ${product_id}, ${major}, ${minor});SELECT LAST_INSERT_ID();"
#            local db_result=$(dbQuery "$query")
#            hw_tag_id="$db_result"
#        fi
#        echo "$hw_tag_id"
#    else
#        echo "User id for ${USER} is not found in test database, contact DB admin (petriu)"
#        exit
#    fi
#}

#insertSwTag()
#{
#    local sw_tag=$1
#
#    local user_id=$(getUserID)
#    local product_id=$(getProductID)
#    if [ "$user_id" != "0" ]
#    then
#        local sw_tag_id=$(getSwTagId ${sw_tag})
#        if [ -z "$sw_tag_id" ]
#        then
#            major=`echo "$sw_tag" | awk -F_ '{print $2}'`
#            minor=`echo "$sw_tag" | awk -F_ '{print $3}'`
#            local query="INSERT INTO system_tag(id, name, created, tagger_id, product_id, major, minor) VALUES (NULL, \"${sw_tag}\", NULL, ${user_id}, ${product_id}, ${major}, ${minor});SELECT LAST_INSERT_ID();"
#            local db_result=$(dbQuery "$query")
#            sw_tag_id=${db_result}
#        fi
#        echo "$sw_tag_id"
#    else
#        echo "User id for $USER is not found in test database, contact DB admin (petriu)"
#        exit
#    fi
#}

#insertHwConfigTag()
#{
#    local hw_config_tag=$1
#
#    local user_id=$(getUserID)
#    local product_id=$(getProductID)
#    if [ "$user_id" != "0" ]
#    then
#        local hw_config_tag_id=$(getHwConfigTagId "$hw_config_tag")
#        if [ -z "$hw_config_tag_id" ]
#        then
#            local query="INSERT INTO config_tag(id, name, created, tagger_id, product_id) VALUES (NULL, \"${hw_config_tag}\", NULL, ${user_id}, ${product_id});SELECT LAST_INSERT_ID();"
#            local db_result=$(dbQuery "$query")
#            hw_config_tag_id="$db_result"
#        fi
#        echo "$hw_config_tag_id"
#    else
#        echo "User id for ${USER} is not found in test database, contact DB admin (petriu)"
#        exit
#    fi
#}

addResult2TestDB()
{
    local testcase_id=$1 
    local status=$2
    local status_date=$3 
    local exec_time=$4
    # configuration is set only when case fails
    local configuration="$status"
    local status_comment=$6
    local max_pics=$7

    #local hw_tag_id=$(insertHwTag "$HWBASETAG")
    #local sw_tag_id=$(insertSwTag "$SWTAG")
    #local hw_config_tag_id=$(insertHwConfigTag "$HWCONFIGTAG")
    local hw_tag_id=$(getHwTagId "$HWBASETAG")
    if [ -z "$hw_tag_id" ]
    then
        echo "Cannot find ID for ${HWBASETAG}"
        exit 1
    fi
    local sw_tag_id=$(getSwTagId "$SWTAG")
    if [ -z "$sw_tag_id" ]
    then
        echo "Cannot find ID for ${SWTAG}"
        exit 1
    fi
    local hw_config_tag_id=$(getHwConfigTagId "$HWCONFIGTAG")
    if [ -z "$hw_config_tag_id" ]
    then
        echo "Cannot find ID for ${HWCONFIGTAG}"
        exit 1
    fi
    local tester_id=$(getUserID)

    # set test status
    if [ "$status" == "NOT_FAILED" ] ||  [ "$status" == "OK" ]
    then
        addTestCasesOk2TestDB "$testcase_id" "$sw_tag_id" "$hw_tag_id" "$hw_config_tag_id"
    fi

    if [ "$status" == "NOT_OK" ] ||  [ "$status" == "FAILED" ]
    then
        addTestCasesFail2TestDB "$testcase_id" "$sw_tag_id" "$hw_tag_id" "$hw_config_tag_id"
        # configuration is set only when case fails
        configuration="$5"
    fi

    local query="SELECT status FROM lab_test_results WHERE sw_tag_id = ${sw_tag_id} AND hw_tag_id = ${hw_tag_id} AND testcase_id = ${testcase_id} AND tester_id = ${tester_id};"
    previous_test_status=$(dbQuery "$query")
    if [ -z "$previous_test_status" ]
    then
        local query="INSERT INTO lab_test_results (id, hw_tag_id, hw_config_tag, sw_tag_id, testcase_id, tester_id, status, status_comment, status_date, exec_time, configuration, pictures, hw_config_tag_id) \
VALUES (NULL, ${hw_tag_id}, \"id\", ${sw_tag_id}, ${testcase_id}, ${tester_id}, \"${status}\", \"${status_comment}\", \"${status_date}\", ${exec_time}, \"${configuration}\", ${max_pics}, ${hw_config_tag_id})"
        $(dbQuery "$query")
        return "$DB_QUERY_ERROR"
    fi

    # set test result
    update_test_status=0
    if [ "$status" == "NOT_FAILED" ] && [[ "$previous_test_status" != "NOT_FAILED" && "$previous_test_status" != "OK" && "$previous_test_status" != "NOT_OK" && "$previous_test_status" != "FAILED" ]]
    then
        update_test_status=1
    fi
    if [ "$status" == "OK" ] && [[ "$previous_test_status" != "OK" && "$previous_test_status" != "NOT_OK" && "$previous_test_status" != "FAILED" ]]
    then
        update_test_status=1
    fi
    if [ "$status" == "NOT_OK" ] && [[ "$previous_test_status" != "NOT_OK" && "$previous_test_status" != "FAILED" ]]
    then
        update_test_status=1
    fi
    if [ "$status" == "FAILED" ] && [ "$previous_test_status" != "FAILED" ]
    then
        update_test_status=1
    fi
    
    if [ "$update_test_status" == 1 ]
    then
        #local query="INSERT INTO lab_test_results (id, hw_tag_id, hw_config_tag, sw_tag_id, testcase_id, tester_id, status, status_comment, status_date, exec_time, configuration, pictures) \
#VALUES (NULL, ${hw_tag_id}, \"${HWCONFIGTAG}\", ${sw_tag_id}, ${testcase_id}, ${tester_id}, \"${status}\", \"${status_comment}\", \"${status_date}\", ${exec_time}, \"${configuration}\", ${TB_MAX_NUM_PICS})"
        local query="UPDATE lab_test_results \
SET hw_config_tag=\"id\", tester_id=${tester_id}, status=\"${status}\", status_comment=\"${status_comment}\", status_date=\"${status_date}\", exec_time=${exec_time}, configuration=\"${configuration}\", pictures=${TB_MAX_NUM_PICS}, hw_config_tag_id=${hw_config_tag_id} \
WHERE hw_tag_id=${hw_tag_id} AND sw_tag_id=${sw_tag_id} AND testcase_id=${testcase_id} AND tester_id = ${tester_id};"
        $(dbQuery "$query")
        return "$DB_QUERY_ERROR"
    fi
}

addTestCasesOk2TestDB()
{
    local testcase_id=$1 
    local sw_tag_id=$2
    local hw_tag_id=$3
    local hw_config_tag_id=$4

    local query="SELECT testcases_ok FROM lab_test_sessions WHERE sw_tag_id=${sw_tag_id} AND hw_tag_id=${hw_tag_id} AND hw_config_tag_id=${hw_config_tag_id} AND testcase_id=${testcase_id};"
    previous_testcases_ok=$(dbQuery "$query")
    if [ -z "$previous_testcases_ok" ]
    then
        echo "Internal Error: addTestCasesOk2TestDB()"
        exit 1
    else
        let 'previous_testcases_ok = previous_testcases_ok + 1'
        local query="UPDATE lab_test_sessions SET testcases_ok=${previous_testcases_ok} WHERE hw_tag_id=${hw_tag_id} AND hw_config_tag_id=${hw_config_tag_id} AND sw_tag_id=${sw_tag_id} AND testcase_id=${testcase_id}"
        $(dbQuery "$query")
        return "$DB_QUERY_ERROR"
    fi
}

addTestCasesFail2TestDB()
{
    local testcase_id=$1 
    local sw_tag_id=$2
    local hw_tag_id=$3
    local hw_config_tag_id=$4

    local query="SELECT testcases_fail FROM lab_test_sessions WHERE sw_tag_id=${sw_tag_id} AND hw_tag_id=${hw_tag_id} AND hw_config_tag_id=${hw_config_tag_id} AND testcase_id=${testcase_id};"
    previous_testcases_fail=$(dbQuery "$query")
    if [ -z "$previous_testcases_fail" ]
    then
        echo "Internal Error: addTestCasesFail2TestDB()"
        exit 1
    else
        let 'previous_testcases_fail = previous_testcases_fail + 1'
        local query="UPDATE lab_test_sessions SET testcases_fail=${previous_testcases_fail} WHERE hw_tag_id=${hw_tag_id} AND hw_config_tag_id=${hw_config_tag_id} AND sw_tag_id=${sw_tag_id} AND testcase_id=${testcase_id}"
        $(dbQuery "$query")
        return "$DB_QUERY_ERROR"
    fi
}

addTestCasesExecute2TestDB()
{
    local configuration_cases="$1"

    #local hw_tag_id=$(insertHwTag "$HWBASETAG")
    #local sw_tag_id=$(insertSwTag "$SWTAG")
    #local hw_config_tag_id=$(insertHwConfigTag "$HWCONFIGTAG")
    local hw_tag_id=$(getHwTagId "$HWBASETAG")
    if [ -z "$hw_tag_id" ]
    then
        echo "Cannot find ID for ${HWBASETAG}"
        exit 1
    fi
    local sw_tag_id=$(getSwTagId "$SWTAG")
    if [ -z "$sw_tag_id" ]
    then
        echo "Cannot find ID for ${SWTAG}"
        exit 1
    fi
    local hw_config_tag_id=$(getHwConfigTagId "$HWCONFIGTAG")
    if [ -z "$hw_config_tag_id" ]
    then
        echo "Cannot find ID for ${HWCONFIGTAG}"
        exit 1
    fi

    case_number=`echo "$configuration_cases" |awk -F -n '{ print $1}' |tr -d '"'`
    case_filter=`echo "$configuration_cases" |awk -F -n '{ print $2}' |tr -d '"'`

    getCases "$category_list_swhw" "$case_number" "$case_filter" "1" "1"
    for test_case in $test_cases
    do
        local query="SELECT testcases_execute FROM lab_test_sessions WHERE sw_tag_id=${sw_tag_id} AND hw_tag_id=${hw_tag_id} AND hw_config_tag_id=${hw_config_tag_id} AND testcase_id = ${test_case};"
        previous_testcases_execute=$(dbQuery "$query")
        if [ -z "$previous_testcases_execute" ]
        then
            local query="INSERT INTO lab_test_sessions (id, hw_tag_id, hw_config_tag, hw_config_tag_id, sw_tag_id, testcase_id, testcases_execute) \
VALUES (NULL, ${hw_tag_id}, \"id\", ${hw_config_tag_id}, ${sw_tag_id}, ${test_case}, 1)"
            $(dbQuery "$query")
            if [ "$DB_QUERY_ERROR" != 0 ]
            then
                return "$DB_QUERY_ERROR"
            fi
        else
            let 'previous_testcases_execute = previous_testcases_execute + 1'
            local query="UPDATE lab_test_sessions SET testcases_execute=${previous_testcases_execute} WHERE hw_tag_id=${hw_tag_id} AND hw_config_tag_id=${hw_config_tag_id} AND sw_tag_id=${sw_tag_id} AND testcase_id=${test_case}"
            $(dbQuery "$query")
            if [ "$DB_QUERY_ERROR" != 0 ]
            then
                return "$DB_QUERY_ERROR"
            fi
        fi
    done
    return 0
}

 
