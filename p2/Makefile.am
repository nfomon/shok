UTIL_DIR = ../util
UTIL_SRCS = \
    $(UTIL_DIR)/Log.cpp \
    $(UTIL_DIR)/Log.h

COMMON_SRCS = $(UTIL_SRCS)

bin_PROGRAMS = shok_p2
shok_p2_SOURCES = $(COMMON_SRCS) \
    Cmd.h \
    ParseError.h \
    Parser.cpp \
    Parser.h \
    Token.cpp \
    Token.h \
    parser.cpp
shok_p2_CPPFLAGS = $(BOOST_CPPFLAGS) -DBOOST_SPIRIT_DEBUG
#shok_p2_CPPFLAGS = $(BOOST_CPPFLAGS)
shok_p2_LDFLAGS = $(BOOST_IOSTREAMS_LDFLAGS)
LIBS = $(BOOST_IOSTREAMS_LIBS)
