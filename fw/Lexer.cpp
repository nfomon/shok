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

auto_ptr<Rule> fw::CreateLexer_Simple(Log& log) {
  auto_ptr<Rule> lexer(MakeRule_Star(log, "* (lexer)"));
  Rule* or_ = lexer->AddChild(MakeRule_Or(log, "Or"));
  //or_->SetOutputFunc(MakeOutputFunc_Winner_Cap(log, "orly"));
  or_->AddChild(MakeRule_Keyword(log, "new"));
  or_->AddChild(MakeRule_Keyword(log, "del"));
  //or_->AddChild(MakeRule_Keyword(log, "x"));
  or_->AddChild(MakeRule_Keyword(log, ";"));
  return lexer;
}

auto_ptr<Rule> fw::CreateLexer_Moderate(Log& log) {
  auto_ptr<Rule> lexer(MakeRule_Star(log, "* (lexer)"));
  Rule* or_ = lexer->AddChild(MakeRule_Or(log, "Or"));
  or_->AddChild(MakeRule_Keyword(log, "new"));
  or_->AddChild(MakeRule_Keyword(log, "del"));
  or_->AddChild(MakeRule_Regexp(log, "ID", boost::regex("[A-Za-z_][0-9A-Za-z_]*")));
  //or_->AddChild(MakeRule_Regexp(log, "INT", boost::regex("[0-9]+")));
  or_->AddChild(MakeRule_Regexp(log, "WS", boost::regex("[ \t\r]+")));
  or_->AddChild(MakeRule_Keyword(log, ";"));
  return lexer;
}

auto_ptr<Rule> fw::CreateLexer_Complex(Log& log) {
  auto_ptr<Rule> lexer(MakeRule_Star(log, "* (lexer)"));
  Rule* or_ = lexer->AddChild(MakeRule_Or(log, "Or"));
  or_->AddChild(MakeRule_Keyword(log, "exit"));
  or_->AddChild(MakeRule_Keyword(log, "new"));
  or_->AddChild(MakeRule_Keyword(log, "renew"));
  or_->AddChild(MakeRule_Keyword(log, "del"));
  or_->AddChild(MakeRule_Keyword(log, "@"));
  or_->AddChild(MakeRule_Keyword(log, "return"));
  or_->AddChild(MakeRule_Keyword(log, "if"));
  or_->AddChild(MakeRule_Keyword(log, "elif"));
  or_->AddChild(MakeRule_Keyword(log, "else"));
  or_->AddChild(MakeRule_Keyword(log, "while"));
  or_->AddChild(MakeRule_Keyword(log, "loop"));
  or_->AddChild(MakeRule_Keyword(log, "times"));
  or_->AddChild(MakeRule_Keyword(log, "not"));
  or_->AddChild(MakeRule_Keyword(log, "nor"));
  or_->AddChild(MakeRule_Keyword(log, "and"));
  or_->AddChild(MakeRule_Keyword(log, "or"));
  or_->AddChild(MakeRule_Keyword(log, "xor"));
  or_->AddChild(MakeRule_Keyword(log, "<"));
  or_->AddChild(MakeRule_Keyword(log, "<="));
  or_->AddChild(MakeRule_Keyword(log, ">"));
  or_->AddChild(MakeRule_Keyword(log, ">="));
  or_->AddChild(MakeRule_Keyword(log, "=="));
  or_->AddChild(MakeRule_Keyword(log, "!="));
  or_->AddChild(MakeRule_Keyword(log, "+"));
  or_->AddChild(MakeRule_Keyword(log, "-"));
  or_->AddChild(MakeRule_Keyword(log, "*"));
  or_->AddChild(MakeRule_Keyword(log, "/"));
  or_->AddChild(MakeRule_Keyword(log, "%"));
  or_->AddChild(MakeRule_Keyword(log, "^"));
  or_->AddChild(MakeRule_Keyword(log, "|"));
  or_->AddChild(MakeRule_Keyword(log, "&"));
  or_->AddChild(MakeRule_Keyword(log, "~~"));
  or_->AddChild(MakeRule_Keyword(log, "~"));
  or_->AddChild(MakeRule_Keyword(log, "="));
  or_->AddChild(MakeRule_Keyword(log, "+="));
  or_->AddChild(MakeRule_Keyword(log, "-="));
  or_->AddChild(MakeRule_Keyword(log, "*="));
  or_->AddChild(MakeRule_Keyword(log, "/="));
  or_->AddChild(MakeRule_Keyword(log, "%="));
  or_->AddChild(MakeRule_Keyword(log, "^="));
  or_->AddChild(MakeRule_Keyword(log, "|="));
  or_->AddChild(MakeRule_Keyword(log, "&="));
  or_->AddChild(MakeRule_Keyword(log, "~~="));
  or_->AddChild(MakeRule_Keyword(log, "~="));
  or_->AddChild(MakeRule_Keyword(log, "->"));
  or_->AddChild(MakeRule_Keyword(log, "("));
  or_->AddChild(MakeRule_Keyword(log, ")"));
  or_->AddChild(MakeRule_Keyword(log, "["));
  or_->AddChild(MakeRule_Keyword(log, "]"));
  or_->AddChild(MakeRule_Keyword(log, "{"));
  or_->AddChild(MakeRule_Keyword(log, "}"));
  or_->AddChild(MakeRule_Keyword(log, ","));
  or_->AddChild(MakeRule_Keyword(log, "."));
  or_->AddChild(MakeRule_Keyword(log, ":"));
  or_->AddChild(MakeRule_Keyword(log, ";"));
  or_->AddChild(MakeRule_Keyword(log, "\n"));
  or_->AddChild(MakeRule_Regexp(log, "INT", boost::regex("[0-9]+")));
  or_->AddChild(MakeRule_Regexp(log, "STR", boost::regex("(\\\'([^\\\'\\\\\\\\]|\\\\.)*\\\')|(\\\"([^\\\"\\\\\\\\]|\\\\.)*\\\")")));
  or_->AddChild(MakeRule_Regexp(log, "ID", boost::regex("[A-Za-z_][0-9A-Za-z_]*")));
  or_->AddChild(MakeRule_Regexp(log, "WS", boost::regex("[ \t\r]+")));
  or_->AddChild(MakeRule_Keyword(log, ";"));
  return lexer;
}
