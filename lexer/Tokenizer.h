// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Tokenizer_h_
#define _Tokenizer_h_

/* Tokenizer: defines tokens and transforms stream of text into stream of
 * tokens
 */

#include <boost/spirit/include/lex_lexertl.hpp>
namespace spirit = boost::spirit;
namespace lex = spirit::lex;

#include <ostream>
using std::endl;

namespace lexer {

enum TokenIDs {
  // Shell control
  T_EXIT = lex::min_token_id + 1,

  // Symbol table modifiers
  T_NEW, T_RENEW, T_DEL, T_ISVAR, T_TYPEOF,

  // Functions
  T_AT, T_RETURN, //T_VOID, T_YIELD,

  // Branch constructs
  T_IF, T_ELIF, T_ELSE,
  // T_SWITCH, T_CASE, T_DEFAULT

  // Loop constructs
  T_WHILE, T_LOOP, T_TIMES,
  //T_EACH, T_IN, T_WHERE,
  //T_BREAK, T_CONTINUE,

  // Logical operators
  T_NOT, T_NOR, T_AND, T_OR, T_XOR, T_XNOR,

  // Equality operators
  T_LT, T_LE, T_GT, T_GE, T_EQ, T_NE,

  // Numeric operators
  T_PLUS, T_MINUS, T_STAR, T_SLASH, T_PERCENT, T_CARAT,

  // Object operators
  T_PIPE, T_AMP, T_DOUBLETILDE, T_TILDE,

  // Assignment operators
  T_EQUALS, T_PLUSEQUALS, T_MINUSEQUALS, T_STAREQUALS, T_SLASHEQUALS,
  T_PERCENTEQUALS, T_CARATEQUALS, T_PIPEEQUALS, T_AMPEQUALS,
  T_DOUBLETILDEEQUALS, T_TILDEEQUALS,

  // Cast: ->
  T_ARROW,

  // Delimeters: () [] {} , . :
  T_LPAREN, T_RPAREN,
  T_LBRACKET, T_RBRACKET,
  T_LBRACE, T_RBRACE,
  T_COMMA, T_DOT, T_COLON,

  // End of line: ; \n
  T_SEMI,
  T_NEWL,

  // Literals
  T_INT,
  //T_FIXED,
  T_STR,
  //T_REGEXP,
  //T_LABEL,
  //T_USEROP,
  T_ID,

  // Whitespace
  T_WS
};

template <typename L>
struct Tokenizer : lex::lexer<L> {
  Tokenizer()
    : Tokenizer::base_type(lex::match_flags::match_not_dot_newline) {

    this->self.add_pattern
      ("ID", "[a-zA-Z_][a-zA-Z0-9_]*")
      ("INT", "[0-9]+")
      //("FIXED", "[0-9]+\\.[0-9]+")
      ("SQSTR", "\\\'([^\\\'\\\\\\\\]|\\\\.)*\\\'")
      ("DQSTR", "\\\"([^\\\"\\\\\\\\]|\\\\.)*\\\"")
      ("STR", "{SQSTR}|{DQSTR}")
      ("WS", "[ \t\r]+")
    ;

    this->self.add
      // Shell control
      ("exit", T_EXIT)

      // Symbol table modifiers
      ("new", T_NEW)
      ("renew", T_RENEW)
      ("del", T_DEL)
      ("isvar", T_ISVAR)

      // Functions
      ('@', T_AT)
      ("return", T_RETURN)

      // Branch constructs
      ("if", T_IF)
      ("elif", T_ELIF)
      ("else", T_ELSE)

      // Loop constructs
      ("while", T_WHILE)
      ("loop", T_LOOP)
      ("times", T_TIMES)

      // Logical operators
      ("not", T_NOT)
      ("nor", T_NOR)
      ("and", T_AND)
      ("or", T_OR)
      ("xor", T_XOR)
      ("xnor", T_XNOR)

      // Equality operators
      ("\"<\"", T_LT)
      ("\"<=\"", T_LE)
      ("\">\"", T_GT)
      ("\">=\"", T_GE)
      ("\"==\"", T_EQ)
      ("\"!=\"", T_NE)

      // Numeric operators
      ("\"+\"", T_PLUS)
      ("\"-\"", T_MINUS)
      ("\"*\"", T_STAR)
      ("\"/\"", T_SLASH)
      ("\"%\"", T_PERCENT)
      ("\"^\"", T_CARAT)

      // Object operators
      ("\"|\"", T_PIPE)
      ("\"&\"", T_AMP)
      ("\"~~\"", T_DOUBLETILDE)
      ("\"~\"", T_TILDE)

      // Assignment operators
      ("\"=\"", T_EQUALS)
      ("\"+=\"", T_PLUSEQUALS)
      ("\"-=\"", T_MINUSEQUALS)
      ("\"*=\"", T_STAREQUALS)
      ("\"/=\"", T_SLASHEQUALS)
      ("\"%=\"", T_PERCENTEQUALS)
      ("\"^=\"", T_CARATEQUALS)
      ("\"|=\"", T_PIPEEQUALS)
      ("\"&=\"", T_AMPEQUALS)
      ("\"~~=\"", T_DOUBLETILDEEQUALS)
      ("\"~=\"", T_TILDEEQUALS)

      // Cast: ->
      ("\"->\"", T_ARROW)

      // Delimeters: () [] {} , . :
      ("\"(\"", T_LPAREN)
      ("\")\"", T_RPAREN)
      ("\"[\"", T_LBRACKET)
      ("\"]\"", T_RBRACKET)
      ("\"{\"", T_LBRACE)
      ("\"}\"", T_RBRACE)
      ("\",\"", T_COMMA)
      ("\".\"", T_DOT)
      ("\":\"", T_COLON)

      // End of line: ; \n
      (';', T_SEMI)
      ("\n", T_NEWL)

      // Literals
      ("{INT}", T_INT)
      //("{FIXED}", T_FIXED)
      ("{STR}", T_STR)
      ("{ID}", T_ID)

      // Whitespace
      ("{WS}", T_WS)
    ;
  }
};

}

#endif // _Tokenizer_h_
