# This script can be used to cross compilation of DDKSource. In order to use
# the script follow the steps below (example for ARM architecture):
#
# 1. remove cmake cache (this is very important otherwise
#    CMAKE_TOOLCHAIN_FILE argument will be ignored by cmake in step #2.
# 2. Use this script to configure cmake cache:
#    cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/Toolchain-arm.cmake \
#       -DLINUX_KERNEL_BUILD_DIR=/path/to/linux/kernel
# 3. call make:
#    ARCH=arm CROSS_COMPILE=arm-linux-androideabi- make
#
# 
# Which compiler to choose:
#
# 1. Prebuilt compiler in android tree:
#      <Android root path>/prebuilts/gcc/
#
#    This compiler is not standalone compiler and has no STL support by default.
#    It's usually used for Android build together with Android environment
#    (i.e. using "mmm" command for compilation which will set variables
#    and paths for this compiler). It's possible to use this compiler as
#    standalone by specyfying "--sysroot" option.
#    Example of sysroot path:
#      <Android root path>/prebuilts/ndk/current/platforms/android-18/arch-arm
#    It doesn't support STL by default and can't be used to build Felix test
#    applications.
#    There is even problem to build Felix kernel module using this compiler due
#    to CMake mechanisms. CMake at the start tries to compile simple test
#    which is dynamically linked and needs to have standalone compiler. In such
#    case "--sysroot" parameter needs to be passed to initial test. It can be
#    done using following setting:
#      SET(CMAKE_C_FLAGS "--sysroot <SYSROOT_PATH>" CACHE STRING "" FORCE)
#    but futher compilation fails since this setting is also passed to host
#    compiler which builds register tools and this option is unknown for it.
#    The easiest way is to use standalone compilers described in point 3 and 4.
#
# 2. Prebuilt compiler in NDK tree:
#      <NDK root path>/toolchains/
#    This compiler behaves exactly the same as the one described in point 1.
#
# 3. Prebuilt compiler from CodeSourcery.
#    This compiler is standalone by default and has STL support.
#
# 4. Building own Android toolchain with STL support (standalone).
#      <Android root path>/ndk/build/tools/make-standalone-toolchain.sh
#    make-standalone-toolchain.sh --platform=android-18 --install-dir=/tmp/my-android-toolchain
#

# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
SET(CMAKE_C_COMPILER <CROSS_COMPILER_PATH>/arm-linux-androideabi-gcc)
SET(CMAKE_CXX_COMPILER <CROSS_COMPILER_PATH>/arm-linux-androideabi-g++)

# where is the target environment 
#SET(CMAKE_FIND_ROOT_PATH ${ANDROID_ROOT_PATH}/bionic/libc/kernel/common)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
