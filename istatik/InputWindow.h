// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _InputWindow_h_
#define _InputWindow_h_

#include "ISError.h"
#include "WindowResponse.h"

#include <vector>

namespace istatik {

class InputWindow {
public:
  WindowResponse Input(int y, int x, int ch);
private:
};

}

#endif // _InputWindow_h_
