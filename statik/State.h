// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_State_h_
#define _statik_State_h_

#include "SError.h"

#include <boost/lexical_cast.hpp>

#include <ostream>
#include <string>

namespace statik {

class State {
public:
  // Ordering: worst to best
  enum Station {
    ST_PENDING,
    ST_BAD,
    ST_OK,
    ST_DONE,
    ST_COMPLETE
  };
  static std::string UnMapStation(Station st);

  State();
  State(Station station);

  void Clear();

  Station GetStation() const { return m_station; }
  void SetStation(Station station) { m_station = station; }

  bool IsPending() const { return ST_PENDING == m_station; }
  bool IsOK() const { return ST_OK == m_station; }
  bool IsBad() const { return ST_BAD == m_station; }
  bool IsDone() const { return ST_DONE == m_station; }
  bool IsComplete() const { return ST_COMPLETE == m_station; }
  bool IsAccepting() const { return ST_OK == m_station || ST_DONE == m_station; }
  bool IsEmitting() const { return ST_DONE == m_station || ST_COMPLETE == m_station; }
  bool IsLocked() const { return m_isLocked; }

  void GoPending() { m_station = ST_PENDING; }
  void GoOK() { m_station = ST_OK; }
  void GoBad() { m_station = ST_BAD; }
  void GoDone() { m_station = ST_DONE; }
  void GoComplete() { m_station = ST_COMPLETE; }
  void Lock() { m_isLocked = true; }
  void Unlock() { m_isLocked = false; }

  bool operator==(const State& rhs) const;
  bool operator!=(const State& rhs) const;
  std::string Print() const { return UnMapStation(m_station); }

private:
  bool m_isLocked;
  Station m_station;
};

std::ostream& operator<< (std::ostream& out, const State& state);

}

#endif // _statik_State_h_
