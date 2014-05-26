AC_INIT([shok], [0.01], [http://shok.io])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CXX
AC_PROG_LIBTOOL

AC_ARG_VAR([QUEX_PATH], [Location of Quex installation])
AC_CHECK_PROG(FOUND_QUEX, quex, yes, no)
test "x$FOUND_QUEX" = "xyes" || exit 1
test -n "$QUEX_PATH" || AC_MSG_ERROR([QUEX_PATH not defined.  Please set QUEX_PATH to the location of your Quex installation.])

AM_PATH_PYTHON([2])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([
  Makefile
  lexer/Makefile
  parser/Makefile
  parser/shokparser/Makefile
  c2/Makefile
  vm/Makefile
  shell/Makefile
])

BOOST_REQUIRE
BOOST_IOSTREAMS

AC_OUTPUT
