// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "State.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <ostream>
#include <string>
using std::ostream;
using std::string;

using namespace statik;

/* public */

State::State()
  : m_isLocked(false),
    m_station(ST_INIT) {
}

void State::Clear() {
  m_station = ST_INIT;
  Unlock();
}

bool State::operator==(const State& rhs) const {
  return m_station == rhs.m_station && m_isLocked == rhs.m_isLocked;
}

bool State::operator!=(const State& rhs) const {
  return !(*this == rhs);
}

/* private */

string State::UnMapStation(Station st) {
  switch (st) {
  case ST_INIT:     return "init";
  case ST_OK:       return "ok";
  case ST_BAD:      return "bad";
  case ST_DONE:     return "done";
  case ST_COMPLETE: return "complete";
  default: throw SError("Failed to unmap Station " + lexical_cast<string>(st));
  }
}

/* non-member */

ostream& statik::operator<< (ostream& out, const State& state) {
  out << string(state);
  return out;
}
