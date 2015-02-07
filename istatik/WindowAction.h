// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _WindowAction_h_
#define _WindowAction_h_

namespace istatik {

struct WindowAction {
  virtual ~WindowAction() {}
  int q;
};

struct MoveAction : public WindowAction {
  MoveAction(int y, int x)
    : y(y), x(x) {}
  int y;
  int x;
};

struct DeleteAction : public WindowAction {
};

struct InsertAction : public WindowAction {
  InsertAction(int ch)
    : ch(ch) {}
  int ch;
};

}

#endif // _WindowAction_h_
