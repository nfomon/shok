AC_INIT([shok], [0.01], [http://shok.io])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

AC_PROG_CXX
AC_PROG_LIBTOOL

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
