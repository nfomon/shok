The shok interpreter-system is built using the GNU autotools:

 - GNU Autoconf 2.69
   https://www.gnu.org/software/autoconf
   GNU GPL v3+

 - GNU Automake 1.13.3+
   https://www.gnu.org/software/automake
   GNU GPL v2+

 - GNU Libtool 2.4.2
   https://www.gnu.org/software/libtool
   GNU GPL v2+

 - GNU Make 3.81
   https://www.gnu.org/software/make
   GNU GPL v2+

To build and install, navigate to the base directory of the package and run:

    autoreconf --install
    ./configure
    make
    make install

The configure script has many options.  In particular I STRONGLY recommend
using the  --prefix  option to install to a sandbox directory, instead of your
system folders.  This is appropriate until shok has matured.  For example:

    ./configure --prefix=${HOME}/apps

will install to a personal "apps" subdirectory of your home dir.  Then just add
the  bin  subdirectory to your shell's PATH environment variable:
    export PATH=${PATH}:${HOME}/apps/bin


The project has the following build dependencies:

Shell, Lexer, Compiler, VM:
 - The GNU C++ compiler (g++) 4.7.2, part of GCC: the GNU Compiler Collection
   http://gcc.gnu.org
   GNU GPL v3
   (C) Free Software Foundation (FSF)

 - Boost C++ libraries (iostreams, spirit, miscellaneous)
   http://www.boost.org
   Boost Software License v1.0

Parser:
 - Python 2.7
   http://python.org/
   See the license of your favourite compiler/interpreter
   Maybe 2.6 or some earlier versions will work
