UTIL_DIR = ../util
UTIL_SRCS = \
    $(UTIL_DIR)/Graphviz.cpp \
    $(UTIL_DIR)/Graphviz.h \
    $(UTIL_DIR)/Log.cpp \
    $(UTIL_DIR)/Log.h \
    $(UTIL_DIR)/Util.cpp \
    $(UTIL_DIR)/Util.h

COMMON_SRCS = $(UTIL_SRCS)

STATIK_SRCS = $(COMMON_SRCS) \
    ComputeFunc.h \
    Connector.cpp \
    Connector.h \
    Grapher.cpp \
    Grapher.h \
    Hotlist.cpp \
    Hotlist.h \
    IConnection.h \
    IList.cpp \
    IList.h \
    Keyword.cpp \
    Keyword.h \
    ListenerTable.h \
    Meta.cpp \
    Meta.h \
    ObjectPool.h \
    OData.h \
    Or.cpp \
    Or.h \
    OutputFunc.cpp \
    OutputFunc.h \
    Regexp.cpp \
    Regexp.h \
    RestartFunc.cpp \
    RestartFunc.h \
    Rule.cpp \
    Rule.h \
    SError.h \
    SLog.cpp \
    SLog.h \
    STree.cpp \
    STree.h \
    Seq.cpp \
    Seq.h \
    Star.cpp \
    Star.h \
    State.cpp \
    State.h

lib_LIBRARIES = libstatik.a
libstatik_a_SOURCES = $(STATIK_SRCS)
libstatik_a_CXXFLAGS = -ggdb -Wall -pedantic $(BOOST_CXXFLAGS)
