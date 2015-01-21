EXSTATIK_SRCS = Compiler.cpp \
    Compiler.h \
    Lexer.cpp \
    Lexer.h \
    Parser.cpp \
    Parser.h

STATIK_CXXFLAGS = -I../

lib_LIBRARIES = libexstatik.a
libexstatik_a_SOURCES = $(EXSTATIK_SRCS)
libexstatik_a_LIBADD = ../statik/libstatik.a
libexstatik_a_CXXFLAGS = -ggdb -Wall -pedantic $(BOOST_CXXFLAGS) $(STATIK_CXXFLAGS)

bin_PROGRAMS = exstatik
exstatik_SOURCES = main.cpp
exstatik_LDADD = libexstatik.a ../statik/libstatik.a
exstatik_CXXFLAGS = -ggdb -Wall -pedantic $(BOOST_CXXFLAGS) $(STATIK_CXXFLAGS)
exstatik_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_REGEX_LDFLAGS)
LIBS = $(BOOST_PROGRAM_OPTIONS_LIBS) $(BOOST_REGEX_LIBS)
