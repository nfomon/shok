UTIL_DIR = ../util
UTIL_SRCS = \
    $(UTIL_DIR)/Log.cpp \
    $(UTIL_DIR)/Log.h

COMMON_SRCS = $(UTIL_SRCS)

bin_PROGRAMS = statik
statik_SOURCES = $(COMMON_SRCS) \
    StatikError.h \
    StatikLog.cpp \
    StatikLog.h \
    main.cpp

statik_CXXFLAGS = -ggdb -Wall -pedantic $(BOOST_CXXFLAGS)
statik_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS)
LIBS = $(BOOST_PROGRAM_OPTIONS_LIBS) $(CURSES_LIB)
