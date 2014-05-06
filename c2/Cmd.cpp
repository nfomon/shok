// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Cmd.h"

#include "CompileError.h"

#include "util/Util.h"

#include <string>
#include <utility>
#include <vector>
using std::pair;
using std::string;
using std::vector;

using namespace compiler;

void Cmd::attach_text(const std::string& text) {
  m_cmdtext += text;
}

void Cmd::attach_exp(const Expression& exp) {
  m_expcode += exp.bytecode() + "\n";
  m_cmdtext += "{}";
}

std::string Cmd::bytecode() const {
  return m_expcode + m_cmdtext;
}
