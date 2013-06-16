#include "Brace.h"

#include "EvalError.h"

#include <string>
using std::string;

using namespace eval;

Brace::Brace(Log& log, const Token& token, bool isOpen)
  : Node(log, token),
    m_isOpen(isOpen) {
}

Brace::~Brace() {
}

bool Brace::isOpen() const {
  return m_isOpen;
}

bool Brace::isIrrelevant() const {
  return "(" == name || ")" == name;
}

bool Brace::matchesCloseBrace(Brace* b) const {
  if (!m_isOpen) {
    throw EvalError("A closing brace will never match with another closing brace... :P");
  }
  if (("(" == name && ")" == b->name) ||
      ("{" == name && "}" == b->name)) {
    return true;
  }
  return false;
}

void Brace::complete() {
  if (m_isComplete) return;
  if (("(" == name || ")" == name) && children.size() < 1) {
    throw EvalError("Empty parens in the AST are not allowed");
  }
  for (child_iter i = children.begin(); i != children.end(); ++i) { 
    (*i)->complete();
  } 
  m_isComplete = true;
}

void Brace::evaluate() {
  if (0 == children.size()) {
    return;
  }
  throw EvalError("Brace::evaluate is incomplete");
}

string Brace::cmdText() const {
  throw EvalError("Cannot get cmdText of Brace");
}
