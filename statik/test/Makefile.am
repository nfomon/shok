STATIK_TEST_SRCS = \
    Battery.cpp \
    Battery.h \
    Keyword.cpp \
    Keyword.h \
    Or.cpp \
    Or.h \
    STError.h \
    STLog.cpp \
    STLog.h \
    Star.cpp \
    Star.h \
    StatikBattery.h \
    Test.cpp \
    Test.h \
    main.cpp

LIB_CXXFLAGS = -I../ -I../../

bin_PROGRAMS = statik_test
statik_test_SOURCES = $(STATIK_TEST_SRCS)
statik_test_LDADD = ../libstatik.a ../../util/libutil.a
statik_test_CXXFLAGS = -ggdb -Wall -pedantic $(BOOST_CXXFLAGS) $(LIB_CXXFLAGS)
statik_test_LDFLAGS = $(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_REGEX_LDFLAGS)
LIBS = $(BOOST_PROGRAM_OPTIONS_LIBS) $(BOOST_REGEX_LIBS)
