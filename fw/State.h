// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _State_h_
#define _State_h_

#include "Rule.h"

#include <boost/lexical_cast.hpp>

#include <string>

namespace fw {

struct DS;

class State {
public:
  const Rule& rule;

  State(const Rule& rule,
        bool startDone = false)
    : rule(rule),
      m_startDone(startDone) {
    Clear();
  }
  virtual ~State() {}

  void Clear() {
    if (m_startDone) {
      m_station = ST_DONE;
    } else {
      m_station = ST_OK;
    }
  }

  bool IsOK() const { return ST_OK == m_station; }
  bool IsBad() const { return ST_BAD == m_station; }
  bool IsDone() const { return ST_DONE == m_station; }
  bool IsComplete() const { return ST_COMPLETE == m_station; }
  bool IsAccepting() const { return ST_OK == m_station || ST_DONE == m_station; }
  bool IsEmitting() const { return ST_DONE == m_station || ST_COMPLETE == m_station; }

  void GoOK() { m_station = ST_OK; }
  void GoBad() { m_station = ST_BAD; }
  void GoDone() { m_station = ST_DONE; }
  void GoComplete() { m_station = ST_COMPLETE; }

  virtual operator std::string() const { return Print(); }
  std::string Print() const { return UnMapStation(m_station); }

private:
  enum Station {
    ST_OK,
    ST_BAD,
    ST_DONE,
    ST_COMPLETE
  };

  static std::string UnMapStation(Station st) {
    switch (st) {
    case ST_OK:       return "ok";
    case ST_BAD:      return "bad";
    case ST_DONE:     return "done";
    case ST_COMPLETE: return "complete";
    default: throw FWError("Failed to unmap Station " + boost::lexical_cast<std::string>(st));
    }
  }

  bool m_startDone;
  Station m_station;
};

}

#endif // _State_h_
