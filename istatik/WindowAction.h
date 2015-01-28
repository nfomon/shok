// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _WindowAction_h_
#define _WindowAction_h_

namespace istatik {

enum ACTION {
  GOTO,
  INSERT,
  DELETE
};

struct WindowAction {
  WindowAction(int y, int x, ACTION action)
    : y(y), x(x), action(action) {}
  int y;
  int x;
  ACTION action;
};

}

#endif // _WindowAction_h_
