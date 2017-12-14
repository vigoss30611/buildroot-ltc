#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([testbenc_hevc_dec], [1.0.0], [bug@infotm.com])
AC_CONFIG_SRCDIR([hevc/dectestbench.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.0.0 foreign no-define subdir-objects])
#AC_CONFIG_MACRO_DIR([m4])


LT_INIT
AC_ENABLE_SHARED
AC_ENABLE_STATIC
# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_MAKE_SET

PKG_CHECK_MODULES(LIBFR,fr >= 1.0.0)
PKG_CHECK_MODULES(LIBHW_VDEC_G2,hw_vdec_g2 >= 1.0.0)



# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_OFF_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FSEEKO
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gettimeofday memset])

#AC_CONFIG_FILES([Makefile])
AC_OUTPUT(Makefile)
