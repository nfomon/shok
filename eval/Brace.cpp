// Copyright (C) 2013 Michael Biggs.  See the COPYRIGHT file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "Brace.h"

#include "EvalError.h"

#include <string>
using std::string;

using namespace eval;

bool Brace::isOpen() const {
  return m_isOpen;
}

bool Brace::isIrrelevant() const {
  return "(" == name || ")" == name;
}

bool Brace::matchesCloseBrace(Brace* closeBrace) const {
  if (!m_isOpen) {
    throw EvalError("A closing brace will never match with another closing brace... :P");
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
    throw EvalError("Empty parens in the AST are not allowed");
  }
}

void Brace::evaluate() {
  if (0 == children.size()) {
    return;
  }
  throw EvalError("Brace::evaluate is not setup");
}
