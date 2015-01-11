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
auto_ptr<Rule> fw::CreateParser_Simple() {
  auto_ptr<Rule> parser(MakeRule_Star("* (parser)"));
  Rule* stmts_ = parser->AddChild(MakeRule_Or("Or (stmts)"));
  stmts_->CapOutput("cmd");
  Rule* newstmt_ = stmts_->AddChild(MakeRule_Seq("new stmt"));
  newstmt_->AddChild(MakeRule_Meta("new"));
  //newstmt_->AddChild(MakeRule_Meta("x"));
  Rule* semi_1 = newstmt_->AddChild(MakeRule_Meta(";"));
  semi_1->SilenceOutput();
  Rule* delstmt_ = stmts_->AddChild(MakeRule_Seq("del stmt"));
  delstmt_->AddChild(MakeRule_Meta("del"));
  //delstmt_->AddChild(MakeRule_Meta("x"));
  Rule* semi_2 = delstmt_->AddChild(MakeRule_Meta(";"));
  semi_2->SilenceOutput();
  return parser;
}

// parser = ((new WS ID ;)|(del WS ID ;))*
auto_ptr<Rule> fw::CreateParser_Moderate() {
  auto_ptr<Rule> parser(MakeRule_Star("* (parser)"));
  Rule* stmts_ = parser->AddChild(MakeRule_Or("Or (stmts)"));
  stmts_->CapOutput("cmd");
  Rule* newstmt_ = stmts_->AddChild(MakeRule_Seq("new stmt"));
  newstmt_->AddChild(MakeRule_Meta("new"));
  Rule* ws_1 = newstmt_->AddChild(MakeRule_Meta("WS"));
  ws_1->SilenceOutput();
  newstmt_->AddChild(MakeRule_Meta("identifier", "ID"));
  Rule* semi_1 = newstmt_->AddChild(MakeRule_Meta(";"));
  semi_1->SilenceOutput();
  Rule* delstmt_ = stmts_->AddChild(MakeRule_Seq("del stmt"));
  delstmt_->AddChild(MakeRule_Meta("del"));
  Rule* ws_2 = delstmt_->AddChild(MakeRule_Meta("WS"));
  ws_2->SilenceOutput();
  delstmt_->AddChild(MakeRule_Meta("identifier", "ID"));
  Rule* semi_2 = delstmt_->AddChild(MakeRule_Meta(";"));
  semi_2->SilenceOutput();
  return parser;
}

// parser = ((cmd|codeblock)end)*
auto_ptr<Rule> fw::CreateParser_Complex() {
  auto_ptr<Rule> parser(MakeRule_Star("* (parser)"));
  Rule* line = parser->AddChild(MakeRule_Seq("line"));
  //line->AddChild(MakeRule_Meta("WS"));
  Rule* cmdorcode = line->AddChild(MakeRule_Or("Or (cmdorcode)"));
  Rule* end = line->AddChild(MakeRule_Or("end"));
  end->AddChild(MakeRule_Meta(";"));
  end->AddChild(MakeRule_Meta("\n"));

  Rule* cmdline = cmdorcode->AddChild(MakeRule_Seq("cmdline"));
  cmdline->AddChild(MakeRule_Meta("ID", "program"));
  Rule* cmdargs = cmdline->AddChild(MakeRule_Star("* (cmdargs)"));
  Rule* cmdarg = cmdargs->AddChild(MakeRule_Seq("cmdarg"));
  cmdarg->AddChild(MakeRule_Meta("WS"));
  cmdarg->AddChild(MakeRule_Meta("arg"));

  Rule* codeblock = cmdorcode->AddChild(MakeRule_Seq("codeblock"));
  codeblock->AddChild(MakeRule_Meta("{"));
  Rule* stmts = codeblock->AddChild(MakeRule_Star("* (stmts)"));
  codeblock->AddChild(MakeRule_Meta("}"));

  Rule* stmt = stmts->AddChild(MakeRule_Or("Or (stmt)"));
  Rule* newstmt = stmt->AddChild(MakeRule_Seq("new stmt"));
  newstmt->AddChild(MakeRule_Meta("new"));
  newstmt->AddChild(MakeRule_Meta("WS"));
  newstmt->AddChild(MakeRule_Meta("identifier", "ID"));
  newstmt->AddChild(MakeRule_Meta(";"));
  Rule* delstmt = stmt->AddChild(MakeRule_Seq("del stmt"));
  delstmt->AddChild(MakeRule_Meta("del"));
  delstmt->AddChild(MakeRule_Meta("WS"));
  delstmt->AddChild(MakeRule_Meta("identifier", "ID"));
  delstmt->AddChild(MakeRule_Meta(";"));
  return parser;
}

// Nifty parser
auto_ptr<Rule> fw::CreateParser_Nifty() {
  auto_ptr<Rule> parser(MakeRule_Star("* (parser)"));
  Rule* stmts_ = parser->AddChild(MakeRule_Or("Or (stmts)"));
  stmts_->CapOutput("stmt");

  Rule* cmd_ = stmts_->AddChild(MakeRule_Seq("cmd stmt"));
  cmd_->AddChild(MakeRule_Meta("cmd", "ID"));

  Rule* cmdexts_ = cmd_->AddChild(MakeRule_Star("cmdexts"));
  Rule* cmdext_ = cmdexts_->AddChild(MakeRule_Seq("cmdext"));
  cmdext_->AddChild(MakeRule_Meta("cmd", "WS"));
  cmdext_->AddChild(MakeRule_Meta("cmd", "ID"));

  Rule* newstmt_ = stmts_->AddChild(MakeRule_Seq("new stmt"));
  newstmt_->AddChild(MakeRule_Meta("new"));
  Rule* ws_1 = newstmt_->AddChild(MakeRule_Meta("WS"));
  ws_1->SilenceOutput();
  Rule* newor_ = newstmt_->AddChild(MakeRule_Star("new*"));
  newor_->AddChildRecursive(newstmt_);
  newstmt_->AddChild(MakeRule_Meta("identifier", "ID"));
  Rule* semi_1 = newstmt_->AddChild(MakeRule_Meta(";"));
  semi_1->SilenceOutput();

  Rule* delstmt_ = stmts_->AddChild(MakeRule_Seq("del stmt"));
  delstmt_->AddChild(MakeRule_Meta("del"));
  Rule* ws_2 = delstmt_->AddChild(MakeRule_Meta("WS"));
  ws_2->SilenceOutput();
  delstmt_->AddChild(MakeRule_Meta("identifier", "ID"));
  Rule* semi_2 = delstmt_->AddChild(MakeRule_Meta(";"));
  semi_2->SilenceOutput();

  return parser;
}
