#!/bin/bash    

# Check script for comparing lab output to system model reference test data
# Checks output stream, encoding status, sw reg trace etc and writes check log
# Return 0 for success, -1 for fail, 3 for not supported, 4 for not failed (out of memory)

#Imports
. ./f_check.sh

importFiles

supportedPicWidth=$MAX_WIDTH
supportedPicHeight=$MAX_HEIGHT
roi=0
sei=0
    
let "case_nro=$1"
DECODER=./vp8x170dec_pclinux

# Dir where to save failed cases
FAILDIR=/export/tmp/$PRODUCT/${hwtag}_${reporttime}

# Reference test data dir
case_dir=$TEST_DATA_HOME/case_$case_nro

# Encoded output data dir
testcasedir=${TESTDIR}/case_${case_nro}
#testcasedir=${TESTDIR}/case_${case_nro}_1

# If random_run in progress, the TESTDIR will be in random_cases
if [ -e random_run ]
then
    echo -en "\rRandom VP8 Case $case_nro   "
else
    echo -en "\rVP8 Case $case_nro   "
fi

if [ ! -e $test_case_list_dir/test_data_parameter_vp8.sh ]
then
    echo "$test_case_list_dir/test_data_parameter_vp8.sh doesn't exist!"
    exit -1
fi

. $test_case_list_dir/test_data_parameter_vp8.sh "$case_nro"

set_nro=$((case_nro-case_nro/5*5))          
picWidth=$((width[set_nro]))
picHeight=$((height[set_nro]))
casetestId=$((testId[set_nro]))

# Create check.log that contains the messages of all checks.
rm -f ${testcasedir}/check.log

# Check if case is supported
if [ $picWidth -gt $supportedPicWidth ] || [ $picHeight -gt $supportedPicHeight ]; then
    echo -n "Unsupported picture width or height!" >> ${testcasedir}/check.log
    exit 3
fi

if [ "$INTERNAL_TEST" == "n" ] && [ $casetestId != "0" ]; then
    echo -n "Internal test, does not work with customer releases!" >> ${testcasedir}/check.log
    exit 3
fi

#stab=$((stabilization[set_nro]))
#if [ $stab -gt 0 ]; then
#    echo -n "Stabilization unsupported!" >> ${testcasedir}/check.log
#    exit 3
#fi

#rot=$((rotation[set_nro]))
#if [ $rot -gt 0 ]; then
#    echo -n "Rotation unsupported!" >> ${testcasedir}/check.log
#    exit 3
#fi

# If reference test data doesn't exist, generate it
if [ ! -e $case_dir ]
then
    (
    cd $SYSTEM_MODEL_HOME
    ./test_data/test_data.sh $case_nro >> /dev/null 2>&1
    sleep 1
    )
fi

decode()
{
    # param1 = stream file
    # param2 = output yuv

    if [ ! -e ${DECODER} ]
    then
        return
    fi

    ${DECODER} -P -O$2 $1
}

###############

