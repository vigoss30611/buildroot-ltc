#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([eventhub], [1.0.0], [bug@infotm.com])
AC_CONFIG_SRCDIR([eventhub.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.0.0 foreign no-define])
#AC_CONFIG_MACRO_DIR([m4])


LT_INIT
#AC_ENABLE_SHARED
#AC_ENABLE_STATIC
# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC

AM_CONDITIONAL(EVENT_TESTING, false)
AC_ARG_ENABLE(testing,
                AS_HELP_STRING([--enable-testing],
                            [enable testing]) ,
                [testing="yes"],
                [testing="no"]
			)

if test x$testing = xyes;then
    AM_CONDITIONAL(EVENT_TESTING, test x$testing = xyes)
fi


#PKG_CHECK_MODULES(LIBEVENT_DEV,event_dev == 1.0.0)
# Checks for libraries.
# FIXME: Replace `main' with a function in `-levent_dev':
#AC_CHECK_LIB([event_dev], [main])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h malloc.h stddef.h stdlib.h string.h sys/mount.h sys/statfs.h sys/vfs.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
# AC_CHECK_HEADER_STDBOOL
AC_TYPE_PID_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([clock_gettime gettimeofday memset strerror strrchr strstr])

AC_OUTPUT(Makefile)
