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

COMPILER = g++ -ggdb -Wall -pedantic
CC = $(COMPILER) -I./ -I$(QUEX_PATH) $(CCFLAG_ASSERTS) \
     # -DQUEX_OPTION_DEBUG_SHOW
     # -DQUEX_OPTION_ASSERTS_DISABLED
     # -DQUEX_OPTION_ASSERTS_WARNING_MESSAGE_DISABLED

LD = $(COMPILER)

# Rules
all: lush
#all: tidy

lush: lush_lexer lush_parser lush_eval shell

lush_lexer: lexer/lexer.cpp lexer/tiny_lexer_st.cpp
	$(CC) -o $@ $^

lexer/tiny_lexer_st.cpp: lexer/lexer.qx $(QUEX_CORE)
	quex -i lexer/lexer.qx --engine tiny_lexer_st \
    --token-memory-management-by-user     \
    --token-policy single
	mv tiny_lexer_st* lexer/

lush_parser: parser/lush_parser.py
	ln -s parser/lush_parser.py lush_parser

eval/*.o: eval/*.cpp
	g++ -Iutil $(CFLAGS) -c $< -o $@

lush_eval: eval/*.o
	g++ -Iutil eval/*.cpp -o lush_eval

shell/shell.o: shell/shell.cpp
	g++ -Iutil $(CFLAGS) -c $< -o $@

shell: shell/file_descriptor.o shell/shell.o
	g++ -Iutil shell/file_descriptor.cpp shell/shell.cpp -o lush

tidy: lexer lush
	rm -f lexer/tiny_lexer_st* parser/*.pyc eval/*.o shell/file_descriptor.o shell/shell.o LushParser.log

clean:
	rm -f lexer/tiny_lexer_st* parser/*.pyc eval/*.o shell/file_descriptor.o shell/shell.o lush_lexer lush_parser lush_eval lush LushParser.log

test:
	python parser/ParserTest.py
	python parser/LushParserTest.py
