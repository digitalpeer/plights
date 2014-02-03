#
#  plights
#
#  Copyright (C) 2006, Joshua D. Henderson <www.digitalpeer.com>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.53)
AC_INIT(plights.c)
AC_CANONICAL_TARGET([])
AM_INIT_AUTOMAKE(plights, 0.1)
AC_CONFIG_HEADER(config.h)
AM_DISABLE_STATIC

AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_PROG_INSTALL
AM_PROG_LIBTOOL

AC_PATH_X

AM_PATH_GLIB(1.2.2,,AC_MSG_ERROR([*** GLIB >= 1.2.2 not installed - please install first ***]))
AM_PATH_XMMS(1.2.4,,AC_MSG_ERROR([*** XMMS >= 1.2.4 not installed - please install first ***]))
AM_PATH_GTK(1.2.2,,AC_MSG_ERROR([*** GTK+ >= 1.2.2 not installed - please install first ***]),gthread)
AM_PATH_SDL(1.2.0,,AC_MSG_ERROR([*** SDL >= 1.2.0 not installed - please install first ***]))

LIBS_save=$LIBS
LIBS="$LIBS $GTK_LIBS"

MATHLIB="-lm"
AC_PATH_X
AC_PATH_XTRA
if test x$have_x = xyes; then
   CFLAGS="$CFLAGS $X_CFLAGS"
   SYS_GL_LIBS="$X_LIBS -lGL -lGLU"
else
   SYS_GL_LIBS="-lGL -lGLU"
fi


CPPFLAGS=$CPPFLAGS_save
LIBS=$LIBS_save

AC_OUTPUT(Makefile)
