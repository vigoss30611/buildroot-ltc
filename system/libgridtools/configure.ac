#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libgridtools], [1.0.0], [bug@infotm.com])
AC_CONFIG_SRCDIR([libgridtools.mk])
AM_INIT_AUTOMAKE([1.0.0 foreign no-define subdir-objects])
AC_CONFIG_HEADERS([config.h])

PKG_CHECK_MODULES(LIBMNEON, libmneon >= 1.0.0)

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CXX
# Checks for libraries.


LT_INIT
AC_ENABLE_SHARED
AC_ENABLE_STATIC
AC_PROG_LIBTOOL(libtool)

AC_OUTPUT(Makefile)
