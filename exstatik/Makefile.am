EXSTATIK_SRCS = Compiler.cpp \
    Compiler.h \
    Lexer.cpp \
    Lexer.h \
    Parser.cpp \
    Parser.h \
    main.cpp

STATIK_CXXFLAGS = -I../

bin_PROGRAMS = exstatik
exstatik_SOURCES = $(EXSTATIK_SRCS)
exstatik_LDADD = ../statik/libstatik.a
exstatik_CXXFLAGS = -ggdb -Wall -pedantic $(BOOST_CXXFLAGS) $(STATIK_CXXFLAGS)
exstatik_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_REGEX_LDFLAGS)
LIBS = $(BOOST_PROGRAM_OPTIONS_LIBS) $(BOOST_REGEX_LIBS)
