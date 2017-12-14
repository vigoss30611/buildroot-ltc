DEFCONFIG_PREFIX=felix_x86_3.4
ARCH=x86
if [ ! -f arch/${ARCH}/configs/${DEFCONFIG_PREFIX}_defconfig ]; then
    echo "Prior to running this script, you have to apply '${DEFCONFIG_PREFIX}_defconfig.patch'"
    exit 1
fi
mkdir -p ${ANDROID_PRODUCT_OUT}/kernel
${ANDROID_BUILD_TOP}/external/qemu/distrib/build-kernel.sh --arch=${ARCH} --out=${ANDROID_PRODUCT_OUT}/kernel --config=${DEFCONFIG_PREFIX}

