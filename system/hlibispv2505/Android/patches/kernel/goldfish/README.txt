This is the location of patches and defconfigs for android-goldfish-3.4.67 kernel for Emulator
- felix_arm_3.4_defconfig.patch - ARM architecture defconfig
- felix_x86_3.4_defconfig.patch - x86 architecture defconfig

Example procedure for x86 (run inside android build environment)

0. set android environment (lunch aosp_x86-eng)

1. add local manifest for kernel

$ patch -p0 < patches/android/kitkat/goldfish/aosp_Kitkat_goldfish_kernel_3.4.67_local_manifest.patch

1. enter $ANDROID_BUILD_TOP/kernel/goldfish-3.4.67

2. apply patch for ion
$ patch -p1 < patches/kernel/goldfish/android_kernel_3.4.patch

3. create defconfig for x86 arch
$ patch -p0 < patches/kernel/goldfish/felix_x86_3.4_defconfig.patch

4. build kernel, the output will be eventually copied to ${ANDROID_PRODUCT_OUT}/kernel

$ cp patches/kernel/goldfish/build_felix_x86_3.4_kernel.sh .
$ ./build_felix_x86_3.4_kernel.sh

