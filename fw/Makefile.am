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
    Compiler.cpp \
    Compiler.h \
    Connector.cpp \
    Connector.h \
    FWError.h \
    FWTree.cpp \
    FWTree.h \
    Grapher.cpp \
    Grapher.h \
    Hotlist.cpp \
    Hotlist.h \
    IConnection.h \
    IList.cpp \
    IList.h \
    Keyword.h \
    Lexer.cpp \
    Lexer.h \
    ListenerTable.h \
    Meta.cpp \
    Meta.h \
    OData.h \
    Or.cpp \
    Or.h \
    OutputStrategy.cpp \
    OutputStrategy.h \
    Parser.cpp \
    Parser.h \
    Regexp.cpp \
    Regexp.h \
    Rule.cpp \
    Rule.h \
    Seq.cpp \
    Seq.h \
    Star.cpp \
    Star.h \
    State.h \
    main.cpp

shok_fw_CXXFLAGS = -ggdb -Wall -pedantic $(BOOST_CXXFLAGS)
shok_fw_LDFLAGS = $(BOOST_REGEX_LDFLAGS)
LIBS = $(BOOST_REGEX_LIBS)
