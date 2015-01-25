// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Compiler.h"

#include "Codegen.h"
#include "Lexer.h"
#include "Parser.h"

#include "statik/SError.h"

#include <string>
using std::string;

using namespace exstatik;

Compiler exstatik::MakeCompiler(const string& name) {
  if ("Simple" == name) {
    Compiler c;
    c.push_back(CreateLexer_Simple());
    c.push_back(CreateParser_Simple());
    return c;
  } else if ("Moderate" == name) {
    Compiler c;
    c.push_back(CreateLexer_Moderate());
    c.push_back(CreateParser_Moderate());
    return c;
  } else if ("Complex" == name) {
    Compiler c;
    c.push_back(CreateLexer_Complex());
    c.push_back(CreateParser_Complex());
    return c;
  } else if ("Nifty" == name) {
    Compiler c;
    c.push_back(CreateLexer_Nifty());
    c.push_back(CreateParser_Nifty());
    c.push_back(CreateCodegen_Nifty());
    return c;
  }
  throw statik::SError("Unknown compiler " + name);
}
