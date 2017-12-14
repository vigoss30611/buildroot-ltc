#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ittd], [1.0.0], [bug@infotm.com])
AC_CONFIG_SRCDIR([IQTest.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.0.0 foreign no-define])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB(rtsp, rtsp_server_init)

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h stdint.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([memset select socket])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
