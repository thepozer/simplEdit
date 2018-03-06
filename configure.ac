AC_INIT([simplEdit], [0.4.0], [Didier prolhac <dev@thepozer.net>])
AC_CONFIG_SRCDIR([src/simplEdit_main.c])
AM_INIT_AUTOMAKE([-Wall foreign])
AM_SILENT_RULES([yes])

AC_PROG_CC
PKG_PROG_PKG_CONFIG	

GLIB_GSETTINGS

PKG_CHECK_MODULES([SIMPLEDIT], [glib-2.0 gmodule-2.0 gtk+-3.0 gtksourceview-3.0], , exit)

AC_CONFIG_FILES([
    Makefile
    src/Makefile
])
AC_OUTPUT
