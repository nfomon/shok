UTIL_DIR = ../util
UTIL_SRCS = \
    $(UTIL_DIR)/Log.cpp \
    $(UTIL_DIR)/Log.h \
    $(UTIL_DIR)/Proc.cpp \
    $(UTIL_DIR)/Proc.h \
    $(UTIL_DIR)/Util.cpp \
    $(UTIL_DIR)/Util.h

COMMON_SRCS = $(UTIL_SRCS)

bin_PROGRAMS = shok_shell
shok_shell_SOURCES = $(COMMON_SRCS) \
    shell.cpp
shok_shell_CPPFLAGS = $(BOOST_CPPFLAGS)
shok_shell_LDFLAGS = $(BOOST_IOSTREAMS_LDFLAGS)
LIBS = $(BOOST_IOSTREAMS_LIBS)
