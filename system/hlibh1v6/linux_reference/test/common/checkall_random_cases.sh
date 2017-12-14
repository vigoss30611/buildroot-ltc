#!/bin/bash

#Imports
.  ./f_check.sh

# Use the same time for all reports and faildirs
export REPORTTIME=`date +%d%m%y_%k%M | sed -e 's/ //'`

importFiles


Usage="\n$0 [-csv]\n\t -csv generate csv file report\n"

csvfile=$csv_path/integrationreport_random_${hwtag}_${swtag}_${reporter}_$reporttime

resultcsv=result.csv

CSV_REPORT=0

sum_ok=0
sum_notrun=0
sum_failed=0

# parse argument list
while [ $# -ge 1 ]; do
        case $1 in
        -csv*)  CSV_REPORT=1 ;;
        -*)     echo -e $Usage; exit 1 ;;
        *)      ;;
        esac

        shift
done

if [ $CSV_REPORT -eq 1 ]
then
    echo "Creating report: $csvfile.csv"
    echo "TestCase;TestPhase;Category;TestedPictures;Language;StatusDate;ExecutionTime;Status;HWTag;EncSystemTag;SWTag;TestPerson;Comments;Performance;KernelVersion;RootfsVersion;CompilerVersion;TestDeviceIP;TestDeviceVersion" >> $csvfile.csv
fi

# Wait until the random case list has been created
echo -n "Waiting for random.log"
while [ ! -e random_log_done ]
do
    echo -n "."
    sleep 10
done

echo ""
echo "Checking random cases"

cases=`cat random.log | tr "\n" " " | tail -n 1`
random_cases_arr=($cases)

