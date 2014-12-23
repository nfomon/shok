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
  auto_ptr<Rule> lexer(new StarRule(log, "lexer"));
  Rule* or_ = lexer->CreateChild<OrRule>("or");
  or_->CreateChild<KeywordRule>("new");
  or_->CreateChild<KeywordRule>("del");
  or_->CreateChild<KeywordRule>(";");
  return lexer;
}

auto_ptr<Rule> fw::CreateLexer_Moderate(Log& log) {
  auto_ptr<Rule> lexer(new StarRule(log, "lexer"));
  Rule* or_ = lexer->CreateChild<OrRule>("or");
  or_->CreateChild<KeywordRule>("new");
  or_->CreateChild<KeywordRule>("del");
  or_->CreateChild<RegexpRule>("ID", boost::regex("[A-Za-z_][0-9A-Za-z_]*"));
  or_->CreateChild<RegexpRule>("INT", boost::regex("[0-9]+"));
  or_->CreateChild<RegexpRule>("WS", boost::regex("[ \t\r]+"));
  or_->CreateChild<KeywordRule>(";");
  return lexer;
}

auto_ptr<Rule> fw::CreateLexer_Complex(Log& log) {
  auto_ptr<Rule> lexer(new StarRule(log, "lexer"));
  Rule* or_ = lexer->CreateChild<OrRule>("or");
  or_->CreateChild<KeywordRule>("exit");
  or_->CreateChild<KeywordRule>("new");
  or_->CreateChild<KeywordRule>("renew");
  or_->CreateChild<KeywordRule>("del");
  or_->CreateChild<KeywordRule>("@");
  or_->CreateChild<KeywordRule>("return");
  or_->CreateChild<KeywordRule>("if");
  or_->CreateChild<KeywordRule>("elif");
  or_->CreateChild<KeywordRule>("else");
  or_->CreateChild<KeywordRule>("while");
  or_->CreateChild<KeywordRule>("loop");
  or_->CreateChild<KeywordRule>("times");
  or_->CreateChild<KeywordRule>("not");
  or_->CreateChild<KeywordRule>("nor");
  or_->CreateChild<KeywordRule>("and");
  or_->CreateChild<KeywordRule>("or");
  or_->CreateChild<KeywordRule>("xor");
  or_->CreateChild<KeywordRule>("<");
  or_->CreateChild<KeywordRule>("<=");
  or_->CreateChild<KeywordRule>(">");
  or_->CreateChild<KeywordRule>(">=");
  or_->CreateChild<KeywordRule>("==");
  or_->CreateChild<KeywordRule>("!=");
  or_->CreateChild<KeywordRule>("+");
  or_->CreateChild<KeywordRule>("-");
  or_->CreateChild<KeywordRule>("*");
  or_->CreateChild<KeywordRule>("/");
  or_->CreateChild<KeywordRule>("%");
  or_->CreateChild<KeywordRule>("^");
  or_->CreateChild<KeywordRule>("|");
  or_->CreateChild<KeywordRule>("&");
  or_->CreateChild<KeywordRule>("~~");
  or_->CreateChild<KeywordRule>("~");
  or_->CreateChild<KeywordRule>("=");
  or_->CreateChild<KeywordRule>("+=");
  or_->CreateChild<KeywordRule>("-=");
  or_->CreateChild<KeywordRule>("*=");
  or_->CreateChild<KeywordRule>("/=");
  or_->CreateChild<KeywordRule>("%=");
  or_->CreateChild<KeywordRule>("^=");
  or_->CreateChild<KeywordRule>("|=");
  or_->CreateChild<KeywordRule>("&=");
  or_->CreateChild<KeywordRule>("~~=");
  or_->CreateChild<KeywordRule>("~=");
  or_->CreateChild<KeywordRule>("->");
  or_->CreateChild<KeywordRule>("(");
  or_->CreateChild<KeywordRule>(")");
  or_->CreateChild<KeywordRule>("[");
  or_->CreateChild<KeywordRule>("]");
  or_->CreateChild<KeywordRule>("{");
  or_->CreateChild<KeywordRule>("}");
  or_->CreateChild<KeywordRule>(",");
  or_->CreateChild<KeywordRule>(".");
  or_->CreateChild<KeywordRule>(":");
  or_->CreateChild<KeywordRule>(";");
  or_->CreateChild<KeywordRule>("\n");
  or_->CreateChild<RegexpRule>("INT", boost::regex("[0-9]+"));
  or_->CreateChild<RegexpRule>("STR", boost::regex("(\\\'([^\\\'\\\\\\\\]|\\\\.)*\\\')|(\\\"([^\\\"\\\\\\\\]|\\\\.)*\\\")"));
  or_->CreateChild<RegexpRule>("ID", boost::regex("[A-Za-z_][0-9A-Za-z_]*"));
  or_->CreateChild<RegexpRule>("WS", boost::regex("[ \t\r]+"));
  or_->CreateChild<KeywordRule>(";");
  return lexer;
}
