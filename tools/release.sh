#!/bin/bash
#
# warits@2016/07/01
#
# release procedure:
# 1. use "./tools/setproduct.sh" to select the product to be released
# 2. make sure the whole QSDK is compiled
# 3. use "./tools/release.sh" to generate a release package
#
# note:
# 1. use "./tools/release.sh -r" to debug RELEASE_RULES
# 2. use "./tools/release.sh -n" to release full source code
#    without any package reserved
#
# if you want to reserve a package:
# 1. you need to write a RELEASE_RULES file in the package to be reserved
# 2. you need to add the package name to the RESERVE_LIST file in the
#    product folder
#
# see package hlibispv2500/RELEASE_RULES for demostration

gen_rel_mk() {
	UNAME=`echo $1 | tr '[a-z]' '[A-Z]'`
	LNAME=`echo $1 | tr '[A-Z]' '[a-z]'`
	S=${P_ABS}/system/${LNAME}

	echo -en "# generic release mk\n\n"
	cat system/$1/${LNAME}.mk | grep "VERSION" | head -1
	echo -en "${UNAME}_SOURCE =\n"
	echo -en "${UNAME}_MAINTAINED = YES\n"
	echo -en "${UNAME}_DEPENDENCIES = \n"
	for pkg in `cat output/build/build-time.log | cut -f 4 -d ':' | uniq`;
		do grep -q $pkg system/$1/${LNAME}.mk && echo -en "${UNAME}_DEPENDENCIES += $pkg\n";
	done
	echo
	echo -en "${UNAME}_LOCAL_SRC = \$(shell pwd)/system/${LNAME}\n\n"
	echo -e "${UNAME}_MAKE_OPTS = \\"
	echo -e "\tCC=\"\$(TARGET_CC)\" \\"
	echo -e "\tARCH=\$(KERNEL_ARCH) \\"
	echo -e "\tCROSS_COMPILE=\"\$(TARGET_CROSS)\" \\"
	echo -en "\tS=\$(${UNAME}_LOCAL_SRC)\n\n"
	echo -en "# generic install\n"
	echo -en "define ${UNAME}_RELEASE_INSTALL\n"

	if [ -n "`ls ${S}/header 2>/dev/null`" ]; then
		echo -en "\n\t# install headers\n"
		echo -en "\tcp -pdrf \$(${UNAME}_LOCAL_SRC)/header/* \$(STAGING_DIR)/\n"
	fi

	if [ -n "`ls ${S}/static 2>/dev/null`" ]; then
		echo -en "\n\t# install static libs\n"
		echo -en "\tcp -pdrf \$(${UNAME}_LOCAL_SRC)/static/* \$(STAGING_DIR)/\n"
	fi

	if [ -n "`ls ${S}/shared 2>/dev/null`" ]; then
		echo -en "\n\t# install shared libs\n"
		echo -en "\tcp -pdrf \$(${UNAME}_LOCAL_SRC)/shared/* \$(STAGING_DIR)/\n"
		echo -en "\tcp -pdrf \$(${UNAME}_LOCAL_SRC)/shared/* \$(TARGET_DIR)/\n"
	fi

	if [ -n "`ls ${S}/binary 2>/dev/null`" ]; then
		echo -en "\n\t# install binaries\n"
		echo -en "\tcp -pdrf \$(${UNAME}_LOCAL_SRC)/binary/* \$(TARGET_DIR)/\n"
	fi

	if [ -n "`ls ${S}/resource 2>/dev/null`" ]; then
		echo -en "\n\t# install resources\n"
		echo -en "\tcp -pdrf \$(${UNAME}_LOCAL_SRC)/resource/* \$(TARGET_DIR)/\n"
	fi

	if [ -n "`ls ${S}/*.pc 2>/dev/null`" ]; then
		echo -en "\n\t# install pkgconfig\n"
		echo -en "\tcp -pdrf \$(${UNAME}_LOCAL_SRC)/*.pc \$(STAGING_DIR)/usr/lib/pkgconfig/\n"
	fi

	if [ -n "`ls ${S}/module 2>/dev/null`" ]; then
		echo -en "\n\t# install ko\n"
		echo -en "\tmkdir -p \$(TARGET_DIR)/lib/modules/${K_VER}/extra/\n"
		echo -en "\tcp -pdrf \$(${UNAME}_LOCAL_SRC)/module/*.ko \$(TARGET_DIR)/lib/modules/${K_VER}/extra/\n"
		echo -en "\t\$(TARGET_MAKE_ENV) \$(MAKE) \$(${UNAME}_MAKE_OPTS) -C \$(${UNAME}_LOCAL_SRC) install\n"
		echo -en "\t\$(TARGET_MAKE_ENV) \$(MAKE) \$(${UNAME}_MAKE_OPTS) -C output/build/linux-local INSTALL_MOD_PATH=../../system modules_install\n"
	fi
	echo -en "endef\n\n"
	echo -en "${UNAME}_POST_INSTALL_TARGET_HOOKS += ${UNAME}_RELEASE_INSTALL\n\n"
	echo -en "\$(eval \$(generic-package))\n"
}

