AC_INIT([shok], [0.01], [http://shok.io])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_PROG_AR

AC_PROG_CXX
AC_PROG_LIBTOOL

AM_PATH_PYTHON([2])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AX_WITH_CURSES

AC_CONFIG_FILES([
  Makefile
  util/Makefile
  statik/Makefile
  statik/test/Makefile
  exstatik/Makefile
  istatik/Makefile
])
#  lexer/Makefile
#  p2/Makefile
#  c2/Makefile
#  vm/Makefile
#  shell/Makefile

BOOST_REQUIRE
BOOST_PROGRAM_OPTIONS
BOOST_REGEX
# BOOST_IOSTREAMS

AC_OUTPUT
