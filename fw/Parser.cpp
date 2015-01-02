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
  std::auto_ptr<Rule> parser(MakeRule_Star(log, "* (parser)"));
  Rule* stmts_ = parser->AddChild(MakeRule_Or(log, "Or (stmts)"));
  stmts_->CapOutput("cmd");
  Rule* newstmt_ = stmts_->AddChild(MakeRule_Seq(log, "new stmt"));
  newstmt_->AddChild(MakeRule_Meta(log, "new"));
  //newstmt_->AddChild(MakeRule_Meta(log, "x"));
  newstmt_->AddChild(MakeRule_Meta(log, ";"));
  Rule* delstmt_ = stmts_->AddChild(MakeRule_Seq(log, "del stmt"));
  delstmt_->AddChild(MakeRule_Meta(log, "del"));
  //delstmt_->AddChild(MakeRule_Meta(log, "x"));
  delstmt_->AddChild(MakeRule_Meta(log, ";"));
  return parser;
}

// parser = ((new WS ID ;)|(del WS ID ;))*
std::auto_ptr<Rule> fw::CreateParser_Moderate(Log& log) {
  std::auto_ptr<Rule> parser(MakeRule_Star(log, "* (parser)"));
  Rule* stmts_ = parser->AddChild(MakeRule_Or(log, "Or (stmts)"));
  Rule* newstmt_ = stmts_->AddChild(MakeRule_Seq(log, "new stmt"));
  newstmt_->AddChild(MakeRule_Meta(log, "new", "new"));
  newstmt_->AddChild(MakeRule_Meta(log, "WS", "WS"));
  newstmt_->AddChild(MakeRule_Meta(log, "identifier", "ID"));
  newstmt_->AddChild(MakeRule_Meta(log, ";", ";"));
  Rule* delstmt_ = stmts_->AddChild(MakeRule_Seq(log, "del stmt"));
  delstmt_->AddChild(MakeRule_Meta(log, "del", "del"));
  delstmt_->AddChild(MakeRule_Meta(log, "WS", "WS"));
  delstmt_->AddChild(MakeRule_Meta(log, "identifier", "ID"));
  delstmt_->AddChild(MakeRule_Meta(log, ";", ";"));
  return parser;
}

// parser = ((cmd|codeblock)end)*
std::auto_ptr<Rule> fw::CreateParser_Complex(Log& log) {
  std::auto_ptr<Rule> parser(MakeRule_Star(log, "* (parser)"));
  Rule* line = parser->AddChild(MakeRule_Seq(log, "line"));
  //line->AddChild(MakeRule_Meta(log, "WS", "WS"));
  Rule* cmdorcode = line->AddChild(MakeRule_Or(log, "Or (cmdorcode)"));
  Rule* end = line->AddChild(MakeRule_Or(log, "end"));
  end->AddChild(MakeRule_Meta(log, ";"));
  end->AddChild(MakeRule_Meta(log, "\n"));

  Rule* cmdline = cmdorcode->AddChild(MakeRule_Seq(log, "cmdline"));
  cmdline->AddChild(MakeRule_Meta(log, "ID", "program"));
  Rule* cmdargs = cmdline->AddChild(MakeRule_Star(log, "* (cmdargs)"));
  Rule* cmdarg = cmdargs->AddChild(MakeRule_Seq(log, "cmdarg"));
  cmdarg->AddChild(MakeRule_Meta(log, "WS", "WS"));
  cmdarg->AddChild(MakeRule_Meta(log, "ID", "arg"));

  Rule* codeblock = cmdorcode->AddChild(MakeRule_Seq(log, "codeblock"));
  codeblock->AddChild(MakeRule_Meta(log, "{"));
  Rule* stmts = codeblock->AddChild(MakeRule_Star(log, "* (stmts)"));
  codeblock->AddChild(MakeRule_Meta(log, "}"));

  Rule* stmt = stmts->AddChild(MakeRule_Or(log, "Or (stmt)"));
  Rule* newstmt = stmt->AddChild(MakeRule_Seq(log, "new stmt"));
  newstmt->AddChild(MakeRule_Meta(log, "new", "new"));
  newstmt->AddChild(MakeRule_Meta(log, "WS", "WS"));
  newstmt->AddChild(MakeRule_Meta(log, "identifier", "ID"));
  newstmt->AddChild(MakeRule_Meta(log, ";", ";"));
  Rule* delstmt = stmt->AddChild(MakeRule_Seq(log, "del stmt"));
  delstmt->AddChild(MakeRule_Meta(log, "del", "del"));
  delstmt->AddChild(MakeRule_Meta(log, "WS", "WS"));
  delstmt->AddChild(MakeRule_Meta(log, "identifier", "ID"));
  delstmt->AddChild(MakeRule_Meta(log, ";", ";"));
  return parser;
}