gen_rel_pkg() {

	P_SRC=system/$1
	P_OBJ=output/build/${1}*
	P_DST=${P_ABS}/system/$1
	P_TGT=output/system

	if [ -z "$1" ]; then
		echo "$0 PKG_NAME"
		exit 1
	fi

	if [ ! -d ${P_SRC} ]; then
		echo "${P_SRC} does not exist"
		exit 1
	fi

	if [ ! -f ${P_SRC}/RELEASE_RULES ]; then
		echo "RELEASE_RULES in package $1 not found"
		exit 1
	fi

	rm ${P_DST}/* -rf
	cp ${P_SRC}/Config.in ${P_DST}/

	for i in `find ${P_SRC} -name "*.pc"`; do 
		cp output/staging/usr/lib/pkgconfig/`basename $i` ${P_DST}/ 2>/dev/null
	done

	if [ -z "`ls ${P_OBJ} 2>/dev/null`" ]; then
		return 2
	fi

	cat ${P_SRC}/RELEASE_RULES | awk '{ if(match($1, ":")) {
		DIR=$1; gsub(":", "", DIR)
	} else if ($1) {
		system("mkdir -p '${P_DST}'/" DIR "/" $2)
		CMD = (DIR == "header")? "cp -pdrf '${P_SRC}'/":
		(DIR == "resource")? "cp -pdrf '${P_TGT}'/": "cp -pdrf '${P_OBJ}'/"
		system(CMD $1 " '${P_DST}'/" DIR "/" $2 " 2>/dev/null")
	}}'

	gen_rel_mk $1 > ${P_DST}/$1.mk
	echo -e "\nall:\ninstall:\n" > ${P_DST}/Makefile
}

gen_rel_dir() {
	rm -rf ${P_ABS}*
	mkdir ${P_ABS}

	for i in ${DIRS}; do
		echo -e "exporting $i"
		mkdir -p ${P_ABS}/$i
		cd $i
		git archive HEAD | tar x -C ${P_ABS}/$i
		git log -1 > ${P_ABS}/$i/GIT_VERSION
		cd - > /dev/null 2>&1
	done
	cp ${FILES} ${P_ABS} -pdrvf
}

gen_rel_tar() {
	DOTS=$((`du ${P_REL} -s | cut -f1`/100))
	echo -en "           ]\r["
	tar czf ${P_ABS}.tgz ${P_REL} --checkpoint=${DOTS} --checkpoint-action=dot
#	rm -rf ${P_REL}
	echo -en "]"
}

# set -x
DIRS="buildroot system bootloader kernel products tools"
FILES=Makefile

P_REL=qsdk_`date +"%y%m%d"`
P_ABS=`pwd`/${P_REL}
K_VER=`ls output/system/lib/modules/ | head -1`

echo -e "\nprepare source code:"
if test "$1" != "-r"; then
	gen_rel_dir
fi

if test "$1" != "-n"; then
echo -e "\nreserve non-disclosure files:"
if [ -f output/product/RESERVE_LIST ]; then
	for pkg in `cat output/product/RESERVE_LIST`; do
		gen_rel_pkg $pkg
		if [ $? -eq 2 ]; then echo "$pkg not available (WARNING)"
		else echo "$pkg reserved"; fi
	done
	echo "done"
else
	echo "nothing"
fi
fi

echo -e "\nremove unrelevant products:"
for i in `ls output/product -l`; do PRODUCT=`basename $i 2>/dev/null`; done
cd ${P_ABS}/products
for i in `ls`; do
	if [ "$i" != "${PRODUCT}" ] && [ "$i" != "GIT_VERSION" ]; then
		rm $i -rf; echo "$i removed"
	fi
done
cd - > /dev/null

echo -e "\nfix kernel version:\ndone"
echo ${K_VER} > ${P_ABS}/kernel/.scmversion

echo -e "\npackage ready: ${P_ABS} !\n"

#echo -e "\ncreate package:"
#if test "$1" != "-r"; then
#	gen_rel_tar
#fi
#echo "done"