# Check that reference stream exists
if [ -e ${case_dir}/stream.ivf ]
then
(
    #echo "$TESTDIR"

    # Wait until the case has been run
    while [ ! -e ${testcasedir}/status ]
    do
        echo -n "."
        sleep 10
    done

    fail=0

    # Check run status
    run_status="`cat ${testcasedir}/status`"
    if [ $run_status -eq 0 ]
    then
        echo -n "Run OK. " >> ${testcasedir}/check.log
    elif [ $run_status -eq 3 ]
    then
        echo -n "Not supported. " >> ${testcasedir}/check.log
        exit 3
    elif [ $run_status -eq 4 ] || [ $run_status -eq 6 ]
    then
        echo -n "Out of memory. " >> ${testcasedir}/check.log
        exit 4
    else
        echo -n "Run FAILED! " >> ${testcasedir}/check.log
    fi

    # Compare output to reference
    if (cmp -s ${testcasedir}/case_${case_nro}.ivf ${case_dir}/stream.ivf)
    then
        # Stream matches reference
        echo -n "Stream OK. " >> ${testcasedir}/check.log
    else
        # Stream doesn't match reference
        fail=-1
        echo -n "Stream FAILED! " >> ${testcasedir}/check.log

        saveFileToDir ${testcasedir}/case_${case_nro}.log $FAILDIR case_${case_nro}.log
        saveFileToDir ${testcasedir}/case_${case_nro}.ivf $FAILDIR case_${case_nro}.ivf
        saveFileToDir ${case_dir}/encoder.log $FAILDIR case_${case_nro}.log.ref
        saveFileToDir ${case_dir}/stream.ivf $FAILDIR case_${case_nro}.ivf.ref

        # Decode output stream
        decode ${testcasedir}/case_${case_nro}.ivf ${testcasedir}/case_$case_nro.yuv &> ${testcasedir}/dec_vp8.log
        if [ ! -s ${testcasedir}/case_${case_nro}.yuv ]
        then
            echo -n "case_${case_nro}.yuv doesn't exist! " >> ${testcasedir}/check.log
        else
            # Decode reference stream
            decode ${case_dir}/stream.ivf ${testcasedir}/ref_case_${case_nro}.yuv &> ${testcasedir}/dec_vp8.log
            if [ ! -s ${testcasedir}/ref_case_${case_nro}.yuv ]
            then
                echo -n "ref_case_${case_nro}.yuv doesn't exist! " >> ${testcasedir}/check.log
            fi
        fi

        # Compare output to reference
        findFailingPicture ${testcasedir}/case_$case_nro.yuv ${testcasedir}/ref_case_$case_nro.yuv >> ${testcasedir}/check.log

        if [ $? == 0 ]
        then
            rm -f ${testcasedir}/case_$case_nro.yuv ${testcasedir}/ref_case_$case_nro.yuv
        else
            saveFileToDir ${testcasedir}/case_${case_nro}.yuv $FAILDIR case_${case_nro}.yuv
            saveFileToDir ${testcasedir}/ref_case_${case_nro}.yuv $FAILDIR case_${case_nro}.yuv.ref
        fi
    fi

    # Check encoder log for partition size mismatch
    grep "ERROR: Frame size" ${testcasedir}/case_${case_nro}.log > /dev/null
    res=$?
    if [ $res -eq 1 ]
    then
        echo -n "Partition sizes OK. " >> ${testcasedir}/check.log
    else
        fail=-1
        echo -n "Partition sizes FAILED! " >> ${testcasedir}/check.log
        saveFileToDir ${testcasedir}/case_${case_nro}.log $FAILDIR case_${case_nro}.log
        saveFileToDir ${case_dir}/encoder.log $FAILDIR case_${case_nro}.log.ref
    fi


    # Compare MV output
    if (cmp -s ${testcasedir}/mv.txt ${case_dir}/mv.txt)
    #if (cmp -s ${testcasedir}/case_${case_nro}_mv.txt ${case_dir}/mv.txt)
    then
        echo -n "MV output OK. " >> ${testcasedir}/check.log
    else
        fail=-1
        echo -n "MV output FAILED! " >> ${testcasedir}/check.log
        saveFileToDir ${testcasedir}/mv.txt $FAILDIR case_${case_nro}_mv.txt
        saveFileToDir ${case_dir}/mv.txt $FAILDIR case_${case_nro}_mv.txt.ref
    fi

    # Compare register trace
    if ( [ "$TRACE_SW_REGISTER" == "y" ] )
    then
        # Parse register traces to get rid of differing regs
        removeBaseAddress "$testcasedir/sw_reg.trc"
        #removeBaseAddress "$testcasedir/case_${case_nro}_swreg.trc"
        removeBaseAddress "$case_dir/sw_reg.trc"

        if (cmp -s ${testcasedir}/sw_reg.trc ${case_dir}/sw_reg.trc)
        #if (cmp -s ${testcasedir}/case_${case_nro}_swreg.trc ${case_dir}/sw_reg.trc)
        then
            echo -n "Reg trace OK." >> ${testcasedir}/check.log
        else
            fail=-1
            echo -n "Reg trace FAILED!" >> ${testcasedir}/check.log
            saveFileToDir ${testcasedir}/case_${case_nro}.log $FAILDIR case_${case_nro}.log
            saveFileToDir ${testcasedir}/sw_reg.trc $FAILDIR case_${case_nro}_sw_reg.trc
            saveFileToDir ${case_dir}/encoder.log $FAILDIR case_${case_nro}.log.ref
            saveFileToDir ${case_dir}/sw_reg.trc $FAILDIR case_${case_nro}_sw_reg.trc.ref
        fi
    fi

    exit $fail
)
else
(
    echo -n "Reference stream.ivf missing!" >> ${testcasedir}/check.log
    exit -1
)
fi
 
