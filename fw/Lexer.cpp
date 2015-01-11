// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Lexer.h"

#include "Keyword.h"
#include "Regexp.h"
#include "Or.h"
#include "Star.h"

#include <boost/regex.hpp>

#include <memory>
using std::auto_ptr;

using namespace fw;

auto_ptr<Rule> fw::CreateLexer_Simple() {
  auto_ptr<Rule> lexer(MakeRule_Star("* (lexer)"));
  Rule* or_ = lexer->AddChild(MakeRule_Or("Or"));
  //or_->SetOutputFunc(MakeOutputFunc_Winner_Cap("orly"));
  or_->AddChild(MakeRule_Keyword("new"));
  or_->AddChild(MakeRule_Keyword("del"));
  //or_->AddChild(MakeRule_Keyword("x"));
  or_->AddChild(MakeRule_Keyword(";"));
  return lexer;
}

auto_ptr<Rule> fw::CreateLexer_Moderate() {
  auto_ptr<Rule> lexer(MakeRule_Star("* (lexer)"));
  Rule* or_ = lexer->AddChild(MakeRule_Or("Or"));
  or_->AddChild(MakeRule_Keyword("new"));
  or_->AddChild(MakeRule_Keyword("del"));
  or_->AddChild(MakeRule_Regexp("ID", boost::regex("[A-Za-z_][0-9A-Za-z_]*")));
  //or_->AddChild(MakeRule_Regexp("INT", boost::regex("[0-9]+")));
  or_->AddChild(MakeRule_Regexp("WS", boost::regex("[ \t\r]+")));
  or_->AddChild(MakeRule_Keyword(";"));
  return lexer;
}

auto_ptr<Rule> fw::CreateLexer_Complex() {
  auto_ptr<Rule> lexer(MakeRule_Star("* (lexer)"));
  Rule* or_ = lexer->AddChild(MakeRule_Or("Or"));
  or_->AddChild(MakeRule_Keyword("exit"));
  or_->AddChild(MakeRule_Keyword("new"));
  or_->AddChild(MakeRule_Keyword("renew"));
  or_->AddChild(MakeRule_Keyword("del"));
  or_->AddChild(MakeRule_Keyword("@"));
  or_->AddChild(MakeRule_Keyword("return"));
  or_->AddChild(MakeRule_Keyword("if"));
  or_->AddChild(MakeRule_Keyword("elif"));
  or_->AddChild(MakeRule_Keyword("else"));
  or_->AddChild(MakeRule_Keyword("while"));
  or_->AddChild(MakeRule_Keyword("loop"));
  or_->AddChild(MakeRule_Keyword("times"));
  or_->AddChild(MakeRule_Keyword("not"));
  or_->AddChild(MakeRule_Keyword("nor"));
  or_->AddChild(MakeRule_Keyword("and"));
  or_->AddChild(MakeRule_Keyword("or"));
  or_->AddChild(MakeRule_Keyword("xor"));
  or_->AddChild(MakeRule_Keyword("<"));
  or_->AddChild(MakeRule_Keyword("<="));
  or_->AddChild(MakeRule_Keyword(">"));
  or_->AddChild(MakeRule_Keyword(">="));
  or_->AddChild(MakeRule_Keyword("=="));
  or_->AddChild(MakeRule_Keyword("!="));
  or_->AddChild(MakeRule_Keyword("+"));
  or_->AddChild(MakeRule_Keyword("-"));
  or_->AddChild(MakeRule_Keyword("*"));
  or_->AddChild(MakeRule_Keyword("/"));
  or_->AddChild(MakeRule_Keyword("%"));
  or_->AddChild(MakeRule_Keyword("^"));
  or_->AddChild(MakeRule_Keyword("|"));
  or_->AddChild(MakeRule_Keyword("&"));
  or_->AddChild(MakeRule_Keyword("~~"));
  or_->AddChild(MakeRule_Keyword("~"));
  or_->AddChild(MakeRule_Keyword("="));
  or_->AddChild(MakeRule_Keyword("+="));
  or_->AddChild(MakeRule_Keyword("-="));
  or_->AddChild(MakeRule_Keyword("*="));
  or_->AddChild(MakeRule_Keyword("/="));
  or_->AddChild(MakeRule_Keyword("%="));
  or_->AddChild(MakeRule_Keyword("^="));
  or_->AddChild(MakeRule_Keyword("|="));
  or_->AddChild(MakeRule_Keyword("&="));
  or_->AddChild(MakeRule_Keyword("~~="));
  or_->AddChild(MakeRule_Keyword("~="));
  or_->AddChild(MakeRule_Keyword("->"));
  or_->AddChild(MakeRule_Keyword("("));
  or_->AddChild(MakeRule_Keyword(")"));
  or_->AddChild(MakeRule_Keyword("["));
  or_->AddChild(MakeRule_Keyword("]"));
  or_->AddChild(MakeRule_Keyword("{"));
  or_->AddChild(MakeRule_Keyword("}"));
  or_->AddChild(MakeRule_Keyword(","));
  or_->AddChild(MakeRule_Keyword("."));
  or_->AddChild(MakeRule_Keyword(":"));
  or_->AddChild(MakeRule_Keyword(";"));
  or_->AddChild(MakeRule_Keyword("\n"));
  or_->AddChild(MakeRule_Regexp("INT", boost::regex("[0-9]+")));
  or_->AddChild(MakeRule_Regexp("STR", boost::regex("(\\\'([^\\\'\\\\\\\\]|\\\\.)*\\\')|(\\\"([^\\\"\\\\\\\\]|\\\\.)*\\\")")));
  or_->AddChild(MakeRule_Regexp("ID", boost::regex("[A-Za-z_][0-9A-Za-z_]*")));
  or_->AddChild(MakeRule_Regexp("WS", boost::regex("[ \t\r]+")));
  or_->AddChild(MakeRule_Keyword(";"));
  return lexer;
}

auto_ptr<Rule> fw::CreateLexer_Nifty() {
  auto_ptr<Rule> lexer(MakeRule_Star("* (lexer)"));
  Rule* or_ = lexer->AddChild(MakeRule_Or("Or"));
  or_->AddChild(MakeRule_Keyword("new"));
  or_->AddChild(MakeRule_Keyword("del"));
  or_->AddChild(MakeRule_Regexp("ID", boost::regex("[A-Za-z_][0-9A-Za-z_]*")));
  or_->AddChild(MakeRule_Regexp("INT", boost::regex("[0-9]+")));
  or_->AddChild(MakeRule_Regexp("WS", boost::regex("[ \t\r]+")));
  or_->AddChild(MakeRule_Keyword(";"));
  return lexer;
}
