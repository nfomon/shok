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
  Rule* semi_1 = newstmt_->AddChild(MakeRule_Meta(log, ";"));
  semi_1->SilenceOutput();
  Rule* delstmt_ = stmts_->AddChild(MakeRule_Seq(log, "del stmt"));
  delstmt_->AddChild(MakeRule_Meta(log, "del"));
  //delstmt_->AddChild(MakeRule_Meta(log, "x"));
  Rule* semi_2 = delstmt_->AddChild(MakeRule_Meta(log, ";"));
  semi_2->SilenceOutput();
  return parser;
}

// parser = ((new WS ID ;)|(del WS ID ;))*
std::auto_ptr<Rule> fw::CreateParser_Moderate(Log& log) {
  std::auto_ptr<Rule> parser(MakeRule_Star(log, "* (parser)"));
  Rule* stmts_ = parser->AddChild(MakeRule_Or(log, "Or (stmts)"));
  stmts_->CapOutput("cmd");
  Rule* newstmt_ = stmts_->AddChild(MakeRule_Seq(log, "new stmt"));
  newstmt_->AddChild(MakeRule_Meta(log, "new"));
  Rule* ws_1 = newstmt_->AddChild(MakeRule_Meta(log, "WS"));
  ws_1->SilenceOutput();
  newstmt_->AddChild(MakeRule_Meta(log, "identifier", "ID"));
  Rule* semi_1 = newstmt_->AddChild(MakeRule_Meta(log, ";"));
  semi_1->SilenceOutput();
  Rule* delstmt_ = stmts_->AddChild(MakeRule_Seq(log, "del stmt"));
  delstmt_->AddChild(MakeRule_Meta(log, "del"));
  Rule* ws_2 = delstmt_->AddChild(MakeRule_Meta(log, "WS"));
  ws_2->SilenceOutput();
  delstmt_->AddChild(MakeRule_Meta(log, "identifier", "ID"));
  Rule* semi_2 = delstmt_->AddChild(MakeRule_Meta(log, ";"));
  semi_2->SilenceOutput();
  return parser;
}

// parser = ((cmd|codeblock)end)*
std::auto_ptr<Rule> fw::CreateParser_Complex(Log& log) {
  std::auto_ptr<Rule> parser(MakeRule_Star(log, "* (parser)"));
  Rule* line = parser->AddChild(MakeRule_Seq(log, "line"));
  //line->AddChild(MakeRule_Meta(log, "WS"));
  Rule* cmdorcode = line->AddChild(MakeRule_Or(log, "Or (cmdorcode)"));
  Rule* end = line->AddChild(MakeRule_Or(log, "end"));
  end->AddChild(MakeRule_Meta(log, ";"));
  end->AddChild(MakeRule_Meta(log, "\n"));

  Rule* cmdline = cmdorcode->AddChild(MakeRule_Seq(log, "cmdline"));
  cmdline->AddChild(MakeRule_Meta(log, "ID", "program"));
  Rule* cmdargs = cmdline->AddChild(MakeRule_Star(log, "* (cmdargs)"));
  Rule* cmdarg = cmdargs->AddChild(MakeRule_Seq(log, "cmdarg"));
  cmdarg->AddChild(MakeRule_Meta(log, "WS"));
  cmdarg->AddChild(MakeRule_Meta(log, "arg"));

  Rule* codeblock = cmdorcode->AddChild(MakeRule_Seq(log, "codeblock"));
  codeblock->AddChild(MakeRule_Meta(log, "{"));
  Rule* stmts = codeblock->AddChild(MakeRule_Star(log, "* (stmts)"));
  codeblock->AddChild(MakeRule_Meta(log, "}"));

  Rule* stmt = stmts->AddChild(MakeRule_Or(log, "Or (stmt)"));
  Rule* newstmt = stmt->AddChild(MakeRule_Seq(log, "new stmt"));
  newstmt->AddChild(MakeRule_Meta(log, "new"));
  newstmt->AddChild(MakeRule_Meta(log, "WS"));
  newstmt->AddChild(MakeRule_Meta(log, "identifier", "ID"));
  newstmt->AddChild(MakeRule_Meta(log, ";"));
  Rule* delstmt = stmt->AddChild(MakeRule_Seq(log, "del stmt"));
  delstmt->AddChild(MakeRule_Meta(log, "del"));
  delstmt->AddChild(MakeRule_Meta(log, "WS"));
  delstmt->AddChild(MakeRule_Meta(log, "identifier", "ID"));
  delstmt->AddChild(MakeRule_Meta(log, ";"));
  return parser;
}

// Nifty parser
std::auto_ptr<Rule> fw::CreateParser_Nifty(Log& log) {
  std::auto_ptr<Rule> parser(MakeRule_Star(log, "* (parser)"));
  Rule* stmts_ = parser->AddChild(MakeRule_Or(log, "Or (stmts)"));
  stmts_->CapOutput("stmt");

  Rule* cmd_ = stmts_->AddChild(MakeRule_Seq(log, "cmd stmt"));
  cmd_->AddChild(MakeRule_Meta(log, "cmd", "ID"));

  Rule* cmdexts_ = cmd_->AddChild(MakeRule_Star(log, "cmdexts"));
  Rule* cmdext_ = cmdexts_->AddChild(MakeRule_Seq(log, "cmdext"));
  cmdext_->AddChild(MakeRule_Meta(log, "cmd", "WS"));
  cmdext_->AddChild(MakeRule_Meta(log, "cmd", "ID"));

  Rule* newstmt_ = stmts_->AddChild(MakeRule_Seq(log, "new stmt"));
  newstmt_->AddChild(MakeRule_Meta(log, "new"));
  Rule* ws_1 = newstmt_->AddChild(MakeRule_Meta(log, "WS"));
  ws_1->SilenceOutput();
  Rule* newor_ = newstmt_->AddChild(MakeRule_Star(log, "new*"));
  newor_->AddChildRecursive(newstmt_);
  newstmt_->AddChild(MakeRule_Meta(log, "identifier", "ID"));
  Rule* semi_1 = newstmt_->AddChild(MakeRule_Meta(log, ";"));
  semi_1->SilenceOutput();

  Rule* delstmt_ = stmts_->AddChild(MakeRule_Seq(log, "del stmt"));
  delstmt_->AddChild(MakeRule_Meta(log, "del"));
  Rule* ws_2 = delstmt_->AddChild(MakeRule_Meta(log, "WS"));
  ws_2->SilenceOutput();
  delstmt_->AddChild(MakeRule_Meta(log, "identifier", "ID"));
  Rule* semi_2 = delstmt_->AddChild(MakeRule_Meta(log, ";"));
  semi_2->SilenceOutput();

  return parser;
}
