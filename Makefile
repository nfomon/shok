# Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
# directory of this distribution and at http://shok.io/code/copyright.html

.PHONY: clean
.PHONY: tidy

# Quex (lexer)
ifndef QUEX_PATH
  $(error The environment variable QUEX_PATH must be defined)
endif

SOURCES = lexer/tiny_lexer \
          lexer/tiny_lexer.cpp \
          lexer/tiny_lexer-token_ids \
          lexer/tiny_lexer-configuration

ifdef ASSERTS_ENABLED_F
  CCFLAG_ASSERTS=# By default asserts are enabled
else
  CCFLAG_ASSERTS=-DQUEX_OPTION_ASSERTS_DISABLED
endif

CC = g++ -ggdb -Wall -pedantic
QUEXCC = $(CC) -I./ -I$(QUEX_PATH) $(CCFLAG_ASSERTS) \
     # -DQUEX_OPTION_DEBUG_SHOW
     # -DQUEX_OPTION_ASSERTS_DISABLED
     # -DQUEX_OPTION_ASSERTS_WARNING_MESSAGE_DISABLED

LD = $(COMPILER)

# Rules
all: shok_lexer shok_parser shok_c2 shok_vm shok

shok_lexer: lexer/lexer.cpp lexer/tiny_lexer_st.cpp
	$(QUEXCC) -o $@ $^

lexer/tiny_lexer_st.cpp: lexer/lexer.qx $(QUEX_CORE)
	quex -i lexer/lexer.qx --engine tiny_lexer_st \
    --token-memory-management-by-user     \
    --token-policy single
	mv tiny_lexer_st* lexer/

shok_parser: parser/shok_parser.py
	ln -s parser/shok_parser.py shok_parser

shok_compiler: util/*.h util/*.cpp compiler/*.h compiler/*.cpp
	$(CC) `llvm-config-3.4 --cxxflags` -fexceptions -frtti -Wno-long-long util/*.cpp compiler/*.cpp `llvm-config-3.4 --libs` `llvm-config-3.4 --ldflags` -o shok_compiler

shok_c2: util/*.h util/*.cpp c2/*.h c2/*.cpp
	$(CC) util/*.cpp c2/*.cpp -o shok_c2

shok_vm: util/*.h util/*.cpp vm/*.h vm/*.cpp
	$(CC) util/*.cpp vm/*.cpp -o shok_vm

shok: util/Proc.h util/Util.h shell/shell.cpp
	$(CC) util/*.cpp shell/shell.cpp -lboost_iostreams -o shok

tidy: lexer shok
	rm -f lexer/tiny_lexer_st* lexer/test_lexer parser/*.pyc compiler/*.o c2/*.o vm/*.o shell/file_descriptor.o shell/shell.o parser.log compiler.log vm.log

clean:
	rm -f lexer/tiny_lexer_st* lexer/test_lexer parser/*.pyc compiler/*.o c2/*.o vm/*.o shell/file_descriptor.o shell/shell.o shok_lexer shok_parser shok_compiler shok_c2 shok_vm shok parser.log compiler.log vm.log

lexer/test_lexer: shok_lexer lexer/test_lexer.cpp
	g++ lexer/test_lexer.cpp -lboost_iostreams -o lexer/test_lexer

test: lexer/test_lexer
	./lexer/test_lexer
	python parser/ParserTest.py
	python parser/ShokParserTest.py
