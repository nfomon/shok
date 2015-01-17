// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Compiler.h"

#include "Meta.h"
#include "Or.h"
#include "Seq.h"
#include "Star.h"

#include <boost/regex.hpp>

#include <memory>
using std::auto_ptr;

using namespace fw;

/*
// compiler =
std::auto_ptr<Rule> fw::CreateCompiler_Simple() {
  std::auto_ptr<Rule> compiler(MakeRule_Star("compiler"));
  Rule* newstmt_ = compiler_->AddChild(MakeRule_Seq("new stmt"));
  Rule* delstmt_ = compiler_->AddChild(MakeRule_Seq("del stmt"));
  newstmt_->AddChild(MakeRule_Meta("new"));
  delstmt_->AddChild(MakeRule_Meta("del"));
  return compiler;
}

// compiler =
std::auto_ptr<Rule> fw::CreateCompiler_Moderate() {
  std::auto_ptr<Rule> compiler(new Rule("compiler"));
  return compiler;
}
*/

// compiler =
/*
std::auto_ptr<Rule> fw::CreateCompiler_Complex() {
  std::auto_ptr<Rule> compiler(new StarRule("compiler"));
  return compiler;
}
*/

std::auto_ptr<Rule> fw::CreateCompiler_Nifty() {
  std::auto_ptr<Rule> compiler(STAR("compiler"));
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
