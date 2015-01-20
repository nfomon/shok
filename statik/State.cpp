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

State::State(bool startDone)
  : m_startDone(startDone),
    m_isLocked(false),
    m_station(ST_BAD) {
  Clear();
}

void State::Clear() {
  if (m_startDone) {
    m_station = ST_DONE;
  } else {
    m_station = ST_OK;
  }
  Unlock();
}

/* private */

string State::UnMapStation(Station st) {
  switch (st) {
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
