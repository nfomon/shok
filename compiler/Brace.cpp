// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Brace.h"

#include "CompileError.h"

#include <string>
using std::string;

using namespace compiler;

bool Brace::isOpen() const {
  return m_isOpen;
}

bool Brace::isIrrelevant() const {
  return "(" == name || ")" == name;
}

bool Brace::matchesCloseBrace(Brace* closeBrace) const {
  if (!m_isOpen) {
    throw CompileError("A closing brace will never match with another closing brace... :P");
  }
  if (("[" == name && "]" == closeBrace->name) ||
      ("(" == name && ")" == closeBrace->name) ||
      ("{" == name && "}" == closeBrace->name)) {
    return true;
  }
  return false;
}

void Brace::setup() {
  if (("(" == name || ")" == name) && children.size() < 1) {
    throw CompileError("Empty parens in the AST are not allowed");
  }
}

void Brace::compile() {
  if (0 == children.size()) {
    return;
  }
  throw CompileError("Brace::compile is not setup");
}
