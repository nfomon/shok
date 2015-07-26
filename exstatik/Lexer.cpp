// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Lexer.h"

#include "statik/Keyword.h"
#include "statik/Regexp.h"
#include "statik/Or.h"
#include "statik/Star.h"
using statik::KEYWORD;
using statik::REGEXP;
using statik::Rule;
using statik::OR;
using statik::STAR;

#include <boost/regex.hpp>

#include <memory>
using std::auto_ptr;

using namespace exstatik;

auto_ptr<Rule> exstatik::CreateLexer_Simple() {
  auto_ptr<Rule> lexer(STAR("star"));
  Rule* or_ = lexer->AddChild(OR("or"));
  or_->AddChild(KEYWORD("new"));
  or_->AddChild(KEYWORD("del"));
  or_->AddChild(KEYWORD(";"));
  return lexer;
}

auto_ptr<Rule> exstatik::CreateLexer_Splash() {
  auto_ptr<Rule> lexer(STAR("star"));
  Rule* or_ = lexer->AddChild(OR("or"));
  or_->AddChild(KEYWORD("cat"));
  or_->AddChild(KEYWORD("dog"));
  or_->AddChild(KEYWORD("car"));
  return lexer;
}

auto_ptr<Rule> exstatik::CreateLexer_Moderate() {
  auto_ptr<Rule> lexer(STAR("lexer"));
  Rule* or_ = lexer->AddChild(OR("Or"));
  or_->AddChild(KEYWORD("new"));
  or_->AddChild(KEYWORD("del"));
  or_->AddChild(REGEXP("ID", boost::regex("[A-Za-z_][0-9A-Za-z_]*")));
  //or_->AddChild(REGEXP("INT", boost::regex("[0-9]+")));
  or_->AddChild(REGEXP("WS", boost::regex("[ \t\r]+")));
  or_->AddChild(KEYWORD(";"));
  return lexer;
}

auto_ptr<Rule> exstatik::CreateLexer_Complex() {
  auto_ptr<Rule> lexer(STAR("lexer"));
  Rule* or_ = lexer->AddChild(OR("Or"));
  or_->AddChild(KEYWORD("exit"));
  or_->AddChild(KEYWORD("new"));
  or_->AddChild(KEYWORD("renew"));
  or_->AddChild(KEYWORD("del"));
  or_->AddChild(KEYWORD("@"));
  or_->AddChild(KEYWORD("return"));
  or_->AddChild(KEYWORD("if"));
  or_->AddChild(KEYWORD("elif"));
  or_->AddChild(KEYWORD("else"));
  or_->AddChild(KEYWORD("while"));
  or_->AddChild(KEYWORD("loop"));
  or_->AddChild(KEYWORD("times"));
  or_->AddChild(KEYWORD("not"));
  or_->AddChild(KEYWORD("nor"));
  or_->AddChild(KEYWORD("and"));
  or_->AddChild(KEYWORD("or"));
  or_->AddChild(KEYWORD("xor"));
  or_->AddChild(KEYWORD("<"));
  or_->AddChild(KEYWORD("<="));
  or_->AddChild(KEYWORD(">"));
  or_->AddChild(KEYWORD(">="));
  or_->AddChild(KEYWORD("=="));
  or_->AddChild(KEYWORD("!="));
  or_->AddChild(KEYWORD("+"));
  or_->AddChild(KEYWORD("-"));
  or_->AddChild(KEYWORD("*"));
  or_->AddChild(KEYWORD("/"));
  or_->AddChild(KEYWORD("%"));
  or_->AddChild(KEYWORD("^"));
  or_->AddChild(KEYWORD("|"));
  or_->AddChild(KEYWORD("&"));
  or_->AddChild(KEYWORD("~~"));
  or_->AddChild(KEYWORD("~"));
  or_->AddChild(KEYWORD("="));
  or_->AddChild(KEYWORD("+="));
  or_->AddChild(KEYWORD("-="));
  or_->AddChild(KEYWORD("*="));
  or_->AddChild(KEYWORD("/="));
  or_->AddChild(KEYWORD("%="));
  or_->AddChild(KEYWORD("^="));
  or_->AddChild(KEYWORD("|="));
  or_->AddChild(KEYWORD("&="));
  or_->AddChild(KEYWORD("~~="));
  or_->AddChild(KEYWORD("~="));
  or_->AddChild(KEYWORD("->"));
  or_->AddChild(KEYWORD("("));
  or_->AddChild(KEYWORD(")"));
  or_->AddChild(KEYWORD("["));
  or_->AddChild(KEYWORD("]"));
  or_->AddChild(KEYWORD("{"));
  or_->AddChild(KEYWORD("}"));
  or_->AddChild(KEYWORD(","));
  or_->AddChild(KEYWORD("."));
  or_->AddChild(KEYWORD(":"));
  or_->AddChild(KEYWORD(";"));
  or_->AddChild(KEYWORD("\n"));
  or_->AddChild(REGEXP("INT", boost::regex("[0-9]+")));
  or_->AddChild(REGEXP("STR", boost::regex("(\\\'([^\\\'\\\\\\\\]|\\\\.)*\\\')|(\\\"([^\\\"\\\\\\\\]|\\\\.)*\\\")")));
  or_->AddChild(REGEXP("ID", boost::regex("[A-Za-z_][0-9A-Za-z_]*")));
  or_->AddChild(REGEXP("WS", boost::regex("[ \t\r]+")));
  return lexer;
}

auto_ptr<Rule> exstatik::CreateLexer_Nifty() {
  auto_ptr<Rule> lexer(STAR("lexer"));
  Rule* or_ = lexer->AddChild(OR("or"));
  or_->AddChild(KEYWORD("new"));
  or_->AddChild(KEYWORD("del"));
  or_->AddChild(REGEXP("ID", boost::regex("[A-Za-z_][0-9A-Za-z_]*")));
  or_->AddChild(REGEXP("INT", boost::regex("[0-9]+")));
  or_->AddChild(REGEXP("WS", boost::regex("[ \t\r]+")));
  or_->AddChild(KEYWORD(";"));
  return lexer;
}
