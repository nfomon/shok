// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _State_h_
#define _State_h_

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
};

}

#endif // _State_h_
