#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([audiobox2], [1.0.0], [bug@infotm.com])
AC_CONFIG_SRCDIR([audiobox.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.0.0 foreign no-define subdir-objects])
#AC_CONFIG_MACRO_DIR([m4])


LT_INIT
AC_ENABLE_SHARED
AC_ENABLE_STATIC
# Checks for programs.
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread])
AC_CHECK_LIB([codecs])
AC_CHECK_LIB([dsp])
AC_CHECK_LIB([vcp7g])

# Checks for libraries.
PKG_CHECK_MODULES(LIBFR, fr == 1.0.0)
PKG_CHECK_MODULES(LIBEVENT, event == 1.0.0)
PKG_CHECK_MODULES(LIBALSA, alsa == 1.0.26)
PKG_CHECK_MODULES(LIBVCP7G, libvcp7g == 1.0.0)
PKG_CHECK_MODULES(LIBCODECS, libcodecs >= 1.0.0)

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

#AC_CONFIG_FILES([Makefile])
AC_OUTPUT(Makefile)
