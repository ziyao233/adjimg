#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.64])
AC_INIT([adjimg], [0.0.1-dev], [Yao Zi <ziyao@disroot.org>])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([Makefile])

AM_INIT_AUTOMAKE([])

AC_PROG_CC

m4_include([m4/optional_set_cflag.m4])

OPTIONAL_SET_CFLAGS_INIT

OPTIONAL_SET_CFLAGS([-Wall])
OPTIONAL_SET_CFLAGS([-pedantic])

PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES([WAYLAND_CLIENT], [wayland-client])
PKG_CHECK_MODULES([WAYLAND_PROTOCOLS], [wayland-protocols])
PKG_CHECK_VAR([wayland_protocols_path], [wayland-protocols], [pkgdatadir], [],
	      [AC_MSG_ERROR([pkgdatadir isn't defined in wayland-protocols.pc])])

AC_MSG_CHECKING([whether stable xdg-shell protocol file is present])
xdg_shell_path="$wayland_protocols_path"/stable/xdg-shell/xdg-shell.xml
AS_IF([test -e "$xdg_shell_path"],
      [
	AC_MSG_RESULT([yes])
	AC_SUBST([XDG_SHELL_PATH], ["$xdg_shell_path"])
      ],
      [
        AC_MSG_RESULT([no])
	AC_MSG_ERROR([Couldn't find xdg-shell protocol in "$xdg_shell_path"])
      ])

AC_CHECK_PROG([have_wayland_scanner], [wayland-scanner], [yes], [no])
AS_IF([test "$have_wayland_scanner" = "no"],
      [AC_MSG_ERROR([wayland-scanner is required])])

AC_OUTPUT
