dnl  Autoconf script for termux-fix-shebang
dnl To rebuild the 'configure' script from this, execute the command
dnl	autoconf
dnl in the directory containing this script.
dnl
dnl Copyright (C) 2024 Termux
dnl
dnl  This file is part of termux-fix-shebang.
dnl
dnl  termux-fix-shebang is free software: you can redistribute it
dnl  and/or modify it under the terms of the GNU General Public
dnl  License as published by the Free Software Foundation, either
dnl  version 3 of the License, or (at your option) any later version.
dnl
dnl  termux-fix-shebang is distributed in the hope that it will be
dnl  useful, but WITHOUT ANY WARRANTY; without even the implied
dnl  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
dnl  See the GNU General Public License for more details.
dnl
dnl  You should have received a copy of the GNU General Public License
dnl  along with termux-fix-shebang.  If not, see
dnl  <https://www.gnu.org/licenses/>.

AC_PREREQ([2.69])
AC_INIT([termux-fix-shebang], [1.0.0], [support@termux.dev])

AM_INIT_AUTOMAKE([foreign])

AC_PROG_MAKE_SET
AC_PROG_CC
AC_LANG(C)

copyright="Copyright (C) 2024 Termux."
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
if test "${TERMUX_PREFIX+set}" = set; then
  termux_prefix="$TERMUX_PREFIX"
else
  termux_prefix="$termux_base_dir/usr"
fi

AC_DEFINE_UNQUOTED(COPYRIGHT, ["$copyright"],
  [Short copyright string for this version of termux-fix-shebang.])
AC_DEFINE_UNQUOTED(TERMUX_APP_PACKAGE, ["$termux_app_package"],
  [Termux namespace])
AC_DEFINE_UNQUOTED(TERMUX_BASE_DIR, ["$termux_base_dir"],
  [Base directory of termux application])
AC_DEFINE_UNQUOTED(TERMUX_PREFIX, ["$termux_prefix"],
  [Termux prefix])

AC_SUBST(termux_app_package)
AC_SUBST(termux_base_dir)
AC_SUBST(termux_prefix)

AC_CONFIG_FILES([Makefile tests/Makefile])

AC_CONFIG_TESTDIR(tests)
AM_MISSING_PROG([AUTOM4TE], [autom4te])

AC_OUTPUT
