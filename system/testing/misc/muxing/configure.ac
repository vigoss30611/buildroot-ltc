#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([muxing], [1.0.0], [bug@infotm.com])
AC_CONFIG_SRCDIR([demuxing.c])
AM_INIT_AUTOMAKE([1.0.0 foreign no-define])
#AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])


PKG_CHECK_MODULES(LIBAVCODEC, libavcodec >= 1.0.0)
PKG_CHECK_MODULES(LIBAVFORMAT, libavformat >= 1.0.0)
PKG_CHECK_MODULES(LIBAVUTIL, libavutil >= 1.0.0)


LT_INIT
AC_ENABLE_SHARED
AC_ENABLE_STATIC
# Checks for programs.
AC_PROG_CC


# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT64_T
AC_TYPE_UINT8_T

# Checks for library functions.

#AC_CONFIG_FILES([Makefile])
AC_OUTPUT(Makefile)
