// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Scope.h"

#include "EvalError.h"
#include "Function.h"

#include <boost/lexical_cast.hpp>

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace eval;

Scope::~Scope() {
  m_log.info("Destroying " + string(m_function ? "function " : "") +
             "scope at depth " + boost::lexical_cast<string>(m_depth));
}

// Remains the root scope if this is never called
void Scope::init(Scope* parentScope) {
  if (!parentScope) {
    throw EvalError("Cannot init the root scope");
  } else if (m_isInit) {
    throw EvalError("Cannot init already-initialized scope");
  }
  m_parentScope = parentScope;
  m_depth = parentScope->m_depth + 1;
  m_log.debug("Init scope at depth " + boost::lexical_cast<string>(m_depth));
  m_isInit = true;
}

// Remains the root scope if this is never called
void Scope::init(Scope* parentScope, Function* parentFunction) {
  if (!parentScope) {
    throw EvalError("Cannot init the root scope");
  } else if (m_isInit) {
    throw EvalError("Cannot init already-initialized scope");
  }
  m_parentScope = parentScope;
  m_function = parentFunction;
  m_depth = 0;
  m_log.debug("Init function-scope at depth " + boost::lexical_cast<string>(m_depth));
  m_isInit = true;
}

// Clears all objects from the scope
void Scope::reset() {
  m_log.debug("Resetting scope at depth " +
              boost::lexical_cast<string>(m_depth));
  m_symbolTable.reset();
}

// Commit (confirm) a pending object into the scope
void Scope::commitFirst() {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->commitFirst();
  }
  m_symbolTable.commitFirst();
}

// Commit all pending-commit objects
void Scope::commitAll() {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->commitAll();
  }
  m_symbolTable.commitAll();
}

// Revert a pending-commit object
void Scope::revertLast() {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->revertLast();
  }
  m_symbolTable.revertLast();
}

// Revert all pending-commit objects
void Scope::revertAll() {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->revertAll();
  }
  m_symbolTable.revertAll();
}

Symbol* Scope::getSymbol(const string& varname) const {
  m_log.debug(string(m_function ? "Function " : "") + "Scope at depth " + boost::lexical_cast<string>(m_depth) + " retrieving symbol " + varname);
  Symbol* s = m_symbolTable.getSymbol(varname);
  if (s) return s;
  // TODO how to getSymbol from a function-scope?  Need to look up the function
  // type "symbols" here...??
  /*
  if (m_function) {
    s = m_function->type().getMember(varname);
    if (s) return s;
  }
  */
  // TODO: object-literal lookup
  if (!m_parentScope) return NULL;
  return m_parentScope->getSymbol(varname);
}

Symbol& Scope::newSymbol(const string& varname, auto_ptr<Type> type) {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->newSymbol(varname, type);
  }
  return m_symbolTable.newSymbol(varname, type);
}

void Scope::delSymbol(const string& varname) {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->delSymbol(varname);
  }
  return m_symbolTable.delSymbol(varname);
}

void Scope::initSymbol(const string& varname, auto_ptr<Object> newObject) {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->initSymbol(varname, newObject);
  }
  return m_symbolTable.initSymbol(varname, newObject);
}

void Scope::replaceObject(const string& varname, auto_ptr<Object> newObject) {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->replaceObject(varname, newObject);
  }
  return m_symbolTable.replaceObject(varname, newObject);
}
