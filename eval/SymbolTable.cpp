// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "SymbolTable.h"

#include "EvalError.h"
#include "Object.h"
#include "Symbol.h"

#include <boost/lexical_cast.hpp>

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace eval;

SymbolTable::~SymbolTable() {
  reset();
}

// Clears all objects from the store
void SymbolTable::reset() {
  revertAll();
  for (ordering_rev_mod_iter i = m_ordering.rbegin();
       i != m_ordering.rend(); ++i) {
    symbol_mod_iter s = m_symbols.find(i->first);
    if (m_symbols.end() == s) {
      throw EvalError("Resetting SymbolTable failed to find " + i->first);
    }
    if (s->second) {
      if (s->second->object.get()) {
        s->second->object->destruct();
      }
      delete s->second;
    }
    m_symbols.erase(s);
  }
  m_ordering.clear();
  if (!m_symbols.empty()) {
    throw EvalError("Resetting SymbolTable failed to clear all objects");
  }
}

// Commit (confirm) the oldest pending change to the store
void SymbolTable::commitFirst() {
  if (m_changeset.empty()) {
    throw EvalError("Cannot commit first of changeset; no changes pending");
  }
  SymbolChange* sc = m_changeset.front();
  symbol_iter s = m_symbols.find(sc->varname);
  if (m_symbols.end() == s) {
    throw EvalError("Cannot commit " + sc->varname + "; symbol missing");
  }
  AddSymbol* add = dynamic_cast<AddSymbol*>(sc);
  DelSymbol* del = dynamic_cast<DelSymbol*>(sc);
  if (add) {
    m_log.debug("Commit: addition of " + add->varname);
    if (!s->second->object.get()) {
      throw EvalError("Cannot commit " + add->varname + "; object missing");
    }
    s->second->object->construct();
  } else if (del) {
    m_log.debug("Commit: deletion of " + del->varname);
    if (!del->oldObject.get()) {
      throw EvalError("Cannot commit deletion of " + del->varname + "; old object missing from changeset");
    }
    bool found = false;
    for (ordering_mod_iter i = m_ordering.begin();
         i != m_ordering.end(); ++i) {
      if ((del->varname == i->first) && i->second) {
        m_ordering.erase(i);
        found = true;
        break;
      }
    }
    if (!found) {
      throw EvalError("Cannot commit deletion of " + del->varname + "; object is missing from ordering");
    }
    del->oldObject->destruct();
  } else {
    throw EvalError("Unknown SymbolChange");
  }
  delete sc;
  m_changeset.pop_front();
}

// Commit all pending-commit objects
void SymbolTable::commitAll() {
  m_log.debug("Committing all " + boost::lexical_cast<string>(m_changeset.size()) + " changes");
  size_t n = m_changeset.size();
  for (size_t i=0; i < n; ++i) {
    commitFirst();
  }
}

// Revert the newest pending-commit change from the store
void SymbolTable::revertLast() {
  if (m_changeset.empty()) {
    throw EvalError(m_log, "Cannot revert last of changeset; no changes pending");
  }
  SymbolChange* sc = m_changeset.back();
  symbol_mod_iter s = m_symbols.find(sc->varname);
  if (m_symbols.end() == s) {
    throw EvalError("Cannot revert " + sc->varname + "; symbol missing");
  }
  AddSymbol* add = dynamic_cast<AddSymbol*>(sc);
  DelSymbol* del = dynamic_cast<DelSymbol*>(sc);
  if (add) {
    m_log.debug("Revert: addition of " + add->varname);
    // The add to revert should be the last element of m_ordering
    if (m_ordering.empty()) {
      throw EvalError("Cannot revert addition of " + add->varname + "; object is missing from ordering");
    }
    ordering_pair order = m_ordering.back();
    if (order.first != add->varname) {
      throw EvalError("Cannot revert addition of " + add->varname + "; object missing or misplaced in ordering");
    } else if (order.second) {
      throw EvalError("Cannot revert addition of " + add->varname + "; ordering suggests object is pending deletion");
    }
    m_ordering.pop_back();
    m_symbols.erase(s);
  } else if (del) {
    m_log.debug("Revert: deletion of " + del->varname);
    if (!del->oldObject.get()) {
      throw EvalError("Cannot revert deletion of " + del->varname + "; old object missing from changeset");
    }
    bool found = false;
    for (ordering_rev_mod_iter i = m_ordering.rbegin();
         i != m_ordering.rend(); ++i) {
      if (del->varname == i->first) {
        if (!i->second) {
          throw EvalError("Cannot revert deletion of " + del->varname + "; ordering suggests object is not pending deletion");
        }
        i->second = false;
        found = true;
        break;
      }
    }
    if (!found) {
      throw EvalError("Cannot revert deletion of " + del->varname + "; object is missing from ordering");
    }
    auto_ptr<Symbol> symbol(new Symbol(m_log, del->oldType));
    symbol->object = del->oldObject;
    symbol_pair sp(del->varname, symbol.release());
    m_symbols.insert(make_pair(del->varname, symbol.release()));
  } else {
    throw EvalError("Unknown SymbolChange");
  }
  delete sc;
  m_changeset.pop_back();
}

