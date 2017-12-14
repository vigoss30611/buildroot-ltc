#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([launcher], [1.0.0], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([sysserver/spv_language_res.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.0.0 foreign subdir-objects])
# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LN_S

PKG_CHECK_MODULES(LIBITEMS,items >= 1.0.0)
PKG_CHECK_MODULES(LIBVIDEOBOX, videobox >= 1.0.0)
PKG_CHECK_MODULES(LIBAUDIOBOX, audiobox >= 1.0.0)
PKG_CHECK_MODULES(LIBAVCODEC, libavcodec >= 1.0.0)
PKG_CHECK_MODULES(LIBDEMUX, demux >= 1.0.0)
PKG_CHECK_MODULES(LIBCODEC, libcodecs >= 1.0.0)
PKG_CHECK_MODULES(LIBG1V6, hlibg1v6 >= 1.0.0)
PKG_CHECK_MODULES(LIBSYSTEM,qlibsys >= 1.0.0)
PKG_CHECK_MODULES(LIBUPGRADE,qlibupgrade >= 1.0.0)
PKG_CHECK_MODULES(LIBFREETYPE,freetype2 >= 1.0.0)
PKG_CHECK_MODULES(LIBMEDIAUTILS,qlibmediautils >= 1.0.0)
PKG_CHECK_MODULES(LIBQVPLAY,qvplay >= 1.0.0)

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h netinet/in.h stdint.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/statfs.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_CHECK_FUNCS([bzero gettimeofday inet_ntoa memset mkdir regcomp select socket strcasecmp strchr strerror strncasecmp strstr])

# Add configure options.
#enable gui
AC_ARG_ENABLE(gui,
				AS_HELP_STRING([--enable-gui],
							 [enable gui , default is no]) ,
			 	[have_gui="$enableval"] ,
				[have_gui="no"]
			 )
AM_CONDITIONAL(HAVE_GUI, test x$have_gui = xyes)

AS_IF([test "x$have_gui" = xyes],
      [PKG_CHECK_MODULES(LIBMINIGUI, minigui >= 1.0.0)])

AC_ARG_ENABLE(wifi,
				AS_HELP_STRING([--enable-wifi],
							 [enable wifi, default is no]) ,
			 	[have_wifi="$enableval"] ,
				[have_wifi="no"]
			 )
#AM_CONDITIONAL(HAVE_WIFI, test x$have_wifi = xyes)
AS_IF([test "x$have_wifi" = xyes],
      [PKG_CHECK_MODULES(LIBWIFI, wifi >= 1.0.0)])


#enable p2p dana
AC_ARG_ENABLE(p2p_dana,
				AS_HELP_STRING([--enable-p2p_dana],
							 [enable p2p dana, default is no]) ,
			 	[have_p2p_dana="$enableval"] ,
				[have_p2p_dana="no"]
			 )
AM_CONDITIONAL(HAVE_P2P_DANA, test x$have_p2p_dana = xyes)


AM_CONDITIONAL(HAVE_PLAYBACK, test x$have_playback = xyes)

AC_ARG_ENABLE(calibration,
				AS_HELP_STRING([--enable-calibration],
							 [enable calibration , default is no]) ,
			 	[have_calibration="$enableval"] ,
				[have_calibration="no"]
			 )
AS_IF([test "x$have_calibration" = xyes],
      [PKG_CHECK_MODULES(LIBGUVC, hlibguvc >= 1.0.0)])

AC_ARG_ENABLE(uvc,
				AS_HELP_STRING([--enable-uvc],
							 [enable uvc, default is no]) ,
				[have_uvc="$enableval"] ,
				[have_uvc="no"]
			 )
AS_IF([test "x$have_uvc" = xyes],
      [PKG_CHECK_MODULES(LIBGUVC, hlibguvc >= 1.0.0)])
AM_CONDITIONAL(HAVE_UVC, test x$have_uvc = xyes)

#AC_CONFIG_SUBDIRS([p2p_dana])
AC_OUTPUT(Makefile )
