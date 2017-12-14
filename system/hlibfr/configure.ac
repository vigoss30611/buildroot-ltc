#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([hlibfr], [1.0.0], [ps_bug@infotm.com])
AC_CONFIG_SRCDIR([fr2.c])
AC_CONFIG_HEADERS([config.h])
AC_OUTPUT([Makefile])
AM_INIT_AUTOMAKE

LT_INIT
#AC_ENABLE_SHARED
#AC_ENABLE_STATIC

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

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

AM_CONDITIONAL(CC_BUFFER, false)
AC_ARG_ENABLE(cc_buffer,
                AS_HELP_STRING([--enable-cc_buffer],
                            [enable cc_buffer]) ,
                [cc_buffer="yes"],
                [cc_buffer="no"]
			)

if test x$cc_buffer = xyes;then
    AM_CONDITIONAL(CC_BUFFER, test x$cc_buffer = xyes)
fi

AM_CONDITIONAL(CC_WRITETHROUGH, false)
AC_ARG_ENABLE(cc_writethrough,
                AS_HELP_STRING([--enable-cc_writethrough],
                            [enable cc_writethrough]) ,
                [cc_writethrough="yes"],
                [cc_writethrough="no"]
			)

if test x$cc_writethrough = xyes;then
    AM_CONDITIONAL(CC_WRITETHROUGH, test x$cc_writethrough = xyes)
fi

AM_CONDITIONAL(CC_WRITEBACK, false)
AC_ARG_ENABLE(cc_writeback,
                AS_HELP_STRING([--enable-cc_writeback],
                            [enable cc_writeback]) ,
                [cc_writeback="yes"],
                [cc_writeback="no"]
			)
if test x$cc_writeback = xyes;then
    AM_CONDITIONAL(CC_WRITEBACK, test x$cc_writeback = xyes)
fi

AM_CONDITIONAL(NEW_MMAP, false)
AC_ARG_ENABLE(new_mmap,
                AS_HELP_STRING([--enable-new_mmap],
                            [enable new_mmap]) ,
                [new_mmap="yes"],
                [new_mmap="no"]
                        )
if test x$new_mmap = xyes;then
    AM_CONDITIONAL(NEW_MMAP, test x$new_mmap = xyes)
fi

AC_OUTPUT