// Revert all pending-commit objects
void SymbolTable::revertAll() {
  m_log.debug("Reverting all " + boost::lexical_cast<string>(m_changeset.size()) + " changes");
  size_t n = m_changeset.size();
  for (size_t i=0; i < n; ++i) {
    revertLast();
  }
}

Symbol* SymbolTable::getSymbol(const string& varname) const {
  symbol_iter s = m_symbols.find(varname);
  if (s != m_symbols.end()) return s->second;
  return NULL;
}

Symbol& SymbolTable::newSymbol(const string& varname, auto_ptr<Type> type) {
  // An object name collision should have already been detected, but repeat
  // this now until we're confident about that
  if (getSymbol(varname)) {
    throw EvalError("Cannot create variable " + varname + "; already exists, and should never have been created");
  }
  m_log.info("Pending addition of object " + varname + " to an object store");
  symbol_pair sp(varname, new Symbol(m_log, type));
  m_symbols.insert(sp);
  // slow paranoid safety check
  for (ordering_iter i = m_ordering.begin(); i != m_ordering.end(); ++i) {
    if (varname == i->first && !i->second) {
      throw EvalError("Cannot create variable " + varname + "; somehow already exists in ordering");
    }
  }
  ordering_pair orp(varname, false);
  m_ordering.push_back(orp);
  auto_ptr<AddSymbol> as(new AddSymbol(varname));
  m_changeset.push_back(as.release());
  return *sp.second;
}

void SymbolTable::delSymbol(const string& varname) {
  m_log.info("Pending deletion of object " + varname + " to an object store");
  symbol_mod_iter s = m_symbols.find(varname);
  if (m_symbols.end() == s) {
    throw EvalError("Cannot delete variable " + varname + "; does not exist in this store");
  }
  // this steals the type and object from s (of m_symbols)
  auto_ptr<DelSymbol> ds(new DelSymbol(varname, s->second->type, s->second->object));
  bool found = false;
  for (ordering_mod_iter i = m_ordering.begin(); i != m_ordering.end(); ++i) {
    if ((varname == i->first) && !i->second) {
      i->second = true;
      found = true;
      break;
    }
  }
  if (!found) {
    throw EvalError("Cannot delete variable " + varname + "; somehow does not exist in ordering");
  }
  delete s->second;
  m_symbols.erase(s);
  m_changeset.push_back(ds.release());
}

void SymbolTable::initSymbol(const string& varname,
                             auto_ptr<Object> newObject) {
  m_log.info("Initializing object " + varname);
  symbol_mod_iter s = m_symbols.find(varname);
  if (m_symbols.end() == s) {
    throw EvalError("Cannot replace variable " + varname + "; does not exist in this store");
  } else if (!s->second) {
    throw EvalError("Cannot initialize variable " + varname + " that exists but has no symbol");
  } else if (s->second->object.get()) {
    throw EvalError("Cannot initialize variable " + varname + " that already has an object");
  } else if (!newObject.get()) {
    throw EvalError("Cannot initialize variable " + varname + " with no newObject");
  }
  if (varname != newObject->getName()) {
    m_log.warning("Replacing SymbolTable name " + varname + " with object that thinks its name is " + newObject->getName());
  }
  s->second->object = newObject;
  // Construction is left to the upcoming commit
}

void SymbolTable::replaceObject(const string& varname,
                                auto_ptr<Object> newObject) {
  m_log.info("Replacing object " + varname);
  symbol_mod_iter s = m_symbols.find(varname);
  if (m_symbols.end() == s) {
    throw EvalError("Cannot replace variable " + varname + "; does not exist in this store");
  } else if (!s->second || !s->second->object.get()) {
    throw EvalError("Cannot replace variable " + varname + "; found deficient symbol in the store");
  } else if (!newObject.get()) {
    throw EvalError("Cannot replace variable " + varname + " with no newObject");
  }
  if (varname != newObject->getName()) {
    m_log.warning("Replacing SymbolTable name " + varname + " with object that thinks its name is " + newObject->getName());
  }
  s->second->object->destruct();
  s->second->object = newObject;
  s->second->object->construct();
}

auto_ptr<SymbolTable> SymbolTable::duplicate() const {
  m_log.debug("Duplicating object store");
  if (!m_changeset.empty() || !m_ordering.empty()) {
    throw EvalError("Cannot duplicate SymbolTable that has pending changes");
  }
  auto_ptr<SymbolTable> os(new SymbolTable(m_log));
  for (symbol_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {
    if (!i->second || !i->second->type.get() || !i->second->object.get()) {
      throw EvalError("Cannot duplicate SymbolTable with deficient symbol");
    }
    os->newSymbol(i->first, i->second->type->duplicate());
    os->initSymbol(i->first, i->second->object->clone(i->second->object->getName()));
  }
  os->commitAll();
  for (symbol_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {
    os->replaceObject(i->first, i->second->object->clone(i->second->object->getName()));
  }
  return os;
}
