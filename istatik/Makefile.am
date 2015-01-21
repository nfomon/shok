UTIL_DIR = ../util
UTIL_SRCS = \
    $(UTIL_DIR)/Log.cpp \
    $(UTIL_DIR)/Log.h

COMMON_SRCS = $(UTIL_SRCS)

ISTATIK_SRCS = $(COMMON_SRCS) \
    ISError.h \
    ISLog.cpp \
    ISLog.h \
    main.cpp

STATIK_CXXFLAGS = -I../

bin_PROGRAMS = istatik
istatik_SOURCES = $(ISTATIK_SRCS)
istatik_LDADD = ../exstatik/libexstatik.a ../statik/libstatik.a
istatik_CXXFLAGS = -ggdb -Wall -pedantic $(BOOST_CXXFLAGS) $(STATIK_CXXFLAGS)
istatik_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS)
LIBS = $(BOOST_PROGRAM_OPTIONS_LIBS) $(CURSES_LIB)
