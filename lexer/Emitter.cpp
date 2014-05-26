// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Emitter.h"

#include "LexError.h"

#include <ostream>
#include <string>
#include <utility>
using std::endl;
using std::ostream;
using std::string;

using namespace lexer;

Emitter::Emitter(ostream& out, linenum_t& linenum, charnum_t& charnum)
  : m_out(out),
    m_linenum(linenum),
    m_charnum(charnum) {
}

void Emitter::p(const string& s) {
  if (1 == m_charnum) {
    m_out << m_linenum << " ";
  } else {
    m_out << " ";
  }
  m_out << m_charnum << ":" << s;
}
