// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "New.h"

#include "VMError.h"

#include "util/Util.h"

#include <string>
#include <utility>
#include <vector>
using std::pair;
using std::string;
using std::vector;

using namespace vm;

// new <name> <source> <# additional members>
// <members...>
void New::insert(const string& line) {
  if (m_membertime) {
    // I would really like to use a "member parser" here...
    --m_membersLeft;
    if (0 == m_membersLeft) {
      m_done = true;
    }
  } else {
    vector<string> words = Util::split(line);
    if (words.size() != 3) {
      throw VMError("Incorrect # args to (new): " + line);
    }
    m_name = words.at(0);
    m_source = words.at(1);
    m_membersLeft = boost::lexical_cast<int>(words.at(2));
    if (0 == m_membersLeft) {
      m_done = true;
    } else {
      m_membertime = true;
    }
  }
}

void New::execute(SymbolTable& symbolTable) {
}
