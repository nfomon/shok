UTIL_DIR = ../util
UTIL_SRCS = \
    $(UTIL_DIR)/Log.cpp \
    $(UTIL_DIR)/Log.h

COMMON_SRCS = $(UTIL_SRCS)

bin_PROGRAMS = shok_c2
shok_c2_SOURCES = $(COMMON_SRCS) \
    Atom.cpp \
    Atom.h \
    Cmd.cpp \
    Cmd.h \
    Code.cpp \
    Code.h \
    Common.h \
    CompileError.h \
    Compiler.cpp \
    Compiler.h \
    ExpParser.h \
    Expression.cpp \
    Expression.h \
    Function.cpp \
    Function.h \
    IntLiteral.cpp \
    IntLiteral.h \
    Literal.h \
    NewInit.cpp \
    NewInit.h \
    Object.cpp \
    Object.h \
    Operator.cpp \
    Operator.h \
    Scope.cpp \
    Scope.h \
    StdLib.cpp \
    StdLib.h \
    StringLiteral.cpp \
    StringLiteral.h \
    SymbolTable.h \
    Type.cpp \
    Type.h \
    Variable.cpp \
    Variable.h \
    compiler.cpp
#shok_c2_CPPFLAGS = $(BOOST_CPPFLAGS) -DBOOST_SPIRIT_DEBUG
shok_c2_CPPFLAGS = $(BOOST_CPPFLAGS)
shok_c2_LDFLAGS = $(BOOST_IOSTREAMS_LDFLAGS)
LIBS = $(BOOST_IOSTREAMS_LIBS)
