#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libramain], [1.0.0], [bug@infotm.com])
AC_CONFIG_SRCDIR([spv_app.h])
AC_CONFIG_HEADERS([config.h])
#AM_INIT_AUTOMAKE(1.0.0)
AM_INIT_AUTOMAKE([1.0.0 foreign])
#LT_INIT([static shared])
LT_INIT
AC_ENABLE_SHARED
AC_ENABLE_STATIC
# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP

PKG_CHECK_MODULES(LIBITEMS,items >= 1.0.0)
PKG_CHECK_MODULES(LIBMINIGUI,minigui >= 1.0.0)
PKG_CHECK_MODULES(LIBVIDEOBOX, videobox >= 1.0.0)
PKG_CHECK_MODULES(LIBAUDIOBOX, audiobox >= 1.0.0)
PKG_CHECK_MODULES(LIBAVCODEC, libavcodec >= 1.0.0)
PKG_CHECK_MODULES(LIBDEMUX, demux >= 1.0.0)
PKG_CHECK_MODULES(LIBCODEC, libcodecs >= 1.0.0)
PKG_CHECK_MODULES(LIBG1V6, hlibg1v6 >= 1.0.0)


# Checks for libraries.v
# FIXME: Replace `main' with a function in `-lasound':
AC_CHECK_LIB([hw_asound], [main])
# FIXME: Replace `main' with a function in `-lavcodec':
AC_CHECK_LIB([libavcodec], [main])
# FIXME: Replace `main' with a function in `-lavformat':
AC_CHECK_LIB([libavformat], [main])
# FIXME: Replace `main' with a function in `-lavutil':
AC_CHECK_LIB([libavutil], [main])
# FIXME: Replace `main' with a function in `-ldemuxer':
AC_CHECK_LIB([demuxer], [main])
# FIXME: Replace `main' with a function in `-ldl':
AC_CHECK_LIB([dl], [main])
# FIXME: Replace `main' with a function in `-lfilemng':
AC_CHECK_LIB([filemng], [main])
# FIXME: Replace `main' with a function in `-lfreetype':
AC_CHECK_LIB([freetype], [main])
# FIXME: Replace `main' with a function in `-lgps':
AC_CHECK_LIB([hw_gps], [main])
# FIXME: Replace `main' with a function in `-lipc':
AC_CHECK_LIB([ipc], [main])
# FIXME: Replace `main' with a function in `-litem':
AC_CHECK_LIB([item], [main])
# FIXME: Replace `main' with a function in `-llog':
AC_CHECK_LIB([log], [main])
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])
# FIXME: Replace `main' with a function in `-lminigui_ths':
AC_CHECK_LIB([minigui], [main])
# FIXME: Replace `main' with a function in `-lminigui_ths_ubuntu':
# FIXME: Replace `main' with a function in `-losa':
AC_CHECK_LIB([osa], [main])
# FIXME: Replace `main' with a function in `-lpipeline':
AC_CHECK_LIB([pipeline], [main])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])
# FIXME: Replace `main' with a function in `-lsharemem':
AC_CHECK_LIB([shm], [main])
# FIXME: Replace `main' with a function in `-lts':
AC_CHECK_LIB([ts], [main])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h netinet/in.h stdlib.h string.h sys/socket.h sys/statfs.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_PID_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_CHECK_FUNCS([bzero gettimeofday inet_ntoa memset mkdir regcomp socket strcasecmp strchr strerror strncasecmp strstr])

AC_OUTPUT(Makefile )
