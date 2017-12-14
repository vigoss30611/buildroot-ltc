#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([testing], [1.0.0], [bug@infotm.com])
AC_CONFIG_SRCDIR([testing.mk])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.0.0 foreign no-define])


LT_INIT
AC_ENABLE_SHARED
AC_ENABLE_STATIC
# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC

PKG_CHECK_MODULES(LIBITEMS,items == 1.0.0)
PKG_CHECK_MODULES(LIBWIFI,wifi == 1.0.0)
PKG_CHECK_MODULES(EVENT,event == 1.0.0)
PKG_CHECK_MODULES(LIBSYSTEM,qlibsys == 1.0.0)
PKG_CHECK_MODULES(LIBUPGRADE,qlibupgrade == 1.0.0)
PKG_CHECK_MODULES(LIBAVCODEC, libavcodec >= 1.0.0)
PKG_CHECK_MODULES(LIBAVFORMAT, libavformat >= 1.0.0)
PKG_CHECK_MODULES(LIBAVUTIL, libavutil >= 1.0.0)
PKG_CHECK_MODULES(LIBVIDEOBOX, videobox >= 1.0.0)
#PKG_CHECK_MODULES(LIBG1V6, hlibg1v6 >= 1.0.0)
PKG_CHECK_MODULES(LIBFR, fr >= 1.0.0)
PKG_CHECK_MODULES(LIBAUDIOBOX, audiobox >= 1.0.0)
PKG_CHECK_MODULES(LIBALSA, alsa == 1.0.26)
PKG_CHECK_MODULES(LIBCODECS, libcodecs >= 1.0.0)
#PKG_CHECK_MODULES(LIBJENC, hlibjenc >= 1.0.0)
PKG_CHECK_MODULES(LIBDEMUX, demux >= 1.0.0)
PKG_CHECK_MODULES(LIBVIDEODEBUG, hlibvideodebug >= 1.0.0)
# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
					vbctrl/Makefile testbench_g1v6/Makefile auto_delayline_test/Makefile fodet/Makefile mv/Makefile
					misc/muxing/Makefile abctrl/Makefile decode_display/Makefile testbench_jenc/Makefile display2/Makefile cma/Makefile watchdog/Makefile mmc/Makefile
					system/Makefile ccf/Makefile gpio/Makefile video_debug_info/Makefile otatest/Makefile])

AC_OUTPUT
