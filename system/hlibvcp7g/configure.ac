#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([hlibvcp7g], [1.0.0], [bug@infotm.com])
#AC_CONFIG_SRCDIR([vcp7g.c])
AC_CONFIG_SRCDIR([vcp7g.c], [vcp7g_arm.c], [vcp7g_dsp.c], [vcp_example.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.0.0 foreign no-define])
#AC_CONFIG_MACRO_DIR([m4])


LT_INIT
AC_ENABLE_SHARED
AC_ENABLE_STATIC
# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread])
AC_CHECK_LIB([codecs])
AC_CHECK_LIB([dsp])

# Checks for header files.
AC_CHECK_HEADERS([math.h stdio.h stdlib.h string.h time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_PID_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([fopen fread fwrite fclose perror memcpy malloc])

# Add configure options
AC_ARG_ENABLE(dspsupport,
			  AS_HELP_STRING([--enable-dspsupport],
							 [enable dsp support for aec, default is yes]),
							 [have_dspsupport="$enableval"],
							 [have_dspsupport="yes"]
							 )
AC_ARG_ENABLE(armsupport,
			  AS_HELP_STRING([--enable-armsupport],
							 [enable arm support for aec, default is no]),
							 [have_armsupport="$enableval"],
							 [have_armsupport="no"]
							 )

AM_CONDITIONAL(HAVE_DSPSUPPORT, test x$have_dspsupport = xyes)
AM_CONDITIONAL(HAVE_ARMSUPPORT, test x$have_armsupport = xyes)
if test x$have_dspsupport = xyes ; then
PKG_CHECK_MODULES(LIBCODECS, libcodecs >= 1.0.0)
PKG_CHECK_MODULES(LIBDSP, libdsp >= 1.0.0)
fi
if test x$have_armsupport = xyes ; then
PKG_CHECK_MODULES(LIBCODECS, libcodecs >= 1.0.0)
fi

AC_OUTPUT(Makefile)
