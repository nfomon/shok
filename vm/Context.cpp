// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Context.h"

#include "VMError.h"

#include "util/Util.h"

using namespace vm;

symbol_map& Context::globals() {
  return m_globals;
}

symbol_map& Context::locals() {
  if (m_callstack.empty()) {
    return m_globals;
  }
  return m_callstack.back();
}

symbol_mod_iter Context::find(const std::string& name) {
  if (!m_callstack.empty()) {
    symbol_mod_iter local = m_callstack.back().find(name);
    if (local != m_callstack.back().end()) {
      return local;
    }
  }
  return m_globals.find(name);
}

symbol_iter Context::find(const std::string& name) const {
  if (!m_callstack.empty()) {
    symbol_iter local = m_callstack.back().find(name);
    if (local != m_callstack.back().end()) {
      return local;
    }
  }
  return m_globals.find(name);
}

symbol_iter Context::end() const {
  return m_globals.end();
}

void Context::addFrame() {
  m_callstack.push_back(stackframe_map());
}

void Context::removeFrame() {
  if (m_callstack.empty()) {
    throw VMError("Cannot return from the top of the call stack!");
  }
  m_callstack.pop_back();
}
