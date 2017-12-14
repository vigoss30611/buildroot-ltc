#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([hlibguvc], [1.0.0], [ps_bug@infotm.com])
AC_CONFIG_SRCDIR([guvc.c])
AC_CONFIG_HEADERS([config.h])
AC_OUTPUT([Makefile])
AM_INIT_AUTOMAKE

LT_INIT
#AC_ENABLE_SHARED
#AC_ENABLE_STATIC

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES(LIBVIDEOBOX, videobox >= 1.0.0)
PKG_CHECK_MODULES(LIBFR, fr >= 1.0.0)

# Checks for header files.
AC_CHECK_HEADERS([stdint.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([munmap])

AC_OUTPUT