for ((i=0; i<=2999; i++))
do
    case_nro=${random_cases_arr[$i]}
    set_nro=${case_nro}-${case_nro}/5*5

    #echo "Case $case_nro"

    if [ $case_nro -ge 1000 ] && [ $case_nro -lt 1990 ]
    then

        . $test_case_list_dir/test_data_parameter_h264.sh "$case_nro"

        if [ ${valid[${set_nro}]} -eq 0 ]
        then
            . checkcase_h264.sh $case_nro
            res=$?
            extra_comment=${comment}" `cat ${TESTDIR}/case_${case_nro}/check.log`"

            getExecutionTime "case_$case_nro"

            if [ $CSV_REPORT -eq 1 ]
            then
                echo -n "case_$case_nro;Integration;H264Case;;$language;$timeform1 $timeform2;$execution_time;" >> $csvfile.csv
                echo -n "case_$case_nro;" >> $resultcsv
                case $res in
                0)
                    echo "OK;$hwtag;$systag;$swtag;$reporter;$extra_comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
                    echo "OK;" >> $resultcsv
                    let "sum_ok++"
                    ;;
                3)
                    echo "NOT SUPPORTED;$hwtag;$systag;$swtag;$reporter;$extra_comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
                    echo "NOT SUPPORTED;" >> $resultcsv
                    let "sum_notrun++"
                    ;;
                4)
                    echo "NOT RUN;$hwtag;$systag;$swtag;$reporter;$extra_comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
                    echo "NOT RUN;" >> $resultcsv
                    let "sum_notrun++"
                    ;;
                *)
                    echo "FAILED;$hwtag;$systag;$swtag;$reporter;$extra_comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
                    echo "FAILED; `cat case_${case_nro}/check.log`" >> $resultcsv
                    echo "$case_nro" >> list
                    let "sum_failed++"
                    ;;
                esac
            else
                cat ${TESTDIR}/case_${case_nro}/check.log
                echo ""
            fi

            if [ $case_nro -ge 1750 ] && [ $case_nro -lt 1800 ]
            then
                . checkcase_vs.sh $case_nro
                res=$?

                getExecutionTime "case_$case_nro"

                if [ $CSV_REPORT -eq 1 ]
                then
                    echo -n "case_$case_nro;Integration;Standalone stabilization;;$language;$timeform1 $timeform2;$execution_time;" >> $csvfile.csv 
                    echo -n "vs_case_$case_nro;" >> $resultcsv
                    case $res in
                    0)
                        echo "OK;$hwtag;$systag;$swtag;$reporter;$comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv 
                        echo "OK;" >> $resultcsv
                        let "sum_ok++"
                        ;;
                    3)
                        echo "NOT SUPPORTED;$hwtag;$systag;$swtag;$reporter;$comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
                        echo "NOT SUPPORTED;" >> $resultcsv
                        let "sum_notrun++"
                        ;;
                    4)
                        echo "NOT RUN;$hwtag;$systag;$swtag;$reporter;$comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
                        echo "NOT RUN;" >> $resultcsv
                        let "sum_notrun++"
                        ;;
                    *)
                        echo "FAILED;$hwtag;$systag;$swtag;$reporter;$comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv 
                        echo "FAILED; `cat case_${case_nro}/check.log`" >> $resultcsv
                        echo "$case_nro" >> list
                        let "sum_failed++"
                        ;;
                    esac
                 else
                    cat ${TESTDIR}/vs_case_${case_nro}/check.log
                    echo ""
                 fi
            fi
        fi

    elif [ $case_nro -ge 2000 ] && [ $case_nro -lt 3000 ]
    then

        . $test_case_list_dir/test_data_parameter_jpeg.sh "$case_nro"

        if [ ${valid[${set_nro}]} -eq 0 ]
        then
            . checkcase_jpeg.sh $case_nro
            res=$?
            extra_comment=${comment}" `cat ${TESTDIR}/case_${case_nro}/check.log`"

            getExecutionTime "case_$case_nro"

            if [ $CSV_REPORT -eq 1 ]
            then
                echo -n "case_$case_nro;Integration;JPEGCase;;$language;$timeform1 $timeform2;$execution_time;" >> $csvfile.csv
                echo -n "case_$case_nro;" >> $resultcsv
                case $res in
                0)
                    echo "OK;$hwtag;$systag;$swtag;$reporter;$extra_comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
                    echo "OK;" >> $resultcsv
                    let "sum_ok++"
                    ;;
                1)
                    echo "NOT RUN;$hwtag;$systag;$swtag;$reporter;$extra_comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
                    echo "NOT RUN;" >> $resultcsv
                    let "sum_notrun++"
                    ;;
                *)
                    echo "FAILED;$hwtag;$systag;$swtag;$reporter;$extra_comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
                    echo "FAILED; `cat case_${case_nro}/check.log`" >> $resultcsv
                    echo "$case_nro" >> list
                    let "sum_failed++"
                    ;;
                esac
            else
                cat ${TESTDIR}/case_${case_nro}/check.log
                echo ""
            fi
        fi
    elif [ $case_nro -ge 3000 ] && [ $case_nro -lt 4000 ]
    then

        . $test_case_list_dir/test_data_parameter_vp8.sh "$case_nro"

        if [ ${valid[${set_nro}]} -eq 0 ]
        then
            . checkcase_vp8.sh $case_nro
            res=$?
            extra_comment=${comment}" `cat ${TESTDIR}/case_${case_nro}/check.log`"

            getExecutionTime "case_$case_nro"

            if [ $CSV_REPORT -eq 1 ]
            then
                echo -n "case_$case_nro;Integration;VP8Case;;$language;$timeform1 $timeform2;$execution_time;" >> $csvfile.csv
                echo -n "case_$case_nro;" >> $resultcsv
                case $res in
                0)
                    echo "OK;$hwtag;$systag;$swtag;$reporter;$extra_comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
                    echo "OK;" >> $resultcsv
                    let "sum_ok++"
                    ;;
                3)
                    echo "NOT SUPPORTED;$hwtag;$systag;$swtag;$reporter;$extra_comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
                    echo "NOT SUPPORTED;" >> $resultcsv
                    let "sum_notrun++"
                    ;;
                4)
                    echo "NOT RUN;$hwtag;$systag;$swtag;$reporter;$extra_comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
                    echo "NOT RUN;" >> $resultcsv
                    let "sum_notrun++"
                    ;;
                *)
                    echo "FAILED;$hwtag;$systag;$swtag;$reporter;$extra_comment;;$kernelversion;$rootfsversion;$compilerversion;$testdeviceip;$testdeviceversion;" >> $csvfile.csv
                    echo "FAILED; `cat case_${case_nro}/check.log`" >> $resultcsv
                    echo "$case_nro" >> list
                    let "sum_failed++"
                    ;;
                esac
            else
                cat ${TESTDIR}/case_${case_nro}/check.log
                echo ""
            fi
        fi
    fi

done

echo ""
if [ $CSV_REPORT -eq 1 ]
then
    echo "Totals: $sum_ok OK   $sum_notrun NOT_RUN   $sum_failed FAILED"
    echo "Failed cases written to 'list'."
fi

rm -f random_run random.log random_log_done
