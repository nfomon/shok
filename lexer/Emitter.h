// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Emitter_h_
#define _Emitter_h_

/* Token emitter */

#include "Common.h"
#include "LexError.h"
#include "Tokenizer.h"

#include <ostream>
#include <string>

namespace lexer {

class Emitter {
public:
  typedef bool result_type;   // for boost::bind

  Emitter(std::ostream& out, linenum_t& linenum, charnum_t& charnum);

  template <typename Token> bool operator() (const Token& t) {
  switch (t.id()) {

  // keywords (pass through literally)
  case T_EXIT:
  case T_NEW:
  case T_RENEW:
  case T_DEL:
  case T_RETURN:
  case T_IF:
  case T_ELIF:
  case T_ELSE:
  case T_WHILE:
  case T_LOOP:
  case T_TIMES:
  case T_NOT:
  case T_NOR:
  case T_AND:
  case T_OR:
  case T_XOR:
  case T_XNOR:
    p(std::string(t.value().begin(), t.value().end()));
    break;

  // symbols
  case T_AT:
    p("AT"); break;
  case T_LT:
    p("LT"); break;
  case T_LE:
    p("LE"); break;
  case T_GT:
    p("GT"); break;
  case T_GE:
    p("GE"); break;
  case T_EQ:
    p("EQ"); break;
  case T_NE:
    p("NE"); break;
  case T_PLUS:
    p("PLUS"); break;
  case T_MINUS:
    p("MINUS"); break;
  case T_STAR:
    p("STAR"); break;
  case T_SLASH:
    p("SLASH"); break;
  case T_PERCENT:
    p("PERCENT"); break;
  case T_CARAT:
    p("CARAT"); break;
  case T_PIPE:
    p("PIPE"); break;
  case T_AMP:
    p("AMP"); break;
  case T_DOUBLETILDE:
    p("DOUBLETILDE"); break;
  case T_TILDE:
    p("TILDE"); break;
  case T_EQUALS:
    p("EQUALS"); break;
  case T_PLUSEQUALS:
    p("PLUSEQUALS"); break;
  case T_MINUSEQUALS:
    p("MINUSEQUALS"); break;
  case T_STAREQUALS:
    p("STAREQUALS"); break;
  case T_SLASHEQUALS:
    p("SLASHEQUALS"); break;
  case T_PERCENTEQUALS:
    p("PERCENTEQUALS"); break;
  case T_CARATEQUALS:
    p("CARATEQUALS"); break;
  case T_PIPEEQUALS:
    p("PIPEEQUALS"); break;
  case T_AMPEQUALS:
    p("AMPEQUALS"); break;
  case T_DOUBLETILDEEQUALS:
    p("DOUBLETILDEEQUALS"); break;
  case T_TILDEEQUALS:
    p("TILDEEQUALS"); break;
  case T_ARROW:
    p("ARROW"); break;
  case T_LPAREN:
    p("LPAREN"); break;
  case T_RPAREN:
    p("RPAREN"); break;
  case T_LBRACKET:
    p("LBRACKET"); break;
  case T_RBRACKET:
    p("RBRACKET"); break;
  case T_LBRACE:
    p("LBRACE"); break;
  case T_RBRACE:
    p("RBRACE"); break;
  case T_COMMA:
    p("COMMA"); break;
  case T_DOT:
    p("DOT"); break;
  case T_COLON:
    p("COLON"); break;
  case T_SEMI:
    p("SEMI"); break;
  case T_WS:
    p("WS"); break;

  // newline
  case T_NEWL:
    p("NEWL");
    m_out << endl;
    m_charnum = 1;
    ++m_linenum;
    break;

  // literals
  case T_INT:
    p("INT:", t);
    break;
  case T_FIXED:
    p("FIXED:", t);
    break;
  case T_ID:
    p("ID:", t);
    break;

  default:
    throw LexError("Unknown token '" + std::string(t.value().begin(), t.value().end()) + "'");
  }

  if (t.id() != T_NEWL) {
    m_charnum += t.value().size();
  }
  return true;    // always continue to tokenize
  }

private:
  // Emit a string
  void p(const std::string& s);
  // Emit the string with the underlying token value attached (quoted)
  template <typename Token> inline void p(const std::string& s, const Token& t) {
    p(s);
    m_out << "'" << t.value() << "'";
  }

  std::ostream& m_out;
  linenum_t& m_linenum;
  charnum_t& m_charnum;
};

}

#endif // _Emitter_h_
