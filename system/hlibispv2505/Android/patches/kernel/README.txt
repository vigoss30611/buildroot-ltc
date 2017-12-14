Here you can find patches for different kernel version used in Android:

1. AOSP Kitkat 4.4 for Emulator (fetched by patches/android/kitkat/goldfish/aosp_Kitkat_goldfish_kernel_3.4.67_local_manifest.patch)
- goldfish/android_kernel_3.4.patch (from kernel/goldfish : 'patch -p1')
- see goldfish/README.txt for more details on building

2. AOSP Kitkat 4.4 for x86 PC-FPGA (fetched by patches/android/kitkat/x86/aosp_Kitkat_kernel_3.10.0_local_manifest.patch)
- x86/ion-staging-x86.patch (from kernel/common : 'patch -p3')
- x86/ion-staging-device-create.patch (from kernel/common : 'patch -p0')

3. Android-x86 Kitkat 4.4 for PC+FPGA
- x86/android-kernel_3.10.patch 

4. Android AOSP Lollipop >= 5.0 (fetched by patches/android/lollipop/aosp_Lollipop_kernel_3.10.0_local_manifest.patch)
- x86/ion-staging-x86.patch (from kernel/common : 'patch -p3')
- x86/ion-staging-device-create.patch (from kernel/common : 'patch -p0')

