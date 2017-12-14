#!/bin/bash

#Imports
.  ./f_check.sh

# Use the same time for all reports and faildirs
export REPORTTIME=`date +%d%m%y_%k%M | sed -e 's/ //'`

importFiles

# Enable this when running 'test_h264.sh size', then the check script will
# check all the cases which have been tested so far
checkExistingDirsOnly=0

Usage="\n$0 [-list] [-csv]\n\t -csv generate csv file report\n"

resultcsv=result.csv

CSV_REPORT=0

csvfile=$csv_path/integrationreport_vp8_${hwtag}_${swtag}_${reporter}_$reporttime
list=0

# parse argument list
if [ $# -eq 1 ]; then
    first_case=3000
    last_case=3999
else
    first_case=$1;
    last_case=$2;
fi
while [ $# -ge 1 ]; do
        case $1 in
        -csv*)  CSV_REPORT=1 ;;
        -list)  list=1;;
        list)  list=1;;
        -*)     echo -e $Usage; exit 1 ;;
        *)      ;;
        esac

        shift
done

if [ $CSV_REPORT -eq 1 ]
then
    echo "Creating report: $csvfile.csv"
    if [ -e case_3000/ewl.trc ]; then
        echo "Tested config:" >> $csvfile.csv
        head -n 14 case_3000/ewl.trc >> $csvfile.csv
    fi
    echo "TestCase;TestPhase;Category;TestedPictures;Language;StatusDate;ExecutionTime;Status;HWTag;EncSystemTag;SWTag;TestPerson;Comments;Performance;KernelVersion;RootfsVersion;CompilerVersion;TestDeviceIP;TestDeviceVersion" >> $csvfile.csv
fi

sum_ok=0
sum_notrun=0
sum_failed=0

check_case() {

    let "set_nro=${case_nr}-${case_nr}/5*5"

    . $test_case_list_dir/test_data_parameter_vp8.sh "$case_nr"

    if [ ${valid[${set_nro}]} -eq 0 ]
    then
        if [ ${validForCheck[${set_nro}]} -eq 0 ]
        then
            if [ $checkExistingDirsOnly -eq 1 ] && [ ! -e case_${case_nr} ]
            then
                # The case dir doesn't exist so move on
                continue
            else
                # Compare case against system model reference
                # If case dir doesn't exist it will be waited for
                sh checkcase_vp8.sh $case_nr
                res=$?
                extra_comment=${comment}" `cat case_${case_nr}/check.log`"
            fi
        else
            # The case is not valid for checking, report as not run
            res=4
            extra_comment=${comment}" Not valid for checking."
        fi

        getExecutionTime "case_$case_nr"

        if [ $CSV_REPORT -eq 1 ]
        then
            echo -n "case_$case_nr;Integration;VP8Case;;$language;$timeform1 $timeform2;$execution_time;" >> $csvfile.csv
            echo -n "case_$case_nr;" >> $resultcsv
            case $res in
            0)
                echo "OK;$hwtag;$systag;$swtag;$reporter;$extra_comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
                echo "OK;" >> $resultcsv
                let "sum_ok++"
                ;;
            3)
                # Returns -VP8ENC_INVALID_ARGUMENT when parameters not supported
                echo "NOT SUPPORTED;$hwtag;$systag;$swtag;$reporter;$extra_comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
                echo "NOT SUPPORTED;" >> $resultcsv
                let "sum_notrun++"
                ;;
            4)
                # Returns -VP8ENC_MEMORY_ERROR when not enough memory available
                echo "NOT RUN;$hwtag;$systag;$swtag;$reporter;$extra_comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
                echo "NOT RUN;" >> $resultcsv
                let "sum_notrun++"
                ;;
            *)
                echo "FAILED;$hwtag;$systag;$swtag;$reporter;$extra_comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
                echo "FAILED; `cat case_${case_nr}/check.log`" >> $resultcsv
                echo "$case_nr" >> list
                let "sum_failed++"
                ;;
            esac
        else
            cat case_${case_nr}/check.log
            echo ""
        fi
    fi
}

if [ $list -eq 1 ]
then
    # Check list of cases
    list=`cat list`
    echo "Checking cases: $list"
    for case_nr in $list
    do
        check_case
    done
else
    # Run all cases in range
    for ((case_nr=$first_case; case_nr<=$last_case; case_nr++))
    do
        check_case
    done
fi

echo ""
if [ $CSV_REPORT -eq 1 ]
then
    echo "Totals: $sum_ok OK   $sum_notrun NOT_RUN   $sum_failed FAILED"
    echo "Failed cases written to 'list'."
    echo "Copying report $csvfile.csv to $PROJECT_DIR/integration/test_reports/"
    cp $csvfile.csv $PROJECT_DIR/integration/test_reports/
fi
