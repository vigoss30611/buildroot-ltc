#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([danap2p], [1.0.0], [bug@infotm.com])
AC_CONFIG_SRCDIR([base64.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.0.0 foreign no-define])
#LT_INIT([static shared])
LT_INIT
AC_ENABLE_SHARED
AC_ENABLE_STATIC

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC

PKG_CHECK_MODULES(LIBFR, fr == 1.0.0)
PKG_CHECK_MODULES(LIBVIDEOBOX, videobox >= 1.0.0)
PKG_CHECK_MODULES(LIBAUDIOBOX, audiobox >= 1.0.0)
PKG_CHECK_MODULES(LIBCODECS, libcodecs >= 1.0.0)

# Checks for libraries.
# FIXME: Replace `main' with a function in `-ldana_video':
AC_CHECK_LIB([dana_video], [main])
# FIXME: Replace `main' with a function in `-ldemuxer':
AC_CHECK_LIB([demuxer], [main])
# FIXME: Replace `main' with a function in `-lipc':
AC_CHECK_LIB([ipc], [main])
# FIXME: Replace `main' with a function in `-litem':
AC_CHECK_LIB([item], [main])
# FIXME: Replace `main' with a function in `-llog':
AC_CHECK_LIB([log], [main])
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])
# FIXME: Replace `main' with a function in `-losa':
AC_CHECK_LIB([osa], [main])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])
# FIXME: Replace `main' with a function in `-lresolv':
AC_CHECK_LIB([resolv], [main])
# FIXME: Replace `main' with a function in `-lrt':
AC_CHECK_LIB([rt], [main])
# FIXME: Replace `main' with a function in `-lsharemem':
AC_CHECK_LIB([sharemem], [main])

# Checks for header files.
AC_CHECK_HEADERS([danavideo.h danavideo_cmd.h info_log.h fcntl.h inttypes.h malloc.h stddef.h stdint.h stdlib.h string.h sys/statfs.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_CHECK_FUNCS([clock_gettime gettimeofday memset mkdir strcasecmp strncasecmp strrchr strstr])

#AC_CONFIG_FILES([Makefile])
AC_OUTPUT(Makefile)
