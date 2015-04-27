EXSTATIK_SRCS = Codegen.cpp \
    Codegen.h \
    Compiler.cpp \
    Compiler.h \
    Lexer.cpp \
    Lexer.h \
    Parser.cpp \
    Parser.h

LIB_CXXFLAGS = -I../

lib_LIBRARIES = libexstatik.a
libexstatik_a_SOURCES = $(EXSTATIK_SRCS)
libexstatik_a_LIBADD = ../statik/libstatik.a ../util/libutil.a
libexstatik_a_CXXFLAGS = -ggdb -Wall -pedantic $(BOOST_CXXFLAGS) $(LIB_CXXFLAGS)

bin_PROGRAMS = exstatik
exstatik_SOURCES = main.cpp
exstatik_LDADD = libexstatik.a ../statik/libstatik.a ../util/libutil.a
exstatik_CXXFLAGS = -ggdb -Wall -pedantic $(BOOST_CXXFLAGS) $(LIB_CXXFLAGS)
exstatik_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_REGEX_LDFLAGS)
LIBS = $(BOOST_PROGRAM_OPTIONS_LIBS) $(BOOST_REGEX_LIBS)
