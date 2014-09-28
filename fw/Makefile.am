UTIL_DIR = ../util
UTIL_SRCS = \
    $(UTIL_DIR)/Log.cpp \
    $(UTIL_DIR)/Log.h

COMMON_SRCS = $(UTIL_SRCS)

bin_PROGRAMS = shok_fw
shok_fw_SOURCES = $(COMMON_SRCS) \
    Char.h \
    Connector.cpp \
    Connector.h \
    DS.h \
    FWError.h \
    Keyword.h \
    ListenerTable.h \
    Or.cpp \
    Or.h \
    Rule.cpp \
    Rule.h \
    Star.cpp \
    Star.h \
    State.h \
    TreeChangeset.h \
    main.cpp

shok_fw_CXXFLAGS = -ggdb -Wall -pedantic
