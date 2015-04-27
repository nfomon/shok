STATIK_SRCS = \
    ComputeFunc.h \
    Connector.cpp \
    Connector.h \
    ConnectorAction.h \
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

LIB_CXXFLAGS = -I ../

lib_LIBRARIES = libstatik.a
libstatik_a_LIBADD = ../util/libutil.a
libstatik_a_SOURCES = $(STATIK_SRCS)
libstatik_a_CXXFLAGS = -ggdb -Wall -pedantic $(BOOST_CXXFLAGS) $(LIB_CXXFLAGS)
