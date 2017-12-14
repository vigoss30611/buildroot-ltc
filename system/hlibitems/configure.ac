#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([hlibitems], [1.0.0], [bug@infotm.com])
AC_CONFIG_SRCDIR([items.c])
AC_CONFIG_HEADERS([config.h])
#AM_INIT_AUTOMAKE(1.0.0)
AM_INIT_AUTOMAKE([foreign -Werror])
#LT_INIT([static shared])
LT_INIT
AC_ENABLE_SHARED
AC_ENABLE_STATIC
# Checks for programs.
AC_PROG_AWK
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h string.h sys/ioctl.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.


AC_OUTPUT(Makefile)
