#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([qlibvplay], [1.0.0], [bug@infotm.com])
AC_CONFIG_SRCDIR([test/vrec.c])
AM_INIT_AUTOMAKE([1.0.0 foreign no-define subdir-objects])
#AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])


PKG_CHECK_MODULES(LIBAVCODEC, libavcodec >= 1.0.0)
PKG_CHECK_MODULES(LIBAVFORMAT, libavformat >= 1.0.0)
PKG_CHECK_MODULES(LIBAVUTIL, libavutil >= 1.0.0)
PKG_CHECK_MODULES(LIBSWREAMPLE, libswresample >= 1.0.0)
PKG_CHECK_MODULES(LIBVB, videobox >= 1.0.0)
PKG_CHECK_MODULES(LIBAUDIOBOX, audiobox >= 1.0.0)
PKG_CHECK_MODULES(LIBCODECS, libcodecs >= 1.0.0)
PKG_CHECK_MODULES(LIBFR, fr >= 1.0.0)
PKG_CHECK_MODULES(LIBDEMUX, demux >= 1.0.0)
PKG_CHECK_MODULES(LIBH264BITSTREAM, libh264bitstream >= 0.1.9)


LT_INIT
AC_ENABLE_SHARED
AC_ENABLE_STATIC
AC_PROG_LIBTOOL(libtool)
# Checks for programs.
AC_PROG_CC
AC_PROG_CXX


# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT64_T
AC_TYPE_UINT8_T

# Add configure options.
#enable feature
AC_ARG_ENABLE(gdbdebug,
				AS_HELP_STRING([--enable-gdbdebug],
							 [enable gdb debug , just add -g -O0 , default is no]) ,
			 	[have_gdbdebug="$enableval"] ,
				[have_gdbdebug="no"]
			 )
AM_CONDITIONAL(HAVE_GDBDEBUG, test x$have_gdbdebug = xyes)
AC_ARG_ENABLE(testutils,
				AS_HELP_STRING([--enable-testutils],
							 [enable test utils , include vplay vrec and other binary tools]) ,
			 	[have_testutils="$enableval"] ,
				[have_testutils="no"]
			 )
AM_CONDITIONAL(HAVE_TESTUTILS, test x$have_testutils = xyes)
AC_ARG_ENABLE(fragmented,
				AS_HELP_STRING([--enable-fragmented],
							 [enable fragmented mp4 ,mov support , default is no]) ,
			 	[have_fragmented="$enableval"] ,
				[have_fragmented="no"]
			 )
AM_CONDITIONAL(HAVE_FRAGMENTED, test x$have_fragmented = xyes)

AC_ARG_VAR(logger_level,"Log level 5:ERR > 4:WARN > 3:INFO > 2:DBG > 1:TRC")

AC_ARG_VAR(player_cache_second,"player nornal mode cache fifo second")
AC_ARG_VAR(recorder_cache_second,"recorder nornal mode cache fifo second")


AM_CONDITIONAL(HAVE_RECORDER_CACHE_TIME, test x$recorder_cache_second != x )
AM_CONDITIONAL(HAVE_PLAYER_CACHE_TIME, test x$player_cache_second != x )


AC_OUTPUT(Makefile)
