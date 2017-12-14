#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libqmediautils], [1.0.0], [bug@infotm.com])
AC_CONFIG_SRCDIR([test/exiftest.c])
AM_INIT_AUTOMAKE([1.0.0 foreign no-define subdir-objects])
# Checks for programs.
AC_PROG_CC

# Checks for libraries.


LT_INIT
AC_ENABLE_SHARED
AC_ENABLE_STATIC
AC_PROG_LIBTOOL(libtool)

AM_CONDITIONAL(QLIBMEDIAUTILS_BUILD_LIB, false)
AC_ARG_ENABLE(calibration,
                AS_HELP_STRING([--enable-calibration],
                            [jepg calibration by exif maker]) ,
                [calibration="$enableval"] ,
                [calibration="no"]
            )
AM_CONDITIONAL(QLIBMEDIAUTILS_CALIBRATION, test x$calibration = xyes)
AC_MSG_RESULT("--------------calibration ->" $calibration )
if test x$calibration = xyes;then
    PKG_CHECK_MODULES(LIBEXIF, libexif >= 0.6.21)
    AM_CONDITIONAL(QLIBMEDIAUTILS_BUILD_LIB, test x$calibration = xyes)
fi

AC_ARG_ENABLE(swscale,
                AS_HELP_STRING([--enable-swscale],
                            [enable software scale]) ,
                [swscale="$enableval"] ,
                [swscale="no"]
            )
AM_CONDITIONAL(QLIBMEDIAUTILS_SWSCALE, test x$swscale = xyes)
AC_MSG_RESULT("--------------swscale ->" $swscale )
if test  x$swscale = xyes ;then
    AM_CONDITIONAL(QLIBMEDIAUTILS_BUILD_LIB, test x$swscale = xyes)
    PKG_CHECK_MODULES(LIBSWSCALE, libswscale >= 1.0.0)
    PKG_CHECK_MODULES(LIBAVUTIL, libavutil >= 1.0.0)
    PKG_CHECK_MODULES(LIBAVFORMAT, libavformat >= 1.0.0)
fi

AC_ARG_ENABLE(swcrop,
                AS_HELP_STRING([--enable-swcrop],
                            [enable software swcrop]) ,
                [swcrop="$enableval"] ,
                [swcrop="no"]
            )
AM_CONDITIONAL(QLIBMEDIAUTILS_SWCROP, test x$swcrop = xyes)
AC_MSG_RESULT("---------------swcrop ->"  $swcrop )
if test  x$swcrop = xyes ;then
    AM_CONDITIONAL(QLIBMEDIAUTILS_BUILD_LIB, test x$swcrop = xyes)
    PKG_CHECK_MODULES(LIBAVUTIL, libavutil >= 1.0.0)
    PKG_CHECK_MODULES(LIBAVFORMAT, libavformat >= 1.0.0)
    PKG_CHECK_MODULES(LIBAVFILTER, libavfilter >= 1.0.0)
fi

AC_ARG_ENABLE(testing,
                AS_HELP_STRING([--enable-testing],
                            [install testing script]) ,
                [testing="$enableval"] ,
                [testing="no"]
            )
AM_CONDITIONAL(QLIBMEDIAUTILS_TESTING, test x$testing = xyes)

AC_OUTPUT(Makefile)
