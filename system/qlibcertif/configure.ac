#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([qlibcertif], [1.0.0], [bug@infotm.com])
AC_CONFIG_SRCDIR([certificate.c])
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

# Checks for libraries.
# FIXME: Replace `main' with a function in `-litem':
AC_CHECK_LIB([item], [main])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h malloc.h stddef.h stdlib.h string.h sys/mount.h sys/statfs.h sys/vfs.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_PID_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([clock_gettime gettimeofday memset strerror strrchr strstr])

# Add configure options
AC_ARG_ENABLE(certiftest,
			  AS_HELP_STRING([--enable-certiftest],
							 [compile certif_test, default is no]),
							 [have_certiftest="$enableval"],
							 [have_certiftest="no"]
							 )
AM_CONDITIONAL(HAVE_CERTIFTEST, test x$have_certiftest = xyes)
if test x$have_certiftest = xyes ; then
PKG_CHECK_MODULES(LIBSYSTEM,qlibsys >= 1.0.0)
fi

AC_OUTPUT(Makefile)
