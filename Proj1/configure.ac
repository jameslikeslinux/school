AC_INIT(uri/uri.c)
AM_INIT_AUTOMAKE(Proj1, 0.1)
AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AC_LANG_C
AC_PROG_LIBTOOL

AC_CHECK_HEADER(ncurses.h, , AC_MSG_ERROR([ui requires ncurses]))
AC_CHECK_LIB(ncurses, initscr, , AC_MSG_ERROR([ui requires ncurses]))

AC_OUTPUT(Makefile list/Makefile log/Makefile uri/Makefile ui/Makefile)
