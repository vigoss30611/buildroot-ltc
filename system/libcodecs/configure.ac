#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libcodecs], [1.0.0], [ps_bug@infotm.com])
AC_CONFIG_SRCDIR([codecs.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.0.0 foreign no-define subdir-objects])

LT_INIT
AC_ENABLE_SHARED
AC_ENABLE_STATIC
# Checks for programs.
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])

PKG_CHECK_MODULES(LIBAVCODEC, libavcodec >= 1.0.0)
PKG_CHECK_MODULES(LIBSWREAMPLE, libswresample >= 1.0.0)
PKG_CHECK_MODULES(LIBAVFORMAT, libavformat >= 1.0.0)
PKG_CHECK_MODULES(LIBAVUTIL, libavutil >= 1.0.0)

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT32_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

# Add configure options
AC_ARG_ENABLE(dspsupport,
			  AS_HELP_STRING([--enable-dspsupport],
							 [enable dsp encode, support, default is no]),
							 [have_dspsupport="$enableval"],
							 [have_dspsupport="no"]
							 )
AM_CONDITIONAL(HAVE_DSPSUPPORT, test x$have_dspsupport = xyes)
if test x$have_dspsupport = xyes ; then
PKG_CHECK_MODULES(LIBDSP, libdsp >= 1.0.0)
fi


#AC_CONFIG_FILES([Makefile])
AC_OUTPUT([Makefile])
