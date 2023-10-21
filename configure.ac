dnl  Autoconf script for termux-tools
dnl To rebuild the 'configure' script from this, execute the command
dnl	autoconf
dnl in the directory containing this script.
dnl
dnl Copyright (C) 2022 Termux
dnl
dnl  This file is part of termux-tools.
dnl
dnl  termux-tools is free software: you can redistribute it
dnl  and/or modify it under the terms of the GNU General Public
dnl  License as published by the Free Software Foundation, either
dnl  version 3 of the License, or (at your option) any later version.
dnl
dnl  termux-tools is distributed in the hope that it will be
dnl  useful, but WITHOUT ANY WARRANTY; without even the implied
dnl  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
dnl  See the GNU General Public License for more details.
dnl
dnl  You should have received a copy of the GNU General Public License
dnl  along with termux-tools.  If not, see
dnl  <https://www.gnu.org/licenses/>.

AC_PREREQ([2.69])
AC_INIT([termux-tools], [1.39.3], [support@termux.dev])

AM_INIT_AUTOMAKE([foreign])

AC_PROG_MAKE_SET

copyright="Copyright (C) 2022 Termux."
if test "${TERMUX_APP_PACKAGE+set}" = set; then
  termux_app_package="$TERMUX_APP_PACKAGE"
else
  termux_app_package="com.termux"
fi
if test "${TERMUX_BASE_DIR+set}" = set; then
  termux_base_dir="$TERMUX_BASE_DIR"
else
  termux_base_dir="/data/data/$termux_app_package/files"
fi
if test "${TERMUX_CACHE_DIR+set}" = set; then
  termux_cache_dir="$TERMUX_CACHE_DIR"
else
  termux_cache_dir="/data/data/$termux_app_package/cache"
fi
if test "${TERMUX_PREFIX+set}" = set; then
  termux_prefix="$TERMUX_PREFIX"
else
  termux_prefix="$termux_base_dir/usr"
fi
if test "${TERMUX_ANDROID_HOME+set}" = set; then
  termux_android_home="$TERMUX_ANDROID_HOME"
else
  termux_android_home="$termux_base_dir/home"
fi
if test "${TERMUX_PACKAGE_FORMAT+set}" = set; then
  termux_package_format="$TERMUX_PACKAGE_FORMAT"
else
  termux_package_format="debian"
fi
if test "${TERMUX_PACKAGE_MANAGER+set}" = set; then
  termux_package_manager="$TERMUX_PACKAGE_MANAGER"
else
  termux_package_manager="apt"
fi

AC_DEFINE_UNQUOTED(COPYRIGHT, ["$copyright"],
  [Short copyright string for this version of termux-tools.])
AC_DEFINE_UNQUOTED(TERMUX_APP_PACKAGE, ["$termux_app_package"],
  [Termux namespace])
AC_DEFINE_UNQUOTED(TERMUX_BASE_DIR, ["$termux_base_dir"],
  [Base directory of termux application])
AC_DEFINE_UNQUOTED(TERMUX_CACHE_DIR, ["$termux_cache_dir"],
  [Cache directory of termux application])
AC_DEFINE_UNQUOTED(TERMUX_PREFIX, ["$termux_prefix"],
  [Termux prefix])
AC_DEFINE_UNQUOTED(TERMUX_ANDROID_HOME, ["$termux_android_home"],
  [Termux home directory])
AC_DEFINE_UNQUOTED(TERMUX_PACKAGE_FORMAT, ["$termux_package_format"],
  [Packaging format (debian or pacman)])
AC_DEFINE_UNQUOTED(TERMUX_PACKAGE_MANAGER, ["$termux_package_manager"],
  [Package manager (apt or pacman)])

AC_SUBST(termux_app_package)
AC_SUBST(termux_base_dir)
AC_SUBST(termux_cache_dir)
AC_SUBST(termux_prefix)
AC_SUBST(termux_android_home)
AC_SUBST(termux_package_format)
AC_SUBST(termux_package_manager)

AC_PROG_LN_S

AC_CONFIG_FILES([Makefile scripts/Makefile doc/Makefile
mirrors/Makefile motds/Makefile])

AC_OUTPUT
