UTIL_DIR = ../util
UTIL_SRCS = \
    $(UTIL_DIR)/Graphviz.cpp \
    $(UTIL_DIR)/Graphviz.h \
    $(UTIL_DIR)/Log.cpp \
    $(UTIL_DIR)/Log.h \
    $(UTIL_DIR)/Util.cpp \
    $(UTIL_DIR)/Util.h

COMMON_SRCS = $(UTIL_SRCS)

bin_PROGRAMS = shok_fw
shok_fw_SOURCES = $(COMMON_SRCS) \
    Char.h \
    Connector.cpp \
    Connector.h \
    DS.cpp \
    DS.h \
    FWError.h \
    Grapher.cpp \
    Grapher.h \
    Hotlist.h \
    Keyword.h \
    ListenerTable.h \
    OData.h \
    Or.cpp \
    Or.h \
    Rule.cpp \
    Rule.h \
    Seq.cpp \
    Seq.h \
    Star.cpp \
    Star.h \
    State.h \
    main.cpp

shok_fw_CXXFLAGS = -ggdb -Wall -pedantic
