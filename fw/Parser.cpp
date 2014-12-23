// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Parser.h"

#include "Meta.h"
#include "Or.h"
#include "Seq.h"
#include "Star.h"

#include <boost/regex.hpp>

#include <memory>
using std::auto_ptr;

using namespace fw;

// parser = ((new ;)|(del ;))*
std::auto_ptr<Rule> fw::CreateParser_Simple(Log& log) {
  std::auto_ptr<Rule> parser(new StarRule(log, "parser"));
  Rule* stmts_ = parser->CreateChild<OrRule>("stmts");
  Rule* newstmt_ = stmts_->CreateChild<SeqRule>("new stmt");
  newstmt_->CreateChild<MetaRule>("new", "new");
  newstmt_->CreateChild<MetaRule>(";", ";");
  Rule* delstmt_ = stmts_->CreateChild<SeqRule>("del stmt");
  delstmt_->CreateChild<MetaRule>("del", "del");
  delstmt_->CreateChild<MetaRule>(";", ";");
  return parser;
}

// parser = ((new WS ID ;)|(del WS ID ;))*
std::auto_ptr<Rule> fw::CreateParser_Moderate(Log& log) {
  std::auto_ptr<Rule> parser(new StarRule(log, "parser"));
  Rule* stmts_ = parser->CreateChild<OrRule>("stmts");
  Rule* newstmt_ = stmts_->CreateChild<SeqRule>("new stmt");
  newstmt_->CreateChild<MetaRule>("new", "new");
  newstmt_->CreateChild<MetaRule>("WS", "WS");
  newstmt_->CreateChild<MetaRule>("ID", "identifier");
  newstmt_->CreateChild<MetaRule>(";", ";");
  Rule* delstmt_ = stmts_->CreateChild<SeqRule>("del stmt");
  delstmt_->CreateChild<MetaRule>("del", "del");
  delstmt_->CreateChild<MetaRule>("WS", "WS");
  delstmt_->CreateChild<MetaRule>("ID", "identifier");
  delstmt_->CreateChild<MetaRule>(";", ";");
  return parser;
}

// parser = ((cmd|codeblock)end)*
std::auto_ptr<Rule> fw::CreateParser_Complex(Log& log) {
  std::auto_ptr<Rule> parser(new StarRule(log, "parser"));
  Rule* line = parser->CreateChild<SeqRule>("line");
  //line->CreateChild<MetaRule>("WS", "WS");
  Rule* cmdorcode = line->CreateChild<OrRule>("cmdorcode");
  Rule* end = line->CreateChild<OrRule>("end");
  end->CreateChild<MetaRule>(";");
  end->CreateChild<MetaRule>("\n");

  Rule* cmdline = cmdorcode->CreateChild<SeqRule>("cmdline");
  cmdline->CreateChild<MetaRule>("ID", "program");
  Rule* cmdargs = cmdline->CreateChild<StarRule>("cmdargs");
  Rule* cmdarg = cmdargs->CreateChild<SeqRule>("cmdarg");
  cmdarg->CreateChild<MetaRule>("WS", "WS");
  cmdarg->CreateChild<MetaRule>("ID", "arg");

  Rule* codeblock = cmdorcode->CreateChild<SeqRule>("codeblock");
  codeblock->CreateChild<MetaRule>("{");
  Rule* stmts = codeblock->CreateChild<StarRule>("stmts");
  codeblock->CreateChild<MetaRule>("}");

  Rule* stmt = stmts->CreateChild<OrRule>("stmt");
  Rule* newstmt = stmt->CreateChild<SeqRule>("new stmt");
  newstmt->CreateChild<MetaRule>("new", "new");
  newstmt->CreateChild<MetaRule>("WS", "WS");
  newstmt->CreateChild<MetaRule>("ID", "identifier");
  newstmt->CreateChild<MetaRule>(";", ";");
  Rule* delstmt = stmt->CreateChild<SeqRule>("del stmt");
  delstmt->CreateChild<MetaRule>("del", "del");
  delstmt->CreateChild<MetaRule>("WS", "WS");
  delstmt->CreateChild<MetaRule>("ID", "identifier");
  delstmt->CreateChild<MetaRule>(";", ";");
  return parser;
}
