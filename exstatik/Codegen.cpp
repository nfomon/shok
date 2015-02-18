// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Codegen.h"

#include "statik/Meta.h"
#include "statik/Or.h"
#include "statik/Seq.h"
#include "statik/Star.h"
using statik::META;
using statik::OR;
using statik::Rule;
using statik::SEQ;
using statik::STAR;

#include <memory>
using std::auto_ptr;

using namespace exstatik;

/*
// compiler =
auto_ptr<Rule> exstatik::CreateCodegen_Simple() {
  auto_ptr<Rule> compiler(MakeRule_Star("codegen"));
  Rule* newstmt_ = compiler_->AddChild(MakeRule_Seq("new stmt"));
  Rule* delstmt_ = compiler_->AddChild(MakeRule_Seq("del stmt"));
  newstmt_->AddChild(MakeRule_Meta("new"));
  delstmt_->AddChild(MakeRule_Meta("del"));
  return compiler;
}

// compiler =
auto_ptr<Rule> exstatik::CreateCodegen_Moderate() {
  auto_ptr<Rule> compiler(new Rule("codegen"));
  return compiler;
}
*/

// compiler =
/*
auto_ptr<Rule> exstatik::CreateCodegen_Complex() {
  auto_ptr<Rule> compiler(new StarRule("codegen"));
  return compiler;
}
*/

auto_ptr<Rule> exstatik::CreateCodegen_Nifty() {
  auto_ptr<Rule> compiler(STAR("codegen"));
  Rule* stmt_ = compiler->AddChild(SEQ("stmt"));
  stmt_->AddChild(META("stmt"));
  Rule* stmts_ = stmt_->AddChild(OR("stmts"));
  stmt_->AddChild(META("/stmt"));

  Rule* cmdstmt_ = stmts_->AddChild(SEQ("cmd stmt"));
  Rule* newstmt_ = stmts_->AddChild(SEQ("new stmt"));
  Rule* delstmt_ = stmts_->AddChild(SEQ("del stmt"));

  Rule* cmd_ = cmdstmt_->AddChild(STAR("cmd"));
  cmd_->AddChild(META("cmdtext", "cmd"));

  newstmt_->AddChild(META("new"));
  newstmt_->AddChild(META("identifier"));

  delstmt_->AddChild(META("del"));
  delstmt_->AddChild(META("identifier"));
  return compiler;
}
