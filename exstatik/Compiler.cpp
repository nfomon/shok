// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Compiler.h"

#include "Codegen.h"
#include "Lexer.h"
#include "Parser.h"

#include "statik/SError.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace exstatik;

auto_ptr<Compiler> exstatik::MakeCompiler(const string& name) {
  auto_ptr<Compiler> c(new Compiler());
  if ("Simple" == name) {
    c->push_back(CreateLexer_Simple());
    c->push_back(CreateParser_Simple());
  } else if ("Seq" == name) {
    c->push_back(CreateLexer_Seq());
  } else if ("KeywordTest" == name) {
    c->push_back(CreateLexer_KeywordTest());
  } else if ("Splash" == name) {
    c->push_back(CreateLexer_Splash());
  } else if ("Moderate" == name) {
    c->push_back(CreateLexer_Moderate());
    //c->push_back(CreateParser_Moderate());
  } else if ("Complex" == name) {
    c->push_back(CreateLexer_Complex());
    //c->push_back(CreateParser_Complex());
  } else if ("Nifty" == name) {
    c->push_back(CreateLexer_Nifty());
    //c->push_back(CreateParser_Nifty());
    //c->push_back(CreateCodegen_Nifty());
  } else if ("C" == name) {
    c->push_back(CreateLexer_C());
  } else {
    throw statik::SError("Unknown compiler " + name);
  }
  return c;
}
