UTIL_DIR = ../util
UTIL_SRCS = \
    $(UTIL_DIR)/Log.cpp \
    $(UTIL_DIR)/Log.h

COMMON_SRCS = $(UTIL_SRCS)

bin_PROGRAMS = shok_lexer
shok_lexer_SOURCES = $(COMMON_SRCS) \
    Common.h \
    Emitter.h \
    Emitter.cpp \
    Lexer.cpp \
    Lexer.h \
    LexError.h \
    Tokenizer.h \
    lexer.cpp
shok_lexer_CPPFLAGS = $(BOOST_CPPFLAGS)
