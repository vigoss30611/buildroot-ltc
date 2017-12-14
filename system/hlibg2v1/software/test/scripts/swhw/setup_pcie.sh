#!/bin/bash
#Copyright 2013 Google Inc. All Rights Reserved.
#Author: mkarki@google.com (Matti Kärki)
#
#Description: Set-up PCIe boards for HW decoder FPGA testing.

# 1) Copies the current sources to "/export/work/${USER}/g2_<date>"
# 2) Compiles the decoder binary, memalloc and driver
# 3) Loads the kernel modules

. ./defs.sh

# the SW directory on export
# TODO: change the suffix to SW tag
LOCAL_HOST_WDIR="${LOCAL_HOST_EXPORT_DIR}/work/${USER}"
now=`date +%F_%H-%M-%S`
LOCAL_HOST_SW_DIR="${LOCAL_HOST_WDIR}/g2_${now}"

REMOTE_HOST_SW_DIR=`echo $LOCAL_HOST_SW_DIR |awk -F\${LOCAL_HOST_EXPORT_PREFIX} '{print $2}'`
MAKE_DIR="${REMOTE_HOST_SW_DIR}/g2"
MEMALLOC_DIR="${REMOTE_HOST_SW_DIR}/g2/software/linux/memalloc"
DRIVER_DIR="${REMOTE_HOST_SW_DIR}/g2/software/linux/pcidriver"

BOARD_IP_BASE="172.28.107"
BOARD_IP_ADDRESSES="${BOARD_IP_BASE}.101 ${BOARD_IP_BASE}.102 ${BOARD_IP_BASE}.114"
OR_FEATURES="2160p/1080p axi/axiahb vhdl/verilog 64b/128b"
HW_FEATURES="vp9 hevc pp ${OR_FEATURES}"

MAKE_PARAMS="-j g2dec ENV=x86_linux_pci USE_ASIC_TRACE=n USE_MODEL_SIMULATION=n RELEASE=n WEBM_ENABLED=y NESTEGG=/export/work/sw_tools/nestegg/i386"
MAKE_PARAMS_MODEL="-j g2dec ENV=x86_linux USE_ASIC_TRACE=n USE_MODEL_SIMULATION=y RELEASE=y WEBM_ENABLED=y NESTEGG=/export/work/sw_tools/nestegg/i386"
#echo "HOX!HOX! using model binary"
#MAKE_PARAMS="$MAKE_PARAMS_MODEL"

MEM_BASE="0x20000000"
MEM_SIZE="512"

printHelp()
{
    echo -ne '\E[32m'
    echo ""
    echo "Command"
    tput sgr0
    echo "${0} [options]"
    echo ""
    echo -ne '\E[32m'
    echo "Options"
    tput sgr0
    echo "  -i \"<address:features>\"    the FPGA boards to set-up"
    echo "                               . valid adresses are: 101, 102, and 114"
    echo "                               . valid features are: vp9, hevc, pp + \
                                     2160p/1080p, axi/axiahb, vhdl/verilog, 64b/128b"
    echo "  -h                        print this help"
    echo ""
    echo -ne '\E[32m'
    echo "Examples"
    tput sgr0
    echo "${0} -i \"101:hevc,pp,2160p,axiahb,vhdl,64b 102:vp9,pp,1080p,axiahb,vhdl,128b 114:hevc,vp9,2160p,axi,verilog,128b\""
    exit 0
}

