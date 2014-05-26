UTIL_DIR = ../util
UTIL_SRCS = \
    $(UTIL_DIR)/Log.cpp \
    $(UTIL_DIR)/Log.h

COMMON_SRCS = $(UTIL_SRCS)

bin_PROGRAMS = shok_vm
shok_vm_SOURCES = $(COMMON_SRCS) \
    Call.cpp \
    Call.h \
    Cmd.cpp \
    Cmd.h \
    Context.cpp \
    Context.h \
    Del.cpp \
    Del.h \
    Executor.cpp \
    Executor.h \
    ExpParser.h \
    Expression.cpp \
    Expression.h \
    Identifier.h \
    Instruction.cpp \
    Instruction.h \
    InstructionParser.h \
    Instructions.h \
    New.cpp \
    New.h \
    Object.cpp \
    Object.h \
    StdLib.cpp \
    StdLib.h \
    Symbol.h \
    VMError.h \
    vm.cpp
shok_vm_CPPFLAGS = $(BOOST_CPPFLAGS)
