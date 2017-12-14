#!/bin/bash

WORKSPACE=`pwd`
TOOLCHAIN_PREFIX=`basename output/host/usr/arm-*`
P_REL=qsdk_sdk_`date +"%y%m%d"`
P_REL_ABS=`pwd`/${P_REL}

rel_init_rel_dir(){
    #echo -e "--> init release dir "${P_REL_ABS}
    rm -rf ${P_REL_ABS}
    mkdir -p  ${P_REL_ABS}/kernel
    mkdir -p  ${P_REL_ABS}/uboot
    mkdir -p  ${P_REL_ABS}/busybox
    mkdir -p  ${P_REL_ABS}/products
    mkdir -p  ${P_REL_ABS}/toolchain
    mkdir -p  ${P_REL_ABS}/packages
    mkdir -p  ${P_REL_ABS}/private
    mkdir -p  ${P_REL_ABS}/tools
    mkdir -p  ${P_REL_ABS}/libs
    mkdir -p  ${P_REL_ABS}/file_system/system
    mkdir -p  ${P_REL_ABS}/file_system/ramdisk
    #tree ${P_REL_ABS}
}

rel_host_toolchain(){
    echo -e "-->release host toolchain"
    cd ${WORKSPACE}
    TOPDIR=${WORKSPACE}
    toolchain_dir=`cat .config | grep "BR2_TOOLCHAIN_EXTERNAL_PATH" | awk -F '/' '{print $4}' | awk -F '"' '{print $1}'`
    #echo -e "\tverison ->"${toolchain_dir}
	mkdir -p ${P_REL_ABS}/toolchain/usr
	mkdir -p ${P_REL_ABS}/toolchain/sbin
    cp -rf buildroot/prebuilts/${toolchain_dir}/*  ${P_REL_ABS}/toolchain/usr
    TOOLCHAIN_PREFIX=`basename output/host/usr/arm-*`
    cd ${P_REL_ABS}/toolchain/usr/${TOOLCHAIN_PREFIX}/sysroot/lib
    thread_link=`ls libpthread.so*`
    ln -sf ${thread_link} libpthread.so
    cd ${WORKSPACE}
}

rel_host_extra(){
    echo -e "-->release host extra"
    cd ${WORKSPACE}
    cp -rf ${WORKSPACE}/buildroot/support/scripts/mkusers  ${P_REL_ABS}/toolchain/usr/bin/
    cp -rf ${WORKSPACE}/buildroot/fs/ext2/genext2fs.sh  ${P_REL_ABS}/toolchain/usr/bin/
    cp -rf ${WORKSPACE}/output/host/usr/bin/mkimage   ${P_REL_ABS}/toolchain/usr/bin/
    cp -rf ${WORKSPACE}/output/host/usr/bin/makedevs   ${P_REL_ABS}/toolchain/usr/bin/
    cp -rf ${WORKSPACE}/output/host/usr/bin/iuw   ${P_REL_ABS}/toolchain/usr/bin/
    #echo -e "\t -->check bin"
    for i in `ls output/host/usr/bin/`
    do
       if [[ ! -e ${P_REL_ABS}/toolchain/usr/bin/${i} ]];then
            #echo -e "\t\twill be add -->  "${i}
            cp  output/host/usr/bin/${i}  ${P_REL_ABS}/toolchain/usr/bin
       fi
    done

    cd ${P_REL_ABS}/toolchain/sbin
    ln -sf ../usr/kmod depmod
    cd ${WORKSPACE}

    #echo -e "\t -->check lib"
    for i in `ls output/host/usr/lib/`
    do
       if [[ ! -e ${P_REL_ABS}/toolchain/usr/lib/${i} || ! -d ${P_REL_ABS}/toolchain/usr/lib/${i} ]];then
            #echo -e "\t\twill be add -->  "${i}
            cp -rf  output/host/usr/lib/${i}  ${P_REL_ABS}/toolchain/usr/lib
       fi
    done
    #echo -e "\t -->check share"
    for i in `ls output/host/usr/share/`
    do
       if [[ ! -e ${P_REL_ABS}/toolchain/usr/share/${i} || ! -d ${P_REL_ABS}/toolchain/usr/share/${i} ]];then
            #echo -e "\t\twill be add -->  "${i}
            cp -rf output/host/usr/share/${i}  ${P_REL_ABS}/toolchain/usr/share
       fi
    done
}

rel_system_files(){
    echo -e "-->release system files"
    cd ${WORKSPACE}
    cp -rf output/system/*  ${P_REL_ABS}/file_system/system/
    cp -rf output/staging/*  ${P_REL_ABS}/file_system/system/
    cd ${P_REL_ABS}/file_system/system
    cp -rf usr/local/lib/*  usr/lib/
    cp -rf usr/local/include/*  usr/include/
    rm -rf usr/local
    rm -rf lib/modules/*
    find usr/lib/pkgconfig  -name "*.pc" | xargs  -i  sed -i "s/\/local//g"  {}
    rm -rf usr/local
    rm -rf usr/share
    rm -rf THIS_IS_NOT_YOUR_ROOT_FILESYSTEM
    rm -rf usr/lib/alsa-lib
    rm -rf usr/lib/engins
    rm -rf usr/lib/libpthread*
    rm -rf usr/lib/libform*
    rm -rf usr/lib/libpanel*

    rel_clean_busybox_files ${P_REL_ABS}/file_system/system/
}

rel_repo_source(){
    echo -e "-->release source "$1
    mkdir -p ${WORKSPACE}/$1
    cd ${WORKSPACE}/$1
    mkdir -p ${P_REL_ABS}/$1
    git archive HEAD | tar -x -C ${P_REL_ABS}/$1
    git log -1  > ${P_REL_ABS}/$1/GIT_VERSION
}

rel_bootloader_fix(){
    cd ${WORKSPACE}/
    board_cpu=`cat output/images/items.itm |grep board.cpu | awk '{print $2}'`
    cd ${P_REL_ABS}
    echo ${board_cpu}  > bootloader/board_cpu
    mv bootloader/${board_cpu}  bootloader/uboot0
    rm -rf bootloader/apollo*
    rm -rf uboot
    mv bootloader uboot
}

rel_busybox(){
    echo -e "-->release busybox"
    cd ${WORKSPACE}
    ver=`cat .config |grep BUSYBOX_VERSION= |awk -F '"' '{print $2}'`
    #echo -e "\t\t busybox version -> "${ver}
    tar xf buildroot/download/busybox-${ver}.tar.bz2 -C  ${P_REL_ABS}
    rm -rf ${P_REL_ABS}/busybox
    mv ${P_REL_ABS}/busybox-${ver}  ${P_REL_ABS}/busybox
    echo ${ver}  > ${P_REL_ABS}/busybox/version
}

rel_busybox_extra(){
    echo -e "-->release busybox extra"
}

rel_tools_extra(){
    echo -e "-->release tools extra"
    mv ${P_REL_ABS}/tools/sdk/qsdk.*  ${P_REL_ABS}/tools/
}

rel_custom_packages(){
    echo -e "-->release custom package"
    cd ${WORKSPACE}

	if [[ -e "output/product/package.txt" ]]; then
		cat output/product/package.txt | while read line
		do
			echo -e "\t-->release package ${line}"
			cp -rf system/${line}  ${P_REL_ABS}/packages/
		done
	fi
}

rel_product_fix(){
    echo -e "-->release product fix"
    cd ${WORKSPACE}
    #echo -e "\tremove unrelevant products:"
    for i in `ls output/product -l`
    do
        PRODUCT=`basename $i 2>/dev/null`
    done
    cd ${P_REL_ABS}/products
    for i in `ls`; do
	    if [ "$i" != "${PRODUCT}" ] && [ "$i" != "GIT_VERSION" ]; then
		    rm $i -rf
            #echo -e  "\t\t$i removed"
	    fi
    done
    #echo -e "\n\tfix module install script"
    #ls  ${P_REL_ABS}/file_system/system/etc/init.d/  | xargs -i ${WORKSPACE}/tools/sdk/kofix   ${P_REL_ABS}/file_system/system/etc/init.d/{}
    #scripts_list=`grep "modprobe"  -R  ${P_REL_ABS}/products |awk -F ':' '{print $1}'`
    #for i in ${scripts_list}
    #do
        #if [[ -e $i ]] ;then
            #${WORKSPACE}/tools/sdk/kofix  $i  |xargs -i echo -e "\t\t"{}
        #fi
    #done
    #scripts_list=`grep "modprobe"  -R  ${P_REL_ABS}/file_system/system/etc/init.d/ |awk -F ':' '{print $1}'`
    #for i in ${scripts_list}
    #do
        #if [[ -e $i ]] ;then
            #${WORKSPACE}/tools/sdk/kofix  $i  |xargs -i echo -e "\t\t"{}
        #fi
    #done
    cp -rf ${WORKSPACE}/output/build/linux-local/.config  ${P_REL_ABS}/products/${PRODUCT}/configs/linux_defconfig
    cp -rf ${WORKSPACE}/output/build/busybox-1.21.1/.config  ${P_REL_ABS}/products/${PRODUCT}/configs/busybox_defconfig
    cp -rf ${WORKSPACE}/output/build/bblite-1.21.1/.config  ${P_REL_ABS}/products/${PRODUCT}/configs/bblite_defconfig
}

rel_clean_busybox_files(){
    #echo -e "\t clean busybox rootfs files ->"$1
    cd $1
    old_path=`pwd`
    cd ${old_path}/bin
    for i in `ls`
    do
        if [[ "busybox" == `readlink $i` ]]; then
            #echo "delete ->"$i
            rm $i
        fi
    done
    cd ${old_path}/sbin
    for i in `ls`
    do
        if [[ "../bin/busybox" == `readlink $i` ]]; then
            #echo "delete ->"$i
            rm $i
        fi
    done
    cd ${old_path}/usr/bin
    for i in `ls`
    do
        if [[ "../../bin/busybox" == `readlink $i` ]]; then
            #echo "delete ->"$i
            rm $i
        fi
    done
    cd ${old_path}/usr/sbin
    for i in `ls`
    do
        if [[ "../../bin/busybox" == `readlink $i` ]]; then
            #echo "delete ->"$i
            rm $i
        fi
    done
    rm -rf ${old_path}/bin/busybox
    rm -rf ${old_path}/linuxrc
}

rel_ramdisk(){
    echo -e "-->release ramdisk"
    cd ${WORKSPACE}
    cp  -rf  output/root/*    ${P_REL_ABS}/file_system/ramdisk
    rel_clean_busybox_files  ${P_REL_ABS}/file_system/ramdisk
}

rel_private_modules(){
    echo -e "-->release private modules"
    cd ${WORKSPACE}
    find output/system  -name "*.ko"  | xargs -i cp -rf {}  ${P_REL_ABS}/private/
}

rel_libs(){
	echo -e "-->release library"
	cd ${WORKSPACE}

	pack=(`ls system`)
	pack1=(`ls ${P_REL_ABS}/packages`)
	l=${#pack[@]}
	l1=${#pack1[@]}
	for((i=0;i<${l1};i++)); do
		for((j=0;j<${l};j++)); do
			if [[ ${pack1[i]} = ${pack[j]} ]]; then
				unset pack[j]
			fi
		done
	done

	for i in ${pack[@]}; do
		for j in `ls output/build/`; do
			if [ "$i" == "${j:0:${#i}}" ]; then
				echo -e "\t-->release library ${i}"
				mkdir -p ${P_REL_ABS}/libs/$i
				mkdir -p ${P_REL_ABS}/libs/${i}/include
				mkdir -p ${P_REL_ABS}/libs/${i}/lib/pkgconfig
				mkdir -p ${P_REL_ABS}/libs/${i}/resource/bin
				mkdir -p ${P_REL_ABS}/libs/${i}/resource/root
				mkdir -p ${P_REL_ABS}/libs/${i}/resource/other
				if [[ -e system/${i}/Makefile.am ]]; then
					if [[ ${i} != "abctrl2" && ${i} != "app-d304-bt" && ${i} != "app-d304-main" && ${i} != "app-d304-upgrade" && \
					    ${i} != "app-d304-ft" && ${i} != "app-boot-selector" && ${i} != "cep" && ${i} != "hlibvcp7g" ]]; then
						if [[ ${i} = "eventhub" ]]; then
							cp -dp ${P_REL_ABS}/file_system/system/usr/bin/eventsend ${P_REL_ABS}/libs/${i}/resource/bin
							cp -dp ${P_REL_ABS}/file_system/system/usr/bin/eventhub ${P_REL_ABS}/libs/${i}/resource/bin
						elif [[ ${i} = "specsbt" ]]; then
							cp -dp ${P_REL_ABS}/file_system/system/usr/bin/specsbt ${P_REL_ABS}/libs/${i}/resource/bin
						elif [[ ${i} = "hlibdsp" ]]; then
							cp -dp ${P_REL_ABS}/file_system/system/usr/bin/test_libdsp ${P_REL_ABS}/libs/${i}/resource/bin
						elif [[ ${i} = "hlibispostv1" ]]; then
							cp -dp ${P_REL_ABS}/file_system/system/usr/bin/test_libispost ${P_REL_ABS}/libs/${i}/resource/bin
						elif [[ ${i} = "qlibfastboot" ]]; then
							cp -dp ${P_REL_ABS}/file_system/system/usr/bin/fastbootd ${P_REL_ABS}/libs/${i}/resource/bin
						fi
						name=`cat system/${i}/Makefile.am | grep "^lib_LTLIBRARIES" | awk -F '.' '{print $1}'| awk -F '=' '{print $2}'`
						libname=$(echo $name)
						cp -dp ${P_REL_ABS}/file_system/system/usr/lib/${libname}.* ${P_REL_ABS}/libs/${i}/lib
						I=`echo $i | tr '[a-z]' '[A-Z]'`

						if [[ ${i} = "libgridtools" ]];then
							f=`cat .config | grep "BR2_PACKAGE_LIBGRIDTOOLS_SHARED=y"`
							if [[ -n $f ]];then
								sed -n "/define ${I}_POST_INSTALL_STAGING_HEADERS2/,/endef/p" system/${i}/${i}.mk > tmp.sh
							else
								sed -n "/define ${I}_POST_INSTALL_STAGING_HEADERS/,/endef/p" system/${i}/${i}.mk > tmp.sh
							fi
						else
							sed -n "/define ${I}_POST_INSTALL_STAGING_HEADERS/,/endef/p" system/${i}/${i}.mk > tmp.sh
						fi
						sed -n "/define ${I}_POST_INSTALL_TARGET_SHARED/,/endef/p" system/${i}/${i}.mk >> tmp.sh
						sed -i -e "/define/d" -e "/endef/d" -e "/echo/d" -e "/\.libs/d" -e "/TARGET_INITRD_DIR/d" -e "s/^[ \t]*//g" tmp.sh
						sed -i "s;\$(STAGING_DIR)/usr;\${P_REL_ABS}/libs/${i};g" tmp.sh
						sed -i "s;\$(TARGET_DIR)/root;\${P_REL_ABS}/libs/${i}/resource/root;g" tmp.sh
						sed -i "s;\$(@D);output/build/${j};g" tmp.sh
						if [[ ${i} = "hlibispostv2" ]]; then
							sed -i "s;\$(TOPDIR);\${WORKSPACE};g" tmp.sh
							sed -i "s;\$\$(ls;\`ls;g" tmp.sh
							sed -i "s;\$\$NF}');\$NF}'\`;g" tmp.sh
						elif [[ ${i} = "qlibwifi" ]]; then
							sed -i "s;\$(TARGET_DIR);\${P_REL_ABS}/libs/qlibwifi/resource/other;g" tmp.sh
							wifi_mode=`cat .config | grep "BR2_WIFI_MODE" | awk -F '=' '{print $2}'`
							wifi_hardware_module=`cat .config | grep "\<BR2_WIFI_HARDWARE_MODULE\>" | awk -F '=' '{print $2}'`
							sed -i "1i\WIFI_MODE=${wifi_mode}\nWIFI_HARDWARE_MODULE=${wifi_hardware_module}" tmp.sh
							sed -i -e 's/(WIFI_MODE)/{WIFI_MODE}/g' -e 's/(WIFI_HARDWARE_MODULE)/{WIFI_HARDWARE_MODULE}/g' tmp.sh
						fi
					elif [[ ${i} = "app-d304-bt" || ${i} = "app-d304-main" || ${i} = "abctrl2" || ${i} = "app-boot-selector" || \
						${i} = "app-d304-ft" ]]; then
						bin=`cat system/${i}/Makefile.am | grep "bin_PROGRAMS" | awk -F '=' '{print $2}'`
						cp -dp ${P_REL_ABS}/file_system/system/usr/bin/${bin} ${P_REL_ABS}/libs/${i}/resource/bin
						I=`echo $i | tr '[a-z]' '[A-Z]'`
						I=`echo $I | awk -F '-' '{print $3}'`
						sed -n "/define APP_D304_${I}_POST_INSTALL_TARGET_SHARED/,/endef/p" system/${i}/${i}.mk > tmp.sh
						sed -i -e "/define/d" -e "/endef/d" -e "/echo/d" -e "s/^[ \t]*//g" tmp.sh
						sed -i "s;\$(TARGET_DIR)/root;\${P_REL_ABS}/libs/${i}/resource/root;g" tmp.sh
						sed -i "s;\$(TARGET_DIR)/usr/bin;\${P_REL_ABS}/libs/${i}/resource/bin;g" tmp.sh
						sed -i "s;\$(TARGET_DIR)/etc/init.d;\${P_REL_ABS}/libs/${i}/resource/other;g" tmp.sh
						sed -i "s;\$(TARGET_DIR);\${P_REL_ABS}/libs/${i}/resource/other;g" tmp.sh
						sed -i "s;\$(@D);output/build/${j};g" tmp.sh
					elif [[ ${i} = "app-d304-upgrade" || ${i} = "cep" ]]; then
						bin=`cat system/${i}/Makefile.am | grep "bin_PROGRAMS" | awk -F '=' '{print $2}'`
						cp -dp ${P_REL_ABS}/file_system/system/usr/bin/${bin} ${P_REL_ABS}/libs/${i}/resource/bin
						I=`echo $i | tr '[a-z]' '[A-Z]'`
						sed -n "/define ${I}_POST_INSTALL_STAGING_HEADERS/,/endef/p" system/${i}/${i}.mk > tmp.sh
						sed -i -e "/define/d" -e "/endef/d" -e "/echo/d" -e "s/^[ \t]*//g" tmp.sh
						sed -i "s;\$(STAGING_DIR)/usr;\${P_REL_ABS}/libs/${i};g" tmp.sh
						sed -i "s;\$(@D);output/build/${j};g" tmp.sh
					elif [[ ${i} = "app-libra-dana" || ${i} = "app-libra-main" || ${i} = "app-launcher" ]]; then
						if [[ ${i} = "app-launcher" ]];then
							cp -dp ${P_REL_ABS}/file_system/system/usr/bin/launcher ${P_REL_ABS}/libs/${i}/resource/bin
							cp -dp ${P_REL_ABS}/file_system/system/usr/bin/upgrade ${P_REL_ABS}/libs/${i}/resource/bin
							cp -dp ${P_REL_ABS}/file_system/system/usr/bin/pcbtest_server ${P_REL_ABS}/libs/${i}/resource/bin
							cp -dp ${P_REL_ABS}/file_system/system/usr/bin/ota_upgrade ${P_REL_ABS}/libs/${i}/resource/bin
						else
							bin=`cat system/${i}/Makefile.am | grep "bin_PROGRAMS" | awk -F '=' '{print $2}'`
							cp -dp ${P_REL_ABS}/file_system/system/usr/bin/${bin} ${P_REL_ABS}/libs/${i}/resource/bin
						fi
						I=`echo $i | tr '[a-z]' '[A-Z]'`
						I=`echo $I | awk -F '-' '{print $1_$2_$3}'`
						sed -n "/define ${I}_POST_INSTALL_STAGING_HEADERS/,/endef/p" system/${i}/${i}.mk > tmp.sh
						sed -n "/define ${I}_POST_INSTALL_TARGET_SHARED/,/endef/p" system/${i}/${i}.mk >> tmp.sh
						sed -i -e "/define/d" -e "/endef/d" -e "/echo/d" -e "s/^[ \t]*//g" tmp.sh
						sed -i "s;-cp;cp;g" tmp.sh
						sed -i "s;-mkdir;cp;g" tmp.sh
						sed -i "s;\$(STAGING_DIR)/usr;\${P_REL_ABS}/libs/${i};g" tmp.sh
						sed -i "s;\$(TARGET_DIR)/opt;\${P_REL_ABS}/libs/${i}/resource/resource/other;g" tmp.sh
						sed -i "s;\$(TARGET_DIR)/etc/init.d;\${P_REL_ABS}/libs/${i}/resource/other;g" tmp.sh
						sed -i "s;\$(TARGET_DIR)/etc;\${P_REL_ABS}/libs/${i}/resource/other;g" tmp.sh
						sed -i "s;\$(@D);output/build/${j};g" tmp.sh
					elif [[ ${i} = "hlibvcp7g" ]]; then
						name=`cat system/${i}/Makefile.am | grep "^lib_LIBRARIES" | awk -F '.' '{print $1}'| awk -F '=' '{print $2}'`
						libname=$(echo $name)
						cp -dp ${P_REL_ABS}/file_system/system/usr/lib/${libname}.* ${P_REL_ABS}/libs/${i}/lib
						bin=`cat system/${i}/Makefile.am | grep "bin_PROGRAMS" | awk -F '=' '{print $2}'`
						bin=$(echo $bin)
						cp -dp ${P_REL_ABS}/file_system/system/usr/bin/${bin} ${P_REL_ABS}/libs/${i}/resource/bin
						I=`echo $i | tr '[a-z]' '[A-Z]'`
						sed -n "/define ${I}_POST_INSTALL_TARGET_HEADER/,/endef/p" system/${i}/${i}.mk > tmp.sh
						vcp_arm=`cat .config | grep "BR2_PACKAGE_HLIBVCP7G_ARM_SUPPORT=y"`
						vcp_dsp=`cat .config | grep "BR2_PACKAGE_HLIBVCP7G_DSP_SUPPORT=y"`
						if [[ -n ${vcp_arm} ]]; then
							sed -n "/define ARM_EXTRA_INSTALL/,/endef/p" system/${i}/${i}.mk >> tmp.sh
						fi
						if [[ -n ${vcp_dsp} ]]; then
							sed -n "/define DSP_EXTRA_INSTALL/,/endef/p" system/${i}/${i}.mk >> tmp.sh
						fi
						if [[ -n ${vcp_arm} && -n ${vcp_dsp} ]]; then
							sed -n "/define EXTRA_INSTALL/,/endef/p" system/${i}/${i}.mk >> tmp.sh
						fi
						sed -i -e "/define/d" -e "/endef/d" -e "/echo/d" -e "s/^[ \t]*//g" tmp.sh
						sed -i "s;\$(STAGING_DIR)/usr;\${P_REL_ABS}/libs/${i};g" tmp.sh
						sed -i "s;\$(@D);output/build/${j};g" tmp.sh
					fi
				elif [[ -e system/${i}/CMakeLists.txt ]]; then
					n=`cat system/${i}/CMakeLists.txt | grep "^install" | awk 'END{print NR}'`
					for ((k=1;k<=${n};k++))
					do
						key=`cat system/${i}/CMakeLists.txt | grep "^install" | awk "NR==$k"'{print $4}'`
						name=`cat system/${i}/CMakeLists.txt | grep "^install" | awk "NR==$k"'{print $2}'`
						if [[ $key = "bin)" ]]; then
							cp -dp ${P_REL_ABS}/file_system/system/usr/bin/${name} ${P_REL_ABS}/libs/${i}/resource/bin
						fi
						if [[ $key = "lib)" ]]; then
							if [[ $name = "audiobox_static" ]]; then
								cp -dp ${P_REL_ABS}/file_system/system/usr/lib/libaudiobox.* ${P_REL_ABS}/libs/${i}/lib
							elif [[ $name =~ "_" ]]; then
								continue
							else
								cp -dp ${P_REL_ABS}/file_system/system/usr/lib/lib${name}.* ${P_REL_ABS}/libs/${i}/lib
							fi
						fi
					done
					I=`echo $i | tr '[a-z]' '[A-Z]'`
					sed -n "/define ${I}_POST_INSTALL_STAGING/,/endef/p" system/${i}/${i}.mk > tmp.sh
					sed -i -e "/define/d" -e "/endef/d" -e "/\.so/d" -e "s/^[ \t]*//g" tmp.sh
					sed -i "s;\$(STAGING_DIR)/usr;\${P_REL_ABS}/libs/${i};g" tmp.sh
					sed -i "s;\$(TARGET_DIR)/root;\${P_REL_ABS}/libs/${i}/resource/root;g" tmp.sh
					sed -i "s;\$(${I}_SRCDIR);system/${i};g" tmp.sh
				elif [[ ${i} = "hlibispv2505" || ${i} = "hlibispv2500" ]]; then
					if [[ ${i} = "hlibispv2505" ]];then
						f=5
					else
						f=0
					fi
					cp -dp system/hlibispv250${f}/DDKSource/changelist.txt ${P_REL_ABS}/libs/${i}/resource/bin
					I=`echo $i | tr '[a-z]' '[A-Z]'`
					sed -n "/define ${I}_POST_INSTALL_STAGING_HEADERS/,/endef/p" system/${i}/${i}.mk > tmp.sh
					mneon=`cat .config | grep "BR2_PACKAGE_LIBISPV250${f}_MNEON=y"`
					if [[ -n ${mneon} ]]; then
						sed -n "/define HLIBISPV250${f}_POST_INSTALL_STAGING_MNEON_FIX/,/endef/p" system/${i}/${i}.mk >> tmp.sh
					fi
					sed -i -e "/define/d" -e "/endef/d" -e "/echo/d" -e "s/^[ \t]*//g" tmp.sh
					sed -i "s;\$(STAGING_DIR)/usr;\${P_REL_ABS}/libs/${i};g" tmp.sh
					sed -i "s;\$(HLIBISPV250${f}_SUBDIR);DDKSource;g" tmp.sh
					sed -i "s;\$(shell pwd)\/;;g" tmp.sh
					sed -i "s;\$(@D);output/build/${j};g" tmp.sh
				elif [[ ${i} = "libffmpeg" ]]; then
					cp -dp ${P_REL_ABS}/file_system/system/usr/lib/libavcodec.* ${P_REL_ABS}/libs/${i}/lib
					cp -dp ${P_REL_ABS}/file_system/system/usr/lib/libavformat.* ${P_REL_ABS}/libs/${i}/lib
					cp -dp ${P_REL_ABS}/file_system/system/usr/lib/libswscale.* ${P_REL_ABS}/libs/${i}/lib
					cp -dp ${P_REL_ABS}/file_system/system/usr/lib/libswresample.* ${P_REL_ABS}/libs/${i}/lib
					cp -dp ${P_REL_ABS}/file_system/system/usr/lib/libavutil.* ${P_REL_ABS}/libs/${i}/lib

					cp -dp ${P_REL_ABS}/file_system/system/usr/lib/pkgconfig/libavcodec.pc ${P_REL_ABS}/libs/${i}/lib/pkgconfig
					cp -dp ${P_REL_ABS}/file_system/system/usr/lib/pkgconfig/libavformat.pc ${P_REL_ABS}/libs/${i}/lib/pkgconfig
					cp -dp ${P_REL_ABS}/file_system/system/usr/lib/pkgconfig/libswscale.pc ${P_REL_ABS}/libs/${i}/lib/pkgconfig
					cp -dp ${P_REL_ABS}/file_system/system/usr/lib/pkgconfig/libswresample.pc ${P_REL_ABS}/libs/${i}/lib/pkgconfig
					cp -dp ${P_REL_ABS}/file_system/system/usr/lib/pkgconfig/libavutil.pc ${P_REL_ABS}/libs/${i}/lib/pkgconfig

					mkdir -p ${P_REL_ABS}/libs/${i}/lib/include/libavcodec
					cp -dprf ${P_REL_ABS}/file_system/system/usr/include/libavcodec/* ${P_REL_ABS}/libs/${i}/lib/include/libavcodec
					cp -dprf ${P_REL_ABS}/file_system/system/usr/include/libavformat ${P_REL_ABS}/libs/${i}/lib/include
					cp -dprf ${P_REL_ABS}/file_system/system/usr/include/libswscale ${P_REL_ABS}/libs/${i}/lib/include
					cp -dprf ${P_REL_ABS}/file_system/system/usr/include/libswresample ${P_REL_ABS}/libs/${i}/lib/include
					cp -dprf ${P_REL_ABS}/file_system/system/usr/include/libavutil ${P_REL_ABS}/libs/${i}/lib/include
					touch tmp.sh
				elif [[ ${i} = "libfreetype" ]]; then
					cp -dp ${P_REL_ABS}/file_system/system/usr/lib/libfreetype.* ${P_REL_ABS}/libs/${i}/lib
					cp -dp ${P_REL_ABS}/file_system/system/usr/bin/freetype-config ${P_REL_ABS}/libs/${i}/resource/bin
					touch tmp.sh
				elif [[ ${i} = "bblite" ]]; then
					touch tmp.sh
				fi
				sed -i '1i\#!/bin/bash' tmp.sh
				if [[ ${i} = "qlibwifi" ]];then
					sed -i '/cp.*$/s//& > \/dev\/null/g' tmp.sh
					sed -i 's/; \\ > \/dev\/null/ > \/dev\/null ; \\/g' tmp.sh
					sed -i 's/; fi fi > \/dev\/null/ > \/dev\/null ; fi fi/g' tmp.sh
				else
					sed -i '/cp.*$/s//& > \/dev\/null/g' tmp.sh
				fi
				chmod a+x tmp.sh
				source tmp.sh
				rm tmp.sh
			fi
		done
	done
}

rel_libs_extra(){
	echo -e "-->release library extra"
	cd ${P_REL_ABS}/libs
	for i in `ls`; do
		if [[ "`ls ${i}/include`" = "" ]]; then
			rm -rf ${i}/include
		fi
		if [[ "`ls ${i}/lib/pkgconfig`" = "" ]]; then
			rm -rf ${i}/lib/pkgconfig
		fi
		if [[ "`ls ${i}/lib`" = "" ]]; then
			rm -rf ${i}/lib
		fi
		if [[ "`ls -A ${i}/resource/bin`" = "" && "`ls -A ${i}/resource/root`" = "" && "`ls -A ${i}/resource/other`" = "" ]]; then
			rm -rf ${i}/resource
		else
			if [[ "`ls -A ${i}/resource/bin`" = "" ]]; then
				rm -rf ${i}/resource/bin
			fi
			if [[ "`ls -A ${i}/resource/root`" = "" ]]; then
				rm -rf ${i}/resource/root
			fi
			if [[ "`ls -A ${i}/resource/other`" = "" ]]; then
				rm -rf ${i}/resource/other
			fi
		fi
	done
}

rel_compile(){
	echo -e "-->release compile"
	cd ${WORKSPACE}
	mv ${P_REL_ABS}/tools/sdk/Kconfig/Kconfig_qsdk ${P_REL_ABS}/Kconfig
	mv ${P_REL_ABS}/tools/sdk/Kconfig/Kconfig_uboot ${P_REL_ABS}/uboot/Kconfig
	mv ${P_REL_ABS}/tools/sdk/Kconfig/Kconfig_busybox ${P_REL_ABS}/busybox/Kconfig
	mv ${P_REL_ABS}/tools/sdk/Kconfig/Kconfig_toolchain ${P_REL_ABS}/toolchain/Kconfig

	touch ${P_REL_ABS}/libs/Kconfig
	for i in `ls ${P_REL_ABS}/libs`; do
		if [[ $i != "Kconfig" ]]; then
			I=`echo $i | tr '[a-z]' '[A-Z]'`
			echo "config $I" >> ${P_REL_ABS}/libs/Kconfig
			echo -e "\tbool \"${i}\"" >> ${P_REL_ABS}/libs/Kconfig
			echo -e "\tdefault y\n" >> ${P_REL_ABS}/libs/Kconfig
		fi
	done

	touch ${P_REL_ABS}/packages/Kconfig
	pack=`ls ${P_REL_ABS}/packages`
	if [[ -n $pack ]]; then
		for i in `ls ${P_REL_ABS}/packages`
		do
			I=`echo $i | tr '[a-z]' '[A-Z]'`
			if [[ $i != "Kconfig" ]]; then
				echo "config PACK_${I}" >> ${P_REL_ABS}/packages/Kconfig
				echo -e "\tbool \"${i} config options\"" >> ${P_REL_ABS}/packages/Kconfig
				echo "if PACK_${I}" >> ${P_REL_ABS}/packages/Kconfig
				echo "config PACK_${I}_CONFIG_PARA" >> ${P_REL_ABS}/packages/Kconfig
				echo -e "\tstring \"${i} config parameters\"" >> ${P_REL_ABS}/packages/Kconfig
				echo -e "endif\n" >> ${P_REL_ABS}/packages/Kconfig
			fi
		done
	fi

	mv ${P_REL_ABS}/tools/sdk/menuconfig/scripts ${P_REL_ABS}/tools
	mv ${P_REL_ABS}/tools/sdk/menuconfig/Makefile ${P_REL_ABS}/
    for i in `ls output/product -l`
    do
        p=`basename $i 2>/dev/null`
    done
	mv ${P_REL_ABS}/tools/sdk/menuconfig/qsdk_defconfig ${P_REL_ABS}/products/${p}/configs/
	rm -rf ${P_REL_ABS}/tools/sdk/menuconfig
	rm -rf ${P_REL_ABS}/tools/sdk/Kconfig
}

rel_sdk(){
    echo -e "-->release sdk "
    rel_init_rel_dir
    rel_ramdisk
    rel_system_files
    rel_host_toolchain
    rel_host_extra
    rel_repo_source tools
    rel_tools_extra
    rel_repo_source bootloader
    rel_repo_source kernel
    rel_bootloader_fix
    rel_busybox
    rel_busybox_extra
    rel_custom_packages
    rel_repo_source products
    rel_product_fix
    rel_private_modules
	rel_libs
	rel_libs_extra
	rel_compile
    echo -e "SDK relese over"
    echo -e "\t"${P_REL_ABS}
}

rel_sdk