# command line parameters
address_features=""
while [ $# -gt 0 ]
do   
    if [ "$1" == "-i" ]
    then
        address_features="${2}"
        shift
        shift
        
    elif [ "$1" == "-h" ]
    then
        printHelp
        
    else
        printErrorAndExit "Unknown option: ${1}"
    fi
done
if [ -z "$address_features" ]
then
    printErrorAndExit "-i must be given!"
fi
# set the suffix to ips (also check that features and addresses are valid)
board_infos=""
# check that same address not given multiple times
ips=`echo $address_features |tr " " "\n" |awk -F\: '{print $1}'`
for ip in $ips
do
    address_count=`echo "$ips" |tr " " "\n" |grep -c -w ${ip}`
    if [ $address_count -gt 1  ]
    then
        printErrorAndExit "Address ${ip} given multiple times!"
    fi
done
for address_feature in $address_features
do
    # check address has valid value
    ip=`echo $address_feature |awk -F\: '{print $1}'`
    full_ip="${BOARD_IP_BASE}.${ip}"
    address_count=`echo "$BOARD_IP_ADDRESSES" |tr " " "\n" |grep -c -w ${full_ip}`
    if [ $address_count -eq 0 ]
    then
        printErrorAndExit "Not a valid address: ${ip}!"
    fi
    # check features    
    features=`echo $address_feature |awk -F\: '{print $2}'`
    full_ip_with_features="${full_ip}:${features}"
    features=`echo $features |tr "," " "`
    hw_features=`echo $HW_FEATURES |tr "/" " "`
    # first, check that same feature is not given multiple times
    for hw_feature in $hw_features
    do
        feature_count=`echo "$features" |tr " " "\n" |grep -c -w ${hw_feature}`
        if [ $feature_count -gt 1  ]
        then
            printErrorAndExit "Feature ${hw_feature} given multiple time for board ${ip}!"
        fi
    done
    for feature in $features
    do
        # next, check that feature has valid value
        feature_count=`echo "$hw_features" |tr " " "\n" |grep -c -w ${feature}`
        if [ $feature_count -eq 0 ]
        then
            printErrorAndExit "Not a valid feature ${feature} for board ${ip}!"
        fi
    done
    # finally, check that a value from each "or"-group is present
    # and check that no other values are present within same "or"-group
    for or_feature_group in $OR_FEATURES
    do
        or_features=`echo $or_feature_group |tr "/" "|"`
        feature_count=`echo "$features" |tr " " "\n" |grep -E -c -w "$or_features"`
        if [ $feature_count -eq 0 ]
        then
            printErrorAndExit "No feature ${or_feature_group} found for board ${ip}!"
        elif [ $feature_count -gt 1 ]
        then
            printErrorAndExit "Multiple features found from group ${or_feature_group} for board ${ip}!"
        fi
    done
    # add full address
    board_infos="${board_infos}${full_ip_with_features} "
done

echo "Setting up PCIe tester(s):"
for board_info in $board_infos
do
    echo "    $board_info"
done
sleep 2s
mountExportDir $LOCAL_HOST_WDIR

# copy the source to "export" using local mount
mkdir $LOCAL_HOST_SW_DIR
if [ $? -ne 0 ]
then
    printErrorAndExit "Cannot create ${LOCAL_HOST_SW_DIR}!"
fi
source_dir=`echo $PWD |awk -F\${SCRIPTS_DIR} '{print $1}'`
echo -n "Copying sources to ${LOCAL_HOST_SW_DIR}... "
cp -r $source_dir $LOCAL_HOST_SW_DIR
if [ $? -ne 0 ]
then
    printErrorAndExit "failed!"
fi
chmod -R 777 $LOCAL_HOST_SW_DIR
if [ $? -ne 0 ]
then
    printErrorAndExit "failed!"
fi
echo "OK"

# compile the decoder binary using remote execution
for board_info in $board_infos
do
    board_ip=`echo $board_info |awk -F\: '{print $1}'`
    # compile the decoder binary
    echo "Compiling decoder software @ ${board_ip}"
    echo -n "Executing \"make ${MAKE_PARAMS}\"... "
    ssh ${USER}@$board_ip "cd ${MAKE_DIR} && make clean > make.log 2>&1 && make ${MAKE_PARAMS} >> make.log 2>&1"
    if [ $? -ne 0 ]
    then
        cat ${LOCAL_HOST_EXPORT_PREFIX}/${MAKE_DIR}/make.log
        printErrorAndExit "Make failed: ${board_ip}:${MAKE_DIR}"
    fi
    echo "OK"
    decoder_bin=`grep "$DECODER_BIN" ${LOCAL_HOST_EXPORT_PREFIX}/${MAKE_DIR}/make.log |grep LINK |awk '{print $NF}'`
    decoder_bin="${LOCAL_HOST_EXPORT_PREFIX}/${MAKE_DIR}/${decoder_bin}"
    if [ ! -e $decoder_bin ]
    then
        printErrorAndExit "Cannot find decoder binary: ${decoder_bin}"
    fi
    target_decoder_bin="${LOCAL_HOST_EXPORT_PREFIX}/${MAKE_DIR}/${SCRIPTS_DIR}/${DECODER_BIN}"
    rm -f $target_decoder_bin
    cp $decoder_bin $target_decoder_bin
    if [ $? -ne 0 ]
    then
        printErrorAndExit "Cannot copy ${DECODER_BIN}"
    fi
    chmod 777 $target_decoder_bin
    if [ $? -ne 0 ]
    then
        printErrorAndExit "Cannot set permissions to ${DECODER_BIN}"
    fi
    
    # compile the model binary
    echo "Compiling model @ ${board_ip}"
    echo -n "Executing \"make ${MAKE_PARAMS_MODEL}\"... "
    ssh ${USER}@$board_ip "cd ${MAKE_DIR} && make clean > make.log 2>&1 && make ${MAKE_PARAMS_MODEL} >> make.log 2>&1"
    if [ $? -ne 0 ]
    then
        cat ${LOCAL_HOST_EXPORT_PREFIX}/${MAKE_DIR}/make.log
        printErrorAndExit "Make failed: ${board_ip}:${MAKE_DIR}"
    fi
    echo "OK"
    # model is actually same name as the decoder
    decoder_bin=`grep "$DECODER_BIN" ${LOCAL_HOST_EXPORT_PREFIX}/${MAKE_DIR}/make.log |grep LINK |awk '{print $NF}'`
    decoder_bin="${LOCAL_HOST_EXPORT_PREFIX}/${MAKE_DIR}/${decoder_bin}"
    if [ ! -e $decoder_bin ]
    then
        printErrorAndExit "Cannot find model binary: ${decoder_bin}"
    fi
    target_decoder_bin="${LOCAL_HOST_EXPORT_PREFIX}/${MAKE_DIR}/${SCRIPTS_DIR}/${MODEL_BIN}"
    rm -f $target_decoder_bin
    cp $decoder_bin $target_decoder_bin
    if [ $? -ne 0 ]
    then
        printErrorAndExit "Cannot copy ${MODEL_BIN}"
    fi
    chmod 777 $target_decoder_bin
    if [ $? -ne 0 ]
    then
        printErrorAndExit "Cannot set permissions to ${MODEL_BIN}"
    fi

    # compile the memalloc using remote execution
    echo -n "Building memalloc... "
    ssh ${USER}@$board_ip "cd ${MEMALLOC_DIR} && make clean > make.log 2>&1 && ./build_for_pcie.sh >> make.log 2>&1"
    if [ $? -ne 0 ]
    then
        cat ${LOCAL_HOST_EXPORT_PREFIX}/${MEMALLOC_DIR}/make.log
        printErrorAndExit "Build failed: ${board_ip}:${MEMALLOC_DIR}"
    fi
    echo "OK"

    # compile the driver using remote execution
    echo -n "Building driver... "
    ssh ${USER}@$board_ip "cd ${DRIVER_DIR} && make clean > make.log 2>&1 && make >> make.log 2>&1"
    if [ $? -ne 0 ]
    then
        cat ${LOCAL_HOST_EXPORT_PREFIX}/${DRIVER_DIR}/make.log
        printErrorAndExit "Build failed: ${board_ip}:${DIRVER_DIR}"
    fi
    echo "OK"
    
    # compile step is required only on one board
    break
done

rm -f $SETUP_INFO
# compile the decoder binary using remote execution
for board_info in $board_infos
do
    board_ip=`echo $board_info |awk -F\: '{print $1}'`
    board_features=`echo $board_info |awk -F\: '{print $2}'`
    echo "Loading kernel modules @ ${board_ip}"
    # load the memalloc using the remote execution
    echo -n "Loading memalloc... "
    ssh ${USER}@$board_ip "cd ${MEMALLOC_DIR} && sudo ./memalloc_load.sh alloc_base=${MEM_BASE} alloc_size=${MEM_SIZE} > load.log 2>&1"
    if [ $? -ne 0 ]
    then
        cat ${LOCAL_HOST_EXPORT_PREFIX}/${MEMALLOC_DIR}/load.log
        #printErrorAndExit "Load failed: ${board_ip}:${MEMALLOC_DIR}"
        echo "Load failed: ${board_ip}:${MEMALLOC_DIR} -> not using board!"
        continue
    fi
    echo "OK"

    # load the driver using the remote execution
    echo -n "Loading driver... "
    ssh ${USER}@$board_ip "cd ${DRIVER_DIR} && sudo ../ldriver/driver_load.sh  > load.log 2>&1"
    if [ $? -ne 0 ]
    then
        cat ${LOCAL_HOST_EXPORT_PREFIX}/${DRIVER_DIR}/load.log
        #printErrorAndExit "Load failed: ${board_ip}:${DRIVER_DIR}"
        echo "Load failed: ${board_ip}:${DRIVER_DIR} -> not using board!"
        continue
    fi
    echo "OK"

    set_up_dir=`echo $LOCAL_HOST_SW_DIR |awk -F\${LOCAL_HOST_EXPORT_PREFIX} '{print $2}'`
    echo "${board_ip}:${set_up_dir}:${board_features}" >> $SETUP_INFO
done
cp $SETUP_INFO ${LOCAL_HOST_EXPORT_PREFIX}/${MAKE_DIR}/${SCRIPTS_DIR}
if [ $? -ne 0 ]
then
    printErrorAndExit "Cannot copy $SETUP_INFO!"
fi
chmod 777 ${LOCAL_HOST_EXPORT_PREFIX}/${MAKE_DIR}/${SCRIPTS_DIR}/${SETUP_INFO}

exit 0
