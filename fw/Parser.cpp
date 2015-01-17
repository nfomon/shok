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
  auto_ptr<Rule> parser(STAR("* (parser)"));
  Rule* stmts_ = parser->AddChild(OR("Or (stmts)"))
    ->CapOutput("cmd");
  Rule* newstmt_ = stmts_->AddChild(SEQ("new stmt"));
  newstmt_->AddChild(META("new"));
  //newstmt_->AddChild(META("x"));
  newstmt_->AddChild(META(";"))
    ->SilenceOutput();
  Rule* delstmt_ = stmts_->AddChild(SEQ("del stmt"));
  delstmt_->AddChild(META("del"));
  //delstmt_->AddChild(META("x"));
  delstmt_->AddChild(META(";"))
    ->SilenceOutput();
  return parser;
}

// parser = ((new WS ID ;)|(del WS ID ;))*
auto_ptr<Rule> fw::CreateParser_Moderate() {
  auto_ptr<Rule> parser(STAR("* (parser)"));
  Rule* stmts_ = parser->AddChild(OR("Or (stmts)"))
    ->CapOutput("cmd");
  Rule* newstmt_ = stmts_->AddChild(SEQ("new stmt"));
  newstmt_->AddChild(META("new"));
  newstmt_->AddChild(META("WS"))
    ->SilenceOutput();
  newstmt_->AddChild(META("identifier", "ID"));
  newstmt_->AddChild(META(";"))
    ->SilenceOutput();
  Rule* delstmt_ = stmts_->AddChild(SEQ("del stmt"));
  delstmt_->AddChild(META("del"));
  delstmt_->AddChild(META("WS"))
    ->SilenceOutput();
  delstmt_->AddChild(META("identifier", "ID"));
  delstmt_->AddChild(META(";"))
    ->SilenceOutput();
  return parser;
}

// parser = ((cmd|codeblock)end)*
auto_ptr<Rule> fw::CreateParser_Complex() {
  auto_ptr<Rule> parser(STAR("* (parser)"));
  Rule* line = parser->AddChild(SEQ("line"));
  //line->AddChild(META("WS"));
  Rule* cmdorcode = line->AddChild(OR("Or (cmdorcode)"));
  Rule* end = line->AddChild(OR("end"));
  end->AddChild(META(";"));
  end->AddChild(META("\n"));

  Rule* cmdline = cmdorcode->AddChild(SEQ("cmdline"));
  cmdline->AddChild(META("ID", "program"));
  Rule* cmdargs = cmdline->AddChild(STAR("* (cmdargs)"));
  Rule* cmdarg = cmdargs->AddChild(SEQ("cmdarg"));
  cmdarg->AddChild(META("WS"));
  cmdarg->AddChild(META("arg"));

  Rule* codeblock = cmdorcode->AddChild(SEQ("codeblock"));
  codeblock->AddChild(META("{"));
  Rule* stmts = codeblock->AddChild(STAR("* (stmts)"));
  codeblock->AddChild(META("}"));

  Rule* stmt = stmts->AddChild(OR("Or (stmt)"));
  Rule* newstmt = stmt->AddChild(SEQ("new stmt"));
  newstmt->AddChild(META("new"));
  newstmt->AddChild(META("WS"));
  newstmt->AddChild(META("identifier", "ID"));
  newstmt->AddChild(META(";"));
  Rule* delstmt = stmt->AddChild(SEQ("del stmt"));
  delstmt->AddChild(META("del"));
  delstmt->AddChild(META("WS"));
  delstmt->AddChild(META("identifier", "ID"));
  delstmt->AddChild(META(";"));
  return parser;
}

// Nifty parser
auto_ptr<Rule> fw::CreateParser_Nifty() {
  auto_ptr<Rule> parser(STAR("* (parser)"));
  Rule* stmts_ = parser->AddChild(OR("Or (stmts)"))
    ->CapOutput("stmt");

  Rule* cmd_ = stmts_->AddChild(SEQ("cmd stmt"));
  cmd_->AddChild(META("cmd", "ID"));

  Rule* cmdexts_ = cmd_->AddChild(STAR("cmdexts"));
  Rule* cmdext_ = cmdexts_->AddChild(SEQ("cmdext"));
  cmdext_->AddChild(META("cmd", "WS"));
  cmdext_->AddChild(META("cmd", "ID"));

  Rule* newstmt_ = stmts_->AddChild(SEQ("new stmt"));
  newstmt_->AddChild(META("new"));
  newstmt_->AddChild(META("WS"))
    ->SilenceOutput();
  Rule* newor_ = newstmt_->AddChild(STAR("new*"));
  newor_->AddChildRecursive(newstmt_);
  newstmt_->AddChild(META("identifier", "ID"));
  newstmt_->AddChild(META(";"))
    ->SilenceOutput();

  Rule* delstmt_ = stmts_->AddChild(SEQ("del stmt"));
  delstmt_->AddChild(META("del"));
  delstmt_->AddChild(META("WS"))
    ->SilenceOutput();
  delstmt_->AddChild(META("identifier", "ID"));
  delstmt_->AddChild(META(";"))
    ->SilenceOutput();

  return parser;
}
