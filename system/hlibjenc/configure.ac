#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([hlibjenc], [1.0.0], [bug@infotm.com])
#AC_CONFIG_SRCDIR([source/h264/h264encapi_ext.c])
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

PKG_CHECK_MODULES(LIBFR, fr >= 1.0.0)


# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stddef.h stdlib.h string.h sys/ioctl.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_OFF_T
AC_TYPE_SIZE_T

# Checks for library functions.

AC_OUTPUT([Makefile])
