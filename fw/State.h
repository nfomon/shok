// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _State_h_
#define _State_h_

#include <boost/lexical_cast.hpp>

#include <string>

namespace fw {

struct State {
  bool ok;
  bool bad;
  bool done;
  bool locked;

  State()
    : ok(false),
      bad(false),
      done(false),
      locked(false) {}
  virtual ~State() {}

  virtual void Clear() {
    ok = false;
    bad = false;
    done = false;
    locked = false;
  }

  virtual operator std::string() const { return "[State:" + StateFlags() + "]"; }

  std::string StateFlags() const { return boost::lexical_cast<std::string>(ok) + "/" + boost::lexical_cast<std::string>(bad) + "/" + boost::lexical_cast<std::string>(done) + "/" + boost::lexical_cast<std::string>(locked) + "]"; }
};

}

#endif // _State_h_
