#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([hlibg2v1], [1.0.0], [bug@infotm.com])
AC_CONFIG_SRCDIR([software/test/common/md5_sink.h])
AC_CONFIG_HEADERS([config.h])
#AM_INIT_AUTOMAKE(1.0.0)
AM_INIT_AUTOMAKE([foreign -Werror subdir-objects])
#LT_INIT([static shared])
LT_INIT
AC_ENABLE_SHARED
AC_ENABLE_STATIC
# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_MAKE_SET
#PKG_CHECK_MODULES(LIBHWHEADER,hwheader >= 1.0.0)
PKG_CHECK_MODULES(LIBFR,fr >= 1.0.0)

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lInfotmMediaBufferAlloc':
AC_CHECK_LIB([InfotmMediaBufferAlloc], [main])
# FIXME: Replace `main' with a function in `-lInfotmMediaION':
AC_CHECK_LIB([InfotmMediaION], [main])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h memory.h stddef.h stdlib.h string.h sys/ioctl.h sys/time.h sys/timeb.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT64_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FSEEKO
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_STRNLEN
AC_CHECK_FUNCS([getpagesize gettimeofday memset munmap strstr strtol])

AC_OUTPUT([Makefile])
