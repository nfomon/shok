ISTATIK_SRCS = \
    ISError.h \
    ISLog.cpp \
    ISLog.h \
    IStatik.cpp \
    IStatik.h \
    InputWindow.cpp \
    InputWindow.h \
    ParserWindow.cpp \
    ParserWindow.h \
    WindowAction.h \
    WindowResponse.h \
    main.cpp

LIB_CXXFLAGS = -I../

bin_PROGRAMS = istatik
istatik_SOURCES = $(ISTATIK_SRCS)
istatik_LDADD = ../exstatik/libexstatik.a ../statik/libstatik.a ../util/libutil.a
istatik_CXXFLAGS = -ggdb -Wall -pedantic $(BOOST_CXXFLAGS) $(LIB_CXXFLAGS)
istatik_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_REGEX_LDFLAGS)
LIBS = $(BOOST_PROGRAM_OPTIONS_LIBS) $(BOOST_REGEX_LIBS) $(CURSES_LIB)
