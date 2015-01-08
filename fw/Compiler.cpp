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
std::auto_ptr<Rule> fw::CreateCompiler_Simple(Log& log) {
  std::auto_ptr<Rule> compiler(MakeRule_Star(log, "compiler"));
  Rule* newstmt_ = compiler_->AddChild(MakeRule_Seq(log, "new stmt"));
  Rule* delstmt_ = compiler_->AddChild(MakeRule_Seq(log, "del stmt"));
  newstmt_->AddChild(MakeRule_Meta(log, "new"));
  delstmt_->AddChild(MakeRule_Meta(log, "del"));
  return compiler;
}

// compiler =
std::auto_ptr<Rule> fw::CreateCompiler_Moderate(Log& log) {
  std::auto_ptr<Rule> compiler(new Rule(log, "compiler"));
  return compiler;
}
*/

// compiler =
/*
std::auto_ptr<Rule> fw::CreateCompiler_Complex(Log& log) {
  std::auto_ptr<Rule> compiler(new StarRule(log, "compiler"));
  return compiler;
}
*/

std::auto_ptr<Rule> fw::CreateCompiler_Nifty(Log& log) {
  std::auto_ptr<Rule> compiler(MakeRule_Star(log, "compiler"));
  Rule* stmt_ = compiler->AddChild(MakeRule_Seq(log, "stmt"));
  stmt_->AddChild(MakeRule_Meta(log, "stmt"));
  Rule* stmts_ = stmt_->AddChild(MakeRule_Or(log, "stmts"));
  stmt_->AddChild(MakeRule_Meta(log, "/stmt"));

  Rule* cmdstmt_ = stmts_->AddChild(MakeRule_Seq(log, "cmd stmt"));
  Rule* newstmt_ = stmts_->AddChild(MakeRule_Seq(log, "new stmt"));
  Rule* delstmt_ = stmts_->AddChild(MakeRule_Seq(log, "del stmt"));

  Rule* cmd_ = cmdstmt_->AddChild(MakeRule_Star(log, "cmd"));
  cmd_->AddChild(MakeRule_Meta(log, "cmdtext", "cmd"));

  newstmt_->AddChild(MakeRule_Meta(log, "new"));
  newstmt_->AddChild(MakeRule_Meta(log, "identifier"));

  delstmt_->AddChild(MakeRule_Meta(log, "del"));
  delstmt_->AddChild(MakeRule_Meta(log, "identifier"));
  return compiler;
}
