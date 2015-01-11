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
  std::auto_ptr<Rule> compiler(MakeRule_Star("compiler"));
  Rule* stmt_ = compiler->AddChild(MakeRule_Seq("stmt"));
  stmt_->AddChild(MakeRule_Meta("stmt"));
  Rule* stmts_ = stmt_->AddChild(MakeRule_Or("stmts"));
  stmt_->AddChild(MakeRule_Meta("/stmt"));

  Rule* cmdstmt_ = stmts_->AddChild(MakeRule_Seq("cmd stmt"));
  Rule* newstmt_ = stmts_->AddChild(MakeRule_Seq("new stmt"));
  Rule* delstmt_ = stmts_->AddChild(MakeRule_Seq("del stmt"));

  Rule* cmd_ = cmdstmt_->AddChild(MakeRule_Star("cmd"));
  cmd_->AddChild(MakeRule_Meta("cmdtext", "cmd"));

  newstmt_->AddChild(MakeRule_Meta("new"));
  newstmt_->AddChild(MakeRule_Meta("identifier"));

  delstmt_->AddChild(MakeRule_Meta("del"));
  delstmt_->AddChild(MakeRule_Meta("identifier"));
  return compiler;
}
