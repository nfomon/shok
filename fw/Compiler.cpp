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

// compiler =
std::auto_ptr<Rule> fw::CreateCompiler_Simple(Log& log) {
  std::auto_ptr<Rule> compiler(new StarRule(log, "compiler"));
  return compiler;
}

// compiler =
std::auto_ptr<Rule> fw::CreateCompiler_Moderate(Log& log) {
  std::auto_ptr<Rule> compiler(new StarRule(log, "compiler"));
  return compiler;
}

// compiler =
std::auto_ptr<Rule> fw::CreateCompiler_Complex(Log& log) {
  std::auto_ptr<Rule> compiler(new StarRule(log, "compiler"));
  return compiler;
}
