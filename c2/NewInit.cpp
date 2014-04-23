// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "NewInit.h"

#include "util/Util.h"

#include <string>
#include <utility>
using std::string;

using namespace compiler;

NewInit::NewInit(Log& log)
  : m_log(log),
    m_hasType(false),
    m_hasExp(false) {
}

void NewInit::attach_name(const std::string& name) {
  m_name = name;
  // Ensure this name does not collide in the local symbol table
  // TODO
}

void NewInit::attach_type(const std::string& type) {
  // TODO
  m_hasType = true;
}

void NewInit::attach_exp(const std::string& exp) {
  // TODO
  m_hasExp = true;
}

std::string NewInit::bytecode() const {
  // if !m_hasExp, then the initial value comes from the type
  return "<newinit>";
}
