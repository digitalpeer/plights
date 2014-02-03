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
AC_INIT(plights,0.1)
AM_INIT_AUTOMAKE

#AM_CONFIG_HEADER([src/config.h])

CFLAGS=" $CFLAGS"
CXXFLAGS=" $CXXFLAGS"
LDFLAGS=" $LDFLAGS"

AC_ARG_ENABLE(prof,    [  --enable-prof          compile with debugging support],,enable_prof=no)
AC_ARG_ENABLE(debug,   [  --enable-debug         compile with debugging support],,enable_debug=no)
AC_ARG_ENABLE(crypto,  [  --enable-crypto        compile with encryption support (use crypto++ library)],,enable_crypto=no)

if test "$enable_prof" = yes ; then
	CFLAGS=" $CFLAGS -pg"
	CXXFLAGS=" $CXXFLAGS -pg"
	LDFLAGS=" $LDFLAGS -pg"
fi

if test "$enable_debug" = yes ; then
	CFLAGS=" $CFLAGS -DDEBUG"
	CXXFLAGS=" $CXXFLAGS -DDEBUG"
	#AC_DEFINE(DEBUG, 1, [Define if debugging is enabled.])
fi

if test "$enable_crypto" = yes ; then
	AC_DEFINE(ENABLE_ENCRYPTION, 1, [Define if encryption is enabled.])
fi

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_AWK
AC_PROG_RANLIB
#AC_PROG_LIBTOOL

# Checks for libraries.
AM_OPTIONS_WXCONFIG
AM_PATH_WXCONFIG(2.4.0, WXFOUND=1)

if test "$WXFOUND" != 1; then
    AC_MSG_ERROR([
        Please check that wx-config is in path, the directory
        where wxWidgets libraries are installed (returned by
        'wx-config --libs' command) is in LD_LIBRARY_PATH or
        equivalent variable and wxWidgets is version 2.4.0 or above.
	Or this might also be a bug in our configure. Please try again
	with --with-wx-config=/usr/local/bin/wx-config
	(replace /usr/bin/wx-config with a valid path to your wx-config)
	* Note:
	Most probably, either one of the above aren't correct, you don't
	have wxGTK installed, or are missing wxGTK-devel (or equivalent) package.
    ])
fi

#CHECK_WX_BUILT_WITH_GTK2

if test x"$GTK_USEDVERSION" = x"2" ; then
  AM_PATH_GTK_2_0(2.0.3, havegtk2=yes, havegtk2=no)
  GTK_DEFS="-D__GTK2__"
else
  AM_PATH_GTK(1.2.0, havegtk=yes, havegtk=no)
  GTK_DEFS=""
fi

#if test "$havegtk2" != yes; then
#    AC_MSG_ERROR([
#        The wxWidgets library must be built with GTK 2.0.3 or higher.
#    ])
#fi

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h limits.h malloc.h time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
#AC_CHECK_TYPES([])

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MEMCMP
AC_CHECK_FUNCS([memset])

AC_PATH_PROG(plightsplaypath, plightsplay)

AC_OUTPUT([Makefile
			src/Makefile
			src/test/Makefile
			DoxyFile])

echo
echo $PACKAGE $VERSION
echo
eval eval echo plights will be installed in $bindir.
if test "x$plightsplaypath" != "x" ; then
	echo Warning: You have an old copy of plights at $plightsplaypath.
fi
echo
echo configure complete, now type \'make\' and then \'make install\'
echo
